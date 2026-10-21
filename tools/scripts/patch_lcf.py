#!/usr/bin/env python3
import re
import sys
from pathlib import Path

# Address-as-value SDK symbols; `dsd lcf` omits them and mwldarm silently resolves them to 0.
LINK_SYMS = {
    "SDK_MAIN_ARENA_LO": 0x021277C0,
    "SDK_AUTOLOAD_ITCM_END": 0x01FFA580,
    "SDK_AUTOLOAD_DTCM_END": 0x027E1200,
    "SDK_AUTOLOAD_DTCM_START": 0x027E0000,
    "SDK_IRQ_STACKSIZE": 0x800,
    "SDK_SYS_STACKSIZE": 0x0,
}


def patch(path):
    path = Path(path)
    text = path.read_text(encoding="utf-8")
    add = [n for n in LINK_SYMS if not re.search(r"^\s*%s\s*=" % n, text, re.M)]
    if not add:
        return "patch_lcf: already patched"
    block = "".join("    %s = 0x%08x;\n" % (n, LINK_SYMS[n]) for n in add)
    path.write_text(text.replace("SECTIONS {\n", "SECTIONS {\n" + block, 1),
                    encoding="utf-8", newline="\n")
    return "patch_lcf: defined %d link symbols" % len(add)


def main():
    if len(sys.argv) != 2:
        sys.exit("usage: patch_lcf.py <arm9.lcf>")
    print(patch(sys.argv[1]))
    return 0


if __name__ == "__main__":
    sys.exit(main())
