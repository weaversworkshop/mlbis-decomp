#!/usr/bin/env python3
"""
Fuzzy SDK near-match scanner.

tu_diff/sdk_equiv only find EXACT instruction-skeleton matches, so SDK functions
with compiler/regalloc *drift* (e.g. FS_LoadOverlayInfo: identical body but one
differing `pop` reg-list) are invisible -- they sit in the unmapped gap looking
like game code. This finds them: for each still-unmapped gap function, it locates
the closest SDK function skeleton allowing N instruction differences, and reports
the diff so a human (then sdk_equiv) can confirm.

Reuses tu_diff's disasm/normalize (reloc-masked instruction tuples).

  python tools/scripts/fuzzy_sdk_match.py                # scan main_7 gap
  python tools/scripts/fuzzy_sdk_match.py --sim 0.80 --min 10
  python tools/scripts/fuzzy_sdk_match.py --show func_0203e22c   # diff one func
"""
import argparse
import difflib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools" / "scripts"))
import tu_diff  # noqa: E402

GAP_DEFAULT = ROOT / "build/delinks/_dsd_gap@main_7.o"


def load_sdk():
    """name -> (key_tuple, source.o) across every built SDK .o."""
    out = {}
    for o in sorted((ROOT / "build" / "lib").rglob("*.o")):
        try:
            for name, (key, _refs) in tu_diff.load_funcs(o).items():
                # ignore the .isa marker line for length; keep full key for diff
                if name not in out:
                    out[name] = (key, o)
        except Exception:
            continue
    return out


def insn_len(key):
    return sum(1 for k in key if not k.startswith(".isa"))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--gap", default=str(GAP_DEFAULT))
    ap.add_argument("--sim", type=float, default=0.85,
                    help="min similarity to report (default 0.85; exact=1.0 excluded)")
    ap.add_argument("--min", type=int, default=12,
                    help="min instruction count to consider (short funcs are noise)")
    ap.add_argument("--lentol", type=float, default=0.20,
                    help="only compare funcs within this relative length delta")
    ap.add_argument("--show", help="print the full skeleton diff for one gap func vs its best SDK candidate")
    args = ap.parse_args()

    sys.stderr.write("loading SDK objects...\n")
    sdk = load_sdk()
    # bucket SDK keys by length for prefilter
    by_len = {}
    for name, (key, src) in sdk.items():
        by_len.setdefault(insn_len(key), []).append((name, key, src))
    sys.stderr.write(f"  {len(sdk)} SDK functions indexed\n")

    gap = tu_diff.load_funcs(Path(args.gap))
    sdk_keyset = {k for k, _ in sdk.values()}  # for exact-match exclusion

    results = []
    for gname, (gkey, _refs) in gap.items():
        if not gname.startswith("func_"):
            continue  # only still-unmapped
        gl = insn_len(gkey)
        if gl < args.min:
            continue
        if gkey in sdk_keyset:
            continue  # exact match -> tu_diff territory, not "near"
        lo, hi = int(gl * (1 - args.lentol)), int(gl * (1 + args.lentol)) + 1
        best = None
        for L in range(lo, hi + 1):
            for sname, skey, src in by_len.get(L, []):
                sm = difflib.SequenceMatcher(None, gkey, skey)
                if sm.real_quick_ratio() < args.sim or sm.quick_ratio() < args.sim:
                    continue
                r = sm.ratio()
                if r >= args.sim and r < 1.0:
                    if best is None or r > best[0]:
                        ndiff = (len(gkey) + len(skey)) - 2 * sum(
                            b.size for b in sm.get_matching_blocks())
                        best = (r, sname, src, ndiff, skey)
        if best:
            results.append((best[0], gname, gl, best[1], best[3], best[2], gkey, best[4]))

    results.sort(reverse=True)

    if args.show:
        for sim, gname, gl, sname, ndiff, src, gkey, skey in results:
            if gname == args.show:
                print(f"# {gname} ~ {sname}  sim={sim*100:.1f}%  diffs={ndiff}")
                for line in difflib.unified_diff(
                    [k for k in gkey if not k.startswith('.isa')],
                    [k for k in skey if not k.startswith('.isa')],
                    fromfile=gname, tofile=sname, lineterm=""):
                    print(line)
                return
        print(f"{args.show}: no near-match candidate found")
        return

    print(f"\n=== FUZZY SDK NEAR-MATCHES (sim >= {args.sim}, >= {args.min} insns) ===")
    print(f"{len(results)} unmapped gap funcs have a close-but-not-exact SDK candidate\n")
    print(f"{'sim':>6} {'diffs':>5}  {'gap func':<18} {'~ SDK candidate':<28} insns  source")
    for sim, gname, gl, sname, ndiff, src, _gk, _sk in results:
        lib = str(src).split("/lib/")[-1].split("/src/")[0].split("/")[-1]
        print(f"{sim*100:5.1f}% {ndiff:5d}  {gname:<18} {sname:<28} {gl:>4}   {lib}")


if __name__ == "__main__":
    main()
