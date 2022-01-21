/**
 ** Name
 **   buzzer.h
 **
 ** Purpose
 **   Buzzer routines
 **
 ** Revision
 **   28-Nov-2020 (SSB) [] Initial
 **/

#ifndef __BUZZER_H__
#define __BUZZER_H__

#include "ptypes.h"

void buzzer_init( void );
void buzzer_off( void );
void buzzer_on( void );
void buzzer_set_toggle( bool_t state );
void buzzer_irq_hdl( void );

#endif /* __BUZZER_H__ */
