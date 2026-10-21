#!/usr/bin/env python3
import argparse
import difflib
import json
import struct
import sys
from bisect import bisect_right
from pathlib import Path

import common
from common import ROOT, read_symbols

SHT_SYMTAB, SHT_REL, SHT_RELA, SHT_NOBITS = 2, 9, 4, 8
STT_OBJECT, STT_FUNC, STT_SECTION = 1, 2, 3
STB_COMDAT = 13
DATA_SECTIONS = (".rodata", ".data", ".bss")
MAPPING_SYMS = ("$a", "$t", "$d")
CALL_RELOCS = {1, 27, 28, 29}
ABS32_RELOCS = {2}
REL32_RELOCS = {3}
BRANCH_MASK = ~0xFFFFFF & 0xFFFFFFFF


def die(msg):
    common.die(f"obj_match: {msg}")


class Elf:
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
            (name, typ, flags, addr, sh_off, size, link, info, align,
             entsize) = struct.unpack_from("<10I", self.buf, e_shoff + i * e_shentsize)
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
        self._funcs = None
        self._data_objs = None
        self._map_syms = None

    def _cstr(self, off):
        return self.buf[off:self.buf.index(b"\0", off)].decode("utf-8", "replace")

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
            for off in range(s["offset"], s["offset"] + (s["size"] // 16) * 16, 16):
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
            tgt = out.setdefault(s["info"], {})
            for off in range(s["offset"], s["offset"] + s["size"], 12 if rela else 8):
                if rela:
                    r_off, r_info, r_add = struct.unpack_from("<IIi", self.buf, off)
                else:
                    r_off, r_info = struct.unpack_from("<II", self.buf, off)
                    r_add = None
                sym_i, rtype = r_info >> 8, r_info & 0xff
                name = syms[sym_i]["name"] if sym_i < len(syms) else ""
                tgt[r_off] = (rtype, name, sym_i, r_add)
        return out

    def section_symbols(self, shndx):
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
        """'a'/'t'/'d' from mapping symbols, so pool words never decode as branches."""
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
        lst = self.section_symbols(shndx)
        i = bisect_right(lst, (addr, "￿", 0)) - 1
        if i < 0:
            return None, addr
        value, name, _size = lst[i]
        return name, addr - value

    def functions(self):
        """name -> [symbol]; names can repeat (two SDK `BeginSleep`s)."""
        if self._funcs is None:
            self._funcs = {}
            for sym in self.symbols:
                if sym["type"] != STT_FUNC or not sym["name"] or not sym["size"]:
                    continue
                if sym["shndx"] == 0 or sym["shndx"] >= len(self.sections):
                    continue
                self._funcs.setdefault(sym["name"], []).append(sym)
        return self._funcs

    def data_objects(self):
        """name -> (section_class, symbol, comdat)."""
        if self._data_objs is None:
            self._data_objs = {}
            for sym in self.symbols:
                if sym["type"] != STT_OBJECT or not sym["name"] or not sym["size"]:
                    continue
                if sym["shndx"] == 0 or sym["shndx"] >= len(self.sections):
                    continue
                sec = self.sections[sym["shndx"]]["name"]
                cls = next((c for c in DATA_SECTIONS if sec.startswith(c)), None)
                if cls and sym["name"] not in self._data_objs:
                    self._data_objs[sym["name"]] = (cls, sym, sym["bind"] == STB_COMDAT)
        return self._data_objs


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
    """[(masked_word, reloc_key)]; relocated bits move from the word into the key."""
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
            if addend is None:
                if kind == "call":
                    addend = sign24(word & 0xFFFFFF) * 4 + 8
                    word &= BRANCH_MASK
                else:
                    addend = word
                    word = 0
            else:
                word &= BRANCH_MASK if kind == "call" else 0
            rsym = elf.symbols[sym_i] if sym_i < len(elf.symbols) else None
            if rsym is not None and rsym["type"] == STT_SECTION:
                named, delta = elf.resolve(rsym["shndx"], addend)
                if named:
                    sname, addend = named, delta
            extern = rsym is not None and rsym["shndx"] == 0
            key = (kind, sname, addend, extern)
        elif (is_code and stride == 4 and (word & 0x0E000000) == 0x0A000000
              and elf.isa_at(shndx, pos) == "a"):
            tgt = pos + 8 + sign24(word & 0xFFFFFF) * 4
            if not (base <= tgt < base + size):
                named, delta = elf.resolve(shndx, tgt)
                if named:
                    key = ("call", named, delta, False)
                    word &= BRANCH_MASK
        toks.append((word, key))
    return toks


_SYM_ADDRS = None
_SYM_SORTED = None


def _load_symbols():
    global _SYM_ADDRS, _SYM_SORTED
    if _SYM_ADDRS is not None:
        return
    _SYM_ADDRS = {}
    if common.SYMBOLS.exists():
        for sym in read_symbols():
            _SYM_ADDRS.setdefault(sym.name, sym.addr)
    _SYM_SORTED = sorted((a, n) for n, a in _SYM_ADDRS.items())


def sym_addr(name):
    _load_symbols()
    return _SYM_ADDRS.get(name)


def _sym_index_after(addr):
    _load_symbols()
    return bisect_right(_SYM_SORTED, (addr, chr(0x10ffff)))


def next_sym_addr(addr):
    i = _sym_index_after(addr)
    return _SYM_SORTED[i][0] if i < len(_SYM_SORTED) else None


def interior_syms(addr, size):
    """Every target symbol strictly inside [addr, addr+size): mwldarm tail-merges strings."""
    i = _sym_index_after(addr)
    out = []
    while i < len(_SYM_SORTED) and _SYM_SORTED[i][0] < addr + size:
        out.append(_SYM_SORTED[i][1])
        i += 1
    return out


def const_vs_symbol(word, key):
    """True/False if a plain word provably equals/differs from an abs32 reloc's target; None if unknown."""
    if key[0] != "abs32":
        return None
    addr = sym_addr(key[1])
    if addr is None:
        return None
    return addr + key[2] == word


def pair_canon(x, y):
    """(token_a, token_b, naming_gap) for one aligned position."""
    kx = x[1] if x is not None else None
    ky = y[1] if y is not None else None
    if kx and ky:
        if kx[1] == ky[1]:
            return (x[0], kx[0], kx[2], kx[1]), (y[0], ky[0], ky[2], ky[1]), None
        return (x[0], kx[0]), (y[0], ky[0]), (kx[1], ky[1])
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
    """(percent, [(target_name, source_name)])"""
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
    return min(ratio * 100.0, 99.99), gaps


def load_config(path):
    try:
        cfg = json.loads(Path(path).read_text(encoding="utf-8"))
    except OSError as e:
        die(f"cannot read {path}: {e} (run `make objdiff.json`)")
    except json.JSONDecodeError as e:
        die(f"{path}: {e}")
    return cfg.get("units", [])


def data_status(tgt, tsym, bsym, pct, addr, inside):
    if pct != 100.0:
        return "diff"
    if inside:
        return "over"
    if bsym["size"] == tsym["size"]:
        return "ok"
    tail = tgt.data(tsym["shndx"])[tsym["value"] + bsym["size"]:
                                   tsym["value"] + tsym["size"]]
    if any(tail):
        return "short"
    # zero tail counts as padding only if it fits the next object's alignment
    nxt = next_sym_addr(addr) if addr else None
    slack = tsym["size"] - bsym["size"]
    return "pad" if nxt is None or slack < min(nxt & -nxt, 32) else "zeros"


def run_unit(unit, cache, claimed, verbose):
    tgt_path, base_path = unit["target_path"], unit["base_path"]
    for p in (tgt_path, base_path):
        if not (ROOT / p).is_file():
            if verbose:
                print(f"  skip {unit['name']}: missing {p}", file=sys.stderr)
            return None
    # not setdefault: it would parse the shared target ELF once per unit
    tgt = cache.get(tgt_path)
    if tgt is None:
        tgt = cache[tgt_path] = Elf(ROOT / tgt_path)
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
        # duplicate target names: never pair with one another unit already matched
        free = [c for c in cands if (tgt_path, c["value"]) not in claimed] or cands
        tsym, pct, fn_gaps = max(
            ((c,) + compare(tokenize(tgt, c), btoks) for c in free),
            key=lambda r: r[1])
        if pct == 100.0:
            claimed.add((tgt_path, tsym["value"]))
        gaps.update(fn_gaps)
        fns.append({
            "name": name,
            "address": tsym["value"],
            "section": tgt.sections[tsym["shndx"]]["name"],
            "size": tsym["size"],
            "fuzzy_match_percent": pct,
        })
    tdata = tgt.data_objects()
    objs, missing_data = [], []
    for name, (cls, bsym, comdat) in sorted(base.data_objects().items(),
                                            key=lambda kv: kv[1][1]["value"]):
        hit = tdata.get(name)
        if hit is None:
            if not comdat:
                missing_data.append(name)
            continue
        tcls, tsym, _ = hit
        # target size is an extent (distance to next symbol): score the common prefix
        ttoks = tokenize(tgt, tsym, is_code=False)
        btoks = tokenize(base, bsym, is_code=False)
        k = min(len(ttoks), len(btoks))
        pct, obj_gaps = compare(ttoks[:k], btoks[:k])
        gaps.update(obj_gaps)
        addr = sym_addr(name)
        inside = interior_syms(addr, bsym["size"]) if addr else []
        objs.append({
            "name": name,
            "address": addr or tsym["value"],
            "size": tsym["size"],
            "source_size": bsym["size"],
            "section": tcls,
            "status": data_status(tgt, tsym, bsym, pct, addr, inside),
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

    # a same-named static compiled by two units: drop the loser's phantom row
    exact_addrs = {f["address"] for u in out_units for f in u["functions"]
                   if f["fuzzy_match_percent"] == 100.0}
    for u in out_units:
        u["functions"] = [f for f in u["functions"]
                          if f["fuzzy_match_percent"] == 100.0
                          or f["address"] not in exact_addrs]

    if args.text:
        for u in out_units:
            done = sum(1 for f in u["functions"] if f["fuzzy_match_percent"] == 100.0)
            n = len(u["functions"])
            flag = "" if done == n else "  <-- "
            print(f"{done:3}/{n:<3} {u['name']}{flag}" + ", ".join(
                f"{f['name']} {f['fuzzy_match_percent']:.2f}%"
                for f in u["functions"] if f["fuzzy_match_percent"] < 100.0))
    else:
        text = json.dumps({"units": out_units}, indent=1)
        if args.out == "-":
            sys.stdout.write(text)
        else:
            Path(args.out).write_text(text, encoding="utf-8")

    total = sum(len(u["functions"]) for u in out_units)
    done = sum(1 for u in out_units for f in u["functions"]
               if f["fuzzy_match_percent"] == 100.0)
    print(f"obj_match: {done}/{total} functions matched in {len(out_units)} units",
          file=sys.stderr)
    if args.verbose:
        for name, miss in missing_any:
            print(f"  {name}: not in target: {', '.join(miss)}", file=sys.stderr)
    elif missing_any:
        print(f"  ({len(missing_any)} units have compiled functions absent from the "
              f"target; -v to list)", file=sys.stderr)
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
