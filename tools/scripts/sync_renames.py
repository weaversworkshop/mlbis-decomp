#!/usr/bin/env python3
"""Sync mwcc's mangled symbol names from compiled game .o files into each
MODULE's symbols.txt (main + autoloads + overlays) so objdiff pairs C++
methods with the delinked target.

Ground truth for a name is the compiled .o (mwcc's actual mangling). We only
need the TARGET ADDRESS each .o symbol maps to. Two ways to recover it:
  1. The simple name embeds it: func_0201b0e4  or  func_ov001_02069cb8.
  2. An address anchor on the function's DECLARATION:
         void testFuncXXYYZZ();   // 0x0201b0e4
     scanned from arm9/include/** and arm9/src/**, so exported (header/class)
     AND file-local (cpp) functions rename identically.
  Ctors/dtors emit several mangled symbols from one decl, so their anchor tags
  each ABI variant: `clFS(...); // C2 0x..` and `~clFS(); // D0 0x.. D1 0x.. D2 0x..`.

MODULE-AWARE: each source file's `// objdiff-target: ..._dsd_gap@<mod>_N.o`
comment assigns it (and its build .o) to a module (main / itcm / dtcm / ovNNN);
that module's own symbols.txt is the one edited, resolved via config.yaml.
Placement is keyed by address WITHIN a module, so overlays that share absolute
addresses never collide (the trap a single global table would hit). An address
its own module doesn't carry falls back to the RESIDENT modules (main + itcm +
dtcm) -- their ranges are disjoint, so at most one matches. That is what lets a
main TU name an itcm global it can only ever declare `extern`. Overlays are
never guessed; an anchor that targets one must name it:  // ov122 0x0206d9b4
(a main-homed vtable whose overrides are defined in an overlay).

Join is by (CLASS SCOPE, SIMPLE NAME): the last two identifiers of the mangled
nested-name vs the anchor's enclosing class {...} block (or explicit X::
qualifier). Overridden virtuals thus anchor independently per class
(clFS::pumpReads vs clDPP::pumpReads). A rename changes the source name and
the .o name together; the anchor address rides along, so param/struct edits
AND renames stay in sync with no hand edits to symbols.txt.

Anti-forget: a compiled method with a semantic name and NO recoverable address
(no func_<addr>, no anchor) is REPORTED, never silently dropped.

Usage: sync_renames.py [--dry-run] [--strict] [-v].  A `make diff`/`make
progress` step.
"""
import argparse
import re
import subprocess
import sys
from collections import defaultdict
from pathlib import Path

import yaml

ROOT = Path(__file__).resolve().parents[2]
CONFIG_DIR = ROOT / "config" / "arm9"
CONFIG_YAML = CONFIG_DIR / "config.yaml"
SRC_ROOT = ROOT / "arm9" / "src"
BUILD = ROOT / "build"
ANCHOR_DIRS = [ROOT / "arm9" / "include", ROOT / "arm9" / "src"]
OBJDUMP = "arm-none-eabi-objdump"

# `tail` keeps trailing dsd flags (e.g. ` local`): the line is rebuilt from these
# groups, so an uncaptured suffix would be dropped on rename -- and an anchored
# pattern would skip the symbol entirely, reporting it as UNPLACED.
SYMBOL_LINE_RE = re.compile(
    r"^(?P<name>\S+)(?P<mid>\s+kind:\S+\s+addr:0x)(?P<addr>[0-9a-fA-F]+)"
    r"(?P<tail>.*?)\s*$"
)
# A declaration/definition line ending in ';' or '{' with a trailing address
# anchor. Captures the identifier immediately before the parameter list and
# an optional explicit X:: qualifier.
ANCHOR_RE = re.compile(
    r"\b(?:(?P<scope>[A-Za-z_]\w*)\s*::\s*)?"
    r"(?P<name>operator\s+(?:new|delete)(?:\s*\[\s*\])?|[A-Za-z_]\w*)"
    r"\s*\((?P<params>[^;{]*)\)\s*(?:const\s*)?[;{].*//.*(?<!\+)0x(?P<addr>[0-9a-fA-F]+)"
)
# Mangled ABI token -> source spelling, for the allocation operators only.
# Implicit ones (operator=) are compiler-emitted and never anchored.
# Keyed WITH arity ('operator new/2'): global placement forms are overloaded
# (heap.h has a 4-param one), so name alone collides the way C1/D0 does.
OPERATOR_TOKENS = {"nw": "operator new", "na": "operator new[]",
                   "dl": "operator delete", "da": "operator delete[]"}
# Ctor/dtor declaration: the function name equals the enclosing (or X::-qualified)
# class, with an optional '~' for the dtor. One such source decl maps to SEVERAL
# mangled symbols at distinct addresses (ctor C1/C2, dtor D0/D1/D2), so a single
# // 0xADDR anchor can't express it. Instead the comment tags each ABI variant:
#     clFS(...);   // C2 0x0201a9e0
#     ~clFS();     // D0 0x0201ab94 D1 0x0201ab4c D2 0x0201abe4
# Each tag becomes an anchor keyed (class, code) that pairs with the .o symbol
# whose parse_name() yields that code, e.g. ('clFS','C2'). An old untagged
# // 0xADDR comment on a ctor/dtor now registers nothing (harmless) instead of
# colliding ctor-vs-dtor under one (class, class) key.
CTORDTOR_DECL_RE = re.compile(
    r"\b(?:(?P<scope>[A-Za-z_]\w*)\s*::\s*)?(?P<tilde>~)?(?P<name>[A-Za-z_]\w*)"
    r"\s*\([^;{]*\)\s*(?:const\s*)?[;{](?P<rest>.*)"
)
CTOR_DTOR_CODE_RE = re.compile(r"\b(C[12]|D[012])\b\s+0x([0-9a-fA-F]+)")
# A named class/struct whose body opens on this line.
CLASS_RE = re.compile(r"\b(?:class|struct)\s+([A-Za-z_]\w*)[^;{()]*\{")
# Simple name that embeds its address: func_0201a610 (main/itcm/dtcm) or
# func_ov001_02069cb8 (overlay-prefixed, as the delinked symbols.txt names them).
FUNC_ADDR_RE = re.compile(r"^func_(?:ov[0-9A-Fa-f]+_)?([0-9a-fA-F]{8})$")
DATA_ADDR_RE = re.compile(r"^data_(?:ov[0-9A-Fa-f]+_)?([0-9a-fA-F]{8})$")
# A global variable definition/declaration with a trailing address anchor:
#     TPData samplingBufs[2];              // 0x0205bac4
#     extern UnkGlobal_0205afa8 inputState;  // 0x0205afa8
# The identifier immediately before optional [dims] / = init / ';' or a brace
# initializer's '{' (multi-line tables put the ';' out of reach). Only
# consulted at global scope (never inside a class/struct body) and only when
# the function ANCHOR_RE didn't match the line.
DATA_ANCHOR_RE = re.compile(
    r"\b(?P<name>[A-Za-z_]\w*)\s*(?:\[[^\]]*\])*\s*(?:=\s*\{|(?:=[^;]*)?;)"
    r".*//.*(?<!\+)0x(?P<addr>[0-9a-fA-F]+)"
)
# Defined global (non-weak) object symbol in a data-ish section of the .o.
OBJ_DATA_SYM_RE = re.compile(r"^\S+\s+g\s+O\s+\.(?:data|bss|sbss|rodata)\b.*\s(\S+)$")
# An anchor may name the module that owns the address:  // ov122 0x0206d9b4
# Needed when a class's vtable is homed in main but an override is DEFINED in an
# overlay: resolve_module() will never guess an overlay (141 of them share
# absolute addresses), so the human resolves it and the tool obeys. Keyed by
# address, which is unique within the named module.
ANCHOR_MOD_RE = re.compile(r"//.*?\b(?P<mod>ov[0-9]{3})\s+(?<!\+)0x(?P<addr>[0-9a-fA-F]+)")
# The // objdiff-target: value -> owning module (main_7 -> main, ov001_1 -> ov001).
OBJTARGET_RE = re.compile(r"^\s*//\s*objdiff-target:\s*(.+?)\s*$")
TARGET_MOD_RE = re.compile(r"_dsd_gap@(?P<mod>.+)_\d+\.o")


def parse_name(sym):
    """(scope, simple): last two source identifiers of a mangled symbol
    ('clFS', 'pumpReads'), or ('', sym) if not mangled/nested. Ctor/dtor codes
    (C1/C2/D0..) return e.g. 'C2' -> won't match a source anchor, so they're
    left to one-time manual mapping."""
    if not sym.startswith("_Z"):
        return "", sym
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
            parts.append(s[:2])  # ctor/dtor token (C1/C2/D0/D1/D2) or the like
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
    """'operator  new [ ]' -> 'operator new[]'; other names unchanged."""
    return re.sub(r"\s*\[\s*\]", "[]", re.sub(r"\s+", " ", n))


def source_arity(params):
    """Parameter count of a source parameter list ('' and 'void' -> 0)."""
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
    """(count, class names) of a mangled parameter-encoding suffix (Itanium
    subset: builtins, P/R/K prefixes, N-length names, S_ substitutions).
    mwcc encodes a class param as its literal source name, so the names
    discriminate same-arity overloads without ever hand-mangling."""
    i, n, names = 0, 0, set()
    while i < len(s):
        while i < len(s) and s[i] in "PRKOCVG":  # prefixes: don't end a param
            i += 1
        if i >= len(s):
            break
        if s[i] == "S":                          # substitution: S_, S0_, ...
            i = s.find("_", i) + 1 or len(s)
        elif s[i].isdigit():                     # N-length source name
            m = re.match(r"\d+", s[i:])
            ln, i = int(m.group()), i + m.end()
            names.add(s[i:i + ln])
            i += ln
        else:
            i += 1                               # single-char builtin
        n += 1
    return n, names


def qual(scope, name):
    return f"{scope}::{name}" if scope else name


def embedded_addr(simple):
    """Address embedded in a func_<addr>/data_<addr> style name, else None."""
    m = FUNC_ADDR_RE.match(simple) or DATA_ADDR_RE.match(simple)
    return int(m.group(1), 16) if m else None


def is_userish(name):
    """A name that SHOULD have been placeable (worth reporting if it isn't)."""
    if not name:
        return False
    if name.split("/")[0] in OPERATOR_TOKENS.values():  # anchorable -> report
        return True
    return not re.match(r"[CD][0-9]$", name) and "operator" not in name


def load_anchors(verbose):
    """(scope, simple-name) -> address (int), from // 0xADDR anchors in .h/.cpp
    decls across arm9/include/** and arm9/src/**. Scope = enclosing class {...}
    block (brace-tracked) or an explicit X:: qualifier; '' for free functions.
    Returns (anchors, conflicts): conflicts = count of scoped names whose
    anchors disagree on the address (skipped). Redundant same-address anchors
    across files are a notice only."""
    raw = {}  # (scope, name) -> list of (addr, "path:line")
    idents = {}  # operator key -> identifier tokens in its source params
    mod_by_addr = {}  # addr -> module named by an `// ovNNN 0xADDR` anchor
    for d in ANCHOR_DIRS:
        if not d.exists():
            continue
        for f in sorted(d.rglob("*.h")) + sorted(d.rglob("*.cpp")) + sorted(d.rglob("*.c")):
            depth, stack = 0, []  # stack: (class name, depth at its open)
            for lineno, line in enumerate(
                f.read_text(encoding="utf-8", errors="replace").splitlines(), 1
            ):
                loc = f"{f.relative_to(ROOT)}:{lineno}"
                for am in ANCHOR_MOD_RE.finditer(line):
                    mod_by_addr[int(am.group("addr"), 16)] = am.group("mod")
                cd = CTORDTOR_DECL_RE.search(line)
                handled = False
                if cd:
                    eff = cd.group("scope") or (stack[-1][0] if stack else "")
                    # name == class ==> ctor (no '~') or dtor ('~'); register one
                    # (class, code) anchor per C*/D* tag in the comment.
                    if eff and cd.group("name") == eff:
                        handled = True
                        for code, addr in CTOR_DTOR_CODE_RE.findall(cd.group("rest")):
                            raw.setdefault((eff, code), []).append((int(addr, 16), loc))
                if not handled:
                    m = ANCHOR_RE.search(line)
                    if m:
                        scope = m.group("scope") or (stack[-1][0] if stack else "")
                        nm = norm_name(m.group("name"))
                        if nm.startswith("operator "):
                            nm += f"/{source_arity(m.group('params'))}"
                            idents.setdefault((scope, nm), set()).update(
                                re.findall(r"[A-Za-z_]\w*", m.group("params")))
                        raw.setdefault((scope, nm), []).append(
                            (int(m.group("addr"), 16), loc))
                    elif not stack:
                        # global-scope data anchor (class members must not register)
                        dm = DATA_ANCHOR_RE.search(line)
                        if dm:
                            raw.setdefault(("", dm.group("name")), []).append(
                                (int(dm.group("addr"), 16), loc))
                if CLASS_RE.search(line):
                    stack.append((CLASS_RE.search(line).group(1), depth))
                depth += line.count("{") - line.count("}")
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
    """Exact (scope, name) match, else name-only fallback iff every anchor of
    that name agrees on one address (covers scope-parse misses; never guesses
    between per-class overrides). Ctor/dtor codes (C1/C2/D0..) are generic ABI
    tokens, NOT unique names -- resolve them ONLY by exact (class, code), never
    the fallback (else the lone anchored C1 captures every class's C1)."""
    if simple.startswith("operator "):
        # 'operator new/2/clYanaUnitSys': match on name+arity, then require every
        # class-typed param to appear in the anchor's source param list. Never
        # falls back by name -- 'operator new' is generic like C1/D0.
        part = simple.split("/")
        key = (scope, "/".join(part[:2]))
        want = set(part[2].split(",")) if len(part) > 2 else set()
        a = anchors.get(key)
        return a if a is not None and want <= idents.get(key, set()) else None
    a = anchors.get((scope, simple))
    if a is not None:
        return a
    # Name-only fallback is ONLY for unscoped (free) functions. A scoped method
    # name (fn_10, m08, run, ...) or a ctor/dtor code (C1/D0) is shared across
    # unrelated classes, so a lone anchor of it must NOT leak to the others via
    # by_name -- require an exact (class, name) match for those.
    if scope or re.match(r"[CD][0-9]$", simple):
        return None
    addrs = by_name.get(simple)
    if addrs and len(addrs) == 1:
        return next(iter(addrs))
    return None


def load_config_modules():
    """(module name -> symbols.txt path, resident module names).

    Resident = main + autoloads (itcm/dtcm). They are all mapped at once at
    disjoint absolute addresses, so an address identifies at most one of them
    -- which is what makes them safe cross-module fallback targets. The 141
    overlays share absolute addresses, so they never are."""
    d = yaml.safe_load(CONFIG_YAML.read_text(encoding="utf-8"))
    mods = {d["main_module"]["name"]: CONFIG_DIR / d["main_module"]["symbols"]}
    resident = [d["main_module"]["name"]]
    for a in d.get("autoloads", []):
        mods[a["name"]] = CONFIG_DIR / a["symbols"]
        resident.append(a["name"])
    for o in d.get("overlays", []):
        mods[o["name"]] = CONFIG_DIR / o["symbols"]
    return mods, resident


def sources_by_module(verbose):
    """module name -> list of build .o paths, from each source file's
    // objdiff-target comment. Files without the comment aren't objdiff units,
    so their names never need syncing -- skipped like gen_objdiff does."""
    by_mod = defaultdict(list)
    for ext in ("*.c", "*.cpp"):
        for src in sorted(SRC_ROOT.rglob(ext)):
            target = None
            for line in src.read_text(encoding="utf-8", errors="replace").splitlines():
                m = OBJTARGET_RE.match(line)
                if m:
                    target = m.group(1)
                    break
            if target is None:
                continue
            mm = TARGET_MOD_RE.search(target)
            if not mm:
                print(f"  WARN: {src.relative_to(ROOT)} target '{target}' "
                      f"unrecognized -- skipped", file=sys.stderr)
                continue
            by_mod[mm.group("mod")].append(
                BUILD / src.relative_to(ROOT).with_suffix(".o")
            )
    return by_mod


def dump_symbols(o_paths, verbose):
    """(scope, simple_name, full_mangled, defined) for every defined .text
    function symbol AND every undefined reference, across one module's .o
    files (deduped within the module, defined wins). Undefined mangled refs
    (declared method with callers, body not yet decompiled) carry the same
    ground-truth mangling the call relocs use, so they sync too."""
    defined, undef = {}, {}
    for o in o_paths:
        if not o.exists():
            if verbose:
                print(f"  (not built yet, skipped: {o.relative_to(ROOT)})",
                      file=sys.stderr)
            continue
        try:
            txt = subprocess.run([OBJDUMP, "-t", str(o)], capture_output=True,
                                 text=True, check=True).stdout
        except (subprocess.CalledProcessError, FileNotFoundError) as e:
            sys.exit(f"objdump failed on {o}: {e}")
        for line in txt.splitlines():
            dm = OBJ_DATA_SYM_RE.match(line)
            if dm and re.match(r"_ZT[VIS]", line.split()[-1]):
                continue  # vtables/RTTI: one-time manual mapping (map-cpp-tu)
            if re.search(r"\bF\s+\.text\b", line) or dm:
                dst = defined
            elif "*UND*" in line:
                dst = undef
            else:
                continue
            name = line.split()[-1]
            dst.setdefault(name, None)
    out = [parse_name(n) + (n, True) for n in defined]
    # plain func_<addr> UND refs are stale externs, no information -- never
    # let them downgrade a real name.
    out += [parse_name(n) + (n, False) for n in undef
            if n not in defined and not FUNC_ADDR_RE.match(n)
            and not DATA_ADDR_RE.match(n)]
    return out


def load_symbols_lines(path):
    lines = path.read_text(encoding="utf-8").splitlines(keepends=True)
    by_addr, present = {}, set()
    for i, line in enumerate(lines):
        m = SYMBOL_LINE_RE.match(line)
        if m:
            by_addr[int(m.group("addr"), 16)] = i
            present.add(m.group("name"))
    return lines, by_addr, present


class SymTable:
    """One module's symbols.txt, loaded once and written once. Shared across
    modules so a main TU anchoring an itcm address and an itcm TU anchoring
    the same file both edit one in-memory copy instead of clobbering."""

    def __init__(self, path):
        self.path = path
        self.lines, self.by_addr, self.present = load_symbols_lines(path)
        self.dirty = False

    def rename(self, addr, full):
        """-> (old, new) if the line changed, else None."""
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
    """Populate `tables` for the named modules (missing files reported once)."""
    for m in mods_needed:
        if m in tables or m not in modules:
            continue
        if not modules[m].exists():
            print(f"  symbols file missing: {modules[m]}", file=sys.stderr)
            continue
        tables[m] = SymTable(modules[m])
    return tables


def resolve_module(addr, mod, tables, resident, mod_by_addr=None):
    """Module whose symbols.txt owns `addr`: an explicit `// ovNNN 0xADDR`
    anchor wins, then the TU's own module, then the resident modules. Resident
    address ranges are disjoint, so at most one can match and the choice is
    never a guess. Returns None if no module carries a symbol at that address
    -- including when an explicit module was named but has nothing there, which
    is a wrong anchor and must not silently fall through to a resident."""
    named = (mod_by_addr or {}).get(addr)
    if named is not None:
        return named if named in tables and addr in tables[named].by_addr else None
    if mod in tables and addr in tables[mod].by_addr:
        return mod
    for m in resident:
        if m != mod and m in tables and addr in tables[m].by_addr:
            return m
    return None


def process_module(mod, o_paths, tables, resident, anchors, idents, mod_by_addr,
                   verbose):
    """Sync one module's .o symbols into the symbols.txt that owns each address
    -- its own module, or a resident one (see resolve_module). A main TU
    referencing an itcm global is the normal case: the itcm object can never be
    DEFINED from a main unit, so an anchored extern is its only route to a name."""
    syms = dump_symbols(o_paths, verbose)
    if not syms or mod not in tables:
        return 0, [], set()

    by_name = defaultdict(set)
    for (_, name), a in anchors.items():
        by_name[name].add(a)

    # Recover each symbol's address once, then the module that owns it.
    resolved = []
    for scope, simple, full, is_def in syms:
        addr = embedded_addr(simple)
        if addr is None:
            addr = resolve_anchor(anchors, by_name, scope, simple, idents)
        tgt = (None if addr is None else
               resolve_module(addr, mod, tables, resident, mod_by_addr))
        resolved.append((addr, tgt, scope, simple, full, is_def))

    # multiple distinct .o symbols resolving to one (module, address) =
    # ambiguous; refuse to guess, report it.
    names_at = {}
    for addr, tgt, _, _, full, _ in resolved:
        if tgt is not None:
            names_at.setdefault((tgt, addr), set()).add(full)
    ambiguous = {k for k, ns in names_at.items() if len(ns) > 1}
    for tgt, a in sorted(ambiguous):
        print(f"  [{mod}] ambiguous: 0x{a:08x} ({tgt}) defined under "
              f"{len(names_at[(tgt, a)])} names -> skipped: "
              f"{', '.join(sorted(names_at[(tgt, a)]))}", file=sys.stderr)

    def known(full):
        """Name already correct in some searched module -> nothing to report."""
        return any(full in tables[m].present
                   for m in [mod] + resident if m in tables)

    changed, unplaced = [], []
    for addr, tgt, scope, simple, full, is_def in resolved:
        if tgt is None:
            # Two different failures, and only one is worth reporting.
            #   addr is None + undefined: an extern we were never going to name
            #     (SDK link symbols like SDK_OVERLAY_*_ID, FX_SinCosTable_).
            #     Out of scope -- stay quiet, as before.
            #   addr recovered but no module carries it: the anchor named an
            #     address nothing owns (typo, or defined in an overlay). The
            #     rename silently does nothing, so say so even for an extern --
            #     that silence is the bug this fallback exists to kill.
            if is_userish(simple) and not known(full) and (is_def or addr is not None):
                unplaced.append((full, addr))
            continue
        if (tgt, addr) in ambiguous:
            continue
        r = tables[tgt].rename(addr, full)
        if r:
            changed.append((tgt, addr, *r))

    for tgt, addr, old, new in changed:
        where = "" if tgt == mod else f" -> {tgt}"
        print(f"  [{mod}]{where} 0x{addr:08x}  {old}  ->  {new}")
    for full, addr in unplaced:
        why = ("add a // 0xADDR anchor to its declaration" if addr is None else
               f"anchored 0x{addr:08x}, which no resident module carries a "
               f"symbol at -- wrong address, or defined in an overlay")
        print(f"  [{mod}] UNPLACED: {full} -- {why}", file=sys.stderr)

    return len(changed), unplaced, ambiguous


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--strict", action="store_true",
                    help="non-zero exit if any conflict or unplaced name remains")
    ap.add_argument("-v", "--verbose", action="store_true")
    args = ap.parse_args()

    anchors, conflicts, idents, mod_by_addr = load_anchors(args.verbose)
    modules, resident = load_config_modules()
    by_mod = sources_by_module(args.verbose)

    # Residents are always loaded: any module's TU may anchor into them.
    # Overlays only when an anchor explicitly names one (141 files otherwise).
    tables = load_tables(modules, resident + sorted(set(mod_by_addr.values())), {})

    total_changed = total_unplaced = total_ambiguous = 0
    for mod in sorted(by_mod):
        if mod not in modules:
            print(f"  WARN: module '{mod}' has source but is not in config.yaml "
                  f"-- skipped", file=sys.stderr)
            continue
        load_tables(modules, [mod], tables)
        n, unplaced, ambiguous = process_module(
            mod, by_mod[mod], tables, resident, anchors, idents,
            mod_by_addr, args.verbose)
        total_changed += n
        total_unplaced += len(unplaced)
        total_ambiguous += len(ambiguous)
        if args.verbose:
            print(f"  module {mod}: {len(by_mod[mod])} unit(s), "
                  f"{'would rename' if args.dry_run else 'renamed'} {n}",
                  file=sys.stderr)

    if not args.dry_run:
        for t in tables.values():
            t.flush()

    verb = "would rename" if args.dry_run else "renamed"
    print(f"sync_renames: {verb} {total_changed}, {total_unplaced} unplaced, "
          f"{total_ambiguous} ambiguous, {conflicts} conflict(s) "
          f"across {len(by_mod)} module(s)")
    if args.strict and (total_unplaced or conflicts):
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
