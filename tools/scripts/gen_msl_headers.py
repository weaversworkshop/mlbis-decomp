import re
import sys
from pathlib import Path

from common import ROOT

MSL = ROOT / "lib/mw-libraries/ARM_EABI_Support_DS2.0/msl"
TEMPLATES = {
    "PREFIX_NITRO.h": MSL / "MSL_C/MSL_ARM/Include/PREFIX_NITRO.h.in",
    "ansi_prefix.ARM.h": MSL / "MSL_C/MSL_ARM/Include/ansi_prefix.ARM.h.in",
    "ctime": MSL / "MSL_C/MSL_Common/Include/ctime.in",
    "mslconfig": MSL / "MSL_C++/MSL_Common/Include/mslconfig.in",
}
IFELSE = re.compile(r"ifelse\(\s*eval\(LIBVER >= LIBVER_ds_2_0_\w+\), 1, (.*?)'\)dnl\n", re.S)


def first_branch(m):
    parts = re.findall(r"`(.*?)'", m.group(1), re.S)
    text = parts[0] if parts else ""
    text = re.sub(r"^dnl\n", "", text)
    return re.sub(r"define\(`LFEOF', `'\)dnl\n", "", text)


def expand(text):
    text = IFELSE.sub(first_branch, text)
    return re.sub(r"dnl\n", "", text.replace("LFEOF", ""))


def main():
    out = Path(sys.argv[1])
    out.mkdir(parents=True, exist_ok=True)
    for name, src in TEMPLATES.items():
        (out / name).write_text(expand(src.read_text(encoding="latin-1")),
                                encoding="latin-1", newline="\n")


if __name__ == "__main__":
    main()
