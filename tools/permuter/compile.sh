#!/usr/bin/env bash
# Permuter compile wrapper for mwccarm.
# Called as: compile.sh <input.c> -o <output.o>
# We rewrite to: mwccarm.exe $MWC_FLAGS -c -o <output> <input>

set -e

INPUT="$1"
shift
# Remaining args include -o <OUTPUT>; pass through.

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"

export LM_LICENSE_FILE="$(cygpath -w "$REPO_ROOT/tools/mwccarm/license.dat")"

MWCCARM="$REPO_ROOT/tools/mwccarm/2.0/sp1p2/mwccarm.exe"
INCLUDE_DIR="$REPO_ROOT/arm9/include"
SHIM_INCLUDE="$REPO_ROOT/lib/nitro/include"
SDK_INCLUDE="$REPO_ROOT/lib/NitroSDK/include"
MSL_INCLUDE="$REPO_ROOT/lib/mw-libraries/ARM_EABI_Support_DS2.0/msl/MSL_C/MSL_Common/Include"
MSL_ARM_INCLUDE="$REPO_ROOT/lib/mw-libraries/ARM_EABI_Support_DS2.0/msl/MSL_C/MSL_ARM/Include"

MWC_FLAGS=(
    -proc arm946e
    -O4,p
    -ipa file
    -interworking
    -inline on
    -sym on
    -enum int
    -lang=c99
    -Cpp_exceptions off
    -gccext,on
    -gccinc
    -msgstyle gcc
    -char signed
    -DSDK_ARM9
    -i "$INCLUDE_DIR"
    -i "$SHIM_INCLUDE"
    -i "$SDK_INCLUDE"
    -i "$MSL_INCLUDE"
    -i "$MSL_ARM_INCLUDE"
    -include ansi_prefix.ARM.h
)

exec "$MWCCARM" "${MWC_FLAGS[@]}" -c "$INPUT" "$@"
