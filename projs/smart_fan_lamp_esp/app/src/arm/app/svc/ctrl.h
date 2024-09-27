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
    PWR_OFF,
    PWR_ON,
    LED_OFF,
    LED_ON,
    FAN_OFF,
    FAN_ON,
    NIGHT_LIGHT_OFF,
    NIGHT_LIGHT_ON,
    LED_BRIGHT_CTRL,
    LED_COLOR_CTRL,
} ctrl_status_t;

typedef struct {
    ctrl_status_t slider_target;

    struct {
        uint32_t _LED1  : 1;
        uint32_t _LED2  : 1;
        uint32_t _LED3  : 1;
        uint32_t _LED4  : 1;
        uint32_t _LED5  : 1;
        uint32_t _LED6  : 1;
        uint32_t _LED7  : 1;
        uint32_t _LED8  : 1;
        uint32_t _LED9  : 1;
        uint32_t _LED10 : 1;
        uint32_t _LED11 : 1;
        uint32_t _LED12 : 1;
        uint32_t _LED13 : 1;
        uint32_t _LED14 : 1;
        uint32_t _LED15 : 1;
        uint32_t _LED16 : 1;
        uint32_t _LED17 : 1;
    } led;

    struct {
        uint16_t _PWR         : 1;
        uint16_t _LED_SW      : 1;
        uint16_t _LED_CTRL    : 1;
        uint16_t _FAN_CTRL    : 1;
        uint16_t _NIGHT_LIGHT : 1;
        uint16_t _USB         : 1;
    } status;

} ctrl_t;

status_t ctrl_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CTRL_H__ */
