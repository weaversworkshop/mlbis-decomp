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

MWC_FLAGS := -proc arm946e -O4,p -ipa file -interworking -inline on -sym on -enum int -lang=c99 -Cpp_exceptions off -gccext,on -gccinc -msgstyle gcc -char signed -DSDK_ARM9 -i $(INCLUDE_DIR) -i $(SHIM_INCLUDE) -i $(SDK_INCLUDE) -i $(NNS_INCLUDE) -i $(MSL_INCLUDE) -i $(MSL_ARM_INCLUDE) -include ansi_prefix.ARM.h

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
LIB_C_FILES  := $(filter-out lib/NitroSDK/% lib/mw-libraries/% lib/NitroSystem/%,$(call rwildcard,lib,*.c))
SDK_C_FILES  := $(call rwildcard,lib/NitroSDK/libraries,*.c)

# fs_overlay.c needs linker-script symbols; os_china.c needs China-region NVRAM.
SDK_SKIP := lib/NitroSDK/libraries/fs/src/fs_overlay.c \
            lib/NitroSDK/libraries/os/src/os_china.c
SDK_C_FILES := $(filter-out $(SDK_SKIP),$(SDK_C_FILES))

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

.PHONY: all clean distclean diff objdiff delinks progress sdk-rename sdk

all: $(FX_CONST_H) $(OBJ_FILES) objdiff.json

$(FX_CONST_H): $(FX_CONST_GEN) $(FX_CONST_CSV)
	@mkdir -p $(@D)
	python $(FX_CONST_GEN) $(FX_CONST_CSV) $@

sdk: $(FX_CONST_H)
	@mkdir -p $(BUILD_DIR)
	@echo "Compiling $(words $(SDK_C_FILES)) NitroSDK .c files..."
	@$(MAKE) -k $(SDK_OBJ_FILES) 2> $(BUILD_DIR)/sdk_build.log; \
	  ok=$$(find $(BUILD_DIR)/lib/NitroSDK -name '*.o' 2>/dev/null | wc -l); \
	  fail=$$(grep -c "Error 1" $(BUILD_DIR)/sdk_build.log 2>/dev/null || echo 0); \
	  echo "SDK build: $$ok compiled, $$fail failed. Log: $(BUILD_DIR)/sdk_build.log"

clean:
	rm -rf $(BUILD_DIR)/arm9 $(BUILD_DIR)/lib
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

# Per-file build overrides. Set MWCC_OVERRIDE for a different mwccarm binary,
# EXTRA_CFLAGS for additional flags. Used to match ROM bytes that depend on
# specific compiler versions or build-time defines.
$(BUILD_DIR)/lib/NitroSDK/libraries/mi/src/mi_memory.o: EXTRA_CFLAGS := -DSP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS
# snd_command.c was built with whole-program IPA; -ipa program makes SND_FlushCommand's
# register allocation match (-ipa file leaves it at 97.86%). Last -ipa flag wins.
$(BUILD_DIR)/lib/NitroSDK/libraries/snd/src/snd_command.o: EXTRA_CFLAGS := -ipa program

MWCC_OVERRIDE ?=
EXTRA_CFLAGS  ?=

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(or $(MWCC_OVERRIDE),$(MWCCARM)) $(MWC_FLAGS) $(EXTRA_CFLAGS) -c -o $@ $<

$(BUILD_DIR)/lib/NitroSDK/%.o: lib/NitroSDK/%.c
	@mkdir -p $(@D)
	$(or $(MWCC_OVERRIDE),$(MWCCARM)) $(MWC_FLAGS) -i $(dir $<) -i $(dir $<)../include $(EXTRA_CFLAGS) -c -o $@ $<
