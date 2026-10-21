#!/usr/bin/env python3
import argparse
import bisect
import json
import re
import subprocess
from collections import Counter, defaultdict
from pathlib import Path

from common import BUILD, OBJDUMP, ROOT, SYMBOLS, die, load_json, read_symbols
from common import GAP_O as ROM_O, SDK_MAPPINGS as CFG_PATH

SDK_O_ROOTS = [BUILD / "lib" / "NitroSDK", BUILD / "lib" / "NitroSystem"]

HDR = re.compile(r"^[0-9a-f]+ <([^>]+)>:")
INSN = re.compile(r"^\s+([0-9a-f]+):\s+[0-9a-f ]+?\t(.*)$")
# also catches data lines objdump renders space-separated (no tab)
ROM_LINE = re.compile(r"^\s+([0-9a-f]+):\s+[0-9a-f]{2,8}(?: [0-9a-f]{2,8})*\s+(.+?)\s*$")
RELOC = re.compile(r"^\s+([0-9a-f]+):\s+R_ARM_\S+\s+(\S+)")
ADDEND = re.compile(r"^(.*?)([+-]0x[0-9a-f]+)$")
FUNC_NAME = re.compile(r"func_[0-9a-f]+")
SYM_TEXT = re.compile(r"([0-9a-f]+)\s+\S+\s+F\s+\.text\s+([0-9a-f]+)\s+(\S+)\s*$")


def objdump(flag, obj):
    try:
        return subprocess.run([OBJDUMP, flag, str(obj)], capture_output=True, text=True).stdout
    except Exception:
        return None


def normalize(body):
    out = []
    for t in body:
        t = re.sub(r"\s+;.*$", "", t)
        t = re.sub(r"\s+@.*$", "", t)
        t = re.sub(r"<[^>]*>", "<t>", t)
        t = re.sub(r"\b[0-9a-f]+ <t>", "<t>", t)
        t = re.sub(r"\[pc, #-?\w+\]", "[pc]", t)
        t = re.sub(r"\[pc, #-?\w+\]!", "[pc]", t)
        if t.startswith((".word", ".short", ".byte", "....")):
            t = ".word"
        out.append(t.strip())
    return tuple(out)


def disasm_funcs(obj):
    """{func: (normalized, n_insns)}"""
    txt = objdump("-d", obj)
    if txt is None:
        return {}
    funcs, cur, body = {}, None, []
    for ln in txt.splitlines():
        h = HDR.match(ln)
        if h:
            if cur:
                funcs[cur] = (normalize(body), len(body))
            cur, body = h.group(1), []
            continue
        m = INSN.match(ln)
        if m and cur:
            body.append(m.group(2).strip())
    if cur:
        funcs[cur] = (normalize(body), len(body))
    return funcs


def disasm_rom(rom_o):
    """Buckets by symbol-table [addr, addr+size) so pool labels don't truncate a function."""
    tt, dd = objdump("-t", rom_o), objdump("-d", rom_o)
    if tt is None or dd is None:
        return {}
    ranges = []
    for ln in tt.splitlines():
        m = SYM_TEXT.match(ln)
        if m:
            a, sz = int(m.group(1), 16), int(m.group(2), 16)
            ranges.append((a, a + sz, m.group(3)))
    ranges.sort()
    starts = [r[0] for r in ranges]
    bodies = {}
    for ln in dd.splitlines():
        m = ROM_LINE.match(ln)
        if not m:
            continue
        a = int(m.group(1), 16)
        i = bisect.bisect_right(starts, a) - 1
        if i >= 0 and a < ranges[i][1]:
            bodies.setdefault(ranges[i][2], []).append(m.group(2).strip())
    return {n: (normalize(b), len(b)) for n, b in bodies.items()}


def func_sizes(obj):
    txt = objdump("-t", obj)
    if txt is None:
        return {}
    out = {}
    for ln in txt.splitlines():
        m = SYM_TEXT.match(ln)
        if m:
            out[m.group(3)] = int(m.group(2), 16)
    return out


def load_symbols():
    """(addr_of, size_of, addrs of already-named functions)"""
    addr_of, size_of, mapped_named = {}, {}, set()
    for s in read_symbols():
        if s.kind != "function" or s.size is None:
            continue
        addr_of[s.name] = s.addr
        size_of[s.name] = s.size
        if not FUNC_NAME.fullmatch(s.name):
            mapped_named.add(s.addr)
    return addr_of, size_of, mapped_named


def sdk_objects():
    """[(obj, source .c path as make spells it)]"""
    objs = []
    for r in SDK_O_ROOTS:
        objs += sorted(r.rglob("*.o")) if r.exists() else []
    return [(o, o.relative_to(BUILD).as_posix().replace(".o", ".c")) for o in objs]


def lib_of(s):
    return s.split("/libraries/")[1].split("/")[0] if "/libraries/" in s else "sdk"


def unit_of(s):
    return ("nns" if "NitroSystem" in s else "sdk") + "/" + lib_of(s) + "/" + Path(s).stem


def apply_proposals(rows):
    """rows: [(addr_int, sdk_name, src)] in insertion order -> (mappings added, units added)"""
    cfg = load_json(CFG_PATH)
    have_a = {e["addr"].lower() for e in cfg["mappings"]}
    have_s = {u["source"] for u in cfg["objdiff_units"]}
    nm = nu = 0
    for a, name, src in rows:
        ha = f"0x{a:08x}"
        if ha.lower() not in have_a:
            cfg["mappings"].append({"addr": ha, "sdk_name": name,
                                    "library": lib_of(src), "confidence": "certain"})
            have_a.add(ha.lower()); nm += 1
        if src not in have_s:
            cfg["objdiff_units"].append({"source": src,
                                         "target": "build/delinks/_dsd_gap@main_7.o",
                                         "name": unit_of(src)})
            have_s.add(src); nu += 1
    CFG_PATH.write_text(json.dumps(cfg, indent=2) + "\n", encoding="utf-8")
    return nm, nu


def match(args):
    addr_of, size_of, mapped = load_symbols()
    rom_index = {}
    for name, (body, n) in disasm_rom(ROM_O).items():
        if FUNC_NAME.fullmatch(name) and addr_of.get(name) not in mapped:
            rom_index.setdefault(body, []).append(name)

    by_src = {}
    for o, src in sdk_objects():
        sizes = func_sizes(o)
        for sname, (body, n) in disasm_funcs(o).items():
            if n == 0:
                continue
            # exact byte size too: rejects shape-only matches (OS_Init)
            cands = [rf for rf in rom_index.get(body, []) if sizes.get(sname) == size_of.get(rf)]
            if len(cands) == 1:
                by_src.setdefault(src, []).append((sname, cands[0], addr_of[cands[0]], n, False))
            elif len(cands) > 1:
                by_src.setdefault(src, []).append((sname, None, None, n, True))

    proposals = []
    print("=== SDK functions found in ROM (proposed mappings) ===\n")
    for src in sorted(by_src):
        rows = sorted((r for r in by_src[src] if r[1]), key=lambda r: r[2])
        if not rows:
            continue
        span = f"0x{rows[0][2]:08x}..0x{rows[-1][2]:08x}"
        contig = len(rows) >= 2
        amb = sum(1 for r in by_src[src] if r[4])
        print(f"  {src}   [{len(rows)} match, span {span}{', CONTIGUOUS' if contig else ''}"
              f"{f', {amb} ambiguous skipped' if amb else ''}]")
        for sname, rf, a, n, _ in rows:
            ok = contig or n >= args.min_insns
            flag = "" if ok else "  (tiny/uncertain -- review)"
            print(f"      0x{a:08x}  {rf:18} -> {sname}{flag}")
            if ok:
                proposals.append((a, sname, src))
    ac = Counter(p[0] for p in proposals)
    collisions = sorted({a for a, c in ac.items() if c > 1})
    proposals = [p for p in proposals if ac[p[0]] == 1]
    if collisions:
        print(f"\n  excluded {len(collisions)} ROM-address collisions: "
              f"{[hex(a) for a in collisions]}")
    print(f"  TOTAL clean proposals: {len(proposals)} mappings across {len(by_src)} files")

    if args.json:
        print("\n=== JSON (review, then merge into config/sdk_mappings.json) ===")
        for a, name, src in sorted(proposals):
            print(f'    {{ "addr": "0x{a:08x}", "sdk_name": "{name}", '
                  f'"library": "{lib_of(src)}", "confidence": "certain" }},')

    if args.apply:
        nm, nu = apply_proposals(sorted(proposals))
        print(f"\n  APPLIED: +{nm} mappings, +{nu} units -> {CFG_PATH.relative_to(ROOT)}")


def sym_addr(sym, addr_of):
    m = re.fullmatch(r"(?:data|func)_([0-9a-f]+)", sym)
    if m:
        return int(m.group(1), 16)
    return addr_of.get(sym)


def disasm_reloc(obj):
    """{func: {'body': instructions-only normalized, 'relocs': [(sym, addend)]}}"""
    txt = objdump("-dr", obj)
    if txt is None:
        return {}
    funcs, cur, body, relocs = {}, None, [], []
    for ln in txt.splitlines():
        h = HDR.match(ln)
        if h:
            name = h.group(1)
            if name.startswith((".L", "data_")) or "+" in name:
                continue
            if cur:
                funcs[cur] = {"body": normalize(body), "relocs": relocs}
            cur, body, relocs = name, [], []
            continue
        if cur is None:
            continue
        s = ROM_LINE.match(ln)
        if s:
            t = s.group(2).strip()
            if normalize([t])[0] != ".word":
                body.append(t)
            continue
        r = RELOC.match(ln)
        if r:
            sym, add = r.group(2), 0
            m = ADDEND.match(sym)
            if m:
                sym, add = m.group(1), int(m.group(2), 16)
            relocs.append((sym, add))
    if cur:
        funcs[cur] = {"body": normalize(body), "relocs": relocs}
    return funcs


def disambiguate(args):
    syms = read_symbols()
    addr_of = {s.name: s.addr for s in syms}
    mapped_named = {s.name for s in syms
                    if FUNC_NAME.fullmatch(s.name) is None and not s.name.startswith("data_")}

    rom = disasm_reloc(ROM_O)
    sdk = {}
    for o, src in sdk_objects():
        for name, info in disasm_reloc(o).items():
            if info["body"]:
                sdk[name] = {**info, "src": src}

    # base(S) = ROM target of the twin reloc - SDK addend, voted across already-mapped funcs
    votes = defaultdict(lambda: defaultdict(int))
    for name, info in sdk.items():
        if name not in mapped_named or name not in rom:
            continue
        sr, rr = info["relocs"], rom[name]["relocs"]
        if len(sr) != len(rr):
            continue
        for (ssym, sadd), (rsym, radd) in zip(sr, rr):
            ta = sym_addr(rsym, addr_of)
            if ta is not None:
                votes[ssym][(ta + radd) - sadd] += 1
    base_rom = {sym: max(v.items(), key=lambda kv: kv[1])[0] for sym, v in votes.items()}

    by_body = defaultdict(list)
    for name, info in sdk.items():
        by_body[info["body"]].append(name)

    def reloc_match(sr, rr):
        if len(sr) != len(rr):
            return False
        for (ssym, sadd), (rsym, radd) in zip(sr, rr):
            if rsym == ssym:
                if radd != sadd:
                    return False
                continue
            ta = sym_addr(rsym, addr_of)
            if not (ssym in base_rom and ta is not None and base_rom[ssym] + sadd == ta + radd):
                return False
        return True

    proposals = []
    for rf, info in rom.items():
        if not FUNC_NAME.fullmatch(rf):
            continue
        cands = by_body.get(info["body"], [])
        if len(cands) < 2:
            continue
        hits = sorted({s for s in cands if reloc_match(sdk[s]["relocs"], info["relocs"])})
        if len(hits) == 1:
            proposals.append((rf, hits[0], sdk[hits[0]]["src"]))

    ac = Counter(p[0] for p in proposals)
    proposals = [p for p in proposals if ac[p[0]] == 1]
    nc = Counter(p[1] for p in proposals)
    proposals = [p for p in proposals if nc[p[1]] == 1]

    print(f"=== reloc-aware disambiguation: {len(proposals)} proposals "
          f"({len(base_rom)} base symbols resolved) ===")
    for rf, name, src in sorted(proposals):
        print(f"  {rf}  -> {name}   [{Path(src).stem}]")

    if args.apply and proposals:
        nm, nu = apply_proposals([(addr_of[rf], n, s) for rf, n, s in sorted(proposals)])
        print(f"\n  APPLIED: +{nm} mappings, +{nu} units")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--min-insns", type=int, default=3,
                    help="ignore functions smaller than this unless part of a >=2 contiguous run")
    ap.add_argument("--json", action="store_true", help="emit sdk_mappings.json entries")
    ap.add_argument("--apply", action="store_true",
                    help="merge clean proposals (mappings + units) into config/sdk_mappings.json")
    ap.add_argument("--disambiguate", action="store_true",
                    help="map byte-identical SDK siblings by their distinguishing reloc addend")
    args = ap.parse_args()

    if not ROM_O.exists():
        die(f"missing {ROM_O} (run `make delinks`)", 1)
    if args.disambiguate:
        disambiguate(args)
    else:
        match(args)


if __name__ == "__main__":
    main()
