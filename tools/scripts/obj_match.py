#!/usr/bin/env python3
"""Reloc-aware match report: compiled .o vs delinked target, per objdiff unit.

Byte-exact where it counts: instruction words are compared with relocated fields
masked, and each reloc is compared as (kind, symbol, addend) after resolving
section-relative relocs and unrelocated cross-function branches to a symbol.
100.00% therefore means "identical after linking", the same claim objdiff makes.

Emits an objdiff-report-compatible JSON so build_todo.py consumes it unchanged.
Usage: obj_match.py [--config objdiff.json] [--out -|FILE] [--unit NAME] [-v]
       obj_match.py --text            # human-readable summary
Exit 0 always (a report is not a verdict); 2 on setup error.
"""
import argparse
import difflib
import json
import re
import struct
import sys
from bisect import bisect_right
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SHT_SYMTAB, SHT_REL, SHT_RELA, SHT_NOBITS = 2, 9, 4, 8
STT_OBJECT, STT_FUNC, STT_SECTION = 1, 2, 3
STB_COMDAT = 13          # mwcc's COMDAT/linkonce binding (STB_LOPROC)
DATA_SECTIONS = (".rodata", ".data", ".bss")
# R_ARM_PC24 / _PLT32 / _CALL / _JUMP24 all encode a 24-bit word displacement.
SYMBOLS_TXT = ROOT / "config" / "arm9" / "symbols.txt"
MAPPING_SYMS = ("$a", "$t", "$d")
CALL_RELOCS = {1, 27, 28, 29}
ABS32_RELOCS = {2}
REL32_RELOCS = {3}


def die(msg):
    print(f"obj_match: {msg}", file=sys.stderr)
    sys.exit(2)


class Elf:
    """Minimal ELF32-LE reader (sections, symbols, relocs).

    objdump text can't disambiguate mwcc's many same-named `.text` sections --
    one per function, all at offset 0 -- so we read section indices directly.
    """

    def __init__(self, path):
        self.path = Path(path)
        try:
            self.buf = self.path.read_bytes()
        except OSError as e:
            die(f"cannot read {path}: {e}")
        if self.buf[:4] != b"\x7fELF" or self.buf[4] != 1 or self.buf[5] != 1:
            die(f"{path}: not an ELF32 little-endian object")
        e_shoff, = struct.unpack_from("<I", self.buf, 0x20)
        e_shentsize, e_shnum, e_shstrndx = struct.unpack_from("<HHH", self.buf, 0x2e)
        self.sections = []
        for i in range(e_shnum):
            off = e_shoff + i * e_shentsize
            (name, typ, flags, addr, sh_off, size, link, info, align,
             entsize) = struct.unpack_from("<10I", self.buf, off)
            self.sections.append({
                "idx": i, "name_off": name, "type": typ, "flags": flags,
                "addr": addr, "offset": sh_off, "size": size, "link": link,
                "info": info, "entsize": entsize,
            })
        shstr = self.sections[e_shstrndx]
        for s in self.sections:
            s["name"] = self._cstr(shstr["offset"] + s["name_off"])
        self.symbols = self._read_symbols()
        self.relocs = self._read_relocs()
        self._sec_syms = None
        self._map_syms = None

    def _cstr(self, off):
        end = self.buf.index(b"\0", off)
        return self.buf[off:end].decode("utf-8", "replace")

    def data(self, shndx):
        s = self.sections[shndx]
        if s["type"] == SHT_NOBITS:
            return b"\0" * s["size"]
        return self.buf[s["offset"]:s["offset"] + s["size"]]

    def _read_symbols(self):
        out = []
        for s in self.sections:
            if s["type"] != SHT_SYMTAB:
                continue
            strtab = self.sections[s["link"]]
            n = s["size"] // 16
            for i in range(n):
                off = s["offset"] + i * 16
                name, value, size, info, other, shndx = struct.unpack_from(
                    "<IIIBBH", self.buf, off)
                out.append({
                    "name": self._cstr(strtab["offset"] + name),
                    "value": value, "size": size, "type": info & 0xf,
                    "bind": info >> 4, "shndx": shndx, "symtab": s["idx"],
                })
        return out

    def _read_relocs(self):
        """{target_shndx: {offset: (type, symbol_name, symbol_index, addend)}}"""
        out = {}
        syms = self.symbols
        for s in self.sections:
            if s["type"] not in (SHT_REL, SHT_RELA):
                continue
            rela = s["type"] == SHT_RELA
            step = 12 if rela else 8
            tgt = out.setdefault(s["info"], {})
            for off in range(s["offset"], s["offset"] + s["size"], step):
                if rela:
                    r_off, r_info, r_add = struct.unpack_from("<IIi", self.buf, off)
                else:
                    r_off, r_info = struct.unpack_from("<II", self.buf, off)
                    r_add = None
                sym_i, rtype = r_info >> 8, r_info & 0xff
                sym = syms[sym_i] if sym_i < len(syms) else {"name": "", "type": 0}
                tgt[r_off] = (rtype, sym["name"], sym_i, r_add)
        return out

    def section_symbols(self, shndx):
        """Sorted [(value, name, size)] of the addressable symbols in a section."""
        if self._sec_syms is None:
            self._sec_syms = {}
            for sym in self.symbols:
                if sym["type"] == STT_SECTION or not sym["name"]:
                    continue
                if sym["name"] in MAPPING_SYMS or sym["shndx"] >= len(self.sections):
                    continue
                self._sec_syms.setdefault(sym["shndx"], []).append(
                    (sym["value"], sym["name"], sym["size"]))
            for lst in self._sec_syms.values():
                lst.sort()
        return self._sec_syms.get(shndx, [])

    def isa_at(self, shndx, addr):
        """'a' (ARM), 't' (Thumb) or 'd' (data-in-code) per ELF mapping symbols.

        Without this, a literal-pool constant whose bits happen to look like a
        branch gets symbolized as a call (rand's 0x2a01b0f7 multiplier did).
        """
        if self._map_syms is None:
            self._map_syms = {}
            for sym in self.symbols:
                if sym["name"] in MAPPING_SYMS:
                    self._map_syms.setdefault(sym["shndx"], []).append(
                        (sym["value"], sym["name"][1]))
            for lst in self._map_syms.values():
                lst.sort()
        lst = self._map_syms.get(shndx)
        if not lst:
            return "a"
        i = bisect_right(lst, (addr, "\xff")) - 1
        return lst[i][1] if i >= 0 else "a"

    def resolve(self, shndx, addr):
        """Address in a section -> ('sym', delta), or (None, addr) if unclaimed."""
        lst = self.section_symbols(shndx)
        if not lst:
            return None, addr
        i = bisect_right(lst, (addr, "￿", 0)) - 1
        if i < 0:
            return None, addr
        value, name, _size = lst[i]
        return name, addr - value

    def functions(self):
        """name -> [symbol]; a name can repeat (two distinct SDK `BeginSleep`s)."""
        out = {}
        for sym in self.symbols:
            if sym["type"] != STT_FUNC or not sym["name"] or not sym["size"]:
                continue
            if sym["shndx"] == 0 or sym["shndx"] >= len(self.sections):
                continue
            out.setdefault(sym["name"], []).append(sym)
        return out

    def data_objects(self):
        """name -> (section_class, symbol, comdat) for .rodata/.data/.bss objects.

        .bss is NOBITS, so data() hands back zeros on both sides and the token
        compare degenerates to a size check -- which is the only thing a delinked
        .bss symbol can prove (see memory reference_delink_symbol_extent).

        mwcc COMDAT (bind 13) is C++ RTTI emitted in every TU that touches the
        class and deduplicated at link, so defining it is NOT ownership (memory
        reference_mwcc_comdat_binding). Reported with comdat=True: the bytes are
        still worth checking, the owning-unit attribution is not.
        """
        out = {}
        for sym in self.symbols:
            if sym["type"] != STT_OBJECT or not sym["name"] or not sym["size"]:
                continue
            if sym["shndx"] == 0 or sym["shndx"] >= len(self.sections):
                continue
            sec = self.sections[sym["shndx"]]["name"]
            cls = next((c for c in DATA_SECTIONS if sec.startswith(c)), None)
            if cls and sym["name"] not in out:
                out[sym["name"]] = (cls, sym, sym["bind"] == STB_COMDAT)
        return out



def norm_kind(rtype):
    if rtype in CALL_RELOCS:
        return "call"
    if rtype in ABS32_RELOCS:
        return "abs32"
    if rtype in REL32_RELOCS:
        return "rel32"
    return f"r{rtype}"


def sign24(x):
    return x - (1 << 24) if x & (1 << 23) else x


def tokenize(elf, sym, is_code=True):
    """Instruction stream of one function as comparable tokens.

    is_code=False for data objects: fixed 4-byte stride and no branch
    symbolization, so a data word that happens to look like `b` isn't rewritten.

    Each token is (masked_word, reloc_key). Relocated bits are masked out of the
    word and carried in the key instead, so an unresolved `bl 0` in a compiled .o
    compares equal to the target's linked branch when both name the same symbol.
    Unrelocated branches leaving the function are resolved through the section
    symbol map, so a delinked object that encodes the call as a plain offset
    still compares symbolically.
    """
    shndx, base, size = sym["shndx"], sym["value"], sym["size"]
    data = elf.data(shndx)[base:base + size]
    rel = elf.relocs.get(shndx, {})
    stride = 2 if is_code and elf.isa_at(shndx, base) == "t" else 4
    toks = []
    for off in range(0, len(data) - stride + 1, stride):
        word = int.from_bytes(data[off:off + stride], "little")
        pos = base + off
        r = rel.get(pos)
        key = None
        if r:
            rtype, sname, sym_i, addend = r
            kind = norm_kind(rtype)
            if addend is None:  # REL: implicit addend lives in the field
                if kind == "call":
                    addend = sign24(word & 0xFFFFFF) * 4 + 8
                    word &= ~0xFFFFFF & 0xFFFFFFFF
                else:
                    addend = word
                    word = 0
            else:
                word &= ~0xFFFFFF & 0xFFFFFFFF if kind == "call" else 0
            rsym = elf.symbols[sym_i] if sym_i < len(elf.symbols) else None
            if rsym is not None and rsym["type"] == STT_SECTION:
                # `.text+0x40` on one side vs a named symbol on the other:
                # resolve to whatever symbol actually owns that address.
                named, delta = elf.resolve(rsym["shndx"], addend)
                if named:
                    sname, addend = named, delta
            # An undefined symbol is a cross-TU reference: this object pair
            # cannot prove the two names denote the same address, so the name is
            # reported as a naming gap rather than scored as a byte difference.
            extern = rsym is not None and rsym["shndx"] == 0
            key = (kind, sname, addend, extern)
        elif (is_code and stride == 4 and (word & 0x0E000000) == 0x0A000000
              and elf.isa_at(shndx, pos) == "a"):
            # B/BL with no reloc: symbolize it if it leaves this function.
            imm = sign24(word & 0xFFFFFF) * 4
            tgt = pos + 8 + imm
            if not (base <= tgt < base + size):
                named, delta = elf.resolve(shndx, tgt)
                if named:
                    key = ("call", named, delta, False)
                    word &= ~0xFFFFFF & 0xFFFFFFFF
        toks.append((word, key))
    return toks


_SYM_ADDRS = None
_SYM_SORTED = None


def _load_symbols():
    global _SYM_ADDRS, _SYM_SORTED
    if _SYM_ADDRS is not None:
        return
    _SYM_ADDRS = {}
    if SYMBOLS_TXT.exists():
        pat = re.compile(r"^(\S+)\s+kind:\S+.*\baddr:0x([0-9a-fA-F]+)", re.M)
        for m in pat.finditer(SYMBOLS_TXT.read_text(encoding="utf-8")):
            _SYM_ADDRS.setdefault(m.group(1), int(m.group(2), 16))
    _SYM_SORTED = sorted((a, n) for n, a in _SYM_ADDRS.items())


def sym_addr(name):
    """Absolute address of a target symbol, from symbols.txt (cached)."""
    _load_symbols()
    return _SYM_ADDRS.get(name)


def next_sym_addr(addr):
    """Address of the next target symbol after `addr`, or None."""
    _load_symbols()
    i = bisect_right(_SYM_SORTED, (addr, chr(0x10ffff)))
    return _SYM_SORTED[i][0] if i < len(_SYM_SORTED) else None


def interior_syms(addr, size):
    """Target symbols landing strictly inside [addr, addr+size).

    Scans the WHOLE object, not just the next symbol: mwldarm tail-merges string
    literals, so an interior ref can sit anywhere (ov017 loads 0x02050001 and
    0x02050005, both inside _ZTS6clTask). Each one wants `add:` re-anchoring to
    the container rather than a symbol of its own.
    """
    _load_symbols()
    i = bisect_right(_SYM_SORTED, (addr, chr(0x10ffff)))
    out = []
    while i < len(_SYM_SORTED) and _SYM_SORTED[i][0] < addr + size:
        out.append(_SYM_SORTED[i][1])
        i += 1
    return out


def const_vs_symbol(word, key):
    """Is a plain word equal to the address the other side's reloc names?

    Proves the two encode the same linked value: a fake delink symbol at the
    referenced address (`data_027e0021` vs the literal 0x027e0021), a pool word
    pointing into its own function (`OSi_DisplayExContext+80`), or an
    address-as-value link symbol (SDK_IRQ_STACKSIZE really is 0x800).
    Returns True (proved equal), False (proved different), or None (unknown).
    """
    if key[0] not in ("abs32",):
        return None
    addr = sym_addr(key[1])
    if addr is None:
        return None
    return addr + key[2] == word


def pair_canon(x, y):
    """Canonical (token_a, token_b, gap) for one aligned position.

    When the two sides name a reloc target differently, neither object can prove
    whether the names denote the same address: our source says `sCurrent` or
    `_ZTV5clDPP+8`, the delink invents `data_0205f670` at the referenced address.
    Symbol and addend are then dropped from BOTH sides and the pair is reported
    as a naming gap instead of scored as a byte difference -- the surrounding
    instruction encoding is still compared exactly. Same name on both sides =>
    the addend IS compared.
    """
    kx = x[1] if x is not None else None
    ky = y[1] if y is not None else None
    if kx and ky:
        if kx[1] == ky[1]:
            return (x[0], kx[0], kx[2], kx[1]), (y[0], ky[0], ky[2], ky[1]), None
        return (x[0], kx[0]), (y[0], ky[0]), (kx[1], ky[1])
    # One side relocated, the other a plain word: equal iff the word is the
    # address that reloc resolves to. Proved via symbols.txt where possible,
    # tolerated-and-reported when the name is unknown to it.
    if kx and ky is None and y is not None and x[0] == 0:
        proved = const_vs_symbol(y[0], kx)
        if proved is not None:
            return (None,), (None,) if proved else (y[0],), None
        return (None,), (None,), (kx[1], f"const {y[0]:#x}")
    if ky and kx is None and x is not None and y[0] == 0:
        proved = const_vs_symbol(x[0], ky)
        if proved is not None:
            return (None,), (None,) if proved else (x[0],), None
        return (None,), (None,), (f"const {x[0]:#x}", ky[1])
    return x, y, None


def compare(a, b):
    """(percent, [(target_name, source_name)]) for one function pair."""
    ca, cb, gaps = [], [], []
    for i in range(max(len(a), len(b))):
        x = a[i] if i < len(a) else None
        y = b[i] if i < len(b) else None
        cx, cy, gap = pair_canon(x, y)
        if gap:
            gaps.append(gap)
        if x is not None:
            ca.append(cx)
        if y is not None:
            cb.append(cy)
    if ca == cb:
        return 100.0, gaps
    if not ca or not cb:
        return 0.0, gaps
    ratio = difflib.SequenceMatcher(None, ca, cb, autojunk=False).ratio()
    # Never round a real difference up to a matching-looking 100.00%.
    return min(ratio * 100.0, 99.99), gaps


def load_config(path):
    try:
        cfg = json.loads(Path(path).read_text(encoding="utf-8"))
    except OSError as e:
        die(f"cannot read {path}: {e} (run `make objdiff.json`)")
    except json.JSONDecodeError as e:
        die(f"{path}: {e}")
    return cfg.get("units", [])


def run_unit(unit, cache, claimed, verbose):
    tgt_path, base_path = unit["target_path"], unit["base_path"]
    for p in (tgt_path, base_path):
        if not (ROOT / p).is_file():
            if verbose:
                print(f"  skip {unit['name']}: missing {p}", file=sys.stderr)
            return None
    tgt = cache.setdefault(tgt_path, Elf(ROOT / tgt_path))
    base = Elf(ROOT / base_path)
    tfuncs = tgt.functions()
    fns, missing, gaps = [], [], set()
    bfuncs = sorted(((n, s) for n, syms in base.functions().items() for s in syms),
                    key=lambda kv: kv[1]["value"])
    for name, sym in bfuncs:
        cands = tfuncs.get(name)
        if not cands:
            missing.append(name)
            continue
        btoks = tokenize(base, sym)
        # Duplicate target names (the two SDK `BeginSleep`s): take the best pair,
        # but never one another unit already matched exactly -- letting two units
        # claim the same target symbol invents a 100% for the second one.
        free = [c for c in cands
                if (tgt_path, c["value"]) not in claimed] or cands
        tsym, pct, fn_gaps = max(
            ((c,) + compare(tokenize(tgt, c), btoks) for c in free),
            key=lambda r: r[1])
        if pct == 100.0:
            claimed.add((tgt_path, tsym["value"]))
        gaps.update(fn_gaps)
        fns.append({
            "name": name,
            "address": tsym["value"],
            "size": tsym["size"],
            "fuzzy_match_percent": pct,
        })
    # Data objects. Homed only -- an `extern` declaration is UND, so it never
    # appears here and the object stays attributed to whoever defines it.
    tdata = tgt.data_objects()
    objs, missing_data = [], []
    for name, (cls, bsym, comdat) in sorted(base.data_objects().items(),
                                            key=lambda kv: kv[1][1]["value"]):
        hit = tdata.get(name)
        if hit is None:
            if not comdat:                # COMDAT absence is expected, not a gap
                missing_data.append(name)     # kept out of `missing` (functions)
            continue
        tcls, tsym, _ = hit
        # A delinked data symbol's "size" is only the distance to the next symbol
        # (no size= exists), so it is an EXTENT, not sizeof. Score content over the
        # common prefix and classify the size difference separately, otherwise
        # inter-object alignment padding reads as a content mismatch.
        ttoks = tokenize(tgt, tsym, is_code=False)
        btoks = tokenize(base, bsym, is_code=False)
        k = min(len(ttoks), len(btoks))
        pct, obj_gaps = compare(ttoks[:k], btoks[:k])
        gaps.update(obj_gaps)
        tail = tgt.data(tsym["shndx"])[tsym["value"] + bsym["size"]:
                                       tsym["value"] + tsym["size"]]
        addr = sym_addr(name)
        inside = interior_syms(addr, bsym["size"]) if addr else []
        if pct != 100.0:
            status = "diff"                     # real content difference
        elif inside:
            status = "over"                     # symbol(s) land inside our object
        elif bsym["size"] == tsym["size"]:
            status = "ok"
        elif any(tail):
            status = "short"                    # trailing content, not padding
        else:
            # A zero tail is only PADDING if it is short enough to be the next
            # object's alignment; a longer run of zeros is undeclared content that
            # happens to be zero (CLAUDE.md "Homing globals": unexplained trailing
            # bytes are content). Address alignment is an upper bound on the real
            # section align, so this deliberately under-reports rather than cry wolf.
            nxt = next_sym_addr(addr) if addr else None
            slack = tsym["size"] - bsym["size"]
            status = ("pad" if nxt is None or slack < min(nxt & -nxt, 32)
                      else "zeros")
        # A delinked object's data sections carry addr=0 with section-relative
        # symbol values, so the absolute address has to come from symbols.txt.
        objs.append({
            "name": name,
            "address": addr or tsym["value"],
            "size": tsym["size"],
            "source_size": bsym["size"],
            "section": tcls,
            "status": status,
            "comdat": comdat,
            "interior": inside,
            "fuzzy_match_percent": pct,
        })
    return {"name": unit["name"], "functions": fns, "objects": objs,
            "missing": missing, "missing_data": missing_data,
            "gaps": sorted(gaps)}


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--config", default=str(ROOT / "objdiff.json"))
    ap.add_argument("--out", default="-", help="report JSON path, or - for stdout")
    ap.add_argument("--unit", action="append", help="limit to unit name(s)")
    ap.add_argument("--text", action="store_true", help="human summary on stdout")
    ap.add_argument("-v", "--verbose", action="store_true")
    args = ap.parse_args()

    units = load_config(args.config)
    if args.unit:
        want = set(args.unit)
        units = [u for u in units if u["name"] in want]
        if not units:
            die(f"no unit matched {sorted(want)}")

    cache, claimed, out_units, missing_any, gaps_any = {}, set(), [], [], []
    for unit in units:
        res = run_unit(unit, cache, claimed, args.verbose)
        if res is None:
            continue
        if res["missing"]:
            missing_any.append((res["name"], res["missing"]))
        if res["gaps"]:
            gaps_any.append((res["name"], res["gaps"]))
        out_units.append({"name": res["name"], "functions": res["functions"],
                          "objects": res["objects"]})

    # Two units can compile a same-named static function while the target has
    # only one: drop the loser's phantom row rather than report a 3% "match".
    exact = {(u["name"], f["address"]) for u in out_units
             for f in u["functions"] if f["fuzzy_match_percent"] == 100.0}
    exact_addrs = {addr for _u, addr in exact}
    for u in out_units:
        u["functions"] = [
            f for f in u["functions"]
            if f["fuzzy_match_percent"] == 100.0
            or f["address"] not in exact_addrs
        ]

    report = {"units": out_units}
    if args.text:
        for u in out_units:
            done = sum(1 for f in u["functions"] if f["fuzzy_match_percent"] == 100.0)
            n = len(u["functions"])
            flag = "" if done == n else "  <-- "
            print(f"{done:3}/{n:<3} {u['name']}{flag}" + ", ".join(
                f"{f['name']} {f['fuzzy_match_percent']:.2f}%"
                for f in u["functions"] if f["fuzzy_match_percent"] < 100.0))
    else:
        text = json.dumps(report, indent=1)
        if args.out == "-":
            sys.stdout.write(text)
        else:
            Path(args.out).write_text(text, encoding="utf-8")

    total = sum(len(u["functions"]) for u in out_units)
    done = sum(1 for u in out_units for f in u["functions"]
               if f["fuzzy_match_percent"] == 100.0)
    print(f"obj_match: {done}/{total} functions matched in {len(out_units)} units",
          file=sys.stderr)
    for name, miss in missing_any:
        if args.verbose:
            print(f"  {name}: not in target: {', '.join(miss)}", file=sys.stderr)
    if missing_any and not args.verbose:
        print(f"  ({len(missing_any)} units have compiled functions absent from the "
              f"target; -v to list)", file=sys.stderr)
    # Cross-TU references (calls, and data relocs such as vtable slots) where our
    # source and symbols.txt disagree on the name: bytes match, but the tree would
    # not link. Candidates for a rename.
    npairs = len({p for _u, g in gaps_any for p in g})
    if gaps_any:
        print(f"  ({npairs} unresolved symbol names across {len(gaps_any)} units"
              f"{'; -v to list' if not args.verbose else ''})", file=sys.stderr)
    if args.verbose:
        for name, g in gaps_any:
            for tname, bname in g:
                print(f"  NAME {name}: target `{tname}` <- source `{bname}`",
                      file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
