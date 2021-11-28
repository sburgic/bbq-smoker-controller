/**
 ** Name
 **   lcd2wire_cfg.h
 **
 ** Purpose
 **   LCD2Wire driver configuration
 **
 ** Revision
 **   28-Nov-2021 (SSB) [] Initial
 **/

#ifndef __LCD2WIRE_CFG_H__
#define __LCD2WIRE_CFG_H__

#include <stm32g0xx_hal.h>

#define LCD2WIRE_SCREEN_SIZE 2

#define LCD2WIRE_DATA_PIN  GPIO_PIN_1
#define LCD2WIRE_DATA_PORT GPIOA

#define LCD2WIRE_CLK_PIN  GPIO_PIN_0
#define LCD2WIRE_CLK_PORT GPIOA

#define LCD2WIRE_GPIO_CLOCK_ENABLE() \
    do { __HAL_RCC_GPIOA_CLK_ENABLE(); } while(0)

#endif /* __LCD2WIRE_CFG_H__ */
