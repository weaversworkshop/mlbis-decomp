#!/usr/bin/env python3
"""tu_cluster.py -- group a module's functions into candidate translation units.

Scope is PER MODULE. Functions in different modules (main vs each overlay) are
never the same TU; they interact only via global symbols ("header exposure").
So this tool operates on ONE module's symbols/relocs at a time (default: main)
and never groups across the module boundary.

Within a module, the strongest "same TU" signal is SHARED PRIVATE DATA: two
functions that reference the same NARROWLY-shared data symbol almost certainly
live in the same TU -- that data is the TU's file-local static state. Widely
shared data (referenced by many functions) is a global, a weak signal, ignored.

  conservative (default): edges only from shared narrow data.
  --calls               : also add call-graph edges (caller<->callee).

A "todo" function is one still named func_XXXX (not yet given an SDK/real name).
Clusters containing >=1 todo function are the work-list: pick one, pull its
members into a shared .c, settle its shared data (static-in-.c if it stays in
this cluster, extern-in-header if other clusters/modules also use it), grow it.

Usage:
  tu_cluster.py                      # main module, conservative
  tu_cluster.py --calls              # also use call edges (bigger clusters)
  tu_cluster.py --module ov002       # an overlay
  tu_cluster.py --min-cluster 2 --max-shared 8
"""
import argparse
import bisect
import json
import re
import subprocess
import sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


def load_match(report_path):
    """name -> (best match%, home unit) from an objdiff report. Home unit is the
    unit where the function is actually scored (its source defines it)."""
    match, home = {}, {}
    p = Path(report_path)
    if not p.exists():
        return match, home
    r = json.loads(p.read_text(encoding="utf-8"))
    for u in r.get("units", []):
        for f in u.get("functions", []):
            pct = f.get("fuzzy_match_percent")
            if pct is None:
                continue
            n = f.get("name")
            if pct > match.get(n, -1):
                match[n] = pct
                home[n] = u.get("name")
    return match, home


def load_sdk_affiliation():
    """func -> SDK unit it is interleaved with (touches that TU's statics)."""
    try:
        out = subprocess.run([sys.executable, str(ROOT / "tools/scripts/is_interleaved.py"),
                              "--all", "--json"], capture_output=True, text=True).stdout
        return {h["func"]: h["unit"] for h in json.loads(out)}
    except Exception:
        return {}

SYM_FN = re.compile(r"^(?P<name>\w+)\s+kind:function\(.*?size=0x(?P<size>[0-9a-fA-F]+).*?\)\s+addr:0x(?P<addr>[0-9a-fA-F]+)")
SYM_ANY = re.compile(r"^(?P<name>\w+)\s+kind:(?P<kind>\w+).*?addr:0x(?P<addr>[0-9a-fA-F]+)")
DELINK_CODE = re.compile(r"start:0x(?P<s>[0-9a-fA-F]+)\s+end:0x(?P<e>[0-9a-fA-F]+)\s+kind:code")
RELOC = re.compile(r"from:0x([0-9a-fA-F]+)\b.*?\bto:0x([0-9a-fA-F]+)\b")


def module_paths(mod):
    if mod == "main":
        d = ROOT / "config" / "arm9"
    else:
        d = ROOT / "config" / "arm9" / "overlays" / mod
    return d / "symbols.txt", d / "relocs.txt", d / "delinks.txt"


def parse(mod):
    sym_p, rel_p, del_p = module_paths(mod)
    for p in (sym_p, rel_p):
        if not p.exists():
            sys.exit(f"error: {p} not found")
    code_ranges = []
    if del_p.exists():
        for line in del_p.read_text(encoding="utf-8").splitlines():
            m = DELINK_CODE.search(line)
            if m:
                code_ranges.append((int(m.group("s"), 16), int(m.group("e"), 16)))

    def in_code(a):
        return any(s <= a < e for s, e in code_ranges) if code_ranges else True

    funcs = {}          # name -> (addr, size)
    addr2sym = {}       # addr -> (name, kind)
    for line in sym_p.read_text(encoding="utf-8").splitlines():
        a = SYM_ANY.match(line)
        if a:
            addr2sym[int(a.group("addr"), 16)] = (a.group("name"), a.group("kind"))
        m = SYM_FN.match(line)
        if m:
            funcs[m.group("name")] = (int(m.group("addr"), 16), int(m.group("size"), 16))

    fr = []
    for line in rel_p.read_text(encoding="utf-8").splitlines():
        m = RELOC.search(line)
        if m:
            fr.append((int(m.group(1), 16), int(m.group(2), 16)))
    fr.sort()
    return funcs, addr2sym, fr, in_code


def resolve_data(addr2sym, sorted_addrs, to):
    """Return (name, kind) of the symbol containing `to`, or None."""
    i = bisect.bisect_right(sorted_addrs, to) - 1
    if i < 0:
        return None
    name, kind = addr2sym[sorted_addrs[i]]
    return name, kind


class UF:
    def __init__(self):
        self.p = {}

    def find(self, x):
        self.p.setdefault(x, x)
        while self.p[x] != x:
            self.p[x] = self.p[self.p[x]]
            x = self.p[x]
        return x

    def union(self, a, b):
        self.p[self.find(a)] = self.find(b)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--module", default="main")
    ap.add_argument("--calls", action="store_true", help="also add call-graph edges")
    ap.add_argument("--min-shared", type=int, default=2, help="min referrers for data to be a grouping signal")
    ap.add_argument("--max-shared", type=int, default=6, help="max referrers (above this = global, ignored)")
    ap.add_argument("--max-span", type=lambda x: int(x, 0), default=0x4000,
                    help="ignore data whose referrers span more than this (bytes) -- a global, not TU-private")
    ap.add_argument("--min-cluster", type=int, default=2, help="min functions to print a cluster")
    ap.add_argument("--report", default=str(ROOT / "build" / "report.json"),
                    help="objdiff report json for real match status")
    ap.add_argument("--only-open", action="store_true", help="hide fully-matched clusters")
    args = ap.parse_args()

    funcs, addr2sym, fr, in_code = parse(args.module)
    sorted_addrs = sorted(addr2sym)
    froms = [f for f, _ in fr]

    # function ranges (only code-section functions of this module)
    franges = sorted((a, a + s, n) for n, (a, s) in funcs.items() if in_code(a))
    fstarts = [r[0] for r in franges]

    def enclosing_fn(a):
        i = bisect.bisect_right(fstarts, a) - 1
        return franges[i][2] if (i >= 0 and a < franges[i][1]) else None

    fn_data = defaultdict(set)      # fn -> {data sym}
    data_fns = defaultdict(set)     # data sym -> {fn}
    fn_calls = defaultdict(set)     # fn -> {callee fn}
    for start, end, name in franges:
        lo = bisect.bisect_left(froms, start)
        hi = bisect.bisect_left(froms, end)
        for _f, to in fr[lo:hi]:
            r = resolve_data(addr2sym, sorted_addrs, to)
            if not r:
                continue
            tname, tkind = r
            if tkind == "function":
                callee = enclosing_fn(to)
                if callee and callee != name:
                    fn_calls[name].add(callee)
            else:                                   # data/bss/label -> a data reference
                fn_data[name].add(tname)
                data_fns[tname].add(name)

    uf = UF()
    for _, _, n in franges:
        uf.find(n)
    # --- grouping edges ---
    grouping_data = {}                              # data -> referrers (narrow + address-bounded)
    for d, fns in data_fns.items():
        if not (args.min_shared <= len(fns) <= args.max_shared):
            continue
        addrs = [funcs[f][0] for f in fns]
        if max(addrs) - min(addrs) > args.max_span: # spread too wide -> a global, not a TU static
            continue
        grouping_data[d] = fns
        fns = sorted(fns)
        for f in fns[1:]:
            uf.union(fns[0], f)
    if args.calls:
        for f, callees in fn_calls.items():
            for c in callees:
                uf.union(f, c)

    comps = defaultdict(list)
    for _, _, n in franges:
        comps[uf.find(n)].append(n)

    match, home = load_match(args.report)
    affil = load_sdk_affiliation()

    def done(m):
        return match.get(m, 0) >= 100.0

    # build printable clusters
    clusters = []
    for members in comps.values():
        if len(members) < args.min_cluster:
            continue
        members = sorted(members, key=lambda n: funcs[n][0])
        opent = [m for m in members if not done(m)]
        if args.only_open and not opent:
            continue
        shared = sorted({d for m in members for d in fn_data[m] if d in grouping_data},
                        key=lambda d: -len(grouping_data[d]))
        sdk = sorted({affil[m] for m in members if m in affil})
        clusters.append((funcs[members[0]][0], members, opent, shared, sdk))

    clusters.sort()
    print(f"=== module '{args.module}': {len(clusters)} candidate TU(s) "
          f"(mode: {'calls+data' if args.calls else 'data-only'}, "
          f"shared-window {args.min_shared}-{args.max_shared}, span<=0x{args.max_span:x}) ===")
    print("    legend: [x]=matched 100%  [~]=partial  [ ]=todo   |homes shown for matched|\n")
    for addr0, members, opent, shared, sdk in clusters:
        tot = sum(funcs[m][1] for m in members)
        ndone = len(members) - len(opent)
        sdk_s = f"   SDK-affiliated: {', '.join(sdk)}" if sdk else ""
        print(f"@0x{addr0:08x}  {len(members)} fn ({ndone} done / {len(opent)} todo), "
              f"0x{tot:x} bytes   shared: {', '.join(shared[:4]) if shared else '(call-graph only)'}{sdk_s}")
        # collect distinct home units of matched members (consolidation target)
        homes = sorted({home[m] for m in members if done(m) and m in home})
        for m in members:
            a, s = funcs[m]
            pct = match.get(m)
            mk = "[x]" if (pct is not None and pct >= 100) else ("[~]" if pct else "[ ]")
            hm = f"  @{home.get(m)}" if done(m) else ""
            af = "  *SDK*" if m in affil else ""
            dl = sorted(d for d in fn_data[m] if d in grouping_data)
            print(f"    {mk} 0x{a:08x} {m:28} 0x{s:<4x}{hm}{af}"
                  f"{('  -> ' + ', '.join(dl)) if dl else ''}")
        if len(homes) > 1:
            print(f"      ^ {len(homes)} matched members live in SEPARATE files -> consolidate into one TU")
        print()
    nfn = sum(len(c[1]) for c in clusters)
    ntodo = sum(len(c[2]) for c in clusters)
    print(f"{len(clusters)} clusters, {nfn} functions; {nfn - ntodo} matched, {ntodo} todo.")


if __name__ == "__main__":
    main()