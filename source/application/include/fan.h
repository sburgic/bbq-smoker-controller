/**
 ** Name
 **   fan.h
 **
 ** Purpose
 **   Fan control rouitnes
 **
 ** Revision
 **   28-Nov-2021 (SSB) [] Initial
 **/

#ifndef __FAN_H__
#define __FAN_H__

#include "ptypes.h"

status_t fan_init( void );
status_t fan_start( void );
void fan_set_load( uint8_t load );
void fan_stop( void );

#endif /* __FAN_H__ */