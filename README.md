# mlbis

Decompilation of **Mario & Luigi: Bowser's Inside Story** (Nintendo DS, 2009) back to byte-matching C/C++ source.

This is a decomp project: the goal is to produce C source that compiles, byte-for-byte, to the same machine code as the original retail ROM.

## Status

Very early. Build pipeline is wired up end-to-end and the first function (`CpuFastSet`, a BIOS wrapper) matches. ~9,400 functions to go.

## Requirements

You'll need to provide several things that **aren't** in the repo (because of license, size, or copyright):

| Item | Where it goes | Notes |
|---|---|---|
| Original ROM | `rom/M_and_L_BIS.nds` | You must legally own it. Not distributed here. |
| `mwccarm` toolchain | `tools/mwccarm/` | CodeWarrior for ARM. |
| `mwccarm` license file | `tools/mwccarm/license.dat` | FLEXlm license. Ships alongside `mwccarm` in most decomp bundles. |
| `dsd` | `tools/dsd.exe` | [github.com/AdrianCX/dsd](https://github.com/AdrianCX/dsd) — NDS delinker. Use the latest Windows release. |
| `objdiff` | `tools/objdiff.exe` | [github.com/encounter/objdiff](https://github.com/encounter/objdiff) — local diff GUI. |

You'll also need (system-wide):

- **GNU Make 4.x** — via MSYS2 (`pacman -S make`), Git Bash, or any other source. The Makefile uses `bash` for recipes.
- **Windows** is the only currently-tested platform. The toolchain should work on Linux/macOS via Wine for `mwccarm.exe`, but nothing has been verified.

## First-time setup

```powershell
# 1. Clone (assuming you've already done this)
git clone <repo-url> mlbis
cd mlbis

# 2. Drop the items from the table above into their target paths.
#    The .gitignore keeps them out of git.

# 3. Extract your ROM into the expected layout.
.\tools\dsd.exe rom extract --rom .\rom\M_and_L_BIS.nds --output-path .\extract

# 4. Build everything and launch the diff GUI.
make diff
```

After step 4, `objdiff` should open. The unit list on the left shows each `.c` file that has a target mapping; clicking one shows a side-by-side disassembly diff against the original ROM. Green = byte-identical match.

## Project layout

```
mlbis/
├── arm9/
│   ├── include/         project headers (types.h, etc.)
│   └── src/             one .c per function (decomp convention)
├── asm/                 dsd disassembly output (.s files; reference only)
├── build/               compiled .o files (output of `make`)
│   ├── arm9/...         your matched code
│   └── delinks/         dsd's delinked target .o files
├── config/
│   └── arm9/            dsd splits / symbols / relocs config
├── extract/             dsd extraction of the ROM (gitignored)
├── rom/                 your local ROM (gitignored)
├── tools/
│   ├── mwccarm/         compiler (you provide)
│   ├── dsd.exe          delinker (you provide)
│   ├── objdiff.exe      diff GUI (you provide)
│   └── scripts/         build helpers (gen_objdiff.sh, etc.)
├── Makefile
├── objdiff.json         auto-generated from `// objdiff-target:` comments
└── README.md
```

## Daily workflow

```powershell
make           # compile all .c files + regenerate objdiff.json (fast)
make diff      # compile + run dsd if needed + open objdiff GUI
make clean     # remove your compiled output, keeps dsd delinks
make distclean # remove everything; next build re-runs dsd
```

## Adding a new function

1. Find your target in `asm/` (e.g., `asm/_dsd_gap@main_7.s`).
2. Create `arm9/src/<module>/<FunctionName>.c`.
3. First line of the file: `// objdiff-target: build/delinks/<target>.o` — tells the generator which delinked `.o` to diff against.
4. Write the function. For BIOS wrappers, use `asm void`:
   ```c
   asm void FunctionName(...) {
       svc N
       bx lr
   }
   ```
   For real C, write it normally matching conventions in existing files.
5. `make diff`. Iterate until green.

## Conventions

- **One function per `.c` file** — keeps diffs scoped.
- **Type names** follow NitroSDK: `u8`/`u16`/`u32`/`u64` map to `unsigned char`/`short`/`long`/`long long`. `s*` for signed. See `arm9/include/types.h`.
- **`asm` keyword** is CodeWarrior-specific. The whole function body is raw ARM assembly; the compiler emits no prologue/epilogue.
- **`#include "header.h"`** for project headers, `<header.h>` for system / CodeWarrior headers.

## Acknowledgements

- [AdrianCX/dsd](https://github.com/AdrianCX/dsd) — the delinker that makes NDS decomp practical
- [encounter/objdiff](https://github.com/encounter/objdiff) — the diff tool
- The broader NDS decomp community (pokediamond, pokeplatinum, dqix-decomp, etc.) for paving the road

## License

The decomp source code in this repository is the original work of contributors and is provided for educational and preservation purposes. The original game, its code, and its assets are the property of their respective rights holders.
