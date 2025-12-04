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




int main(void)
{	
	if(!Nokia_Lcd_Init()){
		printk("SPI Device not working\n\r");
		return 0;	
	}

	LCD_nokia_bitmap(inflames_name_84x48_Bitmap);
	k_msleep(3000);
	LCD_nokia_clear();
	
	drawline(20,20,40,5,100);
	LCD_nokia_sent_FrameBuffer();

	return 0;
}
