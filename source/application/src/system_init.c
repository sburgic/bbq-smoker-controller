/**
 ** Name
 **   system_init.c
 **
 ** Purpose
 **   System configuration
 **
 ** Revision
 **   28-Nov-2020 (SSB) [] Initial
 **/

#include "system_init.h"

void system_clk_cfg( void )
{
    RCC_ClkInitTypeDef       clkinitstruct    = {0};
    RCC_OscInitTypeDef       oscinitstruct    = {0};
    RCC_PeriphCLKInitTypeDef perclkinitstruct = {0};

    /* Configure the main internal regulator output voltage */
    HAL_PWREx_ControlVoltageScaling( PWR_REGULATOR_VOLTAGE_SCALE1 );

    /* Enable HSE Oscillator and activate PLL with HSE as source */
    oscinitstruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
    oscinitstruct.HSIState            = RCC_HSI_ON;
    oscinitstruct.HSIDiv              = RCC_HSI_DIV1;
    oscinitstruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    oscinitstruct.PLL.PLLState        = RCC_PLL_ON;
    oscinitstruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
    oscinitstruct.PLL.PLLM            = RCC_PLLM_DIV1;
    oscinitstruct.PLL.PLLN            = 20;
    oscinitstruct.PLL.PLLP            = RCC_PLLP_DIV2;
    oscinitstruct.PLL.PLLR            = RCC_PLLR_DIV5;

    if ( HAL_OK != HAL_RCC_OscConfig( &oscinitstruct ))
    {
        for (;;)
        {
        }
    }

    clkinitstruct.ClockType      = ( RCC_CLOCKTYPE_SYSCLK  \
                                   | RCC_CLOCKTYPE_HCLK    \
                                   | RCC_CLOCKTYPE_PCLK1   \
                                   );
    clkinitstruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    clkinitstruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV1;

    if ( HAL_OK != HAL_RCC_ClockConfig( &clkinitstruct, FLASH_LATENCY_2 ))
    {
        for (;;)
        {
        }
    }

    perclkinitstruct.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    perclkinitstruct.AdcClockSelection    = RCC_ADCCLKSOURCE_SYSCLK;

    if ( HAL_OK != HAL_RCCEx_PeriphCLKConfig( &perclkinitstruct ))
    {
        for (;;)
        {
        }
    }
}

void HAL_MspInit( void )
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
}
