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

static config_t cfg_data =
{
    .ts_phase_1 = 120,
    .tm_phase_1 = 60,
    .ts_phase_2 = 250,
    .tm_phase_2 = 180,
    .bt_name    = "BBQSmokerDangerZoneBL",
    .bt_pin     = { 0, 6, 6, 6 },
    .magic      = 0xDEADBEEF
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

config_t* config_get_hdl( void )
{
    return &cfg_data;
}
