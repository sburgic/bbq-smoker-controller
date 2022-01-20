/**
 ** Name
 **   state.h
 **
 ** Purpose
 **   S-Controller states
 **
 ** Revision
 **   20-Jan-2022 (SSB) [] Initial
 **/

#ifndef __MODE_H__
#define __MODE_H__

typedef enum
{
    S_CTRL_STATE_IDLE = 0,
    S_CTRL_STATE_PREHEAT_ACTIVE,
    S_CTRL_STATE_PREHEAT_DONE,
    S_CTRL_STATE_HEAT_ACTIVE,
    S_CTRL_STATE_HEAD_DONE
} s_ctrl_state_t;

void state_set( s_ctrl_state_t new_state );
s_ctrl_state_t state_get( void );

#endif /* __MODE_H__ */
