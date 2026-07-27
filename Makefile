TOOLS_DIR := tools
MWCCARM := $(TOOLS_DIR)/mwccarm/2.0/sp1p2/mwccarm.exe
LICENSE := $(TOOLS_DIR)/mwccarm/license.dat
SCRIPTS := $(TOOLS_DIR)/scripts
OBJDIFF := $(TOOLS_DIR)/objdiff.exe
DSD := $(TOOLS_DIR)/dsd.exe
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
MWC_FLAGS := -proc arm946e -O4,p -ipa file -interworking -inline on -sym on -enum int -lang=c99 -Cpp_exceptions off -gccext,on -gccinc -msgstyle gcc -char signed -DSDK_ARM9 -DSDK_FINALROM -DSDK_THREAD_INFINITY -DSDK_CODE_ARM -DSDK_TS -i $(INCLUDE_DIR) -i $(SHIM_INCLUDE) -i $(SDK_INCLUDE) -i $(NNS_INCLUDE) -i $(MSL_INCLUDE) -i $(MSL_ARM_INCLUDE) -include ansi_prefix.ARM.h

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
LIB_C_FILES  := $(filter-out lib/NitroSDK/% lib/mw-libraries/% lib/NitroSystem/%,$(call rwildcard,lib,*.c))
# Whole library surface (NitroSDK + NitroSystem + DS2.0 MSL) for tu_diff content-match.
MSL := lib/mw-libraries/ARM_EABI_Support_DS2.0/msl/MSL_C
SDK_C_FILES  := $(call rwildcard,lib/NitroSDK/libraries,*.c) \
                $(call rwildcard,lib/NitroSystem,*.c) \
                $(call rwildcard,lib/mw-libraries/ARM_EABI_Support_DS2.0,*.c) \
                $(call rwildcard,lib/syscall,*.c)

# Can't compile standalone: os_china.c (China NVRAM); MSL formatted-I/O/alloc/wchar (need <ctime>).
SDK_SKIP := lib/NitroSDK/libraries/os/src/os_china.c \
            $(MSL)/MSL_Common/Src/alloc.c $(MSL)/MSL_Common/Src/printf.c \
            $(MSL)/MSL_Common/Src/scanf.c $(MSL)/MSL_Common/Src/time.c \
            $(MSL)/MSL_Common/Src/char_io.c $(MSL)/MSL_Common/Src/direct_io.c \
            $(MSL)/MSL_Common/Src/mbstring.c \
            $(wildcard $(MSL)/MSL_Common/Src/w*.c) \
            lib/mw-libraries/ARM_EABI_Support_DS2.0/msl/MSL_Extras/MSL_Common/Src/extras.c
# Runtime helpers live in dirs with '(' that break recipe shells; compiled via find in `sdk`.
SDK_RUNTIME  := $(call rwildcard,lib/mw-libraries/ARM_EABI_Support_DS2.0/Runtime,*.c)
SDK_C_FILES  := $(filter-out $(SDK_SKIP) $(SDK_RUNTIME),$(SDK_C_FILES))

# objdiff units (synced with objdiff_units in sdk_mappings.json).
SDK_DIFF_FILES := $(shell python -c "import json; print(' '.join(u['source'] for u in json.load(open('config/sdk_mappings.json')).get('objdiff_units', [])))")

C_FILES   := $(call rwildcard,arm9/src,*.c) $(LIB_C_FILES) $(SDK_DIFF_FILES)
CXX_FILES := $(call rwildcard,arm9/src,*.cpp)
OBJ_FILES := $(C_FILES:%.c=$(BUILD_DIR)/%.o) $(CXX_FILES:%.cpp=$(BUILD_DIR)/%.o)
SDK_OBJ_FILES := $(SDK_C_FILES:%.c=$(BUILD_DIR)/%.o)

DSD_CONFIG_DEPS := $(call rwildcard,config,*)
OBJDIFF_CLI := $(TOOLS_DIR)/objdiff-cli.exe

TARGET ?= main_7

SDK_MAPPINGS := config/sdk_mappings.json
SDK_APPLY    := python $(SCRIPTS)/apply_sdk_mappings.py
SYNC_NAMES   := python $(SCRIPTS)/sync_renames.py

.PHONY: all clean distclean diff objdiff delinks progress progress-py obj-match data-catalog callgraph check-relocs sdk-rename sync-names sdk sdk-sweep sdk-sweep-all sdk-check sdk-audit

all: $(FX_CONST_H) $(OBJ_FILES) objdiff.json

$(FX_CONST_H): $(FX_CONST_GEN) $(FX_CONST_CSV)
	@mkdir -p $(@D)
	python $(FX_CONST_GEN) $(FX_CONST_CSV) $@

sdk: $(FX_CONST_H)
	@mkdir -p $(BUILD_DIR)
	@echo "Compiling $(words $(SDK_C_FILES)) library .c files (+ MSL Runtime)..."
	@$(MAKE) -k $(SDK_OBJ_FILES) 2> $(BUILD_DIR)/sdk_build.log; \
	  find lib/mw-libraries/ARM_EABI_Support_DS2.0/Runtime -name '*.c' ! -name '__NITRO_eabi_init.c' | while IFS= read -r c; do \
	    o="$(BUILD_DIR)/$${c%.c}.o"; mkdir -p "$$(dirname "$$o")"; \
	    $(MWCCARM) $(MWC_FLAGS) -i $(MSL)/MSL_Common_Embedded/Math/Include -c -o "$$o" "$$c" 2>> $(BUILD_DIR)/sdk_build.log || true; \
	  done; \
	  ok=$$(find $(BUILD_DIR)/lib -name '*.o' 2>/dev/null | wc -l); \
	  fail=$$(grep -c "Error 1" $(BUILD_DIR)/sdk_build.log 2>/dev/null || echo 0); \
	  echo "SDK build: $$ok objects compiled, $$fail failed. Log: $(BUILD_DIR)/sdk_build.log"

clean:
	rm -rf $(BUILD_DIR)/arm9
	rm -f objdiff.json

distclean: clean
	rm -rf $(BUILD_DIR)

sdk-rename:
	$(SDK_APPLY)

delinks: $(DELINKS_DIR)/.stamp
$(DELINKS_DIR)/.stamp: $(DSD_CONFIG_DEPS) $(SDK_MAPPINGS)
	$(SDK_APPLY)
	$(DSD) delink --config-path $(DSD_CONFIG)
	$(DSD) dis --config-path $(DSD_CONFIG) --asm-path asm
	@mkdir -p $(@D)
	@touch $@

objdiff: objdiff.json
objdiff.json: $(C_FILES) $(CXX_FILES) $(SDK_MAPPINGS) $(SCRIPTS)/gen_objdiff.py
	python $(SCRIPTS)/gen_objdiff.py > $@

# sync-names harvests mwcc's mangled names from the compiled game .o into
# symbols.txt (edits it), so delinks must run AFTER via recursive make.
sync-names: all
	$(SYNC_NAMES) $(SYNC_ARGS)

diff: all sync-names
	@$(MAKE) --no-print-directory delinks
	@python $(SCRIPTS)/asm_smell.py --sweep
	$(OBJDIFF) -p .

progress: all sync-names
	@$(MAKE) --no-print-directory delinks
	$(OBJDIFF_CLI) report generate -p . -o - | python $(SCRIPTS)/build_todo.py $(TARGET) --report -
	@python $(SCRIPTS)/asm_smell.py --sweep
	@python $(SCRIPTS)/data_catalog.py

# Same table without objdiff-cli (which panics on CODE-flagged data sections and has 4 known false negatives)
progress-py: all sync-names
	@$(MAKE) --no-print-directory delinks
	python $(SCRIPTS)/obj_match.py | python $(SCRIPTS)/build_todo.py $(TARGET) --report -
	@python $(SCRIPTS)/asm_smell.py --sweep
	@python $(SCRIPTS)/data_catalog.py

# Per-unit match summary + unresolved callee names (UNIT=name to narrow).
obj-match: delinks
	@python $(SCRIPTS)/obj_match.py --text $(if $(UNIT),--unit $(UNIT),) $(OBJ_MATCH_ARGS)

data-catalog: delinks
	python $(SCRIPTS)/data_catalog.py

# Faithfulness audit: cross-boundary branches with no reloc.
check-relocs: delinks
	python $(SCRIPTS)/check_relocs.py

# SDK provenance (need `make sdk` first). sweep=DISCOVERY (tu_diff), check=VERIFY (sdk_equiv).
# GAP picks the gap (default main_7; itcm_1/dtcm_1 for TCM-resident SDK).
#   make sdk-sweep [GAP=itcm_1]  |  make sdk-sweep-all
#   make sdk-check FUNC="func_xxxx ..." [GAP=itcm_1] [SDK_EQUIV_ARGS=-v]
GAP ?= main_7
GAP_O = $(BUILD_DIR)/delinks/_dsd_gap@$(GAP).o
sdk-sweep: delinks
	@python $(SCRIPTS)/tu_diff.py --all --new --gap-o $(GAP_O)

sdk-sweep-all: delinks
	@for g in $(BUILD_DIR)/delinks/_dsd_gap@*.o; do \
		out=$$(python $(SCRIPTS)/tu_diff.py --all --new --gap-o "$$g" 2>/dev/null); \
		echo "$$out" | grep -qF '[NEW' && { echo "##### $$g #####"; echo "$$out"; } || true; \
	done

# sdk-audit: which SDK .c fail to compile (tu_diff blind spots).
sdk-audit:
	@python $(SCRIPTS)/sdk_compile_audit.py --reasons

FUNC ?=
SDK_EQUIV_ARGS ?=
sdk-check: delinks
	@test -n "$(FUNC)" || { echo 'usage: make sdk-check FUNC="func_xxxx [func_yyyy ...]" [SDK_EQUIV_ARGS=-v]'; exit 2; }
	@test -n "$$(find $(BUILD_DIR)/lib -name '*.o' 2>/dev/null | head -1)" || { echo "No SDK objects in $(BUILD_DIR)/lib -- run 'make sdk' first."; exit 2; }
	@python $(SCRIPTS)/sdk_equiv.py $(FUNC) --gap-o $(GAP_O) $(SDK_EQUIV_ARGS)

# Mermaid call graph (game-only by default). Scope via CALLGRAPH_ARGS, e.g.
#   make callgraph CALLGRAPH_ARGS="--root func_0200907c --undone"
CALLGRAPH_FLAGS ?= --connected-only --game-only
CALLGRAPH_ARGS ?=
callgraph: all delinks
	$(OBJDIFF_CLI) report generate -p . -o - | python $(SCRIPTS)/call_graph.py $(TARGET) --report - $(CALLGRAPH_FLAGS) $(CALLGRAPH_ARGS)

# Per-file overrides: MWCC_OVERRIDE (compiler), EXTRA_CFLAGS (defines).
$(BUILD_DIR)/lib/NitroSDK/libraries/mi/src/mi_memory.o: EXTRA_CFLAGS := -DSP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS

# Game TUs = sp2p3 (SDK stays sp1p2). b138 == b139 output project-wide (verified
# 2026-07-24); sp2p3 (2008-06) predates JP ship, sp2p4 (2009-06) does not.
$(BUILD_DIR)/arm9/src/%.o: MWCC_OVERRIDE := $(TOOLS_DIR)/mwccarm/2.0/sp2p3/mwccarm.exe

# clScriptSystem/clGlobalEvent are real C++ classes (RTTI in ROM).
$(BUILD_DIR)/arm9/src/main/clGlobalEvent.o: EXTRA_CFLAGS := -RTTI on

MWCC_OVERRIDE ?=
EXTRA_CFLAGS  ?=

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(or $(MWCC_OVERRIDE),$(MWCCARM)) $(MWC_FLAGS) $(EXTRA_CFLAGS) -c -o $@ $<

# C++ TUs: same flags, language swapped to C++.
MWC_FLAGS_CXX := $(subst -lang=c99,-lang=c++,$(MWC_FLAGS))
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(or $(MWCC_OVERRIDE),$(MWCCARM)) $(MWC_FLAGS_CXX) $(EXTRA_CFLAGS) -c -o $@ $<

$(BUILD_DIR)/lib/NitroSDK/%.o: lib/NitroSDK/%.c
	@mkdir -p $(@D)
	$(or $(MWCC_OVERRIDE),$(MWCCARM)) $(MWC_FLAGS) -i $(dir $<) -i $(dir $<)../include $(EXTRA_CFLAGS) -c -o $@ $<

$(BUILD_DIR)/lib/NitroSystem/%.o: lib/NitroSystem/%.c
	@mkdir -p $(@D)
	$(or $(MWCC_OVERRIDE),$(MWCCARM)) $(MWC_FLAGS) -DNNS_FINALROM -i $(dir $<) -i $(dir $<)../include -i $(dir $<)../../include $(EXTRA_CFLAGS) -c -o $@ $<

# MSL C runtime: the double-precision math sources need the fdlibm include dir.
$(BUILD_DIR)/lib/mw-libraries/%.o: lib/mw-libraries/%.c
	@mkdir -p $(@D)
	$(or $(MWCC_OVERRIDE),$(MWCCARM)) $(MWC_FLAGS) -i $(MSL)/MSL_Common_Embedded/Math/Include $(EXTRA_CFLAGS) -c -o $@ $<
