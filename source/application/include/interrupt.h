/**
 ** Name
 **   interrupt.h
 **
 ** Purpose
 **   Interrupt routines
 **
 ** Revision
 **   28-Nov-2020 (SSB) [] Initial
 **/

#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <stm32g0xx_hal.h>

void NMI_Handler( void );
void HardFault_Handler( void );
void SVC_Handler( void );
void PendSV_Handler( void );
void MemManage_Handler( void );
void BusFault_Handler( void );
void UsageFault_Handler( void );
void DebugMon_Handler( void );
void SysTick_Handler( void );
void TIM17_IRQHandler( void );

#endif /*__INTERRUPT_H__ */
