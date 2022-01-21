/**
 ** Name
 **   buzzer.c
 **
 ** Purpose
 **   Buzzer routines
 **
 ** Revision
 **   28-Nov-2020 (SSB) [] Initial
 **/

#include "buzzer.h"

#include "time.h"

#include <stm32g0xx_hal.h>

#define BUZZ_TOGGLE_TIME_MS (50)

static bool_t   toggle_active = FALSE;
static uint8_t  toggle_state  = 0;
static uint16_t toggle_ticks  = BUZZ_TOGGLE_TIME_MS;

void buzzer_init( void )
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();

    buzzer_off();

    gpio.Pin   = GPIO_PIN_3;
    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Pull  = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init( GPIOB, &gpio );

    buzzer_on();
    bsp_wait( 100, BSP_TIME_MSEC );
    buzzer_off();
}

void buzzer_off( void )
{
    HAL_GPIO_WritePin( GPIOB, GPIO_PIN_3, GPIO_PIN_RESET );
}

void buzzer_on( void )
{
    HAL_GPIO_WritePin( GPIOB, GPIO_PIN_3, GPIO_PIN_SET );
}

void buzzer_set_toggle( bool_t state )
{
    if ( FALSE != state )
    {
        toggle_active = TRUE;
        toggle_state  = TRUE;
        buzzer_on();
    }
    else
    {
        toggle_active = FALSE;
        toggle_state  = FALSE;
        buzzer_off();
    }
}

void buzzer_irq_hdl( void )
{
    if ( FALSE != toggle_active )
    {
        toggle_ticks--;

        if ( 0 == toggle_ticks )
        {
            toggle_state ^= 1;
            toggle_ticks  = BUZZ_TOGGLE_TIME_MS;

            if ( 0 != toggle_state )
            {
                buzzer_on();
            }
            else
            {
                buzzer_off();
            }
        }
    }
}
