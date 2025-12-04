/**
 * @file    lcd_nokia_draw.h
 *
 * @brief   Drawing utilities for rendering shapes on the Nokia 5110 LCD.
 *
 * This header exposes user-friendly drawing primitives such as line rendering,
 * pixel plotting, and simple geometric operations. All drawing functions operate
 * on the LCD framebuffer managed by spi_lcd_nokia.c, enabling smooth updates
 * and flicker-free rendering.
 *
 * The draw module acts as a lightweight graphics layer that supplements the
 * text and bitmap capabilities of the SPI driver.
 *
 * @par
 * Maciel Morones, David Missael
 * Sistemas Operativos en Tiempo Real
 */

 #include <stdio.h>
#include <stdint.h>

typedef enum{
    pass_code = 0,
    zero_division_error,
    out_of_bounds_error
}error_code;

#define PIXEL_X_MAX_LIMIT 83
#define PIXEL_X_MIN_LIMIT 0
#define PIXEL_Y_MAX_LIMIT 47
#define PIXEL_Y_MIN_LIMIT 0

uint8_t drawline(float x0, float y0, float x1, float y1, uint8_t mindots);