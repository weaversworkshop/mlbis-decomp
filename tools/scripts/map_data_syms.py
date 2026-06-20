#!/usr/bin/env python3
"""Discover NitroSDK data-global names by aligning matched functions' literal
pools: the SDK .o references a named global (sPlayer, OSi_ExContext, ...) where
the delinked ROM references an unnamed data_xxxx at the same pool position.

Only emits ADDEND-0 1:1 renames (sdk symbol has no +offset and isn't a section):
- `sPlayer+0x18` cases are skipped -- dsd needs an exact split symbol at that
  address and can't synthesize base+addend, so they can't match anyway.
- A data_xxxx that maps to >1 distinct SDK name, or an SDK name claimed by >1
  data_xxxx (ambiguous statics across TUs), is dropped.

Run with --apply to add the renames to sdk_mappings.json (so apply_sdk_mappings
renames symbols.txt + sources consistently).
"""
import argparse
import json
import re
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import tu_diff as t  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
GAP = ROOT / "build" / "delinks" / "_dsd_gap@main_7.o"
MAPPINGS = ROOT / "config" / "sdk_mappings.json"


def src_to_obj(src):
    return ROOT / "build" / (src[:-2] + ".o")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--apply", action="store_true")
    args = ap.parse_args()

    gap = t.load_funcs(str(GAP))
    cfg = json.loads(MAPPINGS.read_text(encoding="utf-8"))
    mapped_names = {m["sdk_name"] for m in cfg["mappings"]}

    # data_xxxx -> {sdk_name: count}, and reverse for conflict detection
    votes = defaultdict(lambda: defaultdict(int))
    skipped_addend = set()
    for u in cfg.get("objdiff_units", []):
        o = src_to_obj(u["source"])
        if not o.exists():
            continue
        sdk = t.load_funcs(str(o))
        for name, (key, refs) in sdk.items():
            if name not in gap:
                continue
            grefs = gap[name][1]
            if len(refs) != len(grefs):
                continue
            for (sk, sv), (gk, gv) in zip(refs, grefs):
                if sk != "rel" or gk != "rel":
                    continue
                if not gv.startswith("data_"):
                    continue
                if sv.startswith("."):  # .bss/.text section refs
                    continue
                if "+" in sv:
                    skipped_addend.add((gv, sv))
                    continue
                if not re.match(r"^[A-Za-z_]\w*$", sv):  # @8974 anon labels
                    continue
                votes[gv][sv] += 1

    # resolve: each data_xxxx -> single name; each name -> single data_xxxx
    data_to_name = {}
    conflicts = {}
    for d, names in votes.items():
        if len(names) == 1:
            data_to_name[d] = next(iter(names))
        else:
            conflicts[d] = dict(names)
    name_to_data = defaultdict(list)
    for d, n in data_to_name.items():
        name_to_data[n].append(d)
    dup_names = {n: ds for n, ds in name_to_data.items() if len(ds) > 1}
    clean = {d: n for d, n in data_to_name.items() if n not in dup_names}

    print(f"clean renames: {len(clean)}")
    for d, n in sorted(clean.items()):
        flag = "  (name already mapped elsewhere!)" if n in mapped_names else ""
        print(f"  {d} -> {n}{flag}")
    if conflicts:
        print(f"\nconflicts (data_xxxx -> multiple names), skipped: {len(conflicts)}")
        for d, ns in sorted(conflicts.items()):
            print(f"  {d} -> {ns}")
    if dup_names:
        print(f"\nambiguous statics (name -> multiple addrs), skipped: {len(dup_names)}")
        for n, ds in sorted(dup_names.items()):
            print(f"  {n} -> {ds}")
    if skipped_addend:
        print(f"\nskipped base+addend (split-symbol, irreducible): {len(skipped_addend)}")
        for gv, sv in sorted(skipped_addend):
            print(f"  {gv} = {sv}")

    if args.apply and clean:
        txt = MAPPINGS.read_text(encoding="utf-8")
        blocks = []
        for d, n in sorted(clean.items()):
            addr = "0x" + d[len("data_"):].lower().rjust(8, "0")
            blocks.append(
                '    {\n'
                f'      "addr": "{addr}",\n'
                f'      "sdk_name": "{n}",\n'
                '      "library": "data",\n'
                '      "confidence": "certain",\n'
                '      "note": "data global; literal-pool alignment of matched SDK funcs"\n'
                '    }'
            )
        idx = txt.index('"objdiff_units"')
        close = txt.rindex("]", 0, idx)
        pre = txt[:close].rstrip()
        assert pre.endswith("}")
        new = pre + ",\n" + ",\n".join(blocks) + "\n  " + txt[close:]
        json.loads(new)
        MAPPINGS.write_text(new, encoding="utf-8", newline="\n")
        print(f"\nAPPLIED {len(clean)} data mappings to sdk_mappings.json")


if __name__ == "__main__":
    main()
