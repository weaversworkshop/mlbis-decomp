#!/usr/bin/env python3
import argparse
import re
from collections import Counter
from pathlib import Path

from common import CONFIG, ROOT, read_symbols, resident_symbols

LABEL_RE = re.compile(r"^([A-Za-z_][A-Za-z0-9_$]*):")
SECTION_RE = re.compile(r"^\s*\.section\s+(\.[A-Za-z0-9_]+)")
SPACE_RE = re.compile(r"\.space\s+(0x[0-9a-fA-F]+|\d+)")
ADDR_IN_NAME = re.compile(r"^data_0*([0-9a-fA-F]+)$")
DATA_SECTIONS = {".rodata", ".data", ".bss", ".ctor", ".init", ".exception", ".exceptix"}
DIRECTIVE_SIZES = ((4, (".word", ".long", ".4byte")),
                   (2, (".short", ".hword", ".2byte")),
                   (1, (".byte",)))


def parse_symbols():
    """name -> ascending data addresses (same-named statics in different TUs repeat a name)."""
    out = {}
    for s in resident_symbols():
        if s.kind != "function":
            out.setdefault(s.name, []).append(s.addr)
    return {n: sorted(a) for n, a in out.items()}


def parse_asm(gap_path, symbols):
    """[{name, section, addr, space}] for every labelled data symbol."""
    section = None
    entries = []
    seen = Counter()
    for line in gap_path.read_text().splitlines():
        sm = SECTION_RE.match(line)
        if sm:
            section = sm.group(1)
            continue
        if section not in DATA_SECTIONS:
            continue
        lm = LABEL_RE.match(line)
        if not lm:
            continue
        name = lm.group(1)
        if name in symbols:
            addrs = symbols[name]
            addr = addrs[min(seen[name], len(addrs) - 1)]
            seen[name] += 1
        else:
            am = ADDR_IN_NAME.match(name)
            if not am:
                continue
            addr = int(am.group(1), 16)
        sp = SPACE_RE.search(line)
        entries.append({
            "name": name,
            "section": section,
            "addr": addr,
            "space": int(sp.group(1), 16) if sp else None,
        })
    return entries


def count_refs():
    """addr -> number of relocs (all modules) whose `to:` is exactly that address."""
    counts = {}
    for rp in CONFIG.glob("**/relocs.txt"):
        for line in rp.read_text().splitlines():
            m = re.search(r"\bto:0x([0-9a-fA-F]+)", line)
            if m:
                a = int(m.group(1), 16)
                counts[a] = counts.get(a, 0) + 1
    return counts


def size_bytes(gap_path, name):
    """Sum of directive bytes from `name:` to the next label."""
    total, active = 0, False
    for line in gap_path.read_text().splitlines():
        lm = LABEL_RE.match(line)
        if lm:
            if active:
                break
            active = lm.group(1) == name
            continue
        if not active:
            continue
        s = line.strip()
        m = SPACE_RE.search(s)
        if m:
            total += int(m.group(1), 16)
            continue
        for width, prefixes in DIRECTIVE_SIZES:
            if s.startswith(prefixes):
                total += width * (s.count(",") + 1)
                break
    return total or None


def fmt_size(e):
    return f"0x{e['size']:x}" if e["size"] is not None else "?"


def note(e):
    flags = []
    small = e["size"] is not None and e["size"] < 0x40
    if e["refs"] == 0:
        flags.append("(!) 0 refs")
    elif e["refs"] <= 2 and small:
        flags.append("alias? (small, few refs)")
    if e["space"] is not None and e["size"] is not None and e["space"] != e["size"]:
        flags.append(f"(!) .space 0x{e['space']:x} != gap 0x{e['size']:x}")
    return "; ".join(flags)


RELOC_RE = re.compile(r"from:0x([0-9a-fA-F]+)\b.*?\bto:0x([0-9a-fA-F]+)\b(?:\s+add:(-?0x[0-9a-fA-F]+))?"
                      r".*?\bmodule:(\S+)")


def reloc_sources():
    """[(src_module, from, to, add)] for every reloc into main."""
    out = []
    for rp in sorted(CONFIG.glob("**/relocs.txt")):
        mod = "main" if rp.parent == CONFIG else rp.parent.name
        for line in rp.read_text().splitlines():
            m = RELOC_RE.search(line)
            if m and m.group(4) == "main":
                out.append((mod, int(m.group(1), 16), int(m.group(2), 16),
                            int(m.group(3), 16) if m.group(3) else 0))
    return out


def owners(gap_path, entries, report_path):
    import bisect
    import json
    import subprocess
    import sys
    import build_todo

    if report_path:
        report = json.loads(Path(report_path).read_text(encoding="utf-8"))
    else:
        out = subprocess.run([sys.executable, str(Path(__file__).with_name("obj_match.py"))],
                             capture_output=True, text=True, check=True).stdout
        report = json.loads(out)
    status = build_todo.parse_data(report)
    asm = build_todo.parse_asm(gap_path)
    owned = build_todo.parse_report(report)
    base = build_todo.derive_base_offset(asm, owned, build_todo.report_sections(report))

    fns = []
    for rec in asm:
        if rec["address"]:
            unit, _ = build_todo.lookup_owned(rec, owned, base)
            fns.append((int(rec["address"], 16), unit or "?"))
    fns.sort()
    faddr = [a for a, _ in fns]
    emitters = Counter(f["name"] for u in report.get("units", []) for f in u.get("functions", []))
    span = {}
    for rec in asm:
        if not rec["address"] or rec["symbol"].startswith("__sinit_") or emitters[rec["symbol"]] > 1:
            continue
        a, u = int(rec["address"], 16), build_todo.lookup_owned(rec, owned, base)[0]
        if not u:
            continue
        lo, hi = span.get(u, (a, a))
        span[u] = (min(lo, a), max(hi, a))
    spans = sorted((lo, hi, u) for u, (lo, hi) in span.items() if u != "?")

    dsorted = sorted(entries, key=lambda e: e["addr"])
    daddr = [e["addr"] for e in dsorted]

    def data_at(a):
        i = bisect.bisect_right(daddr, a) - 1
        if i >= 0 and a < daddr[i] + (dsorted[i]["size"] or 0):
            return dsorted[i]
        return None

    def who(mod, a):
        if mod != "main":
            return mod
        d = data_at(a)
        if d:
            st = status.get((d["name"], d["addr"]))
            return f"`{d['name']}`" + (f"@{st['unit']}" if st and not st["comdat"] else "")
        i = bisect.bisect_right(faddr, a) - 1
        return fns[i][1] if i >= 0 else "?"

    exact = {e["addr"]: e for e in dsorted}
    refs = {}
    for mod, frm, to, add in reloc_sources():
        d = exact.get(to) or data_at(to)
        if not d:
            continue
        off = to + add - d["addr"]
        refs.setdefault(d["name"], []).append((who(mod, frm), off))

    def homed(e):
        st = status.get((e["name"], e["addr"]))
        return st and not st["comdat"] and st["unit"] in span

    by_sec = {}
    for e in entries:
        by_sec.setdefault(e["section"], []).append(e)
    lines = ["# main_7 data owners", "",
             "_Generated by `data_catalog.py --owners`._ Window = `.text` from the lower homed "
             "neighbour's unit start to the upper's unit end (link order mirrors `.text`). "
             "Refs: `unit xN` (code), `` `sym`@unit `` (pointer from data), `ovNNN`; `+0x..` = interior.",
             ""]
    for sec in (".rodata", ".data", ".bss"):
        lst = sorted(by_sec.get(sec, []), key=lambda e: e["addr"])
        rows = []
        for i, e in enumerate(lst):
            if status.get((e["name"], e["addr"])):
                continue
            lo = next((x for x in reversed(lst[:i]) if homed(x)), None)
            hi = next((x for x in lst[i + 1:] if homed(x)), None)
            lu = status[(lo["name"], lo["addr"])]["unit"] if lo else None
            hu = status[(hi["name"], hi["addr"])]["unit"] if hi else None
            wlo = span[lu][0] if lu else 0
            whi = span[hu][1] if hu else 1 << 32
            inverted = lu and hu and span[lu][0] > span[hu][0]
            if inverted:
                wlo, whi = span[hu][0], span[lu][1]
            cands = [u for s, h, u in spans if wlo <= s <= whi] if lu or hu else []
            rs = refs.get(e["name"], [])
            cnt = Counter(r for r, _ in rs)
            interior = sorted({o for _, o in rs if o})
            refstr = ", ".join(f"{r} x{n}" if n > 1 else r for r, n in cnt.most_common())
            if interior:
                refstr += " (+" + ",+".join(f"0x{o:x}" for o in interior) + ")"
            hits = [u for u in cands if u in cnt]
            if len(cands) > 6:
                cstr = f"{len(cands)} units: {cands[0]} … {cands[-1]}"
            else:
                cstr = ", ".join(cands)
            if inverted:
                cstr = "**INVERTED** " + cstr
            rows.append(f"| 0x{e['addr']:08x} | `{e['name']}` | {fmt_size(e)} | "
                        f"{lu or '-'} | {hu or '-'} | {cstr} | {', '.join(hits) or '-'} | {refstr or '0 refs'} |")
        if rows:
            lines += [f"## {sec} ({len(rows)} unhomed)", "",
                      "| Address | Symbol | Extent | Below | Above | Window | Window∩refs | Refs |",
                      "| --- | --- | ---: | --- | --- | --- | --- | --- |"] + rows + [""]
    return lines


def dump(e, width, per_line):
    if e["section"] == ".bss":
        return [f"// {e['name']}: .bss, 0x{e['size']:x} zero bytes"]
    rom = (ROOT / "extract/arm9/arm9.bin").read_bytes()
    off = e["addr"] - 0x02004000
    raw = rom[off:off + e["size"]]
    names = {}
    for s in read_symbols():
        names.setdefault(s.addr, s.name)
    ptrs = {}
    for mod, frm, to, add in reloc_sources():
        if mod == "main" and e["addr"] <= frm < e["addr"] + e["size"]:
            n = names.get(to, f"0x{to:08x}")
            ptrs[frm - e["addr"]] = f"&{n}" + (f" + 0x{add:x}" if add else "")
    vals, i = [], 0
    while i + width <= len(raw):
        if i in ptrs:
            vals.append(ptrs[i])
        else:
            v = int.from_bytes(raw[i:i + width], "little")
            vals.append(f"0x{v:0{width * 2}x}")
        i += width
    out = [f"// {e['name']} @ 0x{e['addr']:08x}, extent 0x{e['size']:x}"]
    if i < len(raw):
        out.append(f"// trailing {len(raw) - i} byte(s): " + " ".join(f"{b:02x}" for b in raw[i:]))
    out.append("{")
    for j in range(0, len(vals), per_line):
        out.append("    " + ", ".join(vals[j:j + per_line]) + ",")
    out.append("}")
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--gap", default="asm/_dsd_gap@main_7.s")
    ap.add_argument("-o", "--out", default="main_7_data.md")
    ap.add_argument("--query", help="print one symbol's row to stdout and exit")
    ap.add_argument("--owners", action="store_true", help="owner census of unhomed objects")
    ap.add_argument("--report", help="obj_match JSON (default: run obj_match)")
    ap.add_argument("--dump", metavar="NAME|0xADDR", help="print ROM contents as a C initializer")
    ap.add_argument("--width", type=int, default=4, choices=(1, 2, 4))
    ap.add_argument("--per-line", type=int, default=8)
    args = ap.parse_args()

    gap_path = ROOT / args.gap
    if not gap_path.is_file():
        raise SystemExit(f"gap asm not found: {gap_path} (run `make delinks`)")

    entries = parse_asm(gap_path, parse_symbols())
    refs = count_refs()

    by_section = {}
    for e in entries:
        by_section.setdefault(e["section"], []).append(e)
    for lst in by_section.values():
        lst.sort(key=lambda e: e["addr"])
        for e, nxt in zip(lst, lst[1:] + [None]):
            if nxt:
                e["size"] = nxt["addr"] - e["addr"]
            else:
                e["size"] = e["space"] or size_bytes(gap_path, e["name"])
            e["refs"] = refs.get(e["addr"], 0)

    if args.dump:
        q = args.dump.lower()
        hit = next((e for lst in by_section.values() for e in lst
                    if e["name"] == args.dump or f"0x{e['addr']:08x}" == q), None)
        if not hit:
            raise SystemExit(f"not found: {args.dump}")
        print("\n".join(dump(hit, args.width, args.per_line)))
        return

    if args.owners:
        out = ROOT / (args.out if args.out != "main_7_data.md" else "build/main_7_owners.md")
        out.write_text("\n".join(owners(gap_path, entries, args.report)) + "\n", encoding="utf-8")
        print(f"wrote {out}")
        return

    if args.query:
        for lst in by_section.values():
            for e in lst:
                if e["name"] == args.query or f"0x{e['addr']:08x}" == args.query.lower():
                    print(f"{e['name']}  addr=0x{e['addr']:08x}  size={fmt_size(e)}  "
                          f"section={e['section']}  refs={e['refs']}  {note(e) or '-'}")
                    return
        print(f"not found: {args.query}")
        return

    lines = ["# main_7 data catalog", "",
             "_Auto-generated by `tools/scripts/data_catalog.py` — do not hand-edit._",
             "Every named data symbol in the delinked gap. Size = gap to next symbol "
             "in section (exact). Refs = relocs targeting this base address across "
             "main + overlays + itcm/dtcm. Low ref count next to a real struct = "
             "likely fake delinker alias (fold via fix-fake-delink-symbols).", ""]
    for sec in sorted(by_section):
        lst = by_section[sec]
        lines += [f"## {sec} ({len(lst)} symbols)", "",
                  "| Name | Address | Size | Refs | Note |",
                  "|---|---|---|---|---|"]
        for e in lst:
            lines.append(f"| `{e['name']}` | 0x{e['addr']:08x} | {fmt_size(e)} | "
                         f"{e['refs']} | {note(e)} |")
        lines.append("")
    total = len(entries)
    lines.insert(4, f"**{total} symbols total.**\n")

    (ROOT / args.out).write_text("\n".join(lines))
    print(f"wrote {ROOT / args.out} ({total} symbols)")


if __name__ == "__main__":
    main()
