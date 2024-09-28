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
 * @file      drv_tp.c
 * @brief     Implementation File for TouchPad Module
 * @version   1.0.0
 * @author    Jasper
 * @date      2024-09-26
 */

/**
   modification history
   --------------------
   01a, 26Sep24, Jasper Created
 */

#ifndef __TP_H__
#define __TP_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes */
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "lib/type/lib_type.h"
#include "main.h"
#include "queue.h"

typedef struct {
    uint8_t slider_value;

    struct {
        uint8_t _KEY_LED1 : 1;
        uint8_t _KEY_LED2 : 1;
        uint8_t _KEY_LED3 : 1;
        uint8_t _KEY_LED4 : 1;
        uint8_t _KEY_LED5 : 1;
        uint8_t _KEY_LED6 : 1;
        uint8_t _KEY_LED7 : 1;
    } key;

} led_msg_t;

typedef struct {
    uint16_t _TP_KEY1 : 1;
    uint16_t _TP_KEY4 : 1;
    uint16_t _TP_KEY5 : 1;
    uint16_t _TP_KEY6 : 1;
    uint16_t _TP_KEY7 : 1;
    uint16_t _TP_KEY8 : 1;
    uint16_t _TP_KEY9 : 1;
} ctrl_msg_tp_t;

typedef struct {
    uint8_t slider_value;
    uint8_t slider_en;
    ctrl_msg_tp_t tp;
} ctrl_msg_t;

extern QueueHandle_t g_led_queue;
extern QueueHandle_t g_ctrl_queue;

void tp_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TP_H__ */
