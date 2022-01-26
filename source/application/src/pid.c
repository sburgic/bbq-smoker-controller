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

#include <arm_math.h>

#define PID_PARAM_KP  (100)  /* Proporcional. */
#define PID_PARAM_KI  (0.01) /* Integral. */
#define PID_PARAM_KD  (10)   /* Derivative. */

static arm_pid_instance_f32 pid;

void pid_init( void )
{
    pid.Kp = PID_PARAM_KP;
    pid.Ki = PID_PARAM_KI;
    pid.Kd = PID_PARAM_KD;

    /* Initialize PID system, float32_t format. */
    arm_pid_init_f32( &pid, 1 );
}

float pid_calculate( float current, float expected )
{
    static volatile float pid_error;
    static volatile float duty = 0;

    pid_error = ( expected - current );

    duty = arm_pid_f32( &pid, pid_error );

    return duty;
}
