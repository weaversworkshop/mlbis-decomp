#!/usr/bin/env python3
"""Move a data symbol back to its object start -- CONFIG ONLY (symbols.txt addr +
relocs.txt re-anchor with a compensating `add:`).

dsd names the address a relocation POINTS AT. When every reference is interior --
a vtable is always referenced as `&vtable[2]`, past the offset-to-top and
type_info header words -- the symbol lands N bytes late, and our compiled
`sym+N` reloc no longer pairs with the target's `sym+0`.

Usage: rebase_data_sym.py NAME NEW_ADDR [--apply]. Dry-run by default.
"""
import argparse
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CONFIG = ROOT / "config" / "arm9"

SYM_RE = re.compile(r"^(?P<name>\S+)(?P<mid>\s+kind:\S+\s+addr:)0x(?P<addr>[0-9a-fA-F]+)"
                    r"(?P<tail>.*?)\s*$")


def find_symbol(name):
    for path in sorted(CONFIG.rglob("symbols.txt")):
        lines = path.read_text(encoding="utf-8").splitlines()
        for i, line in enumerate(lines):
            m = SYM_RE.match(line)
            if m and m.group("name") == name:
                return path, lines, i, int(m.group("addr"), 16), m
    sys.exit(f"error: symbol {name} not found in any symbols.txt")


def check_order(lines, idx, new_addr):
    """symbols.txt is address-sorted; refuse a move that would break it."""
    def addr_of(j):
        m = SYM_RE.match(lines[j]) if 0 <= j < len(lines) else None
        return int(m.group("addr"), 16) if m else None

    prev = next((a for j in range(idx - 1, -1, -1) if (a := addr_of(j)) is not None), None)
    nxt = next((a for j in range(idx + 1, len(lines)) if (a := addr_of(j)) is not None), None)
    if prev is not None and new_addr < prev:
        sys.exit(f"error: 0x{new_addr:08x} sorts before the previous symbol 0x{prev:08x}")
    if nxt is not None and new_addr > nxt:
        sys.exit(f"error: 0x{new_addr:08x} sorts after the next symbol 0x{nxt:08x}")


def rewrite_relocs(old, delta, apply_):
    """to:0x<old>[ add:0xN] -> to:0x<new> add:0x<N+delta> across every module."""
    pat = re.compile(r"\bto:0x%08x\b(?:\s+add:0x(?P<add>[0-9a-fA-F]+))?" % old)
    total, touched = 0, []
    for path in sorted(CONFIG.rglob("relocs.txt")):
        text = path.read_text(encoding="utf-8")

        def sub(m):
            add = int(m.group("add"), 16) if m.group("add") else 0
            return "to:0x%08x add:0x%x" % (old - delta, add + delta)

        new_text, n = pat.subn(sub, text)
        if n:
            total += n
            touched.append((path, n))
            if apply_:
                path.write_text(new_text, encoding="utf-8")
    return total, touched


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("name")
    ap.add_argument("new_addr")
    ap.add_argument("--apply", action="store_true")
    args = ap.parse_args()

    new_addr = int(args.new_addr, 16)
    path, lines, idx, old_addr, m = find_symbol(args.name)
    if new_addr >= old_addr:
        sys.exit(f"error: new addr 0x{new_addr:08x} must be below the current "
                 f"0x{old_addr:08x} (a symbol only ever moves back to its start)")
    check_order(lines, idx, new_addr)
    delta = old_addr - new_addr

    print("%s: 0x%08x -> 0x%08x (-0x%x)" % (args.name, old_addr, new_addr, delta))
    print("  symbols %s:%d" % (path.relative_to(ROOT), idx + 1))
    total, touched = rewrite_relocs(old_addr, delta, args.apply)
    for p, n in touched:
        print("  relocs  %s: %d ref(s) +0x%x" % (p.relative_to(ROOT), n, delta))
    print("  %d relocation(s) across %d module(s)" % (total, len(touched)))

    if args.apply:
        lines[idx] = "%s%s0x%08x%s" % (m.group("name"), m.group("mid"), new_addr,
                                       m.group("tail"))
        path.write_text("\n".join(lines) + "\n", encoding="utf-8")
        print("APPLIED (config only). Now: rm -f build/delinks/.stamp && make delinks")
    else:
        print("DRY RUN -- pass --apply to write the dsd config.")


if __name__ == "__main__":
    main()
