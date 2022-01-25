/**
 ** Name
 **   bluetooth.c
 **
 ** Purpose
 **   Bluetooth module
 **
 ** Revision
 **   22-Jan-2022 (SSB) [] Initial
 **/

#include "bluetooth.h"

#include "configuration.h"
#include "time.h"
#include "utils.h"

#include <max31850.h>

#define BT_RESP_TIMEOUT_MS ((Bsp_Time)1000)
#define BT_MSG_TIMEOUT_MS  ((Bsp_Time)64)

#define BT_CMD_AT       ((uint8_t*)"AT")
#define BT_CMD_SET_NAME ((uint8_t*)"AT+NAME")
#define BT_CMD_SET_PIN  ((uint8_t*)"AT+PASS")

#define BT_RESP_AT_OK    ((uint8_t*)"OK")
#define BT_RESP_AT_NAME  ((uint8_t*)"OK+Set:")
#define BT_RESP_AT_PIN   ((uint8_t*)"OK+Set:")
#define BT_RESP_MIN_SIZE (7)

static uint8_t        bt_buff[BT_BUFF_SIZE] = {0};
static config_t*      cfg;
static Max31850_Hdl_t max;

#if 0
static void bt_gpio_init( void )
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();

    gpio.Pin   = GPIO_PIN_9;
    gpio.Mode  = GPIO_MODE_INPUT;
    gpio.Pull  = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init( GPIOB, &gpio );
}
#endif

static uint16_t bt_get_response( uint8_t* buff, uint8_t* expected )
{
    uint16_t cnt = 0;
    Bsp_Time timeout;
    bool_t   is_timed_out;
    bool_t   is_empty;
    bool_t   first_read = TRUE;
    uint8_t  ch;

    bsp_set_timeout( BT_RESP_TIMEOUT_MS
                   , BSP_TIME_MSEC
                   , &timeout
                   );
    do
    {
        is_timed_out = bsp_is_timeout( timeout );
        is_empty     = uart_buff_empty( BT_UART );

        if ( FALSE == is_empty )
        {
            ch = uart_getc( BT_UART );

            if ( 0 != ch )
            {
                if ( FALSE != first_read )
                {
                    if ( ch == expected[0] )
                    {
                        buff[cnt++]  = ch;
                        first_read = FALSE;
                    }
                }
                else
                {
                    buff[cnt++] = ch;

                    if ( UART2_STRING_DELIMITER == buff[cnt - 1] )
                    {
                        break;
                    }
                }
            }
        }
    } while ( FALSE == is_timed_out );

    return cnt;
}

static bool_t bt_process_msg( uint8_t* msg, uint16_t* len )
{
    bool_t   is_empty = FALSE;
    status_t sret;
    int16_t  temp;
    int16_t  fahrenheit;
    uint32_t i;
    int16_t  res;
    int16_t  pos;
    bool_t   is_celsius = TRUE;

    if ((( 'E' == msg[1] )) && (( 'T' == msg[2] ) && ( '.' == msg[3] )))
    {
        if ( 'S' == msg[0] ) /* Set. */
        {
            res = utils_strcmp((uint8_t*) "TS:", &msg[4], 3 );

            if ( 0 == res )
            {
                i = 0;
            }
            else
            {
                i = 1;

                res = utils_strcmp((uint8_t*) "TM1:", &msg[4], 4 );

                if ( 0 != res )
                {
                    i = 2;
                    res = utils_strcmp((uint8_t*) "TM2:", &msg[4], 4 );

                    if ( 0 != res )
                    {
                        is_empty = TRUE;
                    }
                }
            }

            if ( FALSE == is_empty )
            {
                if ( 0 == i )
                {
                    temp = utils_atoi( &msg[7] );
                    pos  = utils_find_char( &msg[7], 4, 'F' );
                }
                else
                {
                    temp = utils_atoi( &msg[8] );
                    pos  = utils_find_char( &msg[8], 4, 'F' );
                }

                if ( -1 != pos )
                {
                    is_celsius = FALSE;
                }
                else
                {
                    if ( 0 == i )
                    {
                        pos = utils_find_char( &msg[7], 4, 'C' );
                    }
                    else
                    {
                        pos = utils_find_char( &msg[8], 4, 'C' );
                    }
                }

                if ( -1 != pos )
                {
                    if ( FALSE == is_celsius )
                    {
                        temp = (int16_t) utils_float_fahr_to_celsius \
                                                            ( (float) temp );
                    }

                    if ( 0 == i )
                    {
                        cfg->ts = temp;
                    }
                    else if ( 1 == i )
                    {
                        cfg->tm_phase_1 = temp;
                    }
                    else if ( 2 == i )
                    {
                        cfg->tm_phase_2 = temp;
                    }
                    else
                    {
                        is_empty = TRUE;
                    }

                    if ( FALSE == is_empty )
                    {
                        msg[0] = 'O';
                        msg[1] = 'K';

                        *len = 2;
                    }
                }
                else
                {
                    is_empty = TRUE;
                }
            }
        }
        else if ( 'G' == msg[0] ) /* Get. */
        {
            if ( 'T' == msg[4] ) /* Temperatures. */
            {
                if ( 'S' == msg[5] )
                {
                    utils_memcpy( &msg[0], "TS", 2 );
                    msg[2] = ':';

                    temp = cfg->ts;

                    i = 3;

                    /* Invalid message (extra characters), no response. */
                    if ( 6 != *len )
                    {
                        is_empty = TRUE;
                    }
                }
                else if ( 'M' == msg[5] )
                {
                    /* Put index in reponse message. */
                    msg[2] = msg[6];
                    msg[3] = ':';

                    utils_memcpy( &msg[0], "TM", 2 );

                    if ( 1 == ( msg[2] - 48 ))
                    {
                        temp = cfg->tm_phase_1;
                    }
                    else if ( 2 == ( msg[2] - 48 ))
                    {
                        temp = cfg->tm_phase_2;
                    }
                    else
                    {
                        /* Invalid message (extra characters), no response. */
                        is_empty = TRUE;
                    }

                    if ( FALSE == is_empty )
                    {
                        i = 4;

                        /* Invalid message (extra characters), no response. */
                        if ( 7 != *len )
                        {
                            is_empty = TRUE;
                        }
                    }
                }

                if ( FALSE == is_empty )
                {
                    i += utils_itoa( temp, &msg[i], 3 );

                    msg[i++] = 'C';
                    msg[i++] = '/';

                    fahrenheit = utils_int_cels_to_fahr( temp );
                    i += utils_itoa( fahrenheit, &msg[i], 3 );
                    msg[i++] = 'F';

                    *len = i;
                }
            }
            else
            {
                res = utils_strcmp((uint8_t*) "CURRENT", &msg[4], 7 );

                if (( 0 == res ) && ( 11 == *len )) /* Current temperatures. */
                {
                    if ( MAX31850_SENSOR_ERROR !=  max->last_temp_raw[1] )
                    {
                        i    = 3;
                        temp = ( max->last_temp_raw[1] >> 4 );

                        utils_memcpy( &msg[0], "TS:", 3 );
                        i += utils_itoa( temp, &msg[i], 3 );
                        msg[i++] = 'C';
                        msg[i++] = '/';

                        fahrenheit = utils_int_cels_to_fahr( temp );
                        i += utils_itoa( fahrenheit, &msg[i], 3 );
                        msg[i++] = 'F';

                        msg[i++] = '\r';
                        msg[i++] = '\n';
                    }
                    else
                    {
                        utils_memcpy( &msg[0], (uint8_t*) "TS:Error!\r\n", 11 );
                        i = 11;
                    }

                    if ( MAX31850_SENSOR_ERROR !=  max->last_temp_raw[0] )
                    {
                        temp = ( max->last_temp_raw[0] >> 4 );

                        utils_memcpy( &msg[i], "TM:", 3 );
                        i += 3;
                        i += utils_itoa( temp, &msg[i], 3 );
                        msg[i++] = 'C';
                        msg[i++] = '/';

                        fahrenheit = utils_int_cels_to_fahr( temp );
                        i += utils_itoa( fahrenheit, &msg[i], 3 );
                        msg[i++] = 'F';
                    }
                    else
                    {
                        utils_memcpy( &msg[i], (uint8_t*) "TM:Error!\r\n", 11 );
                        i += 11;
                    }

                    *len = i;
                }
                else
                {
                    is_empty = TRUE;
                }
            }
        }
    }
    else
    {
        res = utils_strcmp((uint8_t*) "STORE", &msg[0], 5 );

        if ( 0 == res )
        {
            if ( 5 == *len )
            {
                sret = config_store();

                if ( STATUS_OK == sret )
                {
                    *len = 21;
                    utils_memcpy( &msg[0]
                                , (uint8_t*) "Configuration stored!"
                                , *len
                                );
                }
                else
                {
                    *len = 19;
                    utils_memcpy( &msg[0]
                                , (uint8_t*) "Flash write failed!"
                                , *len
                                );
                }
            }
            else
            {
                is_empty = TRUE;
            }
        }
    }

    return is_empty;
}

status_t bt_init( void )
{
    status_t ret = STATUS_ERROR;
    uint16_t cnt = 0;
    int16_t  res = -1;
    uint8_t  i;
    uint8_t  pin_str[CONFIG_BT_PIN_SIZE] = {0};

    cfg = config_get_hdl();
    max = max31850_get_hdl();

    if (( NULL != cfg ) && ( NULL != max ))
    {
        ret = uart_init( BT_UART, BT_BAUDRATE );

        if ( STATUS_ERROR != ret )
        {
            ret = STATUS_ERROR;
            uart_send( BT_UART, BT_CMD_AT, 2 );

            cnt = bt_get_response( bt_buff, BT_RESP_AT_OK );

            if ( cnt == utils_strnlen( BT_RESP_AT_OK, BT_BUFF_SIZE ))
            {
                res = utils_strcmp( bt_buff, BT_RESP_AT_OK, cnt );

                if ( 0 == res )
                {
                    ret = STATUS_OK;
                }
            }
        }

        if ( STATUS_ERROR != ret )
        {
            bsp_wait( 500, BSP_TIME_MSEC );
            ret = STATUS_ERROR;

            cnt = utils_strnlen( &cfg->bt_name[0], CONFIG_BT_NAME_SIZE );

            uart_send( BT_UART, BT_CMD_SET_NAME, 7 );
            uart_send( BT_UART, &cfg->bt_name[0], cnt );

            cnt = bt_get_response( bt_buff, BT_RESP_AT_NAME );

            if ( cnt > 0 )
            {
                cnt = utils_strnlen( BT_RESP_AT_NAME, BT_RESP_MIN_SIZE );

                /* Check only the first part of the reposnse. */
                res = utils_strcmp( bt_buff, BT_RESP_AT_NAME, cnt );

                if ( 0 == res )
                {
                    ret = STATUS_OK;
                }
            }
        }

        if ( STATUS_ERROR != ret )
        {
            bsp_wait( 500, BSP_TIME_MSEC );
            ret = STATUS_ERROR;
            uart_send( BT_UART, BT_CMD_SET_PIN, 7 );

            for ( i = 0; i < CONFIG_BT_PIN_SIZE; i++ )
            {
                /* Convert number to char. */
                pin_str[i] = ( cfg->bt_pin[i] + 48 );
            }

            uart_send( BT_UART, &pin_str[0], CONFIG_BT_PIN_SIZE );

            cnt = bt_get_response( bt_buff, BT_RESP_AT_PIN );

            if ( cnt > 0 )
            {
                cnt = utils_strnlen( BT_RESP_AT_PIN, BT_RESP_MIN_SIZE );

                /* Check only the first part of the reposnse. */
                res = utils_strcmp( bt_buff, BT_RESP_AT_PIN, cnt );

                if ( 0 == res )
                {
                    ret = STATUS_OK;
                }
            }
        }
    }

    return ret;
}

void bt_task( void )
{
    uint16_t cnt = 0;
    Bsp_Time timeout;
    bool_t   is_timed_out;
    bool_t   is_empty;

    is_empty = uart_buff_empty( BT_UART );

    if ( FALSE == is_empty )
    {
        bsp_set_timeout( BT_MSG_TIMEOUT_MS
                       , BSP_TIME_MSEC
                       , &timeout
                       );
        do
        {
            is_timed_out = bsp_is_timeout( timeout );
            is_empty     = uart_buff_empty( BT_UART );

            if ( FALSE == is_empty )
            {
                bt_buff[cnt] = uart_getc( BT_UART );
                cnt++;
            }
        } while ( FALSE == is_timed_out );
    }

     bt_buff[cnt] = '\0';

    if ( cnt >= BT_MSG_MIN_SIZE )
    {
        cnt -= 1; /* Remove '\0' from count. */

        if ( 0 == utils_strcmp( bt_buff, BT_MSG_HEADER, BT_MSG_HEADER_SIZE ))
        {
            if (( '\n' == bt_buff[cnt - 1] ) || ( ( '\r' == bt_buff[cnt - 1] )))
            {
                cnt--;
            }

            if (( '\n' == bt_buff[cnt - 2] ) || ( ( '\r' == bt_buff[cnt - 2] )))
            {
                cnt--;
            }

            cnt -= BT_MSG_HEADER_SIZE;

            is_empty = bt_process_msg( &bt_buff[BT_MSG_HEADER_SIZE], &cnt );

            if ( FALSE == is_empty )
            {
                uart_send( BT_UART, &bt_buff[BT_MSG_HEADER_SIZE], cnt );
                uart_send( BT_UART, (uint8_t*) "\r\n", 2 );
            }
        }
    }
}
