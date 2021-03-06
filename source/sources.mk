## Name
##   sources.mk
##
## Purpose
##   Smoker controller project sources
##
## Revision
##    19-Nov-2021 (SSB) [] Initial
##

# Check if project root directory is defined.
ifeq ($(PROJECT_ROOT_DIR),)
    $(error Project root directory not defined!)
endif

# Libs root directory
LIBS_ROOT_DIR ?= $(PROJECT_ROOT_DIR)/../libs

# STM CMSIS library root directory
STM_CMSIS_LIB_ROOT_DIR ?= $(LIBS_ROOT_DIR)/STM32Cube_FW_G0_V1.5.0/Drivers/CMSIS

# ARM Math library root directory
ARM_MATH_LIB_ROOT_DIR ?= $(STM_CMSIS_LIB_ROOT_DIR)/DSP

# ARM Math library source file directories
ARM_MATH_SRC_DIR := $(ARM_MATH_LIB_ROOT_DIR)/Source/ControllerFunctions \

# ARM Math library header file directories
ARM_MATH_INC_DIR := $(STM_CMSIS_LIB_ROOT_DIR)/Include \
                    $(ARM_MATH_LIB_ROOT_DIR)/Include

# ARM_MATH library list of files to be compiled
ARM_MATH_LIB_SRC_LIST := arm_pid_init_f32.c

# STM HAL library root directory
STM_HAL_LIB_ROOT_DIR ?= $(LIBS_ROOT_DIR)/STM32Cube_FW_G0_V1.5.0/Drivers/STM32G0xx_HAL_Driver

# STM HAL library source file directories
STM_HAL_SRC_DIR := $(STM_HAL_LIB_ROOT_DIR)/Src

# STM HAL library header file directories
STM_HAL_INC_DIR := $(STM_HAL_LIB_ROOT_DIR)/Inc \
                   $(STM_CMSIS_LIB_ROOT_DIR)/Device/ST/STM32G0xx/Include \
                   $(STM_CMSIS_LIB_ROOT_DIR)/Include \
                   $(PROJECT_ROOT_DIR)/source/application/include

# STM HAL library list of files to be compiled
STM_HAL_LIB_SRC_LIST := stm32g0xx_hal.c \
                        stm32g0xx_hal_adc.c \
                        stm32g0xx_hal_adc_ex.c \
                        stm32g0xx_hal_cortex.c \
                        stm32g0xx_hal_dma.c \
                        stm32g0xx_hal_dma_ex.c \
                        stm32g0xx_hal_flash.c \
                        stm32g0xx_hal_flash_ex.c \
                        stm32g0xx_hal_gpio.c \
                        stm32g0xx_hal_pwr.c \
                        stm32g0xx_hal_pwr_ex.c \
                        stm32g0xx_hal_rcc.c \
                        stm32g0xx_hal_rcc_ex.c \
                        stm32g0xx_hal_tim.c \
                        stm32g0xx_hal_tim_ex.c \
                        stm32g0xx_hal_uart.c

# Application source file directories
APP_SRC_DIR := $(PROJECT_ROOT_DIR)/source/application/src

# Application header file directories
APP_INC_DIR := $(PROJECT_ROOT_DIR)/source/application/include \
               $(ARM_MATH_INC_DIR) \
               $(STM_HAL_INC_DIR) \
               $(LIBS_ROOT_DIR) \
               $(LIBS_ROOT_DIR)/lcd2wire/include \
               $(LIBS_ROOT_DIR)/one_wire/include \
               $(LIBS_ROOT_DIR)/buffer/include \
               $(LIBS_ROOT_DIR)/uart/include

#
# List of application source files to be compiled
#

APP_SRC_LIST += bluetooth.c \
                buzzer.c \
                configuration.c \
                encoder.c \
                fan.c \
                flash.c \
                hal_msp.c \
                interrupt.c \
                main.c \
                menu.c \
                pid.c \
                state.c \
                system_init.c \
                system_stm32g0xx.c \
                time.c \
                utils.c \
                vmon.c

APP_SRC_LIST += startup_stm32g030xx.s

#
# Add source file directories to the path
#

vpath %.c $(ARM_MATH_SRC_DIR) \
          $(STM_HAL_SRC_DIR) \
          $(APP_SRC_DIR)

vpath %.s $(APP_SRC_DIR) \
          $(PROJECT_ROOT_DIR)/source/toolchain/$(TOOLCHAIN)
