#!/usr/bin/env python3
import argparse
import re
import subprocess
import sys
from collections import defaultdict
from pathlib import Path

import yaml

from common import BUILD, CONFIG as CONFIG_DIR, INCLUDE, OBJDUMP, ROOT, SRC, objdiff_target

CONFIG_YAML = CONFIG_DIR / "config.yaml"
ANCHOR_DIRS = [INCLUDE, SRC]

# `tail` keeps trailing dsd flags (e.g. ` local`); the line is rebuilt from the groups.
SYMBOL_LINE_RE = re.compile(
    r"^(?P<name>\S+)(?P<mid>\s+kind:\S+\s+addr:0x)(?P<addr>[0-9a-fA-F]+)"
    r"(?P<tail>.*?)\s*$"
)
# `(?<!\+)0x` everywhere: `// +0xNN` is a struct member offset, never an anchor.
ANCHOR_RE = re.compile(
    r"\b(?:(?P<scope>[A-Za-z_]\w*)\s*::\s*)?"
    r"(?P<name>operator\s+(?:new|delete)(?:\s*\[\s*\])?|[A-Za-z_]\w*)"
    r"\s*\((?P<params>[^;{]*)\)\s*(?:const\s*)?[;{].*//.*?(?<!\+)0x(?P<addr>[0-9a-fA-F]+)"
)
OPERATOR_TOKENS = {"nw": "operator new", "na": "operator new[]",
                   "dl": "operator delete", "da": "operator delete[]"}
CTORDTOR_DECL_RE = re.compile(
    r"\b(?:(?P<scope>[A-Za-z_]\w*)\s*::\s*)?(?P<tilde>~)?(?P<name>[A-Za-z_]\w*)"
    r"\s*\([^;{]*\)\s*(?:const\s*)?[;{](?P<rest>.*)"
)
# Thunk tags: `Thn<N>` (fixed this-adjust), `Tv0_n<K>` / `Tvn<N>_n<K>` (virtual-base, vcall at vptr-K).
THUNK_TAG = r"(Thn\d+|Tv(?:0|n\d+)_n\d+)"
CTOR_DTOR_CODE_RE = re.compile(rf"\b(?:{THUNK_TAG}_)?(C[12]|D[012])\b\s+0x([0-9a-fA-F]+)")
THUNK_CODE_RE = re.compile(rf"\b{THUNK_TAG}\s+0x([0-9a-fA-F]+)")
THUNK_SYM_RE = re.compile(rf"^_Z{THUNK_TAG}_(?P<rest>.*)$")
CLASS_RE = re.compile(r"\b(?:class|struct)\s+([A-Za-z_]\w*)[^;{()]*\{")
FUNC_ADDR_RE = re.compile(r"^func_(?:ov[0-9A-Fa-f]+_)?([0-9a-fA-F]{8})$")
DATA_ADDR_RE = re.compile(r"^data_(?:ov[0-9A-Fa-f]+_)?([0-9a-fA-F]{8})$")
DATA_ANCHOR_RE = re.compile(
    r"\b(?P<name>[A-Za-z_]\w*)\s*(?:\[[^\]]*\])*\s*(?:=\s*\{|(?:=[^;]*)?;)"
    r".*//.*(?<!\+)0x(?P<addr>[0-9a-fA-F]+)"
)
# Multi-line initializer: `T name[N] = {` ... `};  // 0xADDR` -- anchor accepted on the closing line.
DATA_OPEN_RE = re.compile(r"\b(?P<name>[A-Za-z_]\w*)\s*(?:\[[^\]]*\])*\s*=\s*\{")
TRAIL_ANCHOR_RE = re.compile(r"//.*(?<!\+)0x(?P<addr>[0-9a-fA-F]+)")
OBJ_DATA_SYM_RE = re.compile(r"^\S+\s+g\s+O\s+\.(?:data|bss|sbss|rodata)\b.*\s(\S+)$")
ANCHOR_MOD_RE = re.compile(r"//.*?\b(?P<mod>ov[0-9]{3})\s+(?<!\+)0x(?P<addr>[0-9a-fA-F]+)")
TARGET_MOD_RE = re.compile(r"_dsd_gap@(?P<mod>.+)_\d+\.o")
CTOR_CODE_RE = re.compile(r"[CD][0-9]$")


def parse_name(sym):
    if not sym.startswith("_Z"):
        return "", sym
    if tm := THUNK_SYM_RE.match(sym):
        scope, simple = parse_name("_Z" + tm.group("rest"))
        return scope, f"{simple}/{tm.group(1)}"
    s = sym[2:]
    nested = s.startswith("N")
    if nested:
        s = s[1:]
    parts = []
    while s and s[0] != "E":
        m = re.match(r"(\d+)", s)
        if m:
            n = int(m.group(1))
            parts.append(s[m.end():m.end() + n])
            s = s[m.end() + n:]
        else:
            parts.append(s[:2])
            s = s[2:]
        if not nested:
            break
    scope = parts[-2] if len(parts) >= 2 else ""
    simple = parts[-1] if parts else None
    op = OPERATOR_TOKENS.get(simple)
    if op:
        n, names = mangled_params(s[1:] if s[:1] == "E" else s)
        return scope, f"{op}/{n}" + ("/" + ",".join(sorted(names)) if names else "")
    return scope, simple


def norm_name(n):
    return re.sub(r"\s*\[\s*\]", "[]", re.sub(r"\s+", " ", n))


def source_arity(params):
    s = params.strip()
    if not s or s == "void":
        return 0
    depth, n = 0, 1
    for c in s:
        depth += c in "(<["
        depth -= c in ")>]"
        n += c == "," and depth == 0
    return n


def mangled_params(s):
    i, n, names = 0, 0, set()
    while i < len(s):
        while i < len(s) and s[i] in "PRKOCVG":
            i += 1
        if i >= len(s):
            break
        if s[i] == "S":
            i = s.find("_", i) + 1 or len(s)
        elif s[i].isdigit():
            m = re.match(r"\d+", s[i:])
            ln, i = int(m.group()), i + m.end()
            names.add(s[i:i + ln])
            i += ln
        else:
            i += 1
        n += 1
    return n, names


def qual(scope, name):
    return f"{scope}::{name}" if scope else name


def embedded_addr(simple):
    m = FUNC_ADDR_RE.match(simple) or DATA_ADDR_RE.match(simple)
    return int(m.group(1), 16) if m else None


def is_userish(name):
    if not name:
        return False
    if name.split("/")[0] in OPERATOR_TOKENS.values():
        return True
    return not CTOR_CODE_RE.match(name.split("/")[0]) and "operator" not in name


def load_anchors(verbose):
    raw = defaultdict(list)
    idents = {}
    mod_by_addr = {}
    for d in ANCHOR_DIRS:
        if not d.exists():
            continue
        for f in sorted(d.rglob("*.h")) + sorted(d.rglob("*.cpp")) + sorted(d.rglob("*.c")):
            depth, stack, pending = 0, [], None
            frel = f.relative_to(ROOT)
            for lineno, line in enumerate(
                f.read_text(encoding="utf-8", errors="replace").splitlines(), 1
            ):
                loc = f"{frel}:{lineno}"
                cls = stack[-1][0] if stack else ""
                for am in ANCHOR_MOD_RE.finditer(line):
                    mod_by_addr[int(am.group("addr"), 16)] = am.group("mod")
                cd = CTORDTOR_DECL_RE.search(line)
                eff = cd and (cd.group("scope") or cls)
                if eff and cd.group("name") == eff:
                    for tag, code, addr in CTOR_DTOR_CODE_RE.findall(cd.group("rest")):
                        key = f"{code}/{tag}" if tag else code
                        raw[(eff, key)].append((int(addr, 16), loc))
                elif m := ANCHOR_RE.search(line):
                    scope = m.group("scope") or cls
                    nm = norm_name(m.group("name"))
                    if nm.startswith("operator "):
                        nm += f"/{source_arity(m.group('params'))}"
                        idents.setdefault((scope, nm), set()).update(
                            re.findall(r"[A-Za-z_]\w*", m.group("params")))
                    raw[(scope, nm)].append((int(m.group("addr"), 16), loc))
                    for tag, addr in THUNK_CODE_RE.findall(line.split("//", 1)[1]):
                        raw[(scope, f"{nm}/{tag}")].append((int(addr, 16), loc))
                elif not stack and (dm := DATA_ANCHOR_RE.search(line)):
                    raw[("", dm.group("name"))].append((int(dm.group("addr"), 16), loc))
                elif (not stack and pending is None and (om := DATA_OPEN_RE.search(line))
                      and line.count("{") > line.count("}")):
                    pending = (om.group("name"), loc, depth)
                if cm := CLASS_RE.search(line):
                    stack.append((cm.group(1), depth))
                depth += line.count("{") - line.count("}")
                if pending and depth <= pending[2]:
                    if tm := TRAIL_ANCHOR_RE.search(line):
                        raw[("", pending[0])].append((int(tm.group("addr"), 16), pending[1]))
                    pending = None
                while stack and depth <= stack[-1][1]:
                    stack.pop()
    anchors, conflicts = {}, 0
    for key, occ in raw.items():
        addrs = {a for a, _ in occ}
        if len(addrs) > 1:
            conflicts += 1
            locs = ", ".join(f"{loc} (0x{a:08x})" for a, loc in occ)
            print(f"  CONFLICT: '{qual(*key)}' anchored at differing addresses "
                  f"-> skipped: {locs}", file=sys.stderr)
            continue
        if len(occ) > 1:
            locs = ", ".join(loc for _, loc in occ)
            print(f"  duplicate anchor: '{qual(*key)}' declared in {len(occ)} "
                  f"places (same address) -- keep in sync; prefer one: {locs}",
                  file=sys.stderr)
        anchors[key] = addrs.pop()
    if verbose:
        print(f"  {len(anchors)} address anchor(s) from source, "
              f"{len(mod_by_addr)} module-qualified", file=sys.stderr)
    return anchors, conflicts, idents, mod_by_addr


def resolve_anchor(anchors, by_name, scope, simple, idents):
    if simple.startswith("operator "):
        part = simple.split("/")
        key = (scope, "/".join(part[:2]))
        want = set(part[2].split(",")) if len(part) > 2 else set()
        a = anchors.get(key)
        return a if a is not None and want <= idents.get(key, set()) else None
    a = anchors.get((scope, simple))
    if a is not None:
        return a
    # Name-only fallback for free functions only: method names and C1/D0 codes recur across classes.
    if scope or CTOR_CODE_RE.match(simple):
        return None
    addrs = by_name.get(simple)
    if addrs and len(addrs) == 1:
        return next(iter(addrs))
    return None


def load_config_modules():
    d = yaml.safe_load(CONFIG_YAML.read_text(encoding="utf-8"))
    mods = {d["main_module"]["name"]: CONFIG_DIR / d["main_module"]["symbols"]}
    resident = [d["main_module"]["name"]]
    for a in d.get("autoloads", []):
        mods[a["name"]] = CONFIG_DIR / a["symbols"]
        resident.append(a["name"])
    for o in d.get("overlays", []):
        mods[o["name"]] = CONFIG_DIR / o["symbols"]
    return mods, resident


def sources_by_module():
    by_mod = defaultdict(list)
    for ext in ("*.c", "*.cpp"):
        for src in sorted(SRC.rglob(ext)):
            target = objdiff_target(src)
            if target is None:
                continue
            mm = TARGET_MOD_RE.search(target)
            if not mm:
                print(f"  WARN: {src.relative_to(ROOT)} target '{target}' "
                      f"unrecognized -- skipped", file=sys.stderr)
                continue
            by_mod[mm.group("mod")].append(BUILD / src.relative_to(ROOT).with_suffix(".o"))
    return by_mod


def dump_symbols(o_paths, verbose):
    defined, undef = {}, {}
    present = []
    for o in o_paths:
        if o.exists():
            present.append(o)
        elif verbose:
            print(f"  (not built yet, skipped: {o.relative_to(ROOT)})",
                  file=sys.stderr)
    if present:
        try:
            txt = subprocess.run([OBJDUMP, "-t", *map(str, present)],
                                 capture_output=True, text=True,
                                 check=True).stdout
        except (subprocess.CalledProcessError, FileNotFoundError) as e:
            sys.exit(f"objdump failed: {e}")
        for line in txt.splitlines():
            dm = OBJ_DATA_SYM_RE.match(line)
            if dm and re.match(r"_ZT[VIST]", line.split()[-1]):
                continue
            if re.search(r"\bF\s+\.text\b", line) or dm:
                dst = defined
            elif "*UND*" in line:
                dst = undef
            else:
                continue
            dst.setdefault(line.split()[-1], None)
    out = [parse_name(n) + (n, True) for n in defined]
    out += [parse_name(n) + (n, False) for n in undef
            if n not in defined and not FUNC_ADDR_RE.match(n)
            and not DATA_ADDR_RE.match(n)]
    return out


class SymTable:
    def __init__(self, path):
        self.path = path
        self.lines = path.read_text(encoding="utf-8").splitlines(keepends=True)
        self.by_addr, self.present, self.addr_of = {}, set(), {}
        for i, line in enumerate(self.lines):
            if m := SYMBOL_LINE_RE.match(line):
                addr = int(m.group("addr"), 16)
                self.by_addr[addr] = i
                self.present.add(m.group("name"))
                self.addr_of[m.group("name")] = addr
        self.dirty = False

    def rename(self, addr, full):
        i = self.by_addr[addr]
        sm = SYMBOL_LINE_RE.match(self.lines[i])
        old = sm.group("name")
        if old == full:
            return None
        eol = "\n" if self.lines[i].endswith("\n") else ""
        self.lines[i] = (f"{full}{sm.group('mid')}{sm.group('addr')}"
                         f"{sm.group('tail')}{eol}")
        self.present.discard(old)
        self.present.add(full)
        self.dirty = True
        return old, full

    def flush(self):
        if self.dirty:
            self.path.write_text("".join(self.lines), encoding="utf-8")


def load_tables(modules, mods_needed, tables):
    for m in mods_needed:
        if m in tables or m not in modules:
            continue
        if not modules[m].exists():
            print(f"  symbols file missing: {modules[m]}", file=sys.stderr)
            continue
        tables[m] = SymTable(modules[m])
    return tables


def resolve_module(addr, mod, tables, resident, mod_by_addr):
    named = mod_by_addr.get(addr)
    if named is not None:
        return named if named in tables and addr in tables[named].by_addr else None
    for m in [mod] + [r for r in resident if r != mod]:
        if m in tables and addr in tables[m].by_addr:
            return m
    return None


def process_module(mod, o_paths, tables, resident, anchors, idents, mod_by_addr,
                   verbose):
    syms = dump_symbols(o_paths, verbose)
    if not syms or mod not in tables:
        return 0, [], set(), 0

    by_name = defaultdict(set)
    for (_, name), a in anchors.items():
        by_name[name].add(a)

    resolved = []
    for scope, simple, full, is_def in syms:
        addr = anchors.get((scope, simple))
        if addr is None:
            addr = embedded_addr(simple)
        if addr is None:
            addr = resolve_anchor(anchors, by_name, scope, simple, idents)
        tgt = None if addr is None else resolve_module(addr, mod, tables, resident, mod_by_addr)
        resolved.append((addr, tgt, simple, full, is_def))

    names_at = defaultdict(set)
    for addr, tgt, _, full, _ in resolved:
        if tgt is not None:
            names_at[(tgt, addr)].add(full)
    ambiguous = {k for k, ns in names_at.items() if len(ns) > 1}
    for tgt, a in sorted(ambiguous):
        print(f"  [{mod}] ambiguous: 0x{a:08x} ({tgt}) defined under "
              f"{len(names_at[(tgt, a)])} names -> skipped: "
              f"{', '.join(sorted(names_at[(tgt, a)]))}", file=sys.stderr)

    def known(full):
        return any(full in tables[m].present for m in [mod] + resident if m in tables)

    changed, unplaced, unanchored = [], [], []
    for addr, tgt, simple, full, is_def in resolved:
        if tgt is None:
            # An unanchored extern (SDK link symbol) is expected; an anchored address nobody owns is not.
            if (is_userish(simple) or "/T" in simple) and not known(full) and (is_def or addr is not None):
                unplaced.append((full, addr, simple))
            elif is_def and addr is None and is_userish(simple) and known(full):
                unanchored.append(full)
            continue
        if (tgt, addr) in ambiguous:
            continue
        if r := tables[tgt].rename(addr, full):
            changed.append((tgt, addr, *r))

    for tgt, addr, old, new in changed:
        where = "" if tgt == mod else f" -> {tgt}"
        print(f"  [{mod}]{where} 0x{addr:08x}  {old}  ->  {new}")
    for full, addr, simple in unplaced:
        thn = re.search(rf"/{THUNK_TAG}$", simple)
        why = ((f"this-adjust thunk: add `{thn.group(1)} 0xADDR` (or "
                f"`{thn.group(1)}_D0`/`_D1` for dtors) to the base declaration's "
                "anchor comment") if thn and addr is None else
               "add a // 0xADDR anchor to its declaration" if addr is None else
               f"anchored 0x{addr:08x}, which no resident module carries a "
               f"symbol at -- wrong address, or defined in an overlay")
        print(f"  [{mod}] UNPLACED: {full} -- {why}", file=sys.stderr)
    for full in unanchored:
        a = next(tables[m].addr_of[full] for m in [mod] + resident
                 if m in tables and full in tables[m].addr_of)
        print(f"  [{mod}] NOANCHOR: {full} -- add // 0x{a:08x} to its declaration",
              file=sys.stderr)

    return len(changed), unplaced, ambiguous, len(unanchored)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--strict", action="store_true",
                    help="non-zero exit if any conflict or unplaced name remains")
    ap.add_argument("-v", "--verbose", action="store_true")
    args = ap.parse_args()

    anchors, conflicts, idents, mod_by_addr = load_anchors(args.verbose)
    modules, resident = load_config_modules()
    by_mod = sources_by_module()
    tables = load_tables(modules, resident + sorted(set(mod_by_addr.values())), {})

    total_changed = total_unplaced = total_ambiguous = total_unanchored = 0
    for mod in sorted(by_mod):
        if mod not in modules:
            print(f"  WARN: module '{mod}' has source but is not in config.yaml "
                  f"-- skipped", file=sys.stderr)
            continue
        load_tables(modules, [mod], tables)
        n, unplaced, ambiguous, unanchored = process_module(
            mod, by_mod[mod], tables, resident, anchors, idents,
            mod_by_addr, args.verbose)
        total_changed += n
        total_unplaced += len(unplaced)
        total_ambiguous += len(ambiguous)
        total_unanchored += unanchored
        if args.verbose:
            print(f"  module {mod}: {len(by_mod[mod])} unit(s), "
                  f"{'would rename' if args.dry_run else 'renamed'} {n}",
                  file=sys.stderr)

    if not args.dry_run:
        for t in tables.values():
            t.flush()

    verb = "would rename" if args.dry_run else "renamed"
    print(f"sync_renames: {verb} {total_changed}, {total_unplaced} unplaced, "
          f"{total_ambiguous} ambiguous, {conflicts} conflict(s), "
          f"{total_unanchored} unanchored across {len(by_mod)} module(s)")
    return 1 if args.strict and (total_unplaced or conflicts) else 0


if __name__ == "__main__":
    sys.exit(main())
