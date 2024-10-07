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

#ifndef __DRV_CST816T_H_
#define __DRV_CST816T_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes */
#include <stdbool.h>
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "lib/type/lib_type.h"
#include "main.h"
#include "queue.h"
#include "semphr.h"

typedef struct {
    struct {
        uint8_t chip_id;
        uint8_t firmware_version;
        uint8_t gesture_id;
        uint8_t finger_num;
        uint16_t x;
        uint16_t y;
        GPIO_TypeDef* rst_port;
        uint16_t rst_pin;
        GPIO_TypeDef* irq_port;
        uint16_t irq_pin;
        bool tp_event;

    } _ctrl;

    status_t (*iic_read)(uint8_t addr, uint8_t reg, uint8_t* buf, uint8_t len);
    status_t (*iic_write)(uint8_t addr, uint8_t reg, uint8_t* buf, uint8_t len);

} cst816t_hdl_t;

typedef enum {
    EN_TOUCH = 0,
    EN_CHANGE,
    EN_MOTION,
    EN_MOTION_LONGPRESS,
} tp_mode_t;

status_t drv_cst816t_init(cst816t_hdl_t* cst816t_hdl, uint8_t tp_mode);
bool cst816t_available(cst816t_hdl_t* cst816t_hdl);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __DRV_CST816T_H_ */
