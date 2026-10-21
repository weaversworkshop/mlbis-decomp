#!/usr/bin/env python3
import argparse
import difflib
import re
import subprocess
import sys
from pathlib import Path

from common import BUILD, OBJDUMP, ROOT, SDK_MAPPINGS as MAPPINGS, SYMBOLS, read_symbols, sdk_mappings
from common import GAP_O as GAP_DEFAULT

FUNC_RE = re.compile(r"^[0-9a-f]+ <([^>]+)>:$")
SECTION_RE = re.compile(r"^Disassembly of section (.+):$")
INSN_RE = re.compile(r"^\s*([0-9a-f]+):\t([0-9a-f ]+?)\t(.*)$")
DATA_RE = re.compile(r"^\s*([0-9a-f]+):\t([0-9a-f]{8})(?:\s|$)")
RELOC_RE = re.compile(r"^\s*([0-9a-f]+):\s*R_ARM\S+\s+(\S+)")
BRANCH_RE = re.compile(
    r"^(b|bl|bx|blx|beq|bne|bcs|bcc|bmi|bpl|bvs|bvc|bhi|bls|bge|blt|bgt|ble"
    r"|bhs|blo|bal)(\.w)?$"
)
POOL_REF_RE = re.compile(r"\[pc, #-?\d+\]\s*@\s*([0-9a-fA-F]+)\b")


def sdk_objects():
    return sorted((BUILD / "lib").rglob("*.o"))


def disasm(objfile):
    """[(name, section, start, end, [(addr, raw, text, reloc)])]. Sections are
    counted, not named: per-function sections all restart at address 0."""
    out = subprocess.run(
        [OBJDUMP, "-dr", str(objfile)], capture_output=True, text=True
    ).stdout
    funcs = []
    cur = None
    section = 0
    for line in out.splitlines():
        if SECTION_RE.match(line):
            section += 1
            continue
        m = FUNC_RE.match(line)
        if m:
            name = m.group(1)
            if name.startswith((".L", "$", ".word")):
                continue
            cur = {"name": name, "section": section, "start": None, "insns": []}
            funcs.append(cur)
            continue
        m = RELOC_RE.match(line)
        if m and cur is not None and cur["insns"]:
            a, b, t, _ = cur["insns"][-1]
            # a reloc past the last insn belongs to a pool word objdump collapsed to '...'
            if int(m.group(1), 16) == a:
                cur["insns"][-1] = (a, b, t, m.group(2))
            continue
        m = INSN_RE.match(line)
        if m and cur is not None:
            addr = int(m.group(1), 16)
            if cur["start"] is None:
                cur["start"] = addr
            cur["insns"].append((addr, m.group(2).replace(" ", ""), m.group(3), None))
            continue
        m = DATA_RE.match(line)
        if m and cur is not None:
            cur["insns"].append((int(m.group(1), 16), m.group(2), ".word", None))
    return [(f["name"], f["section"], f["start"], f["insns"][-1][0] + 4, f["insns"])
            for f in funcs if f["insns"]]


def build_word_map(funcs):
    """(section, addr) -> ('abs', value) | ('rel', symbol)."""
    wm = {}
    for _name, section, _start, _end, insns in funcs:
        for addr, raw, _text, reloc in insns:
            if reloc is not None:
                wm[(section, addr)] = ("rel", reloc)
            elif len(raw) == 8:
                wm[(section, addr)] = ("abs", int(raw, 16))
    return wm


def pool_refs(insns, section, word_map):
    refs = []
    for _addr, _raw, text, _reloc in insns:
        m = POOL_REF_RE.search(text)
        if m:
            refs.append(word_map.get((section, int(m.group(1), 16)), ("abs", None)))
    return refs


def normalize(start, end, insns):
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
        if mn == ".word" or mn.startswith((".byte", ".short")):
            continue
        if BRANCH_RE.match(mn):
            m = re.match(r"^([0-9a-f]+)\b", ops)
            if not has_reloc and m and start <= int(m.group(1), 16) < end:
                norm.append(f"{mn} {int(m.group(1), 16) - addr:+d}")
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
        key = normalize(start, end, insns)
        if key:
            out[name] = (key, pool_refs(insns, section, wmap))
    return out


def verify_pool(sdk_c, gap_c):
    """Plain constants must be equal; symbol refs can't be byte-compared, so accept them."""
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
    if not SYMBOLS.exists():
        return {}
    return {s.name: s.addr for s in read_symbols() if s.kind == "function"}


def load_mapped_addrs():
    return {int(m["addr"], 16) for m in sdk_mappings()}


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


def insn_len(key):
    return sum(1 for k in key if not k.startswith(".isa"))


def fuzzy(args, target_o):
    sys.stderr.write("loading SDK objects...\n")
    sdk = {}
    for o in sdk_objects():
        try:
            for name, (key, _refs) in load_funcs(o).items():
                sdk.setdefault(name, (key, o))
        except Exception:
            continue
    by_len = {}
    for name, (key, src) in sdk.items():
        by_len.setdefault(insn_len(key), []).append((name, key, src))
    sys.stderr.write(f"  {len(sdk)} SDK functions indexed\n")

    gap = load_funcs(Path(target_o))
    sdk_keyset = {k for k, _ in sdk.values()}

    results = []
    for gname, (gkey, _refs) in gap.items():
        if not gname.startswith("func_"):
            continue
        gl = insn_len(gkey)
        if gl < args.min or gkey in sdk_keyset:
            continue
        lo, hi = int(gl * (1 - args.lentol)), int(gl * (1 + args.lentol)) + 1
        best = None
        for L in range(lo, hi + 1):
            for sname, skey, src in by_len.get(L, []):
                sm = difflib.SequenceMatcher(None, gkey, skey)
                if sm.real_quick_ratio() < args.sim or sm.quick_ratio() < args.sim:
                    continue
                r = sm.ratio()
                if args.sim <= r < 1.0 and (best is None or r > best[0]):
                    ndiff = (len(gkey) + len(skey)) - 2 * sum(
                        b.size for b in sm.get_matching_blocks())
                    best = (r, sname, src, ndiff, skey)
        if best:
            results.append((best[0], gname, gl, best[1], best[3], best[2], gkey, best[4]))
    results.sort(reverse=True)

    if args.show:
        for sim, gname, gl, sname, ndiff, src, gkey, skey in results:
            if gname == args.show:
                print(f"# {gname} ~ {sname}  sim={sim*100:.1f}%  diffs={ndiff}")
                for line in difflib.unified_diff(
                    [k for k in gkey if not k.startswith('.isa')],
                    [k for k in skey if not k.startswith('.isa')],
                    fromfile=gname, tofile=sname, lineterm=""):
                    print(line)
                return
        print(f"{args.show}: no near-match candidate found")
        return

    print(f"\n=== FUZZY SDK NEAR-MATCHES (sim >= {args.sim}, >= {args.min} insns) ===")
    print(f"{len(results)} unmapped gap funcs have a close-but-not-exact SDK candidate\n")
    print(f"{'sim':>6} {'diffs':>5}  {'gap func':<18} {'~ SDK candidate':<28} insns  source")
    for sim, gname, gl, sname, ndiff, src, _gk, _sk in results:
        lib = str(src).split("/lib/")[-1].split("/src/")[0].split("/")[-1]
        print(f"{sim*100:5.1f}% {ndiff:5d}  {gname:<18} {sname:<28} {gl:>4}   {lib}")


def exact(args, ap, target_o):
    gap_funcs = load_funcs(target_o)
    gap_index = {}
    for tname, (key, _consts) in gap_funcs.items():
        gap_index.setdefault(key, []).append(tname)

    sym_addrs = load_symbol_addrs()
    mapped = load_mapped_addrs()

    if args.all:
        sdk_os = sdk_objects()
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


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("sdk_o", nargs="?", help="compiled SDK .o")
    ap.add_argument("target_o", nargs="?", default=None)
    ap.add_argument("--gap-o", dest="gap_o", default=None,
                    help="gap/target .o to sweep (default _dsd_gap@main_7.o); "
                         "e.g. build/delinks/_dsd_gap@itcm_1.o")
    ap.add_argument("--all", action="store_true", help="all built SDK .o vs gap")
    ap.add_argument("--new", action="store_true", help="only unmapped/func_ hits")
    ap.add_argument("--min", type=int, default=None,
                    help="min insns for a high-confidence match (default 8; --fuzzy: 12)")
    ap.add_argument("--low", action="store_true",
                    help="also show low-confidence (short/ambiguous) matches")
    ap.add_argument("--fuzzy", action="store_true",
                    help="near-match scan: unmapped gap funcs vs closest SDK skeleton")
    ap.add_argument("--sim", type=float, default=0.85,
                    help="--fuzzy: min similarity (default 0.85; exact=1.0 excluded)")
    ap.add_argument("--lentol", type=float, default=0.20,
                    help="--fuzzy: only compare funcs within this relative length delta")
    ap.add_argument("--show", help="--fuzzy: full skeleton diff for one gap func vs its best candidate")
    args = ap.parse_args()
    if args.min is None:
        args.min = 12 if args.fuzzy else 8

    target_o = args.gap_o or args.target_o or str(GAP_DEFAULT)
    if args.fuzzy:
        fuzzy(args, target_o)
    else:
        exact(args, ap, target_o)


if __name__ == "__main__":
    main()
