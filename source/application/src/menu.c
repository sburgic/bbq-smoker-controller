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

#include "encoder.h"
#include "fan.h"
#include "state.h"

#include <lcd2wire.h>

static Enc_Hdl_t         enc;
static uint8_t           enc_row_idx = 0;
static bool_t            menu_done   = FALSE;
static menu_page_idx_t   active_page = MENU_PAGE_MAIN;
static bool_t            trans       = FALSE;

static void menu_start_cb( void );
static void menu_set_temperature_cb( void );
static void menu_change_state_cb( void );
static void menu_fan_settings_cb( void );
static void menu_bt_settings_cb( void );
static void menu_exit_cb( void );

static menu_entry_t menu_page_main[] =
{
    { (uint8_t*) "Start", menu_start_cb },
    { (uint8_t*) "Set temperature", menu_set_temperature_cb },
    { (uint8_t*) "Change state", menu_change_state_cb },
    { (uint8_t*) "Fan settings", menu_fan_settings_cb },
    { (uint8_t*) "Bluetooth settings", menu_bt_settings_cb },
    { (uint8_t*) "Exit menu", menu_exit_cb }
};

static menu_page_t menu_page[] =
{
    {
        &menu_page_main[0],
        0,
        sizeof(menu_page_main) / sizeof(menu_page_main[0])
    }
};

static void menu_start_cb( void )
{
    state_set( S_CTRL_STATE_PREHEAT_ACTIVE );
    menu_done = TRUE;
}

static void menu_set_temperature_cb( void )
{
}

static void menu_change_state_cb( void )
{
}

static void menu_fan_settings_cb( void )
{
}

static void menu_bt_settings_cb( void )
{
}

static void menu_exit_cb( void )
{
    menu_done = TRUE;
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

            if ( NULL != entry->callback )
            {
                entry->callback();
            }

            enc->pb_pressed = FALSE;
        }
    }
}

void menu_open( void )
{
    enc = enc_get_hdl();

    if ( NULL != enc )
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
