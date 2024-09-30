/*****************************************************************************
* | File        :   LCD_1IN83.h
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :   Used to shield the underlying layers of each master and enhance portability
*----------------
* | This version:   V1.0
* | Date        :   2024-01-05
* | Info        :   Basic version
*
******************************************************************************/
#ifndef __LCD_1IN83_H
#define __LCD_1IN83_H

#include <stdint.h>
#include "drv/peri/sc/lcd_1in83/drv_lcd_1in83_cfg.h"
#include "main.h"

#include <stdio.h>
#include <stdlib.h> //itoa()

#define LCD_1IN83_HEIGHT 280
#define LCD_1IN83_WIDTH  240

#define HORIZONTAL       0
#define VERTICAL         1

#define LCD_1IN83_CS_0   DEV_Digital_Write(DEV_CS_PIN, 0)
#define LCD_1IN83_CS_1   DEV_Digital_Write(DEV_CS_PIN, 1)

#define LCD_1IN83_RST_0  DEV_Digital_Write(DEV_RST_PIN, 0)
#define LCD_1IN83_RST_1  DEV_Digital_Write(DEV_RST_PIN, 1)

#define LCD_1IN83_DC_0   DEV_Digital_Write(DEV_DC_PIN, 0)
#define LCD_1IN83_DC_1   DEV_Digital_Write(DEV_DC_PIN, 1)

typedef struct {
    uint16_t WIDTH;
    uint16_t HEIGHT;
    uint8_t SCAN_DIR;
} LCD_1IN83_ATTRIBUTES;

extern LCD_1IN83_ATTRIBUTES LCD_1IN83;

/********************************************************************************
function:   Macro definition variable name
********************************************************************************/
void lcd_1in83_init(uint8_t Scan_dir);
void lcd_1in83_clear(uint16_t Color);
void lcd_1in83_display(uint16_t* Image);
void lcd_1in83_display_windows(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint16_t* Image);
void lcd_1in83_draw_point(uint16_t X, uint16_t Y, uint16_t Color);
void lcd_1in83_set_backlight(uint16_t Value);
#endif
