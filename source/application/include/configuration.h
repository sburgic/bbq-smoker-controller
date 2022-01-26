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

#define CONFIG_BT_NAME_SIZE (12)
#define CONFIG_BT_PIN_SIZE  (6)

typedef struct
{
    int16_t   ts;
    int16_t   tm_phase_1;
    int16_t   tm_phase_2;
    uint8_t   bt_name[CONFIG_BT_NAME_SIZE + 1]; /* String termination. */
    uint8_t   bt_pin[CONFIG_BT_PIN_SIZE];
    float     pid_kp;
    float     pid_ki;
    float     pid_kd;
    uint8_t   reserved[7];
    uint32_t  magic;
} config_t;

status_t config_init( void );
status_t config_store( void );
status_t config_restore_fact_defaults( void );
config_t* config_get_hdl( void );

#endif /* __CONFIGURATION_H__ */
