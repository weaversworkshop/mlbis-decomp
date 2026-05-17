# Base constants
TOOLS_DIR := tools
MWCCARM := $(TOOLS_DIR)/mwccarm/2.0/sp1p2/mwccarm.exe
LICENSE := $(TOOLS_DIR)/mwccarm/license.dat
OBJDIFF := $(TOOLS_DIR)/objdiff.exe
BUILD_DIR := build
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


.PHONY: all clean diff

all: $(OBJ_FILES)

clean:
	rm -rf $(BUILD_DIR)

diff: all
	$(OBJDIFF) -p .

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(MWCCARM) $(MWC_FLAGS) -c -o $@ $<