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

#define WHITE            0xFFFF
#define BLACK            0x0000
#define BLUE             0x001F
#define BRED             0XF81F
#define GRED             0XFFE0
#define GBLUE            0X07FF
#define RED              0xF800
#define MAGENTA          0xF81F
#define GREEN            0x07E0
#define CYAN             0x7FFF
#define YELLOW           0xFFE0
#define BROWN            0XBC40
#define BRRED            0XFC07
#define GRAY             0X8430
#define DARKBLUE         0X01CF
#define LIGHTBLUE        0X7D7C
#define GRAYBLUE         0X5458
#define LIGHTGREEN       0X841F
#define LGRAY            0XC618
#define LGRAYBLUE        0XA651
#define LBBLUE           0X2B12

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
