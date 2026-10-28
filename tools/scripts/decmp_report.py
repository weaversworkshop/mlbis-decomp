#!/usr/bin/env python3
import argparse
import json
import re
import sys
from pathlib import Path

from common import BUILD, CONFIG, ROOT, load_json, sdk_mappings
from obj_match import Elf

DATA_DONE = ("ok", "pad")
GAP_RE = re.compile(r"^_dsd_gap@(.+)_\d+$")
SECTION_RE = re.compile(r"^\s*(\.\S+)\s+start:(0x[0-9a-fA-F]+)")
RESIDENT = ("main", "itcm", "dtcm")


def pct(n, d):
    return 100.0 * n / d if d else 0.0


def measures(funcs, data, units=None):
    """funcs: [(size, fuzzy)]; data: [(size, matched)]; units: [(code, data, complete)]."""
    total_code = sum(s for s, _ in funcs)
    matched_code = sum(s for s, p in funcs if p == 100.0)
    total_data = sum(s for s, _ in data)
    matched_data = sum(s for s, m in data if m)
    matched_functions = sum(1 for _, p in funcs if p == 100.0)
    m = {
        "fuzzy_match_percent": pct(sum(s * p / 100.0 for s, p in funcs), total_code),
        "total_code": total_code,
        "matched_code": matched_code,
        "matched_code_percent": pct(matched_code, total_code),
        "total_data": total_data,
        "matched_data": matched_data,
        "matched_data_percent": pct(matched_data, total_data),
        "total_functions": len(funcs),
        "matched_functions": matched_functions,
        "matched_functions_percent": pct(matched_functions, len(funcs)),
    }
    if units is not None:
        complete_code = sum(c for c, _, done in units if done)
        complete_data = sum(d for _, d, done in units if done)
        m.update({
            "complete_code": complete_code,
            "complete_code_percent": pct(complete_code, total_code),
            "complete_data": complete_data,
            "complete_data_percent": pct(complete_data, total_data),
            "total_units": len(units),
            "complete_units": sum(1 for *_, done in units if done),
        })
    return m


def module_dir(module):
    return CONFIG if module == "main" else CONFIG / ("overlays" if module.startswith("ov") else "") / module


def section_starts(module):
    path = module_dir(module) / "delinks.txt"
    out = {}
    for line in path.read_text(encoding="utf-8").splitlines():
        m = SECTION_RE.match(line)
        if not m:
            break
        out[m.group(1)] = int(m.group(2), 16)
    return out


def gaps():
    """{gap stem: module} for the current delink, as listed by asm/."""
    out = {}
    for s in sorted((ROOT / "asm").glob("_dsd_gap@*.s")):
        m = GAP_RE.match(s.stem)
        if m:
            out[s.stem] = m.group(1)
    return out


def module_order(module):
    return (RESIDENT.index(module), "") if module in RESIDENT else (len(RESIDENT), module)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--report", default="-", help="obj_match JSON, or - for stdin")
    ap.add_argument("--config", default=str(ROOT / "objdiff.json"))
    ap.add_argument("-o", "--out", required=True)
    args = ap.parse_args()

    om = json.load(sys.stdin) if args.report == "-" else load_json(args.report)
    unit_gap = {u["name"]: Path(u["target_path"]).stem for u in load_json(args.config)["units"]}
    sdk_addrs = {int(e["addr"], 16) for e in sdk_mappings()}

    gap_module = gaps()
    funcs, data = {}, {}
    for stem, module in gap_module.items():
        elf = Elf(BUILD / "delinks" / f"{stem}.o")
        starts = section_starts(module) if module in RESIDENT else {}
        for name, syms in elf.functions().items():
            for s in syms:
                sec = elf.sections[s["shndx"]]["name"]
                sdk = sec in starts and starts[sec] + s["value"] in sdk_addrs
                funcs[(stem, sec, s["value"])] = {"name": name, "size": s["size"], "module": module,
                                                 "kind": "sdk" if sdk else "game"}
        for name, (_cls, s, _comdat) in elf.data_objects().items():
            data[(stem, name)] = {"size": s["size"], "module": module}

    best, owner, data_done, data_owner = {}, {}, {}, {}
    for u in om["units"]:
        stem = unit_gap.get(u["name"])
        for f in u["functions"]:
            key = (stem, f["section"], f["address"])
            if key in funcs and f["fuzzy_match_percent"] > best.get(key, -1.0):
                best[key] = f["fuzzy_match_percent"]
                owner[key] = u["name"]
        for o in u.get("objects", []):
            key = (stem, o["name"])
            if o["comdat"] or key not in data:
                continue
            data_done[key] = data_done.get(key, False) or o["status"] in DATA_DONE
            data_owner.setdefault(key, u["name"])

    def fn_rows(keys):
        return [{"name": funcs[k]["name"], "size": funcs[k]["size"],
                 "fuzzy_match_percent": best.get(k, 0.0)} for k in sorted(keys, key=lambda k: k[1:])]

    units, unit_rows = [], []
    for u in om["units"]:
        name = u["name"]
        fkeys = [k for k, o in owner.items() if o == name]
        dkeys = [k for k, o in data_owner.items() if o == name]
        if not fkeys and not dkeys:
            continue
        fs = [(funcs[k]["size"], best[k]) for k in fkeys]
        ds = [(data[k]["size"], data_done[k]) for k in dkeys]
        complete = all(p == 100.0 for _, p in fs) and all(m for _, m in ds)
        module = gap_module[unit_gap[name]]
        kinds = {funcs[k]["kind"] for k in fkeys} or {"game" if name.startswith("main/") else "sdk"}
        units.append({
            "name": name,
            "measures": measures(fs, ds),
            "functions": fn_rows(fkeys),
            "metadata": {"complete": complete, "progress_categories": sorted(kinds) + [module]},
        })
        unit_rows.append((sum(s for s, _ in fs), sum(s for s, _ in ds), complete))

    modules = sorted(set(gap_module.values()), key=module_order)
    for module in modules:
        for kind in ("game", "sdk"):
            fkeys = [k for k, f in funcs.items()
                     if k not in owner and f["module"] == module and f["kind"] == kind]
            if not fkeys:
                continue
            units.append({
                "name": f"{module}/_undecompiled" + ("_sdk" if kind == "sdk" else ""),
                "measures": measures([(funcs[k]["size"], 0.0) for k in fkeys], []),
                "functions": fn_rows(fkeys),
                "metadata": {"complete": False, "progress_categories": [kind, module]},
            })

    def cat_measures(fpred, dpred, cat):
        fs = [(f["size"], best.get(k, 0.0)) for k, f in funcs.items() if fpred(f)]
        ds = [(d["size"], data_done.get(k, False)) for k, d in data.items() if dpred(d)]
        us = [r for r, u in zip(unit_rows, units) if cat is None or cat in u["metadata"]["progress_categories"]]
        return measures(fs, ds, us)

    categories = [
        {"id": "game", "name": "Game", "measures": cat_measures(lambda f: f["kind"] == "game", lambda d: True, "game")},
        {"id": "sdk", "name": "SDK", "measures": cat_measures(lambda f: f["kind"] == "sdk", lambda d: False, "sdk")},
    ]
    for module in modules:
        categories.append({"id": module, "name": module, "measures": cat_measures(
            lambda f: f["module"] == module, lambda d: d["module"] == module, module)})

    report = {"measures": cat_measures(lambda f: True, lambda d: True, None),
              "categories": categories, "units": units}

    Path(args.out).parent.mkdir(parents=True, exist_ok=True)
    Path(args.out).write_text(json.dumps(report, indent=1), encoding="utf-8")
    m = report["measures"]
    print(f"decmp: code {m['matched_code']}/{m['total_code']} ({m['matched_code_percent']:.2f}%), "
          f"functions {m['matched_functions']}/{m['total_functions']}, "
          f"data {m['matched_data']}/{m['total_data']}, "
          f"units {m['complete_units']}/{m['total_units']}, {len(modules)} modules -> {args.out}",
          file=sys.stderr)
    for c in categories:
        cm = c["measures"]
        if c["id"].startswith("ov") and not cm["matched_functions"]:
            continue
        print(f"  {c['name']}: {cm['matched_functions']}/{cm['total_functions']} functions, "
              f"{cm['matched_code_percent']:.2f}% code", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
