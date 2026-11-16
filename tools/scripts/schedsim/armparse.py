import re

CONDS = ["eq", "ne", "cs", "hs", "cc", "lo", "mi", "pl", "vs", "vc", "hi", "ls", "ge", "lt", "gt", "le", "al"]
DP3 = ["and", "eor", "sub", "rsb", "add", "adc", "sbc", "rsc", "orr", "bic"]
DP2 = ["mov", "mvn"]
CMP = ["cmp", "cmn", "tst", "teq"]
SHIFT = ["lsl", "lsr", "asr", "ror"]
MULS = ["mul", "mla", "umull", "umlal", "smull", "smlal"]
LOADS = ["ldrsb", "ldrsh", "ldrb", "ldrh", "ldr"]
STORES = ["strb", "strh", "str"]
BASES = sorted(DP3 + DP2 + CMP + SHIFT + MULS + LOADS + STORES + ["push", "pop", "clz", "rrx", "nop", "swi", "svc"], key=len, reverse=True)
REGS = {"r%d" % i: "r%d" % i for i in range(16)}
REGS.update({"sl": "r10", "fp": "r11", "ip": "r12", "sp": "sp", "lr": "lr", "pc": "pc", "r13": "sp", "r14": "lr", "r15": "pc"})


class Insn:
    __slots__ = ("text", "op", "cond", "s", "defs", "uses", "mem", "shape", "branch", "target", "idx")

    def __repr__(self):
        return "<%s>" % self.text


def split_mnem(m):
    for b in BASES:
        if m.startswith(b):
            rest = m[len(b):]
            for s in ("s", ""):
                if rest.startswith(s):
                    c = rest[len(s):]
                    if c == "" or c in CONDS:
                        return b, c, bool(s)
    return None


def reg(tok):
    return REGS.get(tok.strip())


def parse_op2(toks):
    """toks: list of comma-separated operand tokens for a flexible operand -> (uses, shape)"""
    t0 = toks[0].strip()
    if t0.startswith("#"):
        return [], t0
    r = reg(t0)
    uses = [r]
    shape = "R"
    if len(toks) > 1:
        sh = toks[1].strip().split()
        if len(sh) == 1:  # rrx
            shape += "," + sh[0]
        elif sh[1].startswith("#"):
            shape += ",%s %s" % (sh[0], sh[1])
        else:
            uses.append(reg(sh[1]))
            shape += ",%s R" % sh[0]
    return uses, shape


def parse(text, idx=0):
    text = text.replace("\t", " ").strip()
    text = re.sub(r"\s*[;@].*$", "", text)
    i = Insn()
    i.text, i.idx = text, idx
    i.defs, i.uses, i.mem, i.branch, i.target, i.shape = [], [], None, False, None, ""
    parts = text.split(None, 1)
    m = parts[0]
    args = parts[1] if len(parts) > 1 else ""
    # branches
    bm = re.match(r"^(blx|bx|bl|b)(%s)?$" % "|".join(CONDS), m)
    if bm and (args.startswith("<") or bm.group(1) in ("bx", "blx") or re.match(r"^[0-9a-fx<]", args)):
        i.op, i.cond, i.s = bm.group(1), bm.group(2) or "", False
        i.branch = True
        tm = re.search(r"<T([+-]\d+)>", args)
        if tm:
            i.target = idx + int(tm.group(1))
        if reg(args):
            i.uses.append(reg(args))
        if i.op in ("bl", "blx"):
            i.defs += ["r0", "r1", "r2", "r3", "r12", "lr"]
            i.uses += ["r0", "r1", "r2", "r3"]
        if i.cond:
            i.uses.append("cpsr")
        i.shape = i.op + i.cond
        return i
    sm = split_mnem(m)
    if not sm:
        i.op, i.cond, i.s = m, "", False
        i.shape = text
        i.branch = True  # unknown: treat as barrier
        return i
    i.op, i.cond, i.s = sm
    if i.op in ("push", "pop"):
        regs = [reg(x) for x in args.strip("{} ").split(",")]
        if i.op == "push":
            i.uses += regs + ["sp"]
            i.defs += ["sp"]
        else:
            i.defs += regs + ["sp"]
            i.uses += ["sp"]
            if "pc" in regs:
                i.branch = True
        i.mem = ("S" if i.op == "push" else "L", "stackop", None)
        i.shape = i.op
    elif i.op in LOADS or i.op in STORES:
        rd, rest = args.split(",", 1)
        rd = reg(rd)
        rest = rest.strip()
        wb = rest.endswith("!")
        am = re.match(r"^\[([^\]]*)\](.*)$", rest)
        inner = [x.strip() for x in am.group(1).split(",")]
        post = am.group(2).strip().lstrip(",").strip().rstrip("!")
        base = reg(inner[0])
        muses, mshape = [], ""
        if len(inner) > 1:
            muses, mshape = parse_op2(inner[1:])
        puses, pshape = ([], "")
        if post:
            puses, pshape = parse_op2([x for x in post.split(",")])
        load = i.op in LOADS
        if base == "sp" and not muses and not post:
            kind, key = "stack", mshape or "#0"
        elif base == "pc":
            kind, key = "pool", mshape
        else:
            kind, key = "ptr", None
        i.mem = ("L" if load else "S", kind, key)
        i.uses += [base] + [u for u in muses + puses]
        if load:
            i.defs.append(rd)
        else:
            i.uses.append(rd)
        if wb or post:
            i.defs.append(base)
        if kind == "pool":
            mshape = ""
        i.shape = "%s [B%s]%s%s" % (i.op, "," + mshape if mshape else "", "!" if wb else "", "," + pshape if post else "")
    elif i.op in DP3 or i.op in SHIFT:
        toks = args.split(",")
        rd = reg(toks[0])
        if i.op in SHIFT:
            rm = reg(toks[1])
            i.uses.append(rm)
            t2 = toks[2].strip()
            if t2.startswith("#"):
                i.shape = "%s %s" % (i.op, t2)
            else:
                i.uses.append(reg(t2))
                i.shape = "%s R" % i.op
        else:
            if len(toks) == 2:  # op rd, op2 (rd is also rn)
                toks = [toks[0], toks[0]] + toks[1:]
            rn = reg(toks[1])
            u2, s2 = parse_op2(toks[2:])
            i.uses += [rn] + u2
            i.shape = "%s %s" % (i.op, s2)
        i.defs.append(rd)
    elif i.op in DP2:
        toks = args.split(",")
        u2, s2 = parse_op2(toks[1:])
        i.uses += u2
        i.defs.append(reg(toks[0]))
        i.shape = "%s %s" % (i.op, s2)
    elif i.op in CMP:
        toks = args.split(",")
        u2, s2 = parse_op2(toks[1:])
        i.uses += [reg(toks[0])] + u2
        i.shape = "%s %s" % (i.op, s2)
        i.s = True
    elif i.op in MULS:
        toks = [reg(x) for x in args.split(",")]
        if i.op in ("mul", "mla"):
            i.defs.append(toks[0])
            i.uses += toks[1:]
        else:
            i.defs += toks[:2]
            i.uses += toks[2:]
            if i.op.endswith("lal"):
                i.uses += toks[:2]
        i.shape = i.op
    elif i.op == "clz":
        toks = [reg(x) for x in args.split(",")]
        i.defs.append(toks[0]); i.uses.append(toks[1]); i.shape = "clz"
    else:
        i.shape = text
        i.branch = i.op in ("swi", "svc")
    if i.s:
        i.defs.append("cpsr")
        if i.op in ("adc", "sbc", "rsc", "rrx"):
            i.uses.append("cpsr")
    if i.cond:
        i.uses.append("cpsr")
        i.uses += [d for d in i.defs if d not in i.uses and d != "cpsr"]
    if "pc" in i.defs:
        i.branch = True
    i.shape = i.shape + ("s" if i.s and i.op not in CMP else "") + ("?" + i.cond if i.cond else "")
    return i


def blocks(insns):
    """split a function's instruction list into basic blocks (lists of Insn)"""
    starts = {0}
    for k, x in enumerate(insns):
        if x.branch:
            starts.add(k + 1)
            if x.target is not None:
                starts.add(x.target)
    starts = sorted(s for s in starts if 0 <= s < len(insns))
    return [insns[a:b] for a, b in zip(starts, starts[1:] + [len(insns)])]


def webs(block):
    """rename physical regs to block-local value ids. returns list of (insn, defs_v, uses_v)"""
    cur = {}
    n = [0]
    out = []
    for x in block:
        uv = []
        for u in x.uses:
            if u not in cur:
                cur[u] = "in:" + u
            uv.append(cur[u])
        dv = []
        for d in x.defs:
            if x.cond and d != "cpsr":
                # conditional def: same web as the old value
                if d not in cur:
                    cur[d] = "in:" + d
                dv.append(cur[d])
                continue
            n[0] += 1
            cur[d] = "v%d" % n[0]
            dv.append(cur[d])
        out.append((x, dv, uv))
    return out


def sig(block):
    """structural signature per instruction, independent of register names and order of independent insns"""
    import hashlib
    w = webs(block)
    prod = {}
    sigs = []
    memstate = {}
    cnt = {}
    for x, dv, uv in w:
        parts = [x.shape]
        for u in uv:
            if u.startswith("in:"):
                r = u[3:]
                parts.append(u if r in ("r0", "r1", "r2", "r3", "sp", "cpsr", "pc") else "in:?")
            else:
                parts.append(prod[u])
        if x.mem:
            ls, kind, key = x.mem
            if kind == "stack":
                parts.append("slot" + key)
                if ls == "L":
                    parts.append(memstate.get(key, "mem-in"))
            elif kind == "pool":
                parts.append("pool")
        h = hashlib.md5("|".join(parts).encode()).hexdigest()[:10]
        if x.mem and x.mem[1] == "stack" and x.mem[0] == "S":
            memstate[x.mem[2]] = h
        for k, d in enumerate(dv):
            prod[d] = h + ":%d" % k
        sigs.append(h)
    return sigs


def parse_list(lines):
    return [parse(t, k) for k, t in enumerate(lines)]
