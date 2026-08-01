#!/usr/bin/env python3
"""
SDK compile audit: attempt to compile EVERY SDK/library .c file (including the
ones in the Makefile SDK_SKIP list) and report which compile, which fail, and
the failure reason. Surfaces the blind spot: failing/skipped files produce no
.o, so tu_diff can never content-match their functions against the ROM.

Usage:
  python tools/scripts/sdk_compile_audit.py            # audit, print summary
  python tools/scripts/sdk_compile_audit.py --reasons  # also group by error
  python tools/scripts/sdk_compile_audit.py --stub H   # add extra -include H
"""
import argparse
import os
import subprocess
import sys
from collections import Counter, defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
MWCC = ROOT / "tools/mwccarm/2.0/sp1p2/mwccarm.exe"
LICENSE = ROOT / "tools/mwccarm/license.dat"
MSL = "lib/mw-libraries/ARM_EABI_Support_DS2.0/msl/MSL_C"

INCLUDES = [
    "arm9/include", "lib/nitro/include", "lib/NitroSDK/include",
    "lib/NitroSystem/include",
    f"{MSL}/MSL_Common/Include", f"{MSL}/MSL_ARM/Include",
]
BASE_FLAGS = [
    "-proc", "arm946e", "-O4,p", "-ipa", "file", "-interworking",
    "-inline", "on", "-sym", "on", "-enum", "int", "-lang=c99",
    "-Cpp_exceptions", "off", "-gccext,on", "-gccinc", "-msgstyle", "gcc",
    "-char", "signed", "-DSDK_ARM9", "-DSDK_FINALROM", "-DSDK_THREAD_INFINITY",
    "-DSDK_CODE_ARM", "-DSDK_TS",
]

SRC_ROOTS = [
    "lib/NitroSDK/libraries",
    "lib/NitroSystem",
    "lib/mw-libraries/ARM_EABI_Support_DS2.0",
    "lib/syscall",
]


def enumerate_c_files():
    files = []
    for r in SRC_ROOTS:
        for p in (ROOT / r).rglob("*.c"):
            files.append(p)
    return sorted(set(files))


def compile_one(c, extra_includes, stubs):
    rel = c.relative_to(ROOT)
    incs = []
    for i in INCLUDES:
        incs += ["-i", i]
    # per-file include conveniences mirroring the Makefile's per-dir rules
    incs += ["-i", str(c.parent), "-i", str(c.parent.parent / "include"),
             "-i", str(c.parent.parent.parent / "include"),
             "-i", f"{MSL}/MSL_Common_Embedded/Math/Include"]
    # DS2.0's MSL tree lacks <ctime>; mirror the Makefile's MSL_CTIME_FB fallback.
    if rel.as_posix().startswith(f"{MSL}/MSL_Common/Src/"):
        incs += ["-i", "lib/mw-libraries/ARM_EABI_Support/msl/MSL_C/MSL_Common/Include"]
    for e in extra_includes:
        incs += ["-i", e]
    inc_flags = ["-include", "ansi_prefix.ARM.h"]
    for s in stubs:
        inc_flags += ["-include", s]
    outo = ROOT / "build/_audit_tmp.o"
    outo.parent.mkdir(parents=True, exist_ok=True)
    cmd = [str(MWCC), *BASE_FLAGS, *incs, *inc_flags, "-c", "-o", str(outo), str(c)]
    env = dict(os.environ, LM_LICENSE_FILE=str(LICENSE), MWCIncludes="arm9/include")
    try:
        if outo.exists():
            outo.unlink()
        r = subprocess.run(cmd, cwd=ROOT, env=env, capture_output=True,
                           text=True, timeout=120)
    except subprocess.TimeoutExpired:
        return rel, False, "TIMEOUT"
    if r.returncode == 0:
        return rel, True, None
    # extract the most informative error line
    err = ""
    for line in (r.stderr + r.stdout).splitlines():
        ll = line.lower()
        if "undefined identifier" in ll or "could not" in ll or "cannot open" in ll \
           or "illegal" in ll or "error:" in ll or "expected" in ll:
            err = line.strip()
            break
    return rel, False, err or "(unknown error)"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--reasons", action="store_true")
    ap.add_argument("--stub", action="append", default=[])
    ap.add_argument("--inc", action="append", default=[])
    ap.add_argument("--list-fail", action="store_true")
    args = ap.parse_args()

    files = enumerate_c_files()
    ok, fail = [], []
    reasons = Counter()
    by_dir = defaultdict(lambda: [0, 0])
    for c in files:
        rel, good, err = compile_one(c, args.inc, args.stub)
        top = str(rel).split("/")[1] if "/" in str(rel) else str(rel)
        # group by library dir for readability
        parts = str(rel).split("/")
        lib = "/".join(parts[:3]) if len(parts) >= 3 else parts[0]
        if good:
            ok.append(rel); by_dir[lib][0] += 1
        else:
            fail.append((rel, err)); by_dir[lib][1] += 1
            # normalize reason (strip file:line prefix)
            norm = err.split(":", 2)[-1].strip() if ":" in err else err
            reasons[norm[:70]] += 1
        sys.stderr.write(f"\r{len(ok)+len(fail)}/{len(files)} "); sys.stderr.flush()
    sys.stderr.write("\n")

    print(f"\n=== SDK COMPILE AUDIT ===")
    print(f"total .c files : {len(files)}")
    print(f"compiled OK    : {len(ok)}")
    print(f"FAILED         : {len(fail)}")
    print(f"\n--- by library (ok / fail) ---")
    for lib in sorted(by_dir):
        o, f = by_dir[lib]
        mark = "  <-- has failures" if f else ""
        print(f"  {o:4d} / {f:<4d}  {lib}{mark}")
    if args.reasons:
        print(f"\n--- failure reasons (count) ---")
        for r, n in reasons.most_common():
            print(f"  {n:4d}  {r}")
    if args.list_fail:
        print(f"\n--- failing files ---")
        for rel, err in fail:
            print(f"  {rel}\n        {err}")


if __name__ == "__main__":
    main()
