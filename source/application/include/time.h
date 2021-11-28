/**
 ** Name
 **   time.h
 **
 ** Purpose
 **   Time routines
 **
 ** Revision
 **   28-Nov-2021 (SSB) [] Initial
 **/

#ifndef __TIME_H__
#define __TIME_H__

#include "ptypes.h"

#include <stm32g0xx_hal.h>

typedef enum
{
    BSP_TIME_USEC = 1,
    BSP_TIME_MSEC = 1000,
    BSP_TIME_SEC  = 1000000
} Bsp_Time_Base;

typedef uint64_t Bsp_Time;

status_t bsp_tmr_init( void );
void bsp_get_time( Bsp_Time* tv );
void bsp_wait( Bsp_Time time, Bsp_Time_Base base );
bool_t bsp_is_timeout( Bsp_Time timeout );
void bsp_set_timeout( Bsp_Time      time
                    , Bsp_Time_Base base
                    , Bsp_Time*     timeout
                    );
void tim17_overflow_irq_hdl( void );

#endif /* __TIME_H__ */
