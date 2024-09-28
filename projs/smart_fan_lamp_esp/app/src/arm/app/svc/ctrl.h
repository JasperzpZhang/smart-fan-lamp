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
 * @file      ctrl.h
 * @brief     Implementation File for ctrl Module
 * @version   1.0.0
 * @author    Jasper
 * @date      2024-09-26
 */

/**
   modification history
   --------------------
   01a, 26Sep24, Jasper Created
 */

#ifndef __CTRL_H__
#define __CTRL_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes */
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "lib/type/lib_type.h"
#include "main.h"
#include "queue.h"

typedef enum {
    MODE_LED_BRIGHT,
    MODE_LED_COLOR,
    MODE_FAN,
} ctrl_slider_target_t;

typedef enum {
    MAIN_PWR_OFF,
    MAIN_PWR_ON,
    NIGHT_LIGHT_OFF,
    NIGHT_LIGHT_ON,
    USB_CHARGE_OFF,
    USB_CHARGE_ON,
    FAN_OFF,
    FAN_ON,
} ctrl_status_t;

typedef struct {
    uint16_t _SW_MAIN        : 1;
    uint16_t _SW_FAN         : 1;
    uint16_t _SW_NIGHT_LIGHT : 1;
    uint16_t _SW_MODE        : 1;
} ctrl_sw_t;

typedef struct {
    ctrl_slider_target_t slider_target;
    ctrl_status_t status;
    ctrl_sw_t sw;
} ctrl_t;

//typedef struct {
//    uint32_t _LED1  : 1;
//    uint32_t _LED2  : 1;
//    uint32_t _LED3  : 1;
//    uint32_t _LED4  : 1;
//    uint32_t _LED5  : 1;
//    uint32_t _LED6  : 1;
//    uint32_t _LED7  : 1;
//    uint32_t _LED8  : 1;
//    uint32_t _LED9  : 1;
//    uint32_t _LED10 : 1;
//    uint32_t _LED11 : 1;
//    uint32_t _LED12 : 1;
//    uint32_t _LED13 : 1;
//    uint32_t _LED14 : 1;
//    uint32_t _LED15 : 1;
//    uint32_t _LED16 : 1;
//    uint32_t _LED17 : 1;
//} ctrl_led_t;

status_t ctrl_init(void);

extern ctrl_t g_ctrl;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CTRL_H__ */
