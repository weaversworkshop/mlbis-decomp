# Base constants
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
export MWCIncludes := $(INCLUDE_DIR)

# Export license for MWCCARM
export LM_LICENSE_FILE := $(abspath $(LICENSE))

# Build flags
MWC_FLAGS := -proc arm946e -thumb -O4 -inline=on,deferred -enum int -lang c99 -gccext on -msgstyle gcc -nowraplines -i $(INCLUDE_DIR)

# C and .o files
rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
C_FILES := $(call rwildcard,arm9/src,*.c)
OBJ_FILES := $(C_FILES:%.c=$(BUILD_DIR)/%.o)

# dsd inputs: any file under config/ counts as a prereq for re-delinking
DSD_CONFIG_DEPS := $(call rwildcard,config,*)


.PHONY: all clean distclean diff objdiff delinks

all: $(OBJ_FILES) objdiff.json

clean:
	rm -rf $(BUILD_DIR)/arm9
	rm -f objdiff.json

distclean: clean
	rm -rf $(BUILD_DIR)

# Run dsd to produce build/delinks/*.o from the extracted ROM + config.
# The stamp file is the Make-visible marker for "delinks are up to date".
delinks: $(DELINKS_DIR)/.stamp
$(DELINKS_DIR)/.stamp: $(DSD_CONFIG_DEPS)
	$(DSD) delink --config-path $(DSD_CONFIG)
	@mkdir -p $(@D)
	@touch $@

# Generate objdiff.json from `// objdiff-target:` comments in arm9/src/**/*.c
objdiff: objdiff.json
objdiff.json: $(C_FILES) $(SCRIPTS)/gen_objdiff.sh
	bash $(SCRIPTS)/gen_objdiff.sh > $@

diff: all delinks
	$(OBJDIFF) -p .

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(MWCCARM) $(MWC_FLAGS) -c -o $@ $<