#!/usr/bin/env python3
"""Find SDK functions present in the ROM by byte-matching compiled SDK objects.

For every function in the compiled SDK objects (build/lib/NitroSDK, build/lib/NitroSystem),
normalize its disassembly (strip addresses, reloc targets, literal-pool words) and look for
a ROM `func_<addr>` with an identical instruction stream. Proposes addr->name mappings for
still-unmapped ROM functions, so we can map every SDK function the game links instead of
accidentally hand-decompiling one.

Confidence:
  - exact 1:1 normalized match, function is currently `func_<addr>` (unmapped)
  - contiguity bonus: a source file's matched functions should land in a contiguous,
    in-source-order run in the ROM (the linker keeps a TU together). Runs of >=2 are HIGH.

Output: groups proposals by source file with the ROM address span. Use --json to emit
mapping entries ready for config/sdk_mappings.json. Nothing is written automatically.

Usage:
  make sdk                                   # build all SDK objects first
  python tools/scripts/sdk_match.py          # report
  python tools/scripts/sdk_match.py --json   # emit mapping JSON lines
"""
import argparse
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
OBJDUMP = "C:/msys64/ucrt64/bin/arm-none-eabi-objdump.exe"
ROM_O = ROOT / "build" / "delinks" / "_dsd_gap@main_7.o"
SDK_O_ROOTS = [ROOT / "build" / "lib" / "NitroSDK", ROOT / "build" / "lib" / "NitroSystem"]
SYMBOLS = ROOT / "config" / "arm9" / "symbols.txt"

HDR = re.compile(r"^[0-9a-f]+ <([^>]+)>:")
INSN = re.compile(r"^\s+([0-9a-f]+):\s+[0-9a-f ]+?\t(.*)$")


def normalize(body):
    """body: list of raw 'mnemonic operands' strings -> normalized tuple."""
    out = []
    for t in body:
        t = re.sub(r"\s+;.*$", "", t)
        t = re.sub(r"\s+@.*$", "", t)
        t = re.sub(r"<[^>]*>", "<t>", t)        # call/branch targets
        t = re.sub(r"\[pc, #-?\w+\]", "[pc]", t)
        t = re.sub(r"\[pc, #-?\w+\]!", "[pc]", t)
        if t.startswith(".word") or t.startswith(".short") or t.startswith(".byte"):
            t = ".word"                          # literal-pool entry (reloc or const)
        out.append(t.strip())
    return tuple(out)


def disasm_funcs(obj):
    """obj path -> {func_name: (normalized_tuple, n_insns)}; uses one objdump call."""
    try:
        txt = subprocess.run([OBJDUMP, "-d", str(obj)], capture_output=True, text=True).stdout
    except Exception:
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


def load_symbols():
    """func name -> (int addr, int size); plus set of already-mapped (named) addrs."""
    addr_of, size_of, mapped_named = {}, {}, set()
    for ln in SYMBOLS.read_text(encoding="utf-8", errors="replace").splitlines():
        m = re.match(r"(\S+)\s+kind:function\([^)]*size=0x([0-9a-fA-F]+)\)\s+addr:(0x[0-9a-fA-F]+)", ln)
        if not m:
            continue
        name, size, addr = m.group(1), int(m.group(2), 16), int(m.group(3), 16)
        addr_of[name] = addr
        size_of[name] = size
        if not re.fullmatch(r"func_[0-9a-f]+", name):
            mapped_named.add(addr)
    return addr_of, size_of, mapped_named


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--min-insns", type=int, default=3,
                    help="ignore functions smaller than this unless part of a >=2 contiguous run")
    ap.add_argument("--json", action="store_true", help="emit sdk_mappings.json entries")
    ap.add_argument("--apply", action="store_true",
                    help="merge clean proposals (mappings + units) into config/sdk_mappings.json")
    args = ap.parse_args()

    if not ROM_O.exists():
        sys.exit(f"missing {ROM_O} (run `make delinks`)")

    addr_of, size_of, mapped = load_symbols()
    rom = disasm_funcs(ROM_O)
    # index ROM unmapped func_<addr> by normalized body -> [names]
    rom_index = {}
    for name, (body, n) in rom.items():
        if not re.fullmatch(r"func_[0-9a-f]+", name):
            continue
        if addr_of.get(name) in mapped:
            continue
        rom_index.setdefault(body, []).append(name)

    sdk_objs = []
    for r in SDK_O_ROOTS:
        sdk_objs += sorted(r.rglob("*.o")) if r.exists() else []

    # per source file: list of (sdk_func, rom_func, addr, n_insns, ambiguous)
    by_src = {}
    for o in sdk_objs:
        src = o.relative_to(ROOT / "build").as_posix().replace(".o", ".c")  # forward slashes for make
        for sname, (body, n) in disasm_funcs(o).items():
            if n == 0:
                continue
            cands = rom_index.get(body, [])
            if len(cands) == 1:
                rf = cands[0]
                by_src.setdefault(src, []).append((sname, rf, addr_of[rf], n, False))
            elif len(cands) > 1:
                by_src.setdefault(src, []).append((sname, None, None, n, True))

    proposals = []  # (addr, sdk_name, src)
    print("=== SDK functions found in ROM (proposed mappings) ===\n")
    for src in sorted(by_src):
        rows = [r for r in by_src[src] if r[1]]   # resolved only
        if not rows:
            continue
        rows.sort(key=lambda r: r[2])
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
    # never propose the same ROM address twice (cross-file collision -> ambiguous)
    from collections import Counter
    ac = Counter(p[0] for p in proposals)
    collisions = sorted({a for a, c in ac.items() if c > 1})
    proposals = [p for p in proposals if ac[p[0]] == 1]
    if collisions:
        print(f"\n  excluded {len(collisions)} ROM-address collisions: "
              f"{[hex(a) for a in collisions]}")
    print(f"  TOTAL clean proposals: {len(proposals)} mappings across {len(by_src)} files")

    lib_of = lambda s: s.split("/libraries/")[1].split("/")[0] if "/libraries/" in s else "sdk"
    unit_of = lambda s: (("nns" if "NitroSystem" in s else "sdk") + "/" + lib_of(s)
                         + "/" + Path(s).stem)

    if args.json:
        print("\n=== JSON (review, then merge into config/sdk_mappings.json) ===")
        for a, name, src in sorted(proposals):
            print(f'    {{ "addr": "0x{a:08x}", "sdk_name": "{name}", '
                  f'"library": "{lib_of(src)}", "confidence": "certain" }},')

    if args.apply:
        cfg_path = ROOT / "config" / "sdk_mappings.json"
        cfg = json.loads(cfg_path.read_text(encoding="utf-8"))
        have_a = {e["addr"].lower() for e in cfg["mappings"]}
        have_s = {u["source"] for u in cfg["objdiff_units"]}
        nm = nu = 0
        for a, name, src in sorted(proposals):
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
        cfg_path.write_text(json.dumps(cfg, indent=2) + "\n", encoding="utf-8")
        print(f"\n  APPLIED: +{nm} mappings, +{nu} units -> {cfg_path.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
