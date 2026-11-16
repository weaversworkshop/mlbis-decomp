TOOLS_DIR := tools
MWCCARM := $(TOOLS_DIR)/mwccarm/2.0/sp1p2/mwccarm.exe
LICENSE := $(TOOLS_DIR)/mwccarm/license.dat
SCRIPTS := $(TOOLS_DIR)/scripts
OBJDIFF := $(TOOLS_DIR)/objdiff.exe
# objdiff logs every GPU adapter at INFO. Passed via a target-specific `export` on
# `diff` (not inline) so the echoed recipe is just the exe; OBJDIFF_LOG=info restores it.
OBJDIFF_LOG ?= error
# dsd is built from the commit pinned in tools/dsd.pin (`make tools`); a release
# binary is NOT usable.
DSD_EXE := $(if $(filter Windows_NT,$(OS)),dsd.exe,dsd)
DSD := $(TOOLS_DIR)/bin/$(DSD_EXE)
DSD_CONFIG := config/arm9/config.yaml
BUILD_DIR := build
DELINKS_DIR := $(BUILD_DIR)/delinks
INCLUDE_DIR := arm9/include

SDK_INCLUDE     := lib/NitroSDK/include
NNS_INCLUDE     := lib/NitroSystem/include
MSL_INCLUDE     := lib/mw-libraries/ARM_EABI_Support_DS2.0/msl/MSL_C/MSL_Common/Include
MSL_ARM_INCLUDE := lib/mw-libraries/ARM_EABI_Support_DS2.0/msl/MSL_C/MSL_ARM/Include
SHIM_INCLUDE    := lib/nitro/include
FX_CONST_H      := $(SDK_INCLUDE)/nitro/fx/fx_const.h
FX_CONST_GEN    := lib/NitroSDK/gen/nitro/fx/gen_fx_const.py
FX_CONST_CSV    := lib/NitroSDK/gen/nitro/fx/fx_const.csv

export MWCIncludes := $(INCLUDE_DIR)
export LM_LICENSE_FILE := $(abspath $(LICENSE))

# -DSDK_TS: BIS's SDK target (overlay-digest + mmap sizes). See memory/project_sdk_drift.
MWC_FLAGS := -proc arm946e -O4,p -ipa file -interworking -inline on -sym on -enum int -lang=c99 -Cpp_exceptions off -gccext,on -gccinc -msgstyle gcc -nowraplines -char signed -DSDK_ARM9 -DSDK_FINALROM -DSDK_THREAD_INFINITY -DSDK_CODE_ARM -DSDK_TS -DSDK_4M -i $(INCLUDE_DIR) -i $(SHIM_INCLUDE) -i $(SDK_INCLUDE) -i $(NNS_INCLUDE) -i $(MSL_INCLUDE) -i $(MSL_ARM_INCLUDE) -include ansi_prefix.ARM.h

# rwildcard (recursive $(wildcard)) walked ~2.4k lib files on EVERY make invocation
# and cost ~0.5s of the 0.8s parse. One `find` per tree does the same job ~4x faster;
# lists verified identical, entry for entry (2026-08-26).
# NEVER call bare `find`: on Windows PATH it resolves to system32's text-search
# find.exe (empty output, no error), which silently empties these lists.
FIND := $(if $(wildcard /usr/bin/find),/usr/bin/find,find)
rfind = $(shell $(FIND) $(1) -name '$(2)' 2>/dev/null)
# ONE walk of lib/, partitioned with $(filter); four separate finds cost four spawns.
LIB_ALL_C    := $(call rfind,lib,*.c)
LIB_C_FILES  := $(filter-out lib/NitroSDK/% lib/mw-libraries/% lib/NitroSystem/%,$(LIB_ALL_C))
# Whole library surface (NitroSDK + NitroSystem + DS2.0 MSL).
MSL := lib/mw-libraries/ARM_EABI_Support_DS2.0/msl/MSL_C
SDK_C_FILES  := $(filter lib/NitroSDK/libraries/% lib/NitroSystem/% \
                         lib/mw-libraries/ARM_EABI_Support_DS2.0/% lib/syscall/%,$(LIB_ALL_C))

# MSL + Runtime build with the vendor library config, not the game flags.
MSL_ROOT := lib/mw-libraries/ARM_EABI_Support_DS2.0
MSL_GEN  := $(BUILD_DIR)/mslgen
MSL_INC  := -i $(MSL_GEN) -i $(MSL_INCLUDE) -i $(MSL_ARM_INCLUDE) -i $(SDK_INCLUDE) \
            -i $(MSL)/MSL_Common_Embedded/Math/Include \
            -i $(MSL_ROOT)/msl/MSL_Extras/MSL_Common/Include -i $(MSL_ROOT)/msl/MSL_Extras/MSL_ARM/Include
MSL_BASE := -proc arm946e -interworking -sym off -opt level=4,speed -iso_templates on -ipa file \
            -Cpp_exceptions on -gccinc -msgstyle gcc -nowraplines
MSL_FLAGS := $(MSL_BASE) -char signed -include PREFIX_NITRO.h $(MSL_INC)
RT := $(MSL_ROOT)/Runtime
RT_FLAGS = $(MSL_BASE) $(MSL_INC) -i $(MSL_ROOT)/msl/MSL_C++/MSL_Common/Include \
           -i $(MSL_ROOT)/msl/MSL_C++/MSL_ARM/Include -i "$(RT)/(Runtime_Common)" \
           -i "$(RT)/(Runtime_Common)/MWRuntime" -i "$(RT)/(Runtime_Common)/cxxabi" \
           -i "$(RT)/(RISC_Common)" -i "$(RT)/Runtime_ARM/(Common_Source)"
SDK_SKIP := $(foreach f,$(SDK_C_FILES),$(if $(findstring /ARM7/,$f)$(findstring /wl_arm7/,$f),$f))
# Runtime helpers live in dirs with '(' that break recipe shells; compiled via find in `sdk`.
SDK_RUNTIME  := $(filter lib/mw-libraries/ARM_EABI_Support_DS2.0/Runtime/%,$(LIB_ALL_C))
SDK_C_FILES  := $(filter-out $(SDK_SKIP) $(SDK_RUNTIME),$(SDK_C_FILES))

# objdiff units (synced with objdiff_units in sdk_mappings.json).
# sed, not python: saves a ~100ms interpreter spawn on EVERY make invocation.
# Safe because "source" appears only inside objdiff_units (123 keys, 123 units).
SDK_DIFF_FILES := $(shell sed -n 's/.*"source"[[:space:]]*:[[:space:]]*"\([^"]*\)".*/\1/p' config/sdk_mappings.json)

# Prebuilt units (objdiff_units "prebuilt"): CW runtime members with no source.
AR      := arm-none-eabi-ar
MATHLIB := lib/mw-libraries/ARM_EABI_Support/Mathlib/lib_ds_2.0_sp1/FP_fastI_v5t_LE.a
PREBUILT_OBJ := $(addprefix $(BUILD_DIR)/prebuilt/,$(shell sed -n 's/.*"prebuilt"[[:space:]]*:[[:space:]]*"\([^"]*\)".*/\1/p' config/sdk_mappings.json))

SRC_ALL   := $(shell $(FIND) arm9/src \( -name '*.c' -o -name '*.cpp' \) 2>/dev/null)
C_FILES   := $(filter %.c,$(SRC_ALL)) $(LIB_C_FILES) $(SDK_DIFF_FILES)
CXX_FILES := $(filter %.cpp,$(SRC_ALL))
OBJ_FILES := $(C_FILES:%.c=$(BUILD_DIR)/%.o) $(CXX_FILES:%.cpp=$(BUILD_DIR)/%.o)
DGT_ASM_OBJ := $(addprefix $(BUILD_DIR)/lib/NitroSDK/libraries/dgt/asm/,sha1-arm4cw.o sha1s-arm4cw.o)
SDK_OBJ_FILES := $(SDK_C_FILES:%.c=$(BUILD_DIR)/%.o) $(DGT_ASM_OBJ)

DSD_CONFIG_DEPS := $(shell $(FIND) config -mindepth 1 2>/dev/null)
OBJDIFF_CLI := $(TOOLS_DIR)/objdiff-cli.exe

TARGET ?= main_7
TODO_TARGETS ?= $(sort $(TARGET) itcm_1)

SDK_MAPPINGS := config/sdk_mappings.json
SDK_APPLY    := python $(SCRIPTS)/apply_sdk_mappings.py
SYNC_NAMES   := python $(SCRIPTS)/sync_renames.py

.PHONY: tools link delink-config all compile-ok split-tcm clean distclean errors diff objdiff delinks progress progress-py progress-objdiff report obj-match callgraph check-relocs sdk-rename sync-names sdk sdk-propose sdk-audit sdk-patch sdk-patch-update offsets offsets-apply

# A failing compile under -j printed its diagnostics, then make kept echoing the
# in-flight jobs after it. Defer the failure: record it, let the others finish, and
# report from this recipe so the errors are the LAST thing on screen. Direct
# single-object builds are unaffected (DEFER_ERRORS is target-specific to `all`).
all: DEFER_ERRORS := 1
all: compile-ok split-tcm objdiff.json

# notes/itcm.md "Split objects"
SPLIT_TCM := python $(SCRIPTS)/split_tcm.py
split-tcm: objdiff.json | compile-ok
	@$(SPLIT_TCM)
$(BUILD_DIR)/split/%@itcm_1.o: $(BUILD_DIR)/%.o
	@$(SPLIT_TCM) --keep itcm $< $@
$(BUILD_DIR)/split/%@dtcm_1.o: $(BUILD_DIR)/%.o
	@$(SPLIT_TCM) --keep dtcm $< $@
$(BUILD_DIR)/split/%.o: $(BUILD_DIR)/%.o
	@$(SPLIT_TCM) --keep main $< $@

# Gate: every object is a prerequisite, so this runs only once they have all been
# attempted. Deferred failures land in <obj>.o.err -- report them and fail here, which
# stops objdiff.json (an order-only dependent) from running on a broken build. PHONY on
# purpose: a stamp file is skipped on an incremental break, because a failed compile
# writes nothing and the stale object's mtime never moves.
compile-ok: $(FX_CONST_H) $(OBJ_FILES) $(PREBUILT_OBJ)
	@e=`$(FIND) $(BUILD_DIR) -name '*.o.err' -size +0 2>/dev/null`; \
	if [ -n "$$e" ]; then echo; echo "----- !! ERRORS !! -----"; \
	  for f in $$e; do $(SHOWERR) $$f | $(ERRLAST); done; exit 1; fi

# Generated per object by DEPFLAGS. `-include` so a first build (no .d yet) is fine.
-include $(OBJ_FILES:.o=.d)

# Compilation is embarrassingly parallel and the output is byte-identical to a
# serial build (verified 2026-08-26). Pipeline recipes stay sequential: their steps
# are recipe LINES, and sync-names/offsets-apply mutate shared files.
JOBS ?= $(or $(NUMBER_OF_PROCESSORS),$(shell nproc 2>/dev/null),4)
MAKEFLAGS += -j$(JOBS)

$(FX_CONST_H): $(FX_CONST_GEN) $(FX_CONST_CSV)
	@mkdir -p $(@D)
	python $(FX_CONST_GEN) $(FX_CONST_CSV) $@

# docs/RUNBOOK.md "Editing SDK source"
SDK_PATCH         := patches/NitroSDK.patch
SDK_PATCH_STAMP   := $(BUILD_DIR)/.sdk-patch.mk
SDK_PATCH_APPLIED := $(BUILD_DIR)/.sdk-patch.applied
SDK_GIT_APPLY     := git -C lib/NitroSDK apply --whitespace=nowarn

ifneq ($(wildcard $(SDK_PATCH)),)
-include $(SDK_PATCH_STAMP)
compile-ok sdk: | $(SDK_PATCH_STAMP)
endif

sdk-patch: $(SDK_PATCH_STAMP)

$(SDK_PATCH_STAMP): $(SDK_PATCH)
	@mkdir -p $(@D); p=../../$(SDK_PATCH); a=../../$(SDK_PATCH_APPLIED); \
	if $(SDK_GIT_APPLY) --check -R $$p 2>/dev/null; then :; \
	elif [ -f $(SDK_PATCH_APPLIED) ] && $(SDK_GIT_APPLY) --check -R $$a 2>/dev/null; then \
	  $(SDK_GIT_APPLY) -R $$a && $(SDK_GIT_APPLY) $$p && echo "sdk-patch: re-applied updated $(SDK_PATCH)" || exit 1; \
	elif $(SDK_GIT_APPLY) --check $$p 2>/dev/null; then \
	  $(SDK_GIT_APPLY) $$p && echo "sdk-patch: applied $(SDK_PATCH)" || exit 1; \
	else echo "sdk-patch: !! $(SDK_PATCH) does not fit lib/NitroSDK (local edits? run 'make sdk-patch-update' to keep them)"; exit 1; fi; \
	cp $(SDK_PATCH) $(SDK_PATCH_APPLIED) && echo "# sdk-patch stamp" > $@

sdk-patch-update:
	@mkdir -p $(dir $(SDK_PATCH)) $(BUILD_DIR)
	git -C lib/NitroSDK diff --output=../../$(SDK_PATCH)
	@cp $(SDK_PATCH) $(SDK_PATCH_APPLIED) && echo "# sdk-patch stamp" > $(SDK_PATCH_STAMP)
	@echo "sdk-patch: wrote $(SDK_PATCH) ($$(grep -c '^diff --git' $(SDK_PATCH)) files)"

$(BUILD_DIR)/prebuilt/mathlib/%.o: $(MATHLIB)
	@mkdir -p $(@D)
	$(AR) x --output=$(@D) $< $*.o && touch $@

sdk: $(FX_CONST_H)
	@mkdir -p $(BUILD_DIR)
	@echo "Compiling $(words $(SDK_C_FILES)) library .c files (+ MSL Runtime)..."
	@$(MAKE) -k $(SDK_OBJ_FILES) 2> $(BUILD_DIR)/sdk_build.log; \
	  find $(RT) \( -name '*.c' -o -name '*.cpp' \) | while IFS= read -r c; do \
	    o="$(BUILD_DIR)/$${c%.*}.o"; mkdir -p "$$(dirname "$$o")"; \
	    $(MWCCARM) $(RT_FLAGS) -c -o "$$o" "$$c" 2>> $(BUILD_DIR)/sdk_build.log || echo "Error 1: $$c" >> $(BUILD_DIR)/sdk_build.log; \
	  done; \
	  ok=$$($(FIND) $(BUILD_DIR)/lib -name '*.o' 2>/dev/null | wc -l); \
	  fail=$$(grep -c "Error 1" $(BUILD_DIR)/sdk_build.log 2>/dev/null || echo 0); \
	  echo "SDK build: $$ok objects compiled, $$fail failed. Log: $(BUILD_DIR)/sdk_build.log"

clean:
	rm -rf $(BUILD_DIR)/arm9
	rm -f objdiff.json

distclean: clean
	rm -rf $(BUILD_DIR)

sdk-rename:
	$(SDK_APPLY)

# Relink every module from delinked objects and byte-compare vs the ROM.
# Carves data as well, then restores delinks.txt (a carve breaks obj_match).
LINK_DIR   := $(BUILD_DIR)/link
MWLDARM    := $(TOOLS_DIR)/mwccarm/2.0/sp2p3/mwldarm.exe

link: all $(DSD)
	@python $(SCRIPTS)/link_rom.py $(LINK_ARGS)

# Build the pinned dsd if the stamp doesn't match. Cheap no-op once current.
tools:
	@python $(SCRIPTS)/setup_tools.py

$(DSD):
	@python $(SCRIPTS)/setup_tools.py

# delinks.txt file entries are GENERATED from the `// objdiff-target:` scan.
# Needs compiled .o + a synced symbols.txt, so it runs after sync-names and before
# dsd delink. Writes only on change (delinks.txt feeds the delink stamp).
# NOT a prerequisite of diff/progress: carving moves target functions out of the
# gap, where obj_match can't find them (0/0 matched). Link-only, opt-in.
delink-config:
	@python $(SCRIPTS)/gen_delinks.py $(DELINK_ARGS)

delinks: $(DELINKS_DIR)/.stamp
$(DELINKS_DIR)/.stamp: $(DSD) $(DSD_CONFIG_DEPS) $(SDK_MAPPINGS)
	$(SDK_APPLY)
	$(DSD) delink --config-path $(DSD_CONFIG)
	$(DSD) dis --config-path $(DSD_CONFIG) --asm-path asm
	@mkdir -p $(@D)
	@touch $@

objdiff: objdiff.json
objdiff.json: $(C_FILES) $(CXX_FILES) $(SDK_MAPPINGS) $(SCRIPTS)/gen_objdiff.py $(SCRIPTS)/split_tcm.py | compile-ok
	python $(SCRIPTS)/gen_objdiff.py > $@

# sync-names harvests mwcc's mangled names from the compiled game .o into
# symbols.txt (edits it), so delinks must run AFTER via recursive make.
sync-names: all
	$(SYNC_NAMES) $(SYNC_ARGS)

# Diagnostics from the last failed build (each failing object keeps a .err).
errors:
	@$(FIND) $(BUILD_DIR) -name '*.o.err' -size +0 -exec $(SHOWERR) {} \; 2>/dev/null; true

# `// +0xNN` member offsets. `offsets` = check (exit 1 if stale); offsets-apply
# rewrites the headers and runs inside diff/progress, like sync-names. Comment-only
# edits, so a write costs a recompile of the dependents on the NEXT build; idempotent,
# so steady state writes nothing. OFFSET_ARGS passes flags (e.g. --all, --no-bitfields).
offsets:
	python $(SCRIPTS)/struct_offsets.py $(OFFSET_ARGS)

offsets-apply:
	@python $(SCRIPTS)/struct_offsets.py --apply $(OFFSET_ARGS)

diff: export RUST_LOG := $(OBJDIFF_LOG)
diff: all sync-names
	@$(MAKE) --no-print-directory offsets-apply
	@$(MAKE) --no-print-directory delinks
	@python $(SCRIPTS)/asm_smell.py --sweep
	$(OBJDIFF) -p .

# DATA_NOTES=1 expands the unmatched-data count into per-object reasons.
TODO_ARGS = $(if $(DATA_NOTES),--data-notes,)

progress: all sync-names
	@$(MAKE) --no-print-directory offsets-apply
	@$(MAKE) --no-print-directory delinks
	python $(SCRIPTS)/obj_match.py --out $(BUILD_DIR)/obj_match.json
	@$(foreach t,$(TODO_TARGETS),python $(SCRIPTS)/build_todo.py $(t) --report $(BUILD_DIR)/obj_match.json $(TODO_ARGS) &&) true
	@python $(SCRIPTS)/asm_smell.py --sweep

progress-py: progress          # deprecated alias

# objdiff-format report scored by obj_match, for decmp.dev.
REPORT ?= progress/report.json
report: all sync-names
	@$(MAKE) --no-print-directory delinks
	python $(SCRIPTS)/obj_match.py | python $(SCRIPTS)/decmp_report.py --report - -o $(REPORT)

# Cross-check against third-party objdiff. Functions only, and it loses on every
# known disagreement (2026-07-29: 3 false near-misses + 2 unpairable duplicate
# names, all 5 confirmed matched by fn_diff), so this is a second opinion on
# obj_match, not the arbiter. Panics on CODE-flagged TCM data sections.
progress-objdiff: all sync-names
	@$(MAKE) --no-print-directory delinks
	$(OBJDIFF_CLI) report generate -p . -o - | python $(SCRIPTS)/build_todo.py $(TARGET) --report - $(TODO_ARGS)
	@python $(SCRIPTS)/asm_smell.py --sweep

# Per-unit match summary + unresolved callee names (UNIT=name to narrow).
obj-match: delinks
	@python $(SCRIPTS)/obj_match.py --text $(if $(UNIT),--unit $(UNIT),) $(OBJ_MATCH_ARGS)

# Faithfulness audit: cross-boundary branches with no reloc.
check-relocs: delinks
	python $(SCRIPTS)/check_relocs.py

# SDK identity proposals (need `make sdk` first): make sdk-propose [GAP=itcm_1] [SDK_PROPOSE_ARGS="-v --apply"]
GAP ?= main_7
sdk-propose: delinks
	@python $(SCRIPTS)/sdk_propose.py --gap $(GAP) $(SDK_PROPOSE_ARGS)

# sdk-audit: which SDK .c fail to compile (blind spots).
sdk-audit:
	@python $(SCRIPTS)/sdk_compile_audit.py --reasons

# Mermaid call graph (game-only by default). Scope via CALLGRAPH_ARGS, e.g.
#   make callgraph CALLGRAPH_ARGS="--root func_0200907c --undone"
CALLGRAPH_FLAGS ?= --connected-only --game-only
CALLGRAPH_ARGS ?=
callgraph: all delinks
	python $(SCRIPTS)/obj_match.py | python $(SCRIPTS)/call_graph.py $(TARGET) --report - $(CALLGRAPH_FLAGS) $(CALLGRAPH_ARGS)

# Per-file overrides: MWCC_OVERRIDE (compiler), EXTRA_CFLAGS (defines).
$(BUILD_DIR)/lib/NitroSDK/%.o: EXTRA_CFLAGS := -DSP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS -DSDK_PATCH3 -DNULL=0
$(BUILD_DIR)/lib/NitroSDK/libraries/dgt/%.o: EXTRA_CFLAGS += -include nitro.h

# Game TUs = sp2p3 (SDK stays sp1p2). b138 == b139 output project-wide (verified
# 2026-07-24); sp2p3 (2008-06) predates JP ship, sp2p4 (2009-06) does not.
$(BUILD_DIR)/arm9/src/%.o: MWCC_OVERRIDE := $(TOOLS_DIR)/mwccarm/2.0/sp2p3/mwccarm.exe

# clScriptSystem/clGlobalEvent are real C++ classes (RTTI in ROM).
$(BUILD_DIR)/arm9/src/main/clGlobalEvent.o: EXTRA_CFLAGS := -RTTI on

MWCC_OVERRIDE ?=
EXTRA_CFLAGS  ?=

# Header deps: mwcc writes a .d next to the .o, in Windows syntax make can't read
# (backslash separators, bare drive letters), so fix_deps.py rewrites it in place.
# Without this, editing a header rebuilds NOTHING and obj_match scores stale objects.
# -MD, NOT -MMD: mwcc counts every header found via an `-i` search path as a
# "system" header, so -MMD lists ~14 deps and omits arm9/include/main entirely
# (verified 2026-08-26: 14 deps vs 352, no clGameMain.h). SDK headers as extra
# prerequisites are harmless - they only change when the submodule does.
DEPFLAGS := -gccdep -MD
FIXDEP    = @python $(SCRIPTS)/fix_deps.py $(@:.o=.d)

# Echo just the source path (the 600-char command echo buried diagnostics under -j),
# then mwcc's output - it already names file and line, so no prefix. mwcc writes
# `a\\b\\c.c:12:` while make's $< is posix, so SHOWERR normalises the slashes (same
# reason fix_deps.py rewrites its depfiles). A failing object keeps its raw
# diagnostics in <obj>.o.err for `make errors`; V=1 restores the raw command echo.
# Game TUs only - lib/* logs via `make sdk`.
V ?= 0
CC_BIN = $(or $(MWCC_OVERRIDE),$(MWCCARM))
SHOWERR = sed 's|\\|/|g'
# hold back `file:line:` lines and re-emit them at the end, so the last thing on
# screen is the diagnostic and not mwcc's trailer.
ERRLAST = awk '/^[ 	]*$$/{next} /:[0-9]+:/{d[++n]=$$0;next} {print} END{for(i=1;i<=n;i++)print d[i]}'
ifeq ($(V),1)
CC_RUN = $(1)
else
CC_RUN = @echo "$<"; if ! $(1) > $@.err 2>&1; then \
	  test -n "$(DEFER_ERRORS)" && exit 0; $(SHOWERR) $@.err; exit 1; \
	fi; if [ -s $@.err ]; then $(SHOWERR) $@.err; fi; rm -f $@.err
endif

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(call CC_RUN,$(CC_BIN) $(MWC_FLAGS) $(DEPFLAGS) $(EXTRA_CFLAGS) -c -o $@ $<)
	$(FIXDEP)

# C++ TUs: same flags, language swapped to C++.
MWC_FLAGS_CXX := $(subst -lang=c99,-lang=c++,$(MWC_FLAGS))
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(call CC_RUN,$(CC_BIN) $(MWC_FLAGS_CXX) $(DEPFLAGS) $(EXTRA_CFLAGS) -c -o $@ $<)
	$(FIXDEP)

$(BUILD_DIR)/lib/NitroSDK/%.o: lib/NitroSDK/%.c
	@mkdir -p $(@D)
	$(or $(MWCC_OVERRIDE),$(MWCCARM)) $(MWC_FLAGS) -i $(dir $<) -i $(dir $<)../include $(EXTRA_CFLAGS) -c -o $@ $<

$(BUILD_DIR)/lib/NitroSDK/%.o: lib/NitroSDK/%.s
	@mkdir -p $(@D)
	$(dir $(MWCCARM))mwasmarm.exe -proc arm5TE -16 -gccinc -i lib/NitroSDK/include -o $@ $<

$(BUILD_DIR)/lib/NitroSystem/%.o: lib/NitroSystem/%.c
	@mkdir -p $(@D)
	$(or $(MWCC_OVERRIDE),$(MWCCARM)) $(MWC_FLAGS) -DNNS_FINALROM -i $(dir $<) -i $(dir $<)../include -i $(dir $<)../../include $(EXTRA_CFLAGS) -c -o $@ $<

$(MSL_GEN)/PREFIX_NITRO.h: $(SCRIPTS)/gen_msl_headers.py
	python $< $(MSL_GEN)

$(BUILD_DIR)/lib/mw-libraries/%.o: lib/mw-libraries/%.c | $(MSL_GEN)/PREFIX_NITRO.h
	@mkdir -p $(@D)
	$(or $(MWCC_OVERRIDE),$(MWCCARM)) $(MSL_FLAGS) $(EXTRA_CFLAGS) -c -o $@ $<
