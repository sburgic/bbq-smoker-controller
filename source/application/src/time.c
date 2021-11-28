/**
 ** Name
 **   time.c
 **
 ** Purpose
 **   Time routines
 **
 ** Revision
 **   28-Nov-2021 (SSB) [] Initial
 **/

#include "time.h"

// #include <stdint.h>

static volatile uint32_t us_time_high         = 0;
static volatile uint16_t us_time_low          = 0;
static volatile bool_t   us_time_low_overflow = FALSE;

/* 1 us tick used as time base */
static TIM_HandleTypeDef tim17_base =
{
    .Instance               = TIM17,
    .Init.Prescaler         = 0x003F,
    .Init.CounterMode       = TIM_COUNTERMODE_UP,
    .Init.Period            = 0xFFFF,
    .Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1,
    .Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE
};

status_t bsp_tmr_init( void )
{
    status_t          ret = STATUS_ERROR;
    HAL_StatusTypeDef hret;

    hret = HAL_TIM_Base_Init( &tim17_base );

    if ( HAL_OK == hret )
    {
        hret = HAL_TIM_Base_Start_IT( &tim17_base );
    }

    if ( HAL_OK == hret )
    {
        ret = STATUS_OK;
    }

    return ret;
}

void bsp_get_time( Bsp_Time* tv )
{
    volatile uint64_t lo_t;
    volatile uint64_t hi_t;
    uint16_t          master_tmp;
    uint16_t          slave_tmp;

    hi_t = (uint64_t) us_time_high;

    slave_tmp   = us_time_low;
    master_tmp  = tim17_base.Instance->CNT;

    if ( slave_tmp != us_time_low )
    {
        slave_tmp  = us_time_low;
        master_tmp = tim17_base.Instance->CNT;
    }

    lo_t = (( slave_tmp << 16 ) | master_tmp );

    *tv  = ((Bsp_Time) hi_t << 32 );
    *tv |= (Bsp_Time) lo_t;
}

void tim17_overflow_irq_hdl( void )
{
    HAL_TIM_IRQHandler( &tim17_base );
}

void bsp_wait( Bsp_Time time, Bsp_Time_Base base )
{
    Bsp_Time start_time = 0;
    Bsp_Time act_time   = 0;
    Bsp_Time delay      = 0;

    time = time * base;
    bsp_get_time( &start_time );
    do
    {
        bsp_get_time( &act_time );
        delay = act_time - start_time;
    } while ( delay < time );
}

void bsp_set_timeout( Bsp_Time      time
                    , Bsp_Time_Base base
                    , Bsp_Time*     timeout
                    )
{
    Bsp_Time start_time;

    /* Assure usage of valid pointer only */
    if ( timeout != NULL )
    {
        start_time = (Bsp_Time) 0;

        bsp_get_time( &start_time );
        *timeout = start_time + ( time * base );
    }
}

bool_t bsp_is_timeout( Bsp_Time timeout )
{
    Bsp_Time act_time;
    bool_t   ret;

    ret      = FALSE;
    act_time = (Bsp_Time) 0;

    bsp_get_time( &act_time );

    if ( act_time >= timeout )
    {
        ret = TRUE;
    }

    return ret;
}

/* This function is implemented here on purpose!!
 * It is inkoved by HAL_TIM_IRQHandler()
 */
void HAL_TIM_PeriodElapsedCallback( TIM_HandleTypeDef* tmr )
{
    if ( TIM17 == tmr->Instance )
    {
        us_time_low++;

        if ( 0xFFFF == us_time_low )
        {
            us_time_low_overflow = TRUE;
        }

        if (( 0 == us_time_low ) && ( FALSE != us_time_low_overflow ))
        {
            us_time_high++;
            us_time_low_overflow = FALSE;
        }
    }
}

/* This function is implemented here on purpose!!
 * It is inkoved by HAL_TIM_Base_Init()
 */
void HAL_TIM_Base_MspInit( TIM_HandleTypeDef* htim_base )
{
    if ( TIM17 == htim_base->Instance )
    {
        __HAL_RCC_TIM17_CLK_ENABLE();
        HAL_NVIC_SetPriority( TIM17_IRQn, 1, 0 );
        HAL_NVIC_EnableIRQ( TIM17_IRQn );
    }
}
