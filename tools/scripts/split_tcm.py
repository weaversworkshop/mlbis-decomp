#!/usr/bin/env python3
import argparse
import re
import struct
import sys
from pathlib import Path

from common import ROOT, compiled_object, load_json

SHF_WRITE, SHF_ALLOC, SHF_EXEC = 1, 2, 4
SHT_NOBITS = 8
TCM = (".itcm", ".dtcm")
DATA_ONLY = (".version", ".parent")
GAP_RE = re.compile(r"_dsd_gap@(\w+)\.o$")
SPLIT_RE = re.compile(r"^build/split/.+?(?:@(\w+))?\.o$")


def section_tcm(name):
    return next((t for t in TCM if name == t or name.startswith(t + ".")), None)


def read_sections(data):
    shoff, = struct.unpack_from("<I", data, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", data, 0x2e)
    stroff, = struct.unpack_from("<I", data, shoff + shstrndx * shentsize + 0x10)
    out = []
    for i in range(shnum):
        hdr = shoff + i * shentsize
        sh_name, sh_type, flags = struct.unpack_from("<III", data, hdr)
        off = stroff + sh_name
        out.append((hdr, data[off:data.index(b"\0", off)].decode(), sh_name, sh_type, flags))
    return out


def patched(data, keep):
    data = bytearray(data)
    secs = read_sections(data)
    text = next((sh_name for _, n, sh_name, _, _ in secs if n == ".text"), None)
    for hdr, n, sh_name, sh_type, flags in secs:
        if not flags & SHF_ALLOC:
            continue
        tcm = section_tcm(n)
        if keep == "main":
            if tcm:
                flags = 0
            elif n in DATA_ONLY:
                flags &= ~SHF_EXEC
        elif tcm != "." + keep:
            flags = 0
        elif sh_type == SHT_NOBITS:
            sh_name, flags = sh_name + len(n) - len(".bss"), SHF_WRITE | SHF_ALLOC
        elif text is not None:
            sh_name, flags = text, SHF_ALLOC | SHF_EXEC
        struct.pack_into("<III", data, hdr, sh_name, sh_type, flags)
    return bytes(data)


def objdiff_base(base_path, target, primary=True):
    obj = ROOT / base_path
    if not obj.exists():
        return base_path
    data = obj.read_bytes()
    gap = GAP_RE.search(target)
    mod = gap.group(1).split("_")[0] if gap else "main"
    stem = "build/split/" + base_path[len("build/"):-len(".o")]
    if "." + mod in TCM and not primary:
        if not any(section_tcm(n) == "." + mod for _, n, _, _, f in read_sections(data) if f & SHF_ALLOC):
            return base_path
        return f"{stem}@{gap.group(1)}.o"
    return f"{stem}.o" if patched(data, "main") != data else base_path


def split(src, dst, keep):
    out = patched(Path(src).read_bytes(), keep)
    dst = Path(dst)
    dst.parent.mkdir(parents=True, exist_ok=True)
    if not dst.exists() or dst.read_bytes() != out:
        dst.write_bytes(out)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--keep", choices=("main", "itcm", "dtcm"))
    ap.add_argument("src", nargs="?")
    ap.add_argument("dst", nargs="?")
    args = ap.parse_args()
    if args.src:
        split(args.src, args.dst, args.keep)
        return
    for unit in load_json(ROOT / "objdiff.json")["units"]:
        m = SPLIT_RE.match(unit["base_path"])
        if not m:
            continue
        obj = ROOT / compiled_object(unit["base_path"])
        if not obj.exists():
            print(f"split_tcm: missing {obj.relative_to(ROOT).as_posix()}", file=sys.stderr)
            continue
        split(obj, ROOT / unit["base_path"], m.group(1).split("_")[0] if m.group(1) else "main")


if __name__ == "__main__":
    main()
