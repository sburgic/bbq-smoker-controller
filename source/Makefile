## Name
##   Makefile
##
## Purpose
##   Smoker controller central makefile
##
## Revision
##    19-Nov-2021 (SSB) [] Initial
##

# Let make be silent by default
MAKEFLAGS += --silent

# Check if supported toolchain is selected.
# If none toolchain is selected, IAR is used by default.
ifeq ($(TOOLCHAIN),)
    $(info Toolchain not defined, using IAR as default!)
    TOOLCHAIN := iar
else
    ifeq ($(TOOLCHAIN),gcc)
        $(info Building with GCC toolchain.)
    else
        $(error Unknown toolchain selected!)
    endif
endif

# Check build type.
# If none build type is selected the debug is used by default.
ifeq ($(BUILD_TYPE),)
    $(info Build type not defined, using debug as default!)
    BUILD_TYPE := debug
else
    ifeq ($(BUILD_TYPE),release)
        $(info Building with release flags.)
    else
        ifeq ($(BUILD_TYPE),debug)
            $(info Building with debug flags.)
        else
            $(error Unknown build type selected!)
        endif
    endif
endif

CPU_FAMILY := M0+

# Common base directory for all projects
BASE_DIR ?= ../../base

# Keed this to add new line after first information dump
$(info )

# Build output directory
BUILD_DIR ?= build_$(HOST_OS)_$(TOOLCHAIN)_$(BUILD_TYPE)

# Project root directory
PROJECT_ROOT_DIR ?= $(realpath $(subst $(notdir $(CURDIR)),,$(CURDIR)))

# Project name
PROJECT_NAME ?= $(notdir $(PROJECT_ROOT_DIR))

include $(BASE_DIR)/oshelpers.mk

# Application defines
APP_DEFS := -DUSE_HAL_DRIVER \
            -DSTM32G030xx

# Linker script without extension
LDSCRIPT := $(PROJECT_ROOT_DIR)/source/toolchain/$(TOOLCHAIN)/linker/stm32g030xx_flash

#
# Output directories
#

APP_OBJ_DIR := $(BUILD_DIR)/app/$(BUILD_TYPE)/obj
APP_EXE_DIR := $(BUILD_DIR)/app/$(BUILD_TYPE)/exe
APP_DEP_DIR := $(BUILD_DIR)/app/$(BUILD_TYPE)/dep

STM_HAL_LIB_OBJ_DIR := $(BUILD_DIR)/stm/hal/$(BUILD_TYPE)/obj
STM_HAL_LIB_OUT_DIR := $(BUILD_DIR)/stm/hal/$(BUILD_TYPE)/lib
STM_HAL_LIB_DEP_DIR := $(BUILD_DIR)/stm/hal/$(BUILD_TYPE)/dep

# Libraries to create
STM_HAL_LIB := stm32_hal.a

# Created libraries to be added to the build
CC_LIBS := $(STM_HAL_LIB)

# Include toolchain definitions
include $(BASE_DIR)/toolchain_$(TOOLCHAIN).mk

# Include source file definitions
include sources.mk

# Create object list(s)
STM_HAL_LIB_OBJ_LIST := $(addprefix $(STM_HAL_LIB_OBJ_DIR)/,$(filter %.o,$(STM_HAL_LIB_SRC_LIST:.c=.o)))
APP_OBJ_LIST         := $(addprefix $(APP_OBJ_DIR)/,$(filter %.o,$(APP_SRC_LIST:.c=.o) $(APP_SRC_LIST:.s=.o)))

# Create header include flags
CC_INC_FLAG          := -I
STM_HAL_INC_PARAMS   := $(addprefix $(CC_INC_FLAG),$(STM_HAL_INC_DIR))
APP_INC_PARAMS       := $(addprefix $(CC_INC_FLAG),$(APP_INC_DIR))

#
# Add existing libraries to the build
#

LIBS_ROOT_DIR := $(PROJECT_ROOT_DIR)/../libs

LCD2WIRE_LIB_ROOT ?= $(LIBS_ROOT_DIR)/lcd2wire

include $(LCD2WIRE_LIB_ROOT)/lcd2wire.mk

# Create library include flags
CC_LIBS_DIR += $(STM_HAL_LIB_OUT_DIR)

CC_LINK_INC_FLAG   := -L
CC_LINK_INC_PARAMS := $(addprefix $(CC_LINK_INC_FLAG),$(CC_LIBS_DIR))

# Create library link flags
CC_LINK_FLAG := -l
CC_LINK_LIBS := $(addprefix $(CC_LINK_FLAG),$(basename $(CC_LIBS)))

#
# Include dependencies
#

ifeq ($(MAKECMDGOALS),clean)
    INC_DEPENDENCIES := 0
else
    ifeq ($(MAKECMDGOALS),clean_all)
        INC_DEPENDENCIES := 0
    else
        INC_DEPENDENCIES := 1
    endif
endif

ifeq ($(INC_DEPENDENCIES),1)
    -include $(wildcard $(APP_DEP_DIR)/*.d)
    -include $(wildcard $(STM_HAL_LIB_DEP_DIR)/*.d)
endif

#
# Build rules
#

.FORCE:

$(APP_OBJ_DIR)/%.o: %.c | $(APP_OBJ_DIR) $(APP_DEP_DIR)
	@echo "Compiling $(notdir $<)"
	$(CC) $(CFLAGS) $(DEP_FLAGS) $(APP_DEP_DIR)/$*.d $(APP_DEFS) $(APP_INC_PARAMS) -o $@ -c $<

$(APP_OBJ_DIR)/%.o: %.s | $(APP_OBJ_DIR)
	@echo "Assembling $(notdir $<)"
	$(AS) $(ASFLAGS) -o $@ -c $<

$(TARGET_ELF): .FORCE $(CC_LIBS) $(APP_OBJ_LIST) | $(APP_EXE_DIR)
	@echo "Linking $@"
	$(LD) -o $@ $(APP_OBJ_LIST) $(LDFLAGS) $(CC_LINK_INC_PARAMS) $(CC_LINK_LIBS)
	@echo

$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCPY) $(OBJCPY_BIN_FLAGS) $< $@

$(TARGET_HEX): $(TARGET_ELF)
	$(OBJCPY) $(OBJCPY_HEX_FLAGS) $< $@

$(STM_HAL_LIB_OBJ_DIR)/%.o: %.c | $(STM_HAL_LIB_OBJ_DIR) $(STM_HAL_LIB_DEP_DIR)
	@echo "Compiling $(notdir $<)"
	$(CC) $(CFLAGS) $(DEP_FLAGS) $(STM_HAL_LIB_DEP_DIR)/$*.d $(APP_DEFS) $(STM_HAL_INC_PARAMS) -o $@ -c $<

$(STM_HAL_LIB): $(STM_HAL_LIB_OBJ_LIST) | $(STM_HAL_LIB_OUT_DIR)
	@echo "Archiving library $@"
	$(AR) $(ARFLAGS) $(STM_HAL_LIB_OUT_DIR)/lib$@ $^
	@echo

$(APP_OBJ_DIR):
	$(MD) $@

$(APP_EXE_DIR):
	$(MD) $@

$(APP_DEP_DIR):
	$(MD) $@

$(STM_HAL_LIB_OBJ_DIR):
	$(MD) $@

$(STM_HAL_LIB_OUT_DIR):
	$(MD) $@

$(STM_HAL_LIB_DEP_DIR):
	$(MD) $@

size: $(TARGET_ELF)
	$(SIZE) $(TARGET_ELF)

# Extended target list
ifeq ($(TOOLCHAIN),gcc)
    TARGET_EXTENDED := size
endif

#
# Recipes
#

all: $(TARGET_ELF) $(EXT_TARGET) $(TARGET_EXTENDED)
	@echo Building of $(PROJECT_NAME) done.

# Clean recipe will remove only application directory
clean:
	$(RM) $(APP_OBJ_DIR)
	$(RM) $(APP_EXE_DIR)
	$(RM) $(APP_DEP_DIR)
	@echo "Clean of $(PROJECT_NAME) done."

# Clean all recipe will remove the whole build directory
clean_all:
	$(RM) $(BUILD_DIR)
	$(RM) build
	@echo "Clean all of $(PROJECT_NAME) done."

.PHONY: all clean clean_all

.DEFAULT_GOAL := all
