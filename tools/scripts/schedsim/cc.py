import os, shutil, subprocess, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/scripts"))
import grind  # noqa: E402

STOCK = ROOT / "tools/mwccarm/2.0/sp2p3"
CC = ROOT / "build/schedsim/cc"
ENV = dict(os.environ, LM_LICENSE_FILE=str(ROOT / "tools/mwccarm/license.dat"))
# variant -> [(file offset, original bytes, patched bytes)]; see notes/mwcc_scheduler.md section 8.1
PATCHES = {
    "stock": [],
    "nosched": [(0xfee60, bytes.fromhex("53"), bytes.fromhex("c3"))],
    "p1off": [(0x710cf, bytes.fromhex("e88cdd0800"), bytes.fromhex("9090909090"))],
    "p2off": [(0x711d1, bytes.fromhex("e88adc0800"), bytes.fromhex("9090909090"))],
}


def exe(variant):
    if variant == "stock":
        return STOCK / "mwccarm.exe"
    d = CC / variant
    p = d / "mwccarm.exe"
    if not p.exists():
        d.parent.mkdir(parents=True, exist_ok=True)
        shutil.copytree(STOCK, d)
        b = bytearray(p.read_bytes())
        for off, old, new in PATCHES[variant]:
            if bytes(b[off:off + len(old)]) != old:
                raise SystemExit("error: %s: unexpected bytes at 0x%x (not sp2p3?)" % (variant, off))
            b[off:off + len(new)] = new
        p.write_bytes(bytes(b))
    return p


def command(tu, variant, obj, src=None):
    tu = Path(tu).resolve()
    cmd = grind.compile_cmd(tu)
    cmd[0] = str(exe(variant))
    return cmd + ["-c", "-o", str(obj), str(src or tu)]


def compile(tu, variant, obj, src=None):
    Path(obj).parent.mkdir(parents=True, exist_ok=True)
    r = subprocess.run(command(tu, variant, obj, src), cwd=ROOT, capture_output=True, text=True, env=ENV)
    if r.returncode != 0 or not Path(obj).exists():
        raise RuntimeError("compile failed (%s, %s): %s" % (variant, Path(tu).name, (r.stdout + r.stderr)[-300:]))
    return Path(obj)
