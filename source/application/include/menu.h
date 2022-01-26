/**
 ** Name
 **   menu.h
 **
 ** Purpose
 **   Menu
 **
 ** Revision
 **   19-Jan-2022 (SSB) [] Initial
 **/

#ifndef __MENU_H__
#define __MENU_H__

#include "ptypes.h"

#define MENU_NO_OF_ROWS      (4)
#define MENU_CURSOR_POSITION (19)
#define MENU_CURSOR_SIGN     ((uint8_t*)"<")

typedef void (*menu_entry_cb_func_t)( void );

typedef struct
{
    uint8_t*             entry_name;
    menu_entry_cb_func_t callback;
} menu_entry_t;

typedef struct
{
    menu_entry_t* entries;
    uint8_t       curr_pos;
    uint8_t       no_of_entries;
} menu_page_t;

typedef enum
{
    MENU_PAGE_MAIN                = 0,
    MENU_PAGE_SET_TEMPERATURE     = 1,
    MENU_PAGE_CHANGE_STATE        = 2,
    MENU_PAGE_PID_SETTINGS        = 3,
    MENU_PAGE_SAVE_TO_FLASH       = 4,
    MENU_PAGE_RESTORE_FACTORY_DEF = 5
} menu_page_idx_t;

void menu_open( void );
void menu_irq_hdl( void );

#endif /* __MENU_H__ */
