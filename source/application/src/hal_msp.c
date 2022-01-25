/**
 ** Name
 **   hal_msp.c
 **
 ** Purpose
 **   STM3G030xx HAL MSP
 **
 ** Revision
 **   22-Jan-2022 (SSB) [] Initial
 **/

#include <stm32g0xx_hal.h>

void HAL_UART_MspInit( UART_HandleTypeDef* huart )
{
    GPIO_InitTypeDef gpio = {0};

    if ( USART2 == huart->Instance )
    {
        __HAL_RCC_USART2_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        gpio.Pin       = GPIO_PIN_2 | GPIO_PIN_3;
        gpio.Mode      = GPIO_MODE_AF_PP;
        gpio.Pull      = GPIO_NOPULL;
        gpio.Speed     = GPIO_SPEED_FREQ_LOW;
        gpio.Alternate = GPIO_AF1_USART2;
        HAL_GPIO_Init( GPIOA, &gpio );

        HAL_NVIC_SetPriority( USART2_IRQn, 2, 1 );
        HAL_NVIC_EnableIRQ( USART2_IRQn );
        HAL_NVIC_ClearPendingIRQ( USART2_IRQn );
    }
}
