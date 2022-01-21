/**
 ** Name
 **   utils.h
 **
 ** Purpose
 **   Utilities
 **
 ** Revision
 **   28-Nov-2021 (SSB) [] Initial
 **/

#ifndef __UTILS_H__
#define __UTILS_H__

#include "ptypes.h"

uint8_t* utils_float_to_char( float val, uint8_t* out );
float utils_float_cels_to_fahr( float celsius );
void utils_reverse( uint8_t* s, uint32_t s_len );
uint32_t utils_itoa( int32_t n, uint8_t* s, uint32_t s_max );

#endif /* __UTILS_H__ */
