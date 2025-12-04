#ifndef UART_BT_H
#define UART_BT_H

#include <zephyr/kernel.h>

/* Initializes UART and starts the Bluetooth interface */
int uart_bt_init(void);

/* UART thread loops reading messages and dispatching actions */
void uart_bt_thread(void *p1, void *p2, void *p3);

#endif /* UART_BT_H */
