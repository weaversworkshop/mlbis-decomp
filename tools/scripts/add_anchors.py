#!/usr/bin/env python3
"""One-shot: add `  // 0xADDR` anchor comments to function DECLARATIONS across
game source/headers, so sync_renames can auto-place any function after a
rename/param edit.

Anchors go on the DECLARATION (the `;`-terminated declarator), NOT the
definition: class-body method decls and exported-function decls in
arm9/include/**/*.h, plus file-local forward-decls at the top of .c/.cpp.

Address source of truth = each module's symbols.txt: every function symbol's
name -> address, reduced by parse_name() to (scope, simple); func_<addr> names
embed their address. Overloaded (scope,simple) (>1 addr) are dropped (need a
manual anchor). Class scope for a header class-body decl (no explicit `X::`) is
recovered by brace-tracking the enclosing `class/struct`, mirroring sync's
load_anchors so scopes line up exactly.

Guards: the declarator needs a real return type on the line (bare calls have
none); `extern` refs, control-flow, `=`, and definitions (`{`) are rejected;
already-anchored lines are skipped. --dry-run previews; -v lists each.
"""
import argparse
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from sync_renames import (  # reuse the exact same reducers
    parse_name, load_config_modules, SYMBOL_LINE_RE, CLASS_RE,
)

ROOT = Path(__file__).resolve().parents[2]
SCAN_DIRS = [ROOT / "arm9" / "include", ROOT / "arm9" / "src"]

DECL_RE = re.compile(
    r"^(?P<indent>\s*)"
    r"(?P<pre>[A-Za-z_][\w\s\*&<>,]*?[\w\*&>]\s+\**\s*)"
    r"(?:(?P<scope>[A-Za-z_]\w*)\s*::\s*)?"
    r"(?P<name>[A-Za-z_]\w*)\s*"
    r"\((?P<args>[^;{]*)\)\s*(?:const\s*)?"
    r";\s*(?://\s*(?P<comment>.*?)\s*)?$"     # declaration ends in ';', opt comment
)
ANCHORED_RE = re.compile(r"//.*0x[0-9a-fA-F]{6,8}")
# First token that means this line is NOT a local declaration to anchor.
SKIP_FIRST = {"return", "if", "while", "for", "switch", "else", "do", "case",
              "sizeof", "typedef", "goto", "extern"}


def build_addr_map(verbose):
    """(scope, simple) -> addr from every module's symbols.txt; drop overloads."""
    raw = {}
    for mod, path in load_config_modules().items():
        if not path.exists():
            continue
        for line in path.read_text(encoding="utf-8").splitlines():
            m = SYMBOL_LINE_RE.match(line)
            if not m or "function" not in m.group("mid"):
                continue
            scope, simple = parse_name(m.group("name"))
            if simple:
                raw.setdefault((scope, simple), set()).add(int(m.group("addr"), 16))
    amap, dropped = {}, 0
    for key, addrs in raw.items():
        if len(addrs) == 1:
            amap[key] = next(iter(addrs))
        else:
            dropped += 1
            if verbose:
                print(f"  overloaded, skipped: {key[0]}::{key[1]} -> "
                      f"{sorted(hex(a) for a in addrs)}", file=sys.stderr)
    if verbose:
        print(f"  {len(amap)} unique (scope,name) addrs; {dropped} overloaded",
              file=sys.stderr)
    return amap


def process(path, amap, dry_run, verbose, skip_keys=None, record=None):
    """skip_keys: (scope,name) already anchored in a header -> don't re-anchor a
    redundant forward-decl of the same func in its own .c (header is canonical).
    record: set collecting the (scope,name) this pass anchored. Genuine
    cross-file duplicate DECLARATIONS (same func declared in two unrelated files)
    are left as-is -> sync reports them so they can be fixed in source."""
    lines = path.read_text(encoding="utf-8").splitlines(keepends=True)
    changed = 0
    depth, stack = 0, []  # brace-tracked class scope, mirrors sync.load_anchors
    for i, line in enumerate(lines):
        body = line.rstrip("\n")
        m = DECL_RE.match(body)
        if m and not ANCHORED_RE.search(body):
            first = m.group("pre").split()[0]
            if first not in SKIP_FIRST and "=" not in m.group("pre"):
                scope = m.group("scope") or (stack[-1][0] if stack else "")
                key = (scope, m.group("name"))
                addr = amap.get(key)
                if addr is None and not scope:
                    addr = amap.get(("", m.group("name")))
                if addr is not None and (skip_keys is None or key not in skip_keys):
                    if record is not None:
                        record.add(key)
                    semi = body.rindex(";")
                    code = body[:semi + 1]
                    note = m.group("comment")
                    anchor = f"  // 0x{addr:08x}" + (f" {note}" if note else "")
                    eol = "\n" if line.endswith("\n") else ""
                    lines[i] = f"{code}{anchor}{eol}"
                    changed += 1
                    if verbose:
                        q = f"{scope}::" if scope else ""
                        print(f"    {path.relative_to(ROOT)}:{i+1}  "
                              f"{q}{m.group('name')} -> 0x{addr:08x}")
        # advance class-scope stack AFTER handling this line (sync's order)
        cm = CLASS_RE.search(body)
        if cm:
            stack.append((cm.group(1), depth))
        depth += body.count("{") - body.count("}")
        while stack and depth <= stack[-1][1]:
            stack.pop()
    if changed and not dry_run:
        path.write_text("".join(lines), encoding="utf-8")
    return changed


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("-v", "--verbose", action="store_true")
    args = ap.parse_args()

    amap = build_addr_map(args.verbose)
    total, files = 0, 0
    header_keys = set()   # (scope,name) anchored in a header = canonical

    def run(src, skip_keys, record):
        nonlocal total, files
        n = process(src, amap, args.dry_run, args.verbose, skip_keys, record)
        if n:
            files += 1
            total += n
            print(f"  {'would anchor' if args.dry_run else 'anchored'} "
                  f"{n:3d}  {src.relative_to(ROOT)}")

    seen = set()
    # Pass 1: headers (canonical decls); record what got anchored.
    for d in SCAN_DIRS:
        for src in sorted(d.rglob("*.h")):
            if src not in seen:
                seen.add(src)
                run(src, None, header_keys)
    # Pass 2: .c/.cpp forward-decls, skipping funcs already anchored in a header.
    for d in SCAN_DIRS:
        for ext in ("*.c", "*.cpp"):
            for src in sorted(d.rglob(ext)):
                if src not in seen:
                    seen.add(src)
                    run(src, header_keys, None)
    print(f"add_anchors: {'would add' if args.dry_run else 'added'} {total} "
          f"anchors across {files} file(s)")


if __name__ == "__main__":
    main()
