#!/usr/bin/env python3
import re
import subprocess
import sys

from common import BUILD, OBJDUMP, ROOT

DELINKS = BUILD / "delinks"

FUNC_RE = re.compile(r"^[0-9a-f]+ <([^>]+)>:$")
# calls only: plain b/b<cond> are intra-function loops or tail calls dsd resolves by offset
BR_RE = re.compile(r"^\s*([0-9a-f]+):\t[0-9a-f]+ \t(bl|blx)\t[0-9a-f]+ <([^>]+)>")
RELOC_RE = re.compile(r"^\s*[0-9a-f]+:\s*R_ARM")


def is_local(name):
    return name.startswith(".L") or name.startswith("$")


def scan(obj):
    lines = subprocess.run([OBJDUMP, "-dr", str(obj)], capture_output=True, text=True).stdout.splitlines()
    cur = None
    gaps = []
    for i, line in enumerate(lines):
        m = FUNC_RE.match(line)
        if m:
            if not is_local(m.group(1)):
                cur = m.group(1)
            continue
        m = BR_RE.match(line)
        if not m or cur is None:
            continue
        addr, mn, tgt = m.groups()
        tgt_sym, _, addend = tgt.partition("+")
        if is_local(tgt_sym) or tgt_sym == cur:
            continue
        addend = addend or "0"
        # cross-module calls render as a negative-wrapped `sym+0xfffXXXXX`
        if addend.startswith("0xfff") or len(addend) > 7:
            continue
        if not (i + 1 < len(lines) and RELOC_RE.match(lines[i + 1])):
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
