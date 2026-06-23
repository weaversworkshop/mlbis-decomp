#!/usr/bin/env python3
"""Build a Markdown checklist of remaining functions for a delinked target.

Requires Python 3.8+. No third-party packages.

Merges:
  - asm/<target>.s      -> address, line count, # of unique data_* globals referenced
  - objdiff report JSON -> owning unit + fuzzy match % (from `objdiff-cli report generate`)

Default sort: match % desc, address asc. Unstarted functions (no match %) sort
to the bottom by address. Use --sort to override.

Usage (preferred — via Makefile, pipes the report on stdin):
  make progress                              # default TARGET=main_7
  make progress TARGET=ov001_1

Usage (standalone):
  tools/objdiff-cli.exe report generate -p . -o - | \
      python tools/scripts/build_todo.py main_7 --report -
  # ...or read from a saved file:
  python tools/scripts/build_todo.py main_7 --report report.json --sort globals
"""
import argparse
import json
import re
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SDK_MAPPING_FILE = ROOT / "config" / "sdk_mappings.json"
START_RE = re.compile(r"^\s*(?:arm|thumb)_func_start\s+(\S+)")
END_RE = re.compile(r"^\s*(?:arm|thumb)_func_end\s+(\S+)")
ADDR_RE = re.compile(r"^\s*\S+:\s*;\s*(0x[0-9a-fA-F]+)")
DATA_RE = re.compile(r"\bdata_[0-9a-fA-F]+\b")


def load_sdk_mappings():
    if not SDK_MAPPING_FILE.exists():
        return {}
    with open(SDK_MAPPING_FILE, "r", encoding="utf-8") as f:
        data = json.load(f)
    return {e["addr"].lower(): e.get("library", "?") for e in data.get("mappings", [])}


def parse_asm(path):
    # A list, not a name-keyed dict: distinct functions can share a symbol name
    # (e.g. the two SDK `BeginSleep`s) and must both survive. Addresses are unique.
    out = []
    with open(path, "r", encoding="utf-8", errors="replace") as f:
        lines = f.readlines()
    i = 0
    while i < len(lines):
        m = START_RE.match(lines[i])
        if not m:
            i += 1
            continue
        symbol = m.group(1)
        body_start = i + 1
        j = body_start
        addr = ""
        while j < len(lines) and not END_RE.match(lines[j]):
            if not addr:
                am = ADDR_RE.match(lines[j])
                if am:
                    addr = am.group(1)
            j += 1
        body = lines[body_start:j]
        globals_used = {gm.group(0) for ln in body for gm in DATA_RE.finditer(ln)}
        out.append({
            "symbol": symbol,
            "address": addr,
            "lines": len(body),
            "globals": len(globals_used),
        })
        i = j + 1
    return out


def parse_report(path):
    """Return {symbol: {rel_addr: (unit_name, fuzzy_match_percent)}}.

    The objdiff report lists each function once per referencing unit and carries a
    per-function module-relative `address`. Keying by (name, address) keeps functions
    that share a symbol name distinct (e.g. the two `BeginSleep`s) instead of letting
    one clobber the other. As before, the first unit at the running-max pct wins, so a
    unique name resolves to exactly the value the old name-only map produced.

    Pass "-" to read from stdin.
    """
    if path == "-":
        report = json.load(sys.stdin)
    else:
        with open(path, "r", encoding="utf-8") as f:
            report = json.load(f)
    owned = {}
    for unit in report.get("units", []):
        unit_name = unit["name"]
        for fn in unit.get("functions", []):
            pct = fn.get("fuzzy_match_percent")
            if pct is None:
                continue
            name = fn["name"]
            try:
                rel = int(fn["address"])
            except (KeyError, ValueError, TypeError):
                rel = None
            slot = owned.setdefault(name, {})
            prev = slot.get(rel)
            if prev is None or pct > prev[1]:
                slot[rel] = (unit_name, pct)
    return owned


def derive_base_offset(asm, owned):
    """Constant offset between asm absolute addresses and report-relative ones.

    Voted from every symbol whose name is unique in BOTH sources (unambiguous pairing).
    Returns None if no such pair exists. If pairs disagree (should never happen for a
    clean gap) we warn and take the dominant vote rather than silently mis-keying.
    """
    asm_by_name = {}
    for rec in asm:
        asm_by_name.setdefault(rec["symbol"], []).append(rec)
    votes = Counter()
    for name, recs in asm_by_name.items():
        if len(recs) != 1 or not recs[0]["address"]:
            continue
        slot = owned.get(name)
        if not slot or len(slot) != 1:
            continue
        rel = next(iter(slot))
        if rel is None:
            continue
        votes[int(recs[0]["address"], 16) - rel] += 1
    if not votes:
        return None
    if len(votes) > 1:
        print(f"WARNING: inconsistent report/asm base offsets {dict(votes)}; "
              f"duplicate-name match% may be off", file=sys.stderr)
    return votes.most_common(1)[0][0]


def lookup_owned(rec, owned, base):
    """(unit, pct) for one asm function, disambiguating duplicate names by address."""
    slot = owned.get(rec["symbol"])
    if not slot:
        return (None, None)
    if len(slot) == 1:                       # unique name: identical to the old map
        return next(iter(slot.values()))
    if base is not None and rec["address"]:  # duplicate name: pick by absolute address
        rel = int(rec["address"], 16) - base
        if rel in slot:
            return slot[rel]
    return max(slot.values(), key=lambda uv: uv[1])  # last-resort: best pct


def sort_key(row, key, desc):
    pct = row["match"] if row["match"] is not None else -1.0
    addr = int(row["address"], 16) if row["address"] else 0
    if key == "match":
        primary = pct
    elif key == "address":
        primary = addr
    elif key == "globals":
        primary = row["globals"]
    elif key == "lines":
        primary = row["lines"]
    else:
        raise SystemExit(f"unknown sort key: {key}")
    if desc:
        return (-primary if isinstance(primary, (int, float)) else primary, addr)
    return (primary, addr)


def render_md(rows, target, sort_desc):
    total = len(rows)
    done = sum(1 for r in rows if r["match"] == 100.0)
    # "in progress" = started-but-unmatched GAME functions only (SDK near-misses
    # like SND_FlushCommand / NNS_SndPlayerSetTrackMuteEx are tracked under the
    # SDK line, not as outstanding work).
    started = sum(1 for r in rows if r["match"] is not None and r["match"] < 100.0 and not r["sdk"])

    sdk_total = sum(1 for r in rows if r["sdk"])
    sdk_done = sum(1 for r in rows if r["sdk"] and r["match"] == 100.0)
    game_total = total - sdk_total
    game_done = done - sdk_done
    game_pct = 100.0 * game_done / game_total if game_total else 0.0

    lines = []
    lines.append(f"# {target} -- {done}/{total} matched ({started} in progress)")
    lines.append("")
    lines.append(f"- Game:  {game_done}/{game_total} matched ({game_pct:.2f}%)")
    lines.append(f"- SDK:   {sdk_done}/{sdk_total} matched  (functions identified in config/sdk_mappings.json)")
    lines.append("")
    lines.append(f"_Sorted by: {sort_desc}_")
    lines.append("")
    lines.append("| | Address | Symbol | Kind | Unit | Match | Lines | Globals |")
    lines.append("| - | --- | --- | --- | --- | ---: | ---: | ---: |")
    for r in rows:
        check = "x" if r["match"] == 100.0 else " "
        pct = "" if r["match"] is None else f"{r['match']:.2f}%"
        unit = r["unit"] or "_unassigned_"
        kind = f"SDK ({r['sdk']})" if r["sdk"] else "game"
        lines.append(
            f"| [{check}] | {r['address']} | `{r['symbol']}` | {kind} | {unit} | {pct} | {r['lines']} | {r['globals']} |"
        )
    return "\n".join(lines) + "\n"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("target", help="e.g. main_7 (resolves to asm/_dsd_gap@<target>.s)")
    ap.add_argument("--asm", help="override .s path")
    ap.add_argument("--report", default=str(ROOT / "report.json"))
    ap.add_argument("--out-md", help="output .md path (default: <target>_todo.md)")
    ap.add_argument("--sort", default="match", choices=["match", "address", "globals", "lines"])
    ap.add_argument("--desc", action="store_true", help="reverse sort direction")
    args = ap.parse_args()

    asm_path = args.asm or str(ROOT / "asm" / f"_dsd_gap@{args.target}.s")
    out_md = args.out_md or str(ROOT / f"{args.target}_todo.md")

    asm = parse_asm(asm_path)
    owned = parse_report(args.report)
    sdk = load_sdk_mappings()
    base = derive_base_offset(asm, owned)

    rows = []
    for rec in asm:
        unit, pct = lookup_owned(rec, owned, base)
        rows.append({
            "symbol": rec["symbol"],
            "address": rec["address"],
            "lines": rec["lines"],
            "globals": rec["globals"],
            "unit": unit,
            "match": pct,
            "sdk": sdk.get(rec["address"].lower(), None),
        })

    # Default behavior: match desc, address asc. Other keys: asc by default, --desc flips.
    if args.sort == "match":
        desc = not args.desc  # default desc for match
    else:
        desc = args.desc
    rows.sort(key=lambda r: sort_key(r, args.sort, desc))

    direction = "desc" if desc else "asc"
    sort_desc = f"{args.sort} {direction}, address asc tiebreaker"
    Path(out_md).write_text(render_md(rows, args.target, sort_desc), encoding="utf-8")
    print(f"wrote {out_md} ({len(rows)} rows)", file=sys.stderr)


if __name__ == "__main__":
    main()