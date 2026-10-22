#!/usr/bin/env python3
import argparse
import re
import subprocess
import sys
from pathlib import Path

from common import CONFIG as CFG, OBJDUMP, READELF, ROOT, game_sources, objdiff_target
from common import read_symbols as read_symbols_file

# mwcc marks COMDAT symbols with ELF binding 13 (STB_LOPROC).
LINKONCE_RE = re.compile(r"<processor specific>:\s*13\b.*?\s(\S+)$")

GAP_RE = re.compile(r"_dsd_gap@(\w+?)_\d+\.o")
SECTION_LINE = re.compile(r"^\s+\S+\s+start:0x")
SECTION_RE = re.compile(
    r"^\s+(\S+)\s+start:0x([0-9a-fA-F]+)\s+end:0x([0-9a-fA-F]+)"
    r"\s+kind:(\w+)\s+align:(\d+)")
FILE_ENTRY = re.compile(r"^(\S+\.o):$")

# symbols.txt kinds, not delinks.txt kinds: .rodata objects are `kind:data`.
KINDS = {".text": "function", ".rodata": "data", ".data": "data", ".bss": "bss"}


def module_dir(module):
    if module == "main":
        return CFG
    if module in ("itcm", "dtcm"):
        return CFG / module
    return CFG / "overlays" / module


def read_symbols(module):
    out, index = {}, []
    path = module_dir(module) / "symbols.txt"
    if not path.exists():
        return out, index
    for s in read_symbols_file(path):
        out[s.name] = (s.addr, s.size, s.kind)
        index.append((s.addr, s.kind))
    index.sort()
    return out, index


def next_symbol(index, addr, kind):
    return next((a for a, k in index if a > addr and k == kind), None)


def section_table(module):
    out = []
    path = module_dir(module) / "delinks.txt"
    if not path.exists():
        return out
    for line in path.read_text(encoding="utf-8").splitlines():
        if FILE_ENTRY.match(line):
            break
        m = SECTION_RE.match(line)
        if m:
            out.append({"name": m.group(1), "start": int(m.group(2), 16),
                        "end": int(m.group(3), 16), "kind": m.group(4),
                        "align": int(m.group(5))})
    return out


def read_sections(module):
    return {s["name"]: (s["start"], s["end"]) for s in section_table(module)}


def scan_units():
    units = []
    for path in game_sources():
        target = objdiff_target(path)
        if not target:
            continue
        g = GAP_RE.search(target)
        if not g:
            print("gen_delinks: %s: unrecognised target %r" %
                  (path.name, target), file=sys.stderr)
            continue
        units.append((path.relative_to(ROOT).as_posix(), g.group(1)))
    return units


def tool_output(what, tool, flag, obj):
    try:
        return subprocess.run([tool, flag, str(obj)], capture_output=True,
                              text=True, check=True).stdout
    except (OSError, subprocess.CalledProcessError) as exc:
        sys.exit("error: %s failed on %s: %s" % (what, obj, exc))


def linkonce(obj):
    # COMDAT copies (RTTI, inline members) are defined in every TU that uses them: not ownership.
    out = tool_output("readelf", READELF, "-sW", obj)
    return {m.group(1) for m in (LINKONCE_RE.search(l) for l in out.splitlines()) if m}


def obj_symbols(obj):
    syms = []
    for line in tool_output("objdump", OBJDUMP, "-t", obj).splitlines():
        if "*UND*" in line or "*ABS*" in line or " d " in line or " df " in line:
            continue
        parts = line.split()
        if len(parts) < 5:
            continue
        sec = next((p for p in parts if p.startswith(".")), None)
        name = parts[-1]
        try:
            size = int(parts[-2], 16)
        except ValueError:
            size = 0
        if sec and name and not name.startswith("$"):
            syms.append((name, "." + sec.split(".")[1], size))
    return syms


def unit_ranges(rel_source, symbols, index, sections, wanted):
    obj = ROOT / "build" / Path(rel_source).with_suffix(".o")
    if not obj.exists():
        return None, ["object not built: %s" % obj.relative_to(ROOT)]
    who = Path(rel_source).name
    ranges, unplaced, placed = {}, [], {}
    shared = linkonce(obj)
    for name, sec, objsize in obj_symbols(obj):
        if sec not in KINDS or name in shared:
            continue
        if name not in symbols:
            unplaced.append(name)
            continue
        addr, size, _ = symbols[name]
        if size is None:
            size = objsize
        elif objsize and objsize != size:
            print("gen_delinks: %s: %s size %#x (compiled) vs %#x (symbols.txt)"
                  % (who, name, objsize, size), file=sys.stderr)
        lo, hi = ranges.get(sec, (addr, addr))
        ranges[sec] = (min(lo, addr), max(hi, addr + size))
        placed.setdefault(sec, []).append((addr, size, name))

    for sec, (lo, hi) in list(ranges.items()):
        if sec == ".text" or sec not in wanted:
            continue
        home = next((n for n, (a, b) in sections.items() if a <= lo < b), None)
        if home is not None and home != sec:
            # The ROM is ground truth for const-ness: a `const` table placed in the module's
            # .data was writable in the original. Fix the source; a carve cannot interleave.
            print("gen_delinks: %s: %s %#x-%#x lies in the module's %s -- object kind "
                  "differs from the ROM (check `const`)" % (who, sec, lo, hi, home),
                  file=sys.stderr)
        kind = KINDS[sec]
        items = sorted(placed[sec])
        for addr, size, name in items:
            nxt = next_symbol(index, addr, kind)
            if nxt is not None and nxt < addr + size:
                print("gen_delinks: %s: %s is %#x bytes, %d too many -- ends %#x, "
                      "next symbol %#x" % (who, name, size, addr + size - nxt,
                                           addr + size, nxt), file=sys.stderr)

        # The gap to the next symbol belongs to this file; more than alignment padding is a sizing error.
        nxt = next_symbol(index, items[-1][0], kind)
        if nxt is None:
            continue
        end = sections.get(sec, (0, 0))[1]
        if end:
            nxt = min(nxt, end)
        if nxt <= hi:
            continue
        pad = nxt - hi
        if pad >= min(nxt & -nxt, 32):
            print("gen_delinks: %s: %s ends %#x, next symbol %#x -- %d byte "
                  "shortfall not explained by alignment"
                  % (who, sec, hi, nxt, pad), file=sys.stderr)
        ranges[sec] = (lo, nxt)
    return (ranges or None), unplaced


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--sections", default=".text",
                    help="comma list to carve (default .text)")
    ap.add_argument("--with-data", action="store_true",
                    help="alias for --sections .text,.rodata,.data,.bss")
    ap.add_argument("--strict", action="store_true",
                    help="exit non-zero on UNPLACED symbols")
    ap.add_argument("-v", "--verbose", action="store_true")
    args = ap.parse_args()
    wanted = ((".text", ".rodata", ".data", ".bss") if args.with_data else
              tuple(s.strip() for s in args.sections.split(",") if s.strip()))
    bad = [s for s in wanted if s not in KINDS]
    if bad:
        ap.error("unknown section(s): %s" % ", ".join(bad))

    by_module = {}
    all_unplaced = []
    for rel, module in scan_units():
        symbols, index = read_symbols(module)
        ranges, unplaced = unit_ranges(rel, symbols, index, read_sections(module), wanted)
        if unplaced:
            all_unplaced.append((rel, unplaced))
        if ranges:
            obj_path = str(Path(rel).with_suffix(".o")).replace("\\", "/")
            by_module.setdefault(module, []).append((obj_path, ranges))

    for module, entries in sorted(by_module.items()):
        path = module_dir(module) / "delinks.txt"
        head, kept, cur = [], {}, None
        for ln in path.read_text(encoding="utf-8").splitlines():
            m = FILE_ENTRY.match(ln)
            if m:
                cur = m.group(1)
                kept[cur] = []
            elif SECTION_LINE.match(ln):
                (kept[cur] if cur else head).append(ln)
        # Keep entries whose object isn't built, so running before `make all` never drops carves.
        fresh = {obj for obj, _ in entries}
        entries += [(obj, None) for obj, secs in sorted(kept.items())
                    if obj not in fresh and secs]
        lines = list(head)
        for obj_path, ranges in sorted(entries, key=lambda e: e[0]):
            lines.append("%s:" % obj_path)
            if ranges is None:
                lines.extend(kept[obj_path])
                continue
            for sec in wanted:
                if sec in ranges:
                    lo, hi = ranges[sec]
                    lines.append("    %-11s start:0x%08x end:0x%08x" % (sec, lo, hi))
        text = "\n".join(lines) + "\n"
        print("%s: %d sections + %d file entries" %
              (path.relative_to(ROOT), len(head), len(entries)))
        if args.verbose or args.dry_run:
            for ln in lines[len(head):]:
                print("  " + ln)
        # Write only on change: delinks.txt mtime drives the delink stamp.
        if not args.dry_run:
            if path.read_text(encoding="utf-8") == text:
                print("  unchanged")
            else:
                path.write_text(text, encoding="utf-8", newline="\n")
                print("  written")

    if all_unplaced:
        print()
        for rel, names in all_unplaced:
            print("UNPLACED in %s: %s" % (rel, ", ".join(sorted(set(names))[:6])))
        print("  (ctors/static data never sync -- see map-cpp-tu; non-fatal)")
        return 1 if args.strict else 0
    return 0


if __name__ == "__main__":
    sys.exit(main())
