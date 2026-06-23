#!/usr/bin/env python3
"""
Extract a single function's asm from an ELF .o (delink output) into an
assembleable .s file suitable for feeding to decomp-permuter's import.py.

Usage: extract_target_asm.py <input.o> <func_name> [-o out.s]

Output format:
    glabel <func_name>
        <instr>
        ...
    .L_xxx:
        ...
    .Lpool_xxx:
        .word <symbol>          (when reloc'd)
        .word 0x12345678        (raw)
"""
import argparse
import re
import subprocess
import sys
from pathlib import Path

OBJDUMP = r"C:\msys64\ucrt64\bin\arm-none-eabi-objdump.exe"

LABEL_RE = re.compile(r"^([0-9a-f]{8}) <([^>]+)>:\s*$")
INSN_RE = re.compile(r"^\s*([0-9a-f]+):\s+([0-9a-f]+)\s+(.*)$")
RELOC_RE = re.compile(r"^\s*([0-9a-f]+):\s+(R_ARM_\w+)\s+(\S+)\s*$")


def run_objdump(path: str) -> list[str]:
    out = subprocess.check_output(
        [OBJDUMP, "-drz", path], stderr=subprocess.STDOUT
    ).decode("utf-8", errors="replace")
    return out.splitlines()


def extract(lines: list[str], func_name: str) -> list[str]:
    # Find the function and gather all blocks belonging to it (including its
    # local .L_* labels) up until we hit another global function label.
    out: list[str] = []
    in_func = False
    func_addr_lo = None
    func_addr_hi = None
    pending_relocs: dict[str, str] = {}  # addr -> "R_ARM_XXX symbol"

    # First pass: locate function start
    for i, ln in enumerate(lines):
        m = LABEL_RE.match(ln)
        if m and m.group(2) == func_name:
            start_idx = i
            break
    else:
        sys.exit(f"function {func_name} not found")

    out.append(f"glabel {func_name}")
    i = start_idx + 1
    while i < len(lines):
        ln = lines[i].rstrip()
        if not ln:
            i += 1
            continue

        m = LABEL_RE.match(ln)
        if m:
            label_name = m.group(2)
            # If this is another global symbol (not .L_*), the function is over.
            if not label_name.startswith(".L"):
                break
            out.append(f"{label_name}:")
            i += 1
            continue

        m = INSN_RE.match(ln)
        if m:
            addr_str, _hex, body = m.groups()
            # Strip trailing comments like "@ 0xff" or "@ 15b28 <.L_xxx>"
            body = re.sub(r"\s*@.*$", "", body).rstrip()
            # Look ahead for reloc line that targets this address
            reloc_target = None
            if i + 1 < len(lines):
                rm = RELOC_RE.match(lines[i + 1])
                if rm and int(rm.group(1), 16) == int(addr_str, 16):
                    reloc_target = (rm.group(2), rm.group(3))

            # Detect .word data: objdump renders pool words with an ASCII
            # gutter (e.g. 0x0400006c -> "l...") instead of a mnemonic. Detect
            # by reconstructing that gutter from the raw hex (little-endian
            # memory order) and comparing -- robust to printable bytes that the
            # old "all dots" check missed.
            parts = body.split(None, 1)
            try:
                _mem = bytes.fromhex(_hex.zfill(8))[::-1]
                _gutter = "".join(chr(c) if 32 <= c <= 126 else "." for c in _mem)
            except ValueError:
                _gutter = None
            is_word = len(parts) == 1 and (
                parts[0].strip("\t ") == _gutter
                or re.fullmatch(r"\.+", parts[0].strip("\t "))
            )
            if is_word:
                # This is a literal pool .word. Use reloc symbol if available.
                if reloc_target:
                    sym = reloc_target[1]
                    # Strip any addend like "-0x8"
                    sym = re.sub(r"[+-]0x[0-9a-f]+$", "", sym)
                    out.append(f"    .word {sym}")
                    i += 2
                    continue
                else:
                    out.append(f"    .word 0x{_hex.zfill(8)}")
                    i += 1
                    continue

            mnemonic = parts[0]
            args = parts[1] if len(parts) > 1 else ""

            # For branches, replace numeric target with the symbol from <...>
            # objdump format: "bl  33fdc <func_02037fdc>" -> "bl func_02037fdc"
            br_m = re.match(r"^([0-9a-f]+)\s+<([^>]+)>\s*$", args)
            if br_m:
                args = br_m.group(2)

            if reloc_target:
                # BL with R_ARM_PC24 - args already contains the target symbol
                # (we replaced above), so reloc gets generated automatically.
                pass

            if args:
                out.append(f"    {mnemonic}\t{args}")
            else:
                out.append(f"    {mnemonic}")

            # Skip reloc line if we consumed it
            if reloc_target:
                i += 2
            else:
                i += 1
            continue

        # Unknown line - skip
        i += 1

    return out


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("obj")
    ap.add_argument("func")
    ap.add_argument("-o", "--output", default=None)
    args = ap.parse_args()

    lines = run_objdump(args.obj)
    asm = extract(lines, args.func)
    text = "\n".join(asm) + "\n"

    if args.output:
        Path(args.output).write_text(text, encoding="utf-8")
        print(f"wrote {args.output}")
    else:
        sys.stdout.write(text)


if __name__ == "__main__":
    main()
