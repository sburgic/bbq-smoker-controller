/**
 ** Name
 **   vmon.h
 **
 ** Purpose
 **   Voltage monitoring
 **
 ** Revision
 **   28-Nov-2020 (SSB) [] Initial
 **/

#ifndef __VMON_H__
#define __VMON_H__

#include "ptypes.h"

status_t vmon_init( void );
status_t vmon_get_voltage( float* voltage );

#endif /* __VMON_H__ */
