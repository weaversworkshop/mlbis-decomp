#!/usr/bin/env python3
import argparse
import difflib
import re
import subprocess
import sys
from pathlib import Path

from common import BUILD, CONFIG, GAP_O, OBJDUMP, ROOT, die, overlay_gap, overlay_of, read_symbols

SYM_RE = re.compile(r"^[0-9a-f]+ <([^>]+)>:")
ADDR_INSN_RE = re.compile(r"^\s*([0-9a-f]+):\s*([0-9a-f]{4,8})( \t)?\s*(\S.*)$")
RELOC_RE = re.compile(r"^\s*([0-9a-f]+):\s*R_ARM_(\w+)\s+(\S+)")
RELOC_TOKEN_RE = re.compile(r"<R:([^>+]+)>")
BRANCH_RE = re.compile(
    r"\b(b|bl|blx|bx|beq|bne|bcs|bcc|bmi|bpl|bvs|bvc|bhi|bls|bge|blt|bgt|ble|bhs|blo)"
    r"(\.w)?\s+(?:0x)?([0-9a-f]+)"
)
DATA_MNEMONICS = (".word", ".short", ".byte", ".ascii")
SLOT_RE = re.compile(r"\[sp, #(\d+)\]")


def objdump(path, relocs=False):
    try:
        r = subprocess.run([OBJDUMP, "-dzr" if relocs else "-dz", str(path)], capture_output=True, text=True)
    except FileNotFoundError:
        die(f"error: {OBJDUMP} not found on PATH")
    if r.returncode != 0:
        die(f"error: objdump failed on {path}: {r.stderr.strip()}")
    return r.stdout.splitlines()


def extract_items(lines, symbol):
    """[(addr, insn, is_data, branch_target|None)]"""
    out, active, found = [], False, False
    relocated = {}
    for line in lines:
        m = SYM_RE.match(line)
        if m:
            name = m.group(1)
            if name == symbol:
                active, found = True, True
            elif active and not name.startswith(".L"):
                active = False
            continue
        if not active:
            continue
        rm = RELOC_RE.match(line)
        if rm:
            relocated[int(rm.group(1), 16)] = (rm.group(2), rm.group(3))
            continue
        im = ADDR_INSN_RE.match(line)
        if not im:
            continue
        addr = int(im.group(1), 16)
        raw = im.group(2)
        # code prints `HEX \tmnemonic`; data is `.word` or a raw hex+ascii dump with no tab
        is_data = im.group(4).startswith(DATA_MNEMONICS) or im.group(3) is None
        insn = re.sub(r"\s*[@;].*$", "", im.group(4))
        insn = re.sub(r"\s*<[^>]*>\s*$", "", insn).rstrip()
        if not insn and not is_data:
            continue
        if is_data:
            insn = f".{ {8: 'word', 4: 'short', 2: 'byte'}.get(len(raw), 'word') }\t0x{raw}"
        bm = BRANCH_RE.search(insn)
        # calls/bx are relocs (offset 0 in a .o): never relativize those
        tgt = int(bm.group(3), 16) if bm and not is_data and bm.group(1) not in ("bl", "blx", "bx") else None
        out.append((addr, insn, is_data, tgt, raw))
    fixed = []
    for a, i, d, t, raw in out:
        # dsd can leave jump-table words in a code-mapped region; no instruction carries ABS32
        if not d and relocated.get(a, ("",))[0] == "ABS32":
            d, i = True, f".word\t0x{raw}"
        if d and a in relocated:
            kind, sym = relocated[a]
            word = int(i.split("0x")[-1], 16)
            i = f"{i.split(chr(9))[0]}\t<R:{sym}>" + (f"+{word:#x}" if word and kind == "ABS32" else "")
        fixed.append((a, i, d, None if a in relocated else t))
    return fixed, found


def pair_pool(titems, mitems):
    """obj_match's rule, per aligned pool word: a reloc vs a plain word is resolved via the target
    symbol's address when known, else counted equal and reported. -> (titems, mitems, notes)"""
    td = [k for k, it in enumerate(titems) if it[2]]
    md = [k for k, it in enumerate(mitems) if it[2]]
    titems, mitems, notes = list(titems), list(mitems), []
    if len(td) == len(md):
        for ti, mi in zip(td, md):
            t, m = titems[ti], mitems[mi]
            tr, mr = RELOC_TOKEN_RE.search(t[1]), RELOC_TOKEN_RE.search(m[1])
            if bool(tr) == bool(mr):
                continue
            rel, const, is_t = (t, m, True) if tr else (m, t, False)
            sym = (tr or mr).group(1)
            add = re.search(r"\+0x([0-9a-f]+)$", rel[1])
            from obj_match import sym_addr
            a = sym_addr(sym)
            if a is not None:
                rel = (rel[0], f"{rel[1].split(chr(9))[0]}\t0x{(a + (int(add.group(1), 16) if add else 0)) & 0xffffffff:08x}", *rel[2:])
            else:
                c = const[1].split(chr(9))[1]
                notes.append(f"target {sym} / mine {c}" if is_t else f"target {c} / mine {sym}")
                const = (const[0], rel[1], *const[2:])
            if is_t:
                titems[ti], mitems[mi] = rel, const
            else:
                titems[ti], mitems[mi] = const, rel
    strip = lambda items: [(a, RELOC_TOKEN_RE.sub("<R>", i), d, t) for a, i, d, t in items]
    return strip(titems), strip(mitems), notes


def relativize(items, keep_pool):
    """Branch targets -> instruction-index delta `<T+3>`; leaving the function stays `<T>`.
    Returns (texts, is_backward_branch flags)."""
    index = {}
    for addr, _, is_data, _ in items:
        if not is_data:
            index[addr] = len(index)
    out, back = [], []
    for addr, insn, is_data, tgt in items:
        if is_data and not keep_pool:
            continue
        rep = "<T>"
        if tgt is not None and tgt in index and addr in index:
            rep = f"<T{index[tgt] - index[addr]:+d}>"
        out.append(BRANCH_RE.sub(lambda m: m.group(1) + (m.group(2) or "") + " " + rep, insn))
        back.append(tgt is not None and tgt < addr and not insn.startswith("bl"))
    return out, back


def extract(lines, symbol, keep_pool):
    items, found = extract_items(lines, symbol)
    return relativize(items, keep_pool)[0], found


def frame_map(items):
    """{slot: index of first instruction touching it}"""
    first, n = {}, 0
    for _, insn, is_data, _ in items:
        if is_data:
            continue
        m = SLOT_RE.search(insn)
        if m and int(m.group(1)) not in first:
            first[int(m.group(1))] = n
        n += 1
    return first


def print_frame(tgt_items, mine_items):
    by_t = {i: s for s, i in frame_map(tgt_items).items()}
    by_m = {i: s for s, i in frame_map(mine_items).items()}
    print(f"{'first@':>7} {'target':>7} {'mine':>7}")
    for i in sorted(set(by_t) | set(by_m)):
        ts, ms = by_t.get(i), by_m.get(i)
        mark = "" if ts == ms else "  <-"
        print(f"{i:>7} {('' if ts is None else ts):>7} {('' if ms is None else ms):>7}{mark}")
    print("ascending slot order = reverse first-assignment order of the variables (additional webs);")
    print("first webs follow declaration order. Adjacent same-kind slots in section order = one shared variable.")


def defined_syms(path):
    r = subprocess.run([OBJDUMP, "-t", str(path)], capture_output=True, text=True)
    if r.returncode != 0:
        return set()
    return {ln.rsplit(" ", 1)[-1] for ln in r.stdout.splitlines() if ln[:1] != " " and "*UND*" not in ln}


def pick_target(names):
    ovs = {o for n in names if (o := overlay_of(n))}
    if len(ovs) == 1 and (g := overlay_gap(ovs.pop())):
        return g
    gaps = [GAP_O] + sorted((BUILD / "delinks").glob("_dsd_gap@itcm_*.o")) +         sorted((BUILD / "delinks").glob("_dsd_gap@dtcm_*.o"))
    hit = next((g for g in gaps if g.is_file() and names & defined_syms(g)), None)
    if hit:
        return hit
    for sp in sorted((CONFIG / "overlays").glob("*/symbols.txt")):
        if any(s.name in names for s in read_symbols(sp)) and (g := overlay_gap(sp.parent.name)):
            return g
    return GAP_O


def find_mine(symbol):
    """-> (path, defined name): exact name first, else the mangled C++ method embedding func_<addr>."""
    mangled = re.compile(rf"^_Z.*\d{re.escape(symbol)}E") if re.fullmatch(r"func_[0-9a-f]{8}", symbol) else None
    fuzzy = None
    for base in (BUILD / "arm9/src", BUILD / "scratch"):
        if not base.is_dir():
            continue
        for o in sorted(base.rglob("*.o")):
            if o.name.startswith("_dsd_gap@") or "delinks" in o.parts:
                continue
            syms = defined_syms(o)
            if symbol in syms:
                return o, symbol
            if mangled and fuzzy is None:
                hit = [n for n in syms if mangled.match(n)]
                if hit:
                    fuzzy = (o, hit[0])
    return fuzzy


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("symbol")
    ap.add_argument("--mine")
    ap.add_argument("--target", help="default: main gap, else the resident gap defining the symbol")
    ap.add_argument("--pool", action="store_true", help="include literal-pool/data lines")
    ap.add_argument("--context", type=int, default=3)
    ap.add_argument("--target-symbol", help="target-side name when it differs (e.g. func_<addr> vs mangled)")
    ap.add_argument("--frame", action="store_true", help="also print the spill-slot table (target vs mine, by first use)")
    args = ap.parse_args()

    msym = args.symbol
    if args.mine:
        mine_path = Path(args.mine)
    else:
        mine_path, msym = find_mine(args.symbol) or (None, args.symbol)
    if not mine_path or not mine_path.is_file():
        die(f"error: no compiled .o contains '{args.symbol}' (searched build/arm9/src, build/scratch); pass --mine")
    tsym = args.target_symbol or args.symbol
    target_path = Path(args.target) if args.target else pick_target({tsym, msym})
    if not target_path.is_file():
        die(f"error: target not found: {target_path}")
    if target_path != GAP_O and not args.target:
        print(f"note: target is {target_path.name}")
    if msym != args.symbol:
        if not args.target_symbol and msym in defined_syms(target_path):
            tsym = msym
        print(f"note: '{args.symbol}' is compiled as '{msym}' (target: '{tsym}')")
    titems, tfound = extract_items(objdump(target_path, True), tsym)
    mitems, mfound = extract_items(objdump(mine_path, True), msym)
    if args.pool:
        titems, mitems, notes = pair_pool(titems, mitems)
        for n in notes:
            print(f"note: pool word {n} (symbol address unknown; counted equal)")
    tgt, _ = relativize(titems, args.pool)
    mine, _ = relativize(mitems, args.pool)
    if args.frame and tfound and mfound:
        print_frame(titems, mitems)
        print()
    if not tfound:
        die(f"error: symbol '{tsym}' not in target {target_path.name}")
    if not mfound:
        die(f"error: symbol '{msym}' not in {mine_path}")
    if not tgt or not mine:
        die(f"error: EMPTY EXTRACTION (target={len(tgt)}, mine={len(mine)} insns) -- refusing to compare")

    if tgt == mine:
        mp = mine_path.resolve()
        shown = mp.relative_to(ROOT) if mp.is_relative_to(ROOT) else mp
        print(f"MATCH ({len(mine)} instructions)  [{shown}]")
        return 0
    diff = list(difflib.unified_diff(tgt, mine, "target", "mine", n=args.context, lineterm=""))
    changed = sum(1 for d in diff if d[:1] in "+-" and d[:3] not in ("+++", "---"))
    print("\n".join(diff))
    print(f"\nDIFF: {changed} lines differ (target={len(tgt)}, mine={len(mine)} insns)")
    return 1


if __name__ == "__main__":
    sys.exit(main())
