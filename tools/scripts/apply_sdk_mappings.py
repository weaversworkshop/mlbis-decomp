#!/usr/bin/env python3
"""Apply config/sdk_mappings.json renames (func_XXXX -> sdk_name) across all
symbols.txt and arm9/src + lib sources. Idempotent; a `make delinks` prereq.
Flags: --dry-run, -v/--verbose."""
import argparse
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
MAPPING_FILE = ROOT / "config" / "sdk_mappings.json"
CONFIG_ROOT = ROOT / "config"
SOURCE_ROOTS = [ROOT / "arm9" / "src", ROOT / "arm9" / "include", ROOT / "lib"]
SOURCE_EXCLUDES = [ROOT / "lib" / "NitroSDK", ROOT / "lib" / "mw-libraries"]

SYMBOL_LINE_RE = re.compile(
    r"^(?P<name>\S+)(?P<mid>\s+kind:\S+\s+addr:)(?P<addr>0x[0-9a-fA-F]+)\s*$"
)


def load_mappings():
    with open(MAPPING_FILE, "r", encoding="utf-8") as f:
        data = json.load(f)
    by_addr = {}
    for entry in data.get("mappings", []):
        addr = entry["addr"].lower()
        if addr in by_addr:
            sys.exit(f"sdk_mappings.json: duplicate addr {addr}")
        by_addr[addr] = entry
    return by_addr


def find_symbols_files():
    return sorted(CONFIG_ROOT.rglob("symbols.txt"))


def find_source_files():
    out = []
    for root in SOURCE_ROOTS:
        if not root.exists():
            continue
        for ext in ("*.c", "*.h"):
            for path in root.rglob(ext):
                if any(ex in path.parents for ex in SOURCE_EXCLUDES):
                    continue
                out.append(path)
    return out


def rename_in_symbols(symbols_files, mappings, dry_run, verbose):
    renames = {}
    unmatched_addrs = set(mappings.keys())
    conflict_msgs = []

    for sf in symbols_files:
        original_text = sf.read_text(encoding="utf-8")
        new_lines = []
        changed = False
        for raw in original_text.splitlines(keepends=True):
            m = SYMBOL_LINE_RE.match(raw.rstrip("\r\n"))
            if not m:
                new_lines.append(raw)
                continue
            addr = m.group("addr").lower()
            if addr not in mappings:
                new_lines.append(raw)
                continue
            unmatched_addrs.discard(addr)
            old_name = m.group("name")
            new_name = mappings[addr]["sdk_name"]
            if old_name == new_name:
                new_lines.append(raw)
                continue
            if old_name in renames and renames[old_name] != new_name:
                conflict_msgs.append(
                    f"{sf}: addr {addr} maps to {new_name} but {old_name} "
                    f"was earlier mapped to {renames[old_name]}"
                )
                new_lines.append(raw)
                continue
            renames[old_name] = new_name
            new_lines.append(f"{new_name}{m.group('mid')}{m.group('addr')}\n")
            if verbose:
                print(f"  {sf.relative_to(ROOT)}: {old_name} -> {new_name}")
            changed = True
        if changed and not dry_run:
            sf.write_text("".join(new_lines), encoding="utf-8")

    if conflict_msgs:
        for msg in conflict_msgs:
            print(f"ERROR: {msg}", file=sys.stderr)
        sys.exit(1)

    return renames, unmatched_addrs


def rename_in_sources(source_files, renames, dry_run, verbose):
    if not renames:
        return 0
    pattern = re.compile(r"\b(" + "|".join(re.escape(old) for old in renames) + r")\b")

    files_changed = 0
    for path in source_files:
        try:
            original = path.read_text(encoding="utf-8")
        except UnicodeDecodeError:
            continue
        new = pattern.sub(lambda m: renames[m.group(1)], original)
        if new == original:
            continue
        files_changed += 1
        if verbose:
            print(f"  {path.relative_to(ROOT)}")
        if not dry_run:
            path.write_text(new, encoding="utf-8")
    return files_changed


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--verbose", "-v", action="store_true")
    args = ap.parse_args()

    mappings = load_mappings()
    symbols_files = find_symbols_files()
    source_files = find_source_files()

    renames, unmatched = rename_in_symbols(symbols_files, mappings, args.dry_run, args.verbose)
    n_src_changed = rename_in_sources(source_files, renames, args.dry_run, args.verbose)

    print(f"SDK mappings: {len(mappings)} entries")
    print(f"Symbols renamed: {len(renames)} (across {len(symbols_files)} symbols.txt files)")
    print(f"Source files updated: {n_src_changed}")
    if unmatched:
        print(f"WARNING: {len(unmatched)} mapping addr(s) not found:", file=sys.stderr)
        for addr in sorted(unmatched):
            print(f"  {addr}  ({mappings[addr]['sdk_name']})", file=sys.stderr)
    if args.dry_run:
        print("(dry-run -- no files written)")


if __name__ == "__main__":
    main()
