#!/usr/bin/env python3
"""Regenerate delinks.txt file entries from the `// objdiff-target:` scan.

For every decompiled TU, carve its address ranges out of the module gap so the
linker can consume our compiled .o instead of the delinked bytes. Ranges come
from the TU's compiled .o symbols resolved through config/arm9/symbols.txt --
the same name->address path sync_renames.py uses.

  python tools/scripts/gen_delinks.py [--dry-run] [-v]

The module section table (the leading indented block) is preserved verbatim;
only file entries below it are rewritten. Exit 0 ok / 1 unplaced / 2 error.
"""
import argparse
import os
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CFG = ROOT / "config" / "arm9"
OBJDUMP = os.environ.get("OBJDUMP", "arm-none-eabi-objdump")
READELF = os.environ.get("READELF", "arm-none-eabi-readelf")
# mwcc marks COMDAT/linkonce symbols with ELF binding 13 (STB_LOPROC).
LINKONCE_RE = re.compile(r"<processor specific>:\s*13\b.*?\s(\S+)$")

TARGET_RE = re.compile(r"^\s*//\s*objdiff-target:\s*(.+?)\s*$")
GAP_RE = re.compile(r"_dsd_gap@(\w+?)_\d+\.o")
SYM_RE = re.compile(r"^(\S+)\s+kind:(\S+).*?addr:0x([0-9a-fA-F]+)")
SIZE_RE = re.compile(r"size=0x([0-9a-fA-F]+)")
SECTION_LINE = re.compile(r"^\s+\S+\s+start:0x")
SECTION_RE = re.compile(r"^\s+(\S+)\s+start:0x([0-9a-fA-F]+)\s+end:0x([0-9a-fA-F]+)")
FILE_ENTRY = re.compile(r"^(\S+\.o):$")

# .o section name -> the symbols.txt `kind:` it is named by. NOT the delinks.txt
# section kind: symbols.txt only ever says function/data/bss/label, so .rodata
# objects are `kind:data` too. Getting this wrong silently disables every
# next-symbol lookup for that section.
KINDS = {".text": "function", ".rodata": "data", ".data": "data", ".bss": "bss"}


def module_dir(module):
    if module == "main":
        return CFG
    if module in ("itcm", "dtcm"):
        return CFG / module
    return CFG / "overlays" / module


def read_symbols(module):
    """(name -> (addr, size or None, kind), sorted [(addr, kind)])"""
    out, index = {}, []
    path = module_dir(module) / "symbols.txt"
    if not path.exists():
        return out, index
    for line in path.read_text(encoding="utf-8").splitlines():
        m = SYM_RE.match(line)
        if m:
            sz = SIZE_RE.search(m.group(2))   # size= lives inside kind:...(...)
            kind = m.group(2).split("(")[0]
            addr = int(m.group(3), 16)
            out[m.group(1)] = (addr, int(sz.group(1), 16) if sz else None, kind)
            index.append((addr, kind))
    index.sort()
    return out, index


def next_symbol(index, addr, kind):
    """Address of the next symbol of `kind` after `addr`, or None."""
    for a, k in index:
        if a > addr and k == kind:
            return a
    return None


def read_sections(module):
    """delinks section name -> (start, end) from the module's section table.

    Needed to clamp next-symbol lookups: symbols.txt does not distinguish .rodata
    from .data, so the symbol after the last .rodata object can be the first .data
    one, which would extend a carve straight across the section boundary.
    """
    out = {}
    path = module_dir(module) / "delinks.txt"
    if not path.exists():
        return out
    for line in path.read_text(encoding="utf-8").splitlines():
        if FILE_ENTRY.match(line):
            break                     # file entries start; the table is above them
        m = SECTION_RE.match(line)
        if m:
            out[m.group(1)] = (int(m.group(2), 16), int(m.group(3), 16))
    return out


def scan_units():
    """[(rel_source, module)] for every TU with an objdiff-target comment."""
    units = []
    for path in sorted(p for ext in ("*.c", "*.cpp")
                       for p in (ROOT / "arm9" / "src").rglob(ext)):
        target = None
        for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
            m = TARGET_RE.match(line)
            if m:
                target = m.group(1)
                break
        if not target:
            continue
        g = GAP_RE.search(target)
        if not g:
            print("gen_delinks: %s: unrecognised target %r" %
                  (path.name, target), file=sys.stderr)
            continue
        units.append((path.relative_to(ROOT).as_posix(), g.group(1)))
    return units


def linkonce(obj):
    """Names mwcc emits as COMDAT (C++ RTTI, inline members) -- one copy per TU
    that needs them, deduplicated at link. So defining one is NOT ownership:
    _ZTS6clTask is defined in both fs.o and task.o but exists once in ROM.
    Counting them as owned makes a TU's range span every object in between.
    """
    try:
        out = subprocess.run([READELF, "-sW", str(obj)], capture_output=True,
                             text=True, check=True).stdout
    except (OSError, subprocess.CalledProcessError) as exc:
        sys.exit("error: readelf failed on %s: %s" % (obj, exc))
    return {m.group(1) for m in (LINKONCE_RE.search(l) for l in out.splitlines())
            if m}


def obj_symbols(obj):
    """[(name, section, size)] defined (non-UND, non-local-section) symbols.

    size is the compiler's, and is the only size source for data: symbols.txt
    carries size= on functions only (see memory reference_delink_symbol_extent).
    """
    try:
        out = subprocess.run([OBJDUMP, "-t", str(obj)], capture_output=True,
                             text=True, check=True).stdout
    except (OSError, subprocess.CalledProcessError) as exc:
        sys.exit("error: objdump failed on %s: %s" % (obj, exc))
    syms = []
    for line in out.splitlines():
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
            syms.append((name, sec.split(".", 2)[:2] and "." + sec.split(".")[1],
                         size))
    return syms


def unit_ranges(rel_source, module, symbols, index, sections, wanted,
                verbose=False):
    """section -> (start, end), or None if nothing placeable."""
    obj = ROOT / "build" / Path(rel_source).with_suffix(".o")
    if not obj.exists():
        return None, ["object not built: %s" % obj.relative_to(ROOT)]
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
            size = objsize       # data: symbols.txt has no size=
        elif objsize and objsize != size:
            print("gen_delinks: %s: %s size %#x (compiled) vs %#x (symbols.txt)"
                  % (Path(rel_source).name, name, objsize, size), file=sys.stderr)
        lo, hi = ranges.get(sec, (addr, addr))
        ranges[sec] = (min(lo, addr), max(hi, addr + size))
        placed.setdefault(sec, []).append((addr, size, name))

    who = Path(rel_source).name
    for sec, (lo, hi) in list(ranges.items()):
        # Only audit what we are about to carve; obj_match owns the general data
        # audit, so warning here about sections this run ignores is just noise.
        if sec == ".text" or sec not in wanted:
            continue                # .text: real size=, and dsd validates those
        kind = KINDS[sec]
        items = sorted(placed[sec])

        # EVERY object must fit before the next symbol, not just the last one. An
        # interior overrun leaves `hi` untouched, so checking only the section end
        # missed it (subOamBuffer overran frameTimer by 0x1c undetected).
        for addr, size, name in items:
            nxt = next_symbol(index, addr, kind)
            if nxt is not None and nxt < addr + size:
                print("gen_delinks: %s: %s is %#x bytes, %d too many -- ends %#x, "
                      "next symbol %#x" % (who, name, size, addr + size - nxt,
                                           addr + size, nxt), file=sys.stderr)

        # dsd sizes the last symbol by distance to the next one, so a section must run
        # to that symbol -- any shortfall is inter-object alignment padding, which
        # belongs to the preceding file. A shortfall NOT excused by alignment is a real
        # sizing error (CLAUDE.md "Homing globals"), so report it.
        nxt = next_symbol(index, items[-1][0], kind)
        if nxt is None:
            continue
        end = sections.get(sec, (0, 0))[1]
        if end:
            nxt = min(nxt, end)      # never run a carve past its own section
        if nxt <= hi:
            continue
        pad = nxt - hi
        if pad >= min(nxt & -nxt, 32):    # padding must fit the next object's align
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
        ranges, unplaced = unit_ranges(rel, module, symbols, index,
                                       read_sections(module), wanted, args.verbose)
        if unplaced:
            all_unplaced.append((rel, unplaced))
        if not ranges:
            continue
        obj_path = str(Path(rel).with_suffix(".o")).replace("\\", "/")
        by_module.setdefault(module, []).append((obj_path, ranges))

    for module, entries in sorted(by_module.items()):
        path = module_dir(module) / "delinks.txt"
        old = path.read_text(encoding="utf-8").splitlines()
        # Head = the module section table only: everything before the first file entry.
        head = []
        for ln in old:
            if re.match(r"^(\S+\.o):$", ln):
                break
            if SECTION_LINE.match(ln):
                head.append(ln)
        # Preserve entries we could not regenerate (object not built) so that running
        # this before `make all` never silently discards carves.
        kept, cur = {}, None
        for ln in old:
            m = re.match(r"^(\S+\.o):$", ln)
            if m:
                cur = m.group(1)
                kept[cur] = []
            elif cur and SECTION_LINE.match(ln):
                kept[cur].append(ln)
        fresh = {obj for obj, _ in entries}
        for obj, secs in sorted(kept.items()):
            if obj not in fresh and secs:
                entries.append((obj, None))
                by_module[module] = entries
        lines = list(head)
        for obj_path, ranges in sorted(entries, key=lambda e: e[0]):
            lines.append("%s:" % obj_path)
            if ranges is None:          # preserved verbatim, object not built
                lines.extend(kept[obj_path])
                continue
            # Data carving is opt-in only because ANY carve collapses obj_match to 0/0
            # (targets move out of the gap) -- see notes/link.md. Not correctness.
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
        # Write only on change: delinks.txt is a prerequisite of the delink stamp, so
        # bumping its mtime every run would re-delink forever.
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
