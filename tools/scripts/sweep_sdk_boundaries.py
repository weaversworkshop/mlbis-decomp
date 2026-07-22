#!/usr/bin/env python3
"""Sweep SDK-vs-game function mappings for boundary anomalies.

Two false-positive species this catches:
  A) An SDK-named function marooned inside a run of game functions (the
     ENV_SetResourceSet trap: a too-simple wrapper skeleton-matched an SDK
     function, but its bl targets are actually game funcs).
  B) A func_* marooned inside a run of SDK functions (an unnamed SDK internal
     = a mapping opportunity, or rarely a misplaced game func).

Heuristic passes over-report because *unnamed* SDK functions are also func_*,
so they read as "game" neighbors. The DEFINITIVE tests are the reliable ones:
  T1: an SDK-mapped function that CALLS a _ZN game method   -> real mis-map
  T2: an SDK-mapped function whose ADDRESS is in the game gap -> real mis-map
  T3: a _ZN mangled function OUTSIDE known game regions       -> misplaced

Usage: python tools/scripts/sweep_sdk_boundaries.py [--verbose]
Exit 0 if T1/T2/T3 are all clean, 1 otherwise.
"""
import re, sys

SYMBOLS = 'config/arm9/symbols.txt'
RELOCS  = 'config/arm9/relocs.txt'

# Known game-code regions (inferred TU spans). Update as TUs are confirmed.
GAME_REGIONS = [
    (0x02004000, 0x02004074),   # crt0 game entry bits
    (0x02015e00, 0x0201cf20),   # main_7 gap (clGlobalEvent..FS)
    (0x020206e0, 0x02020ab0),   # clYanaAnim / clRfx
    (0x0203a8dc, 0x0203a920),   # cpu_mem
]
GAP_LO, GAP_HI = 0x02015e00, 0x0201cf20  # the big contiguous game gap

def is_game_name(n):
    return bool(re.match(r'func_[0-9a-f]+$', n)) or n.startswith('_Z')
def in_game_region(a):
    return any(lo <= a < hi for lo, hi in GAME_REGIONS)

def load():
    funcs = []
    for line in open(SYMBOLS):
        m = re.match(r'(\S+)\s+kind:function\(arm,size=0x([0-9a-f]+)\)\s+addr:0x([0-9a-f]+)', line)
        if m:
            funcs.append((int(m.group(3),16), m.group(1), int(m.group(2),16)))
    funcs.sort()
    tos = {}
    for line in open(RELOCS):
        m = re.search(r'from:0x([0-9a-f]+) kind:(?:arm_call|load) to:0x([0-9a-f]+)', line)
        if m:
            tos.setdefault(int(m.group(1),16), []).append(int(m.group(2),16))
    return funcs, tos

def callees(a, s, tos, byaddr):
    out = []
    for f, lst in tos.items():
        if a <= f < a+s:
            out += [byaddr.get(t, f'?{t:08x}') for t in lst]
    return out

def main():
    verbose = '--verbose' in sys.argv
    funcs, tos = load()
    byaddr = {a: n for a, n, s in funcs}
    sdk = lambda n: not is_game_name(n) and n != 'main'

    fails = 0
    print("=== T1: SDK-mapped function that CALLS a _ZN game method (mis-map) ===")
    hit = 0
    for a, n, s in funcs:
        if not sdk(n): continue
        zn = [c for c in callees(a, s, tos, byaddr) if c.startswith('_Z')]
        if zn:
            print(f"  MISMAP 0x{a:08x} {n} -> {zn}"); hit += 1; fails += 1
    if not hit: print("  clean")

    print("=== T2: SDK-mapped function whose ADDRESS is in the game gap (mis-map) ===")
    hit = 0
    for a, n, s in funcs:
        if sdk(n) and GAP_LO <= a < GAP_HI:
            print(f"  MISMAP 0x{a:08x} {n}"); hit += 1; fails += 1
    if not hit: print("  clean")

    print("=== T3: _ZN mangled function OUTSIDE known game regions (misplaced) ===")
    hit = 0
    for a, n, s in funcs:
        if n.startswith('_ZN') and not in_game_region(a):
            print(f"  MISPLACED 0x{a:08x} {n}"); hit += 1; fails += 1
    if not hit: print("  clean")

    if verbose:
        print("\n=== [heuristic] SDK marooned between two game-named funcs ===")
        for i in range(1, len(funcs)-1):
            a, n, s = funcs[i]
            if sdk(n) and is_game_name(funcs[i-1][1]) and is_game_name(funcs[i+1][1]):
                cs = callees(a, s, tos, byaddr)
                g = [c for c in cs if c.startswith('func_') or c.startswith('_Z')]
                print(f"  0x{a:08x} {n}{'  <== calls game-ish' if g else ''}")
                if cs: print(f"       calls: {', '.join(cs)}")
        print("\n=== [heuristic] func_* marooned between two SDK funcs (unnamed-SDK candidates) ===")
        for i in range(1, len(funcs)-1):
            a, n, s = funcs[i]
            if n.startswith('func_') and sdk(funcs[i-1][1]) and sdk(funcs[i+1][1]):
                cs = callees(a, s, tos, byaddr)
                print(f"  0x{a:08x} {n}  [{funcs[i-1][1]} | {funcs[i+1][1]}]")
                if cs: print(f"       calls: {', '.join(cs)}")

    print(f"\n{'FAIL' if fails else 'PASS'}: {fails} definitive anomalies")
    return 1 if fails else 0

if __name__ == '__main__':
    sys.exit(main())
