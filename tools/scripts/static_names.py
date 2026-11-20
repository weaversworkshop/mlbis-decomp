import argparse
import difflib
import re
import subprocess
import sys
from collections import defaultdict
from pathlib import Path

from common import CONFIG, ROOT, die

EXPORTS_DIR = Path(ROOT.parent / "static_names")
SOURCES = (("DX", "mlbisdx-static-exports.txt"), ("DT", "mldt-static-exports.txt"))
LINE_RE = re.compile(r"^([0-9]) ([0-9A-Fa-f]{8}) (.+?)\s*$")
CXXFILT = "arm-none-eabi-c++filt"
LIB_NS = ("nn::", "nw::", "std::", "__rw::", "adsl::")
PLACEHOLDER_RE = re.compile(r"^(func_|unk_virtual|data_)")


def split_top(s, sep):
    out, depth, cur, i = [], 0, "", 0
    while i < len(s):
        c = s[i]
        if c in "<(":
            depth += 1
        elif c in ">)":
            depth -= 1
        if depth == 0 and s.startswith(sep, i):
            out.append(cur)
            cur = ""
            i += len(sep)
            continue
        cur += c
        i += 1
    out.append(cur)
    return out


def parse_sig(text):
    text = text.strip()
    const = text.endswith(" const")
    if const:
        text = text[:-6]
    depth = 0
    for i, c in enumerate(text):
        if c == "<":
            depth += 1
        elif c == ">":
            depth -= 1
        elif c == "(" and depth == 0:
            qual, params = text[:i], text[i + 1:text.rindex(")")]
            break
    else:
        return None
    parts = split_top(qual, "::")
    params = [p.strip() for p in split_top(params, ",") if p.strip()]
    if params == ["void"]:
        params = []
    return "::".join(parts[:-1]), parts[-1], params, const


def strip_cl(name):
    return re.sub(r"^(~?)(cl|in|ns)(?=[A-Z])", r"\1", name)


def norm_type(t):
    t = re.sub(r"\bunsigned long\b", "unsigned int", t)
    t = re.sub(r"\blong\b(?! long)", "int", t)
    t = re.sub(r"\b(cl|in|ns)(?=[A-Z])", "", t)
    return re.sub(r"\s+", " ", t).replace(" *", "*").replace(" &", "&").strip()


def class_keys(cls):
    parts = [re.sub(r"^Ctr(?=[A-Z])", "", strip_cl(p)) for p in cls.split("::")]
    return {parts[-1].lower(), "".join(parts).lower()}


def load_exports():
    funcs, classes = [], set()
    for tag, fname in SOURCES:
        path = EXPORTS_DIR / fname
        if not path.exists():
            die(f"missing {path}")
        seen = set()
        for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
            m = LINE_RE.match(line)
            if not m or m[3] in seen:
                continue
            seen.add(m[3])
            name = m[3]
            if name.startswith(LIB_NS):
                continue
            for pre in ("vtable for ", "typeinfo for ", "typeinfo name for "):
                if name.startswith(pre):
                    classes.add((tag, name[len(pre):]))
                    break
            else:
                sig = parse_sig(name) if m[1] == "0" else None
                if sig:
                    funcs.append((tag,) + sig)
                    if sig[0]:
                        classes.add((tag, sig[0]))
                elif "::" in name:
                    cls, member = name.rsplit("::", 1)
                    classes.add((tag, cls))
                    funcs.append((tag, cls, member, None, False))
    return funcs, classes


def symbol_files():
    yield "main", CONFIG / "symbols.txt"
    for mod in ("itcm", "dtcm"):
        yield mod, CONFIG / mod / "symbols.txt"
    for d in sorted((CONFIG / "overlays").iterdir()):
        yield d.name, d / "symbols.txt"


def load_ours():
    rows = []
    for mod, path in symbol_files():
        if not path.exists():
            continue
        for line in path.read_text(encoding="utf-8").splitlines():
            m = re.match(r"^(\S+) kind:(\w+)\S* addr:0x([0-9a-fA-F]+)", line)
            if m and m[2] == "function":
                rows.append((mod, int(m[3], 16), m[1]))
    mangled = [r[2] for r in rows if r[2].startswith("_Z")]
    try:
        out = subprocess.run([CXXFILT], input="\n".join(mangled), capture_output=True,
                             text=True, check=True).stdout.splitlines()
    except (OSError, subprocess.CalledProcessError) as e:
        die(f"{CXXFILT}: {e}")
    dem = dict(zip(mangled, out))
    methods, frees = [], []
    for mod, addr, name in rows:
        if name.startswith("_Z"):
            d = dem.get(name, name)
            if d.startswith(("non-virtual thunk", "virtual thunk")):
                continue
            sig = parse_sig(d)
            if not sig:
                continue
            (methods if sig[0] else frees).append((mod, addr) + sig)
        elif not PLACEHOLDER_RE.match(name):
            frees.append((mod, addr, "", name, None, False))
    return methods, frees


def fmt_params(params):
    return "" if params is None else "(" + ", ".join(params) + ")"


def index_3ds(funcs, classes):
    by_key = defaultdict(set)
    for tag, cls in classes:
        for k in class_keys(cls):
            by_key[k].add(cls)
    members = defaultdict(list)
    for tag, cls, name, params, const in funcs:
        members[cls].append((tag, name, params, const))
    return by_key, members


def find_params(query, funcs):
    rx = re.compile(query, re.I)
    rows = defaultdict(set)
    for t, c, n, p, k in funcs:
        if p is not None and rx.search(", ".join(norm_type(x) for x in p)):
            rows[(c, n, tuple(p), k)].add(t)
    if not rows:
        die(f"no parameter list matches {query!r}", 1)
    for (c, n, p, k), t in sorted(rows.items()):
        print(f"{'+'.join(sorted(t)):5} {c + '::' if c else ''}{n}{fmt_params(list(p))}{' const' if k else ''}")


def lookup(query, funcs, classes):
    by_key, members = index_3ds(funcs, classes)
    q = strip_cl(query).lower()
    hits = sorted(by_key.get(q, ()))
    if not hits:
        rx = re.compile(query, re.I)
        hits = sorted({c for _, c in classes if rx.search(c)})
    if not hits:
        names = sorted({n for _, c, n, p, _ in funcs if re.search(query, n, re.I)
                        for _ in [0]})
        rows = [(t, c, n, p, k) for t, c, n, p, k in funcs if n in names]
        if not rows:
            die(f"no class or function matches {query!r}", 1)
        for t, c, n, p, k in sorted(rows, key=lambda r: (r[1], r[2])):
            print(f"{t}  {c + '::' if c else ''}{n}{fmt_params(p)}{' const' if k else ''}")
        return
    for cls in hits:
        print(f"== {cls}")
        rows = defaultdict(set)
        for tag, name, params, const in members.get(cls, []):
            rows[(name, tuple(params) if params is not None else None, const)].add(tag)
        for (name, params, const), tags in sorted(rows.items(), key=lambda r: r[0][0].lower()):
            p = fmt_params(list(params) if params is not None else None)
            print(f"  {'+'.join(sorted(tags)):5} {name}{p}{' const' if const else ''}")


def survey(funcs, classes, out):
    by_key, members = index_3ds(funcs, classes)
    methods, frees = load_ours()
    ours = defaultdict(list)
    for m in methods:
        ours[m[2]].append(m)
    all3 = sorted({c for _, c in classes})
    w = out.write
    w("# Static-names parity survey\n\n")
    w("Generated by `tools/scripts/static_names.py --survey`. DS types normalized "
      "(long→int, cl/in/ns prefixes dropped) before comparing.\n\n")
    unmatched = []
    stats = defaultdict(int)
    for cls in [c for c in ours if c.startswith(("std", "__cxxabiv1"))]:
        del ours[cls]
    for cls in sorted(ours, key=str.lower):
        keys = class_keys(cls)
        cands = sorted(set().union(*(by_key.get(k, set()) for k in keys)))
        if not cands:
            unmatched.append(cls)
            continue
        three = defaultdict(list)
        for c in cands:
            for tag, name, params, const in members.get(c, []):
                three[name].append((tag, c, params, const))
        rows = sorted(ours[cls], key=lambda r: r[1])
        w(f"## {cls} ↔ {', '.join(cands)}\n\n")
        used = set()
        for mod, addr, _, name, params, const in rows:
            loc = f"{mod} 0x{addr:08x}"
            name = strip_cl(name)
            if PLACEHOLDER_RE.match(name):
                stats["placeholder"] += 1
                arity = [f"{n}{fmt_params(p)}" for n, lst in three.items()
                         for _, _, p, _ in lst[:1] if p is not None and len(p) == len(params)]
                w(f"- `{loc}` {name}{fmt_params(params)} — placeholder; same-arity 3DS: "
                  f"{', '.join(sorted(set(arity))) or '—'}\n")
            elif name in three:
                used.add(name)
                ours_n = [norm_type(p) for p in params]
                sigs = {tuple(norm_type(x) for x in p) for _, _, p, _ in three[name] if p is not None}
                if not sigs or tuple(ours_n) in sigs:
                    stats["same"] += 1
                    w(f"- `{loc}` {name}{fmt_params(params)} — matches\n")
                else:
                    stats["sigdiff"] += 1
                    alt = "; ".join(f"{t} {fmt_params(list(p))}" for t, _, p, _ in three[name]
                                    if p is not None)
                    w(f"- `{loc}` {name}{fmt_params(params)} — **params differ**: {alt}\n")
            else:
                stats["ds_only"] += 1
                close = difflib.get_close_matches(name, list(three), n=3, cutoff=0.6)
                hint = f"; close: {', '.join(close)}" if close else ""
                w(f"- `{loc}` {name}{fmt_params(params)} — not in 3DS{hint}\n")
        rest = sorted(n for n in three if n not in used and not n.startswith("~"))
        if rest:
            w(f"\nUnused 3DS members: {', '.join(f'`{n}`' for n in rest)}\n")
        w("\n")
    w("## DS classes with no 3DS class of the same name\n\n")
    low3 = {strip_cl(c.split('::')[-1]).lower(): c for c in all3}
    for cls in unmatched:
        close = difflib.get_close_matches(strip_cl(cls).lower(), list(low3), n=3, cutoff=0.7)
        hint = f" — close: {', '.join(low3[c] for c in close)}" if close else ""
        w(f"- {cls}{hint}\n")
    w("\n## Free functions with a 3DS name hit\n\n")
    names3 = defaultdict(list)
    for tag, c, n, p, k in funcs:
        if re.search("[A-Z]", n):
            names3[n.lower()].append(f"{tag} {c + '::' if c else ''}{n}{fmt_params(p)}")
    for mod, addr, _, name, params, _ in sorted(frees, key=lambda r: (r[0] != "main", r[0], r[1])):
        hits = names3.get(name.lower())
        if hits:
            stats["free_hit"] += 1
            w(f"- `{mod} 0x{addr:08x}` {name}{fmt_params(params)} — {'; '.join(sorted(set(hits)))}\n")
    w("\n## 3DS free functions (match these by signature)\n\n")
    free3 = defaultdict(set)
    for tag, c, n, p, k in funcs:
        if not c and p is not None and not n.startswith("operator"):
            free3[f"{n}{fmt_params(p)}"].add(tag)
    for sig, tags in sorted(free3.items()):
        w(f"- {'+'.join(sorted(tags))} {sig}\n")
    w(f"\n## Totals\n\n{dict(stats)}; {len(ours) - len(unmatched)}/{len(ours)} DS classes "
      f"have a 3DS namesake\n")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("query", nargs="?")
    ap.add_argument("--survey", action="store_true")
    ap.add_argument("--params")
    ap.add_argument("-o", "--out")
    a = ap.parse_args()
    sys.stdout.reconfigure(encoding="utf-8")
    funcs, classes = load_exports()
    if a.params:
        find_params(a.params, funcs)
    elif a.survey:
        out = open(a.out, "w", encoding="utf-8", newline="\n") if a.out else sys.stdout
        survey(funcs, classes, out)
    elif a.query:
        lookup(a.query, funcs, classes)
    else:
        ap.error("give a class/function query or --survey")


if __name__ == "__main__":
    main()
