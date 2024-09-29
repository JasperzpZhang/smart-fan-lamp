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
 * @file      panel.h
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

#ifndef __PANEL_H__
#define __PANEL_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes */
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "lib/type/lib_type.h"
#include "main.h"
#include "queue.h"

#define KEY_LED_ON              GPIO_PIN_RESET
#define KEY_LED_OFF             GPIO_PIN_SET

#define PANEL_TP_LED1_GPIO_Port KEY_LED12_GPIO_Port
#define PANEL_TP_LED1_Pin       KEY_LED12_Pin
#define PANEL_TP_LED2_GPIO_Port KEY_LED13_GPIO_Port
#define PANEL_TP_LED2_Pin       KEY_LED13_Pin
#define PANEL_TP_LED3_GPIO_Port KEY_LED14_GPIO_Port
#define PANEL_TP_LED3_Pin       KEY_LED14_Pin
#define PANEL_TP_LED4_GPIO_Port KEY_LED15_GPIO_Port
#define PANEL_TP_LED4_Pin       KEY_LED15_Pin

#define PANEL_TP_IDX1_GPIO_Port KEY_LED11_GPIO_Port
#define PANEL_TP_IDX1_Pin       KEY_LED11_Pin
#define PANEL_TP_IDX2_GPIO_Port KEY_LED16_GPIO_Port
#define PANEL_TP_IDX2_Pin       KEY_LED16_Pin
#define PANEL_TP_IDX3_GPIO_Port KEY_LED17_GPIO_Port
#define PANEL_TP_IDX3_Pin       KEY_LED17_Pin

#define PANEL_SET_MAIN_SW_LED(x)                                                                                       \
    HAL_GPIO_WritePin(PANEL_TP_LED1_GPIO_Port, PANEL_TP_LED1_Pin, x ? KEY_LED_ON : KEY_LED_OFF);
#define PANEL_SET_NIGHT_LIGHT_LED(x)                                                                                   \
    HAL_GPIO_WritePin(PANEL_TP_LED2_GPIO_Port, PANEL_TP_LED2_Pin, x ? KEY_LED_ON : KEY_LED_OFF);
#define PANEL_SET_FAN_LED(x)                                                                                           \
    HAL_GPIO_WritePin(PANEL_TP_LED3_GPIO_Port, PANEL_TP_LED3_Pin, x ? KEY_LED_ON : KEY_LED_OFF);
#define PANEL_SET_MODE_SW_LED(x)                                                                                       \
    HAL_GPIO_WritePin(PANEL_TP_LED4_GPIO_Port, PANEL_TP_LED4_Pin, x ? KEY_LED_ON : KEY_LED_OFF);

#define PANEL_SET_IDX_BRIGHTNESS_LED(x)                                                                                \
    HAL_GPIO_WritePin(PANEL_TP_IDX1_GPIO_Port, PANEL_TP_IDX1_Pin, x ? KEY_LED_ON : KEY_LED_OFF);
#define PANEL_SET_IDX_COLOR_LED(x)                                                                                     \
    HAL_GPIO_WritePin(PANEL_TP_IDX2_GPIO_Port, PANEL_TP_IDX2_Pin, x ? KEY_LED_ON : KEY_LED_OFF);
#define PANEL_SET_IDX_FAN_LED(x)                                                                                       \
    HAL_GPIO_WritePin(PANEL_TP_IDX3_GPIO_Port, PANEL_TP_IDX3_Pin, x ? KEY_LED_ON : KEY_LED_OFF);

typedef enum {
    panel_led_off = 0,
    panel_led_on = 1,
} panel_led_status_t;

typedef enum {
    main_sw = 0,
    night_light = 1,
    fan = 2,
    mode_sw = 4,
    idx_brightness = 5,
    idx_color = 6,
    idx_fan = 7,
} panel_led_target_t;

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
    uint8_t slider_led_line_value;
} panel_ctrl_t;

status_t panel_init(void);
status_t slider_set_led_line_smooth(uint8_t value);
status_t slider_blk_set_led_line_smooth(uint8_t value);
status_t panel_set_led_status(panel_led_target_t led_target, panel_led_status_t led_status);

extern panel_ctrl_t g_panel_ctrl;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PANEL_H__ */
