#!/usr/bin/env python3
import argparse
import difflib
import re
import subprocess
import sys
from pathlib import Path

from common import BUILD, GAP_O, OBJDUMP, ROOT, die

SYM_RE = re.compile(r"^[0-9a-f]+ <([^>]+)>:")
ADDR_INSN_RE = re.compile(r"^\s*([0-9a-f]+):\s*[0-9a-f]{4,8}\s+(\S.*)$")
BRANCH_RE = re.compile(
    r"\b(b|bl|blx|bx|beq|bne|bcs|bcc|bmi|bpl|bvs|bvc|bhi|bls|bge|blt|bgt|ble|bhs|blo)"
    r"(\.w)?\s+(?:0x)?([0-9a-f]+)"
)
DATA_MNEMONICS = (".word", ".short", ".byte", ".ascii")
SLOT_RE = re.compile(r"\[sp, #(\d+)\]")


def objdump(path):
    try:
        r = subprocess.run([OBJDUMP, "-d", str(path)], capture_output=True, text=True)
    except FileNotFoundError:
        die(f"error: {OBJDUMP} not found on PATH")
    if r.returncode != 0:
        die(f"error: objdump failed on {path}: {r.stderr.strip()}")
    return r.stdout.splitlines()


def extract_items(lines, symbol):
    """[(addr, insn, is_data, branch_target|None)]"""
    out, active, found = [], False, False
    for line in lines:
        m = SYM_RE.match(line)
        if m:
            name = m.group(1)
            if name == symbol:
                active, found = True, True
            elif active and not name.startswith(".L"):
                active = False
            continue
        if not active:
            continue
        im = ADDR_INSN_RE.match(line)
        if not im:
            continue
        addr = int(im.group(1), 16)
        insn = re.sub(r"\s*[@;].*$", "", im.group(2))
        insn = re.sub(r"\s*<[^>]*>\s*$", "", insn).rstrip()
        if not insn:
            continue
        is_data = bool(
            insn.startswith(DATA_MNEMONICS)
            or not re.match(r"^[a-z]", insn)
            or re.fullmatch(r"[!-~]{0,4}\.\.\.", insn)
        )
        bm = BRANCH_RE.search(insn)
        # calls/bx are relocs (offset 0 in a .o): never relativize those
        tgt = int(bm.group(3), 16) if bm and not is_data and bm.group(1) not in ("bl", "blx", "bx") else None
        out.append((addr, insn, is_data, tgt))
    return out, found


def relativize(items, keep_pool):
    """Branch targets -> instruction-index delta `<T+3>`; leaving the function stays `<T>`.
    Returns (texts, is_backward_branch flags)."""
    index = {}
    for addr, _, is_data, _ in items:
        if not is_data:
            index[addr] = len(index)
    out, back = [], []
    for addr, insn, is_data, tgt in items:
        if is_data and not keep_pool:
            continue
        rep = "<T>"
        if tgt is not None and tgt in index and addr in index:
            rep = f"<T{index[tgt] - index[addr]:+d}>"
        out.append(BRANCH_RE.sub(lambda m: m.group(1) + (m.group(2) or "") + " " + rep, insn))
        back.append(tgt is not None and tgt < addr and not insn.startswith("bl"))
    return out, back


def extract(lines, symbol, keep_pool):
    items, found = extract_items(lines, symbol)
    return relativize(items, keep_pool)[0], found


def frame_map(items):
    """{slot: index of first instruction touching it}"""
    first, n = {}, 0
    for _, insn, is_data, _ in items:
        if is_data:
            continue
        m = SLOT_RE.search(insn)
        if m and int(m.group(1)) not in first:
            first[int(m.group(1))] = n
        n += 1
    return first


def print_frame(tgt_items, mine_items):
    by_t = {i: s for s, i in frame_map(tgt_items).items()}
    by_m = {i: s for s, i in frame_map(mine_items).items()}
    print(f"{'first@':>7} {'target':>7} {'mine':>7}")
    for i in sorted(set(by_t) | set(by_m)):
        ts, ms = by_t.get(i), by_m.get(i)
        mark = "" if ts == ms else "  <-"
        print(f"{i:>7} {('' if ts is None else ts):>7} {('' if ms is None else ms):>7}{mark}")
    print("ascending slot order = reverse first-assignment order of the variables (additional webs);")
    print("first webs follow declaration order. Adjacent same-kind slots in section order = one shared variable.")


def find_mine(symbol):
    for base in (BUILD / "arm9/src", BUILD / "scratch"):
        if not base.is_dir():
            continue
        for o in sorted(base.rglob("*.o")):
            r = subprocess.run([OBJDUMP, "-t", str(o)], capture_output=True, text=True)
            if r.returncode == 0 and re.search(
                rf"^\S+(?!.*\*UND\*).* {re.escape(symbol)}$", r.stdout, re.M
            ):
                return o
    return None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("symbol")
    ap.add_argument("--mine")
    ap.add_argument("--target", default=str(GAP_O))
    ap.add_argument("--pool", action="store_true", help="include literal-pool/data lines")
    ap.add_argument("--context", type=int, default=3)
    ap.add_argument("--target-symbol", help="target-side name when it differs (e.g. func_<addr> vs mangled)")
    ap.add_argument("--frame", action="store_true", help="also print the spill-slot table (target vs mine, by first use)")
    args = ap.parse_args()

    mine_path = Path(args.mine) if args.mine else find_mine(args.symbol)
    if not mine_path or not mine_path.is_file():
        die(f"error: no compiled .o contains '{args.symbol}' (searched build/arm9/src, build/scratch); pass --mine")
    target_path = Path(args.target)
    if not target_path.is_file():
        die(f"error: target not found: {target_path}")

    tsym = args.target_symbol or args.symbol
    titems, tfound = extract_items(objdump(target_path), tsym)
    mitems, mfound = extract_items(objdump(mine_path), args.symbol)
    tgt, _ = relativize(titems, args.pool)
    mine, _ = relativize(mitems, args.pool)
    if args.frame and tfound and mfound:
        print_frame(titems, mitems)
        print()
    if not tfound:
        die(f"error: symbol '{tsym}' not in target {target_path.name}")
    if not mfound:
        die(f"error: symbol '{args.symbol}' not in {mine_path}")
    if not tgt or not mine:
        die(f"error: EMPTY EXTRACTION (target={len(tgt)}, mine={len(mine)} insns) -- refusing to compare")

    if tgt == mine:
        mp = mine_path.resolve()
        shown = mp.relative_to(ROOT) if mp.is_relative_to(ROOT) else mp
        print(f"MATCH ({len(mine)} instructions)  [{shown}]")
        return 0
    diff = list(difflib.unified_diff(tgt, mine, "target", "mine", n=args.context, lineterm=""))
    changed = sum(1 for d in diff if d[:1] in "+-" and d[:3] not in ("+++", "---"))
    print("\n".join(diff))
    print(f"\nDIFF: {changed} lines differ (target={len(tgt)}, mine={len(mine)} insns)")
    return 1


if __name__ == "__main__":
    sys.exit(main())
