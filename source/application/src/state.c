/**
 ** Name
 **   state.c
 **
 ** Purpose
 **   S-Controller states
 **
 ** Revision
 **   20-Jan-2022 (SSB) [] Initial
 **/

#include "state.h"

static s_ctrl_state_t state;

void state_set( s_ctrl_state_t new_state )
{
    state = new_state;
}

s_ctrl_state_t state_get( void )
{
    return state;
}
