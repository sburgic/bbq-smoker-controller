/**
 ** Name
 **   fan.c
 **
 ** Purpose
 **   Fan control rouitnes
 **
 ** Revision
 **   28-Nov-2021 (SSB) [] Initial
 **/

#include "fan.h"

#include <stm32g0xx_hal.h>

/*
 * TIM1 Channel1 duty cycle = (TIM1_CCR1/ TIM1_ARR + 1)* 100 = 50%.
 * To get TIM1 counter clock at SystemCoreClock, the prescaler is set to 0.
 * To get TIM1 output clock at 24 KHz, the period (ARR)) is computed as follows:
 *
 *     ARR = (TIM1 counter clock / TIM1 output clock) - 1
 *         = 332
 *
 * TIM1 Channel1 duty cycle = (TIM1_CCR1/ TIM1_ARR + 1)* 100 = 50%
 */

#define PRESCALER_VALUE (uint32_t)(((SystemCoreClock) / 64000000) - 1)
#define PERIOD_VALUE    (uint32_t)( 333 - 1 )
#define PULSE1_VALUE    (uint32_t)( PERIOD_VALUE/2 )

TIM_HandleTypeDef tim1;

status_t fan_init( void )
{
    status_t          ret = STATUS_ERROR;
    HAL_StatusTypeDef hret;

    TIM_MasterConfigTypeDef        master_cfg = {0};
    TIM_OC_InitTypeDef             oc_cfg     = {0};
    TIM_BreakDeadTimeConfigTypeDef bdt_cfg    = {0};
    GPIO_InitTypeDef               gpio       = {0};

    tim1.Instance               = TIM1;
    tim1.Init.Prescaler         = PRESCALER_VALUE;
    tim1.Init.CounterMode       = TIM_COUNTERMODE_UP;
    tim1.Init.Period            = PERIOD_VALUE;
    tim1.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    tim1.Init.RepetitionCounter = 0;
    tim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    hret = HAL_TIM_PWM_Init( &tim1 );

    if ( HAL_OK == hret )
    {
        master_cfg.MasterOutputTrigger  = TIM_TRGO_RESET;
        master_cfg.MasterOutputTrigger2 = TIM_TRGO2_RESET;
        master_cfg.MasterSlaveMode      = TIM_MASTERSLAVEMODE_DISABLE;

        hret = HAL_TIMEx_MasterConfigSynchronization( &tim1, &master_cfg );
    }

    if ( HAL_OK == hret )
    {
        oc_cfg.OCMode       = TIM_OCMODE_PWM1;
        oc_cfg.Pulse        = PULSE1_VALUE;
        oc_cfg.OCPolarity   = TIM_OCPOLARITY_HIGH;
        oc_cfg.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
        oc_cfg.OCFastMode   = TIM_OCFAST_DISABLE;
        oc_cfg.OCIdleState  = TIM_OCIDLESTATE_RESET;
        oc_cfg.OCNIdleState = TIM_OCNIDLESTATE_RESET;

        hret = HAL_TIM_PWM_ConfigChannel( &tim1, &oc_cfg, TIM_CHANNEL_1 );
    }

    if ( HAL_OK == hret )
    {
        bdt_cfg.OffStateRunMode  = TIM_OSSR_DISABLE;
        bdt_cfg.OffStateIDLEMode = TIM_OSSI_DISABLE;
        bdt_cfg.LockLevel        = TIM_LOCKLEVEL_OFF;
        bdt_cfg.DeadTime         = 0;
        bdt_cfg.BreakState       = TIM_BREAK_DISABLE;
        bdt_cfg.BreakPolarity    = TIM_BREAKPOLARITY_HIGH;
        bdt_cfg.BreakFilter      = 0;
        bdt_cfg.BreakAFMode      = TIM_BREAK_AFMODE_INPUT;
        bdt_cfg.Break2State      = TIM_BREAK2_DISABLE;
        bdt_cfg.Break2Polarity   = TIM_BREAK2POLARITY_HIGH;
        bdt_cfg.Break2Filter     = 0;
        bdt_cfg.Break2AFMode     = TIM_BREAK_AFMODE_INPUT;
        bdt_cfg.AutomaticOutput  = TIM_AUTOMATICOUTPUT_DISABLE;

        hret = HAL_TIMEx_ConfigBreakDeadTime( &tim1, &bdt_cfg );
    }

    if ( HAL_OK == hret )
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();

        gpio.Pin       = GPIO_PIN_8;
        gpio.Mode      = GPIO_MODE_AF_PP;
        gpio.Pull      = GPIO_PULLDOWN;
        gpio.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
        gpio.Alternate = GPIO_AF2_TIM1;
        HAL_GPIO_Init( GPIOA, &gpio );

        ret = STATUS_OK;
    }

    return ret;
}

status_t fan_start( void )
{
    status_t          ret = STATUS_ERROR;
    HAL_StatusTypeDef hret;

    hret = HAL_TIM_PWM_Start( &tim1, TIM_CHANNEL_1 );

    if ( HAL_OK == hret )
    {
        ret = STATUS_OK;
    }

    return ret;
}

/* This function is implemented here on purpose!!
 * It is inkoved by HAL_TIM_PWM_Init()
 */
void HAL_TIM_PWM_MspInit( TIM_HandleTypeDef* tim )
{
    if ( TIM1 == tim->Instance )
    {
        __HAL_RCC_TIM1_CLK_ENABLE();
    }
}
