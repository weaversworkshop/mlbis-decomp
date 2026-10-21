#!/usr/bin/env python3
import argparse
import re
import sys
from pathlib import Path

import tu_diff

ADDEND_RE = re.compile(r"([+-]0x[0-9a-fA-F]+|[+-]\d+)$")


def strip_addend(sym):
    m = ADDEND_RE.search(sym)
    if m:
        return sym[: m.start()], m.group(1)
    return sym, ""


def call_targets(rec):
    """('sym', name|None) for external calls, ('local', rel_offset) for intra-function branches."""
    start, end = rec["start"], rec["end"]
    out = []
    for addr, _raw, text, reloc in rec["insns"]:
        parts = text.split(None, 1)
        if not parts or not tu_diff.BRANCH_RE.match(parts[0]):
            continue
        if reloc:
            out.append(("sym", strip_addend(reloc)[0]))
            continue
        m = re.match(r"^([0-9a-f]+)\b", parts[1] if len(parts) > 1 else "")
        if m and start <= int(m.group(1), 16) < end:
            out.append(("local", int(m.group(1), 16) - addr))
        else:
            out.append(("sym", None))
    return out


def skeleton(rec):
    return tu_diff.normalize(rec["start"], rec["end"], rec["insns"])


class Registry:
    def __init__(self, gap_o):
        self.objs = {}
        self.sym_obj = {}
        self._load(gap_o)
        self.sdk_os = tu_diff.sdk_objects()
        for o in self.sdk_os:
            self._load(o)

    def _load(self, objfile):
        key = str(objfile)
        if key in self.objs:
            return
        funcs = tu_diff.disasm(objfile)
        by_name = {name: dict(section=section, start=start, end=end, insns=insns)
                   for name, section, start, end, insns in funcs}
        self.objs[key] = (by_name, tu_diff.build_word_map(funcs))
        for name in by_name:
            self.sym_obj.setdefault(name, key)

    def get(self, name):
        key = self.sym_obj.get(name)
        if key is None:
            return None, None
        by_name, wmap = self.objs[key]
        return by_name[name], wmap


def compare(reg, gname, sname, recurse):
    """None if equal, else the reason. `recurse(g, s)` -> (ok, why) for differing callees."""
    grec, gw = reg.get(gname)
    srec, sw = reg.get(sname)
    if grec is None or srec is None:
        return (f"name differs and no body to compare "
                f"({'gap' if grec is None else 'sdk'} missing)")

    gk, sk = skeleton(grec), skeleton(srec)
    if gk != sk:
        i = next((j for j in range(min(len(gk), len(sk))) if gk[j] != sk[j]),
                 min(len(gk), len(sk)))
        ctx = ""
        if i < len(gk) and i < len(sk):
            ctx = f": insn {i} `{gk[i]}` vs `{sk[i]}`"
        elif len(gk) != len(sk):
            ctx = f": length {len(gk)} vs {len(sk)}"
        return f"instruction skeleton differs{ctx}"

    gp = tu_diff.pool_refs(grec["insns"], grec["section"], gw)
    sp = tu_diff.pool_refs(srec["insns"], srec["section"], sw)
    if len(gp) != len(sp):
        return f"pool count {len(gp)} vs {len(sp)}"
    for (gk2, gv), (sk2, sv) in zip(gp, sp):
        if gk2 == "abs" and sk2 == "abs":
            if gv != sv:
                gh = f"{gv:#x}" if isinstance(gv, int) else repr(gv)
                sh = f"{sv:#x}" if isinstance(sv, int) else repr(sv)
                return f".word const {gh} vs {sh}"
        elif gk2 == "rel" and sk2 == "rel":
            gs, ss = strip_addend(str(gv)), strip_addend(str(sv))
            if gs[1] != ss[1]:
                return f".word addend {gs[1]!r} vs {ss[1]!r}"
            if gs[0] != ss[0] and (reg.get(gs[0])[0] and reg.get(ss[0])[0]):
                ok, why = recurse(gs[0], ss[0])
                if not ok:
                    return f".word -> {gs[0]} vs {ss[0]}: {why}"
        elif not ((gk2 == "abs" and gv is None) or (sk2 == "abs" and sv is None)):
            # a None abs is a collapsed zero-filled reloc'd word: an unnamed symbol ref
            return f".word kind {gk2} vs {sk2}"

    gc, sc = call_targets(grec), call_targets(srec)
    if len(gc) != len(sc):
        return f"call count {len(gc)} vs {len(sc)}"
    for idx, ((gk3, gn), (sk3, sn)) in enumerate(zip(gc, sc)):
        if gk3 == "local" and sk3 == "local":
            if gn != sn:
                return f"local branch offset {gn} vs {sn}"
            continue
        if gn == sn:
            continue
        if gn is None or sn is None:
            return f"call #{idx}: unknown target {gn} vs {sn}"
        ok, why = recurse(gn, sn, idx)
        if not ok:
            return why
    return None


def equiv(reg, gname, sname, memo, depth, maxdepth):
    if gname == sname:
        return True, "same symbol"
    pair = (gname, sname)
    if pair in memo:
        return memo[pair]
    memo[pair] = (True, "assumed (recursion)")

    def recurse(g, s, idx=None):
        if idx is None:
            return equiv(reg, g, s, memo, depth + 1, maxdepth)
        if depth >= maxdepth:
            return False, f"call #{idx} {g} vs {s}: max depth {maxdepth}"
        ok, why = equiv(reg, g, s, memo, depth + 1, maxdepth)
        return ok, f"call #{idx} `{g}` != `{s}` -> {why}"

    why = compare(reg, gname, sname, recurse)
    memo[pair] = (True, "bodies, pool, and full call graph match") if why is None else (False, why)
    return memo[pair]


def report(reg, gname, sname, maxdepth, verbose):
    memo = {}
    ok, why = equiv(reg, gname, sname, memo, 0, maxdepth)
    verdict = "EQUIVALENT" if ok else "DIVERGES"
    print(f"  [{verdict:10}] {gname}  ==  {sname}")
    print(f"               {why}")
    if verbose:
        for (g, s), (rok, rwhy) in sorted(memo.items()):
            if g == s or "assumed" in rwhy or (g, s) == (gname, sname):
                continue
            mark = "ok " if rok else "DIFF"
            print(f"                 - [{mark}] {g} vs {s}: {rwhy}")
    return ok


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("funcs", nargs="+", help="gap function name(s) to check")
    ap.add_argument("--gap-o", default=str(tu_diff.GAP_DEFAULT))
    ap.add_argument("--sdk-func", help="force comparison vs this SDK symbol")
    ap.add_argument("--depth", type=int, default=8, help="max recursion depth")
    ap.add_argument("--min", type=int, default=6,
                    help="skip candidate SDK funcs with fewer insns (noise guard)")
    ap.add_argument("-v", "--verbose", action="store_true",
                    help="show each recursive sub-decision")
    args = ap.parse_args()

    print("indexing gap + SDK objects ...", file=sys.stderr)
    reg = Registry(Path(args.gap_o))

    for gname in args.funcs:
        grec, _ = reg.get(gname)
        if grec is None:
            print(f"\n## {gname}: not found in {args.gap_o}")
            continue
        gk = skeleton(grec)
        print(f"\n## {gname}  ({len(grec['insns'])} insns)")

        if args.sdk_func:
            cands = [args.sdk_func]
        elif len(gk) < args.min:
            print(f"  skeleton too short (<{args.min} tokens) -> ambiguous, "
                  "pass --sdk-func to force a specific comparison.")
            continue
        else:
            cands = sorted({n for o in reg.sdk_os
                            for n, r in reg.objs[str(o)][0].items() if skeleton(r) == gk})
        if not cands:
            print("  no SDK function shares this instruction skeleton "
                  "-> original (non-SDK) code.")
            continue
        eqs = [s for s in cands if report(reg, gname, s, args.depth, args.verbose)]
        if eqs:
            print(f"  => EQUIVALENT to SDK: {', '.join(eqs)}")
        else:
            print(f"  => NOT equivalent to any of {len(cands)} skeleton-match"
                  f"{'es' if len(cands) != 1 else ''} -> game/forked code.")


if __name__ == "__main__":
    main()
