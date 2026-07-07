#!/usr/bin/env python3
"""Per-function asm diff: compiled .o vs delinked target, normalized.

Usage:
  fn_diff.py <symbol> [--mine PATH] [--target PATH] [--pool] [--context N]

Exit codes: 0 = match, 1 = diff, 2 = extraction/setup error (NEVER trust a
"match" from empty extractions -- this tool guards against that).

If --mine is omitted, searches build/arm9/src/**/*.o (then build/scratch/**/*.o)
for the symbol. Target defaults to build/delinks/_dsd_gap@main_7.o.
"""
import argparse
import difflib
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
OBJDUMP = "arm-none-eabi-objdump"
SYM_RE = re.compile(r"^[0-9a-f]+ <([^>]+)>:")
INSN_RE = re.compile(r"^\s*[0-9a-f]+:\s*[0-9a-f]{4,8}\s+(\S.*)$")
BRANCH_RE = re.compile(
    r"\b(b|bl|blx|bx|beq|bne|bcs|bcc|bmi|bpl|bvs|bvc|bhi|bls|bge|blt|bgt|ble|bhs|blo)"
    r"(\.w)?\s+(0x)?[0-9a-f]+"
)
DATA_MNEMONICS = (".word", ".short", ".byte", ".ascii")


def die(msg):
    print(msg, file=sys.stderr)
    sys.exit(2)


def objdump(path):
    try:
        r = subprocess.run([OBJDUMP, "-d", str(path)], capture_output=True, text=True)
    except FileNotFoundError:
        die(f"error: {OBJDUMP} not found on PATH")
    if r.returncode != 0:
        die(f"error: objdump failed on {path}: {r.stderr.strip()}")
    return r.stdout.splitlines()


def extract(lines, symbol, keep_pool):
    out, active, found = [], False, False
    for line in lines:
        m = SYM_RE.match(line)
        if m:
            name = m.group(1)
            if name == symbol:
                active, found = True, True
                continue
            if active and not name.startswith(".L"):
                active = False
            continue
        if not active:
            continue
        im = INSN_RE.match(line)
        if not im:
            continue
        insn = im.group(1)
        insn = re.sub(r"\s*[@;].*$", "", insn)
        insn = re.sub(r"\s*<[^>]*>\s*$", "", insn)
        insn = insn.rstrip()
        if not insn:
            continue
        is_data = (
            insn.startswith(DATA_MNEMONICS)
            or not re.match(r"^[a-z]", insn)
            or re.fullmatch(r"[!-~]{0,4}\.\.\.", insn)
        )
        if is_data and not keep_pool:
            continue
        insn = BRANCH_RE.sub(lambda m2: m2.group(1) + (m2.group(2) or "") + " <T>", insn)
        out.append(insn)
    return out, found


def find_mine(symbol):
    for base in (ROOT / "build/arm9/src", ROOT / "build/scratch"):
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
    ap.add_argument("--target", default=str(ROOT / "build/delinks/_dsd_gap@main_7.o"))
    ap.add_argument("--pool", action="store_true", help="include literal-pool/data lines")
    ap.add_argument("--context", type=int, default=3)
    args = ap.parse_args()

    mine_path = Path(args.mine) if args.mine else find_mine(args.symbol)
    if not mine_path or not mine_path.is_file():
        die(f"error: no compiled .o contains '{args.symbol}' (searched build/arm9/src, build/scratch); pass --mine")
    target_path = Path(args.target)
    if not target_path.is_file():
        die(f"error: target not found: {target_path}")

    tgt, tfound = extract(objdump(target_path), args.symbol, args.pool)
    mine, mfound = extract(objdump(mine_path), args.symbol, args.pool)
    if not tfound:
        die(f"error: symbol '{args.symbol}' not in target {target_path.name}")
    if not mfound:
        die(f"error: symbol '{args.symbol}' not in {mine_path}")
    if not tgt or not mine:
        die(f"error: EMPTY EXTRACTION (target={len(tgt)}, mine={len(mine)} insns) -- refusing to compare")

    if tgt == mine:
        mp = mine_path.resolve()
        shown = mp.relative_to(ROOT) if mp.is_relative_to(ROOT) else mp
        print(f"MATCH ({len(mine)} instructions)  [{shown}]")
        return 0
    diff = list(
        difflib.unified_diff(tgt, mine, "target", "mine", n=args.context, lineterm="")
    )
    changed = sum(1 for d in diff if d[:1] in "+-" and d[:3] not in ("+++", "---"))
    print("\n".join(diff))
    print(f"\nDIFF: {changed} lines differ (target={len(tgt)}, mine={len(mine)} insns)")
    return 1


if __name__ == "__main__":
    sys.exit(main())
