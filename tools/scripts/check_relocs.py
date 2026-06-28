#!/usr/bin/env python3
"""Audit delinked objects for a branch that crosses a symbol boundary with no
relocation (the `bl <invalid>` / relocs.txt-gap signature; a delink-faithfulness
check, not match-%). Exit 1 if any gap. No args; run after `make delinks`."""
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DELINKS = ROOT / "build" / "delinks"
OBJDUMP = "arm-none-eabi-objdump"

FUNC_RE = re.compile(r"^[0-9a-f]+ <([^>]+)>:$")
# a CALL (bl/blx) with a resolved "<target>" annotation. We deliberately do NOT
# flag plain b/b<cond>: those are overwhelmingly intra-function loops (objdump
# labels them `func+0xNN` when no .L symbol exists) or tail calls, which dsd
# resolves by offset within the object -- not the trampoline class we care about.
BR_RE = re.compile(
    r"^\s*([0-9a-f]+):\t[0-9a-f]+ \t(bl|blx)\t[0-9a-f]+ <([^>]+)>"
)
RELOC_RE = re.compile(r"^\s*[0-9a-f]+:\s*R_ARM")


def scan(obj):
    out = subprocess.run([OBJDUMP, "-dr", str(obj)], capture_output=True, text=True).stdout
    lines = out.splitlines()
    cur = None
    gaps = []
    for i, line in enumerate(lines):
        m = FUNC_RE.match(line)
        if m:
            name = m.group(1)
            if not (name.startswith(".L") or name.startswith("$")):
                cur = name
            continue
        m = BR_RE.match(line)
        if not m or cur is None:
            continue
        addr, mn, tgt = m.group(1), m.group(2), m.group(3)
        tgt_sym = tgt.split("+")[0]
        if tgt_sym.startswith(".L") or tgt_sym.startswith("$"):
            continue  # intra-function local target
        if tgt_sym == cur:
            continue  # self / recursion into own body
        # cross-module calls render as `sym+0xfffXXXXX` (negative wrap); objdump
        # can't resolve them in-object -- they're handled via module: relocs.
        addend = tgt.split("+")[1] if "+" in tgt else "0"
        if addend.startswith("0xfff") or len(addend) > 7:
            continue
        has_reloc = i + 1 < len(lines) and RELOC_RE.match(lines[i + 1])
        if not has_reloc:
            gaps.append((cur, addr, mn, tgt))
    return gaps


def main():
    objs = sorted(DELINKS.glob("*.o")) if DELINKS.exists() else []
    if not objs:
        print("check-relocs: no delinked objects in build/delinks (run `make delinks`)",
              file=sys.stderr)
        return 1
    total = 0
    for obj in objs:
        gaps = scan(obj)
        if not gaps:
            continue
        total += len(gaps)
        print(f"\n{obj.relative_to(ROOT).as_posix()}: {len(gaps)} missing-reloc branch(es)")
        for fn, addr, mn, tgt in gaps:
            print(f"  {fn} @ 0x{addr}: {mn} -> {tgt}  (no relocation; add to relocs.txt)")
    if total:
        print(f"\n{total} missing relocation(s). Each is an `arm_call`/branch the "
              f"delink left un-symbolized -- add `from:0x<addr> kind:arm_call "
              f"to:0x<target> module:<mod>` to the matching relocs.txt.")
        return 1
    print(f"check-relocs: clean ({len(objs)} delinked objects, no missing branch relocations)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
