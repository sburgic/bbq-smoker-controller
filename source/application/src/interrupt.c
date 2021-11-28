/**
 ** Name
 **   interrupt.c
 **
 ** Purpose
 **   Interrupt routines
 **
 ** Revision
 **   28-Nov-2020 (SSB) [] Initial
 **/

#include "interrupt.h"

#include "encoder.h"
#include "time.h"

#include <stm32g0xx_hal.h>

void NMI_Handler( void )
{
    for (;;)
    {
    }
}

void HardFault_Handler( void )
{
    for (;;)
    {
    }
}

void SVC_Handler( void )
{
}

void PendSV_Handler (void )
{
}

void SysTick_Handler( void )
{
  HAL_IncTick();
  enc_sw_irq_hdl();
  enc_irq_hdl();
}

void TIM17_IRQHandler( void )
{
    tim17_overflow_irq_hdl();
}
