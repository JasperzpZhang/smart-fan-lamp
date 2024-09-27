/**
 * \file            led.h
 * \brief           Led driver and application file
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
 * This file contains the entire led driver and application.
 *
 * Author:          Jasper <jasperzhangse@gmail.com>
 * Version:         v1.0.0-dev
 */
 
#ifndef __LED_H__
#define __LED_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "stm32f1xx_hal.h"
#include "lib/type/lib_type.h"

    extern uint16_t g_last_led_brightness;
    extern uint16_t g_last_led_color_temperature;

    status_t led_init(void);
    status_t adjust_led_brightness(uint16_t led_brightness);
    status_t adjust_led_color_temperature(uint16_t led_color_temperature);
    status_t save_led_data(void);
    status_t stop_led_pwm(void);
    status_t start_led_pwm(void);
    status_t toggle_led(uint16_t on_off);
    status_t smooth_adjust_led_brightness(uint16_t target_led_brightness);
    status_t smooth_adjust_led_color_temperature(uint16_t target_led_color_temperature);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LED_H__ */
