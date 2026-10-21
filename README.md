# mlbis

In-progress matching decompilation of **Mario & Luigi: Bowser's Inside Story** (Nintendo DS, 2009).

The goal is C/C++ source that compiles byte-for-byte to the retail ROM.

## AI Disclosure

This project uses NO AI for any actual decompilation work. All C/C++ functions are hand-authored by humans. Only some python tooling is AI-assisted, but still stress tested and verified by humans.

## Status

| | |
|---|---|
| `main_7` functions matched | **1295 / 1849** |
| game code | 548/1099 matched (49.86%) |
| SDK / NitroSystem | 735 / 737 |
| data objects homed | 77 / 520 (70 verified byte-exact, + 38 RTTI COMDAT) |
| Relink vs ROM | 134 modules byte-exact, main 9 bytes off (35 bytes total) |

`make progress` regenerates the live per-function and per-data-object tables (`main_7_todo.md`).

## Requirements

Submodules (`git submodule update --init --recursive`):

| Submodule | Source |
|---|---|
| `lib/NitroSDK` | [ntrtwl/NitroSDK](https://github.com/ntrtwl/NitroSDK) |
| `lib/NitroSystem` | [ntrtwl/NitroSystem](https://github.com/ntrtwl/NitroSystem) |
| `lib/mw-libraries` | [mid-kid/mw-libraries](https://github.com/mid-kid/mw-libraries) (MSL C) |

You must provide:

| Item | Path | Notes |
|---|---|---|
| Original ROM | `rom/M_and_L_BIS.nds` | You must legally own it. |
| `mwccarm` | `tools/mwccarm/` | CodeWarrior for DS. Needs **both** `2.0/sp1p2` (SDK libs) and `2.0/sp2p3` (game TUs). |
| `mwccarm` license | `tools/mwccarm/license.dat` | FLEXlm. |
| `objdiff` GUI | `tools/objdiff.exe` | [encounter/objdiff](https://github.com/encounter/objdiff) |
| `objdiff-cli` | `tools/objdiff-cli.exe` | Same project. |

Toolchain:

- **GNU Make 4.x** + **bash** (Git Bash / MSYS2): recipes are bash.
- **Python 3**: everything in `tools/scripts/`.
- **[rustup](https://rustup.rs) ≥ 1.85**: `make tools` builds `dsd` from a pinned commit.
- **`arm-none-eabi-objdump`** on PATH (MSYS2 binutils).
- **Windows** is the only tested platform.

`dsd` is *not* a download. It is built from the commit pinned in `tools/dsd.pin`, because no
released version can link this ROM's 141 overlays. See the runbook.

## Setup

```bash
git clone <repo-url> mlbis && cd mlbis
git submodule update --init --recursive
# drop the provided items above into their paths (gitignored)

make tools                 # builds pinned dsd -> tools/bin/ (first run: a few minutes)
tools/bin/dsd.exe rom extract --rom rom/M_and_L_BIS.nds --output-path extract
make diff                  # build + delink + open objdiff
```

(`tools/bin/dsd` without `.exe` off Windows; the Makefile picks the right one.)

## Documentation

- **[docs/RUNBOOK.md](docs/RUNBOOK.md)**: daily workflow, adding a TU, the matching loop,
  SDK mapping, relinking, and bumping the pinned toolchain.

## Layout

```
arm9/include/  arm9/src/     game headers + source (one TU per file)
asm/                         dsd disassembly (generated, gitignored)
build/                       compiled .o; build/delinks/ = dsd target .o
config/arm9/                 dsd config: config.yaml, symbols.txt, relocs.txt, delinks.txt
config/sdk_mappings.json     ROM addr -> SDK symbol map + objdiff units
docs/                        committed documentation
lib/                         NitroSDK, NitroSystem, mw-libraries (submodules)
rom/  extract/               your ROM + dsd extraction (gitignored)
tools/scripts/               Python helpers
tools/dsd.pin                pinned dsd commit (built by `make tools`)
main_7_todo.md               generated progress report
```

## Conventions

- One TU per `.c`/`.cpp`, named `unk_<addr>` until it earns a semantic name.
- C++ class TUs use `.cpp` (auto-compiled `-lang=c++`; RTTI TUs get `-RTTI on`).
- NitroSDK types (`u8`/`s32`/…), see `arm9/include/types.h`.
- `"project.h"` vs `<system.h>`.
- `asm` is a CodeWarrior keyword: raw ARM body, no prologue/epilogue.

### `IDE_PARSE`: hiding `asm` bodies from IntelliSense

An `asm void f() { ... }` body is raw ARM, not parseable C++, so editors flag every line of it. Wrap the definition in a guard:

```c
#ifndef IDE_PARSE
asm void func_02011320(register void * src, register void * dst, register s32 stride) {
    stmdb sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    ...
}
#endif
```

Keep the function's prototype **outside** the guard (in the `extern "C"` block with the other
declarations) so callers still resolve; the editor then sees a normal declared-but-undefined
function and stays quiet. Adjacent asm functions can share one guard.

`IDE_PARSE` is defined **only** in your editor config. For VS Code, add it to `defines` in `.vscode/c_cpp_properties.json`. Also make sure that `defines` matches
`MWC_FLAGS` (`SDK_ARM9 SDK_FINALROM SDK_THREAD_INFINITY SDK_CODE_ARM SDK_TS`) or IntelliSense will walk different `#if` branches through the NitroSDK headers than the compiler does.

## Acknowledgements

[AetiasHax/ds-decomp](https://github.com/AetiasHax/ds-decomp) ·
[encounter/objdiff](https://github.com/encounter/objdiff) ·
[ntrtwl](https://github.com/ntrtwl) NitroSDK/NitroSystem ·
[mid-kid/mw-libraries](https://github.com/mid-kid/mw-libraries) ·

## License

The decomp source here is the original work of contributors, for educational and preservation
purposes. The original game, its code, and its assets belong to their respective rights holders.
