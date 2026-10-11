#!/usr/bin/env python3
"""Register-coloring rank diff for mwcc reg-rotation residues.

Rebuilds webs (def-use live ranges) and their interference from a function's
final asm, then infers the order mwcc's select phase must have colored them in
(pop order; each web takes the lowest free colour, claiming a new callee-saved
register only when no volatile/claimed one is free -- mwcc Coloring.c, see
notes/rank_diff.md). Diffs the forced "A colored before B" facts of the target
against the compiled build.

Usage:
  rank_diff.py <symbol> [--mine PATH] [--target PATH] [-v]
  rank_diff.py --validate [--limit N]   # model check over all matched functions
"""
import argparse
import re
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import fn_diff

ROOT = fn_diff.ROOT
OBJDUMP = fn_diff.OBJDUMP
LINE_RE = re.compile(r"^\s*([0-9a-f]+):\s*([0-9a-f]{8})\s+(\S+)\s*(.*)$")
REGN = {**{f"r{i}": i for i in range(16)}, "sl": 10, "fp": 11, "ip": 12, "sp": 13, "lr": 14, "pc": 15}
RNAME = {i: n for n, i in REGN.items() if n[0] != "r" or i < 10}
CONDS = "eq|ne|cs|cc|mi|pl|vs|vc|hi|ls|ge|lt|gt|le|hs|lo|al"
VOLATILE = [0, 1, 2, 3, 12, 14]
CALLEE = list(range(4, 12))
ALLOC = set(VOLATILE) | set(CALLEE)
DP3 = "and eor sub rsb add adc sbc rsc orr bic".split()
DP2 = "mov mvn clz".split()
SHF = "lsl lsr asr ror rrx".split()
CMP = "cmp cmn tst teq".split()
MUL = "mul mla mls smulbb smulbt smultb smultt smulwb smulwt smlabb".split()
MULL = "umull smull umlal smlal".split()
LD = "ldrsb ldrsh ldrb ldrh ldrd ldr".split()
ST = "strb strh strd str".split()
LM = "ldm stm".split()
UNSUPPORTED = "mrs msr mcr mrc swp swi svc cdp ldc stc".split()
BASES = sorted(DP3 + DP2 + SHF + CMP + MUL + MULL + LD + ST + LM + ["push", "pop"] + UNSUPPORTED, key=len, reverse=True)


class Unsupported(Exception):
    pass


def regs_in(s):
    return [REGN[t] for t in re.findall(r"\b(r1[0-5]|r[0-9]|sl|fp|ip|sp|lr|pc)\b", s)]


def reglist(s):
    m = re.search(r"\{([^}]*)\}", s)
    out = []
    for part in m.group(1).split(","):
        part = part.strip()
        if "-" in part:
            a, b = (REGN[x.strip()] for x in part.split("-"))
            out += range(a, b + 1)
        elif part:
            out.append(REGN[part])
    return out


def split_mnem(m):
    m = m.split(".")[0]
    if re.fullmatch(rf"b({CONDS})?", m):
        return "b", m[1:] or None, False
    if re.fullmatch(rf"bl({CONDS})?", m):
        return "bl", m[2:] or None, False
    if re.fullmatch(rf"blx({CONDS})?", m):
        return "blx", m[3:] or None, False
    if re.fullmatch(rf"bx({CONDS})?", m):
        return "bx", m[2:] or None, False
    for b in BASES:
        if m.startswith(b):
            rest = m[len(b):]
            if b in LM:
                rest = re.sub(r"^(ia|ib|da|db|fd|ed|fa|ea)", "", rest)
            s = False
            if rest.startswith("s") and not re.fullmatch(CONDS, rest):
                s, rest = True, rest[1:]
            if rest and not re.fullmatch(CONDS, rest):
                continue
            return b, (rest or None) if rest != "al" else None, s
    raise Unsupported(m)


class Insn:
    __slots__ = ("addr", "text", "op", "cond", "defs", "uses", "kind", "target", "idx")

    def __init__(self, addr, mnem, ops):
        self.addr, self.text = addr, f"{mnem}\t{ops}".strip()
        self.op, self.cond, _ = split_mnem(mnem)
        self.defs, self.uses, self.kind, self.target = [], [], "op", None
        o = re.sub(r"\s*[@;].*$", "", ops)
        o = re.sub(r"\s*<[^>]*>\s*$", "", o).strip()
        op = self.op
        if op in ("b", "bl", "blx", "bx"):
            if op == "b":
                self.kind, self.target = "br", int(o, 16)
            elif op == "bx":
                if o != "lr":
                    raise Unsupported("bx " + o)
                self.kind = "ret"
            elif op == "blx" and re.fullmatch(r"r\d+|sl|fp|ip|lr", o):
                self.kind, self.uses = "call", [REGN[o]]
            else:
                self.kind = "call"
            return
        if op in UNSUPPORTED:
            raise Unsupported(op)
        if op == "push":
            return
        if op == "pop":
            if 15 in reglist(o):
                self.kind = "ret"
            return
        if op in LM:
            base = REGN[o.split(",")[0].strip().rstrip("!")]
            lst = reglist(o)
            if op == "ldm":
                if base == 13:
                    if 15 in lst:
                        self.kind = "ret"
                    return
                self.defs, self.uses = lst, [base]
                if 15 in lst:
                    raise Unsupported("ldm pc")
            else:
                if base == 13:
                    return
                self.uses = lst + [base]
            if "!" in o:
                self.defs = self.defs + [base]
            return
        if op in LD or op in ST:
            first, rest = o.split(",", 1)
            rd = [REGN[first.strip()]]
            if op in ("ldrd", "strd"):
                rd.append(rd[0] + 1)
            addr = regs_in(rest)
            wb = "!" in rest or re.search(r"\]\s*,", rest)
            if op in LD:
                if rd[0] == 15:
                    raise Unsupported("ldr pc")
                self.defs, self.uses = rd, addr
            else:
                self.uses = rd + addr
            if wb:
                self.defs = self.defs + [addr[0]]
            return
        rs = regs_in(o)
        if op in CMP:
            self.uses = rs
        elif op in MULL:
            self.defs, self.uses = rs[:2], rs[2:] + (rs[:2] if op.endswith("lal") else [])
        else:
            self.defs, self.uses = rs[:1], rs[1:]
        if 15 in self.defs:
            if self.text.startswith("mov") and o.replace(" ", "") == "pc,lr":
                self.defs, self.uses, self.kind = [], [], "ret"
            else:
                raise Unsupported("write pc")


def disasm(path, symbol):
    r = subprocess.run([OBJDUMP, "-d", str(path)], capture_output=True, text=True)
    out, active = [], False
    for line in r.stdout.splitlines():
        m = fn_diff.SYM_RE.match(line)
        if m:
            if m.group(1) == symbol:
                active = True
            elif active and not m.group(1).startswith(".L"):
                active = False
            continue
        if not active:
            continue
        m = LINE_RE.match(line)
        if m:
            out.append((int(m.group(1), 16), m.group(3), m.group(4)))
    return out


class Func:
    def __init__(self, rows):
        if not rows:
            raise Unsupported("empty")
        byaddr = {a: (mn, ops) for a, mn, ops in rows}
        start = rows[0][0]
        seen, order, work = set(), [], [start]
        insns = {}
        while work:
            a = work.pop()
            while a in byaddr and a not in seen:
                seen.add(a)
                ins = Insn(a, *byaddr[a])
                insns[a] = ins
                if ins.kind == "br":
                    work.append(ins.target)
                    if ins.cond is None:
                        break
                elif ins.kind == "ret" and ins.cond is None:
                    break
                a += 4
        self.ins = [insns[a] for a in sorted(insns)]
        for i, x in enumerate(self.ins):
            x.idx = i
        pos = {x.addr: i for i, x in enumerate(self.ins)}
        n = len(self.ins)
        self.succ = [[] for _ in range(n)]
        for i, x in enumerate(self.ins):
            if x.kind == "br":
                if x.target not in pos:
                    raise Unsupported("branch out")
                self.succ[i].append(pos[x.target])
                if x.cond and i + 1 < n:
                    self.succ[i].append(i + 1)
            elif x.kind == "ret":
                if x.cond and i + 1 < n:
                    self.succ[i].append(i + 1)
            elif i + 1 < n:
                self.succ[i].append(i + 1)
        self.pred = [[] for _ in range(n)]
        for i, ss in enumerate(self.succ):
            for s in ss:
                self.pred[s].append(i)
        self._call_args()
        self._webs()

    def _call_args(self):
        """Arg regs of each call: r0..r(k-1). r is an arg if written in the call's block, or if its
        reaching def is otherwise never read (mwcc computes no dead values)."""
        calls = [i for i, x in enumerate(self.ins) if x.kind == "call"]
        for i in calls:
            self.ins[i].defs = [0, 1, 2, 3, 12, 14]
        rin, _ = self._reach()
        readers = {}
        for j, y in enumerate(self.ins):
            for u in set(y.uses) | (set(y.defs) if y.cond else set()) | ({0} if y.kind == "ret" else set()):
                for d in rin[j].get(u, ()):
                    readers.setdefault(d, set()).add(j)
        for i in calls:
            x = self.ins[i]
            hi, j = -1, i - 1
            while j >= 0 and self.ins[j].kind not in ("call", "br", "ret") and len(self.pred[j + 1]) <= 1:
                for d in self.ins[j].defs:
                    if d <= 3:
                        hi = max(hi, d)
                j -= 1
            for r in range(4):
                ds = [d for d in rin[i].get(r, ()) if d[0] >= 0 and self.ins[d[0]].kind != "call"]
                if ds and all(not (readers.get(d, set()) - {i}) for d in ds):
                    hi = max(hi, r)
            x.uses = sorted(set(x.uses) | set(range(hi + 1)))

    def _reach(self):
        n = len(self.ins)
        rin, rout = [None] * n, [None] * n
        start = {r: {(-1, r)} for r in range(15)}
        work = list(range(n))
        while work:
            i = work.pop(0)
            cur = {}
            srcs = ([start] if i == 0 else []) + [rout[p] for p in self.pred[i] if rout[p] is not None]
            for src in srcs:
                for r, ds in src.items():
                    cur.setdefault(r, set()).update(ds)
            rin[i] = cur
            x = self.ins[i]
            out = {r: set(ds) for r, ds in cur.items()}
            for d in x.defs:
                if x.cond:
                    out.setdefault(d, set()).add((i, d))
                else:
                    out[d] = {(i, d)}
            if out != rout[i]:
                rout[i] = out
                for t in self.succ[i]:
                    if t not in work:
                        work.append(t)
        return rin, rout

    def _webs(self):
        n = len(self.ins)
        ENTRY = -1
        rin, rout = self._reach()
        self.rin, self.rout = rin, rout
        parent = {}

        def find(a):
            parent.setdefault(a, a)
            while parent[a] != a:
                parent[a] = parent[parent[a]]
                a = parent[a]
            return a

        def union(a, b):
            ra, rb = find(a), find(b)
            if ra != rb:
                parent[max(ra, rb)] = min(ra, rb)

        for i, x in enumerate(self.ins):
            for d in x.defs:
                find((i, d))
            uses = list(x.uses) + ([d for d in x.defs] if x.cond else [])
            if x.kind == "ret":
                uses.append(0)
            for u in uses:
                ds = list(rin[i].get(u, ()))
                for d in ds:
                    find(d)
                for d in ds[1:]:
                    union(ds[0], d)
        self.find = find
        # liveness per reg (backward)
        live_in = [set() for _ in range(n)]
        live_out = [set() for _ in range(n)]
        changed = True
        while changed:
            changed = False
            for i in range(n - 1, -1, -1):
                x = self.ins[i]
                lo = set()
                for s in self.succ[i]:
                    lo |= live_in[s]
                uses = set(x.uses) | (set(x.defs) if x.cond else set())
                if x.kind == "ret":
                    uses.add(0)
                li = (lo - (set() if x.cond else set(x.defs))) | uses
                if lo != live_out[i] or li != live_in[i]:
                    live_out[i], live_in[i], changed = lo, li, True
        self.live_out = live_out

        def web_at(i, r, out=True):
            ds = (self.rout if out else self.rin)[i].get(r)
            return find(next(iter(ds))) if ds else None

        webs = {}
        for i, x in enumerate(self.ins):
            for d in x.defs:
                w = find((i, d))
                webs.setdefault(w, {"reg": d, "defs": set(), "uses": set(), "pre": False, "xcall": False})["defs"].add(i)
            for u in set(x.uses) | ({0} if x.kind == "ret" else set()):
                w = web_at(i, u, out=False)
                if w is None:
                    continue
                webs.setdefault(w, {"reg": u, "defs": set(), "uses": set(), "pre": False, "xcall": False})["uses"].add(i)
                if x.kind in ("call", "ret") and u <= 3:
                    webs[w]["pre"] = True
        for w, info in webs.items():
            if w[0] == ENTRY or any(self.ins[d].kind == "call" for d in info["defs"]):
                info["pre"] = True
            if info["reg"] not in ALLOC:
                info["pre"] = True
        inter = {w: set() for w in webs}
        for i, x in enumerate(self.ins):
            lo = [web_at(i, r) for r in self.live_out[i] if r in ALLOC]
            lo = [w for w in lo if w in webs]
            if x.kind == "call":
                for w in lo:
                    if w[0] != i:
                        webs[w]["xcall"] = True
            for d in x.defs:
                if d not in ALLOC:
                    continue
                w = find((i, d))
                for o in lo:
                    if o != w:
                        inter[w].add(o)
                        inter[o].add(w)
        live0 = [web_at(0, r, out=False) for r in live_in_entry(self)]
        for a in live0:
            for b in live0:
                if a and b and a != b and a in webs and b in webs:
                    inter[a].add(b)
        # post-RA scheduling reorders within a block: webs present in the same block may have overlapped
        leaders = {0} | {s for i, ss in enumerate(self.succ) for s in ss if len(self.pred[s]) != 1 or self.ins[self.pred[s][0]].kind != "op" or len(ss) > 1}
        leaders |= {i + 1 for i, x in enumerate(self.ins) if x.kind != "op"}
        blk, b = [], -1
        for i in range(len(self.ins)):
            if i in leaders:
                b += 1
            blk.append(b)
        present = {}
        for i, x in enumerate(self.ins):
            ws = {web_at(i, r) for r in self.live_out[i] if r in ALLOC} | {web_at(i, r, out=False) for r in set(x.uses) if r in ALLOC}
            ws |= {find((i, d)) for d in x.defs if d in ALLOC}
            for w in ws:
                if w in webs:
                    present.setdefault(blk[i], set()).add(w)
        maybe = {w: set() for w in webs}
        for ws in present.values():
            for a in ws:
                maybe[a] |= ws - {a}
        self.webs, self.inter, self.maybe = webs, inter, maybe

    def key(self, w):
        return frozenset(self.webs[w]["defs"]) or frozenset(("use",) + tuple(sorted(self.webs[w]["uses"])))

    def desc(self, w):
        info = self.webs[w]
        i = min(info["defs"]) if info["defs"] else min(info["uses"])
        return f"#{i:<3} {self.ins[i].text}"


def live_in_entry(f):
    return [r for r in range(15) if (-1, r) in {d for ds in f.rin[0].values() for d in ds}]


def blocked_by_pre(f, w, loose=True):
    near = f.inter[w] | (f.maybe[w] if loose else set())
    b = {f.webs[o]["reg"] for o in near if f.webs[o]["pre"]} - ({f.webs[w]["reg"]} if loose else set())
    if f.webs[w]["xcall"]:
        b |= set(VOLATILE)
    return b


def ok(f, w, colored, claimed):
    c = f.webs[w]["reg"]
    blocked = blocked_by_pre(f, w) | ({f.webs[o]["reg"] for o in f.inter[w] | f.maybe[w] if o in colored} - {c})
    avail = sorted((set(VOLATILE) | claimed) - blocked)
    if avail:
        return avail[0] == c
    free = [r for r in CALLEE if r not in claimed]
    return bool(free) and free[0] == c


def find_order(f, budget=200000):
    """DFS for a pop order reproducing every colour. None if none found within budget."""
    nodes = [w for w in f.webs if not f.webs[w]["pre"]]
    steps = [0]

    def rec(colored, claimed, order):
        if len(order) == len(nodes):
            return order
        steps[0] += 1
        if steps[0] > budget:
            return None
        cands = [w for w in nodes if w not in colored and ok(f, w, colored, claimed)]
        cands.sort(key=lambda w: (f.webs[w]["reg"] in CALLEE and f.webs[w]["reg"] not in claimed, f.webs[w]["reg"]))
        for w in cands:
            c = f.webs[w]["reg"]
            r = rec(colored | {w}, claimed | ({c} if c in CALLEE else set()), order + [w])
            if r is not None:
                return r
        return None

    sys.setrecursionlimit(10000)
    return rec(frozenset(), frozenset(), [])


def forced(f):
    """u -> v edges: u must be colored before v (sole neighbour holding a lower volatile colour)."""
    edges = set()
    for v, info in f.webs.items():
        if info["pre"]:
            continue
        c = info["reg"]
        pre = blocked_by_pre(f, v, loose=False)
        for c2 in VOLATILE:
            if c2 == c or (c in VOLATILE and c2 > c) or c2 in pre:
                continue
            holders = [u for u in f.inter[v] if not f.webs[u]["pre"] and f.webs[u]["reg"] == c2]
            if len(holders) == 1:
                edges.add((holders[0], v))
    return edges


def load(path, symbol):
    return Func(disasm(path, symbol))


_TCACHE = {}


def flips(sym, mine_path, target_path):
    """(rank flips, webs colored differently) -- a search objective. Target model is cached."""
    tkey = (str(target_path), sym, Path(target_path).stat().st_mtime)
    if tkey not in _TCACHE:
        _TCACHE[tkey] = load(target_path, sym)
    t, m = _TCACHE[tkey], load(mine_path, sym)
    tk = {t.key(w): w for w in t.webs}
    mk = {m.key(w): w for w in m.webs}
    common = set(tk) & set(mk)
    nd = sum(t.webs[tk[k]]["reg"] != m.webs[mk[k]]["reg"] for k in common) + len(set(tk) ^ set(mk))
    tkey = {(t.key(a), t.key(b)) for a, b in forced(t)}
    mkey = {(m.key(a), m.key(b)) for a, b in forced(m)}
    return sum((b, a) in mkey for a, b in tkey), nd


def _shape(f):
    out = []
    for x in f.ins:
        s = re.sub(r"\s*[@;].*$", "", x.text)
        s = re.sub(r"\s*<[^>]*>", "", s)
        s = re.sub(r"\b(r1[0-5]|r[0-9]|sl|fp|ip|lr)\b", "R", s)
        s = re.sub(r"\[sp(, #-?\d+)?\]", "[sp,S]", s)
        s = re.sub(r"\[pc, #\d+\]", "[pc]", s)
        s = re.sub(r"^(b\w*)\s+[0-9a-f]+$", r"\1 T", s)
        out.append(s)
    return out


def objective(sym, mine_path, target_path):
    """(flips, webs differing, order_intact): order = reg- and slot-blind instruction stream equal."""
    fl, nd = flips(sym, mine_path, target_path)
    t = _TCACHE[(str(target_path), sym, Path(target_path).stat().st_mtime)]
    return fl, nd, _shape(t) == _shape(load(mine_path, sym))


def report(sym, mine_path, target_path, verbose):
    t, m = load(target_path, sym), load(mine_path, sym)
    tk = {t.key(w): w for w in t.webs}
    mk = {m.key(w): w for w in m.webs}
    common = set(tk) & set(mk)
    print(f"webs: target {len(t.webs)}, mine {len(m.webs)}, paired {len(common)}")
    diffs = [k for k in common if t.webs[tk[k]]["reg"] != m.webs[mk[k]]["reg"]]
    print(f"\n{len(diffs)} webs colored differently (target -> mine):")
    for k in sorted(diffs, key=lambda k: min(k) if all(isinstance(e, int) for e in k) else 0):
        w = tk[k]
        print(f"  {RNAME[t.webs[w]['reg']]:>3} -> {RNAME[m.webs[mk[k]]['reg']]:<3} {t.desc(w)}")
    for side, f in (("target", t), ("mine", m)):
        o = find_order(f)
        print(f"\n{side}: model {'REPRODUCES' if o else 'cannot reproduce'} its coloring"
              + (f" (pop order of {len(o)} webs found)" if o is not None else ""))
    te, me = forced(t), forced(m)
    tkey = {(t.key(a), t.key(b)) for a, b in te}
    mkey = {(m.key(a), m.key(b)) for a, b in me}
    flips = [(a, b) for a, b in tkey if (b, a) in mkey]
    only_t = [(a, b) for a, b in tkey - mkey if (b, a) not in mkey and a in common and b in common]
    print(f"\nRANK FLIPS (target forces A before B, mine forces B before A): {len(flips)}")
    for a, b in sorted(flips, key=lambda e: (t.desc(tk[e[0]]), t.desc(tk[e[1]]))):
        print(f"  A {RNAME[t.webs[tk[a]]['reg']]:>3}/{RNAME[m.webs[mk[a]]['reg']]:<3} {t.desc(tk[a])}")
        print(f"  B {RNAME[t.webs[tk[b]]['reg']]:>3}/{RNAME[m.webs[mk[b]]['reg']]:<3} {t.desc(tk[b])}\n")
    if verbose:
        print(f"target-only forced edges: {len(only_t)}")
        for a, b in only_t:
            print(f"  {t.desc(tk[a])}  BEFORE  {t.desc(tk[b])}")


def validate(limit):
    target = ROOT / "build/delinks/_dsd_gap@main_7.o"
    tgt_dump = fn_diff.objdump(target)
    objs = sorted((ROOT / "build/arm9/src").rglob("*.o"))
    import asm_smell
    asm_defs = asm_smell.transcribed_symbols()
    tot = good = unsup = asm = 0
    bad = []
    for o in objs:
        syms = subprocess.run([OBJDUMP, "-t", str(o)], capture_output=True, text=True).stdout
        dump = fn_diff.objdump(o)
        for line in syms.splitlines():
            p = line.split()
            if len(p) < 6 or "F" not in p[2:4] or ".text" not in line:
                continue
            sym = p[-1]
            a, fa = fn_diff.extract(dump, sym, False)
            b, fb = fn_diff.extract(tgt_dump, sym, False)
            if not (fa and fb and a and a == b):
                continue
            if sym in asm_defs:
                asm += 1
                continue
            try:
                f = load(o, sym)
            except Unsupported:
                unsup += 1
                continue
            tot += 1
            if find_order(f, budget=20000) is not None:
                good += 1
            else:
                bad.append(sym)
            if limit and tot >= limit:
                break
        if limit and tot >= limit:
            break
    print(f"matched functions modelled: {tot} (skipped: {asm} asm-defined, {unsup} unsupported insns)")
    print(f"coloring reproduced by model: {good}/{tot}")
    for s in bad[:40]:
        print("  no order:", s)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("symbol", nargs="?")
    ap.add_argument("--mine")
    ap.add_argument("--target", default=str(ROOT / "build/delinks/_dsd_gap@main_7.o"))
    ap.add_argument("--validate", action="store_true")
    ap.add_argument("--limit", type=int, default=0)
    ap.add_argument("-v", action="store_true")
    ap.add_argument("--brief", action="store_true", help="print '<flips> <webs differing>' only")
    a = ap.parse_args()
    if a.validate:
        return validate(a.limit)
    mine = Path(a.mine) if a.mine else fn_diff.find_mine(a.symbol)
    if a.brief:
        print(*flips(a.symbol, mine, Path(a.target)))
        return 0
    report(a.symbol, mine, Path(a.target), a.v)


if __name__ == "__main__":
    sys.exit(main())
