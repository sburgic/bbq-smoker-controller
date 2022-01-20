/**
 ** Name
 **   vmon.c
 **
 ** Purpose
 **   Voltage monitoring
 **
 ** Revision
 **   28-Nov-2020 (SSB) [] Initial
 **/

#include "vmon.h"

#include <stm32g0xx_hal.h>

#define VMON_ADC_NO_OF_SAMPLES (200)
#define VMON_ADC_MAX_RETRIES   (5)
#define VMON_ADC_TIMEOUT_MS    (1)

static ADC_HandleTypeDef adc1;
static const float       offset = -0.33; /* TODO: Store offset in flash. */

status_t vmon_init( void )
{
    status_t               ret = STATUS_ERROR;
    HAL_StatusTypeDef      hret;
    ADC_ChannelConfTypeDef ch_cfg = {0};

    adc1.Instance = ADC1;
    adc1.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV2;
    adc1.Init.Resolution            = ADC_RESOLUTION_12B;
    adc1.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    adc1.Init.ScanConvMode          = ADC_SCAN_DISABLE;
    adc1.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;
    adc1.Init.LowPowerAutoWait      = DISABLE;
    adc1.Init.LowPowerAutoPowerOff  = DISABLE;
    adc1.Init.ContinuousConvMode    = DISABLE;
    adc1.Init.NbrOfConversion       = 1;
    adc1.Init.DiscontinuousConvMode = DISABLE;
    adc1.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
    adc1.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adc1.Init.DMAContinuousRequests = DISABLE;
    adc1.Init.Overrun               = ADC_OVR_DATA_PRESERVED;
    adc1.Init.SamplingTimeCommon1   = ADC_SAMPLETIME_1CYCLE_5;
    adc1.Init.SamplingTimeCommon2   = ADC_SAMPLETIME_1CYCLE_5;
    adc1.Init.OversamplingMode      = DISABLE;
    adc1.Init.TriggerFrequencyMode  = ADC_TRIGGER_FREQ_HIGH;

    hret = HAL_ADC_Init( &adc1 );

    if ( HAL_OK == hret )
    {
        ch_cfg.Channel      = ADC_CHANNEL_11;
        ch_cfg.Rank         = ADC_REGULAR_RANK_1;
        ch_cfg.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;

        hret = HAL_ADC_ConfigChannel( &adc1, &ch_cfg );
    }

    if ( HAL_OK == hret )
    {
        ret = STATUS_OK;
    }

    return ret;
}

status_t vmon_get_voltage( float* voltage )
{
    status_t          ret  = STATUS_ERROR;
    HAL_StatusTypeDef hret = HAL_ERROR;
    uint16_t          i;
    uint32_t          sum   = 0;
    uint8_t           retry;

    for ( i = 0; i < VMON_ADC_NO_OF_SAMPLES; i++ )
    {
        hret = HAL_ADC_Start( &adc1 );

        if ( HAL_OK != hret )
        {
            break;
        }
        else
        {
            retry = 0;

            do
            {
                hret = HAL_ADC_PollForConversion( &adc1, VMON_ADC_TIMEOUT_MS );
                retry++;
            } while (( HAL_OK != hret ) && ( retry < VMON_ADC_MAX_RETRIES ));

            if ( HAL_OK != hret )
            {
                break;
            }
            else
            {
                sum += HAL_ADC_GetValue( &adc1 );
            }
        }
    }

    if ( HAL_OK == hret )
    {
        sum     /= VMON_ADC_NO_OF_SAMPLES;
        *voltage = (((float) sum / 4096.0 ) * 3.3 ) * 5.7;
        *voltage += offset;

        ret = STATUS_OK;
    }

    return ret;
}

/* This function is implemented here on purpose!!
 * It is inkoved by HAL_ADC_Init()
 */
void HAL_ADC_MspInit( ADC_HandleTypeDef* adc )
{
    GPIO_InitTypeDef gpio = {0};

    if( ADC1 == adc->Instance )
    {
        __HAL_RCC_ADC_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        gpio.Pin  = GPIO_PIN_7;
        gpio.Mode = GPIO_MODE_ANALOG;
        gpio.Pull = GPIO_NOPULL;
        HAL_GPIO_Init( GPIOB, &gpio );
    }
}
