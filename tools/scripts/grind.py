#!/usr/bin/env python3
import argparse
import difflib
import glob
import itertools
import os
import re
import shlex
import subprocess
import sys
import threading
import time
from collections import Counter
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import fn_diff as fd  # noqa: E402
from common import BUILD, GAP_O, ROOT, die, game_sources  # noqa: E402

WORK = BUILD / "scratch/grind"
REG_RE = re.compile(r"\b(r\d+|sl|fp|ip|lr|sp)\b")
SLOT_RE = re.compile(r"\[sp, #\d+\]")
_lock = threading.Lock()
_n = [0]


def norm(line):
    return REG_RE.sub("R", SLOT_RE.sub("[sp,#S]", line))


def func_name_from_symbol(sym):
    m = re.search(r"(func_[0-9a-fA-F]{8})", sym)
    return m.group(1) if m else sym


def find_tu(name):
    hits = []
    for p in game_sources():
        if re.search(
            rf"^\S[^;]*\b{re.escape(name)}\s*\(|^//\s+arm_func_end {re.escape(name)}\b", p.read_text(errors="replace"), re.M
        ):
            hits.append(p)
    if len(hits) != 1:
        die(f"error: {len(hits)} TUs define {name} ({', '.join(str(h) for h in hits)}); pass --tu")
    return hits[0]


def locate(lines, name):
    sig = re.compile(rf"^\S[^;]*\b{re.escape(name)}\s*\(")
    for i, l in enumerate(lines):
        if l.lstrip().startswith("//") or not sig.match(l):
            continue
        depth, seen = 0, False
        for j in range(i, len(lines)):
            code = re.sub(r"//.*", "", lines[j])
            code = re.sub(r'"(\\.|[^"\\])*"', '""', code)
            if ";" in code and not seen and "{" not in code:
                break  # a declaration, not a definition
            depth += code.count("{") - code.count("}")
            seen = seen or "{" in code
            if seen and depth == 0:
                return i, j
    for i, l in enumerate(lines):  # not yet decompiled: insert after the asm-comment block
        if re.match(rf"//\s+arm_func_end {re.escape(name)}\b", l):
            return i + 1, i
    die(f"error: no definition of {name} (and no `arm_func_end {name}` comment) found")


def compile_cmd(tu):
    rel = tu.relative_to(ROOT).as_posix()
    obj = "build/" + str(Path(rel).with_suffix(".o").as_posix())
    r = subprocess.run(["make", "-n", "-W", rel, obj, "V=1"], cwd=ROOT, capture_output=True, text=True)
    for line in r.stdout.splitlines():
        if "mwccarm" in line and " -c " in line:
            toks = shlex.split(line)
            k = next(i for i, t in enumerate(toks) if "mwccarm" in t)
            toks = toks[k:toks.index("-c")]
            toks = [t for t in toks if t not in ("-gccdep", "-MD")]
            toks[0] = str(ROOT / toks[0])
            return toks
    die(f"error: could not get compile line from make for {tu}")


class Harness:
    def __init__(self, tu, name, symbol, target, target_symbol, keep_dir=None):
        self.tu, self.symbol, self.tsym = tu, symbol, target_symbol or symbol
        self.lines = tu.read_text(newline="").splitlines(True)
        self.s, self.e = locate(self.lines, name)
        self.base = "".join(self.lines[self.s:self.e + 1])  # empty when inserting
        self.cmd = compile_cmd(tu)
        self.ext = tu.suffix
        WORK.mkdir(parents=True, exist_ok=True)
        self.tgt = self.extract(target, self.tsym)
        if not self.tgt:
            die(f"error: {self.tsym} not in target {target}")
        self.regions = self.make_regions()
        self.env = dict(os.environ, LM_LICENSE_FILE=str(ROOT / "tools/mwccarm/license.dat"))

    @staticmethod
    def extract(obj, symbol):
        items, _ = fd.extract_items(fd.objdump(obj), symbol)
        return list(zip(*fd.relativize(items, False)))

    def make_regions(self):
        regs, lo = [], 0
        for i, (_, back) in enumerate(self.tgt):
            if back:
                regs.append((lo, i + 1))
                lo = i + 1
        if lo < len(self.tgt):
            regs.append((lo, len(self.tgt)))
        return regs

    def splice(self, body):
        if not body.endswith("\n"):
            body += "\n"
        return "".join(self.lines[:self.s]) + body + "".join(self.lines[self.e + 1:])

    def build(self, body, keep=None):
        with _lock:
            _n[0] += 1
            k = _n[0]
        base = WORK / (keep or f"v{k}")
        src, obj = base.with_suffix(self.ext), base.with_suffix(".o")
        src.write_text(self.splice(body), newline="")
        r = subprocess.run(self.cmd + ["-c", "-o", str(obj), str(src)], cwd=ROOT, capture_output=True, text=True, env=self.env)
        if r.returncode != 0 or not obj.exists():
            if not keep:
                rm(src)
            return None, (r.stdout + r.stderr).strip()[-800:]
        mine = [t for t, _ in self.extract(obj, self.symbol)]
        if not keep:
            rm(src, obj)
        return mine, None

    def score(self, body, keep=None):
        mine, err = self.build(body, keep)
        if mine is None:
            return None, err
        T = [t for t, _ in self.tgt]
        Tn, Mn = [norm(t) for t in T], [norm(m) for m in mine]
        res = {"raw": dcount(T, mine), "norm": dcount(Tn, Mn), "insn": len(mine),
               "regions": [(region(T, mine, a, b), region(Tn, Mn, a, b)) for a, b in self.regions],
               "swaps": swaps(T, mine), "mine": mine}
        return res, None


def rm(*paths):
    for p in paths:
        for _ in range(20):  # Windows: mwcc may still hold the file briefly
            try:
                p.unlink(missing_ok=True)
                break
            except PermissionError:
                time.sleep(0.05)


def dcount(A, B):
    sm = difflib.SequenceMatcher(None, A, B, autojunk=False)
    return sum((i2 - i1) + (j2 - j1) for tag, i1, i2, j1, j2 in sm.get_opcodes() if tag != "equal")


def region(T, M, lo, hi):
    sm = difflib.SequenceMatcher(None, T, M, autojunk=False)
    n = 0
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag != "equal" and i2 > lo and i1 < hi:
            n += (i2 - i1) + (j2 - j1)
    return n


def swaps(T, M):
    c = Counter()
    sm = difflib.SequenceMatcher(None, [norm(t) for t in T], [norm(m) for m in M], autojunk=False)
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag != "equal":
            continue
        for t, m in zip(T[i1:i2], M[j1:j2]):
            if t != m:
                for a, b in zip(REG_RE.findall(t), REG_RE.findall(m)):
                    if a != b:
                        c[(a, b)] += 1
    return c


def sub(text, old, new, count=1):
    assert old in text, f"not found: {old!r}"
    return text.replace(old, new, count)


def perm_decls(text, decls, prefix=None):
    names = [d.strip().rstrip(";") for d in decls.split(";") if d.strip()]
    lines = [re.search(rf"^([ \t]*){re.escape(n)};[ \t]*\r?\n", text, re.M) for n in names]
    for n, m in zip(names, lines):
        assert m, f"declaration not found: {n};"
    indent, nl = lines[0].group(1), "\r\n" if "\r\n" in text else "\n"
    block = "".join(m.group(0) for m in lines)
    assert block in text, "declarations are not adjacent in that order"
    out = {}
    for p in itertools.permutations(names):
        key = (prefix or "ord_") + "_".join(n.split()[-1].lstrip("*") for n in p)
        out[key] = text.replace(block, "".join(f"{indent}{n};{nl}" for n in p), 1)
    return out


def load_variants(args, base):
    V = {}
    if args.variants:
        g = {"base": base, "sub": sub, "perm_decls": perm_decls, "ROOT": ROOT, "re": re, "itertools": itertools}
        exec(compile(Path(args.variants).read_text(), args.variants, "exec"), g)
        if "V" not in g:
            die(f"error: {args.variants} must define V")
        V.update(g["V"])
    for pat in args.bodies or []:
        for f in sorted(glob.glob(pat)):
            V[Path(f).stem] = Path(f).read_text(newline="")
    if args.perm_decls:
        V.update(perm_decls(base, args.perm_decls))
    return V


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("symbol")
    ap.add_argument("--tu")
    ap.add_argument("--func", help="source name of the function (default: derived from symbol)")
    ap.add_argument("--target", default=str(GAP_O))
    ap.add_argument("--target-symbol")
    ap.add_argument("--variants", help="python file defining V = {name: function text}")
    ap.add_argument("--bodies", nargs="*", help="globs of files, each a whole function definition")
    ap.add_argument("--perm-decls", help="'a;b;c': try every order of these adjacent declaration lines")
    ap.add_argument("--show", help="print the full diff of this variant (or 'base')")
    ap.add_argument("--write", nargs=2, metavar=("NAME", "OUT"), help="write a variant's text to a file")
    ap.add_argument("--keep", help="keep this variant's spliced TU and .o in build/scratch/grind/")
    ap.add_argument("--jobs", type=int, default=os.cpu_count() or 4)
    ap.add_argument("--top", type=int, default=0, help="print only the N best rows")
    args = ap.parse_args()

    name = args.func or func_name_from_symbol(args.symbol)
    tu = Path(args.tu) if args.tu else find_tu(name)
    tu = tu if tu.is_absolute() else ROOT / tu
    H = Harness(tu, name, args.symbol, args.target, args.target_symbol)
    V = load_variants(args, H.base)
    if args.write:
        nm, out = args.write
        Path(out).write_text(H.base if nm == "base" else V[nm], newline="")
        print(f"wrote {out}")
        return 0
    if not H.base and not V:
        die(f"error: {name} has no definition yet; pass --bodies or --variants")
    if not V or args.show == "base" or args.keep == "base":
        V = {"base": H.base, **V}
    print(f"{tu.relative_to(ROOT).as_posix()}:{H.s + 1}-{H.e + 1}  target={len(H.tgt)} insns  regions="
          + " ".join(f"R{i + 1}[{a}:{b}]" for i, (a, b) in enumerate(H.regions)))

    if args.show:
        res, err = H.score(V[args.show], keep=args.keep if args.keep == args.show else None)
        if err:
            print(err)
            return 2
        T = [t for t, _ in H.tgt]
        print("\n".join(difflib.unified_diff(T, res["mine"], "target", "mine", n=3, lineterm="")))
        print(f"\nraw {res['raw']} norm {res['norm']} insn {res['insn']}")
        return 0 if res["raw"] == 0 else 1

    def one(nm):
        return nm, H.score(V[nm], keep=nm if nm == args.keep else None)

    rows = []
    with ThreadPoolExecutor(args.jobs) as ex:
        for nm, (res, err) in ex.map(one, list(V)):
            if err:
                rows.append((10 ** 9, 10 ** 9, nm, None, err))
            else:
                rows.append((res["raw"], res["norm"], nm, res, None))
    rows.sort(key=lambda r: (r[0], r[1], r[2]))
    for raw, nm_, nm, res, err in rows[: args.top or None]:
        if err:
            el = [l for l in err.splitlines() if re.search(r":\d+: \S", l)] or err.splitlines()[-1:]
            print(f"{nm:24s} COMPILE FAIL: {el[0] if el else ''}")
            continue
        reg = " ".join(f"R{i + 1}={a}/{b}" for i, (a, b) in enumerate(res["regions"]) if a or b) or "-"
        sw = " ".join(f"{a}>{b}x{c}" for (a, b), c in res["swaps"].most_common(6))
        tag = "MATCH" if raw == 0 else f"raw {raw:4d} norm {res['norm']:4d}"
        print(f"{nm:24s} {tag:18s} insn {res['insn']:4d} | {reg} | {sw}")
    return 0 if rows and rows[0][0] == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
