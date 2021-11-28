/**
 ** Name
 **   encoder.h
 **
 ** Purpose
 **   Encoder routines
 **
 ** Revision
 **   28-Nov-2021 (SSB) [] Initial
 **/

#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "ptypes.h"

typedef enum
{
    ENC_DIRECTION_NONE  = 0x00,
    ENC_DIRECTION_RIGHT = 0x10,
    ENC_DIRECTION_LEFT  = 0x20
} Enc_Direction_t ;

/* Encoder main data structure */
typedef struct
{
    volatile int32_t         count;      /* Data between rotation
                                          * and user check
                                          */
    volatile bool_t          updated;    /* Updated on interrupt and
                                          * need to be cleared in SW
                                          */
    volatile Enc_Direction_t direction;  /* Last direction */
    volatile bool_t          pb_pressed; /* Updated when PB is pressed */
} Enc_t;

/* Initialize encoder */
void enc_init ( void );
/* Check and get new value from encoder */
int32_t enc_get_rotations ( void );
/* Encoder irq handler */
void enc_irq_hdl ( void );
/* Encoder PB irq handler */
void enc_sw_irq_hdl ( void );
/* Return encoder handle */
Enc_t* enc_get_hdl ( void );

#endif /* __ENCODER_H__ */
