/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * @file    main.c
 *
 * @brief   Application entry point for greenhouse control system.
 *
 * This file orchestrates initialization of environment state, sensors, display,
 * mode control, actuator management, and Bluetooth communication. It runs the
 * central update loop where fresh sensor data is sampled, UI elements are
 * refreshed, and actuator behavior is evaluated according to user-defined
 * setpoints.
 *
 * main.c serves as the high-level coordinator that binds together all modules
 * into a functioning real-time system.
 *
 * @par
 * Rodriguez Padilla, Daniel Jiram  
 * IE703331  
 * Martin del Campo, Mauricio  
 * IE734429
 */

#include <stdio.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/kernel.h>
#include "SPI_LCD/spi_lcd_nokia.h"
#include "SPI_LCD/lcd_nokia_images.h"
#include "SPI_LCD/lcd_nokia_draw.h"
#include "display_manager.h"
#include "sensor_manager.h"
#include "env_controller.h"
#include "adjust_manager.h"
#include "command_parser.h"
#include "mode_controller.h"
#include "uart_bt.h"

/* Update period for sensor readings (ms) */
#define SENSOR_UPDATE_MS   1000

/* Global mode variable — kept simple for now */
static system_mode_t current_mode = MODE_READ_ONLY;

/* Converts sensor_data_t into display_data_t for the display manager */
static void convert_sensor_to_display(const sensor_data_t *sens, display_data_t *disp)
{
    /* Copy values even if some are invalid; display manager will show them as-is */
    disp->temperature = sens->temperature;
    disp->light_level = sens->light_level;
    disp->humidity    = sens->humidity;
    disp->mode        = current_mode;
}

/* Process sensor errors (optional future expansion) */
static void log_sensor_status(const sensor_data_t *data)
{
    if (!data->temperature_valid) {
        printk("Warning: Temperature reading invalid\n");
    }
    if (!data->light_valid) {
        printk("Warning: Light reading invalid\n");
    }
    if (!data->humidity_valid) {
        printk("Warning: Humidity reading invalid\n");
    }
}

int main(void)
{
    int ret;

    printk("=== System Boot ===\n");

    /* --- Initialize display --- */
    ret = display_init();
    if (ret != 0) {
        printk("ERROR: Display initialization failed\n");
        return 0;
    }

    /* Show logo for 2 seconds */
    display_show_logo();

    /* --- Initialize sensors --- */
    ret = sensor_manager_init();
    if (ret != 0) {
        printk("ERROR: Sensor manager initialization failed: %s\n",
               sensor_manager_get_error());
    } else {
        printk("Sensors initialized successfully\n");
    }

    /* Main loop */
    while (1) {
        sensor_data_t sens;
        display_data_t disp;

        /* Read all sensors */
        if (sensor_manager_read_all(&sens) != 0) {
            printk("ERROR reading sensors: %s\n", sensor_manager_get_error());
        }

        /* Optional debug information */
        log_sensor_status(&sens);

        /* Convert sensor structure into the display structure */
        convert_sensor_to_display(&sens, &disp);

        /* Update the screen */
        display_update(&disp);

        k_msleep(SENSOR_UPDATE_MS);
    }

    return 0;
}
