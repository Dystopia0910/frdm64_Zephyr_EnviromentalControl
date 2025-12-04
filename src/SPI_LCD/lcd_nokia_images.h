/**
 * @file    lcd_nokia_images.h
 *
 * @brief   Static bitmap assets for the Nokia 5110 LCD.
 *
 * This header declares external bitmap arrays that represent logo images or
 * graphical patterns displayed on the Nokia LCD. These bitmaps match the
 * PCD8544 controller's 84x48 pixel resolution and are stored as 504-byte
 * arrays compatible with the display driver's `LCD_nokia_bitmap()` function.
 *
 * Typical usage includes splash screens, branding, and graphical icons in
 * embedded UI systems.
 *
 * @par
 * Maciel Morones, David Missael
 * Sistemas Operativos en Tiempo Real
 */

 #include <stdio.h>
#include <stdint.h>

extern const uint8_t ITESO_Logo[];
extern const uint8_t NXP[];
