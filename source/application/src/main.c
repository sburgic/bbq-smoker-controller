/**
 ** Name
 **   main.c
 **
 ** Purpose
 **   Main application
 **
 ** Revision
 **   28-Nov-2021 (SSB) [] Initial
 **/

#include "main.h"

#include "buzzer.h"
#include "lcd2wire.h"
#include "ptypes.h"
#include "time.h"
#include "system_init.h"

int main( void )
{
    HAL_Init();
    system_clk_cfg();
    bsp_tmr_init();
    buzzer_init();
    lcd_init();

    lcd_puts_xy( APP_WELCOME_TEXT_1, 3, 0 );
    lcd_puts_xy( APP_WELCOME_TEXT_2, 1, 1 );
    lcd_puts_xy( APP_WELCOME_TEXT_3, 1, 2 );
    lcd_puts_xy( APP_SW_VERSION, 4, 3 );
    bsp_wait( 2, BSP_TIME_SEC );
    lcd_clear();

    for (;;)
    {
    }

#ifndef __ICCARM__
    return 0;
#endif
}
