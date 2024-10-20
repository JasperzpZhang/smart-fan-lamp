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
 * @file      sc_lvgl.c
 * @brief     Implementation File for lvgl Module
 * @version   1.0.0
 * @author    Jasper
 * @date      2024-09-28
 */

/**
   modification history
   --------------------
   01a, 28Sep24, Jasper Created
 */

#ifndef __LVGL_H__
#define __LVGL_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes */
#include "FreeRTOS.h"
#include "lib/type/lib_type.h"
#include "task.h"
#include "custom.h"
#include "lib/lvgl/lvgl.h"

typedef struct {
    uint8_t main_sw;
    uint8_t slider_brightness;
    uint8_t slider_color;
} scr_lvgl_ctrl_t;

extern scr_lvgl_ctrl_t g_scr_lvgl_ctrl;

extern lv_ui guider_ui;

extern TaskHandle_t g_lvgl_task_hdl;

status_t lvgl_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LVGL_H__ */
