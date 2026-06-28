#!/usr/bin/env python3
"""Merge dsd-split data_* symbols into one array -- CONFIG ONLY (symbols.txt
delete + relocs.txt re-anchor with computed `add:`; dsd needs an explicit add:
for mid-symbol refs). Dry-run by default.
Usage: merge_data_syms.py BASE MERGE... [--apply] [--elem-size N].
For the .c/.h edits run merge_data_syms_source.py."""
import argparse
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SYM_RE = re.compile(r"^(?P<name>\w+)\s+.*\baddr:0x(?P<addr>[0-9a-fA-F]+)\b")


def find_config_files(config_dir, leaf):
    files = sorted(config_dir.rglob(leaf))
    if not files:
        sys.exit(f"error: no {leaf} found under {config_dir}")
    return files


def parse_symbols(path):
    """addr(int) -> (name, lineno, rawline) for one symbols.txt."""
    out = {}
    for i, line in enumerate(path.read_text(encoding="utf-8").splitlines()):
        m = SYM_RE.match(line)
        if m:
            out[int(m.group("addr"), 16)] = (m.group("name"), i, line)
    return out


def locate_symbol(sym_files, name):
    for path in sym_files:
        amap = parse_symbols(path)
        for addr, (sname, _, _) in amap.items():
            if sname == name:
                return path, amap, addr
    sys.exit(f"error: symbol {name} not found in any symbols.txt")


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("base", help="base symbol to keep (becomes the array)")
    ap.add_argument("merges", nargs="+", help="element symbol(s) to fold into base")
    ap.add_argument("--config-dir", default=str(ROOT / "config" / "arm9"))
    ap.add_argument("--apply", action="store_true", help="write config changes")
    ap.add_argument("--elem-size", type=int, default=4,
                    help="element size in bytes (informational, for the summary only)")
    args = ap.parse_args()

    config_dir = Path(args.config_dir)
    sym_files = find_config_files(config_dir, "symbols.txt")
    reloc_files = find_config_files(config_dir, "relocs.txt")

    sym_path, amap, base_addr = locate_symbol(sym_files, args.base)
    name_by_addr = {a: n for a, (n, _, _) in amap.items()}

    merges = []  # (name, addr, delta, lineno)
    for name in args.merges:
        hit = [(a, ln) for a, (n, ln, _) in amap.items() if n == name]
        if not hit:
            sys.exit(f"error: merge symbol {name} not in {sym_path} "
                     f"(must share the base's module)")
        addr, lineno = hit[0]
        delta = addr - base_addr
        if delta <= 0:
            sys.exit(f"error: {name} (0x{addr:08x}) is not after base "
                     f"{args.base} (0x{base_addr:08x})")
        merges.append((name, addr, delta, lineno))

    merges.sort(key=lambda x: x[1])
    merge_addrs = {a for _, a, _, _ in merges}
    top_merge = max(merge_addrs)

    interlopers = [name_by_addr[a] for a in amap
                   if base_addr < a <= top_merge and a not in merge_addrs]
    if interlopers:
        sys.exit(f"error: symbols between base and merged range not being merged: "
                 f"{', '.join(sorted(interlopers))}\n"
                 f"       merge them too, or pick a contiguous set.")

    count = (top_merge - base_addr) // args.elem_size + 1
    print(f"base  {args.base}  @ 0x{base_addr:08x}  in {sym_path.relative_to(ROOT)}")
    for name, addr, delta, _ in merges:
        print(f"merge {name}  @ 0x{addr:08x}  -> {args.base} + 0x{delta:x} "
              f"(= [{delta // args.elem_size}])")
    print(f"result: {args.base}[{count}] (elem size {args.elem_size}, informational)\n")

    # ---- relocs.txt: re-anchor `to:<merge>` -> `to:<base> add:<delta>` ----
    reloc_edits = []
    total = 0
    for path in reloc_files:
        lines = path.read_text(encoding="utf-8").splitlines()
        changed = False
        for i, line in enumerate(lines):
            toks = line.split()
            tomap = dict(t.split(":", 1) for t in toks if ":" in t)
            to_s = tomap.get("to")
            if not to_s:
                continue
            try:
                to_addr = int(to_s, 16)
            except ValueError:
                continue
            if to_addr not in merge_addrs:
                continue
            new_add = int(tomap.get("add", "0x0"), 16) + (to_addr - base_addr)
            new_toks = []
            for t in toks:
                if t.startswith("to:"):
                    new_toks.append(f"to:0x{base_addr:08x}")
                    new_toks.append(f"add:0x{new_add:x}")
                elif t.startswith("add:"):
                    continue
                else:
                    new_toks.append(t)
            lines[i] = " ".join(new_toks)
            changed = True
            total += 1
            print(f"  reloc {path.relative_to(ROOT)}:{i + 1}\n"
                  f"    - {line}\n    + {lines[i]}")
        if changed:
            reloc_edits.append((path, "\n".join(lines) + "\n"))
    if total == 0:
        print("  (no relocs reference the merged address(es) -- nothing to re-anchor)")

    # ---- symbols.txt: drop merged symbol lines ----
    sym_lines = sym_path.read_text(encoding="utf-8").splitlines()
    drop = {ln for _, _, _, ln in merges}
    new_sym_text = "\n".join(l for i, l in enumerate(sym_lines) if i not in drop) + "\n"
    print(f"\n  symbols {sym_path.relative_to(ROOT)}: drop "
          f"{', '.join(n for n, _, _, _ in merges)}")

    if not args.apply:
        print("\nDRY RUN -- pass --apply to write the dsd config.\n"
              "Source (.c/.h) is left to you: edit globals.h + refs by hand, or run\n"
              "  python tools/scripts/merge_data_syms_source.py "
              f"{args.base} {' '.join(args.merges)} --apply")
        return

    for path, text in reloc_edits:
        path.write_text(text, encoding="utf-8")
    sym_path.write_text(new_sym_text, encoding="utf-8")
    print(f"\nAPPLIED (config only): {len(reloc_edits)} relocs.txt + 1 symbols.txt.\n"
          "Now update the source yourself (globals.h array size + drop externs, and\n"
          f"`&{args.merges[0]}` -> `&{args.base}[i]`), then `make delinks` && objdiff.")


if __name__ == "__main__":
    main()
