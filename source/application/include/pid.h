/**
 ** Name
 **   pid.h
 **
 ** Purpose
 **   PID
 **
 ** Revision
 **   25-Jan-2022 (SSB) [] Initial
 **/

#ifndef __PID_H__
#define __PID_H__

#include "ptypes.h"

void pid_init( void );
float pid_calculate( float current, float expected );

#endif /* __PID_H__ */
