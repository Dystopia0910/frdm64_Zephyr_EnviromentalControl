/**
 * @file    uart_bt.h
 *
 * @brief   Bluetooth UART interface for command reception and reporting.
 *
 * This header declares initialization and threading functions for the UART
 * interface used to communicate with the HC-05 module. Incoming characters are
 * assembled into full command strings and forwarded to the command parser before
 * being handled by the adjust_manager.
 *
 * @par
 * Rodriguez Padilla, Daniel Jiram  
 * IE703331  
 * Martin del Campo, Mauricio  
 * IE734429
 */

 #ifndef UART_BT_H
#define UART_BT_H

#include <zephyr/kernel.h>

/* Initializes UART and starts the Bluetooth interface */
int uart_bt_init(void);

/* UART thread loops reading messages and dispatching actions */
void uart_bt_thread(void *p1, void *p2, void *p3);

#endif /* UART_BT_H */
