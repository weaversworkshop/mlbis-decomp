#!/usr/bin/env python3
import argparse
import json
import re
from collections import defaultdict

import tu_diff as t
from common import BUILD, GAP_O, SDK_MAPPINGS as MAPPINGS, load_json


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--apply", action="store_true")
    args = ap.parse_args()

    gap = t.load_funcs(str(GAP_O))
    cfg = load_json(MAPPINGS)
    mapped_names = {m["sdk_name"] for m in cfg["mappings"]}

    votes = defaultdict(lambda: defaultdict(int))
    skipped_addend = set()
    for u in cfg.get("objdiff_units", []):
        o = BUILD / (u["source"][:-2] + ".o")
        if not o.exists():
            continue
        for name, (_key, refs) in t.load_funcs(str(o)).items():
            if name not in gap:
                continue
            grefs = gap[name][1]
            if len(refs) != len(grefs):
                continue
            for (sk, sv), (gk, gv) in zip(refs, grefs):
                if sk != "rel" or gk != "rel" or not gv.startswith("data_") or sv.startswith("."):
                    continue
                if "+" in sv:
                    skipped_addend.add((gv, sv))
                    continue
                if not re.match(r"^[A-Za-z_]\w*$", sv):
                    continue
                votes[gv][sv] += 1

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
