/**
 ** Name
 **   fan.h
 **
 ** Purpose
 **   Fan control routines
 **
 ** Revision
 **   28-Nov-2021 (SSB) [] Initial
 **/

#ifndef __FAN_H__
#define __FAN_H__

#include "ptypes.h"

status_t fan_init( void );
status_t fan_start( void );
void fan_set_pwm( uint8_t pwm );
uint8_t fan_get_pwm( void );
void fan_stop( void );

#endif /* __FAN_H__ */
