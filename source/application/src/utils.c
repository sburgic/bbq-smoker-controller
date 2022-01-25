/**
 ** Name
 **   utils.c
 **
 ** Purpose
 **   Utilities
 **
 ** Revision
 **   28-Nov-2021 (SSB) [] Initial
 **/

#include "utils.h"

uint8_t* utils_float_to_char( float val, uint8_t* out )
{
    uint8_t  tmp[20] = {0};
    uint8_t* s       = tmp + 20; /* Go to end of buffer */
    uint16_t decimals;           /* Store the decimals */
    int16_t  units;              /* Store the units
                                  * (part to left of decimal place)
                                  */
    uint8_t  i;
    uint8_t  j = 0;

    /* Clear output buffer */
    for ( i = 0; i < 20; i++ )
    {
        out[i] = 0;
    }

    if ( val < 0 )
    {
        /*
         * Negative numbers
         */

        /* 1000 for 3 decimals, etc. */
        decimals = (int16_t)( val * -100 ) % 100;
        units    = (int16_t)( -1 * val );
    }
    else
    {
        /*
         * Positive numbers
         */

        decimals = (int16_t)( val * 100 ) % 100;
        units    = (int16_t) val;
    }

    *--s      = ( decimals % 10 ) + '0';
    decimals /= 10; /* Repeat for as many decimal places as needed */
    *--s = ( decimals % 10 ) + '0';
    *--s = '.';

    while ( units > 0 )
    {
        *--s = ( units % 10 ) + '0';
        units /= 10;
    }

    if ( val < 0 )
    {
        *--s = '-';
    }

    /*
     * Remove trailing zeros
     */

    for ( i = 0; i < 20; i++ )
    {
        if ( 0 != tmp[i] )
        {
            out[j] = tmp[i];
            j++;
        }
    }

    return s;
}

float utils_float_cels_to_fahr( float celsius )
{
    float ret;

    ret = (( celsius * 1.8 ) + 32.0 );

    return ret;
}

int16_t utils_int_cels_to_fahr( int16_t celsius )
{
    int16_t ret;

    ret = ((( celsius * 18 ) / 10 ) + 32 );

    return ret;
}

float utils_float_fahr_to_celsius( float fahrenheit )
{
    float ret;

    ret = (( fahrenheit - 32.0 ) / 1.8 );

    return ret;
}

int16_t utils_int_fahr_to_celsius( int16_t fahrenheit )
{
    int16_t ret;

    ret = ((( fahrenheit - 32 ) / 18 ) * 10 );

    return ret;
}

void utils_reverse( uint8_t* s, uint32_t s_len )
{
    uint8_t  c;
    uint32_t i;
    uint32_t j;

    j = ( s_len - 1 );

    for ( i = 0;  i < j; i++ )
    {
        c    = s[i];
        s[i] = s[j];
        s[j] = c;
        j--;
    }
}

uint32_t utils_itoa( int32_t n, uint8_t* s, uint32_t s_max )
{
    uint32_t i;
    bool_t   is_minus;
    uint32_t nr;

    if ( n < 0)
    {
        is_minus = TRUE;
        nr       = (uint32_t)(-n);
    }
    else
    {
        is_minus = FALSE;
        nr       = (uint32_t)n;
    }

    i = 0;
    do
    {
        s[i] = ((uint8_t)( nr % 10 )) + '0';
        i++;
        nr /= 10;
    }
    while (( nr > 0 ) && ( i < s_max ));

    if ( 0 == nr )
    {
        if ( FALSE != is_minus )
        {
            s[i] = '-';
            i++;
        }

        s[i] = '\0';
        utils_reverse( s, i );
    }

    return i;
}

int16_t utils_atoi( uint8_t* str )
{
    int16_t res = 0;
    uint8_t i;

    for ( i = 0; '\0' != str[i] && (( str[i] >= 48 ) && ( str[i] <= 57 )); i++ )
    {
        res = ( res * 10 + ( str[i] - '0' ));
    }

    return res;
}

int32_t utils_memcmp( const void *p1, const void *p2, uint32_t len )
{
    const uint8_t* cp1;
    const uint8_t* cp2;
    int32_t        ret = -1;

    if (( NULL != p1 ) && ( NULL != p2 ))
    {
        cp1 = (const uint8_t*) p1;
        cp2 = (const uint8_t*) p2;

        ret = 0;

        while ( len > 0 )
        {
            len--;

            if ( *cp1 != *cp2 )
            {
                ret = ((int32_t) *cp1 - (int32_t) *cp2 );
                break;
            }

            cp1++;
            cp2++;
        }
    }

    return ret;
}

void utils_memset( void* dest, uint8_t val, uint32_t len )
{
    uint32_t i;
    uint8_t* ptr;

    if ( NULL != dest )
    {
        ptr = (uint8_t*) dest;

        for ( i = 0; i < len; i++ )
        {
            ptr[i] = val;
        }
    }
}

uint16_t utils_strnlen( const uint8_t* s, uint16_t len )
{
    uint16_t i;

    for ( i = (uint16_t)0; (i < len)&&(*s); i++, s++ )
    {
    }
    ;

    return ( i );
}

int16_t utils_strcmp( uint8_t* s1, uint8_t* s2, uint16_t len )
{
    int16_t retval = (int16_t)0;

    if (( NULL != s1 ) && ( NULL != s2 ))
    {
        while( len )
        {
            len--;
            if (( *s1 != *s2 ) || ( 0 == *s1 ) || ( 0 == *s2 ))
            {
                retval = (int16_t)( *s1 - *s2 );
                break;
            }
            s1++;
            s2++;
        }
    }

    return retval;
}

void utils_memcpy( uint8_t* dest, uint8_t* src, uint16_t len )
{
    uint16_t i;

    if (( NULL != dest ) && ( NULL != src ))
    {
        for ( i = 0; i < len; i++ )
        {
            dest[i] = src[i];
        }
    }
}

int16_t utils_find_char( uint8_t* str, uint16_t len, uint8_t ch )
{
    int16_t  ret = -1;
    uint16_t i;

    for ( i = 0; i < len; i++ )
    {
        if ( str[i] == ch )
        {
            ret = i;
            break;
        }
    }

    return ret;
}
