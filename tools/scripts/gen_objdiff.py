#!/usr/bin/env python3
import json
import re
import sys

from common import ROOT, SDK_MAPPINGS, game_sources, load_json, objdiff_targets
from split_tcm import objdiff_base

GAP_RE = re.compile(r"_dsd_gap@(\w+)\.o$")


def game_units():
    units = []
    for path in game_sources():
        rel = path.relative_to(ROOT).with_suffix("").as_posix()
        targets = objdiff_targets(path)
        if not targets:
            print(f"gen_objdiff: {path.relative_to(ROOT).as_posix()} has no "
                  f"'// objdiff-target:' comment -- it will NOT be diffed", file=sys.stderr)
            continue
        name = rel[len("arm9/src/"):]
        for i, target in enumerate(targets):
            gap = GAP_RE.search(target)
            units.append({
                "name": name if i == 0 else f"{name}@{gap.group(1) if gap else i}",
                "target_path": target,
                "base_path": objdiff_base(f"build/{rel}.o", target, i == 0),
            })
    return units


def sdk_units():
    if not SDK_MAPPINGS.exists():
        return []
    units = []
    for e in load_json(SDK_MAPPINGS).get("objdiff_units", []):
        if "prebuilt" in e:
            units.append({"name": e["name"], "target_path": e["target"],
                          "base_path": f"build/prebuilt/{e['prebuilt']}"})
            continue
        if "object" in e:
            units.append({"name": e["name"], "target_path": e["target"],
                          "base_path": e["object"]})
            continue
        name = e.get("name") or "sdk/" + e["source"][len("lib/"):-len(".c")]
        units.append({
            "name": name,
            "target_path": e["target"],
            "base_path": objdiff_base(f"build/{e['source'][:-len('.c')]}.o", e["target"], "@" not in name),
        })
    return units


def main():
    print(json.dumps({
        "min_version": "2.0.0-beta.5",
        "custom_make": "make",
        "build_target": False,
        "build_base": True,
        "watch_patterns": ["*.c", "*.cpp", "*.h"],
        "units": game_units() + sdk_units(),
    }, indent=4))


if __name__ == "__main__":
    main()
