/**
 * @file    main.c
 *
 * @brief   Application entry point for the real-time environmental monitoring
 *          and control system.
 *
 * This file initializes all system modules, launches mode control and Bluetooth
 * communication threads, and runs the main loop responsible for sampling sensors,
 * updating the global environment controller, refreshing the display, and
 * evaluating actuator behavior based on user-defined thresholds.
 *
 * @par
 * Rodriguez Padilla, Daniel Jiram
 * IE703331
 * Martin del Campo, Mauricio
 * IE734429
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "env_controller.h"
#include "sensor_manager.h"
#include "display_manager.h"
#include "mode_controller.h"
#include "adjust_manager.h"
#include "uart_bt.h"

#define MAIN_LOOP_PERIOD_MS   500

/* Thread stacks */
K_THREAD_STACK_DEFINE(mode_thread_stack, 1024);
K_THREAD_STACK_DEFINE(bt_thread_stack,   2048);

/* Thread control blocks */
static struct k_thread mode_thread;
static struct k_thread bt_thread;

int main(void)
{
    printk("\n=== Starting Greenhouse Control System ===\n");

    /* Initialize global environment state */
    env_controller_init();

    /* Initialize sensors */
    if (sensor_manager_init() != 0) {
        printk("ERROR: sensor_manager_init() failed!\n");
    }

    /* Initialize LCD display (driver + framebuffer) */
    if (display_init() != 0) {
        printk("ERROR: display_init() failed!\n");
    } else {
        /* Optional: show logo at startup if you want */
        display_show_logo();
    }

    /* Initialize actuator logic */
    adjust_manager_init();

    /* Initialize button-based mode control */
    if (mode_controller_init() != 0) {
        printk("ERROR: mode_controller_init() failed!\n");
    }

    /* Spawn mode controller thread */
    k_thread_create(&mode_thread,
                    mode_thread_stack,
                    K_THREAD_STACK_SIZEOF(mode_thread_stack),
                    mode_controller_thread,
                    NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);

    /* Spawn Bluetooth UART thread */
    k_thread_create(&bt_thread,
                    bt_thread_stack,
                    K_THREAD_STACK_SIZEOF(bt_thread_stack),
                    uart_bt_thread,
                    NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);

    /* ------------------------------------------------------------------ */
    /*                         MAIN SYSTEM LOOP                           */
    /* ------------------------------------------------------------------ */

    while (1) {

        /* 1. Read all environmental sensors */
        sensor_data_t sensor_data;
        sensor_manager_read_all(&sensor_data);

        /* 2. Update global environment state */
        k_mutex_lock(&env.lock, K_FOREVER);
        env.measurements.temperature = sensor_data.temperature;
        env.measurements.humidity    = sensor_data.humidity;
        env.measurements.light       = sensor_data.light_level;
        env_mode_t mode              = env.mode;
        k_mutex_unlock(&env.lock);

        /* 3. Prepare display data (no setpoints in display_data_t) */
        display_data_t disp;
        disp.temperature = sensor_data.temperature;
        disp.humidity    = sensor_data.humidity;
        disp.light_level = sensor_data.light_level;
        disp.mode        = mode;

        /* 4. Update display */
        display_update(&disp);

        /* 5. Update actuator states based on thresholds */
        adjust_manager_update_actuators();

        /* 6. Loop timing */
        k_msleep(MAIN_LOOP_PERIOD_MS);
    }

    return 0;
}
