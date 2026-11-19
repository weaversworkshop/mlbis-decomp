import argparse, json, math, sys
from fractions import Fraction
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import colsim as C  # noqa: E402


class View:
    def __init__(self, rec):
        self.r = rec
        self.s = rec["start"]
        self.nodes = rec["nodes"]
        self.defs, self.uses = C.refs(rec)
        self.tr = C.run(rec)
        self.push = {p["v"]: p for p in self.tr["pushes"]}
        self.npop = len(self.tr["order"])
        self.dc = C.dumped_costs(rec)
        self.mc = None

    def rn(self, v):
        return C.rname(v, self.s)

    def reg(self, v):
        c = self.nodes[v]["colour"]
        return C.rname(c) if c is not None and c >= 0 else "SPILL"

    def var(self, v):
        return self.nodes[v]["var"] or ""

    def dead(self, v):
        return v >= self.s and v not in self.defs and v not in self.uses and not self.nodes[v]["flags"] & 4

    def pos(self, bi, ki):
        return "b%d:%d" % (self.r["blocks"][bi]["id"], ki)

    def ins(self, bi, ki):
        return C.fmt(self.r["blocks"][bi]["insns"][ki], self.s)

    def deftext(self, v):
        d = self.defs.get(v) or self.uses.get(v)
        if not d:
            return "dead" if self.dead(v) else ""
        bi, ki = d[0]
        return "%-36s [%s]%s" % (self.ins(bi, ki), self.pos(bi, ki), "" if v in self.defs else " (use; no def)")

    def costs(self):
        if self.dc:
            return self.dc, "dumped"
        if self.mc is None:
            self.mc = C.costs(self.r)
        return self.mc, "computed (no stall this round: the compiler never ran the cost pass)"

    def popno(self, v):
        p = self.push.get(v)
        return self.npop - p["t"] if p else None

    def how(self, v):
        p = self.push.get(v)
        if not p:
            return "not pushed"
        if p["how"] == "stall":
            return "stall %d pick (after sweep %d), deg %d" % (p["stall"], p["sweep"], p["deg"])
        return "sweep %d, deg %d" % (p["sweep"], p["deg"])


def header(V):
    r = V.r
    n = r["N"] - r["start"]
    nd = sum(1 for v in range(r["start"], r["N"]) if V.dead(v))
    nm = sum(1 for v in range(r["start"], r["N"]) if r["nodes"][v]["flags"] & 4)
    print("%s  fn %d  round %d  K=%d  N=%d  vregs %d (%d dead, %d coalesced)  edges %d  stalls %d  %s%s" % (
        r["func"], r["fn"], r["round"], r["K"], r["N"], n, nd, nm, len(C.dumped_edges(r)), len(r["picks"]),
        "ok" if r["ok"] else "FAILED -> spill " + " ".join(V.rn(i) for i in V.tr["failed"]),
        "" if r.get("trace_ok", True) else "  WARNING: simulated push order differs from the dump"))
    if r["merges"]:
        print("merges in this round's builds: " + " ".join("%s<-%s" % (V.rn(m["keep"]), V.rn(m["gone"])) for m in r["merges"]))


def show_default(V):
    header(V)
    print("   %-8s %-5s %-4s %-10s %-7s %s" % ("pop", "vreg", "reg", "variable", "degree", "first def"))
    acq = {v: r for r, v in V.tr["acquired"].items()}
    group = None
    dead = []

    def flush_dead():
        if dead:
            print("   dead x%-3d %s" % (len(dead), " ".join(V.rn(v) for v in dead[:12]) + (" ..." if len(dead) > 12 else "")))
            dead.clear()
    for p in V.tr["pushes"]:
        v = p["v"]
        g = ("stall %d (after sweep %d)" % (p["stall"], p["sweep"])) if p["how"] == "stall" else "sweep %d" % p["sweep"]
        if g != group:
            flush_dead()
            group = g
            print(" " + g)
        if V.dead(v):
            dead.append(v)
            continue
        flush_dead()
        extra = "  (acquired %s)" % C.rname(acq[v]) if v in acq else ""
        print("   pop %3d  %-5s %-4s %-10s deg %-3d %s%s" % (V.npop - p["t"], V.rn(v), V.reg(v), V.var(v), p["deg"], V.deftext(v), extra))
    flush_dead()


def breakdown(c, i, remat):
    R, D, M = c["R"][i], c["D"][i], c["M"][i]
    if remat:
        return "R-D-M = %d-%d-%d (remat)" % (R, D, M)
    if c["flags"][i] & 0x8000:
        return "2R-1-M = 2*%d-1-%d" % (R, M)
    return "2R+D-M = 2*%d+%d-%d" % (R, D, M)


def show_stalls(V):
    header(V)
    c, src = V.costs()
    if not V.tr["stalls"]:
        print("no stalls: every web reached degree < K in a sweep")
    for k, s in enumerate(V.tr["stalls"], 1):
        print(" stall %d after sweep %d: pick %s -> %s   (scan order: descending vreg, strict <, ties to the higher vreg)" % (
            k, s["after_sweep"], V.rn(s["pick"]), V.reg(s["pick"])))
        print("   %-6s %-10s %5s  %-24s %4s  %-9s" % ("vreg", "var", "cost", "", "deg", "value"))
        for i, ci, di, val, f in s["cands"]:
            v = C.value(ci, di, f)
            print("   %-6s %-10s %5d  %-24s %4d  %-9s %s%s" % (V.rn(i), V.var(i), ci, breakdown(c, i, V.nodes[i]["remat"]), di,
                                                       C.vstr(v), "<- pick" if i == s["pick"] else "",
                                                       "  (0x80 never spill)" if f & 0x80 else ""))
    if V.tr["stalls"] and not V.dc:
        print("costs " + src)


def show_node(V, v):
    n = V.nodes[v]
    fl = n["flags"]
    print("%s  %s  colour %s  flags 0x%x  deg %d  nadj %d%s" % (V.rn(v), V.var(v) or "(no variable)", V.reg(v), fl, n["deg"], n["nadj"],
                                                            "  DEAD (no def/use)" if V.dead(v) else ""))
    if fl & 4:
        print("  coalesced away (flag 4)")
    if n["remat"]:
        print("  rematerialisable (single def)")
    for kind, lst in (("def", V.defs.get(v, [])), ("use", V.uses.get(v, []))):
        for bi, ki in lst:
            print("  %s  %-8s %s" % (kind, V.pos(bi, ki), V.ins(bi, ki)))
    adj = sorted(n["adj"])
    print("  adj (%d): %s" % (len(adj), " ".join("%s=%s" % (V.rn(j), C.rname(V.nodes[j]["colour"]) if V.nodes[j]["colour"] is not None and V.nodes[j]["colour"] >= 0 else "-") for j in adj)))
    taken = sorted({V.nodes[j]["colour"] for j in adj if V.nodes[j]["colour"] is not None and V.nodes[j]["colour"] >= 0})
    print("  neighbour colours: %s" % " ".join(C.rname(c) for c in taken))
    if v in V.push:
        print("  pushed %s; popped %d of %d" % (V.how(v), V.popno(v), V.npop))
    c, src = V.costs()
    print("  cost %d  (%s)  %s" % (c["cost"][v], breakdown(c, v, n["remat"]), "" if src == "dumped" else src))


def show_cost(V, v):
    c, src = V.costs()
    mc = C.costs(V.r, [v])
    ev = mc["events"][v]
    n = V.nodes[v]
    print("%s %s  cost %d = %s   (%s%s)" % (V.rn(v), V.var(v), mc["cost"][v], breakdown(mc, v, n["remat"]), src,
                                        "" if src != "dumped" or mc["cost"][v] == c["cost"][v] else ", DUMP SAYS %d" % c["cost"][v]))
    if mc["flags"][v] & 0x80:
        print("  flag 0x80: never spilled (value FLT_MAX)")
    if mc["flags"][v] & 0x100:
        print("  single-block web (0x100)")
    ev = sorted(ev, key=lambda e: (e[1], e[2] if e[2] >= 0 else -1))
    for kind, bi, ki, w, grp in ev:
        b = V.r["blocks"][bi]
        uses = ",".join("%d" % k for k in sorted(grp))
        if kind == "R":
            if ki < 0:
                at = "block top b%d (live in)" % b["id"]
            else:
                ins = b["insns"][ki]
                dies = " ".join(V.rn(o[3]) for o in C.regops(ins, V.r["cls"]) if o[2] & 4)
                at = "kill at %s (%s dies): %s" % (V.pos(bi, ki), dies, V.ins(bi, ki))
            print("  R +%-4d uses b%d:{%s} reload; group closed by %s" % (w, b["id"], uses, at))
        elif kind == "D":
            print("  D +%-4d %-8s %s" % (w, V.pos(bi, ki), V.ins(bi, ki)))
        elif kind == "M":
            print("  M +1    %-8s %s" % (V.pos(bi, ki), V.ins(bi, ki)))
        elif kind == "free":
            print("  -       uses b%d:{%s} follow the def at %s with no kill between: no reload" % (b["id"], uses, V.pos(bi, ki)))
        elif kind == "never":
            print("  0x80    def %s reaches its last use with no kill between" % V.pos(bi, ki))
    print("  weights: block weight = 8^loop depth; D and R weighted, M not")


def win_cost(cands, v):
    by = {i: (ci, di, f) for i, ci, di, _, f in cands}
    ci, di, f = by[v]
    if f & 0x480:
        return None

    def wins(c):
        return C.pick([(i, cc, dd, C.value(c if i == v else cc, dd, ff)) for i, cc, dd, _, ff in cands]) == v
    others = [C.value(cc, dd, ff) for i, cc, dd, _, ff in cands if i != v]
    if not others or min(others) >= C.HMAX:
        return "any"
    c0 = math.floor(min(others) * di)
    for c in range(c0 + 1, c0 - 3, -1):
        if wins(c):
            return c
    return None


def lose_cost(cands, v):
    by = {i: (ci, di, f) for i, ci, di, _, f in cands}
    ci, di, f = by[v]
    others = [C.value(cc, dd, ff) for i, cc, dd, _, ff in cands if i != v]
    if not others or min(others) >= C.HMAX:
        return None

    def wins(c):
        return C.pick([(i, cc, dd, C.value(c if i == v else cc, dd, ff)) for i, cc, dd, _, ff in cands]) == v
    c0 = math.ceil(min(others) * di)
    for c in range(c0 - 1, c0 + 3):
        if not wins(c):
            return c
    return None


def try_edit(V, a, b, edit):
    t = C.run(V.r, [C.parse_edit(edit)])
    pos = {v: k for k, v in enumerate(t["order"])}
    first = a if pos[a] < pos[b] else b
    ch = sum(1 for v in t["order"] if t["colour"][v] != V.tr["colour"][v])
    return "%s: %s coloured first; %s %s->%s, %s %s->%s; %d webs change%s" % (
        edit, V.rn(first), V.rn(a), C.rname(V.tr["colour"][a]), C.rname(t["colour"][a]), V.rn(b),
        C.rname(V.tr["colour"][b]), C.rname(t["colour"][b]), ch, "" if t["ok"] else "; round FAILS (spill)")


def show_why(V, a, b):
    K = V.r["K"]
    pa, pb = V.push.get(a), V.push.get(b)
    if not pa or not pb:
        print("both webs must be on the stack (coalesced/physical nodes are not)")
        return
    x, y = (a, b) if pa["t"] > pb["t"] else (b, a)
    px, py = V.push[x], V.push[y]
    print("%s (%s) coloured before %s (%s): pushed later -> popped earlier (pop %d vs %d of %d)" % (
        V.rn(x), V.reg(x), V.rn(y), V.reg(y), V.popno(x), V.popno(y), V.npop))
    print("  %s pushed %s" % (V.rn(x), V.how(x)))
    print("  %s pushed %s" % (V.rn(y), V.how(y)))
    inter = y in V.nodes[x]["adj"]
    print("  they %s" % ("interfere: the first popped takes the lower register" if inter else
                         "do NOT interfere: their relative order does not decide either colour directly"))
    if px["how"] == "sweep" and py["how"] == "sweep" and px["sweep"] == py["sweep"]:
        print("  same sweep: ascending vreg order inside a sweep, so the higher vreg (%s) is pushed later" % V.rn(x))
    print("to colour %s before %s, one of:" % (V.rn(y), V.rn(x)))
    vis = [t for t in V.tr["visits"].get(x, []) if t[0] <= py["t"]]
    if vis:
        t, s, d = min(vis, key=lambda q: (q[2], -q[0]))
        print("  - %s pushed before %s: at its sweep-%d visit it had degree %d; needs %d fewer interferences "
              "(with webs still unpushed then) to reach < %d" % (V.rn(x), V.rn(y), s, d, d - K + 1, K))
    else:
        print("  - %s is pushed in sweep %d before the ascending scan reaches %s" % (V.rn(y), py["sweep"], V.rn(x)))
    if py["how"] == "sweep":
        print("  - %s kept out of sweep %d: it had degree %d < %d at that visit; needs +%d interferences (first order: "
              "neighbour degrees move too; check with --what-if \"edge +%s,vN\")" % (V.rn(y), py["sweep"], py["deg"], K, K - py["deg"], V.rn(y)))
    edits = []
    for k, s in enumerate(V.tr["stalls"], 1):
        if V.tr["pushes"][next(i for i, p in enumerate(V.tr["pushes"]) if p["how"] == "stall" and p["stall"] == k)]["t"] > py["t"]:
            continue
        ids = [c[0] for c in s["cands"]]
        if x in ids:
            c = win_cost(s["cands"], x)
            cur = next(q for q in s["cands"] if q[0] == x)
            if c == "any":
                print("  - stall %d: %s wins at any cost (others are never-spill)" % (k, V.rn(x)))
            elif c is not None:
                print("  - stall %d: %s picked instead of %s if its cost <= %d (now %d, deg %d; value must beat %s)" % (
                    k, V.rn(x), V.rn(s["pick"]), c, cur[1], cur[2], C.vstr(min(C.value(q[1], q[2], q[4]) for q in s["cands"] if q[0] != x))))
                edits.append("cost %s=%d" % (V.rn(x), c))
            else:
                print("  - stall %d: %s cannot be picked by cost (flag 0x80/0x400)" % (k, V.rn(x)))
        if s["pick"] == y:
            c = lose_cost(s["cands"], y)
            cur = next(q for q in s["cands"] if q[0] == y)
            if c is not None:
                print("  - stall %d: %s not picked if its cost >= %d (now %d, deg %d)" % (k, V.rn(y), c, cur[1], cur[2]))
                edits.append("cost %s=%d" % (V.rn(y), c))
    for e in edits:
        print("    what-if " + try_edit(V, a, b, e))


def show_whatif(V, edits):
    header(V)
    ed = [C.parse_edit(e) for e in edits]
    for e in ed:
        if e[0] == "cost" and (V.costs()[0]["flags"][e[1]] & 0x480):
            print("note: %s has flag 0x%x: its value is fixed, cost edits do not move it" % (V.rn(e[1]), V.costs()[0]["flags"][e[1]] & 0x480))
    t = C.run(V.r, ed)
    base = V.tr
    print("edits: " + "; ".join(edits))
    if any(base["colour"][v] != V.nodes[v]["colour"] for v in base["order"]):
        print("WARNING: the unedited simulation does not reproduce the dumped colours; comparing against the simulation")
    bp, tp = [s["pick"] for s in base["stalls"]], [s["pick"] for s in t["stalls"]]
    if bp != tp:
        print("stall picks: %s -> %s" % (" ".join(V.rn(v) for v in bp) or "-", " ".join(V.rn(v) for v in tp) or "-"))
    ch = [v for v in sorted(set(base["order"]) | set(t["order"])) if base["colour"].get(v) != t["colour"].get(v)]
    for v in ch:
        print("  %-6s %-10s %s -> %s" % (V.rn(v), V.var(v), C.rname(base["colour"].get(v, -1)), C.rname(t["colour"].get(v, -1)) if t["colour"].get(v, -1) >= 0 else "SPILL"))
    if not ch:
        print("  no web changes register")
    if not t["ok"]:
        print("round fails: spill " + " ".join(V.rn(v) for v in t["failed"]) + " (the compiler would then re-run with spill code)")
    po = [v for v in t["order"] if v in base["order"]]
    moved = sum(1 for p, q in zip(po, base["order"]) if p != q)
    print("pop order: %s" % ("unchanged" if moved == 0 else "%d positions differ" % moved))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("dump")
    ap.add_argument("--func")
    ap.add_argument("--fn", type=int)
    ap.add_argument("--round", type=int)
    ap.add_argument("--stalls", action="store_true")
    ap.add_argument("--node", nargs="+")
    ap.add_argument("--cost", nargs="+")
    ap.add_argument("--why", nargs=2)
    ap.add_argument("--what-if", nargs="+", action="extend", dest="whatif")
    a = ap.parse_args()
    recs = [json.loads(l) for l in open(a.dump)]
    recs = [r for r in recs if r.get("kind") == "round"]
    if a.func is None and a.fn is None:
        seen = {}
        for r in recs:
            seen.setdefault((r["fn"], r["func"]), []).append(r)
        for (fn, name), rs in seen.items():
            print("fn %3d  %-40s rounds %d  stalls %s  %s" % (fn, name, len(rs), "/".join(str(len(r["picks"])) for r in rs), "ok" if rs[-1]["ok"] else "FAILED"))
        return
    sel = [r for r in recs if (a.func is None or r["func"] == a.func) and (a.fn is None or r["fn"] == a.fn)]
    if not sel:
        sys.exit("no rounds for %s" % (a.func or a.fn))
    fns = sorted({r["fn"] for r in sel})
    if len(fns) > 1:
        print("note: %d functions named %s (fn %s); showing fn %d, pick one with --fn" % (len(fns), a.func, fns, fns[0]))
        sel = [r for r in sel if r["fn"] == fns[0]]
    rec = sel[-1] if a.round is None else next((r for r in sel if r["round"] == a.round), None)
    if rec is None:
        sys.exit("no round %d (rounds %s)" % (a.round, [r["round"] for r in sel]))
    V = View(rec)
    if a.stalls:
        show_stalls(V)
    elif a.node:
        header(V)
        for v in a.node:
            show_node(V, C.parse_reg(v))
    elif a.cost:
        header(V)
        for v in a.cost:
            show_cost(V, C.parse_reg(v))
    elif a.why:
        header(V)
        show_why(V, C.parse_reg(a.why[0]), C.parse_reg(a.why[1]))
    elif a.whatif:
        show_whatif(V, a.whatif)
    else:
        show_default(V)


if __name__ == "__main__":
    main()
