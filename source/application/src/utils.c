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
