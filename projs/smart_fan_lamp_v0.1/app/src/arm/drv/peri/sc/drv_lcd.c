/**
 * Copyright (c) 2024 Jasper All rights reserved
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 *
 * @file      drv_lcd.c
 * @brief     Implementation File for lcd Module
 * @version   1.0.0
 * @author    Jasper
 * @date      2024-09-26
 */

/**
   modification history
   --------------------
   01a, 26Sep24, Jasper Created
 */

#include "drv/peri/sc/drv_lcd.h"

/* Debug config */
#if TP_DEBUG
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* TP_DEBUG */
#if TP_ASSERT
#undef ASSERT
#define ASSERT(a)                                                                                                      \
    while (1) {                                                                                                        \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                    \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* TP_ASSERT */

void
lcd_init(void) {
    DEV_Module_Init();
    lcd_1in83_set_backlight(100);
    lcd_1in83_init(HORIZONTAL); // HORIZONTAL VERTICAL
    lcd_1in83_clear(WHITE);
}

#if 0
void
lcd_1in83_test() { 

    DEV_Module_Init();
    lcd_1in83_set_backlight(100);
    lcd_1in83_init(HORIZONTAL); // HORIZONTAL VERTICAL
    lcd_1in83_clear(BLACK);

    Paint_NewImage(LCD_1IN83_WIDTH, LCD_1IN83_HEIGHT, 0, WHITE);

    Paint_SetClearFuntion(lcd_1in83_clear);
    Paint_SetDisplayFuntion(lcd_1in83_draw_point);

    Paint_Clear(WHITE);
    DEV_DELAY_MS(100);

    lcd_1in83_display_windows(30, 30, 200, 200, (uint16_t*)gImage_1);

    // Paint_DrawString_EN(30, 10, "123", &Font24, YELLOW, RED);
    // Paint_DrawString_EN(30, 34, "ABC", &Font24, BLUE, CYAN);
    // Paint_DrawFloatNum(30, 58, 987.652, 3, &Font12, WHITE, BLACK);

    // Paint_DrawString_CN(50, 180, "12345", &Font24CN, WHITE, RED);
    // Paint_DrawImage(gImage_1, 25, 70, 60, 60);

    // Paint_DrawRectangle(125, 10, 225, 58, RED, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    // Paint_DrawLine(125, 10, 225, 58, MAGENTA, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
    // Paint_DrawLine(225, 10, 125, 58, MAGENTA, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
    // Paint_DrawCircle(150, 100, 25, BLUE, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    // Paint_DrawCircle(180, 100, 25, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    // Paint_DrawCircle(210, 100, 25, RED, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    // Paint_DrawCircle(165, 125, 25, YELLOW, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    // Paint_DrawCircle(195, 125, 25, GREEN, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);

    // DEV_DELAY_MS(3000);

    //    DEV_Module_Exit();
}

#endif
