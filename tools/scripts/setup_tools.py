#!/usr/bin/env python3
"""Build the pinned `dsd` from source, or confirm the existing one is current.

The pin lives in tools/dsd.pin.  dsd's --version does not track `main` (it still
prints 0.11.0), so the built revision is recorded in tools/.dsd.stamp and that is
what gets compared -- never the version string.

  python tools/scripts/setup_tools.py [--check] [--force]

Exit 0 ok / 1 needs action / 2 error.  See notes/link.md.
"""
import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
TOOLS = ROOT / "tools"
PIN = TOOLS / "dsd.pin"
STAMP = TOOLS / ".dsd.stamp"
MIN_RUST = (1, 85)   # ds-decomp is edition 2024

RUST_HELP = """dsd must be built from source and cargo was not found.

Install the Rust toolchain (one command, no admin needed):
  https://rustup.rs
    unix:    curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
    windows: winget install Rustlang.Rustup

Then re-run:  make tools"""


def read_pin():
    if not PIN.exists():
        sys.exit("error: missing %s" % PIN.relative_to(ROOT))
    pin = {}
    for line in PIN.read_text(encoding="utf-8").splitlines():
        line = line.split("#", 1)[0].strip()
        if "=" in line:
            k, v = line.split("=", 1)
            pin[k.strip()] = v.strip()
    for key in ("repo", "rev", "crate", "bin"):
        if key not in pin:
            sys.exit("error: %s missing '%s'" % (PIN.name, key))
    return pin


def binary(pin):
    name = pin["bin"] + (".exe" if os.name == "nt" else "")
    return TOOLS / "bin" / name


def current(pin):
    """Is the installed binary built from the pinned rev?"""
    if not binary(pin).exists() or not STAMP.exists():
        return False
    return STAMP.read_text(encoding="utf-8").strip() == pin["rev"]


def rust_version():
    try:
        out = subprocess.run(["cargo", "--version"], capture_output=True,
                             text=True, check=True).stdout
    except (OSError, subprocess.CalledProcessError):
        return None
    parts = out.split()
    return tuple(int(x) for x in parts[1].split(".")[:2]) if len(parts) > 1 else (0, 0)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--check", action="store_true",
                    help="report status only, never build")
    ap.add_argument("--force", action="store_true", help="rebuild even if current")
    args = ap.parse_args()
    pin = read_pin()
    short = pin["rev"][:8]

    if current(pin) and not args.force:
        print("dsd: OK %s (cached)" % short)
        return 0
    have = STAMP.read_text(encoding="utf-8").strip()[:8] if STAMP.exists() else "none"
    print("dsd: stamp mismatch (have %s, want %s)" % (have, short))
    if args.check:
        print("     run `make tools`")
        return 1

    ver = rust_version()
    if ver is None:
        print(RUST_HELP, file=sys.stderr)
        return 1
    if ver < MIN_RUST:
        print("error: cargo %d.%d is too old; ds-decomp needs >= %d.%d "
              "(run `rustup update`)" % (ver + MIN_RUST), file=sys.stderr)
        return 1

    cmd = ["cargo", "install", "--git", pin["repo"], "--rev", pin["rev"],
           "--root", str(TOOLS), "--locked", pin["crate"]]
    print("  " + " ".join(cmd))
    if subprocess.run(cmd).returncode != 0:
        print("error: cargo install failed", file=sys.stderr)
        return 1
    if not binary(pin).exists():
        print("error: cargo succeeded but %s is missing" %
              binary(pin).relative_to(ROOT), file=sys.stderr)
        return 1

    # Stamp only after the binary is in place, so a half-failed build never looks current.
    STAMP.write_text(pin["rev"] + "\n", encoding="utf-8", newline="\n")
    shutil.rmtree(TOOLS / ".crates.toml", ignore_errors=True)
    print("dsd: OK %s" % short)
    return 0


if __name__ == "__main__":
    sys.exit(main())
