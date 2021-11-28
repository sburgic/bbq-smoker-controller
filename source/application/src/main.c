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
#include "encoder.h"
#include "fan.h"
#include "lcd2wire.h"
#include "ptypes.h"
#include "system_init.h"
#include "time.h"
#include "utils.h"
#include "vmon.h"

#define LCD_OUT_BUFF_SIZE (20)

static bool_t vmon_active = FALSE;

static void critical_error_handler( void )
{
    lcd_puts_xy((uint8_t*) "Critical error!", 0, 0 );
    lcd_puts_xy((uint8_t*) "Contact support at", 0, 2 );
    lcd_puts_xy((uint8_t*) "info@burgiclab.com", 0, 3 );

    for (;;)
    {
    }
}

int main( void )
{
    status_t ret;
    uint8_t  out[LCD_OUT_BUFF_SIZE] = {0};
    float    voltage;
    Enc_t*   enc = NULL;

    HAL_Init();
    system_clk_cfg();
    bsp_tmr_init();
    buzzer_init();
    enc_init();
    lcd_init();

    lcd_puts_xy( APP_WELCOME_TEXT_1, 3, 0 );
    lcd_puts_xy( APP_WELCOME_TEXT_2, 1, 1 );
    lcd_puts_xy( APP_WELCOME_TEXT_3, 1, 2 );
    lcd_puts_xy( APP_SW_VERSION, 4, 3 );
    bsp_wait( 2, BSP_TIME_SEC );
    lcd_clear();

    enc = enc_get_hdl();
    {
        if ( NULL == enc )

        {
            critical_error_handler();
        }
    }

    ret = fan_init();
    if ( STATUS_OK != ret )
    {
        critical_error_handler();
    }

    ret = vmon_init();
    if ( STATUS_OK != ret )
    {
        lcd_puts_xy((uint8_t*) "VMON error!", 0, 0 );
    }
    else
    {
        vmon_active = TRUE;
    }

    bsp_wait( 1, BSP_TIME_SEC );
    lcd_clear();

    /* Reset encoder button if pressed during startup */
    if ( FALSE != enc->pb_pressed )
    {
        enc->pb_pressed = FALSE;
    }

    for (;;)
    {
        if ( FALSE != vmon_active )
        {
            ret = vmon_get_voltage( &voltage );

            if ( STATUS_OK == ret )
            {
                utils_float_to_char( voltage, out );
                lcd_puts_xy((uint8_t*) "V:", 13, 0 );
                lcd_puts_xy_cl( out, 15, 0 );
            }
            else
            {
                vmon_active = FALSE;
            }
        }
    }

#ifndef __ICCARM__
    return 0;
#endif
}
