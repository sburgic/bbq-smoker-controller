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
                        stm32g0xx_hal_rtc.c \
                        stm32g0xx_hal_rtc_ex.c \
                        stm32g0xx_hal_spi.c \
                        stm32g0xx_hal_spi_ex.c \
                        stm32g0xx_hal_tim.c \
                        stm32g0xx_hal_tim_ex.c \
                        stm32g0xx_hal_uart.c \
                        stm32g0xx_hal_uart_ex.c \
                        stm32g0xx_hal_usart.c

# Application source file directories
APP_SRC_DIR := $(PROJECT_ROOT_DIR)/source/application/src

# Application header file directories
APP_INC_DIR := $(PROJECT_ROOT_DIR)/source/application/include \
               $(STM_HAL_INC_DIR)

#
# List of application source files to be compiled
#

APP_SRC_LIST += interrupt.c \
                main.c \
                system_stm32g0xx.c

APP_SRC_LIST += startup_stm32g030xx.s

#
# Add source file directories to the path
#

vpath %.c $(STM_HAL_SRC_DIR) \
          $(APP_SRC_DIR)

vpath %.s $(APP_SRC_DIR) \
          $(PROJECT_ROOT_DIR)/source/toolchain/$(TOOLCHAIN)
