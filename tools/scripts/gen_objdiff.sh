#!/usr/bin/env bash
#
# gen_objdiff.sh — emit objdiff.json by scanning arm9/src/**/*.c
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

SRC_DIR="arm9/src"
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

    # Unit name: strip "arm9/src/" prefix and ".c" suffix
    name="${src#${SRC_DIR}/}"
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
done < <(find "$SRC_DIR" -type f -name '*.c' | sort)

cat <<EOF
{
    "min_version": "2.0.0-beta.5",
    "custom_make": "echo",
    "build_target": false,
    "build_base": false,
    "units": [${entries}
    ]
}
EOF
