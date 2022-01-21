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
#include "flash.h"
#include "menu.h"
#include "ptypes.h"
#include "state.h"
#include "system_init.h"
#include "time.h"
#include "utils.h"
#include "vmon.h"

#include <lcd2wire.h>
#include <max31850.h>

#define LCD_OUT_BUFF_SIZE   (20)
#define VMON_UPDATE_RATE_MS (1000)

static bool_t   vmon_active                 = FALSE;
static uint8_t  lcd_buff[LCD_OUT_BUFF_SIZE] = {0};
static Bsp_Time timeout_vmon;

static void critical_error_handler( void )
{
    fan_stop();

    lcd_clear();
    lcd_puts_xy((uint8_t*) "Critical error!", 0, 0 );
    lcd_puts_xy((uint8_t*) "Contact support at", 0, 2 );
    lcd_puts_xy((uint8_t*) "info@burgiclab.com", 0, 3 );

    for (;;)
    {
    }
}

static void disp_print_vmon( float voltage )
{
    utils_float_to_char( voltage, lcd_buff );
    lcd_puts_xy((uint8_t*) "V:", 10, 0 );
    lcd_puts_xy_cl( lcd_buff, 12, 0 );
}

static void disp_print_fan_load( uint8_t load )
{
    uint8_t i = 0;

    utils_itoa( load, lcd_buff, 3 );
    i += lcd_puts_xy((uint8_t*) "Fan:", 0, 0 );
    i += lcd_puts_xy( lcd_buff, i, 0 );
    i += lcd_puts_xy((uint8_t*) "%", i, 0 );

    /* Clear last digit in case it is a leftover from the previous value. */
    lcd_puts_xy((uint8_t*) " ", i, 0 );
}

static void disp_print_temperature( Max31850_Hdl_t max, uint16_t idx )
{
    uint8_t i      = 0;
    uint8_t x_axis = 2;
    float   fahrenheit;

    max31850_temp_to_string( lcd_buff, max->last_temp_raw[idx] );

    if ( 1 == idx )
    {
        i += lcd_puts_xy((uint8_t*) "TS:", 0, x_axis );
    }
    else
    {
        x_axis = 3;
        i += lcd_puts_xy((uint8_t*) "TM:", 0, x_axis );
    }

     i += lcd_puts_xy( lcd_buff, i, x_axis );
     i += lcd_puts_xy((uint8_t*) "C/", i, x_axis );

    fahrenheit = utils_float_cels_to_fahr
                            ((float)((float) max->last_temp_raw[idx] / 16.0 ));

    utils_float_to_char( fahrenheit, lcd_buff );
    i += lcd_puts_xy( lcd_buff, i, x_axis );
    lcd_puts_xy((uint8_t*) "F", i, x_axis );
}

static void disp_print_state( s_ctrl_state_t state )
{
    uint16_t i = 0;

    i += lcd_puts_xy((uint8_t*) "State:", 0, 1 );

    switch ( state )
    {
        case S_CTRL_STATE_IDLE:
            lcd_puts_xy_cl((uint8_t*) "Idle", i, 1 );
            break;
        case S_CTRL_STATE_PREHEAT_ACTIVE:
            lcd_puts_xy_cl((uint8_t*) "Preheating", i, 1 );
            break;
        case S_CTRL_STATE_HEAT_ACTIVE:
            lcd_puts_xy_cl((uint8_t*) "Heating", i, 1 );
            break;
        default:
            break;
    }
}

static void signal_alarm( s_ctrl_state_t state )
{
    lcd_clear();
    buzzer_set_toggle( TRUE );

    if ( S_CTRL_STATE_PREHEAT_DONE == state )
    {
        lcd_puts_xy((uint8_t*) "Preheating done!", 3, 2 );
    }
    else
    {
        lcd_puts_xy((uint8_t*) "All done!", 3, 2 );
    }

    lcd_puts_xy((uint8_t*) "Press knob to", 3, 2 );
    lcd_puts_xy((uint8_t*) "continue...", 4, 3 );
}

int main( void )
{
    status_t       ret;
    bool_t         bret;
    float          voltage;
    Enc_Hdl_t      enc_hdl = NULL;
    Max31850_Hdl_t max_hdl = NULL;
    uint16_t       tm_idx;
    uint16_t       ts_idx;
    uint8_t        fan_load     = 0;
    s_ctrl_state_t s_ctrl_state = S_CTRL_STATE_IDLE;

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

    enc_hdl = enc_get_hdl();
    {
        if ( NULL == enc_hdl )

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
        bsp_wait( 1, BSP_TIME_SEC );
    }
    else
    {
        vmon_active = TRUE;
    }

    lcd_clear();

    /* Reset encoder button if pressed during the startup. */
    if ( FALSE != enc_hdl->pb_pressed )
    {
        enc_hdl->pb_pressed = FALSE;
    }

    max_hdl = max31850_get_hdl();

    if ( NULL != max_hdl )
    {
        ret = max31850_init( GPIOA, GPIO_PIN_12 );
    }

    if ( STATUS_OK != ret )
    {
        critical_error_handler();
    }
    else
    {
        ts_idx = max31850_get_dev_idx( 0x01 );
        tm_idx = max31850_get_dev_idx( 0x00 );

        if (( MAX31850_SENSOR_ERROR == ts_idx )
         || ( MAX31850_SENSOR_ERROR == tm_idx ))
         {
            lcd_puts_xy((uint8_t*) "MAX31850 error!", 0, 0 );
            bsp_wait( 1, BSP_TIME_SEC );
            critical_error_handler();
         }
    }

    fan_init();

    for (;;)
    {
        bret = bsp_is_timeout( timeout_vmon );

        if ( FALSE != bret )
        {
            if ( FALSE != vmon_active )
            {
                ret = vmon_get_voltage( &voltage );

                if ( STATUS_OK == ret )
                {
                    disp_print_vmon( voltage );
                    bsp_set_timeout( VMON_UPDATE_RATE_MS
                                   , BSP_TIME_MSEC
                                   , &timeout_vmon
                                   );
                }
                else
                {
                    vmon_active = FALSE;

                    /* Clear last entry. */
                    lcd_puts_xy_cl((uint8_t*) " ", 13, 0 );
                }
            }

            disp_print_fan_load( fan_load );
        }

        bret = max31850_update();
        if ( FALSE != bret )
        {
            disp_print_temperature( max_hdl, ts_idx );
            disp_print_temperature( max_hdl, tm_idx );
        }

        if ( FALSE != enc_hdl->pb_pressed )
        {
            enc_hdl->pb_pressed = FALSE;
            menu_open();
            s_ctrl_state = state_get();
        }

        if (( S_CTRL_STATE_PREHEAT_DONE == s_ctrl_state )
         || ( S_CTRL_STATE_HEAD_DONE == s_ctrl_state ))
        {
            signal_alarm( s_ctrl_state );

            while ( FALSE == enc_hdl->pb_pressed );
            enc_hdl->pb_pressed = FALSE;
            buzzer_set_toggle( FALSE );

            if ( S_CTRL_STATE_PREHEAT_DONE == s_ctrl_state )
            {
                state_set( S_CTRL_STATE_HEAT_ACTIVE );
            }
            else
            {
                state_set( S_CTRL_STATE_IDLE );
            }
        }

        disp_print_state( s_ctrl_state );
    }

#ifndef __ICCARM__
    return 0;
#endif
}
