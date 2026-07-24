# mlbis

Decompilation of **Mario & Luigi: Bowser's Inside Story** (Nintendo DS, 2009) back to byte-matching C source.

The goal is C source that compiles, byte-for-byte, to the same machine code as the original retail ROM.

## Status

Active. The `main_7` ARM9 module is the current focus: **~950 of ~1,850 functions** are identified and matching. SDK library code is essentially complete (**687/693**, recovered with the automated SDK-mapping pipeline below); game-code decomp is the active front (~23%). One region (a prebuilt third-party graphics library built with an unreleased compiler) is excluded from decomp. Overlays are delinked but largely untouched.

Run `make progress` for the live per-function breakdown (written to `main_7_todo.md`).

## Requirements

**Git submodules** (run `git submodule update --init --recursive` — these are the SDK sources we compile and diff against):

| Submodule | Source |
|---|---|
| `lib/NitroSDK` | [ntrtwl/NitroSDK](https://github.com/ntrtwl/NitroSDK) |
| `lib/NitroSystem` | [ntrtwl/NitroSystem](https://github.com/ntrtwl/NitroSystem) |
| `lib/mw-libraries` | [mid-kid/mw-libraries](https://github.com/mid-kid/mw-libraries) (MSL C) |

**You must provide**:

| Item | Where it goes | Notes |
|---|---|---|
| Original ROM | `rom/M_and_L_BIS.nds` | You must legally own it. |
| `mwccarm` toolchain | `tools/mwccarm/` | CodeWarrior for DS: `2.0/sp1p2` (SDK libs) **and** `2.0/sp2p3` (game TUs) — the Makefile selects per-file. |
| `mwccarm` license | `tools/mwccarm/license.dat` | FLEXlm license. |
| `dsd` | `tools/dsd.exe` | [AdrianCX/dsd](https://github.com/AdrianCX/dsd) — NDS delinker. |
| `objdiff` GUI | `tools/objdiff.exe` | [encounter/objdiff](https://github.com/encounter/objdiff) — interactive diff. |
| `objdiff-cli` | `tools/objdiff-cli.exe` | Same project; used by `make progress`/scripts. |

**System-wide:**
- **GNU Make 4.x** and a **bash** (MSYS2 or Git Bash) — Makefile recipes use bash.
- **Python 3** — all build helpers in `tools/scripts/` are Python.
- **MSYS2 binutils** — `arm-none-eabi-objdump` (used by the mapping/audit scripts).
- **Windows** is the only tested platform.

## First-time setup

```powershell
git clone <repo-url> mlbis
cd mlbis
git submodule update --init --recursive   # NitroSDK / NitroSystem / mw-libraries

# Drop the provided items from the table above into their paths (.gitignore keeps them out of git).

.\tools\dsd.exe rom extract --rom .\rom\M_and_L_BIS.nds --output-path .\extract
make diff
```

`objdiff` opens with a unit list on the left; clicking a unit shows a side-by-side disassembly diff against the ROM. Green = byte-identical.

## Daily workflow

```powershell
make              # compile all .c (+ SDK diff units) + regenerate objdiff.json
make diff         # build + delink + open the objdiff GUI
make progress     # per-function match report (Game vs SDK), via objdiff-cli
make sdk          # compile every NitroSDK/NitroSystem .c into build/ (for diffing)
make check-relocs # audit delinks for branches missing a relocation (bl <invalid>)
make callgraph    # emit a Mermaid call graph
make clean        # remove compiled output (keeps dsd delinks)
make distclean    # remove everything; next build re-runs dsd
```

Fast inner loop for a single function: `python tools/scripts/fn_diff.py <symbol>` — normalized instruction diff of the compiled `.o` against the delinked ROM target.

## SDK mapping pipeline

Most of `main_7` is NitroSDK/NitroSystem library code. Rather than hand-decomp it, we identify it and diff the SDK source directly:

- **`config/sdk_mappings.json`** — the source of truth: each entry maps a ROM address to its SDK symbol name (functions and data globals).
- **`tools/scripts/apply_sdk_mappings.py`** — pushes those names into `config/arm9/symbols.txt` and propagates renames through sources.
- **`tools/scripts/tu_diff.py`** — content-matches compiled SDK objects against the delinked ROM to *discover* SDK functions the name-based sweep misses (`--all --new`).
- **`tools/scripts/map_data_syms.py`** — names data globals by aligning matched functions' literal pools.
- **Build defines**: the SDK is compiled with `-DSDK_FINALROM -DSDK_THREAD_INFINITY -DSDK_CODE_ARM` (in `MWC_FLAGS`) to match how the retail ROM was built. Missing one of these is a common cause of a near-but-not-100% match.

## Adding a new function

1. Find the target in `asm/` (e.g. `asm/_dsd_gap@main_7.s`).
2. Create `arm9/src/<module>/unk_<addr>.c` (one translation unit per address, pret-style) with a matching header in `arm9/include/`.
3. First line: `// objdiff-target: build/delinks/<target>.o`.
4. Write the function (for BIOS wrappers use `asm void { svc N; bx lr }`; otherwise idiomatic C).
5. `make diff` and iterate until green.

Some TUs are real C++ classes (RTTI/vtables in ROM): use `.cpp` — the Makefile compiles it with `-lang=c++` automatically (RTTI TUs get a per-file `-RTTI on`). When you rename a function from `func_<addr>` to a semantic name, put a `// 0x<addr>` anchor comment on its declaration — `sync_mangled_names.py` (runs inside `make diff`/`make progress`) uses it to keep objdiff paired with the mangled symbol.

If a stubborn function looks like it accesses a struct via base+offset but is really MMIO or SDK code, check `lib/NitroSDK` / run `tu_diff.py` before hand-decompiling — it may be an inlined or linked SDK routine.

## Project layout

```
mlbis/
├── arm9/
│   ├── include/         project + per-TU headers
│   └── src/             game source (one .c per function/TU)
├── asm/                 dsd disassembly (reference only; gitignored)
├── build/               compiled .o + build/delinks/ (dsd target .o)
├── config/arm9/         dsd config: symbols.txt, relocs.txt, delinks.txt, config.yaml
├── config/sdk_mappings.json   ROM addr -> SDK symbol map
├── main_7_todo.md       generated progress report (make progress)
├── lib/                 NitroSDK, NitroSystem, mw-libraries (submodules)
├── rom/ , extract/      your ROM + dsd extraction (gitignored)
├── tools/
│   ├── mwccarm/ dsd.exe objdiff.exe objdiff-cli.exe   (you provide)
│   └── scripts/         Python build helpers
├── Makefile
└── objdiff.json         generated by gen_objdiff.py
```

## Conventions

- **One TU per `.c`/`.cpp`** (C++ class TUs hold the whole class), pret-style `unk_<addr>` naming + matching header.
- **Type names** follow NitroSDK (`u8/u16/u32/u64`, `s*`); see `arm9/include/types.h`.
- **`asm` keyword** is CodeWarrior-specific: the whole body is raw ARM, no prologue/epilogue.
- `"header.h"` for project headers, `<header.h>` for system/CodeWarrior headers.

## Acknowledgements

- [AdrianCX/dsd](https://github.com/AdrianCX/dsd), [encounter/objdiff](https://github.com/encounter/objdiff)
- [ntrtwl](https://github.com/ntrtwl) NitroSDK/NitroSystem, [mid-kid/mw-libraries](https://github.com/mid-kid/mw-libraries)
- The broader NDS decomp community (pokediamond, pokeplatinum, dqix-decomp, etc.)

## License

The decomp source in this repository is the original work of contributors, for educational and preservation purposes. The original game, its code, and its assets belong to their respective rights holders.
