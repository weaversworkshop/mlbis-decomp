#!/usr/bin/env python3
"""Companion to merge_data_syms.py: rewrite .c/.h for a merge -- globals.h
base[N] + drop merged externs, and `&elem` -> `&base[i]` (skips // comments).
Dry-run by default; review the diff (regex-based).
Usage: merge_data_syms_source.py BASE MERGE... --elem-size N [--apply]
       [--addr-from PRE_MERGE_SYMBOLS.txt]."""
import argparse
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SYM_RE = re.compile(r"^(?P<name>\w+)\s+.*\baddr:0x(?P<addr>[0-9a-fA-F]+)\b")


def addr_of(sym_files, name):
    for path in sym_files:
        for line in path.read_text(encoding="utf-8").splitlines():
            m = SYM_RE.match(line)
            if m and m.group("name") == name:
                return int(m.group("addr"), 16)
    return None


def rewrite_outside_comments(text, subs):
    """Apply (pattern, repl) subs only to the code part of each line (pre `//`)."""
    out = []
    changed = False
    for line in text.splitlines(keepends=True):
        nl = line[len(line.rstrip("\r\n")):]
        body = line[:len(line) - len(nl)]
        code, sep, comment = body.partition("//")
        new_code = code
        for pat, repl in subs:
            new_code = pat.sub(repl, new_code)
        if new_code != code:
            changed = True
        out.append(new_code + sep + comment + nl)
    return "".join(out), changed


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("base")
    ap.add_argument("merges", nargs="+")
    ap.add_argument("--elem-size", type=int, default=4)
    ap.add_argument("--config-dir", default=str(ROOT / "config" / "arm9"))
    ap.add_argument("--addr-from", default=None,
                    help="symbols.txt to read addresses from (default: scan config-dir)")
    ap.add_argument("--src-root", default=str(ROOT / "arm9"))
    ap.add_argument("--globals", default=str(ROOT / "arm9" / "include" / "globals.h"))
    ap.add_argument("--apply", action="store_true")
    args = ap.parse_args()

    if args.addr_from:
        sym_files = [Path(args.addr_from)]
    else:
        sym_files = sorted(Path(args.config_dir).rglob("symbols.txt"))
    if not sym_files:
        sys.exit("error: no symbols.txt to read addresses from")

    base_addr = addr_of(sym_files, args.base)
    if base_addr is None:
        sys.exit(f"error: base {args.base} not found in symbols.txt "
                 "(pass --addr-from a pre-merge symbols.txt)")
    idx_of = {}
    for n in args.merges:
        a = addr_of(sym_files, n)
        if a is None:
            sys.exit(f"error: {n} not found in symbols.txt; if the config merge "
                     "already dropped it, pass --addr-from the original file")
        idx_of[n] = (a - base_addr) // args.elem_size
    count = max(idx_of.values()) + 1

    print(f"base {args.base}[{count}]  (elem size {args.elem_size})")
    for n, i in idx_of.items():
        print(f"  {n} -> {args.base}[{i}]")
    print()

    edits = []

    # globals.h: drop merged externs, size the base array.
    gpath = Path(args.globals)
    if gpath.exists():
        text = gpath.read_text(encoding="utf-8")
        for n in idx_of:
            text = re.sub(rf"^[ \t]*extern\b.*\b{re.escape(n)}\b.*;[ \t]*\r?\n", "",
                          text, flags=re.MULTILINE)
        text = re.sub(rf"(\b{re.escape(args.base)})\s*(\[\s*\d*\s*\])?\s*;",
                      rf"\1[{count}];", text)
        edits.append((gpath, text))
        print(f"  {gpath.relative_to(ROOT)}: size {args.base}[{count}], "
              f"drop {', '.join(idx_of)}")

    # .c/.h: rewrite refs, skipping comments.
    subs = []
    for n, i in idx_of.items():
        subs.append((re.compile(rf"&\s*{re.escape(n)}\b"), f"&{args.base}[{i}]"))
        subs.append((re.compile(rf"\b{re.escape(n)}\b"), f"{args.base}[{i}]"))
    src_root = Path(args.src_root)
    for cpath in sorted(src_root.rglob("*.c")) + sorted(src_root.rglob("*.h")):
        if cpath == gpath:
            continue
        new_text, changed = rewrite_outside_comments(
            cpath.read_text(encoding="utf-8"), subs)
        if changed:
            edits.append((cpath, new_text))
            print(f"  {cpath.relative_to(ROOT)}: rewrote refs (comments left intact)")

    if not edits:
        print("  (nothing to rewrite)")
    if not args.apply:
        print("\nDRY RUN -- pass --apply to write the source edits.")
        return
    for path, text in edits:
        path.write_text(text, encoding="utf-8")
    print(f"\nAPPLIED: {len(edits)} source file(s). Review `git diff`, then objdiff.")


if __name__ == "__main__":
    main()
