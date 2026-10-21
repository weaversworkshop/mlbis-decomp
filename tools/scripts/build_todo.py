#!/usr/bin/env python3
import argparse
import re
import sys
from collections import Counter
from pathlib import Path

import data_catalog
from common import ROOT, load_json, sdk_mappings

START_RE = re.compile(r"^\s*(?:arm|thumb)_func_start\s+(\S+)")
END_RE = re.compile(r"^\s*(?:arm|thumb)_func_end\s+(\S+)")
ADDR_RE = re.compile(r"^\s*\S+:\s*;\s*(0x[0-9a-fA-F]+)")
DATA_RE = re.compile(r"\bdata_[0-9a-fA-F]+\b")


def load_sdk_mappings():
    return {e["addr"].lower(): e.get("library", "?") for e in sdk_mappings()}


def asm_functions(path):
    """[(name, body_lines)] in file order; names can repeat (two SDK `BeginSleep`s)."""
    lines = Path(path).read_text(encoding="utf-8", errors="replace").splitlines()
    out = []
    i = 0
    while i < len(lines):
        m = START_RE.match(lines[i])
        if not m:
            i += 1
            continue
        j = i + 1
        while j < len(lines) and not END_RE.match(lines[j]):
            j += 1
        out.append((m.group(1), lines[i + 1:j]))
        i = j + 1
    return out


def first_addr(body):
    return next((m.group(1) for m in map(ADDR_RE.match, body) if m), "")


def parse_asm(path):
    return [{
        "symbol": name,
        "address": first_addr(body),
        "lines": len(body),
        "globals": len({gm.group(0) for ln in body for gm in DATA_RE.finditer(ln)}),
    } for name, body in asm_functions(path)]


def parse_data(report):
    """name -> status dict for every homed data object (obj_match reports only)."""
    out = {}
    for unit in report.get("units", []):
        for o in unit.get("objects", []):
            prev = out.get(o["name"])
            comdat = o.get("comdat", False)
            if comdat and prev:
                # a real owner wins; otherwise tally the emitting TUs
                if not prev["comdat"]:
                    continue
                prev["tus"] += 1
                continue
            out[o["name"]] = {
                "unit": unit["name"],
                "status": o.get("status"),
                "source_size": o.get("source_size"),
                "extent": o.get("size"),
                "comdat": comdat,
                "interior": o.get("interior") or [],
                "tus": 1,
            }
    return out


def parse_report(report):
    """{symbol: {rel_addr: (unit, pct)}}; first unit at the max pct wins."""
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


def report_sections(report):
    out = {}
    for unit in report.get("units", []):
        for fn in unit.get("functions", []):
            try:
                out[(fn["name"], int(fn["address"]))] = fn.get("section")
            except (KeyError, ValueError, TypeError):
                pass
    return out


def derive_base_offset(asm, owned, sections=None):
    """asm-absolute minus report-relative address, voted over names unique in both."""
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
        # only .text addresses are module-relative (__sinit_* nodes are section-relative)
        if sections and sections.get((name, rel), ".text") != ".text":
            continue
        votes[int(recs[0]["address"], 16) - rel] += 1
    if not votes:
        return None
    if len(votes) > 1:
        print(f"WARNING: inconsistent report/asm base offsets {dict(votes)}; "
              f"duplicate-name match% may be off", file=sys.stderr)
    return votes.most_common(1)[0][0]


def lookup_owned(rec, owned, base):
    slot = owned.get(rec["symbol"])
    if not slot:
        return (None, None)
    if len(slot) == 1:
        return next(iter(slot.values()))
    if base is not None and rec["address"]:
        rel = int(rec["address"], 16) - base
        if rel in slot:
            return slot[rel]
    return max(slot.values(), key=lambda uv: uv[1])


def sort_key(row, key, desc):
    addr = int(row["address"], 16) if row["address"] else 0
    primary = {
        "match": row["match"] if row["match"] is not None else -1.0,
        "address": addr,
        "globals": row["globals"],
        "lines": row["lines"],
    }[key]
    return (-primary if desc else primary, addr)


def render_md(rows, target, sort_desc):
    total = len(rows)
    done = sum(1 for r in rows if r["match"] == 100.0)
    # "in progress" counts game functions only; SDK near-misses sit on the SDK line
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


DATA_DONE = ("ok", "pad")
DATA_NOTE = {
    "ok": "",
    "pad": "extent is alignment padding",
    "over": "**interior symbol(s)**",
    "zeros": "**zero tail too long for alignment**: undeclared trailing content",
    "short": "**ROM tail is non-zero**: undeclared trailing content",
    "diff": "**content differs**",
}


def data_note(st):
    note = DATA_NOTE.get(st["status"], st["status"])
    if st["status"] == "over" and st["interior"]:
        note += ": " + ", ".join("`%s`" % n for n in st["interior"])
    return note


def render_data(entries, status, refs, notes=False):
    by_sec = {}
    for e in entries:
        by_sec.setdefault(e["section"], []).append(e)

    total = len(entries)
    scored = [e for e in entries if e["name"] in status]
    homed = [e for e in scored if not status[e["name"]]["comdat"]]
    comdat = [e for e in scored if status[e["name"]]["comdat"]]
    done = [e for e in homed if status[e["name"]]["status"] in DATA_DONE]

    lines = ["", "## Data objects", ""]
    if not status:
        lines.append("_Data status unavailable: this report has no `objects` "
                     "(objdiff-cli does not emit them). Use `make progress-py`._")
        lines.append("")
    lines.append(f"- Homed:   {len(homed)}/{total} data symbols")
    lines.append(f"- Matched: {len(done)}/{len(homed)} homed "
                 f"({sum(1 for e in done if status[e['name']]['status'] == 'pad')} "
                 f"via alignment padding)")
    cdone = [e for e in comdat if status[e["name"]]["status"] in DATA_DONE]
    if comdat:
        lines.append(f"- COMDAT:  {len(cdone)}/{len(comdat)} byte-verified "
                     "(C++ RTTI, emitted by every TU touching the class, so `[~]`"
                     " not `[x]`: emitting it is not ownership)")
    todo = [e for e in scored if status[e["name"]]["status"] not in DATA_DONE]
    if todo and not notes:
        lines.append(f"- Needs attention: {len(todo)} object(s) ; per-object reasons "
                     "with `make progress DATA_NOTES=1`; the `[ ]` rows below mark them")
    elif todo:
        lines.append("")
        lines.append("### Needs attention")
        lines.append("")
        lines.append("| Symbol | Owner | Issue |")
        lines.append("| --- | --- | --- |")
        for e in todo:
            st = status[e["name"]]
            who = "_COMDAT_" if st["comdat"] else st["unit"]
            lines.append(f"| `{e['name']}` | {who} | {data_note(st)} |")
    lines.append("")
    lines.append("Size is the **extent** (distance to the next symbol), not `sizeof`:"
                 " data symbols carry no size in symbols.txt.")
    lines.append("")
    for sec in (".data", ".rodata", ".bss"):
        rows = sorted(by_sec.get(sec, []), key=lambda e: e["addr"])
        if not rows:
            continue
        lines.append(f"### {sec} ({len(rows)} symbols)")
        lines.append("")
        lines.append("| | Address | Symbol | Extent | sizeof | Refs | Unit |")
        lines.append("| - | --- | --- | ---: | ---: | ---: | --- |")
        for i, e in enumerate(rows):
            nxt = rows[i + 1]["addr"] if i + 1 < len(rows) else None
            extent = nxt - e["addr"] if nxt else e["space"]
            st = status.get(e["name"])
            check = " "
            if st and st["status"] in DATA_DONE:
                check = "~" if st["comdat"] else "x"
            szof = f"0x{st['source_size']:x}" if st else ""
            if not st:
                unit = ""
            elif st["comdat"]:
                unit = "_COMDAT x%d_" % st["tus"]
            else:
                unit = st["unit"]
            lines.append(
                f"| [{check}] | 0x{e['addr']:08x} | `{e['name']}` | "
                f"{'0x%x' % extent if extent else ''} | {szof} | "
                f"{refs.get(e['addr'], 0)} | {unit} |")
        lines.append("")
    return lines


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("target", help="e.g. main_7 (resolves to asm/_dsd_gap@<target>.s)")
    ap.add_argument("--asm", help="override .s path")
    ap.add_argument("--report", default=str(ROOT / "report.json"))
    ap.add_argument("--out-md", help="output .md path (default: <target>_todo.md)")
    ap.add_argument("--sort", default="match", choices=["match", "address", "globals", "lines"])
    ap.add_argument("--desc", action="store_true", help="reverse sort direction")
    ap.add_argument("--data-notes", action="store_true",
                    help="list per-object reasons for unmatched data (default: count only)")
    args = ap.parse_args()

    asm_path = args.asm or str(ROOT / "asm" / f"_dsd_gap@{args.target}.s")
    out_md = args.out_md or str(ROOT / f"{args.target}_todo.md")

    report = load_json(args.report)
    asm = parse_asm(asm_path)
    owned = parse_report(report)
    sections = report_sections(report)
    sdk = load_sdk_mappings()
    base = derive_base_offset(asm, owned, sections)

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

    desc = args.desc != (args.sort == "match")
    rows.sort(key=lambda r: sort_key(r, args.sort, desc))

    direction = "desc" if desc else "asc"
    sort_desc = f"{args.sort} {direction}, address asc tiebreaker"
    md = render_md(rows, args.target, sort_desc)

    dstatus = parse_data(report)
    dentries = data_catalog.parse_asm(Path(asm_path), data_catalog.parse_symbols())
    md += "\n".join(render_data(dentries, dstatus, data_catalog.count_refs(),
                                args.data_notes)) + "\n"

    Path(out_md).write_text(md, encoding="utf-8")
    print(f"wrote {out_md} ({len(rows)} functions, {len(dentries)} data symbols)",
          file=sys.stderr)


if __name__ == "__main__":
    main()