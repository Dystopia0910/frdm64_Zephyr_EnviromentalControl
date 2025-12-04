/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/kernel.h>
#include "SPI_LCD/spi_lcd_nokia.h"
#include "SPI_LCD/lcd_nokia_images.h"
#include "SPI_LCD/lcd_nokia_draw.h"

/* Mode enumeration */
typedef enum {
    MODE_READ_ONLY,
    MODE_ADJUSTING
} system_mode_t;

/* Global variables */
static system_mode_t current_mode = MODE_READ_ONLY;
static float temperature = 0.0f;
static float light_level = 0.0f;
static float humidity = 0.0f;

/**
 * @brief Display NXP logo on startup
 */
static void display_nxp_logo(void)
{
    LCD_nokia_bitmap(NXP);
    k_msleep(2000);  // Show logo for 2 seconds
    LCD_nokia_clear();
}

/**
 * @brief Update environmental data display on LCD
 */
static void update_display(void)
{
    char temp_str[16];
    char light_str[16];
    char humid_str[16];
    char mode_str[16];
    
    // Clear display area
    LCD_nokia_clear();
    
    // Format strings
    snprintf(temp_str, sizeof(temp_str), "Temp: %.1fC", temperature);
    snprintf(light_str, sizeof(light_str), "Light: %.0f lux", light_level);
    snprintf(humid_str, sizeof(humid_str), "Humid: %.1f%%", humidity);
    snprintf(mode_str, sizeof(mode_str), "Mode: %s", 
             (current_mode == MODE_READ_ONLY) ? "Read Only" : "Adjusting");
    
    // Display formatted strings using FrameBuffer
    LCD_nokia_write_string_xy_FB(0, 0, (uint8_t *)temp_str);
    LCD_nokia_write_string_xy_FB(0, 1, (uint8_t *)light_str);
    LCD_nokia_write_string_xy_FB(0, 2, (uint8_t *)humid_str);
    LCD_nokia_write_string_xy_FB(0, 3, (uint8_t *)mode_str);
    
    // Send FrameBuffer to LCD
    LCD_nokia_sent_FrameBuffer();
}

/**
 * @brief Mock function to simulate sensor readings
 * TODO: Replace with actual sensor drivers
 */
static void read_sensors(void)
{
    // Simulate sensor readings (replace with actual sensor code)
    temperature = 25.5f + (rand() % 10) / 10.0f;  // 25.5-26.4°C
    light_level = 300.0f + (rand() % 200);       // 300-500 lux
    humidity = 65.0f + (rand() % 10);            // 65-75%
}

int main(void)
{	
    // Initialize LCD
    if (!Nokia_Lcd_Init()) {
        printk("SPI Device not working\n\r");
        return 0;	
    }
    
    // Step 1: Display NXP logo on startup
    display_nxp_logo();
    
    // Step 2: Continuous monitoring loop
    while (1) {
        // Read sensors (mock data for now)
        read_sensors();
        
        // Update display with current readings
        update_display();
        
        // Wait before next update (e.g., 1 second)
        k_msleep(1000);
    }

    return 0;
}