#!/usr/bin/env python3
"""Inject SDK link-time constants into a dsd-generated linker script.

`dsd lcf` emits placement + overlay IDs but not the NitroSDK symbols whose
ADDRESS is the value (see CLAUDE.md "Near-match decision tree").  Undefined,
mwldarm silently resolves them to 0, leaving zeroed pointers in .rodata.

  python tools/scripts/patch_lcf.py build/arm9.lcf
"""
import re
import sys

LINK_SYMS = {
    "SDK_MAIN_ARENA_LO": 0x021277C0,
    "SDK_AUTOLOAD_ITCM_END": 0x01FFA580,
    "SDK_AUTOLOAD_DTCM_END": 0x027E1200,
    "SDK_AUTOLOAD_DTCM_START": 0x027E0000,
    "SDK_IRQ_STACKSIZE": 0x800,
    "SDK_SYS_STACKSIZE": 0x0,
}


def main():
    if len(sys.argv) != 2:
        sys.exit("usage: patch_lcf.py <arm9.lcf>")
    path = sys.argv[1]
    text = open(path, encoding="utf-8").read()
    add = [n for n in LINK_SYMS if not re.search(r"^\s*%s\s*=" % n, text, re.M)]
    if not add:
        print("patch_lcf: already patched")
        return 0
    block = "".join("    %s = 0x%08x;\n" % (n, LINK_SYMS[n]) for n in add)
    text = text.replace("SECTIONS {\n", "SECTIONS {\n" + block, 1)
    open(path, "w", encoding="utf-8", newline="\n").write(text)
    print("patch_lcf: defined %d link symbols" % len(add))
    return 0


if __name__ == "__main__":
    sys.exit(main())
