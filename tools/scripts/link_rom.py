#!/usr/bin/env python3
"""Relink every module from delinked objects and byte-compare against the ROM.

  python tools/scripts/link_rom.py [-v] [--keep-carve]

Pipeline: gen_delinks --with-data -> dsd delink -> dsd lcf -> patch_lcf -> mwldarm.
The lcf's MEMORY block writes the module binaries itself (paths are relative to the
-o ELF's directory, hence build/link/build/).

delinks.txt is ALWAYS restored on exit: carving moves target functions out of the gap,
where obj_match cannot find them, so leaving a carve in place makes `make progress`
report 0/0. Differences are reported, not fatal -- pass --strict
to exit non-zero on any.
"""
import argparse
import os
import re
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS = ROOT / "tools" / "scripts"
CFG = ROOT / "config" / "arm9" / "config.yaml"
DELINKS_TXT = ROOT / "config" / "arm9" / "delinks.txt"
STAMP = ROOT / "build" / "delinks" / ".stamp"
BUILD = ROOT / "build"
LINK = BUILD / "link"
LCF = BUILD / "arm9.lcf"
OBJECTS = BUILD / "objects.txt"
DSD = ROOT / "tools" / "bin" / ("dsd.exe" if os.name == "nt" else "dsd")
MWLDARM = ROOT / "tools" / "mwccarm" / "2.0" / "sp2p3" / "mwldarm.exe"
LICENSE = ROOT / "tools" / "mwccarm" / "license.dat"

# Linked binary basename -> extracted reference.
EXTRACT = ROOT / "extract"
OV_RE = re.compile(r"^arm9_(ov\d+)$")


def reference(stem):
    if stem == "arm9":
        return EXTRACT / "arm9" / "arm9.bin"
    if stem in ("itcm", "dtcm"):
        return EXTRACT / "arm9" / (stem + ".bin")
    m = OV_RE.match(stem)
    return EXTRACT / "arm9_overlays" / (m.group(1) + ".bin") if m else None


def run(cmd, what, env=None, quiet=True):
    r = subprocess.run([str(c) for c in cmd], capture_output=True, text=True,
                       env=env, cwd=str(ROOT))
    if r.returncode != 0:
        sys.stderr.write(r.stdout + r.stderr)
        sys.exit("link_rom: %s failed" % what)
    if not quiet:
        sys.stdout.write(r.stdout)
    return r.stdout + r.stderr


def compare(verbose):
    rows, total, exact = [], 0, 0
    classes = {}
    for f in sorted((LINK / "build").glob("*.bin")):
        ref = reference(f.stem)
        if ref is None or not ref.exists():
            rows.append((f.stem, None))
            continue
        a, b = f.read_bytes(), ref.read_bytes()
        if len(a) != len(b):
            rows.append((f.stem, "size %d vs %d" % (len(a), len(b))))
            continue
        diff = [(i, x, y) for i, (x, y) in enumerate(zip(a, b)) if x != y]
        total += len(diff)
        if diff:
            rows.append((f.stem, "%d bytes" % len(diff)))
            for _, x, y in diff:
                classes["0x%02x->0x%02x" % (x, y)] = \
                    classes.get("0x%02x->0x%02x" % (x, y), 0) + 1
        else:
            exact += 1
    return rows, total, exact, classes


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("-v", "--verbose", action="store_true")
    ap.add_argument("--keep-carve", action="store_true",
                    help="leave the data carve in delinks.txt (breaks `make progress`)")
    ap.add_argument("--strict", action="store_true",
                    help="exit non-zero if any module differs (default: report only)")
    args = ap.parse_args()

    for tool in (DSD, MWLDARM):
        if not tool.exists():
            sys.exit("link_rom: missing %s (run `make tools`)" % tool)

    saved = DELINKS_TXT.read_bytes()
    try:
        run([sys.executable, SCRIPTS / "gen_delinks.py", "--with-data"],
            "gen_delinks", quiet=not args.verbose)
        run([DSD, "delink", "-c", CFG], "dsd delink")
        run([DSD, "lcf", "-c", CFG], "dsd lcf")
        run([sys.executable, SCRIPTS / "patch_lcf.py", LCF], "patch_lcf",
            quiet=not args.verbose)

        # mwldarm resolves the MEMORY block's output paths relative to the ELF dir.
        (LINK / "build").mkdir(parents=True, exist_ok=True)
        env = dict(os.environ, LM_LICENSE_FILE=str(LICENSE))
        log = run([MWLDARM, "-proc", "arm946e", "-nostdlib", "-main", "",
                   "-o", LINK / "dsd.elf", LCF, "@%s" % OBJECTS], "mwldarm", env=env)
        if args.verbose:
            sys.stdout.write(log)

        rows, total, exact, classes = compare(args.verbose)
    finally:
        if args.keep_carve:
            print("link_rom: carve LEFT IN PLACE -- `make progress` will report 0/0")
        else:
            DELINKS_TXT.write_bytes(saved)
            STAMP.unlink(missing_ok=True)   # force an uncarved re-delink next time

    noref = [n for n, r in rows if r is None]
    bad = [(n, r) for n, r in rows if r]
    print("link: %d modules byte-exact, %d differ, %d bytes total"
          % (exact, len(bad), total))
    for n, r in bad:
        print("  %-12s %s" % (n, r))
    if classes:
        print("  byte classes: " + ", ".join("%s x%d" % (k, v)
                                             for k, v in sorted(classes.items())))
    if noref:
        print("  no reference for: " + ", ".join(noref))
    return 1 if (bad and args.strict) else 0


if __name__ == "__main__":
    sys.exit(main())
