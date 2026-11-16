import json
import os
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BUILD = ROOT / "build"
CONFIG = ROOT / "config" / "arm9"
SYMBOLS = CONFIG / "symbols.txt"
SDK_MAPPINGS = ROOT / "config" / "sdk_mappings.json"
SRC = ROOT / "arm9" / "src"
INCLUDE = ROOT / "arm9" / "include"
GAP_O = BUILD / "delinks" / "_dsd_gap@main_7.o"
GAP_S = ROOT / "asm" / "_dsd_gap@main_7.s"

OBJDUMP = os.environ.get("OBJDUMP", "arm-none-eabi-objdump")
NM = os.environ.get("NM", "arm-none-eabi-nm")
READELF = os.environ.get("READELF", "arm-none-eabi-readelf")

OBJDIFF_TARGET_RE = re.compile(r"^\s*//\s*objdiff-target:\s*(.+?)\s*$")
SYMBOL_RE = re.compile(
    r"^(?P<name>\S+)\s+kind:(?P<kind>[a-z_]+)(?:\((?P<attrs>[^)]*)\))?"
    r"\s+addr:0x(?P<addr>[0-9a-fA-F]+)(?P<tail>.*?)\s*$")
SIZE_RE = re.compile(r"size=0x([0-9a-fA-F]+)")


class Symbol:
    __slots__ = ("name", "kind", "attrs", "addr", "size", "tail")

    def __init__(self, m):
        self.name, self.kind, self.attrs = m["name"], m["kind"], m["attrs"] or ""
        self.addr = int(m["addr"], 16)
        s = SIZE_RE.search(self.attrs)
        self.size = int(s.group(1), 16) if s else None
        self.tail = m["tail"]


def read_symbols(path=SYMBOLS):
    out = []
    for line in Path(path).read_text(encoding="utf-8").splitlines():
        m = SYMBOL_RE.match(line)
        if m:
            out.append(Symbol(m))
    return out


def resident_symbols():
    out = []
    for path in (SYMBOLS, CONFIG / "itcm" / "symbols.txt", CONFIG / "dtcm" / "symbols.txt"):
        if path.exists():
            out += read_symbols(path)
    return out


def word_symbols(path=SYMBOLS):
    return [s for s in read_symbols(path) if re.fullmatch(r"\w+", s.name)]


def read_relocs(path=CONFIG / "relocs.txt"):
    """sorted [(from, to)]"""
    rx = re.compile(r"from:0x([0-9a-fA-F]+)\b.*?\bto:0x([0-9a-fA-F]+)\b")
    pairs = (rx.search(line) for line in Path(path).read_text(encoding="utf-8").splitlines())
    return sorted((int(m.group(1), 16), int(m.group(2), 16)) for m in pairs if m)


def die(msg, code=2):
    print(msg, file=sys.stderr)
    sys.exit(code)


def load_json(path):
    if str(path) == "-":
        return json.load(sys.stdin)
    return json.loads(Path(path).read_text(encoding="utf-8"))


def sdk_mappings():
    if not SDK_MAPPINGS.exists():
        return []
    return load_json(SDK_MAPPINGS).get("mappings", [])


def game_sources():
    return sorted(p for ext in ("*.c", "*.cpp") for p in SRC.rglob(ext))


def objdiff_targets(path):
    out = []
    for line in Path(path).read_text(encoding="utf-8", errors="replace").splitlines():
        m = OBJDIFF_TARGET_RE.match(line)
        if m and m.group(1) not in out:
            out.append(m.group(1))
    return out


SPLIT_OBJ_RE = re.compile(r"^build/split/(.+?)(?:@\w+)?\.o$")


def compiled_object(base_path):
    m = SPLIT_OBJ_RE.match(base_path)
    return f"build/{m.group(1)}.o" if m else base_path


def objdiff_target(path):
    targets = objdiff_targets(path)
    return targets[0] if targets else None
