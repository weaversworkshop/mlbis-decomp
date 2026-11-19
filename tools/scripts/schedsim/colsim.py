import json, re
from fractions import Fraction
from pathlib import Path

OPNAME = {r[0]: r[1].replace("%s", "") for r in json.load(open(Path(__file__).resolve().parent / "opinfo.json"))}
REGN = ["r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "ip", "sp", "lr", "pc"]
CONDN = ["eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc", "hi", "ls", "ge", "lt", "gt", "le", ""]
FMAX = Fraction(3.4028234663852886e38)
HMAX = Fraction(1.7014117331926443e38)


def rname(r, start=16):
    if r is None or r < 0:
        return "-"
    return REGN[r] if r < start else "v%d" % r


def parse_reg(s):
    s = s.strip().lower()
    if s in REGN:
        return REGN.index(s)
    m = re.fullmatch(r"[vr](\d+)", s)
    if not m:
        raise ValueError("bad register %r" % s)
    return int(m.group(1))


def fmt(ins, start=16):
    out = []
    for kind, _, f, v in ins["ops"]:
        if kind in (0, 1):
            out.append({1: "", 2: "=", 3: "+"}.get(f & 3, "") + rname(v, start))
        else:
            out.append("#%x" % (v & 0xffffffff))
    c = ins.get("cond", 14)
    name = OPNAME.get(ins["op"], "op%x" % ins["op"]) + (CONDN[c] if 0 <= c < 14 else "")
    return "%s %s" % (name, ",".join(out))


def regops(ins, cls=0):
    return [o for o in ins["ops"] if o[0] == 0 and o[1] == cls]


def refs(rec):
    defs, uses = {}, {}
    for bi, b in enumerate(rec["blocks"]):
        for ki, ins in enumerate(b["insns"]):
            for o in regops(ins, rec["cls"]):
                if o[2] & 2:
                    defs.setdefault(o[3], []).append((bi, ki))
                if o[2] & 1:
                    uses.setdefault(o[3], []).append((bi, ki))
    return defs, uses


def special_edges(ins, nxt, g):
    op = ins["op"]
    r = [o[3] if o[0] == 0 else None for o in ins["ops"]]
    no_v6 = not (g["ba5a"] & 0x8000000)
    e = []
    if op == 0x25:
        if g["ba64"] and no_v6:
            e = [(0, 1)]
    elif op in (0x30, 0x32, 0x34):
        if g["ba64"]:
            e = [(0, 2)]
    elif op in (0x46, 0x60):
        e = [(1, 4), (1, 2)]
    elif op in (0x47, 0x61):
        e = [(1, 2)]
    elif op in (0x48, 0x55, 0x62):
        e = [(0, 5), (0, 3)]
    elif op in (0x49, 0x56, 0x63):
        e = [(0, 3)]
    elif op in (0x57, 0x58):
        e = [(0, 3), (1, 3)]
    elif op == 0x5a:
        e = [(0, 3), (0, 2), (1, 2)]
    elif op in (0x59, 0x5b, 0x5c):
        e = [(0, 2), (1, 2)]
    elif op == 0x53:
        e = [(1, 2)]
    elif op == 0x54:
        e = [(0, 3)]
    elif op == 0x77:
        return [(r[0], 14)]
    elif op in (0xb0, 0xbb, 0xbc, 0x202):
        if no_v6:
            e = [(0, 1)]
    elif op in (0xc8, 0xca, 0xfa, 0xfc):
        e = [(0, 1), (0, 2), (1, 2)]
    elif op in (0xf6, 0xf7):
        e = [(0, 2), (1, 2)]
    elif op == 0x111:
        e = [(2, 3)]
    elif op in (0x133, 0x134, 0x137, 0x138, 0x160, 0x235, 0x236, 0x237, 0x238):
        e = [(0, 1)]
    elif op == 0x1df:
        if ins["flags"] & 0x800 and nxt is not None and nxt["flags"] & 0x400:
            e = [(0, 1)]
    return [(r[a], r[b]) for a, b in e if a < len(r) and b < len(r)]


def build_edges(rec):
    cls, start, end = rec["cls"], rec["start"], rec["N"]
    g = rec["globals"]
    E = set()

    def add(a, b):
        if a is None or b is None or a == b:
            return
        E.add((min(a, b), max(a, b)))
    for i in range(start):
        for j in range(i + 1, start):
            add(i, j)
    for blk in rec["blocks"]:
        bits = bytes.fromhex(blk["liveout"])
        live = {i for i in range(end) if i // 8 < len(bits) and bits[i // 8] >> (i % 8) & 1}
        il = blk["insns"]
        for k in range(len(il) - 1, -1, -1):
            ins = il[k]
            ops = ins["ops"]
            ro = regops(ins, cls)
            src1 = ops[1][3] if len(ops) > 1 and ops[1][0] == 0 else None
            for o in ro:
                if o[2] & 2:
                    for r in live:
                        if ins["flags"] & 0x10 and r == src1:
                            continue
                        add(o[3], r)
            for o in ro:
                if o[2] & 2 and not o[2] & 1:
                    live.discard(o[3])
            for o in ro:
                if o[2] & 1:
                    live.add(o[3])
            for a, b in special_edges(ins, il[k + 1] if k + 1 < len(il) else None, g):
                add(a, b)
            if ins["flags"] & 8:
                for o in ro:
                    if o[2] & 8:
                        for c in g["callregs"]:
                            add(o[3], c)
    return E


def dumped_edges(rec):
    return {(min(n["i"], j), max(n["i"], j)) for n in rec["nodes"] for j in n["adj"]}


def degree(i, adj, flags):
    return sum(2 if (flags[i] | flags[j]) & 0x200 else 1 for j in adj[i])


def costs(rec, want=()):
    cls = rec["cls"]
    nodes = rec["nodes"]
    n = len(nodes)
    want = set(want)
    ev = {v: [] for v in want}
    flags = [nd["flags"] & ~0x480 for nd in nodes]
    remat = [nd["remat"] != 0 for nd in nodes]
    R = [0] * n; D = [0] * n; M = [0] * n
    blocks = rec["blocks"]
    opf = {}
    for bi, b in enumerate(blocks):
        for ki, ins in enumerate(b["insns"]):
            for oi, op in enumerate(ins["ops"]):
                opf[bi, ki, oi] = op[2]
    owner = [None] * n; single = [True] * n; fdef = [None] * n; luse = [None] * n
    for bi, b in enumerate(blocks):
        for ki, ins in enumerate(b["insns"]):
            for oi, (kind, kc, f, reg) in enumerate(ins["ops"]):
                if kind != 0 or kc != cls:
                    continue
                if owner[reg] is None:
                    owner[reg] = bi
                elif owner[reg] != bi:
                    single[reg] = False
                if f & 2 and fdef[reg] is None:
                    fdef[reg] = (bi, ki, oi)
                if f & 1:
                    luse[reg] = (bi, ki, oi)
    for i in range(n):
        if single[i]:
            if fdef[i]:
                opf[fdef[i]] |= 0x100
            if luse[i]:
                opf[luse[i]] |= 0x200
            flags[i] |= 0x100
        else:
            flags[i] &= ~0x100
    optsize = rec["globals"].get("optsize", 0)
    for bi, b in enumerate(blocks):
        w = 1 if optsize else b["w"]
        A, B = {}, set()
        il = b["insns"]
        for ki in range(len(il) - 1, -1, -1):
            ins = il[ki]
            ops = ins["ops"]
            if ins["flags"] & 0x10 and len(ops) > 1 and ops[1][1] == cls:
                for r in (ops[1][3], ops[0][3]):
                    M[r] += 1
                    if r in want:
                        ev[r].append(("M", bi, ki, 1, []))
            condl = ins["flags2"] & 0x40000000 and ins["cond"] != 0xe
            for oi, (kind, kc, _, reg) in enumerate(ops):
                f = opf[bi, ki, oi]
                if kind != 0 or kc != cls or not f & 2:
                    continue
                if condl or f & 0x10:
                    flags[reg] |= 0x400
                if f & 0x40:
                    flags[reg] |= 0x80
                if reg in A:
                    g = A.pop(reg)
                    if reg in want:
                        ev[reg].append(("free", bi, ki, 0, g))
                if f & 0x100 and reg in B:
                    flags[reg] |= 0x80
                    B.discard(reg)
                    if reg in want:
                        ev[reg].append(("never", bi, ki, 0, []))
                D[reg] += w
                if reg in want:
                    ev[reg].append(("D", bi, ki, w, []))
            for oi, (kind, kc, _, reg) in enumerate(ops):
                f = opf[bi, ki, oi]
                if kind != 0 or kc != cls or not f & 4:
                    continue
                for j, g in A.items():
                    R[j] += w
                    if j in want:
                        ev[j].append(("R", bi, ki, w, g))
                A = {}; B = set()
            for oi, (kind, kc, _, reg) in enumerate(ops):
                f = opf[bi, ki, oi]
                if kind != 0 or kc != cls or not f & 1:
                    continue
                if condl or f & 0x10:
                    flags[reg] |= 0x400
                if f & 0x40:
                    flags[reg] |= 0x80
                if f & 0x200:
                    B.add(reg)
                A.setdefault(reg, [])
                if not A[reg] or A[reg][-1] != ki:
                    A[reg].append(ki)
        for j, g in A.items():
            R[j] += w
            if j in want:
                ev[j].append(("R", bi, -1, w, g))
    cost = []
    for i in range(n):
        if remat[i]:
            c = R[i] - D[i]
        elif flags[i] & 0x8000:
            c = 2 * R[i] - 1
        else:
            c = 2 * R[i] + D[i]
        cost.append(c - M[i])
    return {"cost": cost, "R": R, "D": D, "M": M, "flags": flags, "events": ev}


def dumped_costs(rec):
    if not rec.get("costs"):
        return None
    nd = rec["nodes"]
    return {"cost": [n["cost"] for n in nd], "R": [n["R"] for n in nd], "D": [n["D"] for n in nd],
            "M": [n["M"] for n in nd], "flags": [n["cflags"] for n in nd]}


def value(c, d, f):
    if f & 0x80:
        return FMAX
    if f & 0x400:
        return HMAX
    return Fraction(c, d)


def vstr(v):
    if v == FMAX:
        return "never"
    if v == HMAX:
        return "half-max"
    return "%.3f" % float(v)


def pick(cands):
    best = cands[0]
    for c in cands[1:]:
        if c[3] < best[3]:
            best = c
    return best[0]


def simplify(rec, adj, deg, flags, getcost):
    K, start, end = rec["K"], rec["start"], rec["N"]
    deg = dict(deg); fl = dict(flags)
    stack, pushes, stalls, visits = [], [], [], {}
    st = {"t": 0, "cost": None}

    def push(i, how, s, k):
        pushes.append({"v": i, "how": how, "sweep": s, "stall": k, "deg": deg[i]})
        for j in adj[i]:
            deg[j] -= 2 if (fl[i] | fl[j]) & 0x200 else 1
        fl[i] |= 2
        stack.append(i)
        st["t"] += 1
    sweep = 0
    while True:
        while True:
            changed = False
            rem = []
            for i in range(start, end):
                if fl[i] & 6:
                    continue
                visits.setdefault(i, []).append((st["t"], sweep, deg[i]))
                if deg[i] < K:
                    push(i, "sweep", sweep, None)
                    changed = True
                else:
                    rem.insert(0, i)
            sweep += 1
            if not changed:
                break
        if not rem:
            break
        if st["cost"] is None:
            st["cost"] = getcost()
        c = st["cost"]
        cands = [(i, c["cost"][i], deg[i], value(c["cost"][i], deg[i], c["flags"][i])) for i in rem]
        best = pick(cands)
        stalls.append({"after_sweep": sweep - 1, "t": st["t"], "pick": best,
                       "cands": [[i, ci, di, float(v), c["flags"][i]] for i, ci, di, v in cands]})
        push(best, "stall", sweep - 1, len(stalls))
    for k, p in enumerate(pushes):
        p["t"] = k
    return {"order": stack[::-1], "pushes": pushes, "stalls": stalls, "visits": visits, "costs": st["cost"]}


def assign(rec, order, adj, flags, col0):
    rs = rec["rs"]
    nphys = rs["nphys"]
    res = rs["saved_reserved"]
    mask = 0
    for r in rs["scratch"][:rs["nscratch"]]:
        if res[r] == 0:
            mask |= 1 << r
    counter = rs["saved_counter"]
    callee = rs["callee"][:rs["ncallee"]]
    col = dict(col0)
    ok = 1
    failed, acq = [], {}
    for i in order:
        while True:
            m = mask
            for j in adj[i]:
                c = col[j]
                if c != -1:
                    m &= ~((3 if flags[j] & 0x200 else 1) << c)
            if m:
                if flags[i] & 0x200:
                    for r in range(0, nphys, 2):
                        if (m >> r) & 3 == 3:
                            col[i] = r
                            break
                else:
                    for r in range(nphys):
                        if m >> r & 1:
                            col[i] = r
                            break
            if col[i] != -1:
                break
            nr = -1
            while counter < len(callee):
                r = callee[counter]; counter += 1
                if res[r] == 0:
                    nr = r
                    break
            if nr == -1:
                ok = 0
                failed.append(i)
                break
            mask |= 1 << nr
            acq[nr] = i
    return {"colour": col, "ok": ok, "failed": failed, "acquired": acq}


def graph(rec):
    adj = {n["i"]: set(n["adj"]) for n in rec["nodes"]}
    flags = {n["i"]: n["flags"] for n in rec["nodes"]}
    deg = {n["i"]: n["deg"] for n in rec["nodes"]}
    col0 = {n["i"]: n["col0"] for n in rec["nodes"]}
    return adj, deg, flags, col0


def parse_edit(s):
    s = s.strip()
    m = re.fullmatch(r"cost\s+(\S+)\s*(\+=|-=|=)\s*(-?\d+)", s)
    if m:
        return ("cost", parse_reg(m.group(1)), m.group(2), int(m.group(3)))
    m = re.fullmatch(r"edge\s+([+-])\s*(\S+?)\s*,\s*(\S+)", s)
    if m:
        return ("edge", m.group(1), parse_reg(m.group(2)), parse_reg(m.group(3)))
    raise ValueError("bad edit %r (want 'cost vN=K', 'cost vN+=K', 'edge -vA,vB', 'edge +vA,vB')" % s)


def run(rec, edits=()):
    adj, deg, flags, col0 = graph(rec)
    adj = {k: set(v) for k, v in adj.items()}
    cedits = []
    for e in edits:
        if e[0] == "edge":
            _, sign, a, b = e
            if a == b or a not in adj or b not in adj:
                raise ValueError("bad edge %s,%s" % (rname(a), rname(b)))
            inc = 2 if (flags[a] | flags[b]) & 0x200 else 1
            if sign == "+" and b not in adj[a]:
                adj[a].add(b); adj[b].add(a); deg[a] += inc; deg[b] += inc
            elif sign == "-" and b in adj[a]:
                adj[a].discard(b); adj[b].discard(a); deg[a] -= inc; deg[b] -= inc
        else:
            cedits.append(e)

    def getcost():
        c = dumped_costs(rec) or costs(rec)
        c = {k: list(v) for k, v in c.items() if k != "events"}
        for _, v, op, k in cedits:
            c["cost"][v] = k if op == "=" else c["cost"][v] + (k if op == "+=" else -k)
        return c
    tr = simplify(rec, adj, deg, flags, getcost)
    aflags = dict(flags)
    if tr["costs"]:
        aflags = {i: tr["costs"]["flags"][i] for i in flags}
    tr.update(assign(rec, tr["order"], adj, aflags, col0))
    return tr


def annotate(rec):
    tr = run(rec)
    rec["pushes"] = [{k: p[k] for k in ("v", "how", "sweep", "stall", "deg")} for p in tr["pushes"]]
    rec["stalls"] = [{k: s[k] for k in ("after_sweep", "pick", "cands")} for s in tr["stalls"]]
    rec["trace_ok"] = tr["order"] == rec["order"] and [s["pick"] for s in tr["stalls"]] == rec["picks"]
    return rec


def check(rec):
    err = {"graph": [], "cost": [], "push": [], "colour": [], "post": []}
    real, sim = dumped_edges(rec), build_edges(rec)
    if real != sim:
        err["graph"].append("edges: %d missing %s, %d extra %s" % (len(real - sim), sorted(real - sim)[:8],
                                                                     len(sim - real), sorted(sim - real)[:8]))
    adj, deg, flags, col0 = graph(rec)
    for n in rec["nodes"]:
        i = n["i"]
        if i >= rec["start"] and not n["flags"] & 4 and (degree(i, adj, flags) != n["deg"] or len(adj[i]) != n["nadj"]):
            err["graph"].append("degree v%d: %d/%d vs rule %d/%d" % (i, n["deg"], n["nadj"], degree(i, adj, flags), len(adj[i])))
    dc = dumped_costs(rec)
    if dc:
        mc = costs(rec)
        for i in range(len(rec["nodes"])):
            a = (mc["cost"][i], mc["R"][i], mc["D"][i], mc["M"][i], mc["flags"][i] & 0x480)
            b = (dc["cost"][i], dc["R"][i], dc["D"][i], dc["M"][i], dc["flags"][i] & 0x480)
            if a != b:
                err["cost"].append("v%d sim %s dump %s" % (i, a, b))
    tr = run(rec)
    if tr["order"] != rec["order"]:
        k = next((k for k, (x, y) in enumerate(zip(tr["order"], rec["order"])) if x != y), min(len(tr["order"]), len(rec["order"])))
        err["push"].append("stack differs at pop %d of %d/%d" % (k, len(tr["order"]), len(rec["order"])))
    picks = [s["pick"] for s in tr["stalls"]]
    if picks != rec["picks"]:
        err["push"].append("picks sim %s dump %s" % (picks, rec["picks"]))
    adj2, _, fl2, col02 = graph(rec)
    aflags = {i: (dc["flags"][i] if dc else fl2[i]) for i in fl2}
    a = assign(rec, rec["order"], adj2, aflags, col02)
    for n in rec["nodes"]:
        if n["i"] in rec["order"] and a["colour"][n["i"]] != n["colour"]:
            err["colour"].append("v%d sim %s dump %s" % (n["i"], rname(a["colour"][n["i"]]), rname(n["colour"])))
    if a["ok"] != rec["ok"]:
        err["colour"].append("ok sim %d dump %d" % (a["ok"], rec["ok"]))
    for i in a["failed"]:
        if not rec["nodes"][i]["pflags"] & 1:
            err["colour"].append("failed v%d not flagged" % i)
    post = rec.get("post")
    if post is not None:
        col = {n["i"]: n["colour"] for n in rec["nodes"]}
        for b in rec["blocks"]:
            for ins in b["insns"]:
                got = post.get(str(ins["p"]))
                if got is None:
                    continue
                for o, g in zip(ins["ops"], got):
                    if o[0] == 0 and o[1] == rec["cls"]:
                        want = col[o[3]] if o[3] >= rec["start"] else o[3]
                        if want != g:
                            err["post"].append("%s: v%d colour %s rewritten %s" % (fmt(ins), o[3], rname(want), rname(g)))
    elif rec["ok"]:
        err["post"].append("no post-rewrite record")
    return err
