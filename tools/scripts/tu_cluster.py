#!/usr/bin/env python3
import argparse
import bisect
import re
from collections import defaultdict
from pathlib import Path

import is_interleaved
from common import BUILD, CONFIG, die, load_json
from is_interleaved import read_relocs, word_symbols

DELINK_CODE = re.compile(r"start:0x(?P<s>[0-9a-fA-F]+)\s+end:0x(?P<e>[0-9a-fA-F]+)\s+kind:code")


def load_match(report_path):
    """({name: best match%}, {name: unit scoring that best})"""
    match, home = {}, {}
    p = Path(report_path)
    if not p.exists():
        return match, home
    r = load_json(p)
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


def parse(mod):
    d = CONFIG / ("" if mod == "main" else f"overlays/{mod}")
    sym_p, rel_p, del_p = d / "symbols.txt", d / "relocs.txt", d / "delinks.txt"
    for p in (sym_p, rel_p):
        if not p.exists():
            die(f"error: {p} not found", 1)
    code_ranges = []
    if del_p.exists():
        for line in del_p.read_text(encoding="utf-8").splitlines():
            m = DELINK_CODE.search(line)
            if m:
                code_ranges.append((int(m.group("s"), 16), int(m.group("e"), 16)))

    def in_code(a):
        return any(s <= a < e for s, e in code_ranges) if code_ranges else True

    funcs = {}
    addr2sym = {}
    for s in word_symbols(sym_p):
        addr2sym[s.addr] = (s.name, s.kind)
        if s.kind == "function" and s.size is not None:
            funcs[s.name] = (s.addr, s.size)
    return funcs, addr2sym, read_relocs(rel_p), in_code


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
    ap = argparse.ArgumentParser()
    ap.add_argument("--module", default="main")
    ap.add_argument("--calls", action="store_true", help="also add call-graph edges")
    ap.add_argument("--min-shared", type=int, default=2, help="min referrers for data to be a grouping signal")
    ap.add_argument("--max-shared", type=int, default=6, help="max referrers (above this = global, ignored)")
    ap.add_argument("--max-span", type=lambda x: int(x, 0), default=0x4000,
                    help="ignore data whose referrers span more than this (bytes) -- a global, not TU-private")
    ap.add_argument("--min-cluster", type=int, default=2, help="min functions to print a cluster")
    ap.add_argument("--report", default=str(BUILD / "report.json"),
                    help="objdiff report json for real match status")
    ap.add_argument("--only-open", action="store_true", help="hide fully-matched clusters")
    args = ap.parse_args()

    funcs, addr2sym, fr, in_code = parse(args.module)
    sorted_addrs = sorted(addr2sym)
    froms = [f for f, _ in fr]

    franges = sorted((a, a + s, n) for n, (a, s) in funcs.items() if in_code(a))
    fstarts = [r[0] for r in franges]

    def enclosing_fn(a):
        i = bisect.bisect_right(fstarts, a) - 1
        return franges[i][2] if (i >= 0 and a < franges[i][1]) else None

    fn_data = defaultdict(set)
    data_fns = defaultdict(set)
    fn_calls = defaultdict(set)
    for start, end, name in franges:
        lo = bisect.bisect_left(froms, start)
        hi = bisect.bisect_left(froms, end)
        for _f, to in fr[lo:hi]:
            i = bisect.bisect_right(sorted_addrs, to) - 1
            if i < 0:
                continue
            tname, tkind = addr2sym[sorted_addrs[i]]
            if tkind == "function":
                callee = enclosing_fn(to)
                if callee and callee != name:
                    fn_calls[name].add(callee)
            else:
                fn_data[name].add(tname)
                data_fns[tname].add(name)

    uf = UF()
    for _, _, n in franges:
        uf.find(n)
    grouping_data = {}
    for d, fns in data_fns.items():
        if not (args.min_shared <= len(fns) <= args.max_shared):
            continue
        addrs = [funcs[f][0] for f in fns]
        if max(addrs) - min(addrs) > args.max_span:
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
    affil = is_interleaved.affiliation()

    def done(m):
        return match.get(m, 0) >= 100.0

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
