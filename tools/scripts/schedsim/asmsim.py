"""mwccarm 2.0 list scheduler, ARM9 model, per build/scratch/sched/ARM_SCHEDULER.md.
Works on physical-register asm (pass 2 semantics); pass-1 extras (descriptor level) optional."""
import re
import armparse as A

LAT3 = ("ldrb", "ldrh", "ldrsb", "ldrsh")
EPI_RE = re.compile(r"^(add\w*\s+sp, sp, #|sub\w*\s+sp, sp, #|pop|push|ldm|stm)")


class Node:
    __slots__ = ("i", "x", "key", "lat", "cyc", "defs", "uses", "mem", "clob", "reader", "cdef", "barrier", "term",
                 "succ", "npred", "h", "ready", "deadline", "issued", "desc")


class Opts:
    ptr_alias_stack = False     # pointer access aliases stack slots
    ptr_alias_ptr = True
    cond_def_is_use = False     # conditional instr reads its own Rd
    all_setters_cdef = True     # every flag setter is a "consumed flag def"
    regshift_extra = 1
    immshift_extra = 0
    pool_lat = 2
    mul_cyc = 6
    use_desc = False
    pool_is_load = False        # literal pool load participates in memory deps
    sp_dep = True
    exposed = ()
    reader_to_clob = True
    call_regs = 1        # 0 none, 1 uses + lat-0 defs, 2 uses only, 3 uses + primary defs
    call_is_load = 0
    outargs = ()
    wslots = ()          # sp offsets of NULL-alias (RA spill) slots: worst-case alias


def regshift(x):
    sh = x.shape.split("?")[0]
    if sh.endswith("s") and not sh.endswith("#s"):
        sh = sh[:-1] if sh[-2:-1] in ("R",) or sh.endswith(" Rs") else sh
    return sh.endswith(" R") and (x.op in A.SHIFT or "," in sh)


def immshift(x):
    sh = x.shape.split("?")[0]
    return bool(re.search(r"(lsl|lsr|asr|ror) #", sh))


def make_node(i, x, key, o):
    n = Node()
    n.i, n.x, n.key = i, x, key
    n.succ, n.npred, n.ready, n.issued = [], 0, 0, False
    op = x.op
    cyc = 1
    if op in A.DP3 or op in A.DP2 or op in A.SHIFT or op in A.CMP or op == "clz":
        if regshift(x):
            cyc += o.regshift_extra
        elif immshift(x):
            cyc += o.immshift_extra
        if "pc" in x.defs:
            cyc += 2
    elif op in ("mul", "mla"):
        cyc = o.mul_cyc
    elif op in A.MULS:
        cyc = 7
    elif op in ("b", "bl", "bx", "blx"):
        cyc = 3
    elif op in ("push", "pop"):
        cyc = max(2, len(x.uses) - 1)
    lat = cyc
    if op in A.LOADS:
        lat = 3 if op in LAT3 else 2
    if x.mem and x.mem[1] == "pool":
        lat = o.pool_lat
    n.cyc, n.lat = cyc, lat
    primary = None
    defs = [d for d in x.defs if d != "cpsr"]
    uses = [u for u in x.uses if u != "cpsr"]
    iscall = op in ("bl", "blx")
    if iscall and o.call_regs == 0:
        defs, uses = [], [u for u in uses if u not in ("r0", "r1", "r2", "r3")]
    if iscall and o.call_regs == 2:
        defs = []
    if x.cond and not o.cond_def_is_use:
        # armparse adds Rd to uses for conditional instrs; undo
        tail = [d for d in defs]
        for d in tail:
            if uses and uses[-1] == d:
                uses.pop()
    if not o.sp_dep:
        defs = [d for d in defs if d != "sp"]
        uses = [u for u in uses if u != "sp"]
    n.defs = []
    for k, d in enumerate(defs):
        prim = (k == 0)
        if x.mem and x.mem[0] == "S":
            prim = False            # store: operand 0 is a use; a written-back base is non-primary
        if x.mem and x.mem[0] == "L" and k > 0:
            prim = False
        if iscall and o.call_regs != 3:
            prim = False
        n.defs.append((d, prim))
    n.uses = uses
    n.mem = None
    if x.mem and x.op not in ("push", "pop"):
        ls, kind, mkey = x.mem
        if kind == "pool":
            if o.pool_is_load:
                n.mem = ("L", "pool", key)
        else:
            n.mem = (ls, kind, mkey)
    if iscall and o.call_is_load:
        n.mem = ("L", "call", None)
    setter = x.s or op in A.CMP
    n.clob = setter
    n.cdef = setter and o.all_setters_cdef
    n.reader = bool(x.cond) or op in ("adc", "sbc", "rsc")
    n.barrier = op in ("bl", "blx", "push", "pop", "swi", "svc") or (x.branch and op not in ("b", "bx"))
    n.term = x.branch and not n.barrier
    n.h = lat
    if x.op in A.DP2 and x.shape.startswith("mov R") and "," not in x.shape:
        n.desc = 0
    elif n.mem and n.mem[0] == "L":
        n.desc = 3
    elif not n.defs:
        n.desc = 1
    elif not n.uses or (x.mem and x.mem[1] == "pool"):
        n.desc = 4
    else:
        n.desc = 2
    return n


SIZE = {"ldr": 4, "str": 4, "ldrh": 2, "strh": 2, "ldrsh": 2, "ldrb": 1, "strb": 1, "ldrsb": 1}


def exposed(n, o):
    off = int(n.mem[2].lstrip("#"))
    ex = sorted(o.exposed)
    for k, N in enumerate(ex):
        hi = ex[k + 1] if k + 1 < len(ex) else N + 4096
        if N <= off < hi:
            return True
    return False


def alias(a, b, o):
    ka, kb = a.mem[1], b.mem[1]
    wa = ka == "ptr" or (ka == "stack" and int(a.mem[2].lstrip("#")) in o.wslots)
    wb = kb == "ptr" or (kb == "stack" and int(b.mem[2].lstrip("#")) in o.wslots)
    if (wa or wb) and not (ka == "ptr" and kb == "ptr") and "pool" not in (ka, kb) and "call" not in (ka, kb):
        if wa and wb:
            return True
        return exposed(b if wa else a, o) if (a if wb else b).mem[1] == "stack" else False
    if ka == "stack" and kb == "stack":
        x, y = int(a.mem[2].lstrip("#")), int(b.mem[2].lstrip("#"))
        return x < y + SIZE.get(b.x.op, 4) and y < x + SIZE.get(a.x.op, 4)
    if ka == "pool" or kb == "pool":
        return False
    if ka == "call" or kb == "call":
        m = b if ka == "call" else a
        return m.mem[1] == "stack" and int(m.mem[2].lstrip("#")) in o.outargs
    if ka == "ptr" and kb == "ptr":
        return o.ptr_alias_ptr
    if o.ptr_alias_stack:
        return True
    return exposed(a if ka == "stack" else b, o)


def add_edge(a, b, lat):
    for s, _ in a.succ:
        if s is b:
            return
    a.succ.insert(0, (b, lat))
    b.npred += 1
    if b.h + lat > a.h:
        a.h = b.h + lat


def build(nodes, o):
    defs, uses = {}, {}
    loads, stores = [], []
    l96, l9e, l9a = [], [], []   # consumed flag defs, clobberers, readers
    barriers, later = [], []
    term = None
    maxh = 0
    for n in reversed(nodes):
        for r, prim in n.defs:
            lat = n.lat if prim else 0
            for u in uses.get(r, ()):
                if u is not n:
                    add_edge(n, u, lat)
            d = defs.get(r)
            if d is not None and d is not n:
                add_edge(n, d, lat)
            defs[r] = n
            uses[r] = []
        for r in n.uses:
            d = defs.get(r)
            if d is not None and d is not n:
                add_edge(n, d, 0)
            uses.setdefault(r, []).insert(0, n)
        if n.mem:
            if n.mem[0] == "L":
                for s in stores:
                    if alias(n, s, o):
                        add_edge(n, s, n.lat)
                loads.insert(0, n)
            else:
                for l in loads:
                    if alias(n, l, o):
                        add_edge(n, l, n.lat)
                for s in stores:
                    if alias(n, s, o):
                        add_edge(n, s, n.lat)
                stores.insert(0, n)
        if n.clob:
            for m in l96:
                add_edge(n, m, 0)
            l9e.insert(0, n)
        if n.reader:
            if o.reader_to_clob:
                for m in l9e:
                    if m is not n:
                        add_edge(n, m, 0)
            l9a.insert(0, n)
        if n.cdef:
            for m in l9a:
                if m is not n:
                    add_edge(n, m, 0)
            l96.insert(0, n)
        if n.barrier:
            for m in later:
                add_edge(n, m, 0)
            barriers.insert(0, n)
        for m in barriers:
            if m is not n:
                add_edge(n, m, 0)
        if not n.succ and term is not None:
            add_edge(n, term, 0)
        if n.term:
            term = n
        later.insert(0, n)
        maxh = max(maxh, n.h)
    for n in nodes:
        n.deadline = maxh - n.h


def relcount(n):
    return sum(1 for s, _ in n.succ if s.npred == 1)


def pick(nodes, cycle, o):
    best = None
    for c in nodes:
        if c.issued or c.npred or c.ready > cycle:
            continue
        if best is None:
            best = c
            continue
        bd, cd = best.deadline <= cycle, c.deadline <= cycle
        if bd and not cd:
            continue
        if cd and not bd:
            best = c
            continue
        rb, rc = relcount(best), relcount(c)
        if rb > rc:
            continue
        if rb < rc:
            best = c
            continue
        if best.h > c.h:
            continue
        if best.h < c.h:
            best = c
            continue
        if o.use_desc and best.desc > c.desc:
            best = c
    return best


def schedule(nodes, o, trace=None):
    build(nodes, o)
    order = []
    cycle, busy = 0, 0
    left = len(nodes)
    guard = 0
    while left:
        if busy == 0:
            n = pick(nodes, cycle, o)
            if n is not None:
                if trace is not None:
                    trace.append((cycle, n, [(c.key, c.deadline, relcount(c), c.h) for c in nodes if not c.issued and not c.npred and c.ready <= cycle]))
                n.issued = True
                order.append(n)
                left -= 1
                for s, lat in n.succ:
                    s.npred -= 1
                    s.ready = max(s.ready, cycle + lat)
                busy = n.cyc
        if busy:
            busy -= 1
        cycle += 1
        guard += 1
        if guard > 200000:
            raise RuntimeError("stuck")
    return order


def sim_block(texts, keys, o, trace=None):
    """texts: fn_diff-style instruction strings for one block (parseable); keys: identity strings. returns keys in scheduled order"""
    xs = [A.parse(t, k) for k, t in enumerate(texts)]
    nodes = [make_node(k, x, keys[k], o) for k, x in enumerate(xs)]
    return [n.key for n in schedule(nodes, o, trace)]


def frame_info(texts, o):
    """per-function: address-exposed frame offsets and outgoing-argument slots"""
    ex, loaded, stored = set(), set(), set()
    for t in texts:
        m = re.match(r"^add\w*\s+(?!sp)\w+, sp, #(\d+)", t)
        if m:
            ex.add(int(m.group(1)))
        if re.match(r"^mov\w*\s+\w+, sp$", t):
            ex.add(0)
        m = re.match(r"^(ldr|str)\w*\s+\w+, \[sp(?:, #(\d+))?\]$", t)
        if m:
            (loaded if m.group(1) == "ldr" else stored).add(int(m.group(2) or 0))
    lim = min(ex | loaded) if (ex | loaded) else 1 << 30
    o.exposed = ex
    o.outargs = set(x for x in stored if x not in loaded and x < lim)
    if getattr(o, "wslots_auto", False):
        o.wslots = narrow_spill_slots(texts)


NARROW_RE = re.compile(r"^(?:(?:lsr|asr)s?\s+(\w+), \w+, #(?:16|24)|and\s+(\w+), \w+, #255)$")
DEF_RE = re.compile(r"^\w+\s+(\w+),")


def narrow_spill_slots(texts):
    """heuristic: sp slots whose every word store is of a value just narrowed to 8/16 bits
    (= word-sized spill of a char/short local => worst-case alias, Alias.c 0x5595e0)"""
    last, narrow, wide, sub = {}, set(), set(), set()
    for t in texts:
        m = re.match(r"^(ldr|str)(\w*)\s+(\w+), \[sp(?:, #(\d+))?\]$", t)
        if m:
            off = int(m.group(4) or 0)
            if m.group(2):
                sub.add(off)
            elif m.group(1) == "str":
                (narrow if last.get(m.group(3)) else wide).add(off)
            if m.group(1) == "str":
                continue
        n = NARROW_RE.match(t)
        d = DEF_RE.match(t)
        if n:
            last[n.group(1) or n.group(2)] = True
        elif d and not t.startswith(("str", "cmp", "cmn", "tst", "teq", "b", "push")):
            last[d.group(1)] = False
        if t.startswith(("bl ", "blx ")):
            for r in ("r0", "r1", "r2", "r3", "ip", "lr"):
                last[r] = False
    return narrow - wide - sub
