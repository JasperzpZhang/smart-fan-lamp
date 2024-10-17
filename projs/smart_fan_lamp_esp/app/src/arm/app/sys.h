/**
 * \file            sys.h
 * \brief           System file
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
 * This file includes all the app functions for system.
 *
 * Author:          Jasper <jasperzhangse@gmail.com>
 * Version:         v1.0.0-dev
 */

#ifndef __SYS_H__
#define __SYS_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes */
#include "lib/type/lib_type.h"

/* Defines */
#define SYS_TASK_DELAY 5 /* ms */

typedef enum {
    SYS_EVENT_IDLE,
    SYS_EVENT_LED_OFF,
    SYS_EVENT_LED_ON,
    SYS_EVENT_NIGHT_LIGHT_OFF,
    SYS_EVENT_NIGHT_LIGHT_ON,
    SYS_EVENT_FAN_OFF,
    SYS_EVENT_FAN_ON,
    SYS_EVENT_TIMER_STOP,
    SYS_EVENT_TIMER_START,
    SYS_EVENT_LED_SET_BRIGHTNESS,
    SYS_EVENT_LED_SET_COLOR_TEMP,
    SYS_EVENT_FAN_SET_SPEED,
    SYS_EVENT_SLIDER_SWITCH_TARGET
} sys_event_type_t;

typedef enum {
    SYS_MODE_IDLE,
    SYS_MODE_LED_BRIGHT,
    SYS_MODE_LED_COLOR,
    SYS_MODE_NIGHT_LIGHT,
    SYS_MODE_FAN,
    SYS_MODE_TIMER,
    SYS_MODE_SWITCH
} sys_mode_t;

typedef enum { SOURCE_PANEL, SOURCE_SCREEN, SOURCE_VOICE, SOURCE_OTHER } sys_enent_source_t;

typedef enum { LED_OFF, LED_ON } led_state_t;

typedef enum { FAN_OFF, FAN_ON } fan_state_t;

typedef enum { TIMER_STOPPED, TIMER_RUNNING } timer_state_t;

typedef struct {
    struct {
        led_state_t state;
        uint16_t brightness;
        uint16_t last_brightness;
        uint16_t color_temperature;
        uint16_t last_color_temperature;
    } led;

    struct {
        fan_state_t state;
        uint16_t speed;
        uint16_t last_speed;
    } fan;

    struct {
        timer_state_t state;
    } timer;

    panel_slider_target_t slider_target;
} sys_ctrl_t;

typedef struct {

    uint8_t source;        // 消息来源
    sys_event_type_t type; // 消息类型
    uint16_t value_data;   // 用于传递数值，如亮度、色温、风扇速度
    uint8_t mode;
    panel_slider_target_t slider_target;
} sys_msg_t;

/* Functions */
status_t sys_init(void);
void charge_set_sattus(uint8_t charge_status);
void charge_save_sattus(void);

status_t sys_led_on(sys_enent_source_t source);
status_t sys_led_off(sys_enent_source_t source);
status_t sys_led_set_brightness(sys_enent_source_t source, uint16_t brightness);
status_t sys_led_set_color_temp(sys_enent_source_t source, uint16_t color_temperature);
status_t sys_night_light_on(sys_enent_source_t source);
status_t sys_night_light_off(sys_enent_source_t source);
status_t sys_fan_on(sys_enent_source_t source);
status_t sys_fan_off(sys_enent_source_t source);
status_t sys_fan_set_speed(sys_enent_source_t source, uint16_t speed);
status_t sys_timer_start(sys_enent_source_t source);
status_t sys_timer_stop(sys_enent_source_t source);
status_t sys_slider_switch_target(sys_enent_source_t source, panel_slider_target_t slider_target);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __SYS_H__ */
