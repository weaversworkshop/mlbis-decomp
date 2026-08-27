#!/usr/bin/env python3
"""Rewrite an mwcc `-gccdep -MMD` dependency file so GNU make can `include` it.

mwcc emits Windows paths (`C:\\Users\\...\\foo.h`) with backslash separators. Make
treats `\\` as an escape and a bare drive letter's `:` as a target separator, so the
raw file is unusable: normalize to `/c/Users/.../foo.h`. Line-continuation
backslashes are protected first, since those are the one kind make DOES want.

Idempotent (re-running on a converted file is a no-op) and silent on a missing
file, so it can sit unconditionally in a compile recipe.
"""
import re
import sys
from pathlib import Path

CONT = "\x00"          # placeholder for a real line continuation
DRIVE_RE = re.compile(r"\b([A-Za-z]):[/\\]")


def normalize(text):
    text = re.sub(r"\\\s*$", CONT, text, flags=re.M)   # protect continuations
    text = text.replace("\\", "/")                     # separators -> posix
    text = DRIVE_RE.sub(lambda m: f"/{m.group(1).lower()}/", text)
    return text.replace(CONT, "\\")


def main(argv):
    for arg in argv[1:]:
        p = Path(arg)
        if not p.exists():
            continue
        text = p.read_text(encoding="utf-8", errors="replace")
        new = normalize(text)
        if new != text:
            p.write_text(new, encoding="utf-8")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
