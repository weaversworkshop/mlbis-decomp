#!/usr/bin/env python3
import argparse
import json
import re
import subprocess
import sys
from bisect import bisect_left
from collections import defaultdict

import obj_match as om
from common import CONFIG, ROOT, SDK_MAPPINGS, load_json, read_symbols

ARCHIVES = {
    "mathlib": "lib/mw-libraries/ARM_EABI_Support/Mathlib/lib_ds_2.0_sp1/FP_fastI_v5t_LE.a",
}
CAND_DIRS = ["build/lib", "build/prebuilt"]
PLACEHOLDER = re.compile(r"^(func|data)_(ov\d+_)?[0-9a-f]{8}$|^\.L_")
LOCAL = re.compile(r"^@\d+$|\$\d+$|^const 0x|^__\w+__$|^SDK_")
IDENT = re.compile(r"^[A-Za-z_]\w*$")


def is_placeholder(name):
    return bool(PLACEHOLDER.match(name))


def load_candidates():
    for name, ar in ARCHIVES.items():
        out = ROOT / "build/prebuilt" / name
        out.mkdir(parents=True, exist_ok=True)
        subprocess.run(["arm-none-eabi-ar", "x", f"--output={out}", str(ROOT / ar)], check=True)
    objs = {}
    for d in CAND_DIRS:
        for p in sorted((ROOT / d).rglob("*.o")):
            try:
                e = om.Elf(p)
            except SystemExit:
                continue
            fns = sorted(((s["shndx"], s["value"], n, om.tokenize(e, s))
                          for n, syms in e.functions().items() for s in syms))
            if fns:
                objs[p.relative_to(ROOT).as_posix()] = [(n, t) for _, _, n, t in fns]
    return objs


def target_functions(tgt, module):
    """address-sorted [(addr, name, sym)]; section bases come from the module's symbols.txt"""
    path = CONFIG / "symbols.txt" if module == "main" else CONFIG / module / "symbols.txt"
    known = defaultdict(list)
    for s in read_symbols(path):
        known[s.name].append(s.addr)
    funcs = [(n, s) for n, syms in tgt.functions().items() for s in syms]
    base = {}
    for n, s in funcs:
        if len(known[n]) == 1 and len(tgt.functions()[n]) == 1:
            base.setdefault(s["shndx"], known[n][0] - s["value"])
    return sorted(((s["value"] + base[s["shndx"]], n, s) for n, s in funcs if s["shndx"] in base),
                  key=lambda t: t[0])


def gaps_ok(gaps, renames):
    for tname, sname in gaps:
        if LOCAL.search(sname) or LOCAL.search(tname):
            continue
        if not is_placeholder(tname):
            return False
        if tname.startswith("func_") and renames.get(tname, sname) != sname:
            return False
    return True


def library_of(obj):
    if "/NitroSDK/libraries/" in obj:
        return obj.split("/libraries/")[1].split("/")[0]
    if "/NitroSystem/libraries/" in obj:
        return "nns"
    return "msl"


def unit_for(obj, target):
    stem = obj.rsplit("/", 1)[1][:-2]
    if obj.startswith("build/prebuilt/"):
        return {"prebuilt": obj[len("build/prebuilt/"):], "target": target, "name": "msl/runtime/" + stem}
    src = obj[len("build/"):-2] + ".c"
    if not (ROOT / src).is_file():
        return {"object": obj, "target": target, "name": "msl/runtime/" + stem}
    lib = library_of(obj)
    prefix = {"nns": "nns/" + obj.split("/libraries/")[1].split("/")[0], "msl": "msl"}.get(lib, "sdk/" + lib)
    return {"source": src, "target": target, "name": f"{prefix}/{stem}"}


def unit_base(u):
    if "prebuilt" in u:
        return "build/prebuilt/" + u["prebuilt"]
    return u.get("object") or "build/" + u["source"][:-2] + ".o"


def apply(funcs, data, target):
    cfg = load_json(SDK_MAPPINGS)
    have = {m["addr"].lower() for m in cfg["mappings"]}
    bases = {(unit_base(u), u["target"]) for u in cfg["objdiff_units"]}
    for addr, (obj, name, _) in sorted(funcs.items()):
        if f"{addr:#010x}" in have:
            continue
        cfg["mappings"].append({"addr": f"{addr:#010x}", "sdk_name": name,
                                "library": library_of(obj), "confidence": "certain"})
        if (obj, target) not in bases:
            bases.add((obj, target))
            unit = unit_for(obj, target)
            if any(u["name"] == unit["name"] for u in cfg["objdiff_units"]):
                unit["name"] += "@" + target.rsplit("@", 1)[1][:-2]
            cfg["objdiff_units"].append(unit)
    for d, name in sorted(data.items()):
        addr = "0x" + d[len("data_"):]
        if addr not in have:
            cfg["mappings"].append({"addr": addr, "sdk_name": name,
                                    "library": "data", "confidence": "certain"})
    SDK_MAPPINGS.write_text(json.dumps(cfg, indent=2, ensure_ascii=False) + "\n",
                            encoding="utf-8", newline="\r\n")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--gap", default="main_7")
    ap.add_argument("--lo", default="0")
    ap.add_argument("--apply", action="store_true")
    ap.add_argument("-v", action="store_true")
    args = ap.parse_args()
    target = f"build/delinks/_dsd_gap@{args.gap}.o"

    tgt = om.Elf(ROOT / target)
    tfuncs = [t for t in target_functions(tgt, args.gap.rsplit("_", 1)[0]) if t[0] >= int(args.lo, 16)]
    addrs = [t[0] for t in tfuncs]
    ttoks = {a: om.tokenize(tgt, s) for a, n, s in tfuncs}
    tnames = {n for _, n, _ in tfuncs} | set(tgt.data_objects())
    objs = load_candidates()

    # owner[addr] = (obj, index) for functions already scored by a unit
    owner, renames, votes = {}, {}, defaultdict(set)
    for u in load_json(ROOT / "objdiff.json")["units"]:
        fns = objs.get(u["base_path"])
        if not fns or u["target_path"] != target:
            continue
        names = {}
        for i, (n, _) in enumerate(fns):
            names.setdefault(n, i)
        for a, n, _ in tfuncs:
            if n in names and not is_placeholder(n) and a not in owner:
                btoks = fns[names[n]][1]
                pct, gaps = om.compare(ttoks[a], btoks)
                if pct != 100.0:
                    continue
                owner[a] = (u["base_path"], names[n])
                for tname, sname in gaps:
                    if tname.startswith("func_") and not LOCAL.search(sname):
                        renames.setdefault(tname, sname)
                for (_, tk), (_, bk) in zip(ttoks[a], btoks):
                    if (tk and bk and tk[0] == bk[0] == "abs32" and tk[2] == bk[2] == 0
                            and tk[1].startswith("data_") and IDENT.match(bk[1])
                            and not LOCAL.search(bk[1])):
                        votes[tk[1]].add(bk[1])

    bylen = defaultdict(list)
    for o, fns in objs.items():
        for i, (n, t) in enumerate(fns):
            bylen[len(t)].append((o, i))

    cands = {}
    for a, n, _ in tfuncs:
        if a in owner or not is_placeholder(n):
            continue
        cands[a] = [(o, i, gaps) for o, i in bylen[len(ttoks[a])]
                    for pct, gaps in [om.compare(ttoks[a], objs[o][i][1])] if pct == 100.0]

    name_of = {a: n for a, n, _ in tfuncs}
    accepted, taken = {}, set(owner.values())

    def fits(a, o, i):
        """None = contradicts object order; True = the neighbouring mapped function is in o"""
        k = bisect_left(addrs, a)
        near = []
        for rng, bad in ((range(k - 1, -1, -1), lambda w: w[1] >= i),
                         (range(k + 1, len(addrs)), lambda w: w[1] <= i)):
            w = next((owner[addrs[j]] for j in rng if addrs[j] in owner), None)
            if w and w[0] == o and bad(w):
                return None
            near.append(w)
        return any(w and w[0] == o for w in near)

    changed = True
    while changed:
        changed = False
        for a in sorted(cands):
            if a in owner:
                continue
            live = []
            for o, i, gaps in cands[a]:
                if (o, i) in taken or not gaps_ok(gaps, renames):
                    continue
                f = fits(a, o, i)
                if f is not None:
                    live.append((f, o, i, gaps))
            adj = [c for c in live if c[0]]
            want = renames.get(name_of[a])
            called = [c for c in live if objs[c[1]][c[2]][0] == want]
            if len(called) > 1:
                called = [c for c in called if c[0]]
            pick = (called if len(called) == 1 else adj if len(adj) == 1
                    else live if len(live) == 1 and not adj else None)
            if not pick:
                continue
            _, o, i, gaps = pick[0]
            owner[a] = (o, i)
            taken.add((o, i))
            accepted[a] = (o, objs[o][i][0],
                           "called" if pick is called else "adjacent" if pick is adj else "unique")
            renames[name_of[a]] = objs[o][i][0]
            for tname, sname in gaps:
                if tname.startswith("func_") and not LOCAL.search(sname):
                    renames.setdefault(tname, sname)
            changed = True

    for a in sorted(cands):
        if a in accepted:
            o, n, why = accepted[a]
            print(f"{a:#010x} {n:40s} {why:8s} {o}")
        elif args.v:
            c = sorted({(objs[o][i][0], o.rsplit('/', 1)[-1]) for o, i, _ in cands[a]})
            print(f"{a:#010x} -- {len(c)} cand: {c[:4]}")

    # data: one source name per data_x, one data_x per name, name not already in the target
    single = {d: next(iter(ns)) for d, ns in votes.items() if len(ns) == 1}
    count = defaultdict(int)
    for n in single.values():
        count[n] += 1
    data = {d: n for d, n in single.items() if count[n] == 1 and n not in tnames}
    for d, n in sorted(data.items()):
        print(f"{d} -> {n}")
    if args.v:
        for d, ns in sorted(votes.items()):
            if d not in data:
                print(f"{d} -- skipped: {sorted(ns)}")

    print(f"{len(accepted)} functions / {len(cands)} unassigned, {len(data)} data", file=sys.stderr)
    if args.apply:
        apply(accepted, data, target)


if __name__ == "__main__":
    main()
