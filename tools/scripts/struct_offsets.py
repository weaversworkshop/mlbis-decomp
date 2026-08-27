#!/usr/bin/env python3
"""Annotate/verify `// +0xNN` member-offset comments on game structs & classes.

Offsets come from mwcc itself: a generated probe TU takes `&((T*)0)->m` for every
member, compiles with the real game flags (sp2p3, -lang=c++), and the resulting
`.data` words ARE the offsets. Nothing here re-derives mwcc layout rules
(base-class prefix, vtable ptr, alignment, bitfield packing).

  --check (default)  report missing/stale comments, exit 1 if any
  --apply            rewrite the headers in place

`+0x` is a reserved sigil: sync_renames.py's anchor regexes refuse an address
preceded by '+', so offsets never collide with `// 0xADDR` anchors.

Bitfields can't use that route (`offsetof` is illegal on them), so their storage
unit comes from mwcc's DWARF (`-sym on`), cross-checked against the probe on every
non-bitfield member of the same type. Only the FIRST line of each run is annotated;
no bit numbering. `--no-bitfields` turns it off.

Skipped by design: self-encoding fieldNN/pad_NN names (--all includes them).
"""
import argparse
import hashlib
import json
import os
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BUILD = ROOT / "build" / "offsets"
INCLUDE_ROOT = ROOT / "arm9" / "include"
DEFAULT_DIRS = ["arm9/include/main", "arm9/src/main"]

AGG_OPEN_RE = re.compile(r"\b(?:class|struct|union)\s+(?P<name>[A-Za-z_]\w*)"
                         r"(?:\s*:[^{;]*)?\s*\{")
ANON_OPEN_RE = re.compile(r"\b(?P<kind>struct|union)\s*\{")
TYPEDEF_CLOSE_RE = re.compile(r"^\s*\}\s*(?P<name>[A-Za-z_]\w*)\s*;")
MEMBER_RE = re.compile(r"^\s*.*?\b(?P<name>[A-Za-z_]\w*)\s*(?:\[[^\]]*\]\s*)*;")
BITFIELD_RE = re.compile(r"^\s*.*?(?:\b(?P<name>[A-Za-z_]\w*)\s*)?:\s*\d+\s*;")
OFFSET_CMT_RE = re.compile(r"//\s*\+0x[0-9a-fA-F]+")
ADDR_CMT_RE = re.compile(r"//.*(?<!\+)\b0x[0-9a-fA-F]{6,8}\b")
SELF_ENCODING_RE = re.compile(r"^(?:field|pad_|unk_)[0-9a-fA-F]+")
NOT_A_MEMBER = re.compile(r"^\s*(?:$|//|/\*|\*|#|\}|public:|private:|protected:"
                          r"|typedef\b|using\b|friend\b|static\b|enum\b|virtual\b)")
# A function BODY opening: `...) {` (optionally const / ctor-init-list). Used to
# ignore structs declared as function locals. `extern "C" {` and namespaces also
# open braces at depth 0, so brace depth alone can NOT stand in for this.
FUNC_OPEN_RE = re.compile(r"\)\s*(?:const\s*)?(?::[^;{]*)?\{\s*$")
CONTROL_RE = re.compile(r"^\s*(?:if|for|while|switch|else|do|catch)\b")


def make_vars():
    """Makefile `X := ...` assignments, recursively expanded."""
    raw = {}
    for line in (ROOT / "Makefile").read_text(encoding="utf-8").splitlines():
        m = re.match(r"^(?:export\s+)?(\w+)\s*:?=\s*(.*)$", line)
        if m:
            raw.setdefault(m.group(1), m.group(2))

    def expand(v, depth=0):
        if depth > 10:
            return v
        return re.sub(r"\$\((\w+)\)",
                      lambda m: expand(raw.get(m.group(1), ""), depth + 1), v)
    return {k: expand(v) for k, v in raw.items()}


VARS = make_vars()
FLAGS = [f for f in VARS.get("MWC_FLAGS", "").split() if f != "-lang=c99"]
FLAGS.append("-lang=c++")
CC = ROOT / VARS.get("TOOLS_DIR", "tools") / "mwccarm/2.0/sp2p3/mwccarm.exe"
LICENSE = ROOT / VARS.get("LICENSE", "tools/mwccarm/license.dat")
OBJDUMP = "arm-none-eabi-objdump"
READELF = "arm-none-eabi-readelf"


CACHE = BUILD / "cache.json"
INCLUDE_RE = re.compile(r'^\s*#\s*include\s+"([^"]+)"')


def local_includes(path, seen=None):
    """Transitive closure of QUOTED includes resolvable inside arm9/include.
    SDK/system headers are excluded: they don't change, and hashing them every
    run would cost more than the compile we're trying to skip."""
    seen = seen if seen is not None else set()
    if path in seen or not path.exists():
        return seen
    seen.add(path)
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        m = INCLUDE_RE.match(line)
        if not m:
            continue
        for cand in (INCLUDE_ROOT / m.group(1), path.parent / m.group(1)):
            if cand.exists():
                local_includes(cand, seen)
                break
    return seen


def fingerprint(path, opts):
    """Content key for one header: itself, every local header it pulls in, the
    TU used for the whole-TU fallback, the compiler, and the flags. Editing an
    unrelated .cpp must NOT invalidate it - that's the whole point."""
    h = hashlib.sha1()
    h.update(repr(opts).encode() + "\0".join(FLAGS).encode())
    # This script's own logic decides what gets probed and how it's parsed, so a
    # change to it must invalidate every entry.
    h.update(hashlib.sha1(Path(__file__).read_bytes()).digest())
    try:
        st = CC.stat()
        h.update(f"{st.st_mtime_ns}:{st.st_size}".encode())
    except OSError:
        pass
    files = local_includes(path)
    tu = tu_for(path)
    if tu:
        files = files | {tu}
    for f in sorted(files):
        h.update(f.name.encode())
        h.update(hashlib.sha1(f.read_bytes()).digest())
    return h.hexdigest()


def load_cache():
    try:
        return json.loads(CACHE.read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return {}


class Member:
    __slots__ = ("type_name", "name", "lineno", "offset", "in_union")

    def __init__(self, type_name, name, lineno, in_union=False):
        self.type_name, self.name, self.lineno = type_name, name, lineno
        self.offset, self.in_union = None, in_union


def parse(path):
    """-> (members, bitfields). Brace-tracked. Members of an anonymous
    struct/union are transparent, so they attribute to the enclosing named
    type; `} Name;` names an anonymous aggregate retroactively."""
    # stack: [name, depth, is_union, ignored]; func_levels: depths of open bodies
    members, bitfields, stack, depth = [], [], [], 0
    func_levels = []
    for lineno, line in enumerate(
            path.read_text(encoding="utf-8", errors="replace").splitlines(), 1):
        code = line.split("//")[0]
        agg = AGG_OPEN_RE.search(code)
        anon = ANON_OPEN_RE.search(code)
        enclosing = next((e[0] for e in reversed(stack) if e[0]), None)
        in_union = any(e[2] for e in stack)
        ignored = any(e[3] for e in stack) or bool(func_levels)

        if stack and not ignored and not agg and not anon and "(" not in code \
                and not NOT_A_MEMBER.match(line):
            bf = BITFIELD_RE.match(code)
            if bf and not re.search(r"\w\s*::\s*\w", code):
                # `u8 : 2;` has ONE identifier before the ':' (the type), so the
                # regex's name group caught the type, not a member name.
                unnamed = len(re.findall(r"[A-Za-z_]\w*",
                                         code.split(":")[0])) < 2
                bitfields.append([enclosing,
                                  "<anon>" if unnamed else bf.group("name"),
                                  lineno])
            else:
                mm = MEMBER_RE.match(code)
                if mm:
                    members.append(
                        Member(enclosing, mm.group("name"), lineno, in_union))

        if agg or anon:
            # An aggregate opening at brace depth > 0 with nothing on the stack is
            # declared INSIDE a function body (`struct { ... } w;` as a local), so
            # its `} w;` is a variable, not a type: naming it would emit `((w*)0)`.
            stack.append([agg.group("name") if agg else None, depth,
                          bool(anon and anon.group("kind") == "union"), ignored])
        tc = TYPEDEF_CLOSE_RE.match(code)
        if tc and stack and stack[-1][0] is None and not stack[-1][3]:
            for mem in members:
                if mem.type_name is None:
                    mem.type_name = tc.group("name")
            for bf in bitfields:
                if bf[0] is None:
                    bf[0] = tc.group("name")
            stack[-1][0] = tc.group("name")
        if not agg and not anon and FUNC_OPEN_RE.search(code) \
                and not CONTROL_RE.match(code):
            func_levels.append(depth)
        depth += code.count("{") - code.count("}")
        while stack and depth <= stack[-1][1]:
            stack.pop()
        while func_levels and depth <= func_levels[-1]:
            func_levels.pop()
    return [m for m in members if m.type_name], bitfields


def read_probe(obj, count):
    """Read `count` words of the _probe array out of the object.

    Addressed by SECTION INDEX, not name: mwcc emits every data object into its
    own section and names them all `.data`, so `objdump -j .data` merges them
    and silently yields another object's bytes."""
    t = subprocess.run([READELF, "-sW", str(obj)], cwd=ROOT,
                       capture_output=True, text=True)
    ndx = off = None
    for line in t.stdout.splitlines():
        f = line.split()
        # Num: Value Size Type Bind Vis Ndx Name
        if len(f) >= 8 and f[-1] == "_probe" and f[-2].isdigit():
            off, ndx = int(f[1], 16), int(f[-2])
    if ndx is None:
        return None
    d = subprocess.run([READELF, "-x", str(ndx), str(obj)], cwd=ROOT,
                       capture_output=True, text=True)
    blob = {}
    for line in d.stdout.splitlines():
        mm = re.match(r"^\s*0x([0-9a-f]+)\s((?:[0-9a-f]{2,8}\s){1,4})", line)
        if mm:
            addr = int(mm.group(1), 16)
            for w in mm.group(2).split():
                for i, b in enumerate(bytes.fromhex(w)):
                    blob[addr + i] = b
                addr += len(w) // 2
    try:
        raw = bytes(blob[off + i] for i in range(count * 4))
    except KeyError:
        return None
    return [int.from_bytes(raw[i * 4:i * 4 + 4], "little") for i in range(count)]


def sweep_probes():
    """Probe TUs/objects are read in-process; only cache.json outlives the run."""
    for f in BUILD.glob("*"):
        if f.name != CACHE.name and f.is_file():
            try:
                f.unlink()
            except OSError:
                pass


def compile_src(text, stem, suffix, verbose=False):
    """Write and compile a generated TU. -> object Path, or None."""
    BUILD.mkdir(parents=True, exist_ok=True)
    src = BUILD / (stem + suffix)
    src.write_text(text, encoding="utf-8")
    obj = src.with_suffix(".o")
    flags = list(FLAGS)
    if suffix == ".c":
        flags = [f for f in flags if f != "-lang=c++"] + ["-lang=c99"]
    flags += ["-i", str(ROOT / "arm9/src/main")]
    env = dict(os.environ, LM_LICENSE_FILE=str(LICENSE), MWCIncludes="arm9/include")
    r = subprocess.run([str(CC), *flags, "-c", "-o", str(obj), str(src)],
                       cwd=ROOT, env=env, capture_output=True, text=True)
    if r.returncode != 0:
        if verbose:
            print((r.stdout + r.stderr).strip(), file=sys.stderr)
        return None
    return obj


def tu_for(path):
    """The game TU that includes this header, if any. A source file IS its own
    TU: a struct declared inside a .c/.cpp is unreachable any other way."""
    if path.suffix in (".c", ".cpp"):
        return path
    needle = f'"main/{path.name}"'
    for src in sorted((ROOT / "arm9/src").rglob("*.c")) +             sorted((ROOT / "arm9/src").rglob("*.cpp")):
        for line in src.read_text(encoding="utf-8", errors="replace").splitlines():
            if line.startswith("#include") and needle in line:
                return src
    return None


def probe_via_tu(path, members, verbose=False):
    """Last resort: some headers only compile inside their TU (types defined in
    the .cpp). Append the probe table to a copy of the whole TU and build that."""
    tu = tu_for(path)
    if tu is None:
        return False
    BUILD.mkdir(parents=True, exist_ok=True)
    # SENTINEL: an all-zero initializer goes to .bss, which has no bytes to read
    # (bit any probe whose every member sits at offset 0). Keep the array nonzero.
    body = ",\n".join(["    0xffffffffUL"]
                      + [f"    (unsigned long)&(({m.type_name}*)0)->{m.name}"
                         for m in members])
    src = BUILD / (path.stem + "_tu_probe" + tu.suffix)
    src.write_text(tu.read_text(encoding="utf-8", errors="replace")
                   + f"\n\nunsigned long _probe[] = {{\n{body}\n}};\n",
                   encoding="utf-8")
    obj = src.with_suffix(".o")
    flags = list(FLAGS)
    if tu.suffix == ".c":
        flags = [f for f in flags if f != "-lang=c++"] + ["-lang=c99"]
    flags += ["-i", str(ROOT / "arm9/src/main")]
    env = dict(os.environ, LM_LICENSE_FILE=str(LICENSE), MWCIncludes="arm9/include")
    r = subprocess.run([str(CC), *flags, "-c", "-o", str(obj), str(src)],
                       cwd=ROOT, env=env, capture_output=True, text=True)
    if r.returncode != 0:
        if verbose:
            print((r.stdout + r.stderr).strip(), file=sys.stderr)
        return False
    words = read_probe(obj, len(members) + 1)
    if words is None or words[0] != 0xffffffff:
        return False
    for mem, off in zip(members, words[1:]):
        mem.offset = off
    return True


def prelude_for(path):
    """Some headers aren't self-contained (they rely on include order). Recover a
    known-good order from a real TU that includes this header: take that file's
    #include lines through the one naming it. Mechanical - no guessing."""
    needle = f'"main/{path.name}"'
    for src in sorted((ROOT / "arm9/src").rglob("*.c")) +             sorted((ROOT / "arm9/src").rglob("*.cpp")):
        lines = src.read_text(encoding="utf-8", errors="replace").splitlines()
        for i, line in enumerate(lines):
            if line.startswith("#include") and needle in line:
                return [x for x in lines[:i] if x.startswith("#include")]
    return None


def probe(path, members, verbose=False, prelude=None):
    """Compile a probe TU for these members; fill .offset. -> ok?
    Headers only: a .c/.cpp can't be #included, so a source file's own structs
    are reached through probe_via_tu instead."""
    if path.suffix != ".h":
        return False
    BUILD.mkdir(parents=True, exist_ok=True)
    rel = path.resolve().relative_to(INCLUDE_ROOT).as_posix()
    src = BUILD / (path.stem + "_probe.cpp")
    obj = BUILD / (path.stem + "_probe.o")
    # SENTINEL: an all-zero initializer goes to .bss, which has no bytes to read
    # (bit any probe whose every member sits at offset 0). Keep the array nonzero.
    body = ",\n".join(["    0xffffffffUL"]
                      + [f"    (unsigned long)&(({m.type_name}*)0)->{m.name}"
                         for m in members])
    src.write_text(f'#include "{rel}"\nunsigned long _probe[] = {{\n{body}\n}};\n',
                   encoding="utf-8")
    env = dict(os.environ, LM_LICENSE_FILE=str(LICENSE), MWCIncludes="arm9/include")
    r = subprocess.run([str(CC), *FLAGS, "-c", "-o", str(obj), str(src)],
                       cwd=ROOT, env=env, capture_output=True, text=True)
    if r.returncode != 0:
        if verbose:
            print((r.stdout + r.stderr).strip(), file=sys.stderr)
        return False
    words = read_probe(obj, len(members) + 1)
    if words is None or words[0] != 0xffffffff:
        return False
    for mem, off in zip(members, words[1:]):
        mem.offset = off
    return True


def bisect_probe(path, members, verbose, prelude=None):
    """Whole-file probe failed: keep only members that compile individually."""
    good = []
    for mem in members:
        if probe(path, [mem], False, prelude):
            good.append(mem)
        elif verbose:
            print(f"  probe failed: {mem.type_name}::{mem.name}", file=sys.stderr)
    return good


def sanity(members):
    """Distinct members of one type sharing an offset is legal only in a union,
    and is also the exact signature of a bad extraction (it once read another
    object's section). Warn loudly rather than annotate silently."""
    seen, bad = {}, []
    for m in members:
        if m.in_union:
            continue
        key = (m.type_name, m.offset)
        if key in seen and seen[key] != m.name:
            bad.append(f"{m.type_name}: {seen[key]} and {m.name} both +0x{m.offset:x}")
        seen.setdefault(key, m.name)
    return bad


def dwarf_offsets(path, types, prelude=None, verbose=False):
    """{(type, member): byte offset} from mwcc's own debug info (-sym on).

    `offsetof` is illegal on a bitfield, but DWARF carries a byte offset for
    EVERY member, so this is the only compiler-authoritative source for a
    bitfield's storage unit. A type is only emitted if something references it,
    hence the pointer variables."""
    decls = "\n".join(f"{t} * _ty{i};" for i, t in enumerate(sorted(types)))
    head = "\n".join(prelude or []) + ("\n" if prelude else "")
    attempts = []
    if path.suffix == ".h":
        rel = path.resolve().relative_to(INCLUDE_ROOT).as_posix()
        attempts.append((f'{head}#include "{rel}"\n{decls}\n', ".cpp"))
    tu = tu_for(path)
    if tu is not None:
        attempts.append((tu.read_text(encoding="utf-8", errors="replace")
                         + f"\n{decls}\n", tu.suffix))
    obj = None
    for text, suffix in attempts:
        obj = compile_src(text, path.stem + "_dw", suffix, verbose)
        if obj:
            break
    if obj is None:
        return {}
    d = subprocess.run([READELF, "--debug-dump=info", str(obj)], cwd=ROOT,
                       capture_output=True, text=True)
    return parse_dwarf(d.stdout)


AGG_TAGS = ("DW_TAG_structure_type", "DW_TAG_class_type", "DW_TAG_union_type")
DIE_RE = re.compile(r"\s*<(\d+)><([0-9a-f]+)>:.*\((DW_TAG_\w+)\)")
AT_NAME_RE = re.compile(r"DW_AT_name\s*:\s*(\S+)")
AT_LOC_RE = re.compile(r"DW_AT_data_member_location.*?DW_OP_const4u:\s*(\d+)")
AT_TYPE_RE = re.compile(r"DW_AT_type\s*:\s*<0x([0-9a-f]+)>")


def parse_dwarf(text):
    """{(type, member): byte offset} from `readelf --debug-dump=info`.

    Two passes, because a C `typedef struct {...} T;` emits an ANONYMOUS
    structure DIE whose name lives on a separate DW_TAG_typedef pointing at it.
    (C++ names the struct directly, which is why headers worked and .c files
    silently produced nothing.) Members attribute to the nearest enclosing
    aggregate that resolves to a name, so anonymous struct/union members still
    land on their outer type."""
    dies = []                       # (depth, offset, tag, attrs)
    for line in text.splitlines():
        m = DIE_RE.match(line)
        if m:
            dies.append((int(m.group(1)), int(m.group(2), 16), m.group(3), {}))
            continue
        if not dies:
            continue
        attrs = dies[-1][3]
        m = AT_LOC_RE.search(line)
        if m:
            attrs["loc"] = int(m.group(1))
            continue
        m = AT_TYPE_RE.search(line)
        if m:
            attrs["type"] = int(m.group(1), 16)
            continue
        m = AT_NAME_RE.search(line)
        if m:
            attrs.setdefault("name", m.group(1))

    # typedef name -> the DIE it aliases; first wins (matches source order)
    alias = {}
    for _d, _o, tag, at in dies:
        if tag == "DW_TAG_typedef" and "name" in at and "type" in at:
            alias.setdefault(at["type"], at["name"])
    agg_name = {off: (at.get("name") or alias.get(off))
                for _d, off, tag, at in dies if tag in AGG_TAGS}

    out, stack = {}, []             # stack: [(depth, die offset)]
    for depth, off, tag, at in dies:
        while stack and stack[-1][0] >= depth:
            stack.pop()
        if tag in AGG_TAGS:
            stack.append((depth, off))
        elif tag == "DW_TAG_member" and "name" in at and "loc" in at:
            owner = next((agg_name.get(o) for _dd, o in reversed(stack)
                          if agg_name.get(o)), None)
            if owner:
                out.setdefault((owner, at["name"]), at["loc"])
    return out


def bitfield_heads(bitfields, dw, probed):
    """One synthetic Member per bitfield RUN (a maximal group of consecutive
    bitfield lines sharing a storage unit), anchored on the run's FIRST line.
    Unnamed fillers (`u8 : 6;`) carry no DWARF entry, so they inherit the offset
    of the next named bitfield in the same group."""
    heads, run = [], []

    def flush():
        if not run:
            return
        offs = [dw.get((t, n)) for t, n, _ in run]
        # walk backwards so an unnamed leading filler inherits from its successor
        filled, last = [], None
        for o in reversed(offs):
            last = o if o is not None else last
            filled.append(last)
        filled.reverse()
        start = 0
        for i in range(1, len(run) + 1):
            if i == len(run) or filled[i] != filled[start]:
                if filled[start] is not None:
                    t, n, ln = run[start]
                    m = Member(t, n, ln)
                    m.offset = filled[start]
                    heads.append(m)
                start = i
        run.clear()

    prev_ln = prev_t = None
    for t, n, ln in bitfields:
        if prev_ln is not None and (ln != prev_ln + 1 or t != prev_t):
            flush()
        run.append((t, n, ln))
        prev_ln, prev_t = ln, t
    flush()
    # a run head must not collide with a real member we already annotated
    taken = {m.lineno for m in probed}
    return [m for m in heads if m.lineno not in taken]


def render(lines, members):
    """-> (new_lines, [(lineno, kind, text)])"""
    out, notes = list(lines), []
    for mem in sorted({m.lineno: m for m in members}.values(),
                      key=lambda m: m.lineno):
        line = out[mem.lineno - 1]
        want = f"// +0x{mem.offset:x}"
        code, sep, tail = line.partition("//")
        cmt = (sep + tail) if sep else ""
        if cmt and ADDR_CMT_RE.search(cmt) and not OFFSET_CMT_RE.search(cmt):
            notes.append((mem.lineno, "skip", f"{mem.name}: address anchor present"))
            continue
        have = OFFSET_CMT_RE.search(cmt)
        if have:
            if have.group(0).strip() == want:
                continue
            notes.append((mem.lineno, "stale",
                          f"{mem.name}: {have.group(0).strip()} -> {want}"))
            cmt = OFFSET_CMT_RE.sub(want, cmt, count=1)
        else:
            notes.append((mem.lineno, "add", f"{mem.name}: {want}"))
            rest = tail.strip()
            cmt = f"{want}  {rest}" if rest else want
        out[mem.lineno - 1] = code.rstrip() + "  " + cmt.rstrip()
    return out, notes


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("paths", nargs="*", help="headers or dirs "
                    f"(default: {' '.join(DEFAULT_DIRS)})")
    ap.add_argument("--apply", action="store_true", help="rewrite files in place")
    ap.add_argument("--all", action="store_true",
                    help="also annotate fieldNN/pad_NN (self-encoding) members")
    ap.add_argument("--no-cache", action="store_true",
                    help="ignore build/offsets/cache.json and re-probe everything")
    ap.add_argument("--no-bitfields", action="store_true",
                    help="skip bitfield-run heads (DWARF-sourced)")
    ap.add_argument("--keep-probes", action="store_true",
                    help="keep generated probe TUs (implied by -v)")
    ap.add_argument("-v", "--verbose", action="store_true")
    a = ap.parse_args()

    targets = []
    for p in (a.paths or DEFAULT_DIRS):
        p = Path(p) if Path(p).is_absolute() else ROOT / p
        if p.is_dir():
            for pat in ("*.h", "*.c", "*.cpp"):
                targets += sorted(p.rglob(pat))
        else:
            targets.append(p)

    probed = pending = 0
    failed, bitfields, head_lines = [], [], set()
    opts = (a.all, a.no_bitfields)
    cache = {} if a.no_cache else load_cache()
    new_cache, hits = {}, 0
    for path in targets:
        members, bfs = parse(path)
        if not a.all:
            members = [m for m in members if not SELF_ENCODING_RE.match(m.name)]
        if not members:
            continue
        rel = path.relative_to(ROOT).as_posix()
        fp = fingerprint(path, opts)
        hit = cache.get(rel)
        if hit and hit.get("fp") == fp:
            # Every compile for this header is skipped; the offsets are a pure
            # function of the inputs the fingerprint covers.
            new_cache[rel] = hit
            hits += 1
            probed += hit["probed"]
            bitfields += [b for b in bfs]
            head_lines |= set(hit["heads"])
            cached = []
            for ln, off, name in hit["ann"]:
                m = Member(None, name, ln)
                m.offset = off
                cached.append(m)
            lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
            new, notes = render(lines, cached)
            if notes:
                for lineno, kind, text in notes:
                    print(f"  {kind:5} {rel}:{lineno}  {text}")
                pending += sum(1 for _, k, _ in notes if k in ("add", "stale"))
                if a.apply:
                    path.write_text("\n".join(new) + "\n", encoding="utf-8")
            continue
        pre = None
        if not probe(path, members, False):
            pre = prelude_for(path)
            if pre is None or not probe(path, members, False, pre):
                if not probe_via_tu(path, members, a.verbose):
                    members = bisect_probe(path, members, a.verbose, pre)
            if not members:
                failed.append(path)
                continue
        bitfields += bfs
        n_probed = len(members)          # BEFORE bitfield heads are merged in
        probed += n_probed
        file_heads = set()
        for warn in sanity(members):
            print(f"  WARN  {path.relative_to(ROOT).as_posix()}  {warn}")
        if bfs and not a.no_bitfields:
            types = {m.type_name for m in members} | {b[0] for b in bfs if b[0]}
            dw = dwarf_offsets(path, types, pre, a.verbose)
            # Cross-validate DWARF against the probe on every member we already
            # know: they must agree, or the debug info isn't describing this
            # layout and its bitfield offsets can't be trusted either.
            drift = {m.type_name for m in members
                     if dw.get((m.type_name, m.name), m.offset) != m.offset}
            for t in sorted(drift):
                print(f"  WARN  {path.relative_to(ROOT).as_posix()}  {t}: "
                      f"DWARF disagrees with probe; bitfields skipped")
            heads = [h for h in bitfield_heads(bfs, dw, members)
                     if h.type_name not in drift]
            file_heads = {h.lineno for h in heads}
            head_lines |= file_heads
            members = members + heads
        new_cache[rel] = {
            "fp": fp, "probed": n_probed, "heads": sorted(file_heads),
            "ann": [[m.lineno, m.offset, m.name] for m in members
                    if m.offset is not None],
        }
        lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
        new, notes = render(lines, members)
        if notes:
            for lineno, kind, text in notes:
                print(f"  {kind:5} {rel}:{lineno}  {text}")
            pending += sum(1 for _, k, _ in notes if k in ("add", "stale"))
            if a.apply:
                path.write_text("\n".join(new) + "\n", encoding="utf-8")
    # A write changes the file, hence its fingerprint: drop the entries we just
    # rewrote so the next run re-probes them instead of trusting a stale key.
    if a.apply and pending:
        new_cache = {k: v for k, v in new_cache.items()
                     if fingerprint(ROOT / k, opts) == v["fp"]}
    if not a.no_cache and targets:
        try:
            BUILD.mkdir(parents=True, exist_ok=True)
            CACHE.write_text(json.dumps(new_cache), encoding="utf-8")
        except OSError:
            pass
    if not (a.verbose or a.keep_probes or failed):
        sweep_probes()
    for path in failed:
        print(f"PROBE FAILED {path.relative_to(ROOT).as_posix()} (rerun with -v)")
    if a.verbose:
        for t, n, ln in bitfields:
            if ln not in head_lines:
                print(f"  bitfield {t}::{n} line {ln} (in-run, no comment)")
    if hits:
        print(f"  ({hits} unchanged headers served from cache)")
    print(f"{probed} members probed, {pending} "
          f"{'rewritten' if a.apply else 'need annotation'}; "
          f"{len(bitfields)} bitfields in {len(head_lines)} runs")
    return 1 if (pending and not a.apply) or failed else 0


if __name__ == "__main__":
    sys.exit(main())
