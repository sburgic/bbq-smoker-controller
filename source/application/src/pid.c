/**
 ** Name
 **   pid.c
 **
 ** Purpose
 **   PID
 **
 ** Revision
 **   25-Jan-2022 (SSB) [] Initial
 **/

#include "pid.h"

#include "configuration.h"
#include "fan.h"
#include "state.h"

#include <max31850.h>

static arm_pid_instance_f32 pid;
static config_t*            cfg;
static Max31850_Hdl_t       max;

void pid_init( void )
{
    cfg = config_get_hdl();
    max = max31850_get_hdl();

    if (( NULL != cfg ) && ( NULL != max ))
    {
        pid.Kp = cfg->pid_kp;
        pid.Ki = cfg->pid_ki;
        pid.Kd = cfg->pid_kd;

        /* Initialize PID system, float32_t format. */
        arm_pid_init_f32( &pid, 1 );
    }
}

float pid_calculate( void )
{
    float duty;
    float pid_error;
    float expected;
    float current;

    expected = cfg->ts;
    current  = ( max->last_temp_raw[1] / 16.0 );

    pid_error = ( expected - current );
    duty      = arm_pid_f32( &pid, pid_error );

    return duty;
}
