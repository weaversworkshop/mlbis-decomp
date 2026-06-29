#!/usr/bin/env python3
"""Emit objdiff.json (to stdout) by scanning .c for `// objdiff-target:` comments
plus SDK units from config/sdk_mappings.json.  Usage: gen_objdiff.py > objdiff.json."""
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BUILD_DIR = "build"
SCAN_DIRS = [ROOT / "arm9" / "src", ROOT / "lib"]
SCAN_EXCLUDES = [ROOT / "lib" / "NitroSDK", ROOT / "lib" / "mw-libraries"]
MAPPING_FILE = ROOT / "config" / "sdk_mappings.json"

TARGET_RE = re.compile(r"^\s*//\s*objdiff-target:\s*(.+?)\s*$")


def scan_for_comment_targets():
    units = []
    for root in SCAN_DIRS:
        if not root.exists():
            continue
        paths = sorted(
            p for ext in ("*.c", "*.cpp") for p in root.rglob(ext)
        )
        for path in paths:
            if any(ex in path.parents for ex in SCAN_EXCLUDES):
                continue
            target = None
            with open(path, "r", encoding="utf-8", errors="replace") as f:
                for line in f:
                    m = TARGET_RE.match(line)
                    if m:
                        target = m.group(1).rstrip("\r")
                        break
            rel = path.relative_to(ROOT).as_posix()
            ext_len = len(path.suffix)  # ".c" or ".cpp"
            if target is None:
                print(f"gen_objdiff: skipping {rel} (no '// objdiff-target:' comment)",
                      file=sys.stderr)
                continue
            if rel.startswith("lib/"):
                name = "sdk/" + rel[len("lib/"):-ext_len]
            else:
                name = rel[len("arm9/src/"):-ext_len]
            units.append({
                "name": name,
                "target_path": target,
                "base_path": f"{BUILD_DIR}/{rel[:-ext_len]}.o",
            })
    return units


def load_explicit_sdk_units():
    if not MAPPING_FILE.exists():
        return []
    with open(MAPPING_FILE, "r", encoding="utf-8") as f:
        data = json.load(f)
    out = []
    for entry in data.get("objdiff_units", []):
        src = entry["source"]
        name = entry.get("name") or ("sdk/" + src[len("lib/"):-len(".c")])
        out.append({
            "name": name,
            "target_path": entry["target"],
            "base_path": f"{BUILD_DIR}/{src[:-len('.c')]}.o",
        })
    return out


def main():
    units = scan_for_comment_targets() + load_explicit_sdk_units()
    print(json.dumps({
        "min_version": "2.0.0-beta.5",
        "custom_make": "make",
        "build_target": False,
        "build_base": True,
        "watch_patterns": ["*.c", "*.cpp", "*.h"],
        "units": units,
    }, indent=4))


if __name__ == "__main__":
    main()
