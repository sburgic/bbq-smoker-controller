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
int16_t utils_int_cels_to_fahr( int16_t celsius );
void utils_reverse( uint8_t* s, uint32_t s_len );
uint32_t utils_itoa( int32_t n, uint8_t* s, uint32_t s_max );
int32_t utils_memcmp( const void *p1, const void *p2, uint32_t len );
void utils_memset( void* dest, uint8_t val, uint32_t len );

#endif /* __UTILS_H__ */
