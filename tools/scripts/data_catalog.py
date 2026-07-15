#!/usr/bin/env python3
"""Catalog every named global data symbol in the delinked gap.

Parses the delink asm (section membership) + symbols.txt (addresses) and counts
references across ALL relocs (main + overlays + itcm/dtcm). Emits a markdown table
per section, sorted by address, with size and reference count.

Purpose: a one-stop view of every data_* / vtable / RTTI symbol so fake delinker
aliases (a symbol that is really `container+offset`) stand out — they show as a
tiny gap after a real struct with a low reference count. Parallel to main_7_todo.md.

Usage: data_catalog.py [--gap asm/_dsd_gap@main_7.s] [-o main_7_data.md]
"""
import argparse
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
LABEL_RE = re.compile(r"^([A-Za-z_][A-Za-z0-9_]*):")
SECTION_RE = re.compile(r"^\s*\.section\s+(\.[A-Za-z0-9_]+)")
SPACE_RE = re.compile(r"\.space\s+(0x[0-9a-fA-F]+|\d+)")
ADDR_IN_NAME = re.compile(r"^data_0*([0-9a-fA-F]+)$")
# data-bearing sections we catalog (skip .text)
DATA_SECTIONS = {".rodata", ".data", ".bss", ".ctor", ".init", ".exception", ".exceptix"}


def parse_symbols():
    """name -> (addr:int, kind:str) from config/arm9/symbols.txt (main module)."""
    out = {}
    p = ROOT / "config/arm9/symbols.txt"
    for line in p.read_text().splitlines():
        m = re.match(r"^(\S+)\s+kind:(\S+)\s+addr:0x([0-9a-fA-F]+)", line)
        if m:
            out[m.group(1)] = (int(m.group(3), 16), m.group(2))
    return out


def parse_asm(gap_path, symbols):
    """Walk the delink asm; return list of dicts {name, section, addr, space}."""
    section = None
    entries = []
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
        # address: prefer symbols.txt, fall back to hex in the name
        addr = None
        if name in symbols:
            addr = symbols[name][0]
        else:
            am = ADDR_IN_NAME.match(name)
            if am:
                addr = int(am.group(1), 16)
        if addr is None:
            continue
        sp = SPACE_RE.search(line)
        entries.append({
            "name": name,
            "section": section,
            "addr": addr,
            "space": int(sp.group(1), 16) if sp else None,
        })
    return entries


def count_refs():
    """addr:int -> count of relocs whose base `to:` is that exact address (any add)."""
    counts = {}
    for rp in ROOT.glob("config/arm9/**/relocs.txt"):
        for line in rp.read_text().splitlines():
            m = re.search(r"\bto:0x([0-9a-fA-F]+)", line)
            if m:
                a = int(m.group(1), 16)
                counts[a] = counts.get(a, 0) + 1
    return counts


def size_bytes(gap_path, name):
    """Fallback: sum directive bytes from `name:` to the next label (last-in-section)."""
    total, active = 0, False
    for line in gap_path.read_text().splitlines():
        lm = LABEL_RE.match(line)
        if lm:
            if active:
                break
            if lm.group(1) == name:
                active = True
            continue
        if not active:
            continue
        s = line.strip()
        m = SPACE_RE.search(s)
        if m:
            total += int(m.group(1), 16)
        elif s.startswith(".word") or s.startswith(".long") or s.startswith(".4byte"):
            total += 4 * (s.count(",") + 1)
        elif s.startswith(".short") or s.startswith(".hword") or s.startswith(".2byte"):
            total += 2 * (s.count(",") + 1)
        elif s.startswith(".byte"):
            total += s.count(",") + 1
    return total or None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--gap", default="asm/_dsd_gap@main_7.s")
    ap.add_argument("-o", "--out", default="main_7_data.md")
    ap.add_argument("--query", help="print one symbol's row to stdout and exit")
    args = ap.parse_args()

    gap_path = ROOT / args.gap
    if not gap_path.is_file():
        raise SystemExit(f"gap asm not found: {gap_path} (run `make delinks`)")

    symbols = parse_symbols()
    entries = parse_asm(gap_path, symbols)
    refs = count_refs()

    # size = gap to next symbol in the same section; last uses byte-sum fallback
    by_section = {}
    for e in entries:
        by_section.setdefault(e["section"], []).append(e)
    for sec, lst in by_section.items():
        lst.sort(key=lambda e: e["addr"])
        for i, e in enumerate(lst):
            if i + 1 < len(lst):
                e["size"] = lst[i + 1]["addr"] - e["addr"]
            else:
                e["size"] = e["space"] or size_bytes(gap_path, e["name"])
            e["refs"] = refs.get(e["addr"], 0)

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

    if args.query:
        for sec, lst in by_section.items():
            for e in lst:
                if e["name"] == args.query or f"0x{e['addr']:08x}" == args.query.lower():
                    sz = f"0x{e['size']:x}" if e["size"] is not None else "?"
                    print(f"{e['name']}  addr=0x{e['addr']:08x}  size={sz}  "
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
    total = 0
    for sec in sorted(by_section):
        lst = by_section[sec]
        total += len(lst)
        lines += [f"## {sec} ({len(lst)} symbols)", "",
                  "| Name | Address | Size | Refs | Note |",
                  "|---|---|---|---|---|"]
        for e in lst:
            sz = f"0x{e['size']:x}" if e["size"] is not None else "?"
            lines.append(f"| `{e['name']}` | 0x{e['addr']:08x} | {sz} | "
                         f"{e['refs']} | {note(e)} |")
        lines.append("")
    lines.insert(4, f"**{total} symbols total.**\n")

    (ROOT / args.out).write_text("\n".join(lines))
    print(f"wrote {ROOT / args.out} ({total} symbols)")


if __name__ == "__main__":
    main()
