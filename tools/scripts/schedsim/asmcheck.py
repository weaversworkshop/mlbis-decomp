import sys, os, re, subprocess, collections
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
import cc
ROOT = cc.ROOT
sys.path.insert(0, str(ROOT / "tools/scripts"))
import fn_diff as fd
import asmsim as S
OUT = ROOT / "build/schedsim/asm"
POOL_RE = re.compile(r"\[pc, #(-?\d+)\]")


def compile_tu(tu, variant, src=None):
    tu = Path(tu).resolve()
    obj = OUT / variant / (tu.stem + ".o")
    s_ = Path(src) if src else tu
    if obj.exists() and obj.stat().st_mtime > s_.stat().st_mtime:
        return obj
    return cc.compile(tu, variant, obj, src)


def functions(obj):
    """{symbol: [(text, key)]} with text parseable by armparse and key position-independent"""
    lines = fd.objdump(obj, True)
    syms = []
    for ln in lines:
        m = fd.SYM_RE.match(ln)
        if m and not m.group(1).startswith((".L", "$")):
            syms.append(m.group(1))
    out = {}
    for sym in dict.fromkeys(syms):
        items, found = fd.extract_items(lines, sym)
        code = [(a, t, tg) for a, t, isd, tg in items if not isd]
        if not code:
            continue
        index = {a: k for k, (a, _, _) in enumerate(code)}
        res = []
        for k, (a, t, tg) in enumerate(code):
            text, key = t, t
            bm = fd.BRANCH_RE.search(t)
            if bm:
                if tg is not None and tg in index:
                    text = "%s <T%+d>" % (bm.group(1), index[tg] - k)
                    key = "%s L%d" % (bm.group(1), index[tg])
                else:
                    text = key = "%s <T>" % bm.group(1) if bm.group(1) not in ("bx", "blx") or not re.search(r"\b(bx|blx)\s+(r\d+|lr|ip)", t) else t
                    if bm.group(1) in ("bl", "blx") and "<T>" in key:
                        key = "%s @%d" % (bm.group(1), k)  # calls: reloc target unknown here; position-keyed
            pm = POOL_RE.search(t)
            if pm:
                key = POOL_RE.sub("=P%x" % (a + 8 + int(pm.group(1))), t)
            res.append((text.replace("\t", " "), key.replace("\t", " ")))
        out[sym] = res
    return out


def split_blocks(fn):
    """index ranges of schedulable blocks"""
    n = len(fn)
    starts = set([0])
    excl = set()
    for k, (t, key) in enumerate(fn):
        if S.EPI_RE.match(t):
            excl.add(k)
            starts.add(k); starts.add(k + 1)
            continue
        m = re.match(r"^(b\w*)\s+<T([+-]\d+)>", t)
        if m and not t.startswith("bl "):
            starts.add(k + 1)
            starts.add(k + int(m.group(2)))
        elif re.match(r"^(bx|b)\w*\s", t) and not re.match(r"^(bl|blx|bic)\w*\s", t):
            starts.add(k + 1)
    st = sorted(x for x in starts if 0 <= x <= n)
    return [(a, b) for a, b in zip(st, st[1:] + [n]) if b > a and a not in excl]


def validate(tu, vin, vout, o, verbose=0, only=None):
    fa = functions(compile_tu(tu, vin))
    fb = functions(compile_tu(tu, vout))
    stats = collections.Counter()
    bad = []
    for sym, A_ in fa.items():
        if only and sym != only:
            continue
        B_ = fb.get(sym)
        if B_ is None or len(B_) != len(A_):
            stats["fn_incomparable"] += 1
            continue
        S.frame_info([t for t, _ in A_], o)
        for a, b in split_blocks(A_):
            ka = [k for _, k in A_[a:b]]
            kb = [k for _, k in B_[a:b]]
            if b - a < 2:
                stats["tiny"] += 1
                continue
            if sorted(ka) != sorted(kb):
                stats["blk_incomparable"] += 1
                continue
            try:
                pred = S.sim_block([t for t, _ in A_[a:b]], ka, o)
            except Exception as e:
                stats["sim_error"] += 1
                bad.append((sym, a, b, "ERR %r" % e))
                continue
            if pred == kb:
                stats["ok_moved" if ka != kb else "ok_same"] += 1
            else:
                stats["FAIL_moved" if ka != kb else "FAIL_same"] += 1
                bad.append((sym, a, b, None))
    return stats, bad, fa, fb


if __name__ == "__main__":
    import argparse
    ap = argparse.ArgumentParser()
    ap.add_argument("tus", nargs="+")
    ap.add_argument("--vin", default="nosched"); ap.add_argument("--vout", default="p1off")
    ap.add_argument("--wslots", default="", help="comma list of sp offsets of narrow spilled locals")
    ap.add_argument("-v", type=int, default=0); ap.add_argument("--only")
    ap.add_argument("--set", action="append", default=[])
    a = ap.parse_args()
    o = S.Opts()
    o.wslots = set(int(x) for x in a.wslots.split(",") if x)
    for kv in a.set:
        k, v = kv.split("=")
        setattr(o, k, int(v))
    tot = collections.Counter()
    for tu in a.tus:
        try:
            st, bad, fa, fb = validate(tu, a.vin, a.vout, o, a.v, a.only)
        except RuntimeError as e:
            print("SKIP", tu, str(e)[:120]); continue
        tot += st
        print(Path(tu).name, dict(st))
        for sym, x, y, err in bad[:a.v]:
            print("  FAIL", sym, x, y, err or "")
            if a.v > 1 and not err:
                ka = [k for _, k in fa[sym][x:y]]; kb = [k for _, k in fb[sym][x:y]]
                S.frame_info([t for t, _ in fa[sym]], o)
                tr = []
                pred = S.sim_block([t for t, _ in fa[sym][x:y]], ka, o, tr)
                for i in range(len(ka)):
                    print("   %-34s | %-34s | %-34s %s" % (ka[i][:34], kb[i][:34], pred[i][:34], "" if pred[i] == kb[i] else "<<"))
    print("TOTAL", dict(tot))
