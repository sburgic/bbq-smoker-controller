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
float utils_float_fahr_to_celsius( float fahrenheit );
int16_t utils_int_fahr_to_celsius( int16_t fahrenheit );
void utils_reverse( uint8_t* s, uint32_t s_len );
uint32_t utils_itoa( int32_t n, uint8_t* s, uint32_t s_max );
int16_t utils_atoi( uint8_t* str );
int32_t utils_memcmp( const void *p1, const void *p2, uint32_t len );
void utils_memset( void* dest, uint8_t val, uint32_t len );
uint16_t utils_strnlen( const uint8_t* s, uint16_t len );
int16_t utils_strcmp( uint8_t* s1, uint8_t* s2, uint16_t len );
void utils_memcpy( uint8_t* dest, uint8_t* src, uint16_t len );
int16_t utils_find_char( uint8_t* str, uint16_t len, uint8_t ch );

#endif /* __UTILS_H__ */
