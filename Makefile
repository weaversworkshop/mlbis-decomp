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

MWC_FLAGS := -proc arm946e -O4,p -ipa file -interworking -inline on -sym on -enum int -lang=c99 -Cpp_exceptions off -gccext,on -gccinc -msgstyle gcc -char signed -DSDK_ARM9 -DSDK_FINALROM -DSDK_THREAD_INFINITY -DSDK_CODE_ARM -i $(INCLUDE_DIR) -i $(SHIM_INCLUDE) -i $(SDK_INCLUDE) -i $(NNS_INCLUDE) -i $(MSL_INCLUDE) -i $(MSL_ARM_INCLUDE) -include ansi_prefix.ARM.h

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
LIB_C_FILES  := $(filter-out lib/NitroSDK/% lib/mw-libraries/% lib/NitroSystem/%,$(call rwildcard,lib,*.c))
# `make sdk` compiles every library .o we can, so tu_diff.py can content-match
# the whole SDK surface against the ROM (catches inlined/linked SDK in game TUs).
# Covers NitroSDK + NitroSystem + the DS2.0 MSL C runtime (the version this game
# built with -- NOT the OLD / DS1.2 MSL trees in the submodule).
MSL := lib/mw-libraries/ARM_EABI_Support_DS2.0/msl/MSL_C
SDK_C_FILES  := $(call rwildcard,lib/NitroSDK/libraries,*.c) \
                $(call rwildcard,lib/NitroSystem,*.c) \
                $(call rwildcard,lib/mw-libraries/ARM_EABI_Support_DS2.0,*.c) \
                $(call rwildcard,lib/syscall,*.c)

# Files that can't compile standalone in this tree:
# - fs_overlay.c needs linker-script symbols; os_china.c needs China NVRAM.
# - MSL formatted-I/O, alloc, wide-char, and EABI runtime-init reference a C++
#   <ctime> header (and heap-config defines) not shipped in this MSL.
SDK_SKIP := lib/NitroSDK/libraries/fs/src/fs_overlay.c \
            lib/NitroSDK/libraries/os/src/os_china.c \
            $(MSL)/MSL_Common/Src/alloc.c $(MSL)/MSL_Common/Src/printf.c \
            $(MSL)/MSL_Common/Src/scanf.c $(MSL)/MSL_Common/Src/time.c \
            $(MSL)/MSL_Common/Src/char_io.c $(MSL)/MSL_Common/Src/direct_io.c \
            $(MSL)/MSL_Common/Src/mbstring.c \
            $(wildcard $(MSL)/MSL_Common/Src/w*.c) \
            lib/mw-libraries/ARM_EABI_Support_DS2.0/msl/MSL_Extras/MSL_Common/Src/extras.c
# MSL Runtime helpers (setjmp, rt_div0, Thumb_interworking, fp env, ...) live in
# dirs with '(' in the path, which break make's recipe shell when passed in a
# file list. Excluded here and compiled separately in the `sdk` recipe via find.
SDK_RUNTIME  := $(call rwildcard,lib/mw-libraries/ARM_EABI_Support_DS2.0/Runtime,*.c)
SDK_C_FILES  := $(filter-out $(SDK_SKIP) $(SDK_RUNTIME),$(SDK_C_FILES))

# SDK files we treat as objdiff units (kept in sync with the `objdiff_units`
# array in config/sdk_mappings.json). Compiled in the normal `make` so their
# .o exists for objdiff to diff against the ROM.
SDK_DIFF_FILES := $(shell python -c "import json; print(' '.join(u['source'] for u in json.load(open('config/sdk_mappings.json')).get('objdiff_units', [])))")

C_FILES   := $(call rwildcard,arm9/src,*.c) $(LIB_C_FILES) $(SDK_DIFF_FILES)
OBJ_FILES := $(C_FILES:%.c=$(BUILD_DIR)/%.o)
SDK_OBJ_FILES := $(SDK_C_FILES:%.c=$(BUILD_DIR)/%.o)

DSD_CONFIG_DEPS := $(call rwildcard,config,*)
OBJDIFF_CLI := $(TOOLS_DIR)/objdiff-cli.exe

TARGET ?= main_7

SDK_MAPPINGS := config/sdk_mappings.json
SDK_APPLY    := python $(SCRIPTS)/apply_sdk_mappings.py

.PHONY: all clean distclean diff objdiff delinks progress callgraph check-relocs sdk-rename sdk sdk-sweep sdk-check

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
objdiff.json: $(C_FILES) $(SDK_MAPPINGS) $(SCRIPTS)/gen_objdiff.py
	python $(SCRIPTS)/gen_objdiff.py > $@

diff: all delinks
	$(OBJDIFF) -p .

progress: all delinks
	$(OBJDIFF_CLI) report generate -p . -o - | python $(SCRIPTS)/build_todo.py $(TARGET) --report -

# Faithfulness audit: branches across a symbol boundary with no relocation
# (the `bl <invalid>` / trampoline class). Independent of the match-% report.
check-relocs: delinks
	python $(SCRIPTS)/check_relocs.py

# SDK provenance -- two layers (both need `make sdk` run once for the SDK .o):
#   sdk-sweep : DISCOVERY. tu_diff.py content-matches every built SDK .o against
#               the whole gap -> surfaces *candidate* SDK functions. Fast, but
#               matches modulo relocations (call targets masked) -> confirm hits.
#   sdk-check : VERIFICATION of specific function(s). sdk_equiv.py is
#               relocation-aware and RECURSES through call targets + .word data,
#               so a stock-looking body over a forked callee is reported DIVERGES
#               with the exact fork point. This is the authoritative check.
#   make sdk-check FUNC=func_02009fcc
#   make sdk-check FUNC="func_0200a024 func_0200a0c8" SDK_EQUIV_ARGS=-v
#   make sdk-check FUNC=func_0200a0c8 SDK_EQUIV_ARGS="--sdk-func DisposeCallback"
sdk-sweep: delinks
	@python $(SCRIPTS)/tu_diff.py --all --new

FUNC ?=
SDK_EQUIV_ARGS ?=
sdk-check: delinks
	@test -n "$(FUNC)" || { echo 'usage: make sdk-check FUNC="func_xxxx [func_yyyy ...]" [SDK_EQUIV_ARGS=-v]'; exit 2; }
	@test -n "$$(find $(BUILD_DIR)/lib -name '*.o' 2>/dev/null | head -1)" || { echo "No SDK objects in $(BUILD_DIR)/lib -- run 'make sdk' first."; exit 2; }
	@python $(SCRIPTS)/sdk_equiv.py $(FUNC) $(SDK_EQUIV_ARGS)

# Mermaid call graph (vertical). By default drops lone nodes AND mapped SDK
# functions (game functions only) via CALLGRAPH_FLAGS; scope a huge graph with
# CALLGRAPH_ARGS, e.g.:
#   make callgraph CALLGRAPH_ARGS="--root func_0200907c --undone"
#   make callgraph CALLGRAPH_ARGS="--leaves"
# To include SDK and/or lone nodes, override: make callgraph CALLGRAPH_FLAGS=
CALLGRAPH_FLAGS ?= --connected-only --game-only
CALLGRAPH_ARGS ?=
callgraph: all delinks
	$(OBJDIFF_CLI) report generate -p . -o - | python $(SCRIPTS)/call_graph.py $(TARGET) --report - $(CALLGRAPH_FLAGS) $(CALLGRAPH_ARGS)

# Per-file build overrides. Set MWCC_OVERRIDE for a different mwccarm binary,
# EXTRA_CFLAGS for additional flags. Used to match ROM bytes that depend on
# specific compiler versions or build-time defines.
$(BUILD_DIR)/lib/NitroSDK/libraries/mi/src/mi_memory.o: EXTRA_CFLAGS := -DSP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS

MWCC_OVERRIDE ?=
EXTRA_CFLAGS  ?=

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(or $(MWCC_OVERRIDE),$(MWCCARM)) $(MWC_FLAGS) $(EXTRA_CFLAGS) -c -o $@ $<

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
