#!/usr/bin/env python3
"""Find SDK funcs still named func_XXXX by normalized-disassembly content match
(masks branch/call targets + relocated operands; keeps pool consts + ISA).
Usage: tu_diff.py [SDK.o [TARGET.o]] | --all [--new] [--low] [--min N].
Run `make sdk` first so build/lib/**/*.o exist."""
import argparse
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
OBJDUMP = "arm-none-eabi-objdump"
GAP_DEFAULT = ROOT / "build" / "delinks" / "_dsd_gap@main_7.o"
SYMBOLS = ROOT / "config" / "arm9" / "symbols.txt"
MAPPINGS = ROOT / "config" / "sdk_mappings.json"

FUNC_RE = re.compile(r"^[0-9a-f]+ <([^>]+)>:$")
SECTION_RE = re.compile(r"^Disassembly of section (.+):$")
INSN_RE = re.compile(r"^\s*([0-9a-f]+):\t([0-9a-f ]+?)\t(.*)$")
DATA_RE = re.compile(r"^\s*([0-9a-f]+):\t([0-9a-f]{8})(?:\s|$)")
RELOC_RE = re.compile(r"^\s*([0-9a-f]+):\s*R_ARM\S+\s+(\S+)")
BRANCH_RE = re.compile(
    r"^(b|bl|bx|blx|beq|bne|bcs|bcc|bmi|bpl|bvs|bvc|bhi|bls|bge|blt|bgt|ble"
    r"|bhs|blo|bal)(\.w)?$"
)


def disasm(objfile):
    """Return ordered list of (name, start_addr, end_addr, insns).

    Each insn is (addr, text, has_reloc). Relocations are attached to the
    immediately preceding instruction positionally -- per-function sections in
    a relocatable .o restart at address 0, so an address-keyed map collides.
    """
    out = subprocess.run(
        [OBJDUMP, "-dr", str(objfile)], capture_output=True, text=True
    ).stdout
    funcs = []
    cur = None
    section = 0  # per-function sections all named ".text" -> count instances
    for line in out.splitlines():
        if SECTION_RE.match(line):
            section += 1
            continue
        m = FUNC_RE.match(line)
        if m:
            name = m.group(1)
            if name.startswith(".L") or name.startswith("$") or name.startswith(".word"):
                continue  # local label, keep accumulating in current function
            cur = {"name": name, "section": section, "start": None, "insns": []}
            funcs.append(cur)
            continue
        m = RELOC_RE.match(line)
        if m and cur is not None and cur["insns"]:
            a, b, t, _ = cur["insns"][-1]
            cur["insns"][-1] = (a, b, t, m.group(2))
            continue
        m = INSN_RE.match(line)
        if m and cur is not None:
            addr = int(m.group(1), 16)
            raw = m.group(2).replace(" ", "")
            text = m.group(3)
            if cur["start"] is None:
                cur["start"] = addr
            cur["insns"].append((addr, raw, text, None))
            continue
        m = DATA_RE.match(line)  # bare pool word (one tab, no mnemonic column)
        if m and cur is not None:
            cur["insns"].append((int(m.group(1), 16), m.group(2), ".word", None))
    result = []
    for f in funcs:
        if not f["insns"]:
            continue
        end = f["insns"][-1][0] + 4
        result.append((f["name"], f["section"], f["start"], end, f["insns"]))
    return result


def build_word_map(funcs):
    """(section, addr) -> ('abs', value) | ('rel', symbol) for every word, read
    from the raw-bytes column (the gap renders pool data as bare hex, not
    `.word`). Keyed by section because per-function sections restart at addr 0."""
    wm = {}
    for _name, section, _start, _end, insns in funcs:
        for addr, raw, _text, reloc in insns:
            if reloc is not None:
                wm[(section, addr)] = ("rel", reloc)
            elif len(raw) == 8:
                wm[(section, addr)] = ("abs", int(raw, 16))
    return wm


POOL_REF_RE = re.compile(r"\[pc, #-?\d+\]\s*@\s*([0-9a-fA-F]+)\b")


def pool_refs(insns, section, word_map):
    """Ordered list of pool constants this function loads via [pc, #N],
    resolved by the objdump '@ <addr>' annotation -> robust to pool placement."""
    refs = []
    for addr, raw, text, reloc in insns:
        m = POOL_REF_RE.search(text)
        if m:
            refs.append(word_map.get((section, int(m.group(1), 16)), ("abs", None)))
    return refs


def normalize(name, start, end, insns):
    # Record instruction set so a Thumb function can't match an ARM one with the
    # same mnemonic skeleton (objdiff caught OS_Init: SDK Thumb vs ROM ARM).
    isa = "thumb" if any(
        len(raw) == 4 and not t.strip().startswith(".") for _a, raw, t, _r in insns
    ) else "arm"
    norm = [f".isa {isa}"]
    for addr, raw, text, has_reloc in insns:
        text = re.sub(r"\s*@.*$", "", text).strip()
        text = re.sub(r"\s+", " ", text)
        parts = text.split(None, 1)
        if not parts:
            continue
        mn = parts[0]
        ops = parts[1] if len(parts) > 1 else ""
        if mn == ".word" or mn.startswith(".byte") or mn.startswith(".short"):
            continue  # literal pool / data
        if BRANCH_RE.match(mn):
            # external call (resolved address outside fn, OR R_ARM_PC24 reloc)
            # and local branch (relative offset) must normalize identically
            # whether or not the .o has resolved the target.
            m = re.match(r"^([0-9a-f]+)\b", ops)
            if has_reloc:
                norm.append(f"{mn} <C>")
            elif m:
                tgt = int(m.group(1), 16)
                if start <= tgt < end:
                    norm.append(f"{mn} {tgt - addr:+d}")
                else:
                    norm.append(f"{mn} <C>")
            else:
                norm.append(f"{mn} <C>")
            continue
        if has_reloc:
            norm.append(f"{mn} <R>")
            continue
        ops = re.sub(r"\[pc, #-?\d+\]", "[pc]", ops)
        norm.append(f"{mn} {ops}".strip())
    return tuple(norm)


def load_funcs(objfile):
    funcs = disasm(objfile)
    wmap = build_word_map(funcs)
    out = {}
    for name, section, start, end, insns in funcs:
        key = normalize(name, start, end, insns)
        if key:
            out[name] = (key, pool_refs(insns, section, wmap))
    return out


def verify_pool(sdk_c, gap_c):
    """Strict literal-pool check. When both sides load a plain constant, the
    values must be identical (catches same-shape funcs that read a different
    MMIO reg / mask). Symbol refs (either side relocated) can't be byte-compared
    -- accept them; the exact instruction match already constrains the rest."""
    if len(sdk_c) != len(gap_c):
        return False, f"pool count {len(sdk_c)}!={len(gap_c)}"
    nconst = nref = 0
    for (sk, sv), (gk, gv) in zip(sdk_c, gap_c):
        if sk == "abs" and gk == "abs" and sv is not None and gv is not None:
            if sv != gv:
                return False, f"const {sv:#x} vs {gv:#x}"
            nconst += 1
        else:
            nref += 1
    return True, f"{nconst} const ok / {nref} ref"


def load_symbol_addrs():
    addrs = {}
    if SYMBOLS.exists():
        for line in SYMBOLS.read_text(encoding="utf-8").splitlines():
            m = re.match(r"(\S+)\s+kind:function.*addr:0x([0-9a-fA-F]+)", line)
            if m:
                addrs[m.group(1)] = int(m.group(2), 16)
    return addrs


def load_mapped_addrs():
    if not MAPPINGS.exists():
        return set()
    d = json.loads(MAPPINGS.read_text(encoding="utf-8"))
    return {int(m["addr"], 16) for m in d.get("mappings", [])}


def match_object(sdk_o, gap_funcs, gap_index, sym_addrs, mapped, only_new, min_insns):
    sdk_funcs = load_funcs(sdk_o)
    raw = []
    sdk_hit_count = {}
    gap_claim_count = {}
    for sname, (key, consts) in sdk_funcs.items():
        hits = gap_index.get(key, [])
        sdk_hit_count[sname] = len(hits)
        for tname in hits:
            gap_claim_count[tname] = gap_claim_count.get(tname, 0) + 1
            raw.append((sname, tname, len(key), consts))
    rows = []
    for sname, tname, nins, consts in raw:
        taddr = sym_addrs.get(tname)
        is_mapped = taddr in mapped if taddr is not None else False
        unnamed = tname.startswith("func_")
        ambiguous = sdk_hit_count[sname] > 1 or gap_claim_count[tname] > 1
        high_conf = (nins >= min_insns) and not ambiguous
        ok, detail = verify_pool(consts, gap_funcs[tname][1])
        if only_new and (is_mapped or not unnamed):
            continue
        rows.append((sname, tname, taddr, is_mapped, unnamed, nins, high_conf, ok, detail))
    return rows, len(sdk_funcs)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("sdk_o", nargs="?", help="compiled SDK .o")
    ap.add_argument("target_o", nargs="?", default=str(GAP_DEFAULT))
    ap.add_argument("--all", action="store_true", help="all built SDK .o vs gap")
    ap.add_argument("--new", action="store_true", help="only unmapped/func_ hits")
    ap.add_argument("--min", type=int, default=8,
                    help="min insns for a high-confidence match (default 8)")
    ap.add_argument("--low", action="store_true",
                    help="also show low-confidence (short/ambiguous) matches")
    args = ap.parse_args()

    gap_funcs = load_funcs(args.target_o)
    gap_index = {}
    for tname, (key, _consts) in gap_funcs.items():
        gap_index.setdefault(key, []).append(tname)

    sym_addrs = load_symbol_addrs()
    mapped = load_mapped_addrs()

    if args.all:
        sdk_os = sorted((ROOT / "build" / "lib").rglob("*.o"))
    else:
        if not args.sdk_o:
            ap.error("provide an SDK .o or use --all")
        sdk_os = [Path(args.sdk_o)]

    total_new_hi = total_new_lo = total_flag = 0
    for o in sdk_os:
        rows, n = match_object(o, gap_funcs, gap_index, sym_addrs, mapped,
                               args.new, args.min)
        shown = [r for r in rows if r[6] or args.low]
        if not shown:
            continue
        rel = o.relative_to(ROOT).as_posix() if o.is_absolute() else str(o)
        n_lo = sum(1 for r in rows if not r[6])
        suffix = f", {n_lo} low-conf hidden" if (n_lo and not args.low) else ""
        print(f"\n=== {rel}  ({len(shown)} shown / {n} funcs{suffix}) ===")
        for sname, tname, taddr, is_mapped, unnamed, nins, hi, ok, detail in sorted(
            shown, key=lambda r: (not r[6], r[2] is None, r[2] or 0)
        ):
            if not ok:
                tag = "POOL!?"
            elif is_mapped:
                tag = "MAPPED"
            elif unnamed:
                tag = "NEW!  " if hi else "new?  "
            else:
                tag = "named "
            astr = f"{taddr:#08x}" if taddr is not None else "  ?     "
            conf = "" if hi else "  (low-conf)"
            print(f"  [{tag}] {sname:28} -> {tname:24} @ {astr}  "
                  f"({nins} insns, {detail}){conf}")
            if not ok:
                total_flag += 1
            elif not is_mapped and unnamed:
                if hi:
                    total_new_hi += 1
                else:
                    total_new_lo += 1

    if total_flag:
        print(f"\n!! {total_flag} POOL-MISMATCH rows (instructions match but a "
              f"literal constant differs -- NOT the same function)")
    print(f"\nNEW unmapped candidates: {total_new_hi} high-confidence"
          f", {total_new_lo} low-confidence.")


if __name__ == "__main__":
    main()
