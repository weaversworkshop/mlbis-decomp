"""Exact model of the mwccarm 2.0 sp2p3 scheduler on pcode records from pcdump (notes/mwcc_scheduler.md)."""
import json
from pathlib import Path

OPINFO = {r[0]: r[1].replace("%s", "") for r in json.load(open(Path(__file__).resolve().parent / "opinfo.json"))}
LOAD1 = {0x80, 0x81, 0xa5, 0xae, 0xa9, 0xaa, 0xab, 0xac, 0xad}   # latency > cycles: machine busy 1 cycle
SWP = {0xf6, 0xf7}
F_BRANCH, F_COPY, F_VOL, F_CLOB, F_READ, F_CDEF = 0x1, 0x10, 0x80, 0x200, 0x400, 0x800
F_LOAD, F_STORE, F_SWP, F_BARRIER = 0x20002, 0x40004, 0x40000, 0x100108


def cycles(node):
    op = node["instr"]["op"]
    if op in LOAD1:
        return 1
    if op in SWP:
        return 2
    return max(node["L"], 1)


def bit(vec, i):
    k = (i >> 5) * 8
    if i < 0 or k + 8 > len(vec):
        return 0
    return (int.from_bytes(bytes.fromhex(vec[k:k + 8]), "little") >> (i & 31)) & 1


def alias(a, b, worst):
    if a is None or b is None:
        return False
    if a is b:
        return True
    ta, tb = a["type"], b["type"]
    if ta != 2 and tb != 2 and a["obj"] != b["obj"]:
        return False
    if ta == 0 and tb == 0:
        return False
    if ta != 2 and tb != 2:
        if ta == 1 and tb == 1:
            return a["off"] == b["off"] or b["off"] < a["off"] < b["off"] + b["size"] or a["off"] < b["off"] < a["off"] + a["size"]
        return True
    if ta == 2 and tb == 2:
        n = min(len(a["vec"]), len(b["vec"]))
        return n > 0 and (int(a["vec"][:n], 16) & int(b["vec"][:n], 16)) != 0
    w, o = (a, b) if ta == 2 else (b, a)
    return bool(bit(w["vec"], o["idx"]))


class G:
    def __init__(self, n):
        self.succ = [[] for _ in range(n)]
        self.npred = [0] * n
        self.h = [0] * n

    def edge(self, a, b, lat):
        for t, _ in self.succ[a]:
            if t == b:
                return
        self.succ[a].insert(0, (b, lat))
        self.npred[b] += 1
        if self.h[b] + lat > self.h[a]:
            self.h[a] = self.h[b] + lat


def real_graph(rec):
    n = len(rec["nodes"])
    g = G(n)
    for k, nd in enumerate(rec["nodes"]):
        g.succ[k] = [(t, l) for t, l in nd["succ"]]
        g.h[k] = nd["h"]
        g.npred[k] = nd["npred"]
    return g


def model_graph(rec):
    nodes = rec["nodes"]
    al = rec["aliases"]
    worst = rec["worst"]
    g = G(len(nodes))
    for k, nd in enumerate(nodes):
        g.h[k] = nd["L"]
    defs, uses = {}, {}
    loads, stores, vol, l96, l9e, l9a, barriers, later = [], [], [], [], [], [], [], []
    term = None

    def regdeps(n, key, isdef, primary):
        L = nodes[n]["L"]
        lat = L if primary else 0
        if isdef:
            for u in uses.get(key, ()):
                if u != n:
                    g.edge(n, u, lat)
            d = defs.get(key)
            if d is not None and d != n:
                g.edge(n, d, lat)
            defs[key] = n
            uses[key] = []
        else:
            d = defs.get(key)
            if d is not None and d != n:
                g.edge(n, d, lat)
            uses.setdefault(key, []).insert(0, n)

    for n in range(len(nodes) - 1, -1, -1):
        ins = nodes[n]["instr"]
        fl = ins["flags"]
        ops = ins["ops"]
        L = nodes[n]["L"]
        for i, (kind, f, reg) in enumerate(ops):
            if kind == 0 and f & 2:
                prim = i == 0 or (ops[0][0] == 0 and ops[0][1] & 0x20 and reg == ops[0][2] + 1)
                regdeps(n, (0, reg), True, prim)
            elif kind == 1:
                regdeps(n, (1, reg), bool(f & 2), True)
        for kind, f, reg in ops:
            if kind == 0 and f & 1:
                regdeps(n, (0, reg), bool(f & 2), False)
            elif kind == 1:
                regdeps(n, (1, reg), bool(f & 2), False)
        a = al.get(str(ins["alias"]))
        if fl & F_LOAD:
            for s in stores:
                if alias(a, al.get(str(nodes[s]["instr"]["alias"])), worst):
                    g.edge(n, s, L)
            loads.insert(0, n)
        elif fl & F_STORE:
            for m in loads:
                if alias(a, al.get(str(nodes[m]["instr"]["alias"])), worst):
                    g.edge(n, m, L)
            for s in stores:
                if alias(a, al.get(str(nodes[s]["instr"]["alias"])), worst):
                    g.edge(n, s, L)
            stores.insert(0, n)
            if fl & F_SWP:
                loads.insert(0, n)
        if fl & F_VOL:
            for m in vol:
                g.edge(n, m, 0)
            vol.insert(0, n)
        if fl & F_CLOB:
            for m in l96:
                g.edge(n, m, 0)
            l9e.insert(0, n)
        if fl & F_READ:
            for m in l9e:
                if m != n:
                    g.edge(n, m, 0)
            l9a.insert(0, n)
        if fl & F_CDEF:
            for m in l9a:
                if m != n:
                    g.edge(n, m, 0)
            l96.insert(0, n)
        if fl & F_BARRIER:
            for m in later:
                g.edge(n, m, 0)
            barriers.insert(0, n)
        for m in barriers:
            if m != n:
                g.edge(n, m, 0)
        if not g.succ[n] and term is not None:
            g.edge(n, term, 0)
        if fl & F_BRANCH:
            term = n
        later.insert(0, n)
    return g


def schedule(rec, g, pressure=None):
    nodes = rec["nodes"]
    n = len(nodes)
    maxh = max(g.h) if n else 0
    dl = [maxh - h for h in g.h]
    npred = list(g.npred)
    ready = [0] * n
    done = [False] * n
    cyc = [cycles(nd) for nd in nodes]
    desc = [nd["desc"] for nd in nodes]
    vregs = rec["vregs"]
    order = []
    cycle, busy, left, guard = 0, 0, n, 0
    while left:
        if busy == 0:
            best = None
            for c in range(n):
                if done[c] or npred[c] or ready[c] > cycle:
                    continue
                if best is None:
                    best = c
                    if pressure is not None and pressure.active():
                        break
                    continue
                bd, cd = dl[best] <= cycle, dl[c] <= cycle
                if bd and not cd:
                    continue
                if cd and not bd:
                    best = c
                    continue
                rb = sum(1 for t, _ in g.succ[best] if npred[t] == 1)
                rc = sum(1 for t, _ in g.succ[c] if npred[t] == 1)
                if rb != rc:
                    if rb < rc:
                        best = c
                    continue
                if g.h[best] != g.h[c]:
                    if g.h[best] < g.h[c]:
                        best = c
                    continue
                if vregs and desc[best] > desc[c]:
                    best = c
            if best is not None and pressure is not None and pressure.active():
                for c in range(best + 1, n):
                    if not (done[c] or npred[c] or ready[c] > cycle) and pressure.cost(c) < pressure.cost(best):
                        best = c
            if best is not None:
                done[best] = True
                order.append(best)
                left -= 1
                for t, lat in g.succ[best]:
                    npred[t] -= 1
                    ready[t] = max(ready[t], cycle + lat)
                busy = cyc[best]
                if pressure is not None:
                    pressure.issue(best)
        if busy:
            busy -= 1
        cycle += 1
        guard += 1
        if guard > 1000000:
            raise RuntimeError("scheduler stuck")
    return order


class Pressure:
    """register-pressure pick mode (pass 1 only)"""

    def __init__(self, rec):
        self.nodes = rec["nodes"]
        self.avail, self.live = rec["avail"], rec["live"]
        self.res = set(r for r, s in enumerate(rec["rstate"]) if s == 2)
        nuse = {}
        for nd in self.nodes:
            for kind, f, reg in nd["instr"]["ops"]:
                if kind == 0 and f & 1:
                    nuse[reg] = nuse.get(reg, 0) + 1
        ok4 = all(nuse.get(int(r), 0) <= v <= nuse.get(int(r), 0) + 1 for r, v in rec["uc4"].items())
        self.uc = {int(r): v for r, v in (rec["uc4"] if ok4 else rec["uc2"]).items()}

    def active(self):
        return self.live >= self.avail

    def cost(self, n):
        d = max(1, self.live - self.avail)
        c = 16
        tmp = {}
        for kind, f, reg in self.nodes[n]["instr"]["ops"]:
            if kind != 0 or reg in self.res:
                continue
            if f & 1:
                tmp[reg] = tmp.get(reg, self.uc.get(reg, 0)) - 1
                if tmp[reg] == 0:
                    c -= 1 + d
            elif f & 2 and self.uc.get(reg, 0) != 0:
                c += d
        return c

    def issue(self, n):
        for kind, f, reg in self.nodes[n]["instr"]["ops"]:
            if kind != 0 or reg in self.res:
                continue
            if f & 1:
                self.uc[reg] = self.uc.get(reg, 0) - 1
                if self.uc[reg] == 0:
                    self.live -= 1
            elif f & 2 and self.uc.get(reg, 0) != 0:
                self.live += 1


def fmt(ins):
    def reg(r):
        return "r%d" % r if 0 <= r < 16 else "v%d" % r
    s = OPINFO.get(ins["op"], "op%x" % ins["op"]) + ("" if ins["cond"] == 14 else "?%d" % ins["cond"]) + " "
    return s + ",".join(("%s%s" % ({1: "", 2: "=", 3: "+"}.get(o[1] & 3, "?"), reg(o[2])) if o[0] in (0, 1) else "#%x" % (o[2] & 0xffffffff)) for o in ins["ops"])
