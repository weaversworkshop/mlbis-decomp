#!/usr/bin/env python3
"""Byte+size-match every compiled SDK function against unmapped ROM func_XXXX
and propose mappings (unambiguous only; skips byte-identical-sibling collisions).
Flags: --apply (write sdk_mappings.json + units), --json, --min-insns N.
Loop with `make delinks` until 0 proposals; then run sdk_disambiguate.py."""
import argparse
import bisect
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
# robust line for the delinked ROM: addr, hex encoding, then text -- catches data
# lines like `....`/`.word` that objdump renders space-separated (no tab).
ROM_LINE = re.compile(r"^\s+([0-9a-f]+):\s+[0-9a-f]{2,8}(?: [0-9a-f]{2,8})*\s+(.+?)\s*$")


def normalize(body):
    """body: list of raw 'mnemonic operands' strings -> normalized tuple."""
    out = []
    for t in body:
        t = re.sub(r"\s+;.*$", "", t)
        t = re.sub(r"\s+@.*$", "", t)
        t = re.sub(r"<[^>]*>", "<t>", t)        # call/branch targets (symbol)
        t = re.sub(r"\b[0-9a-f]+ <t>", "<t>", t)  # strip the b/bl target ADDRESS too
        t = re.sub(r"\[pc, #-?\w+\]", "[pc]", t)
        t = re.sub(r"\[pc, #-?\w+\]!", "[pc]", t)
        if (t.startswith(".word") or t.startswith(".short") or t.startswith(".byte")
                or t.startswith("....")):
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


def disasm_rom(rom_o):
    """Disassemble the delinked ROM, bucketing instructions into functions by each
    function's [addr, addr+size) range from the object's OWN symbol table -- so internal
    data-symbol labels (literal pools) do NOT truncate a function the way splitting on
    `<label>:` does. Uses the .o's address space (consistent between -t and -d)."""
    try:
        tt = subprocess.run([OBJDUMP, "-t", str(rom_o)], capture_output=True, text=True).stdout
        dd = subprocess.run([OBJDUMP, "-d", str(rom_o)], capture_output=True, text=True).stdout
    except Exception:
        return {}
    ranges = []
    for ln in tt.splitlines():
        m = re.match(r"([0-9a-f]+)\s+\S+\s+F\s+\.text\s+([0-9a-f]+)\s+(\S+)\s*$", ln)
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
    """obj path -> {func_name: byte_size} from the symbol table (exact)."""
    try:
        txt = subprocess.run([OBJDUMP, "-t", str(obj)], capture_output=True, text=True).stdout
    except Exception:
        return {}
    out = {}
    for ln in txt.splitlines():
        m = re.match(r"[0-9a-f]+\s+\S+\s+F\s+\.text\s+([0-9a-f]+)\s+(\S+)\s*$", ln)
        if m:
            out[m.group(2)] = int(m.group(1), 16)
    return out


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
    rom = disasm_rom(ROM_O)
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
        sizes = func_sizes(o)
        for sname, (body, n) in disasm_funcs(o).items():
            if n == 0:
                continue
            cands = rom_index.get(body, [])
            # require EXACT byte-size equality too (rejects shape-only matches, e.g. OS_Init)
            cands = [rf for rf in cands if sizes.get(sname) == size_of.get(rf)]
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
