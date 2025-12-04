/**
 * @file    display_manager.c
 *
 * @brief   Implementation of Nokia LCD UI rendering for greenhouse monitoring.
 *
 * This module formats environmental values into human-readable output and draws
 * them to the LCD framebuffer. It supports boot logos, live measurement updates,
 * and contextual mode displays, ensuring that the user receives immediate visual
 * feedback on system status.
 *
 * Display updates are triggered by the application layer and do not manage
 * asynchronous state transitions on their own.
 *
 * @par
 * Rodriguez Padilla, Daniel Jiram  
 * IE703331  
 * Martin del Campo, Mauricio  
 * IE734429
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zephyr/kernel.h>
#include "SPI_LCD/spi_lcd_nokia.h"
#include "SPI_LCD/lcd_nokia_images.h"
#include "display_manager.h"

/* Macro to avoid float to double promotion warning */
#define FLOAT_TO_DBL(f) ((double)(f))

/* Private variables */
static bool display_initialized = false;

int display_init(void)
{
    int ret;
    
    ret = Nokia_Lcd_Init();
    if (ret != NOKIA_LCD_OK) {
        printk("[DISPLAY] Failed to initialize LCD\n");
        return -1;
    }
    
    display_initialized = true;
    printk("[DISPLAY] Display manager initialized successfully\n");
    return 0;
}

void display_show_logo(void)
{
    if (!display_initialized) {
        printk("[DISPLAY] Error: Display not initialized\n");
        return;
    }
    
    LCD_nokia_bitmap(NXP);
    printk("[DISPLAY] Showing NXP logo\n");
    k_msleep(2000);
    LCD_nokia_clear();
}

void display_update(const display_data_t *data)
{
    char temp_str[20];
    char light_str[20];
    char humid_str[20];
    char mode_str[20];
    
    if (!display_initialized || data == NULL) {
        return;
    }
    
    /* Clear the screen completely */
    LCD_nokia_clear();
    
    /* Or clear specific rows if you want to keep some graphics */
    /* 
    for (int i = 0; i < 4; i++) {
        LCD_nokia_clear_range_FrameBuffer(0, i, 84);
    }
    */
    
    /* Format and display strings */
    snprintf(temp_str, sizeof(temp_str), "Temp: %.1fC", (double)data->temperature);
    LCD_nokia_write_string_xy_FB(0, 0, (uint8_t *)temp_str);
    
    snprintf(light_str, sizeof(light_str), "Light: %.0f lux", (double)data->light_level);
    LCD_nokia_write_string_xy_FB(0, 1, (uint8_t *)light_str);
    
    snprintf(humid_str, sizeof(humid_str), "Humid: %.1f%%", (double)data->humidity);
    LCD_nokia_write_string_xy_FB(0, 2, (uint8_t *)humid_str);
    
    snprintf(mode_str, sizeof(mode_str), "Mode: %s", 
             (data->mode == MODE_READ_ONLY) ? "Read Only" : "Adjusting");
    LCD_nokia_write_string_xy_FB(0, 3, (uint8_t *)mode_str);
    
    LCD_nokia_sent_FrameBuffer();
}


void display_clear(void)
{
    if (!display_initialized) return;
    LCD_nokia_clear();
}

bool display_is_initialized(void)
{
    return display_initialized;
}