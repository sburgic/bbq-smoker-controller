/**
 ** Name
 **   menu.c
 **
 ** Purpose
 **   Menu
 **
 ** Revision
 **   19-Jan-2022 (SSB) [] Initial
 **/

#include "menu.h"

#include "configuration.h"
#include "encoder.h"
#include "fan.h"
#include "flash.h"
#include "state.h"
#include "time.h"
#include "utils.h"

#include <lcd2wire.h>
#include <max31850.h>

static Enc_Hdl_t         enc;
static config_t*         cfg;
static Max31850_Hdl_t    max;
static uint8_t           enc_row_idx = 0;
static bool_t            menu_done   = FALSE;
static menu_page_idx_t   active_page = MENU_PAGE_MAIN;
static bool_t            trans       = FALSE;

/*
 * Main page menu callbacks.
 */

static void menu_main_start_cb( void );
static void menu_main_get_temperatures_cb( void );
static void menu_main_set_temperatures_cb( void );
static void menu_main_change_state_cb( void );
static void menu_main_fan_settings_cb( void );
static void menu_main_bt_info_cb( void );
static void menu_main_save_to_flash_cb( void );
static void menu_main_exit_cb( void );

/*
 * Set temperature page menu callbacks.
 */

static void menu_set_temp_ts1_cb( void );
static void menu_set_temp_ts2_cb( void );
static void menu_set_temp_tm1_cb( void );
static void menu_set_temp_tm2_cb( void );
static void menu_set_temp_back_cb( void );

/*
 * Change state page menu callbacks.
 */

static void menu_ch_state_idle_cb( void );
static void menu_ch_state_preheat_cb( void );
static void menu_ch_state_heat_cb( void );
static void menu_ch_state_back_cb( void );

/*
 * Save to flash page menu callbacks.
 */

static void menu_main_save_to_flash_yes_cb( void );
static void menu_main_save_to_flash_no_cb( void );

static menu_entry_t menu_page_main[] =
{
    { (uint8_t*) "Start", menu_main_start_cb },
    { (uint8_t*) "Get temperatures", menu_main_get_temperatures_cb },
    { (uint8_t*) "Set temperatures", menu_main_set_temperatures_cb },
    { (uint8_t*) "Change state", menu_main_change_state_cb },
    { (uint8_t*) "Fan settings", menu_main_fan_settings_cb },
    { (uint8_t*) "Bluetooth info", menu_main_bt_info_cb },
    { (uint8_t*) "Save to flash", menu_main_save_to_flash_cb },
    { (uint8_t*) "Exit menu", menu_main_exit_cb }
};

static menu_entry_t menu_page_set_temperatures[] =
{
    { (uint8_t*) "Smoke temp. phase 1", menu_set_temp_ts1_cb },
    { (uint8_t*) "Smoke temp. phase 2", menu_set_temp_ts2_cb },
    { (uint8_t*) "Meat  temp. phase 1", menu_set_temp_tm1_cb },
    { (uint8_t*) "Meat  temp. phase 2", menu_set_temp_tm2_cb },
    { (uint8_t*) "Back", menu_set_temp_back_cb }
};

static menu_entry_t menu_page_change_state[] =
{
    { (uint8_t*) "Goto IDLE", menu_ch_state_idle_cb },
    { (uint8_t*) "Goto PREHEAT", menu_ch_state_preheat_cb },
    { (uint8_t*) "Goto HEAT", menu_ch_state_heat_cb },
    { (uint8_t*) "Back", menu_ch_state_back_cb }
};

static menu_entry_t menu_page_save_to_flash[] =
{
    { (uint8_t*) "Yes", menu_main_save_to_flash_yes_cb },
    { (uint8_t*) "No", menu_main_save_to_flash_no_cb }
};

static menu_page_t menu_page[] =
{
    {
        &menu_page_main[0],
        0,
        sizeof(menu_page_main) /
        sizeof(menu_page_main[0])
    },
    {
        &menu_page_set_temperatures[0],
        0,
        sizeof(menu_page_set_temperatures) /
        sizeof(menu_page_set_temperatures[0])
    },
    {
        &menu_page_change_state[0],
        0,
        sizeof(menu_page_change_state) /
        sizeof(menu_page_change_state[0])
    },
    {
        &menu_page_save_to_flash[0],
        0,
        sizeof(menu_page_save_to_flash) /
        sizeof(menu_page_save_to_flash[0])
    }
};

static void menu_change_active_page( menu_page_idx_t page )
{
    active_page                     = page;
    enc_row_idx                     = ( menu_page[active_page].curr_pos
                                    % MENU_NO_OF_ROWS );
    trans                           = TRUE;
}

static void menu_dump_temperature( bool_t  clear
                                 , bool_t  name
                                 , uint8_t idx
                                 , uint8_t row
                                 )
{
    int16_t temp;
    uint8_t buff[3] = {0};
    uint8_t i;
    float   fahrenheit;

    if ( FALSE != clear )
    {
        lcd_clear();
    }

    if ( FALSE != name )
    {
        if ( 0 == idx )
        {
            lcd_puts_xy((uint8_t*) "TS1:", 0, row );
        }
        else if ( 1 == idx )
        {
            lcd_puts_xy((uint8_t*) "TS2:", 0, row );
        }
        else if ( 2 == idx )
        {
            lcd_puts_xy((uint8_t*) "TM1:", 0, row );
        }
        else
        {
            lcd_puts_xy((uint8_t*) "TM2:", 0, row );
        }
    }

    if ( 0 == idx )
    {
        temp = cfg->ts_phase_1;
    }
    else if ( 1 == idx )
    {
        temp = cfg->ts_phase_2;
    }
    else if ( 2 == idx )
    {
        temp = cfg->tm_phase_1;
    }
    else
    {
        temp = cfg->tm_phase_2;
    }

    i = 4;

    utils_itoa((uint32_t) temp, buff, 3 );
    i += lcd_puts_xy( buff, i, row );
    i += lcd_puts_xy((uint8_t*) "C/", i, row );
    fahrenheit = utils_float_cels_to_fahr((float) temp );
    utils_itoa((uint32_t) fahrenheit, buff, 3 );
    i += lcd_puts_xy( buff, i, row );
    lcd_puts_xy((uint8_t*) "F ", i, row );
}

static void menu_dump_all_temperatures( bool_t clear, bool_t names )
{
    uint8_t i;

    if ( FALSE != clear )
    {
        lcd_clear();
    }

    for ( i = 0; i < 4; i++ )
    {
        menu_dump_temperature( FALSE, TRUE, i, i );
    }
}

static void menu_main_start_cb( void )
{
    state_set( S_CTRL_STATE_PREHEAT_ACTIVE );
    menu_done = TRUE;
}

static void menu_main_get_temperatures_cb( void )
{
    menu_dump_all_temperatures( TRUE, TRUE );

    while ( FALSE == enc->pb_pressed );

    enc->pb_pressed = FALSE;

    /* In case encoder was rotating for any reason, clear the rotation. */
    enc->updated = FALSE;
}

static void menu_main_set_temperatures_cb( void )
{
    menu_change_active_page( MENU_PAGE_SET_TEMPERATURE );
}

static void menu_main_change_state_cb( void )
{
    menu_change_active_page( MENU_PAGE_CHANGE_STATE );
}

static void menu_main_fan_settings_cb( void )
{
}

static void menu_main_bt_info_cb( void )
{
    uint8_t i;
    uint8_t buff[CONFIG_BT_PIN_SIZE] = {0};

    lcd_clear();
    lcd_puts_xy((uint8_t*) "Bluetooth name:", 0, 0 );
    lcd_puts_xy((uint8_t*) &cfg->bt_name[0], 0, 1 );
    lcd_puts_xy((uint8_t*) "Bluetooth pin:", 0, 2 );

    for ( i = 0; i < CONFIG_BT_PIN_SIZE; i++ )
    {
        utils_itoa( cfg->bt_pin[i], &buff[i], 1 );
    }

    lcd_puts_xy( buff, 0, 3 );

    while ( FALSE == enc->pb_pressed );
    enc->pb_pressed = FALSE;

    /* In case encoder was rotating for any reason, clear the rotation. */
    enc->updated = FALSE;
}

static void menu_main_save_to_flash_cb( void )
{
    menu_change_active_page( MENU_PAGE_SAVE_TO_FLASH );
}

static void menu_main_exit_cb( void )
{
    menu_done = TRUE;
}

static void menu_set_temp_ts1_cb( void )
{
    menu_dump_temperature( TRUE, TRUE, 0, 0 );

    do
    {
        if ( FALSE != enc->updated )
        {
            if ( ENC_DIRECTION_RIGHT == enc->direction )
            {
                if ( cfg->ts_phase_1 < 400 )
                {
                    cfg->ts_phase_1++;
                }
            }
            else
            {
                if ( cfg->ts_phase_1 > 40 )
                {
                    cfg->ts_phase_1--;
                }
            }

            menu_dump_temperature( FALSE, FALSE, 0, 0 );

            enc->updated = FALSE;
        }
    } while ( FALSE == enc->pb_pressed );

    enc->pb_pressed = FALSE;
}

static void menu_set_temp_ts2_cb( void )
{
    menu_dump_temperature( TRUE, TRUE, 1, 0 );

    do
    {
        if ( FALSE != enc->updated )
        {
            if ( ENC_DIRECTION_RIGHT == enc->direction )
            {
                if ( cfg->ts_phase_2 < 400 )
                {
                    cfg->ts_phase_2++;
                }
            }
            else
            {
                if ( cfg->ts_phase_2 > 40 )
                {
                    cfg->ts_phase_2--;
                }
            }

            menu_dump_temperature( FALSE, FALSE, 1, 0 );

            enc->updated = FALSE;
        }
    } while ( FALSE == enc->pb_pressed );

    enc->pb_pressed = FALSE;
}


static void menu_set_temp_tm1_cb( void )
{
    menu_dump_temperature( TRUE, TRUE, 2, 0 );

    do
    {
        if ( FALSE != enc->updated )
        {
            if ( ENC_DIRECTION_RIGHT == enc->direction )
            {
                if ( cfg->tm_phase_1 < 400 )
                {
                    cfg->tm_phase_1++;
                }
            }
            else
            {
                if ( cfg->tm_phase_1 > 40 )
                {
                    cfg->tm_phase_1--;
                }
            }

            menu_dump_temperature( FALSE, FALSE, 2, 0 );

            enc->updated = FALSE;
        }
    } while ( FALSE == enc->pb_pressed );

    enc->pb_pressed = FALSE;
}


static void menu_set_temp_tm2_cb( void )
{
    menu_dump_temperature( TRUE, TRUE, 3, 0 );

    do
    {
        if ( FALSE != enc->updated )
        {
            if ( ENC_DIRECTION_RIGHT == enc->direction )
            {
                if ( cfg->tm_phase_2 < 400 )
                {
                    cfg->tm_phase_2++;
                }
            }
            else
            {
                if ( cfg->tm_phase_2 > 40 )
                {
                    cfg->tm_phase_2--;
                }
            }

            menu_dump_temperature( FALSE, FALSE, 3, 0 );

            enc->updated = FALSE;
        }
    } while ( FALSE == enc->pb_pressed );

    enc->pb_pressed = FALSE;
}

static void menu_set_temp_back_cb( void )
{
    menu_page[MENU_PAGE_SET_TEMPERATURE].curr_pos = 0;
    menu_change_active_page( MENU_PAGE_MAIN );
}

static void menu_ch_state_idle_cb( void )
{
    state_set( S_CTRL_STATE_IDLE );
    menu_done = TRUE;
}

static void menu_ch_state_preheat_cb( void )
{
    state_set( S_CTRL_STATE_PREHEAT_ACTIVE );
    menu_done = TRUE;
}

static void menu_ch_state_heat_cb( void )
{
    state_set( S_CTRL_STATE_HEAT_ACTIVE );
    menu_done = TRUE;
}

static void menu_ch_state_back_cb( void )
{
    menu_page[MENU_PAGE_CHANGE_STATE].curr_pos = 0;
    menu_change_active_page( MENU_PAGE_MAIN );
}

static void menu_main_save_to_flash_yes_cb( void )
{
    status_t ret;

    lcd_clear();

    ret = config_store();

    if ( STATUS_OK == ret )
    {
        lcd_puts_xy((uint8_t*) "Configuration saved!", 0 , 0 );
    }
    else
    {
        lcd_puts_xy((uint8_t*) "Flash write failed!", 0 , 0 );
    }

    bsp_wait( 2, BSP_TIME_SEC );

    /* In case encoder was rotating for any reason, clear the rotation. */
    enc->updated = FALSE;

    /* In case button was pressed for any reason, clear the flag. */
    enc->pb_pressed = FALSE;

    menu_page[MENU_PAGE_SAVE_TO_FLASH].curr_pos = 0;
    menu_change_active_page( MENU_PAGE_MAIN );
}

static void menu_main_save_to_flash_no_cb( void )
{
    menu_page[MENU_PAGE_SAVE_TO_FLASH].curr_pos = 0;
    menu_change_active_page( MENU_PAGE_MAIN );
}

static void menu_print_cursor( uint8_t row )
{
    lcd_puts_xy( MENU_CURSOR_SIGN, MENU_CURSOR_POSITION, row );
}

static void menu_handle_cursor( void )
{
    if ( FALSE != enc->updated )
    {
        if ( ENC_DIRECTION_RIGHT == enc->direction )
        {
            if ( menu_page[active_page].curr_pos
             < ( menu_page[active_page].no_of_entries - 1 ))
            {
                menu_page[active_page].curr_pos++;
                enc_row_idx++;

                if ( enc_row_idx > ( MENU_NO_OF_ROWS - 1 ))
                {
                    enc_row_idx = ( MENU_NO_OF_ROWS - 1 );
                    trans       = TRUE;
                }
            }

            /* Clear last cursor position. */
            lcd_puts_xy((uint8_t*) " "
                       , MENU_CURSOR_POSITION
                       , enc_row_idx - 1
                       );
        }
        else if ( ENC_DIRECTION_LEFT == enc->direction )
        {
            if ( menu_page[active_page].curr_pos > 0 )
            {
                menu_page[active_page].curr_pos--;

                if ( enc_row_idx > 0 )
                {
                    enc_row_idx--;

                    /* Clear last cursor position. */
                    lcd_puts_xy((uint8_t*) " "
                            , MENU_CURSOR_POSITION
                            , enc_row_idx + 1
                            );
                }
                else
                {
                    trans = TRUE;
                }
            }
        }

        enc_row_idx = ( menu_page[active_page].curr_pos % MENU_NO_OF_ROWS );
        menu_print_cursor( enc_row_idx  );
        enc->updated = FALSE;
    }
}

static void menu_dump_entries( void )
{
    uint8_t i;
    uint8_t j;

    if ( menu_page[active_page].curr_pos > ( MENU_NO_OF_ROWS - 1 ))
    {
        j = ( menu_page[active_page].curr_pos
          - ( menu_page[active_page].curr_pos % MENU_NO_OF_ROWS ));
    }
    else
    {
        j = 0;
    }

    lcd_clear();

    for ( i = 0; i < MENU_NO_OF_ROWS; i++ )
    {
        if ( j < menu_page[active_page].no_of_entries )
        {
            if ( NULL != menu_page[active_page].entries[j].entry_name )
            {
                lcd_puts_xy( menu_page[active_page].entries[j].entry_name
                        , 0
                        , i
                        );
            }

            j++;
        }
    }

    menu_print_cursor( enc_row_idx );
}

static void menu_listen( void )
{
    uint8_t       cmd_idx;
    menu_entry_t* entry;

    menu_dump_entries();

    while ( FALSE == menu_done )
    {
        menu_handle_cursor();

        if ( FALSE != trans )
        {
            menu_dump_entries();
            trans = FALSE;
        }

        if ( FALSE != enc->pb_pressed )
        {
            cmd_idx = menu_page[active_page].curr_pos;
            entry   = &menu_page[active_page].entries[cmd_idx];

            enc->pb_pressed = FALSE;

            if ( NULL != entry->callback )
            {
                entry->callback();
                menu_dump_entries();
            }
        }
    }
}

void menu_open( void )
{
    enc = enc_get_hdl();
    cfg = config_get_hdl();
    max = max31850_get_hdl();

    if ((( NULL != enc ) && ( NULL != cfg )) && ( NULL != max ))
    {
        /* Clear encoder button pressed flag. */
        enc->pb_pressed = FALSE;

        /* Clear encoder updated flag. */
        enc->updated = FALSE;

        menu_listen();
        lcd_clear();

        /*
         * Reset variables.
         */

        menu_done                       = FALSE;
        enc_row_idx                     = 0;
        active_page                     = MENU_PAGE_MAIN;
        menu_page[active_page].curr_pos = 0;
    }
}
