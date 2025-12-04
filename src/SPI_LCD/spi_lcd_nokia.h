/**
 * @file    spi_lcd_nokia.h
 *
 * @brief   API for interfacing with the Nokia 5110 LCD via SPI.
 *
 * This header defines high-level functions used to initialize and control the
 * Nokia 5110 display using the Zephyr SPI and GPIO subsystems. It allows users
 * to send commands, write individual bytes, draw characters or text strings,
 * load full-frame images, and update the display framebuffer.
 *
 * The module abstracts all hardware-level details such as command/data pin
 * switching, reset line control, and SPI transactions, making the display easy
 * to integrate into higher-level UI managers.
 *
 * @par
 * Maciel Morones, David Missael
 * Sistemas Operativos en Tiempo Real
 */

 #include <stdio.h>
#include <zephyr/kernel.h>

#define NOKIA_LCD_ERROR      0u
#define NOKIA_LCD_OK         1u
#define NOKIA_LCD_RESET_ON   0u
#define NOKIA_LCD_RESET_OFF  1u
#define NOKIA_LCD_X          84u
#define NOKIA_LCD_Y          48u
#define NOKIA_LCD_DATA       1u
#define NOKIA_LCD_CMD        0u
#define CHAR_LENGTH          5u

/* Add this after the includes */
uint8_t* LCD_nokia_get_frame_buffer(void);

extern int Nokia_Lcd_Init(void);
/*it clears all the figures in the LCD*/
void LCD_nokia_clear(void);
/*It is used to indicate the place for writing a new character in the LCD. The values that x can take are 0 to 84 and y can take values
 * from 0 to 5*/
void LCD_nokia_goto_xy(uint8_t x, uint8_t y);
/*It allows to write a figure represented by constant array*/
void LCD_nokia_bitmap(const uint8_t bitmap []);
/*It write a character in the LCD*/
void LCD_nokia_send_char(uint8_t);
/*It write a string into the LCD*/
void LCD_nokia_send_string(uint8_t string []);
/*It used in the initialization routine*/
void LCD_nokia_delay(void);
/*Writes a set of bytes at any FrameBuffer position*/
void LCD_nokia_write_xy_FB(uint8_t x, uint8_t y, uint8_t *ptr, uint16_t bytes);
/*Writes a String into any FrameBuffer position*/
void LCD_nokia_write_string_xy_FB(uint8_t x, uint8_t y, uint8_t *ptr);
/*Write single character into any frame buffer position*/
void LCD_nokia_write_char_xy_FB(uint8_t x, uint8_t y, uint8_t character);
/*Sets a pixel on any of the 84x48 LCD range*/
void LCD_nokia_set_pixel(uint8_t x, uint8_t y);
/*Clears a pixel on any of the 84x48 LCD range*/
void LCD_nokia_clear_pixel(uint8_t x, uint8_t y);
/*Clear x number of bytes from x,y point*/
void LCD_nokia_clear_range_FrameBuffer(uint8_t x, uint8_t y, uint16_t bytes);
/*Writes the FrameBuffer to the LCD*/
void LCD_nokia_sent_FrameBuffer();
