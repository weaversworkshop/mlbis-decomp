#!/usr/bin/env python3
import argparse
import re
import sys
from pathlib import Path

from common import CONFIG, INCLUDE, ROOT

MERGE_SYM_RE = re.compile(r"^(?P<name>\w+)\s+.*\baddr:0x(?P<addr>[0-9a-fA-F]+)\b")
REBASE_SYM_RE = re.compile(r"^(?P<name>\S+)(?P<mid>\s+kind:\S+\s+addr:)0x(?P<addr>[0-9a-fA-F]+)"
                           r"(?P<tail>.*?)\s*$")


def config_files(config_dir, leaf):
    files = sorted(config_dir.rglob(leaf))
    if not files:
        sys.exit(f"error: no {leaf} found under {config_dir}")
    return files


def parse_symbols(path):
    out = {}
    for i, line in enumerate(path.read_text(encoding="utf-8").splitlines()):
        if m := MERGE_SYM_RE.match(line):
            out[int(m.group("addr"), 16)] = (m.group("name"), i)
    return out


def addr_of(sym_files, name):
    for path in sym_files:
        for line in path.read_text(encoding="utf-8").splitlines():
            m = MERGE_SYM_RE.match(line)
            if m and m.group("name") == name:
                return int(m.group("addr"), 16)
    return None


def merge(args):
    config_dir = Path(args.config_dir)
    sym_files = config_files(config_dir, "symbols.txt")
    reloc_files = config_files(config_dir, "relocs.txt")

    for sym_path in sym_files:
        amap = parse_symbols(sym_path)
        base_addr = next((a for a, (n, _) in amap.items() if n == args.base), None)
        if base_addr is not None:
            break
    else:
        sys.exit(f"error: symbol {args.base} not found in any symbols.txt")

    merges = []
    for name in args.merges:
        hit = [(a, ln) for a, (n, ln) in amap.items() if n == name]
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

    interlopers = [amap[a][0] for a in amap if base_addr < a <= top_merge and a not in merge_addrs]
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

    reloc_edits = []
    total = 0
    for path in reloc_files:
        lines = path.read_text(encoding="utf-8").splitlines()
        changed = False
        for i, line in enumerate(lines):
            toks = line.split()
            tomap = dict(t.split(":", 1) for t in toks if ":" in t)
            try:
                to_addr = int(tomap.get("to", ""), 16)
            except ValueError:
                continue
            if to_addr not in merge_addrs:
                continue
            new_add = int(tomap.get("add", "0x0"), 16) + (to_addr - base_addr)
            new_toks = []
            for t in toks:
                if t.startswith("to:"):
                    new_toks += [f"to:0x{base_addr:08x}", f"add:0x{new_add:x}"]
                elif not t.startswith("add:"):
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

    sym_lines = sym_path.read_text(encoding="utf-8").splitlines()
    drop = {ln for _, _, _, ln in merges}
    new_sym_text = "\n".join(l for i, l in enumerate(sym_lines) if i not in drop) + "\n"
    print(f"\n  symbols {sym_path.relative_to(ROOT)}: drop "
          f"{', '.join(n for n, _, _, _ in merges)}")

    if not args.apply:
        print("\nDRY RUN -- pass --apply to write the dsd config.\n"
              "Source (.c/.h) is left to you: edit globals.h + refs by hand, or run\n"
              "  python tools/scripts/data_syms.py merge-source "
              f"{args.base} {' '.join(args.merges)} --apply")
        return

    for path, text in reloc_edits:
        path.write_text(text, encoding="utf-8")
    sym_path.write_text(new_sym_text, encoding="utf-8")
    print(f"\nAPPLIED (config only): {len(reloc_edits)} relocs.txt + 1 symbols.txt.\n"
          "Now update the source yourself (globals.h array size + drop externs, and\n"
          f"`&{args.merges[0]}` -> `&{args.base}[i]`), then `make delinks` && objdiff.")


def rewrite_outside_comments(text, subs):
    out = []
    changed = False
    for line in text.splitlines(keepends=True):
        nl = line[len(line.rstrip("\r\n")):]
        code, sep, comment = line[:len(line) - len(nl)].partition("//")
        new_code = code
        for pat, repl in subs:
            new_code = pat.sub(repl, new_code)
        changed |= new_code != code
        out.append(new_code + sep + comment + nl)
    return "".join(out), changed


def merge_source(args):
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

    subs = []
    for n, i in idx_of.items():
        subs.append((re.compile(rf"&\s*{re.escape(n)}\b"), f"&{args.base}[{i}]"))
        subs.append((re.compile(rf"\b{re.escape(n)}\b"), f"{args.base}[{i}]"))
    src_root = Path(args.src_root)
    for cpath in sorted(src_root.rglob("*.c")) + sorted(src_root.rglob("*.h")):
        if cpath == gpath:
            continue
        new_text, changed = rewrite_outside_comments(cpath.read_text(encoding="utf-8"), subs)
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


def rebase(args):
    new_addr = int(args.new_addr, 16)
    for path in sorted(CONFIG.rglob("symbols.txt")):
        lines = path.read_text(encoding="utf-8").splitlines()
        idx = next((i for i, line in enumerate(lines)
                    if (m := REBASE_SYM_RE.match(line)) and m.group("name") == args.name), None)
        if idx is not None:
            break
    else:
        sys.exit(f"error: symbol {args.name} not found in any symbols.txt")
    m = REBASE_SYM_RE.match(lines[idx])
    old_addr = int(m.group("addr"), 16)
    if new_addr >= old_addr:
        sys.exit(f"error: new addr 0x{new_addr:08x} must be below the current "
                 f"0x{old_addr:08x} (a symbol only ever moves back to its start)")

    def addr_at(j):
        mm = REBASE_SYM_RE.match(lines[j])
        return int(mm.group("addr"), 16) if mm else None

    prev = next((a for j in range(idx - 1, -1, -1) if (a := addr_at(j)) is not None), None)
    nxt = next((a for j in range(idx + 1, len(lines)) if (a := addr_at(j)) is not None), None)
    if prev is not None and new_addr < prev:
        sys.exit(f"error: 0x{new_addr:08x} sorts before the previous symbol 0x{prev:08x}")
    if nxt is not None and new_addr > nxt:
        sys.exit(f"error: 0x{new_addr:08x} sorts after the next symbol 0x{nxt:08x}")
    delta = old_addr - new_addr

    print("%s: 0x%08x -> 0x%08x (-0x%x)" % (args.name, old_addr, new_addr, delta))
    print("  symbols %s:%d" % (path.relative_to(ROOT), idx + 1))
    pat = re.compile(r"\bto:0x%08x\b(?:\s+add:0x(?P<add>[0-9a-fA-F]+))?" % old_addr)

    def sub(mm):
        add = int(mm.group("add"), 16) if mm.group("add") else 0
        return "to:0x%08x add:0x%x" % (new_addr, add + delta)

    total, modules = 0, 0
    for rpath in sorted(CONFIG.rglob("relocs.txt")):
        new_text, n = pat.subn(sub, rpath.read_text(encoding="utf-8"))
        if n:
            total += n
            modules += 1
            print("  relocs  %s: %d ref(s) +0x%x" % (rpath.relative_to(ROOT), n, delta))
            if args.apply:
                rpath.write_text(new_text, encoding="utf-8")
    print("  %d relocation(s) across %d module(s)" % (total, modules))

    if args.apply:
        lines[idx] = "%s%s0x%08x%s" % (m.group("name"), m.group("mid"), new_addr, m.group("tail"))
        path.write_text("\n".join(lines) + "\n", encoding="utf-8")
        print("APPLIED (config only). Now: rm -f build/delinks/.stamp && make delinks")
    else:
        print("DRY RUN -- pass --apply to write the dsd config.")


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)

    p = sub.add_parser("merge", help="fold data_* symbols into BASE (symbols.txt + relocs.txt)")
    p.add_argument("base", help="base symbol to keep (becomes the array)")
    p.add_argument("merges", nargs="+", help="element symbol(s) to fold into base")
    p.add_argument("--config-dir", default=str(CONFIG))
    p.add_argument("--apply", action="store_true", help="write config changes")
    p.add_argument("--elem-size", type=int, default=4,
                   help="element size in bytes (informational, for the summary only)")
    p.set_defaults(fn=merge)

    p = sub.add_parser("merge-source", help="rewrite globals.h + refs for a merge")
    p.add_argument("base")
    p.add_argument("merges", nargs="+")
    p.add_argument("--elem-size", type=int, default=4)
    p.add_argument("--config-dir", default=str(CONFIG))
    p.add_argument("--addr-from", default=None,
                   help="symbols.txt to read addresses from (default: scan config-dir)")
    p.add_argument("--src-root", default=str(ROOT / "arm9"))
    p.add_argument("--globals", default=str(INCLUDE / "globals.h"))
    p.add_argument("--apply", action="store_true")
    p.set_defaults(fn=merge_source)

    p = sub.add_parser("rebase", help="move a data symbol back to its object start")
    p.add_argument("name")
    p.add_argument("new_addr")
    p.add_argument("--apply", action="store_true")
    p.set_defaults(fn=rebase)

    args = ap.parse_args()
    args.fn(args)


if __name__ == "__main__":
    main()
