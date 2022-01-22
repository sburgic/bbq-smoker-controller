/**
 ** Name
 **   configuration.h
 **
 ** Purpose
 **   Configuration routines
 **
 ** Revision
 **   21-Jan-2022 (SSB) [] Initial
 **/

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include "ptypes.h"

#define CONFIG_BT_NAME_SIZE (20)
#define CONFIG_BT_PIN_SIZE  (4)

typedef struct
{
    int16_t  ts_phase_1;
    int16_t  ts_phase_2;
    int16_t  tm_phase_1;
    int16_t  tm_phase_2;
    uint8_t  bt_name[CONFIG_BT_NAME_SIZE];
    uint8_t  bt_pin[CONFIG_BT_PIN_SIZE];
    uint32_t magic;
    uint8_t  reserved[4];
} config_t;

status_t config_init( void );
status_t config_store( void );
config_t* config_get_hdl( void );

#endif /* __CONFIGURATION_H__ */
