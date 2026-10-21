#!/usr/bin/env python3
import argparse
import bisect
import json
import re
import subprocess
import sys

from common import CONFIG, OBJDUMP, ROOT, SDK_MAPPINGS, SYMBOLS, die, load_json, read_symbols

RELOCS = CONFIG / "relocs.txt"
OBJDIFF = ROOT / "objdiff.json"

WORD_RE = re.compile(r"\w+")
RELOC_RE = re.compile(r"from:0x([0-9a-fA-F]+)\b.*?\bto:0x([0-9a-fA-F]+)\b")


def word_symbols(path=SYMBOLS):
    return [s for s in read_symbols(path) if WORD_RE.fullmatch(s.name)]


def word_functions(path=SYMBOLS):
    return [s for s in word_symbols(path) if s.kind == "function" and s.size is not None]


def read_relocs(path=RELOCS):
    """sorted [(from, to)]"""
    pairs = []
    for line in path.read_text(encoding="utf-8").splitlines():
        m = RELOC_RE.search(line)
        if m:
            pairs.append((int(m.group(1), 16), int(m.group(2), 16)))
    return sorted(pairs)


def parse_symbols():
    """({name: [(addr, size)]}, address-sorted [(addr, name, size)]); a name can repeat (dup statics)."""
    if not SYMBOLS.exists():
        die(f"error: {SYMBOLS} not found", 1)
    by_name = {}
    for s in word_functions():
        by_name.setdefault(s.name, []).append((s.addr, s.size))
    funcs = sorted((a, n, s) for n, lst in by_name.items() for (a, s) in lst)
    return by_name, funcs


def unit_sources():
    if not SDK_MAPPINGS.exists():
        return {}
    data = load_json(SDK_MAPPINGS)
    return {u["name"]: u["source"] for u in data.get("objdiff_units", [])
            if u.get("name") and u.get("source")}


def sdk_units():
    data = load_json(OBJDIFF)
    out = []
    for u in data.get("units", []):
        base = (u.get("base_path") or "").replace("\\", "/")
        if base.startswith("build/lib/"):
            out.append((u.get("name", base), ROOT / base))
    return out


def obj_symbols(opath):
    """({(local name, size)}, {global function names}), or None if the .o is missing."""
    if not opath.exists():
        return None
    try:
        out = subprocess.check_output([OBJDUMP, "-t", str(opath)],
                                      stderr=subprocess.STDOUT).decode("utf-8", "replace")
    except Exception:
        return set(), set()
    local, gfunc = set(), set()
    for line in out.splitlines():
        parts = line.split()
        if len(parts) < 5:
            continue
        name, size, section = parts[-1], parts[-2], parts[-3]
        flags = parts[1:-3]
        if not section.startswith("."):
            continue
        is_fn = "F" in flags
        if not (is_fn or "O" in flags):
            continue
        try:
            sz = int(size, 16)
        except ValueError:
            sz = 0
        if "l" in flags:
            local.add((name, sz))
        elif "g" in flags and is_fn:
            gfunc.add(name)
    return local, gfunc


def build_index(by_name):
    """({static addr: {unit}}, {static addr: name}, {global SDK func: unit}, [unbuilt units])"""
    static_addr2unit, static_addr2name, sdk_gfunc, missing = {}, {}, {}, []
    for unit, opath in sdk_units():
        syms = obj_symbols(opath)
        if syms is None:
            missing.append(unit)
            continue
        local, gfunc = syms
        for n, sz in local:
            cands = by_name.get(n)
            if not cands:
                continue
            if len(cands) == 1:
                addrs = [cands[0][0]]
            else:
                addrs = [a for (a, s) in cands if s == sz] or [a for (a, _) in cands]
            for a in addrs:
                static_addr2unit.setdefault(a, set()).add(unit)
                static_addr2name[a] = n
        for n in gfunc:
            sdk_gfunc[n] = unit
    return static_addr2unit, static_addr2name, sdk_gfunc, missing


def parse_relocs():
    pairs = read_relocs() if RELOCS.exists() else []
    return [p[0] for p in pairs], [p[1] for p in pairs]


class Index:
    def __init__(self):
        self.by_name, self.funcs = parse_symbols()
        (self.static_addr2unit, self.static_addr2name,
         self.sdk_gfunc, self.missing) = build_index(self.by_name)
        self.froms, self.tos = parse_relocs()
        self.usources = unit_sources()

    def classify(self, name):
        """(kind, unit, info): kind in unknown / sdk / interleaved / game."""
        info = {"statics": [], "candidates": [], "source": None, "ambiguous": False}
        if name not in self.by_name:
            info["msg"] = f"{name} is not a function symbol in symbols.txt"
            return ("unknown", None, info)
        if name in self.sdk_gfunc:
            unit = self.sdk_gfunc[name]
            info["source"] = self.usources.get(unit)
            info["msg"] = f"{name} IS a public SDK function (unit '{unit}')"
            return ("sdk", unit, info)

        unit_counts, unit_statics = {}, {}
        for addr, size in self.by_name[name]:
            lo = bisect.bisect_left(self.froms, addr)
            hi = bisect.bisect_left(self.froms, addr + size)
            for to in self.tos[lo:hi]:
                units = self.static_addr2unit.get(to)
                if not units:
                    continue
                sname = self.static_addr2name.get(to, f"0x{to:08x}")
                if len(units) > 1:
                    info["ambiguous"] = True
                for u in units:
                    unit_counts[u] = unit_counts.get(u, 0) + 1
                    unit_statics.setdefault(u, set()).add(sname)

        addr0 = self.by_name[name][0][0]
        if not unit_counts:
            info["msg"] = f"{name} @ 0x{addr0:08x} is standalone game code (no SDK-private refs)"
            return ("game", None, info)
        unit = max(unit_counts, key=unit_counts.get)
        statics = sorted(unit_statics[unit])
        info["statics"] = statics
        info["candidates"] = sorted(unit_counts, key=unit_counts.get, reverse=True)
        info["source"] = self.usources.get(unit)
        amb = ""
        if len(info["candidates"]) > 1:
            amb = f"; candidate units: {', '.join(info['candidates'])}"
        info["msg"] = (
            f"{name} @ 0x{addr0:08x} is INTERLEAVED with SDK unit '{unit}' "
            f"(refs static: {', '.join(statics)}){amb}")
        return ("interleaved", unit, info)

    def hits(self):
        out = []
        for addr, name, _ in self.funcs:
            if not name.startswith("func_"):
                continue
            kind, unit, info = self.classify(name)
            if kind == "interleaved":
                out.append({"func": name, "addr": f"0x{addr:08x}", "unit": unit,
                            "statics": info["statics"], "source": info["source"],
                            "candidates": info["candidates"]})
        return out


def affiliation():
    """{func: SDK unit} for every interleaved func_*, or {} if the index can't be built."""
    try:
        idx = Index()
        return {h["func"]: h["unit"] for h in idx.hits()} if idx.static_addr2unit else {}
    except (Exception, SystemExit):
        return {}


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("func", nargs="?")
    ap.add_argument("--all", action="store_true")
    ap.add_argument("--json", action="store_true")
    args = ap.parse_args()

    idx = Index()
    if idx.missing:
        m = idx.missing
        print(f"warning: {len(m)} SDK unit(s) not built (run `make sdk`): "
              f"{', '.join(m[:4])}{'...' if len(m) > 4 else ''}", file=sys.stderr)
    if not idx.static_addr2unit:
        die("error: no SDK static symbols mapped -- run `make sdk` first", 1)

    if args.all:
        hits = idx.hits()
        if args.json:
            print(json.dumps(hits, indent=2))
        else:
            for h in hits:
                src = f"  [{h['source']}]" if h["source"] else ""
                amb = f"  (ambiguous: {', '.join(h['candidates'])})" if len(h["candidates"]) > 1 else ""
                print(f"{h['func']}  @ {h['addr']}  <- {h['unit']}  "
                      f"via {', '.join(h['statics'])}{src}{amb}")
            print(f"\n{len(hits)} interleaved function(s) -- host each inside a forked "
                  f"copy of its owning SDK source (Option A).")
        return

    if not args.func:
        ap.error("provide a function name or --all")
    kind, unit, info = idx.classify(args.func)
    if args.json:
        print(json.dumps({"func": args.func, "kind": kind, "unit": unit,
                          "statics": info["statics"], "source": info["source"],
                          "candidates": info["candidates"], "ambiguous": info["ambiguous"]}))
    else:
        print(info["msg"])
        if kind == "interleaved":
            if info["source"]:
                print(f"  -> host source: {info['source']}")
            print(f"  -> NOT standalone game code. Run `sdk_equiv.py {args.func}`: if it "
                  f"matches stock, name it from the SDK source; if not, it's a BIS fork -- "
                  f"host it inside a forked copy of the owning SDK file so the static stays "
                  f"in-TU (Option A).")
    sys.exit(1 if kind == "interleaved" else 0)


if __name__ == "__main__":
    main()
