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
