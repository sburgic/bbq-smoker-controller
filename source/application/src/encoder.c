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

#include "encoder.h"

#include <stm32g0xx_hal.h>

/* Encoder PB debounce time in ms */
#define ENC_SW_DEBOUNCE_TIME    ((uint8_t)75)
/* Encoder PB pressed state */
#define ENC_SW_PRESSED          ((uint8_t)0)

#define ENC_START     0x0
#define ENC_CW_FINAL  0x1
#define ENC_CW_BEGIN  0x2
#define ENC_CW_NEXT   0x3
#define ENC_CCW_BEGIN 0x4
#define ENC_CCW_FINAL 0x5
#define ENC_CCW_NEXT  0x6

static const uint8_t enc_lookup_tbl[7][4] =
{
  /* ENC_START */
  {ENC_START,    ENC_CW_BEGIN,  ENC_CCW_BEGIN, ENC_START},
  /* ENC_CW_FINAL */
  {ENC_CW_NEXT,  ENC_START,     ENC_CW_FINAL,  ENC_START | ENC_DIRECTION_RIGHT},
  /* ENC_CW_BEGIN */
  {ENC_CW_NEXT,  ENC_CW_BEGIN,  ENC_START,     ENC_START},
  /* ENC_CW_NEXT */
  {ENC_CW_NEXT,  ENC_CW_BEGIN,  ENC_CW_FINAL,  ENC_START},
  /* ENC_CCW_BEGIN */
  {ENC_CCW_NEXT, ENC_START,     ENC_CCW_BEGIN, ENC_START},
  /* ENC_CCW_FINAL */
  {ENC_CCW_NEXT, ENC_CCW_FINAL, ENC_START,     ENC_START | ENC_DIRECTION_LEFT},
  /* ENC_CCW_NEXT */
  {ENC_CCW_NEXT, ENC_CCW_FINAL, ENC_CCW_BEGIN, ENC_START},
};

/* Encoder main data structure */
static Enc_t encoder;

void enc_init ( void )
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    gpio.Pin    = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
    gpio.Pull   = GPIO_NOPULL;
    gpio.Mode   = GPIO_MODE_INPUT;
    gpio.Speed  = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init ( GPIOA, &gpio );

    encoder.count       = 0;
    encoder.updated     = FALSE;
    encoder.pb_pressed  = FALSE;
    encoder.direction   = ENC_DIRECTION_NONE;
}

int32_t enc_get_rotations ( void )
{
    return encoder.count;
}

void enc_irq_hdl ( void )
{
    uint8_t pin_state;
    uint8_t result;
    static volatile uint8_t state;

    pin_state = (( HAL_GPIO_ReadPin( GPIOA, GPIO_PIN_4 ) << 1 )
              |  ( HAL_GPIO_ReadPin( GPIOA, GPIO_PIN_5 )));

    state  = enc_lookup_tbl[state & 0xF][pin_state];
    result = state & 0x30;

    if ( ENC_DIRECTION_NONE != result )
    {
        switch ( result )
        {
            case ENC_DIRECTION_LEFT:
            case ENC_DIRECTION_RIGHT:
                encoder.count     = ( result == ENC_DIRECTION_LEFT ) ? \
                                    encoder.count - 1 : encoder.count + 1;
                encoder.direction = (Enc_Direction_t) result;
                encoder.updated   = TRUE;
            break;

            default:
                encoder.direction = ENC_DIRECTION_NONE;
            break;
        }
    }
}

void enc_sw_irq_hdl ( void )
{
    uint8_t now_sw;
    static volatile uint8_t debounce;

    now_sw = HAL_GPIO_ReadPin ( GPIOA, GPIO_PIN_6 );

    if ( ENC_SW_PRESSED == now_sw )
    {
        debounce--;

        if ( 0 == debounce )
        {
            encoder.pb_pressed = TRUE;
        }
    }
    else
    {
        debounce = ENC_SW_DEBOUNCE_TIME;
        /* Acknowledge and clear pb_pressed flag in application where needed */
        /* encoder.pb_pressed = FALSE; */
    }
}

Enc_t* enc_get_hdl ( void )
{
    return &encoder;
}
