/**
 * \file            fan.h
 * \brief           Fan driver file
 */

/*
 * Copyright (c) 2024 Jasper
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file contains the entire fan driver and application.
 *
 * Author:          Jasper <jasperzhangse@gmail.com>
 * Version:         v1.0.0-dev
 */

#ifndef __FAN_H__
#define __FAN_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "lib/type/lib_type.h"
#include "main.h"

#define FAN_LEVEL_0 0
#define FAN_LEVEL_1 50
#define FAN_LEVEL_2 70
#define FAN_LEVEL_3 100

typedef struct {
    uint16_t fan_speed;
    uint16_t last_fan_speed;
    uint16_t fan_status;
} fan_ctrl_t;

status_t fan_init(void);
status_t fan_start(void);
status_t fan_stop(void);
status_t fan_set_speed(uint16_t fan_speed);
status_t fan_set_speed_smooth(uint16_t target_speed);
status_t fan_set_status(uint16_t on_off);
status_t fan_set_level(uint16_t fan_level);

extern fan_ctrl_t g_fan_ctrl;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __FAN_H__ */
