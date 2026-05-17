#!/usr/bin/env bash
#
# gen_objdiff.sh — emit objdiff.json by scanning arm9/src/**/*.c and lib/**/*.c
#
# Each .c file should contain a line of the form:
#     // objdiff-target: build/delinks/_dsd_gap@main_7.o
# (anywhere in the file; first match wins). Files without that
# comment are skipped with a warning on stderr.
#
# Writes JSON to stdout. Invoke from project root or via the Makefile.

set -euo pipefail

# Move to project root (script lives at tools/scripts/, root is two up)
cd "$(dirname "$0")/../.."

SRC_DIRS=("arm9/src" "lib")
BUILD_DIR="build"

first=1
entries=""

while IFS= read -r src; do
    target=$(grep -m1 -E '^[[:space:]]*//[[:space:]]*objdiff-target:' "$src" \
             | sed -E 's|^[[:space:]]*//[[:space:]]*objdiff-target:[[:space:]]*||' \
             | tr -d '\r' \
             | sed -E 's/[[:space:]]+$//' \
             || true)

    if [[ -z "$target" ]]; then
        echo "gen_objdiff: skipping $src (no '// objdiff-target:' comment)" >&2
        continue
    fi

    # Unit name: strip leading "arm9/src/" if present (game code), else keep
    # the path as-is so lib/ entries surface as e.g. "lib/syscall/CpuFastSet".
    name="${src#arm9/src/}"
    name="${name%.c}"

    # Base path: mirror source tree under build/  (matches Makefile rule)
    base="${BUILD_DIR}/${src%.c}.o"

    if [[ $first -eq 1 ]]; then
        first=0
    else
        entries+=","
    fi
    entries+=$'\n        {'
    entries+=$'\n            "name": "'"$name"'",'
    entries+=$'\n            "target_path": "'"$target"'",'
    entries+=$'\n            "base_path": "'"$base"'"'
    entries+=$'\n        }'
    # lib/NitroSDK is the ntrtwl submodule with its own build system —
    # don't include its sources in objdiff until we deliberately wire pieces in.
done < <(find "${SRC_DIRS[@]}" -type f -name '*.c' -not -path 'lib/NitroSDK/*' 2>/dev/null | sort)

cat <<EOF
{
    "min_version": "2.0.0-beta.5",
    "custom_make": "make",
    "build_target": false,
    "build_base": true,
    "watch_patterns": ["*.c", "*.h"],
    "units": [${entries}
    ]
}
EOF
