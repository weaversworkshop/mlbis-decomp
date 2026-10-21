#!/usr/bin/env python3
import json
import sys

from common import ROOT, SDK_MAPPINGS, game_sources, load_json, objdiff_target


def game_units():
    units = []
    for path in game_sources():
        rel = path.relative_to(ROOT).with_suffix("").as_posix()
        target = objdiff_target(path)
        if target is None:
            print(f"gen_objdiff: {path.relative_to(ROOT).as_posix()} has no "
                  f"'// objdiff-target:' comment -- it will NOT be diffed", file=sys.stderr)
            continue
        units.append({
            "name": rel[len("arm9/src/"):],
            "target_path": target,
            "base_path": f"build/{rel}.o",
        })
    return units


def sdk_units():
    if not SDK_MAPPINGS.exists():
        return []
    return [{
        "name": e.get("name") or "sdk/" + e["source"][len("lib/"):-len(".c")],
        "target_path": e["target"],
        "base_path": f"build/{e['source'][:-len('.c')]}.o",
    } for e in load_json(SDK_MAPPINGS).get("objdiff_units", [])]


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
