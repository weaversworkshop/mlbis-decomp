#!/usr/bin/env python3
"""Decide whether a gap func_XXXX is genuinely an SDK function: matches the
instruction skeleton + literal pool + reloc symbols and RECURSES into call
targets (equivalent only if the whole call graph is stock SDK).
Usage: sdk_equiv.py FUNC... [--sdk-func NAME] [--depth N] [--min N] [-v].
Prefer `make sdk-check FUNC=...`."""
import argparse
import re
import sys
from pathlib import Path

import tu_diff  # same directory; reuse disasm/normalize/pool helpers

ROOT = tu_diff.ROOT
GAP_DEFAULT = tu_diff.GAP_DEFAULT
BRANCH_RE = tu_diff.BRANCH_RE

ADDEND_RE = re.compile(r"([+-]0x[0-9a-fA-F]+|[+-]\d+)$")


def strip_addend(sym):
    """`DisposeCallback-0x8` -> (`DisposeCallback`, '-0x8')."""
    m = ADDEND_RE.search(sym)
    if m:
        return sym[: m.start()], m.group(1)
    return sym, ""


def load_object(objfile):
    """name -> dict(section,start,end,insns); plus the object's word map."""
    funcs = tu_diff.disasm(objfile)
    wmap = tu_diff.build_word_map(funcs)
    by_name = {}
    for name, section, start, end, insns in funcs:
        by_name[name] = dict(
            objfile=str(objfile), section=section, start=start, end=end, insns=insns
        )
    return by_name, wmap


def call_targets(rec):
    """Ordered list of branch targets: ('sym', name) for relocated/external
    calls, ('local', rel_offset) for intra-function branches."""
    start, end, insns = rec["start"], rec["end"], rec["insns"]
    out = []
    for addr, _raw, text, reloc in insns:
        parts = text.split(None, 1)
        if not parts:
            continue
        mn = parts[0]
        if not BRANCH_RE.match(mn):
            continue
        if reloc:
            out.append(("sym", strip_addend(reloc)[0]))
        else:
            ops = parts[1] if len(parts) > 1 else ""
            m = re.match(r"^([0-9a-f]+)\b", ops)
            if m:
                tgt = int(m.group(1), 16)
                if start <= tgt < end:
                    out.append(("local", tgt - addr))
                else:
                    out.append(("sym", None))  # external, name unknown
            else:
                out.append(("sym", None))
    return out


def pool_seq(rec, wmap):
    return tu_diff.pool_refs(rec["insns"], rec["section"], wmap)


class Registry:
    """Lazily indexes gap + all built SDK objects: symbol name -> record."""

    def __init__(self, gap_o):
        self.objs = {}  # objfile -> (by_name, wmap)
        self.sym_obj = {}  # symbol -> objfile (first wins)
        self.gap_o = gap_o
        self._load(gap_o)
        self.sdk_os = sorted((ROOT / "build" / "lib").rglob("*.o"))
        for o in self.sdk_os:
            self._load(o)
        self.sym_addrs = tu_diff.load_symbol_addrs()

    def _load(self, objfile):
        key = str(objfile)
        if key in self.objs:
            return
        by_name, wmap = load_object(objfile)
        self.objs[key] = (by_name, wmap)
        for name in by_name:
            self.sym_obj.setdefault(name, key)

    def get(self, name):
        key = self.sym_obj.get(name)
        if key is None:
            return None, None
        by_name, wmap = self.objs[key]
        return by_name[name], wmap

    def skeleton(self, name):
        rec, _ = self.get(name)
        if rec is None:
            return None
        return tu_diff.normalize(name, rec["start"], rec["end"], rec["insns"])


def fmt(kind, name):
    return name if kind == "sym" and name else f"<{kind}>"


def equiv(reg, gname, sname, memo, trail, depth, maxdepth):
    """Return (bool, reason). Recurses through differing call targets."""
    pair = (gname, sname)
    if gname == sname:
        return True, "same symbol"
    if pair in memo:
        return memo[pair]  # cycle / already decided
    memo[pair] = (True, "assumed (recursion)")  # optimistic to break cycles

    grec, gw = reg.get(gname)
    srec, sw = reg.get(sname)
    if grec is None or srec is None:
        # one side has no body we can inspect -> can't prove; names already
        # differ, and an established mapping would have set gname==sname.
        res = (False, f"name differs and no body to compare "
                      f"({'gap' if grec is None else 'sdk'} missing)")
        memo[pair] = res
        return res

    gk = tu_diff.normalize(gname, grec["start"], grec["end"], grec["insns"])
    sk = tu_diff.normalize(sname, srec["start"], srec["end"], srec["insns"])
    if gk != sk:
        i = next((j for j in range(min(len(gk), len(sk))) if gk[j] != sk[j]),
                 min(len(gk), len(sk)))
        ctx = ""
        if i < len(gk) and i < len(sk):
            ctx = f": insn {i} `{gk[i]}` vs `{sk[i]}`"
        elif len(gk) != len(sk):
            ctx = f": length {len(gk)} vs {len(sk)}"
        res = (False, f"instruction skeleton differs{ctx}")
        memo[pair] = res
        return res

    # skeleton equal -> compare literal pool / .word data
    gp, sp = pool_seq(grec, gw), pool_seq(srec, sw)
    if len(gp) != len(sp):
        res = (False, f"pool count {len(gp)} vs {len(sp)}")
        memo[pair] = res
        return res
    for (gk2, gv), (sk2, sv) in zip(gp, sp):
        if gk2 == "abs" and sk2 == "abs":
            if gv != sv:
                gh = f"{gv:#x}" if isinstance(gv, int) else repr(gv)
                sh = f"{sv:#x}" if isinstance(sv, int) else repr(sv)
                res = (False, f".word const {gh} vs {sh}")
                memo[pair] = res
                return res
        elif gk2 == "rel" and sk2 == "rel":
            gs = strip_addend(str(gv)); ss = strip_addend(str(sv))
            if gs[1] != ss[1]:
                res = (False, f".word addend {gs[1]!r} vs {ss[1]!r}")
                memo[pair] = res
                return res
            # rel symbol names may legitimately differ (renamed same data);
            # recurse only if both name functions, else note as data ref.
            if gs[0] != ss[0] and (reg.get(gs[0])[0] and reg.get(ss[0])[0]):
                ok, why = equiv(reg, gs[0], ss[0], memo,
                                trail + [(gname, sname)], depth + 1, maxdepth)
                if not ok:
                    res = (False, f".word -> {gs[0]} vs {ss[0]}: {why}")
                    memo[pair] = res
                    return res
        else:
            res = (False, f".word kind {gk2} vs {sk2}")
            memo[pair] = res
            return res

    # compare call targets, recursing where names differ
    gc, sc = call_targets(grec), call_targets(srec)
    if len(gc) != len(sc):
        res = (False, f"call count {len(gc)} vs {len(sc)}")
        memo[pair] = res
        return res
    for idx, ((gk3, gn), (sk3, sn)) in enumerate(zip(gc, sc)):
        if gk3 == "local" and sk3 == "local":
            if gn != sn:
                res = (False, f"local branch offset {gn} vs {sn}")
                memo[pair] = res
                return res
            continue
        if gn == sn:
            continue
        if gn is None or sn is None:
            res = (False, f"call #{idx}: unknown target {gn} vs {sn}")
            memo[pair] = res
            return res
        if depth >= maxdepth:
            res = (False, f"call #{idx} {gn} vs {sn}: max depth {maxdepth}")
            memo[pair] = res
            return res
        ok, why = equiv(reg, gn, sn, memo, trail + [(gname, sname)],
                        depth + 1, maxdepth)
        if not ok:
            res = (False, f"call #{idx} `{gn}` != `{sn}` -> {why}")
            memo[pair] = res
            return res

    memo[pair] = (True, "bodies, pool, and full call graph match")
    return memo[pair]


def report(reg, gname, sname, maxdepth, verbose):
    memo = {}
    trail_lines = []
    ok, why = equiv(reg, gname, sname, memo, [], 0, maxdepth)
    verdict = "EQUIVALENT" if ok else "DIVERGES"
    print(f"  [{verdict:10}] {gname}  ==  {sname}")
    print(f"               {why}")
    if verbose:
        recursed = [(g, s, r) for (g, s), r in sorted(memo.items())
                    if g != s and "assumed" not in r[1]]
        for g, s, (rok, rwhy) in recursed:
            if (g, s) == (gname, sname):
                continue
            mark = "ok " if rok else "DIFF"
            print(f"                 - [{mark}] {g} vs {s}: {rwhy}")
    return ok


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("funcs", nargs="+", help="gap function name(s) to check")
    ap.add_argument("--gap-o", default=str(GAP_DEFAULT))
    ap.add_argument("--sdk-func", help="force comparison vs this SDK symbol")
    ap.add_argument("--depth", type=int, default=8, help="max recursion depth")
    ap.add_argument("--min", type=int, default=6,
                    help="skip candidate SDK funcs with fewer insns (noise guard)")
    ap.add_argument("-v", "--verbose", action="store_true",
                    help="show each recursive sub-decision")
    args = ap.parse_args()

    print("indexing gap + SDK objects ...", file=sys.stderr)
    reg = Registry(Path(args.gap_o))

    any_eq = False
    for gname in args.funcs:
        grec, _ = reg.get(gname)
        if grec is None:
            print(f"\n## {gname}: not found in {args.gap_o}")
            continue
        gk = reg.skeleton(gname)
        print(f"\n## {gname}  ({len(grec['insns'])} insns)")

        if args.sdk_func:
            cands = [args.sdk_func]
        elif len(gk) < args.min:
            print(f"  skeleton too short (<{args.min} tokens) -> ambiguous, "
                  "pass --sdk-func to force a specific comparison.")
            continue
        else:
            cands = set()
            for o in reg.sdk_os:
                by_name, _ = reg.objs[str(o)]
                for n, r in by_name.items():
                    if tu_diff.normalize(n, r["start"], r["end"], r["insns"]) == gk:
                        cands.add(n)
            cands = sorted(cands)
        if not cands:
            print("  no SDK function shares this instruction skeleton "
                  "-> original (non-SDK) code.")
            continue
        eqs = []
        for sname in cands:
            if report(reg, gname, sname, args.depth, args.verbose):
                any_eq = True
                eqs.append(sname)
        if eqs:
            print(f"  => EQUIVALENT to SDK: {', '.join(eqs)}")
        else:
            print(f"  => NOT equivalent to any of {len(cands)} skeleton-match"
                  f"{'es' if len(cands) != 1 else ''} -> game/forked code.")
    sys.exit(0 if any_eq or True else 1)


if __name__ == "__main__":
    main()
