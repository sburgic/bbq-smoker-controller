/**
 ** Name
 **   flash.h
 **
 ** Purpose
 **   Fan control routines
 **
 ** Revision
 **   21-Jan-2022 (SSB) [] Initial
 **/

#ifndef __FLASH_H__
#define __FLASH_H__

#include "ptypes.h"

#include <stm32g0xx_hal.h>

#define FLASH_ADDR_PAGE_31   ((uint32_t)0x08007C00)
#define FLASH_USER_PAGE_ADDR FLASH_ADDR_PAGE_31
#define FLASH_PAGE_ERASE_OK  (uint32_t)(0xFFFFFFFF)

status_t flash_read( void* buff, uint32_t size, uint32_t offset );
status_t flash_write( void* buff, uint32_t size, uint32_t offset );
status_t flash_erase( void );

#endif /* __FLASH_H__ */
