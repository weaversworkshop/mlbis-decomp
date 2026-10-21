#!/usr/bin/env python3
import re
import sys
from pathlib import Path

CONT = "\x00"
DRIVE_RE = re.compile(r"\b([A-Za-z]):[/\\]")


def normalize(text):
    text = re.sub(r"\\\s*$", CONT, text, flags=re.M)
    text = text.replace("\\", "/")
    text = DRIVE_RE.sub(lambda m: f"/{m.group(1).lower()}/", text)
    return text.replace(CONT, "\\")


for p in map(Path, sys.argv[1:]):
    if p.exists():
        text = p.read_text(encoding="utf-8", errors="replace")
        if (new := normalize(text)) != text:
            p.write_text(new, encoding="utf-8")
