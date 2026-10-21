#!/usr/bin/env python3
import argparse
import re
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "scripts"))
from common import OBJDUMP  # noqa: E402

LABEL_RE = re.compile(r"^([0-9a-f]{8}) <([^>]+)>:\s*$")
INSN_RE = re.compile(r"^\s*([0-9a-f]+):\s+([0-9a-f]+)\s+(.*)$")
RELOC_RE = re.compile(r"^\s*([0-9a-f]+):\s+(R_ARM_\w+)\s+(\S+)\s*$")


def run_objdump(path: str) -> list[str]:
    out = subprocess.check_output([OBJDUMP, "-drz", path], stderr=subprocess.STDOUT)
    return out.decode("utf-8", errors="replace").splitlines()


def ascii_gutter(hexword: str):
    try:
        mem = bytes.fromhex(hexword.zfill(8))[::-1]
    except ValueError:
        return None
    return "".join(chr(c) if 32 <= c <= 126 else "." for c in mem)


def extract(lines: list[str], func_name: str) -> list[str]:
    for start, ln in enumerate(lines):
        m = LABEL_RE.match(ln)
        if m and m.group(2) == func_name:
            break
    else:
        sys.exit(f"function {func_name} not found")

    out = [f"glabel {func_name}"]
    i = start + 1
    while i < len(lines):
        ln = lines[i].rstrip()
        m = LABEL_RE.match(ln)
        if m:
            if not m.group(2).startswith(".L"):
                break
            out.append(f"{m.group(2)}:")
            i += 1
            continue
        m = INSN_RE.match(ln)
        if not m:
            i += 1
            continue
        addr, hexword, body = m.groups()
        body = re.sub(r"\s*@.*$", "", body).rstrip()
        reloc = None
        if i + 1 < len(lines):
            rm = RELOC_RE.match(lines[i + 1])
            if rm and int(rm.group(1), 16) == int(addr, 16):
                reloc = rm.group(3)
        i += 2 if reloc else 1

        parts = body.split(None, 1)
        word = parts[0].strip("\t ") if len(parts) == 1 else None
        # objdump renders pool words as an ASCII gutter instead of a mnemonic
        if word is not None and (word == ascii_gutter(hexword) or re.fullmatch(r"\.+", word)):
            out.append(f"    .word {reloc}" if reloc else f"    .word 0x{hexword.zfill(8)}")
            continue

        args = parts[1] if len(parts) > 1 else ""
        br = re.match(r"^([0-9a-f]+)\s+<([^>]+)>\s*$", args)
        if br:
            args = br.group(2)
        out.append(f"    {parts[0]}\t{args}" if args else f"    {parts[0]}")
    return out


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("obj")
    ap.add_argument("func")
    ap.add_argument("-o", "--output", default=None)
    args = ap.parse_args()

    text = "\n".join(extract(run_objdump(args.obj), args.func)) + "\n"
    if args.output:
        Path(args.output).write_text(text, encoding="utf-8")
        print(f"wrote {args.output}")
    else:
        sys.stdout.write(text)


if __name__ == "__main__":
    main()
