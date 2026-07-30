# Runbook

Task-oriented reference. See [README](../README.md) for setup.

## Make targets

| Target | Does |
|---|---|
| `make` | compile all TUs + SDK diff units, regenerate `objdiff.json` |
| `make tools` | build the pinned `dsd` (no-op once current) |
| `make diff` | build → sync names → delink → open objdiff GUI |
| `make progress-py` | function **and** data-object match report → `main_7_todo.md` |
| `make progress` | same via objdiff-cli, functions only, no data status |
| `make link` | relink every module from delinked objects, byte-compare vs ROM |
| `make sdk` | compile every NitroSDK/NitroSystem `.c` into `build/` |
| `make sdk-check FUNC=<name>` | authoritative SDK-identity check for one function |
| `make sync-names` | harvest mangled names from compiled `.o` into `symbols.txt` |
| `make check-relocs` | find branches missing a relocation |
| `make clean` / `distclean` | drop compiled output / drop everything |

## The matching loop

```bash
python tools/scripts/fn_diff.py <symbol>     # exit 0 match, 1 diff, 2 error
```

Normalized instruction diff of the compiled `.o` against the delinked ROM target.

To rebuild one TU: `touch <src> && make build/arm9/src/main/<tu>.o`.

Edit, rebuild, re-diff. If you'd rather try variants outside your working file, copy the TU
somewhere under `build/` and compile it with the command line from `make -n`.

## Adding a TU

1. Find the range in `asm/_dsd_gap@main_7.s`.
2. `python tools/scripts/asm_smell.py <symbol>` on **every** function first. Hand-written asm
   gets transcribed as `asm void`, not fought. Any one hard tell ⇒ hand-asm.
3. Create `arm9/src/main/unk_<addr>.c` + header. First line:
   `// objdiff-target: build/delinks/<target>.o`
4. Write bodies, grind with `fn_diff.py` or `make diff`.
5. Once everything matches, name the TU in one pass (see Naming).

Before hand-decompiling, check whether it is inlined SDK code. Run the SDK pipeline as a first pass.

## Naming and anchors

Convention is to keep `func_<addr>` until the whole TU matches, then name everything in one pass.
A name assigned early tends to anchor how you read the rest of the TU, so only commit to a name when you're fairly certain it's right.

Once a function has a semantic name, its **declaration** needs an address anchor:

```c
void readChunkSize();   // 0x0201b0e4
```

`sync_renames.py` (inside `make diff`/`make progress`) reads the compiled `.o` for mwcc's real
mangled names and rewrites `config/arm9/symbols.txt`. Never hand-mangle names.

- Function and Data symbol anchors are matched at **global scope only**.
- `static` data does **not** sync (needs `g` binding)
  - This is the only case for a manual `symbols.txt` edit.
- Missing anchor ⇒ `UNPLACED: <name>` from `make diff`. Contradictory ⇒ `CONFLICT`.

## SDK mapping

Much of `main_7` is library code. Try to match it rather than decompile it if possible.

```bash
make sdk                          # compile the SDK surface
make sdk-sweep                    # candidates (tu_diff)
make sdk-check FUNC=func_02016254 # verify ONE candidate
```

**Tool hierarchy:**

| Tool | Authority |
|---|---|
| `tu_diff.py`, `fuzzy_sdk_match.py` | **discovery only**,  relocs masked, false-positives over forked code. Never bulk-map. |
| `sdk_equiv.py` (`make sdk-check`) | **authoritative** for individual identity. Map only if `[EQUIVALENT]`. |
| `objdiff` / `obj_match.py` | ground-truth byte match vs ROM. Final arbiter. |

`DIVERGES` is either a real fork or a cascade from an unmapped callee. Resolve leaf-first and
read `-v` to tell which.

Then: add to `mappings` in `config/sdk_mappings.json`, **also add the source to
`objdiff_units`** (mapped ≠ matched), run `apply_sdk_mappings.py`, `make delinks`, `make progress`.

## Relinking

Relinking verifies data layout, symbol sizes, and TU membership, the things
`obj_match` cannot score.

```bash
make link            # LINK_ARGS='--strict' to fail on any difference
```

Current result: **134 modules byte-exact** (132 overlays + ITCM + DTCM), main 9 bytes off,
35 bytes total. All of them are one class: `0xeb`(bl) vs `0xfa`(blx), ARM/Thumb interworking
lost in the delinked object.

`link_rom.py` carves data into `delinks.txt`, links, compares, then **always restores
`delinks.txt`**. A carve moves target functions out of the gap where `obj_match` can't find
them, which makes `make progress` report `0/0`. Use `--keep-carve` only if you want to
inspect the carved objects, and expect progress to be wrong until you re-run `make delinks`.

Notes:
- mwldarm resolves the MEMORY block's output paths **relative to the `-o` ELF's directory**,
  hence `build/link/build/`.
- Duplicate symbols across split gap objects are fixed with a trailing ` local` flag in
  `symbols.txt`, not by renaming. (Two different `static BeginSleep` genuinely exist in
  NitroSystem snd.)

## Data object progress

`main_7_todo.md` carries a **Data objects** table beside the function table: every named data
symbol in the gap, with `[x]` where a TU homes it and the bytes match.

Size shown is the **extent** (distance to the next symbol), data symbols carry no size in
`symbols.txt`, so extent is not `sizeof`. The status column separates the cases:

| status | meaning |
|---|---|
| (blank note) | bytes and extent both agree |
| `extent is alignment padding` | extent > `sizeof`, ROM tail is zero: benign |
| `interior symbol(s)` | a symbol lands inside the object: fake/interior ref, or over-sized declaration |
| `ROM tail is non-zero` | undeclared trailing content, not padding |
| `content differs` | real byte mismatch |

The interior check scans the whole object, not just the next symbol, because an interior
reference can sit anywhere. `[x]` means owned and matched, `[~]` means bytes verified but
not owned: mwcc emits C++ RTTI (`_ZTS`/`_ZTI`) as COMDAT (ELF binding 13) in every TU that
touches the class, so defining it proves nothing about which TU owns the address. Vtables
(`_ZTV`) are `GLOBAL` and unique, so they get a real `[x]`.

Two ways a symbol ends up misplaced rather than mismatched, both config-only fixes:

| symptom | cause | fix |
|---|---|---|
| symbol inside a larger object | dsd names every referenced address, so an interior pool word or a tail-merged string literal becomes its own symbol | `merge_data_syms.py CONTAINER FAKE...` |
| whole object reads as `content differs` | every reference is interior, so dsd placed the symbol N bytes late (a vtable is always referenced as `&vtable[2]`) | `rebase_data_sym.py NAME REAL_START` |

## Bumping dsd

Edit `rev` in `tools/dsd.pin`, run `make tools`, then re-run `make progress-py` and confirm the
match count holds, a new revision can change `dsd dis` output.

Verification is by `tools/.dsd.stamp`, **never `dsd --version`**: `main` reports `0.11.0`, the
same string as the broken release.
