#!/usr/bin/env python3
"""Disambiguate SDK functions that sdk_match leaves AMBIGUOUS because they are
byte-identical except for a literal-pool RELOCATION that differs only by addend
(e.g. every GX_ResetBankForXxx is `bx resetBankForX_` + a `.word gGXState+N`, with N
the only difference).

sdk_match collapses every literal word to `.word`, so those functions look identical.
This pass is relocation-aware: it parses `objdump -dr`, and for each collision group it
resolves the shared base symbol's ROM address from an ALREADY-MAPPED anchor function that
references the same base, then matches each collided ROM function by its unique addend.

No contiguity / source-order / dead-strip assumption -- each function self-identifies by
the absolute address its distinguishing relocation points at.

  python tools/scripts/sdk_disambiguate.py          # report
  python tools/scripts/sdk_disambiguate.py --apply   # merge into config/sdk_mappings.json
"""
import argparse
import json
import re
import subprocess
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import sdk_match  # reuse normalize(), func_sizes(), paths

ROOT = sdk_match.ROOT
OBJDUMP = sdk_match.OBJDUMP
ROM_O = sdk_match.ROM_O
SDK_O_ROOTS = sdk_match.SDK_O_ROOTS
SYMBOLS = sdk_match.SYMBOLS

HDR = re.compile(r"^[0-9a-f]+ <([^>]+)>:")
SLOT = sdk_match.ROM_LINE                       # addr, text
RELOC = re.compile(r"^\s+([0-9a-f]+):\s+R_ARM_\S+\s+(\S+)")
ADDEND = re.compile(r"^(.*?)([+-]0x[0-9a-f]+)$")


def sym_addr(sym, addr_of):
    """Resolve a relocation target symbol to an absolute address."""
    m = re.fullmatch(r"(?:data|func)_([0-9a-f]+)", sym)
    if m:
        return int(m.group(1), 16)
    return addr_of.get(sym)


def _is_label(name):
    """A literal-pool / local label, NOT a real function boundary."""
    return name.startswith(".L") or name.startswith("data_") or "+" in name


def disasm_reloc(obj):
    """obj -> {func: {'body': instr-only normtuple, 'relocs': [(sym, addend), ...]}}.

    `body` is INSTRUCTIONS ONLY (data/literal words dropped) so it groups identically
    across the compiled object (literals shown as `...`) and the delinked ROM (literals
    shown as `.L_*` `.word` lines). `relocs` is the function's ordered relocation list
    (across both call relocs and literal-pool data relocs)."""
    try:
        txt = subprocess.run([OBJDUMP, "-dr", str(obj)], capture_output=True, text=True).stdout
    except Exception:
        return {}
    funcs, cur, body, relocs = {}, None, [], []
    for ln in txt.splitlines():
        h = HDR.match(ln)
        if h:
            if _is_label(h.group(1)):
                continue                       # literal-pool label stays in current func
            if cur:
                funcs[cur] = {"body": sdk_match.normalize(body), "relocs": relocs}
            cur, body, relocs = h.group(1), [], []
            continue
        if cur is None:
            continue
        s = SLOT.match(ln)
        if s:
            t = s.group(2).strip()
            if sdk_match.normalize([t])[0] != ".word":   # keep instructions, drop data words
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
        funcs[cur] = {"body": sdk_match.normalize(body), "relocs": relocs}
    return funcs


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--apply", action="store_true")
    args = ap.parse_args()

    # all symbols (functions AND data) -> address; plus mapped-named function set
    addr_of, mapped_named = {}, set()
    for ln in SYMBOLS.read_text(encoding="utf-8", errors="replace").splitlines():
        m = re.match(r"(\S+)\s+kind:\S+[^)]*\)?\s*addr:(0x[0-9a-fA-F]+)", ln)
        if not m:
            m = re.match(r"(\S+)\s+kind:\S+\s+addr:(0x[0-9a-fA-F]+)", ln)
        if not m:
            continue
        name, a = m.group(1), int(m.group(2), 16)
        addr_of[name] = a
        if re.fullmatch(r"func_[0-9a-f]+", name) is None and not name.startswith("data_"):
            mapped_named.add(name)

    rom = disasm_reloc(ROM_O)

    sdk_objs = []
    for r in SDK_O_ROOTS:
        sdk_objs += sorted(r.rglob("*.o")) if r.exists() else []

    # gather all compiled SDK funcs (with relocs + source)
    sdk = {}
    for o in sdk_objs:
        src = o.relative_to(ROOT / "build").as_posix().replace(".o", ".c")
        for name, info in disasm_reloc(o).items():
            if not info["body"]:
                continue
            sdk[name] = {**info, "src": src}

    # ---- resolve base-symbol ROM addresses from mapped anchors ----
    # For a compiled symbol S referenced (with addend a) at reloc position i by a func that
    # is ALSO mapped in the ROM, the ROM twin's reloc i points at abs addr T(+r) ->
    # base(S) = (T + r) - a.  Vote across anchors; take the majority.
    base_rom = {}
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
    for sym, vmap in votes.items():
        base_rom[sym] = max(vmap.items(), key=lambda kv: kv[1])[0]

    # ---- find collision groups & disambiguate by relocation ----
    by_body = defaultdict(list)
    for name, info in sdk.items():
        by_body[info["body"]].append(name)

    proposals = []     # (rom_func, sdk_name, src)
    for rf, info in rom.items():
        if not re.fullmatch(r"func_[0-9a-f]+", rf):
            continue                                   # only unmapped ROM funcs
        cands = by_body.get(info["body"], [])
        if len(cands) < 2:
            continue                                   # not an sdk_match collision
        rr = info["relocs"]
        hits = []
        for sname in cands:
            sr = sdk[sname]["relocs"]
            if len(sr) != len(rr):
                continue
            ok = True
            for (ssym, sadd), (rsym, radd) in zip(sr, rr):
                if rsym == ssym:                       # same named symbol (e.g. a helper fn)
                    if radd != sadd:
                        ok = False; break
                    continue
                ta = sym_addr(rsym, addr_of)           # distinguishing reloc off a base
                if ssym in base_rom and ta is not None and base_rom[ssym] + sadd == ta + radd:
                    continue
                ok = False; break
            if ok:
                hits.append(sname)
        hits = sorted(set(hits))
        if len(hits) == 1:
            proposals.append((rf, hits[0], sdk[hits[0]]["src"]))

    # one rom addr -> one name only
    from collections import Counter
    ac = Counter(p[0] for p in proposals)
    proposals = [p for p in proposals if ac[p[0]] == 1]
    # one name -> one rom addr only
    nc = Counter(p[1] for p in proposals)
    proposals = [p for p in proposals if nc[p[1]] == 1]

    print(f"=== reloc-aware disambiguation: {len(proposals)} proposals "
          f"({len(base_rom)} base symbols resolved) ===")
    for rf, name, src in sorted(proposals):
        print(f"  {rf}  -> {name}   [{Path(src).stem}]")

    if args.apply and proposals:
        cfg_path = ROOT / "config" / "sdk_mappings.json"
        cfg = json.loads(cfg_path.read_text(encoding="utf-8"))
        have_a = {e["addr"].lower() for e in cfg["mappings"]}
        have_s = {u["source"] for u in cfg["objdiff_units"]}
        lib_of = lambda s: s.split("/libraries/")[1].split("/")[0] if "/libraries/" in s else "sdk"
        unit_of = lambda s: (("nns" if "NitroSystem" in s else "sdk") + "/" + lib_of(s)
                             + "/" + Path(s).stem)
        nm = nu = 0
        for rf, name, src in sorted(proposals):
            ha = f"0x{addr_of[rf]:08x}"
            if ha.lower() not in have_a:
                cfg["mappings"].append({"addr": ha, "sdk_name": name,
                                        "library": lib_of(src), "confidence": "certain"})
                have_a.add(ha.lower()); nm += 1
            if src not in have_s:
                cfg["objdiff_units"].append({"source": src,
                                             "target": "build/delinks/_dsd_gap@main_7.o",
                                             "name": unit_of(src)})
                have_s.add(src); nu += 1
        cfg_path.write_text(json.dumps(cfg, indent=2) + "\n", encoding="utf-8")
        print(f"\n  APPLIED: +{nm} mappings, +{nu} units")


if __name__ == "__main__":
    main()
