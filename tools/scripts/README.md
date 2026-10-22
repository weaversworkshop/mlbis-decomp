# tools/scripts

Run from the repo root (`python tools/scripts/<x>.py`). Most are driven by `make` targets; the target is
listed where one exists.

`common.py` holds what every script needs: repo paths (`ROOT`, `BUILD`, `CONFIG`, `SYMBOLS`, `SRC`,
`INCLUDE`, `SDK_MAPPINGS`, `GAP_O`, `GAP_S`), binutils names (`OBJDUMP`/`NM`/`READELF`, env-overridable),
`die`, `load_json` (`-` = stdin), `sdk_mappings()`, `game_sources()`, `objdiff_target()`, and
`read_symbols()` (parses any `symbols.txt` into `Symbol(name, kind, attrs, addr, size, tail)`).

| Workflow | Scripts |
|---|---|
| Build glue | `fix_deps`, `gen_objdiff`, `setup_tools` |
| Delink / link | `gen_delinks`, `check_relocs`, `link_rom`, `patch_lcf` |
| Scoring / progress | `obj_match`, `build_todo`, `call_graph`, `data_catalog` |
| Naming | `sync_renames`, `apply_sdk_mappings`, `struct_offsets`, `data_syms` |
| Per-function matching | `fn_diff`, `grind`, `rank_diff`, `asm_smell`, `../permuter/extract_target_asm` |
| SDK identification | `tu_diff`, `sdk_equiv`, `sdk_match`, `map_data_syms`, `is_interleaved`, `sdk_compile_audit` |
| TU scouting | `tu_cluster`, `is_interleaved` |

---

## Build glue

### fix_deps.py
Rewrites mwcc `.d` files to POSIX paths (`C:\x\y.h` → `/c/x/y.h`) so make can include them. Idempotent;
missing file is silent. Called by the compile rule (`FIXDEP`).
`fix_deps.py FILE.d...`

### gen_objdiff.py
Prints `objdiff.json`. Game units come from each `arm9/src` file's `// objdiff-target:` comment; SDK units
from `objdiff_units` in `config/sdk_mappings.json`. A game file without the comment is warned about and
**never diffed**. Driven by the `objdiff.json` make target (gated on a clean compile).

### setup_tools.py
Builds dsd via `cargo install` at the commit in `tools/dsd.pin` (Rust ≥ 1.85). Currency is decided by
`tools/.dsd.stamp`, never `--version` (always prints `0.11.0`). `make tools`.
`setup_tools.py [--check] [--force]` — `--check` never builds; exit 1 if stale.

---

## Delink / link

### gen_delinks.py
Rewrites each module's `delinks.txt` file entries from the `// objdiff-target:` scan; ranges from each TU's
compiled `.o` symbols resolved through `symbols.txt`. The section table header is kept verbatim.
`make delink-config` (`DELINK_ARGS`).
`gen_delinks.py [--dry-run] [-v] [--sections .text,.bss] [--with-data] [--strict]`
- Not part of `diff`/`progress`: a carve moves targets out of the gap and `obj_match` then reports 0/0.
- Writes only on change (mtime drives the delink stamp); entries for unbuilt objects are preserved.
- COMDAT symbols (bind 13) don't own ranges. Warns on overruns and gaps larger than alignment padding.
- Warns when an object's section lands in a module section of another kind (`.rodata` range inside the ROM's
  `.data`): the original wasn't `const`. dsd allows one range per section name per file, so fix the source.
- Exit 1 on UNPLACED only with `--strict`; 2 on bad args.

### check_relocs.py
Delink-faithfulness check: `bl`/`blx` to another symbol with no relocation in `build/delinks/*.o`.
`make check-relocs` (after `make delinks`). Exit 1 on any hit.
- Fix with `from:0x<addr> kind:arm_call to:0x<target> module:<mod>` in that module's `relocs.txt`.
- Plain `b`/`b<cond>`, self-calls and cross-module calls (`sym+0xfff…`) are skipped by design.

### link_rom.py
Relinks and byte-compares with the ROM: `gen_delinks --with-data` → `dsd delink` → `dsd lcf` → `patch_lcf`
→ `mwldarm`, then compares `build/link/build/*.bin` against `extract/`. `make link`.
`link_rom.py [-v] [--keep-carve] [--strict]`
- Always restores `delinks.txt` and removes `build/delinks/.stamp` on exit; `--keep-carve` leaves the carve
  (then `make progress` reports 0/0).
- Mismatches are reported, not fatal, unless `--strict`.

### patch_lcf.py
Injects `SDK_MAIN_ARENA_LO`, `SDK_AUTOLOAD_*`, `SDK_IRQ_STACKSIZE`, `SDK_SYS_STACKSIZE` at the top of
`SECTIONS`; `dsd lcf` omits them and mwldarm would silently resolve them to 0. Idempotent. `link_rom`
imports `patch()`.
`patch_lcf.py build/arm9.lcf`

---

## Scoring / progress

### obj_match.py — the final arbiter
Reloc-aware byte match of every `objdiff.json` unit vs the delinked target: words compared with relocated
bits masked, each reloc compared as (kind, symbol, addend), section-relative relocs and reloc-less branches
resolved to symbols first. **100.00% = identical after linking**; any real diff caps at 99.99%. Also scores
homed data objects.
`obj_match.py [--config objdiff.json] [--out -|FILE] [--unit main/X ...] [--text] [-v]`
- `make progress` (piped into `build_todo`), `make obj-match [UNIT=main/X] [OBJ_MATCH_ARGS=-v]`.
- Output: objdiff-report-shaped JSON plus `objects` (data status) and per-function `section`. Exit 0; 2 on
  setup error.
- `-v`: skipped units, compiled functions absent from the target, and `NAME` lines (bytes match but source
  and `symbols.txt` disagree on a cross-TU name → rename candidate).
- Data status: `ok`; `pad` (zero tail that fits the next object's alignment); `zeros` (zero tail too long —
  undeclared content); `short` (non-zero tail); `over` (target symbols inside the object — re-anchor with
  `add:`); `diff`. COMDAT objects (bind 13, RTTI) are byte-checked but never owned.
- Duplicate target names: a pair already matched by another unit is never reused; if two units compile the
  same static, the loser's row is dropped.

### build_todo.py
Writes `<target>_todo.md`: per gap function address, lines, distinct `data_*` refs, unit, match %, game/SDK
kind, plus a data-objects section from obj_match `objects`.
`obj_match.py | build_todo.py main_7 --report - [--sort match|address|globals|lines] [--desc] [--data-notes] [--out-md F] [--asm F]`
- `make progress` (`TODO_ARGS`; `DATA_NOTES=1` → `--data-notes`), `make progress-objdiff` (no data section).
- Default sort is match descending; other keys ascending; `--desc` flips. Ties by address.
- "In progress" counts game functions only; SDK membership from `config/sdk_mappings.json`.
- `asm_smell --sweep` reads its "matched" set from this file — fresh only after `make progress`.

### call_graph.py
Mermaid call graph `<target>_callgraph.mmd`. Default: touched functions (have a match %) plus one frontier
layer of untouched callees (what to tackle next). Colour = match % bucket; shape = game / SDK / leaf /
callee frontier (parallelogram) / caller frontier (reverse parallelogram).
`make callgraph [CALLGRAPH_ARGS=...]`, or `obj_match.py | call_graph.py main_7 --report - -o out.mmd [flags]`
- `--all` everything expanded; `--touched-only` no frontier; `--callers` add untouched callers;
  `--root F [--depth N]` subtree (exit 1 if F absent; with `--callers` adds F's direct callers);
  `--undone`, `--leaves`, `--game-only`, `--connected-only`; `--max-nodes` (600) warns.

### data_catalog.py
Catalog of every named gap data symbol per section: address, extent (distance to next symbol), reloc ref
count across all modules; flags 0-ref and "small, few refs" symbols (likely fake delinker aliases).
`data_catalog.py [--gap asm/_dsd_gap@main_7.s] [-o main_7_data.md] [--query NAME|0xADDR]`
- Main module only; `-o` is repo-root relative. `build_todo` imports its parsers.

---

## Naming

### sync_renames.py
Copies mwcc's real mangled names from compiled game `.o` files into each module's `symbols.txt` so objdiff
pairs them. Runs inside `make diff`/`make progress`; standalone `make sync-names` (`SYNC_ARGS`).
`sync_renames.py [--dry-run] [--strict] [-v]`
- Address comes from a `func_<addr>`/`data_<addr>`/`func_ovNNN_<addr>` name, or an anchor on the
  **declaration** (headers and sources under `arm9/include`, `arm9/src`): `void foo();  // 0x0201b0e4`.
- Ctors/dtors tag each ABI variant: `clFS(...);  // C2 0x0201a9e0`, `~clFS();  // D0 0x.. D1 0x.. D2 0x..`.
- Data: anchor a global-scope definition/extern: `TPData bufs[2];  // 0x0205bac4`; a multi-line
  initializer may carry it on its closing `};` line instead. Vtables/RTTI are manual
  (`map-cpp-tu`).
- Allocation operators anchor like functions; keyed by (class, name, arity, class-typed param names), never
  by the name-only fallback.
- Join key is (enclosing class, name) — overridden virtuals anchor per class. Name-only fallback applies only
  to free functions whose anchors agree.
- Module: the file's `// objdiff-target: …_dsd_gap@<mod>_N.o` picks whose `symbols.txt` is edited; misses fall
  back to main/itcm/dtcm. Overlays are never guessed — name them: `// ov122 0x0206d9b4`.
- `// +0xNN` is a struct offset (see `struct_offsets`), never an anchor.
- Reports: `UNPLACED` (semantic name, no address; or anchor nobody owns), `CONFLICT` (same key, different
  addresses), `ambiguous` (one address, two names), `duplicate anchor`, `NOANCHOR` (named in
  `symbols.txt` but its declaration lacks the anchor; prints the address to add). `--strict` exits 1 on
  UNPLACED/CONFLICT.

### apply_sdk_mappings.py
Renames `func_XXXX` → SDK name from `config/sdk_mappings.json` across every `config/**/symbols.txt` and
sources in `arm9/src`, `arm9/include`, `lib` (NitroSDK/mw-libraries excluded). Idempotent; prerequisite of
`make delinks`. Exit 1 on conflicting mappings.
`apply_sdk_mappings.py [--dry-run] [-v]`

### struct_offsets.py — `// +0xNN`
Checks/annotates trailing `// +0xNN` on named members in `arm9/include/main` and `arm9/src/main`.
`make offsets` (check, exit 1 if missing/stale), `make offsets-apply` (inside `diff`/`progress`); `OFFSET_ARGS`.
`struct_offsets.py [paths...] [--apply] [--all] [--no-cache] [--no-bitfields] [--keep-probes] [-v]`
- Offsets come from mwcc: a probe TU takes `&((T*)0)->m` with game flags; falls back to appending to a copy
  of an including TU, then per-member probes.
- `+0x` is reserved: `sync_renames` ignores an address preceded by `+`. Lines already carrying a `// 0xADDR`
  anchor are skipped.
- Bitfields: storage unit from DWARF (`-sym on`), cross-checked against the probe; disagreement ⇒ skip. Only
  the first line of a run is annotated.
- `field*`/`pad_*`/`unk_*` are skipped unless `--all` (currently crashes on `arm9/src/main/data/clMesWin.h`).
- Cache `build/offsets/cache.json`, keyed on content of header + local includes + fallback TU + this script +
  compiler + flags. Probes are deleted unless `-v`/`--keep-probes` or a probe failed.

### data_syms.py
Fixes dsd's data-symbol splits. **Config-only and dry-run unless `--apply`.**
- `merge BASE ELEM... [--elem-size N] [--config-dir D]`: drop ELEM symbols, re-point relocs `to:<elem>` →
  `to:<base> add:<off>`. Refuses if ELEMs aren't after BASE, cross modules, or leave an unmerged symbol between.
- `merge-source BASE ELEM... --elem-size N [--addr-from F] [--src-root D] [--globals H]`: source side — size
  `BASE[N]` in `globals.h`, drop element externs, rewrite `&elem` → `&BASE[i]`. Regex-based; review the diff.
  Use `--addr-from` with the pre-merge `symbols.txt` if `merge --apply` already ran.
- `rebase NAME NEW_ADDR`: move a symbol down to its object start, adding the delta to each reloc's `add:`
  (e.g. vtable referenced only at `+8` ⇒ dsd placed it 8 late). Then `rm -f build/delinks/.stamp && make delinks`.

---

## Per-function matching

### fn_diff.py
Normalized instruction diff of one function: compiled `.o` vs delinked target.
`fn_diff.py <symbol> [--mine X.o] [--target X.o] [--target-symbol NAME] [--pool] [--context N] [--frame]`
- Exit 0 match / 1 diff / 2 error. Empty extractions are refused, so a match is never vacuous.
- Without `--mine` searches `build/arm9/src/**/*.o`, then `build/scratch/**/*.o`.
- Branch targets become instruction deltas (`b <T+12>`), so a wrong-block jump is a diff; `bl`/`blx`/`bx`
  stay `<T>` (relocs).
- `--target-symbol` for a mangled name vs a still-`func_<addr>` target. `--pool` includes literal pools.
- `--frame` prints the spill-slot table by first use: slot order is reverse first-assignment order; adjacent
  same-kind slots = one shared variable.

### grind.py
Batch-scores source variants of one function: each is spliced into a copy of the live TU under
`build/scratch/grind/` and compiled with the TU's real `make -n` line. Never writes game source.
`grind.py <symbol> [--tu P] [--func NAME] [--variants vars.py | --bodies GLOB... | --perm-decls "a;b;c"] [--show NAME|base] [--write NAME OUT] [--keep NAME] [--jobs N] [--top N] [--target-symbol S]`
- No variants = identity check of the live function. An undecompiled function is inserted after its
  `// arm_func_end` comment.
- `--variants` file is exec'd with `base`, `sub(text, old, new)` (asserts `old` exists),
  `perm_decls(text, "a;b;c")`, `re`, `itertools`, `ROOT`; must define `V = {name: function text}`.
- `--perm-decls`: the declarations must be adjacent lines with nothing after `;`.
- Columns: `raw` differing lines; `norm` same with registers/stack slots masked (`raw>0, norm==0` ⇒ pure
  rotation); `insn`; `R<i>=raw/norm` per region (target chunk ending at a backward branch, i.e. a loop);
  register swaps.
- Exit 0 if the best variant matches, 1 if not, 2 on a `--show` compile error. `--keep` leaves the spliced
  TU/`.o` (findable by `fn_diff`).

### rank_diff.py
Register-coloring rank diff for rotation residues: rebuilds webs/interference from the final asm and infers
mwcc's color-pop order (`notes/rank_diff.md`).
`rank_diff.py <symbol> [--mine X.o] [--target X.o] [-v] [--brief]`, `rank_diff.py --validate [--limit N]`
- "RANK FLIPS" = pairs the target forces A-before-B but yours forces B-before-A; fix those, the rest cascades.
- `-v` adds target-only forced edges; `--brief` prints `<flips> <webs differing>`; `--validate` re-checks the
  model on matched functions. `objective()`/`flips()` are used by the patched permuter scorer.

### asm_smell.py
Hand-asm vs compiled-C classifier — run before any decomp.
`asm_smell.py <symbol>... [--target X.o]`, `asm_smell.py --sweep` (run by `make progress`/`diff`)
- Exit 0 C-like / 1 ASM / 2 error; `--sweep` always 0.
- Hard tells (any ⇒ asm): H1 `lr` popped then `bx lr` (except `push {r0-r3}` + `add sp` before `bx` = C with
  an address-taken reg param); H2 push after the prologue; H3 conditional `ldm`/`stm`; H4 conditional insn
  before any flag-setter; H5 `swp`/`mrs`/`msr`/`mcr`/`mrc`; H6 `add pc,pc` into non-`b` rows.
- C tells reported: `pop_pc`, `pad_push` (`push {r3,…}`), `switch_table`.
- In SDK/vendor regions a hard tell may be vendor-built C with an older frame (FS_OpenFileFast) → SDK pipeline.
- `--sweep` covers compiled-but-unmatched functions, skips `asm`-defined ones and the quarantined graphics
  lib, and warns when `main_7_todo.md` is older than the newest `.o`. Duplicate names: first instance only.

### ../permuter/extract_target_asm.py
Writes one function from a delinked `.o` as assembleable `.s` (`glabel`, `.L` labels, pool `.word`s via
reloc symbols) for decomp-permuter's `import.py`.
`extract_target_asm.py <delink.o> <func> [-o out.s]`

---

## SDK identification

### tu_diff.py — discovery only
Matches a gap `.o` against compiled SDK `.o` by normalized skeleton (branch/call targets + relocs masked;
ISA and pool constants kept). **Candidates only — false positives on forked game code; never bulk-map.**
Verify each with `sdk_equiv`. Needs `make sdk`.
- `tu_diff.py --all [--new] [--low] [--min N] [--gap-o build/delinks/_dsd_gap@<mod>.o]` (`make sdk-sweep`);
  `tu_diff.py SDK.o [TARGET.o]` for one object.
- `--new` unmapped `func_*` only; `--low` adds short / multi-candidate hits; `--min` high-confidence insn
  count (8).
- Tags: `NEW!`/`new?` unmapped candidate, `MAPPED`, `named`, `POOL!?` (same insns, different literal ⇒ not
  the same function).
- `--fuzzy [--sim 0.85] [--min 12] [--lentol 0.20] [--show func_X]`: near-matches with compiler drift that
  exact matching misses (excludes exact matches); `--show` prints one skeleton diff.

### sdk_equiv.py — authoritative identity
Compares skeleton, literal pool and reloc symbols, recursing into every differing callee. `EQUIVALENT` only
if the whole call graph is stock SDK — **map only `EQUIVALENT`**. `DIVERGES` = real fork or a cascade from an
unmapped callee: read `-v`, resolve leaf-first.
`make sdk-check FUNC=func_x [SDK_EQUIV_ARGS=...]`, or `sdk_equiv.py FUNC... [--gap-o O] [--sdk-func NAME] [--depth 8] [--min 6] [-v]`
- Skeletons shorter than `--min` are skipped as ambiguous; `--sdk-func` forces a comparison. Exit 0.

### sdk_match.py
Proposes SDK mappings by exact normalized body + byte size against unmapped `func_*` in main; only
unambiguous, collision-free proposals.
- `sdk_match.py [--min-insns 3] [--json] [--apply]` — `--apply` writes mappings + objdiff units to
  `config/sdk_mappings.json`; re-run with `make delinks` until 0 proposals.
- `sdk_match.py --disambiguate [--apply]` — byte-identical siblings (GX banks, `OS_Lock*`, `SNDi_*Mutex`,
  `CARD_*`) told apart by their one distinguishing pool reloc addend. Run after the main pass.

### map_data_syms.py
Aligns literal pools of matched SDK functions against the gap to name `data_xxxx` → SDK globals. Proposes
only 1:1 renames with addend 0; conflicts / ambiguous statics / base+addend splits are listed and skipped.
`map_data_syms.py [--apply]` — `--apply` writes to `sdk_mappings.json`.

### is_interleaved.py
Does a function belong inside an SDK TU? Interleaved = its relocs hit a static function private to a compiled
SDK unit (duplicate statics told apart by size; static data not detected). Points at the SDK source to fork.
`is_interleaved.py func_X [--json]` (exit 1 if interleaved), `is_interleaved.py --all [--json]`.
Needs `make sdk` + `objdiff.json`; main only.

### sdk_compile_audit.py
Which SDK/MSL/syscall `.c` files fail to compile (sp1p2, incl. `SDK_SKIP`) — those have no `.o`, so
`tu_diff` can't match them. `make sdk-audit`. ~2 min.
`sdk_compile_audit.py [--reasons] [--list-fail] [--stub H] [--inc DIR]`

---

## TU scouting

### tu_cluster.py
Candidate TUs for one module by narrowly-shared data (file-local statics). Data with more than
`--max-shared` referrers or spread wider than `--max-span` counts as global. Modules never mix.
`tu_cluster.py [--module main|ovNNN] [--calls] [--min-shared 2] [--max-shared 6] [--max-span 0x4000] [--min-cluster 2] [--only-open] [--report R]`
- `--report` (default `build/report.json`, usually absent — pass an `obj_match.py --out` file) adds `[x]`/`[~]`.
- Flags matched members living in separate files and SDK affiliation (via `is_interleaved`).
