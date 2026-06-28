#!/usr/bin/env python3
"""Detect functions interleaved inside a compiled SDK (NitroSDK/NitroSystem) TU.

A function is "interleaved" when it references the PRIVATE static FUNCTIONS of a
compiled SDK translation unit -- only code that lives inside that TU, or a
per-title fork of it, can touch its file-local statics. These are link-gated
(finishing them cleanly needs the SDK file forked so the static stays in-TU), so
they are the functions to host inside a forked copy of the owning SDK source.

Address-based detection does NOT work in this ROM: the linker scatters and
interleaves TU functions (e.g. sndarc_player and sndarc_stream functions are
mixed together, and MIi_CpuClear32 is relocated far from the rest of mi_memory).
So we use a content signal instead:

  1. For each SDK objdiff unit (base_path under build/lib/), read its LOCAL
     symbols (statics) from the .o and map them to ROM addresses via symbols.txt.
  2. A function is interleaved with unit U if any of its relocations target one
     of U's static symbols. (Calling a PUBLIC SDK API -- a global symbol -- does
     not count; every game function does that.)

Dup-named statics (e.g. the two `BeginSleep`s, in snd/main.c and snd/stream.c)
are disambiguated by SIZE: objdump reports each local's size, and we match it to
the symbols.txt entry with the same size so the reference is attributed to the
correct unit.

LIMITATION: only static FUNCTIONS are detected, not static DATA. SDK data
statics keep generated `data_xxxx` names in symbols.txt that don't bridge to the
SDK object's real symbol names, so a reference to private SDK *data* is not yet
flagged. (TODO: bridge via the data-symbol renames in sdk_mappings.json.)

Usage:
  is_interleaved.py func_020318f0     # check one (exit 1 if interleaved)
  is_interleaved.py --all             # list every interleaved function
  is_interleaved.py --all --json      # machine-readable

Exit: 0 = not interleaved (safe) / list mode; 1 = interleaved.
"""
import argparse
import bisect
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
OBJDUMP = r"C:\msys64\ucrt64\bin\arm-none-eabi-objdump.exe"
SYMBOLS = ROOT / "config" / "arm9" / "symbols.txt"
RELOCS = ROOT / "config" / "arm9" / "relocs.txt"
OBJDIFF = ROOT / "objdiff.json"
SDK_MAPPINGS = ROOT / "config" / "sdk_mappings.json"

SYM_RE = re.compile(
    r"^(?P<name>\w+)\s+kind:function\(.*?size=0x(?P<size>[0-9a-fA-F]+).*?\)\s+addr:0x(?P<addr>[0-9a-fA-F]+)"
)
RELOC_RE = re.compile(r"from:0x([0-9a-fA-F]+)\b.*?\bto:0x([0-9a-fA-F]+)\b")


def parse_symbols():
    """by_name{name: [(addr, size), ...]} -- a name may map to several addresses
    (duplicate statics such as the two `BeginSleep`s). funcs is the flattened,
    address-sorted list."""
    by_name = {}
    if not SYMBOLS.exists():
        sys.exit(f"error: {SYMBOLS} not found")
    for line in SYMBOLS.read_text(encoding="utf-8").splitlines():
        m = SYM_RE.match(line)
        if m:
            by_name.setdefault(m.group("name"), []).append(
                (int(m.group("addr"), 16), int(m.group("size"), 16)))
    funcs = sorted((a, n, s) for n, lst in by_name.items() for (a, s) in lst)
    return by_name, funcs


def unit_sources():
    """objdiff-unit name -> host source path (lib/.../foo.c), from sdk_mappings."""
    out = {}
    if SDK_MAPPINGS.exists():
        data = json.loads(SDK_MAPPINGS.read_text(encoding="utf-8"))
        for u in data.get("objdiff_units", []):
            if u.get("name") and u.get("source"):
                out[u["name"]] = u["source"]
    return out


def sdk_units():
    data = json.loads(OBJDIFF.read_text(encoding="utf-8"))
    out = []
    for u in data.get("units", []):
        base = (u.get("base_path") or "").replace("\\", "/")
        if base.startswith("build/lib/"):
            out.append((u.get("name", base), ROOT / base))
    return out


def obj_symbols(opath):
    """(local{(name, size)}, global_func_names{name}) defined in a .o via objdump -t.
    Local sizes are kept so dup-named statics can be matched to the right addr."""
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
        if not section.startswith("."):           # skip *UND*/*ABS*/filename
            continue
        is_fn, is_obj = "F" in flags, "O" in flags
        if not (is_fn or is_obj):
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
    """static_addr2unit{addr: {unit, ...}}, static_addr2name{addr: name},
    global SDK func names{name: unit}, missing[]."""
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
            else:                                  # dup name -> disambiguate by size
                addrs = [a for (a, s) in cands if s == sz] or [a for (a, _) in cands]
            for a in addrs:
                static_addr2unit.setdefault(a, set()).add(unit)
                static_addr2name[a] = n
        for n in gfunc:
            sdk_gfunc[n] = unit
    return static_addr2unit, static_addr2name, sdk_gfunc, missing


def parse_relocs():
    froms, tos = [], []
    if RELOCS.exists():
        pairs = []
        for line in RELOCS.read_text(encoding="utf-8").splitlines():
            m = RELOC_RE.search(line)
            if m:
                pairs.append((int(m.group(1), 16), int(m.group(2), 16)))
        pairs.sort()
        froms = [p[0] for p in pairs]
        tos = [p[1] for p in pairs]
    return froms, tos


def func_reloc_targets(addr, size, froms, tos):
    lo = bisect.bisect_left(froms, addr)
    hi = bisect.bisect_left(froms, addr + size)
    return tos[lo:hi]


def classify(name, by_name, static_addr2unit, static_addr2name, sdk_gfunc, froms, tos,
             usources):
    """Returns (kind, unit, info) where info carries the referenced static
    symbol(s), the candidate units, the host source file, and a human message."""
    info = {"statics": [], "candidates": [], "source": None, "ambiguous": False}
    if name not in by_name:
        info["msg"] = f"{name} is not a function symbol in symbols.txt"
        return ("unknown", None, info)
    if name in sdk_gfunc:
        unit = sdk_gfunc[name]
        info["source"] = usources.get(unit)
        info["msg"] = f"{name} IS a public SDK function (unit '{unit}')"
        return ("sdk", unit, info)

    unit_counts, unit_statics = {}, {}
    for addr, _size in by_name[name]:
        for to in func_reloc_targets(addr, _size, froms, tos):
            units = static_addr2unit.get(to)
            if not units:
                continue
            sname = static_addr2name.get(to, f"0x{to:08x}")
            if len(units) > 1:
                info["ambiguous"] = True
            for u in units:
                unit_counts[u] = unit_counts.get(u, 0) + 1
                unit_statics.setdefault(u, set()).add(sname)

    addr0 = by_name[name][0][0]
    if unit_counts:
        unit = max(unit_counts, key=unit_counts.get)
        statics = sorted(unit_statics[unit])
        info["statics"] = statics
        info["candidates"] = sorted(unit_counts, key=unit_counts.get, reverse=True)
        info["source"] = usources.get(unit)
        amb = ""
        if len(info["candidates"]) > 1:
            amb = f"; candidate units: {', '.join(info['candidates'])}"
        info["msg"] = (
            f"{name} @ 0x{addr0:08x} is INTERLEAVED with SDK unit '{unit}' "
            f"(refs static: {', '.join(statics)}){amb}")
        return ("interleaved", unit, info)
    info["msg"] = f"{name} @ 0x{addr0:08x} is standalone game code (no SDK-private refs)"
    return ("game", None, info)


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("func", nargs="?")
    ap.add_argument("--all", action="store_true")
    ap.add_argument("--json", action="store_true")
    args = ap.parse_args()

    by_name, funcs = parse_symbols()
    static_addr2unit, static_addr2name, sdk_gfunc, missing = build_index(by_name)
    froms, tos = parse_relocs()
    usources = unit_sources()
    if missing:
        print(f"warning: {len(missing)} SDK unit(s) not built (run `make sdk`): "
              f"{', '.join(missing[:4])}{'...' if len(missing) > 4 else ''}", file=sys.stderr)
    if not static_addr2unit:
        sys.exit("error: no SDK static symbols mapped -- run `make sdk` first")

    if args.all:
        hits = []
        for addr, name, _ in funcs:
            if not name.startswith("func_"):
                continue
            kind, unit, info = classify(name, by_name, static_addr2unit, static_addr2name,
                                        sdk_gfunc, froms, tos, usources)
            if kind == "interleaved":
                hits.append({"func": name, "addr": f"0x{addr:08x}", "unit": unit,
                             "statics": info["statics"], "source": info["source"],
                             "candidates": info["candidates"]})
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
    kind, unit, info = classify(args.func, by_name, static_addr2unit, static_addr2name,
                                sdk_gfunc, froms, tos, usources)
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
