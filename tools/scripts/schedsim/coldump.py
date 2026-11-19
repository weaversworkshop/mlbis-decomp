import argparse, ctypes, json, shutil, struct, subprocess, sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import cc, colsim, pcdump as P  # noqa: E402

k32 = P.k32
BP_D = 0x513e90
BP_BUILD = 0x50a810
BP_MERGE = 0x50b128
BP_CR = 0x51389f
BP_S = 0x513799
BP_P = 0x513947
BP_A = 0x513b20
BP_X = 0x513c75
BP_FIN = 0x513f99
BPS = (BP_D, BP_BUILD, BP_MERGE, BP_CR, BP_S, BP_P, BP_A, BP_X, BP_FIN)
NODES, CLS, FUNC, BLOCKS = 0x63bcb8, 0x6466a9, 0x63a8ac, 0x63a834
WORK = cc.ROOT / "build/schedsim/col"


def run(cmdline, cwd, env, out, want=None):
    si = P.SI(); si.cb = ctypes.sizeof(P.SI); pi = P.PI()
    envb = "".join("%s=%s\0" % kv for kv in env.items()) + "\0"
    if not k32.CreateProcessW(None, ctypes.create_unicode_buffer(cmdline), None, None, False,
                              2 | 0x400, ctypes.create_unicode_buffer(envb), str(cwd), ctypes.byref(si), ctypes.byref(pi)):
        raise ctypes.WinError(ctypes.get_last_error())
    hp = pi.hProcess

    def rd(a, n):
        b = ctypes.create_string_buffer(n)
        if not a or not n or not k32.ReadProcessMemory(hp, a, b, n, None):
            return b"\0" * n
        return b.raw

    def u32(a):
        return struct.unpack("<I", rd(a, 4))[0]

    def i32(a):
        return struct.unpack("<i", rd(a, 4))[0]

    def objname(o):
        hn = u32(o + 8) if o else 0
        return rd(hn + 0xa, 256).split(b"\0")[0].decode("latin1") if hn else None

    def insn(p):
        raw = rd(p, 0x2e)
        fl, fl2 = struct.unpack_from("<II", raw, 0xc)
        op, cond, _, nops = struct.unpack_from("<hBBh", raw, 0x28)
        o = rd(p + 0x2e, 0xe * max(nops, 0))
        ops = []
        for i in range(max(nops, 0)):
            kind, ocls, f = struct.unpack_from("<BbH", o, i * 0xe)
            if kind in (0, 1):
                v = struct.unpack_from("<h", o, i * 0xe + 4)[0]
            else:
                v = struct.unpack_from("<i", o, i * 0xe + 2)[0]
            ops.append([kind, ocls, f, v])
        return {"p": p, "op": op, "cond": cond, "flags": fl, "flags2": fl2, "ops": ops}

    def blocks(cls, nregs):
        res = []
        tab = u32(0x63cf30 + 12 * cls)
        nw = (nregs + 31) // 32
        b = u32(BLOCKS)
        while b and len(res) < 20000:
            braw = rd(b, 0x2c)
            first, = struct.unpack_from("<I", braw, 0x14)
            bid, = struct.unpack_from("<I", braw, 0x1c)
            w, = struct.unpack_from("<i", braw, 0x24)
            bfl, = struct.unpack_from("<H", braw, 0x2a)
            ins = []
            p = first
            while p and len(ins) < 20000:
                ins.append(insn(p))
                p = u32(p)
            lo = u32(tab + 16 * bid + 0xc) if tab and nregs else 0
            res.append({"id": bid, "w": w, "flags": bfl, "liveout": rd(lo, 4 * nw).hex() if lo else "", "insns": ins})
            b = struct.unpack_from("<I", braw, 0)[0]
        return res

    def nodes(end):
        base = u32(NODES)
        raw = rd(base, end * 0x36)
        res = []
        for i in range(end):
            r = raw[i * 0x36:(i + 1) * 0x36]
            obj, defi = struct.unpack_from("<II", r, 4)
            idx, color, flags, deg, nedge = struct.unpack_from("<hhHhH", r, 0x28)
            e = struct.unpack_from("<I", r, 0x32)[0]
            adj = []
            while e and len(adj) < 20000:
                n4, n8, lo, hi = struct.unpack_from("<IIhh", rd(e, 0x10), 4)
                if i < hi:
                    adj.append(hi); e = n4
                else:
                    adj.append(lo); e = n8
            res.append({"i": idx, "var": objname(obj), "flags": flags, "deg": deg, "nadj": nedge, "col0": color,
                        "remat": defi, "adj": adj})
        return res

    def regstate(cls):
        return {"nphys": i32(0x63a2c4 + 4 * cls), "reserved": list(rd(0x6398e0 + 16 * cls, 16)),
                "saved_reserved": list(rd(0x627df0, 16)), "nscratch": i32(0x63a11c + 4 * cls),
                "scratch": list(struct.unpack("<16i", rd(0x6397f8 + 64 * cls, 64))),
                "ncallee": i32(0x63a210 + 4 * cls), "callee": list(struct.unpack("<16i", rd(0x639860 + 64 * cls, 64))),
                "counter": i32(0x63cf40 + 4 * cls), "saved_counter": struct.unpack("<h", rd(0x627e00, 2))[0]}

    st = {"fobj": None, "name": None, "fn": -1, "rnd": 0, "builds": [], "merges": [], "cur": None, "pending": None}

    def emit(r):
        out(colsim.annotate(r))

    def flush():
        if st["pending"] is not None:
            emit(st["pending"])
            st["pending"] = None

    def setfunc():
        fo = u32(FUNC)
        if fo != st["fobj"]:
            st.update(fobj=fo, name=objname(fo), builds=[], merges=[])
    ev = P.DE(); ctx = ctypes.create_string_buffer(716)
    code = None
    while True:
        if not k32.WaitForDebugEvent(ctypes.byref(ev), 0xFFFFFFFF):
            break
        cont = 0x00010002
        if ev.code == 3:
            for bp in BPS:
                k32.WriteProcessMemory(hp, bp, b"\xcc", 1, None); k32.FlushInstructionCache(hp, bp, 1)
        elif ev.code == 1:
            exc = struct.unpack_from("<I", bytes(ev.u), 0)[0]
            addr = struct.unpack_from("<Q", bytes(ev.u), 16)[0]
            if exc in (0x80000003, 0x4000001f) and addr in BPS:
                ht = k32.OpenThread(0x1FFFFF, False, ev.tid)
                struct.pack_into("<I", ctx, 0, 0x10007)
                k32.Wow64GetThreadContext(ht, ctx)
                G = lambda off: struct.unpack_from("<I", ctx, off)[0]
                S = lambda off, v: struct.pack_into("<I", ctx, off, v & 0xffffffff)
                esp = G(0xc4)
                cls = struct.unpack("<b", rd(CLS, 1))[0]
                setfunc()
                active = want is None or st["name"] == want
                cur = st["cur"]
                if addr in (BP_D, BP_A):
                    if addr == BP_D:
                        flush()
                        st["fn"] += 1; st["rnd"] = 0
                    elif cur is not None:
                        order = []; ptrs = []
                        p = u32(esp + 4)
                        while p and len(order) < 100000:
                            order.append(struct.unpack("<h", rd(p + 0x28, 2))[0]); ptrs.append(p)
                            p = u32(p)
                        cur["order"] = order; cur["ptrs"] = ptrs; cur["rs"] = regstate(cls)
                    esp -= 4
                    k32.WriteProcessMemory(hp, esp, struct.pack("<I", G(0xa4)), 4, None)
                    S(0xc4, esp); S(0xb8, addr + 1)
                elif addr == BP_BUILD:
                    st["builds"].append([hex(u32(esp)), 0])
                    S(0xa8, cls); S(0xb8, addr + 7)
                elif addr == BP_MERGE:
                    ebp = G(0xb4)
                    gone = struct.unpack("<h", struct.pack("<H", ebp & 0xffff))[0]
                    if active:
                        st["merges"].append({"build": len(st["builds"]) - 1, "caller": st["builds"][-1][0] if st["builds"] else None,
                                             "keep": struct.unpack("<h", rd(esp + 8, 2))[0], "gone": gone, "copy": insn(u32(esp))})
                    if st["builds"]:
                        st["builds"][-1][1] += 1
                    S(0x9c, gone); S(0xb8, addr + 3)
                elif addr == BP_S:
                    if active:
                        start = i32(0x63a2c4 + 4 * cls); end = i32(0x63d31c + 4 * cls)
                        g = {"ba64": rd(0x63ba64, 1)[0], "ba72": rd(0x63ba72, 1)[0], "a0bc": u32(0x63a0bc),
                             "ba5a": u32(0x63ba5a), "optsize": rd(0x63bc46, 1)[0],
                             "callregs": [u32(0x6397f8 + 4 * (cls * 16 + k)) for k in range(i32(0x63a11c + 4 * cls))]}
                        st["cur"] = {"kind": "round", "fn": st["fn"], "func": st["name"], "round": st["rnd"], "cls": cls,
                                     "K": G(0xb0), "start": start, "N": end, "ok": None, "builds": st["builds"],
                                     "merges": st["merges"], "globals": g, "nodes": nodes(end), "blocks": blocks(cls, end),
                                     "picks": [], "costs": False}
                    st["rnd"] += 1
                    st["builds"] = []; st["merges"] = []
                    S(0xa4, 0); S(0xb8, addr + 2)
                elif addr == BP_CR:
                    if cur is not None:
                        raw = rd(u32(NODES), cur["N"] * 0x36)
                        for n in cur["nodes"]:
                            c, R, D, M = struct.unpack_from("<iiii", raw, n["i"] * 0x36 + 0xc)
                            n.update(cost=c, R=R, D=D, M=M, cflags=struct.unpack_from("<H", raw, n["i"] * 0x36 + 0x2c)[0])
                        cur["costs"] = True
                    S(0xb8, addr + 1)
                elif addr == BP_P:
                    edx = G(0xa8)
                    if cur is not None:
                        cur["picks"].append(struct.unpack("<h", rd(edx + 0x28, 2))[0])
                    S(0xb4, u32(edx + 0x32)); S(0xb8, addr + 3)
                elif addr == BP_X:
                    ok = u32(esp + 4)
                    if cur is not None and "ptrs" in cur:
                        for p in cur.pop("ptrs"):
                            idx, c, f = struct.unpack("<hhH", rd(p + 0x28, 6))
                            cur["nodes"][idx].update(colour=c, pflags=f)
                        for n in cur["nodes"]:
                            n.setdefault("colour", n["col0"]); n.setdefault("pflags", n["flags"])
                        cur["ok"] = ok
                        flush()
                        if ok:
                            st["pending"] = cur
                        else:
                            emit(cur)
                        st["cur"] = None
                    S(0xb0, ok); S(0xb8, addr + 4)
                else:
                    if st["pending"] is not None:
                        post = {}
                        for blk in blocks(cls, 0):
                            for ins in blk["insns"]:
                                post[str(ins["p"])] = [o[3] if o[0] == 0 and o[1] == cls else None for o in ins["ops"]]
                        st["pending"]["post"] = post
                        flush()
                    S(0xb0, u32(esp + 4)); S(0xb8, addr + 4)
                k32.Wow64SetThreadContext(ht, ctx)
                k32.CloseHandle(ht)
            elif exc not in (0x80000003, 0x4000001f):
                cont = 0x80010001
        elif ev.code == 5:
            code = struct.unpack_from("<I", bytes(ev.u), 0)[0]
            k32.ContinueDebugEvent(ev.pid, ev.tid, cont)
            break
        k32.ContinueDebugEvent(ev.pid, ev.tid, cont)
    flush()
    k32.CloseHandle(pi.hThread); k32.CloseHandle(hp)
    return code


def scratch(tu):
    tu = Path(tu).resolve()
    rel = tu.relative_to(cc.ROOT / "arm9/src") if (cc.ROOT / "arm9/src") in tu.parents else Path(tu.name)
    src = WORK / "src" / "_".join(rel.parts)
    src.parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(tu, src)
    return src


def dump(tu, outp, src=None, want=None, obj=None):
    src = src or scratch(tu)
    obj = Path(obj or WORK / "obj" / (Path(src).stem + ".dbg.o"))
    obj.parent.mkdir(parents=True, exist_ok=True)
    if obj.exists():
        obj.unlink()
    Path(outp).parent.mkdir(parents=True, exist_ok=True)
    n = {"rounds": 0, "funcs": set()}
    with open(outp, "w") as f:
        def w(r):
            n["rounds"] += 1; n["funcs"].add(r["fn"])
            f.write(json.dumps(r) + "\n")
        code = run(subprocess.list2cmdline(cc.command(tu, "stock", obj, src)), cc.ROOT, cc.ENV, w, want)
    return n["rounds"], len(n["funcs"]), obj if obj.exists() and code == 0 else None


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("tu"); ap.add_argument("out")
    ap.add_argument("--src"); ap.add_argument("--func"); ap.add_argument("--obj")
    a = ap.parse_args()
    nr, nf, obj = dump(a.tu, a.out, a.src, a.func, a.obj)
    print("%d functions, %d colouring rounds -> %s%s" % (nf, nr, a.out, "" if obj else "  (COMPILE FAILED)"))
    sys.exit(0 if obj else 1)
