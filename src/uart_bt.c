/**
 * @file    uart_bt.c
 *
 * @brief   Implementation of Bluetooth UART communication for greenhouse control.
 *
 * This module sets up interrupt-driven UART reception for the HC-05 Bluetooth
 * module, buffers incoming data into complete command strings, and dispatches
 * parsed actions to adjust_manager. It also provides formatted telemetry and
 * status reporting back to the remote user.
 *
 * The uart_bt thread serves as the communication bridge between the greenhouse
 * system and external controllers, without directly modifying core state.
 *
 * @par
 * Rodriguez Padilla, Daniel Jiram  
 * IE703331  
 * Martin del Campo, Mauricio  
 * IE734429
 */

 #include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <string.h>
#include <stdio.h>

#include "uart_bt.h"
#include "env_controller.h"
#include "command_parser.h"
#include "adjust_manager.h"
#include "mode_controller.h"

/* UART node defined in the overlay */
#define UART_BT_NODE DT_NODELABEL(uart3)
static const struct device *const uart_bt = DEVICE_DT_GET(UART_BT_NODE);

/* RX buffers */
#define BT_RX_BUF_SIZE 64
#define BT_MSGQ_SIZE   10

K_MSGQ_DEFINE(bt_rx_msgq, BT_RX_BUF_SIZE, BT_MSGQ_SIZE, 4);
static char rx_buffer[BT_RX_BUF_SIZE];
static int rx_pos = 0;

/* UART RX callback */
static void uart_bt_callback(const struct device *dev, void *user_data)
{
    uint8_t c;

    if (!uart_irq_update(dev)) return;
    if (!uart_irq_rx_ready(dev)) return;

    while (uart_fifo_read(dev, &c, 1) == 1) {

        /* Command finished on newline or carriage return */
        if ((c == '\n' || c == '\r') && rx_pos > 0) {
            rx_buffer[rx_pos] = '\0';

            /* Queue message regardless of mode.
               adjust_manager decides what can be applied. */
            k_msgq_put(&bt_rx_msgq, rx_buffer, K_NO_WAIT);

            printk("[BT] Received: %s\n", rx_buffer);
            rx_pos = 0;
        }
        else if (rx_pos < BT_RX_BUF_SIZE - 1) {
            rx_buffer[rx_pos++] = c;
        }
        else {
            printk("[BT] Buffer overflow\n");
            rx_pos = 0;
        }
    }
}

/* UART initialization */
int uart_bt_init(void)
{
    if (!device_is_ready(uart_bt)) {
        printk("ERROR: Bluetooth UART device not ready!\n");
        return -ENODEV;
    }

    uart_irq_callback_user_data_set(uart_bt, uart_bt_callback, NULL);
    uart_irq_rx_enable(uart_bt);

    printk("Bluetooth UART initialized (9600 baud)\n");
    return 0;
}

/* Helper to send a string to HC-05 */
static void uart_bt_send(const char *str)
{
    if (!device_is_ready(uart_bt)) return;

    for (size_t i = 0; str[i] != '\0'; i++)
        uart_poll_out(uart_bt, str[i]);
}

/* Main Bluetooth thread */
void uart_bt_thread(void *p1, void *p2, void *p3)
{
    uart_bt_init();

    uart_bt_send("\r\n=== Greenhouse Control System ===\r\n");
    uart_bt_send("Commands:\r\n");
    uart_bt_send("  TEMP=25,HUM=60,LUX=400\r\n");
    uart_bt_send("  MODE=READ\r\n");
    uart_bt_send("  MODE=ADJUST\r\n");

    char cmd_buffer[BT_RX_BUF_SIZE];

    while (1) {

        /* Wait for a valid line of text */
        if (k_msgq_get(&bt_rx_msgq, cmd_buffer, K_FOREVER) == 0) {

            /* Read the current setpoints */
            env_setpoints_t current_sp;
            k_mutex_lock(&env.lock, K_FOREVER);
            current_sp = env.setpoints;
            k_mutex_unlock(&env.lock);

            /* Parse command using the NEW parser */
            parser_result_t parsed =
                command_parser_parse(cmd_buffer, &current_sp);

            bool apply_setpoints = false;
            bool change_mode = false;
            env_mode_t new_mode = env.mode;

            /* Interpret parser output */
            switch (parsed.action) {

            case PARSER_ACTION_SET_SETPOINTS:
                apply_setpoints = true;
                break;

            case PARSER_ACTION_MODE_READ:
                new_mode = ENV_MODE_READ_ONLY;
                change_mode = true;
                break;

            case PARSER_ACTION_MODE_ADJUST:
                new_mode = ENV_MODE_ADJUSTING;
                change_mode = true;
                break;

            default:
                uart_bt_send("ERROR: Invalid command\r\n");
                continue;
            }

            /* Apply the action */
            adjust_manager_process_action(
                new_mode,
                &parsed.new_setpoints,
                apply_setpoints,
                change_mode
            );

            /* Send confirmation */
            if (apply_setpoints) {
                char response[80];
                snprintf(response, sizeof(response),
                        "OK: T=%.1f H=%.1f L=%.1f\r\n",
                        (double)parsed.new_setpoints.target_temperature,
                        (double)parsed.new_setpoints.target_humidity,
                        (double)parsed.new_setpoints.target_light);
                uart_bt_send(response);
            }
            else if (change_mode) {
                uart_bt_send("OK: Mode changed\r\n");
            }
        }

        k_msleep(10);
    }

}
