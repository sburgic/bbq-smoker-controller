/**
 ** Name
 **   configuration.c
 **
 ** Purpose
 **   Configuration routines
 **
 ** Revision
 **   21-Jan-2022 (SSB) [] Initial
 **/

#include "configuration.h"

#include "flash.h"
#include "utils.h"

#include <stm32g0xx_hal.h>

static config_t cfg_data =
{
    .ts         = 122,
    .tm_phase_1 = 77,
    .tm_phase_2 = 94,
    .bt_name    = "DangerZoneBL\0",
    .bt_pin     = { 6, 6, 6, 6, 6, 6 },
    .magic      = 0xDEADBEEF,
    .pid_kp     = 100,
    .pid_ki     = 0.01,
    .pid_kd     = 10
};

status_t config_init( void )
{
    status_t ret;
    config_t cfg_tmp;

    ret = flash_read((uint8_t*) &cfg_tmp, sizeof(config_t), 0 );

    if ( STATUS_OK == ret )
    {
        if ( 0xFFFFFFFF == cfg_tmp.magic )
        {
            ret = flash_write((uint8_t*) &cfg_data, sizeof(config_t), 0 );
        }
        else
        {
            ret = flash_read((uint8_t*) &cfg_data, sizeof(config_t), 0 );
        }
    }

    return ret;
}

status_t config_store( void )
{
    status_t ret;

    ret = flash_write((uint8_t*) &cfg_data, sizeof(config_t), 0 );

    return ret;
}

status_t config_restore_fact_defaults( void )
{
    status_t ret;

    cfg_data.ts         = 122;
    cfg_data.tm_phase_1 = 77;
    cfg_data.tm_phase_2 = 94;
    cfg_data.bt_pin[0]  = 6;
    cfg_data.bt_pin[1]  = 6;
    cfg_data.bt_pin[2]  = 6;
    cfg_data.bt_pin[3]  = 6;
    cfg_data.bt_pin[4]  = 6;
    cfg_data.bt_pin[5]  = 6;
    cfg_data.magic      = 0xDEADBEEF;
    cfg_data.pid_kp     = 100;
    cfg_data.pid_ki     = 0.01;
    cfg_data.pid_kd     = 10;

    utils_memcpy( &cfg_data.bt_name[0]
                , (uint8_t*) "DangerZoneBL\0"
                , CONFIG_BT_NAME_SIZE + 1
                );

    ret = flash_erase();

    if ( STATUS_OK == ret )
    {
        ret = config_init();
    }

    return ret;
}

config_t* config_get_hdl( void )
{
    return &cfg_data;
}
