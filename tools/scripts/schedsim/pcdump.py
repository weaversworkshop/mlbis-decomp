import ctypes, json, struct, subprocess, sys
from ctypes import wintypes as W
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import cc  # noqa: E402

BP1 = 0x4ff6d0   # after build_deps of a block: `mov ecx, [0x6166ba]` (6 bytes)
BP2 = 0x4ffa8d   # after schedule_block returns: `pop ecx`, ebx = block
k32 = ctypes.WinDLL("kernel32", use_last_error=True)


class SI(ctypes.Structure):
    _fields_ = [("cb", W.DWORD), ("a", W.LPWSTR), ("b", W.LPWSTR), ("c", W.LPWSTR)] + [(n, W.DWORD) for n in "defghijk"] + \
               [("l", W.WORD), ("m", W.WORD), ("n", ctypes.c_void_p), ("o", W.HANDLE), ("p", W.HANDLE), ("q", W.HANDLE)]


class PI(ctypes.Structure):
    _fields_ = [("hProcess", W.HANDLE), ("hThread", W.HANDLE), ("pid", W.DWORD), ("tid", W.DWORD)]


class DE(ctypes.Structure):
    _fields_ = [("code", W.DWORD), ("pid", W.DWORD), ("tid", W.DWORD), ("pad", W.DWORD), ("u", ctypes.c_byte * 176)]


k32.ReadProcessMemory.argtypes = [W.HANDLE, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_size_t, ctypes.c_void_p]
k32.WriteProcessMemory.argtypes = [W.HANDLE, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_size_t, ctypes.c_void_p]
k32.OpenThread.restype = W.HANDLE
k32.Wow64GetThreadContext.argtypes = [W.HANDLE, ctypes.c_void_p]
k32.Wow64SetThreadContext.argtypes = [W.HANDLE, ctypes.c_void_p]
k32.FlushInstructionCache.argtypes = [W.HANDLE, ctypes.c_void_p, ctypes.c_size_t]


def run(cmdline, cwd, env, on_record):
    """run mwccarm under a debugger; on_record(rec) is called once per scheduled block"""
    si = SI(); si.cb = ctypes.sizeof(SI); pi = PI()
    envb = "".join("%s=%s\0" % kv for kv in env.items()) + "\0"
    if not k32.CreateProcessW(None, ctypes.create_unicode_buffer(cmdline), None, None, False,
                              2 | 0x400, ctypes.create_unicode_buffer(envb), str(cwd), ctypes.byref(si), ctypes.byref(pi)):
        raise ctypes.WinError(ctypes.get_last_error())
    hp = pi.hProcess

    def rd(a, n):
        b = ctypes.create_string_buffer(n)
        if not k32.ReadProcessMemory(hp, a, b, n, None):
            return b"\0" * n
        return b.raw

    def u32(a):
        return struct.unpack("<I", rd(a, 4))[0]

    def instr(p, aliases, nwords):
        raw = rd(p, 0x2e)
        fl, fl2 = struct.unpack_from("<II", raw, 0xc)
        al = struct.unpack_from("<I", raw, 0x1c)[0]
        op, cond, form, nops = struct.unpack_from("<hBBh", raw, 0x28)
        ops = []
        o = rd(p + 0x2e, 0xe * max(nops, 0))
        for i in range(nops):
            kind, _, f = struct.unpack_from("<BbH", o, i * 0xe)
            reg = struct.unpack_from("<h", o, i * 0xe + 4)[0]
            imm = struct.unpack_from("<i", o, i * 0xe + 2)[0]
            ops.append([kind, f, reg if kind in (0, 1) else imm])
        if al and al not in aliases:
            ar = rd(al, 0x34)
            vp = struct.unpack_from("<I", ar, 0x28)[0]
            aliases[al] = {"type": ar[0x30], "obj": struct.unpack_from("<I", ar, 0x14)[0], "off": struct.unpack_from("<i", ar, 0x18)[0],
                           "size": struct.unpack_from("<i", ar, 0x1c)[0], "idx": struct.unpack_from("<i", ar, 0x2c)[0],
                           "vec": rd(vp, 4 * nwords).hex() if vp else ""}
        return {"p": p, "op": op, "cond": cond, "form": form, "flags": fl, "flags2": fl2, "alias": al, "ops": ops}

    def dump():
        nodes, aliases = [], {}
        nwords = min((u32(0x62e644) + 31) >> 5, 4096)
        worst = u32(0x63be04)
        if worst:
            ar = rd(worst, 0x34)
            vp = struct.unpack_from("<I", ar, 0x28)[0]
            aliases[worst] = {"type": ar[0x30], "obj": 0, "off": 0, "size": 0, "idx": struct.unpack_from("<i", ar, 0x2c)[0],
                              "vec": rd(vp, 4 * nwords).hex() if vp else ""}
        n = u32(0x6166ba)
        ptrs = []
        while n:
            raw = rd(n, 0x1c)
            nxt, _, succ, ip = struct.unpack_from("<IIII", raw, 0)
            L, desc, _, _, h, np_ = struct.unpack_from("<HHHHHH", raw, 0x10)
            es = []
            e = succ
            while e:
                en, to, lat = struct.unpack("<IIH", rd(e, 10))
                es.append([to, lat])
                e = en
            ptrs.append(n)
            nodes.append({"instr": instr(ip, aliases, nwords), "L": L, "desc": desc, "h": h, "npred": np_, "succ": es})
            n = nxt
        ucb = u32(0x616678)
        regs = sorted(set(o[2] for nd in nodes for o in nd["instr"]["ops"] if o[0] == 0))
        uc4 = {str(r): struct.unpack("<i", rd(ucb + 4 * r, 4))[0] for r in regs} if ucb else {}
        uc2 = {str(r): struct.unpack("<h", rd(ucb + 2 * r, 2))[0] for r in regs} if ucb else {}
        idx = {p: k for k, p in enumerate(ptrs)}
        for nd in nodes:
            nd["succ"] = [[idx[t], l] for t, l in nd["succ"]]
        return {"vregs": u32(0x63a36c), "worst": worst, "avail": struct.unpack("<i", rd(0x61667c, 4))[0],
                "live": struct.unpack("<i", rd(0x616680, 4))[0], "uc4": uc4, "uc2": uc2, "rstate": list(rd(0x6398e0, 16)), "nodes": nodes, "aliases": {str(k): v for k, v in aliases.items()}}

    def result(block, rec):
        pos = {nd["instr"]["p"]: k for k, nd in enumerate(rec["nodes"])}
        out = []
        p = u32(block + 0x14)
        while p:
            out.append(pos.get(p, -1))
            p = u32(p)
        rec["out"] = out

    ev = DE(); ctx = ctypes.create_string_buffer(716); pending = None
    while True:
        if not k32.WaitForDebugEvent(ctypes.byref(ev), 0xFFFFFFFF):
            break
        cont = 0x00010002
        if ev.code == 3:  # CREATE_PROCESS
            for bp in (BP1, BP2):
                k32.WriteProcessMemory(hp, bp, b"\xcc", 1, None); k32.FlushInstructionCache(hp, bp, 1)
        elif ev.code == 1:
            exc = struct.unpack_from("<I", bytes(ev.u), 0)[0]
            addr = struct.unpack_from("<Q", bytes(ev.u), 16)[0]
            if exc in (0x80000003, 0x4000001f):
                if addr in (BP1, BP2):
                    ht = k32.OpenThread(0x1FFFFF, False, ev.tid)
                    struct.pack_into("<I", ctx, 0, 0x10007)
                    k32.Wow64GetThreadContext(ht, ctx)
                    if addr == BP1:
                        pending = dump()
                        struct.pack_into("<I", ctx, 0xac, u32(0x6166ba))   # ecx
                        struct.pack_into("<I", ctx, 0xb8, BP1 + 6)         # eip
                    else:
                        ebx, esp = struct.unpack_from("<I", ctx, 0xa4)[0], struct.unpack_from("<I", ctx, 0xc4)[0]
                        if pending is not None:
                            result(ebx, pending)
                            on_record(pending)
                            pending = None
                        struct.pack_into("<I", ctx, 0xac, u32(esp))        # pop ecx
                        struct.pack_into("<I", ctx, 0xc4, esp + 4)
                        struct.pack_into("<I", ctx, 0xb8, BP2 + 1)
                    k32.Wow64SetThreadContext(ht, ctx)
                    k32.CloseHandle(ht)
            else:
                cont = 0x80010001
        elif ev.code == 5:  # EXIT_PROCESS
            k32.ContinueDebugEvent(ev.pid, ev.tid, cont)
            break
        k32.ContinueDebugEvent(ev.pid, ev.tid, cont)


def dump_tu(tu, variant, on_record, src=None):
    obj = cc.ROOT / "build/schedsim/obj" / variant / (Path(tu).stem + ".o")
    obj.parent.mkdir(parents=True, exist_ok=True)
    run(subprocess.list2cmdline(cc.command(tu, variant, obj, src)), cc.ROOT, cc.ENV, on_record)
    return obj


if __name__ == "__main__":
    import argparse
    ap = argparse.ArgumentParser()
    ap.add_argument("tu"); ap.add_argument("out")
    ap.add_argument("--variant", default="stock"); ap.add_argument("--src")
    a = ap.parse_args()
    with open(a.out, "w") as f:
        n = [0]

        def rec(r):
            n[0] += 1
            f.write(json.dumps(r) + "\n")
        dump_tu(a.tu, a.variant, rec, a.src)
    print("%d blocks -> %s" % (n[0], a.out))
