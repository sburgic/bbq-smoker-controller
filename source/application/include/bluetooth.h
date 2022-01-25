/**
 ** Name
 **   bluetooth.h
 **
 ** Purpose
 **   Bluetooth module
 **
 ** Revision
 **   22-Jan-2022 (SSB) [] Initial
 **/

#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#include <uart.h>

#define BT_UART     USART2
#define BT_BAUDRATE (9600)

#define BT_BUFF_SIZE       UART2_BUFFER_SIZE
#define BT_MSG_MIN_SIZE    (6)
#define BT_MSG_HEADER      ((uint8_t*)"BBQ.")
#define BT_MSG_HEADER_SIZE (4)

status_t bt_init( void );
void bt_task( void );
bool_t bt_get_state( void );

#endif /* __BLUETOOTH_H__ */
