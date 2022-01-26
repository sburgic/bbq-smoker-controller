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

static arm_pid_instance_f32 pid;

void pid_init( void )
{
    config_t* cfg;

    cfg = config_get_hdl();

    if ( NULL != cfg )
    {
        pid.Kp = cfg->pid_kp;
        pid.Ki = cfg->pid_ki;
        pid.Kd = cfg->pid_kd;

        /* Initialize PID system, float32_t format. */
        arm_pid_init_f32( &pid, 1 );
    }
}

float pid_calculate( float current, float expected )
{
    float pid_error;
    float duty = 0;

    pid_error = ( expected - current );

    duty = arm_pid_f32( &pid, pid_error );

    return duty;
}
