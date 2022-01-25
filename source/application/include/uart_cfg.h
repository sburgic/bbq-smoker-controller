/**
 ** Name
 **   uart_cfg.h
 **
 ** Purpose
 **   UART driver configuration
 **
 ** Revision
 **   30-Jan-2021 (SSB) [] Initial
 **/

#ifndef __UART_CFG_H__
#define __UART_CFG_H__

#include <stm32g0xx_hal.h>

#define UART2_ENABLE           1
#define UART2_BUFFER_SIZE      1024
#define UART2_STRING_DELIMITER ((uint8_t)'\n')

#endif /* __UART_CFG_H__ */
