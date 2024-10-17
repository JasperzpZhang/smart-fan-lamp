/**
 * \file            sys.c
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

#include "app/include.h"

#include "drv/peri/sc/lcd_1in83/drv_lcd_1in83.h"

#include "lvgl/lvgl.h"

#include "task.h"

/* Debug config */
#if SYS_DEBUG || 1
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* SYS_DEBUG */
#if SYS_ASSERT
#undef ASSERT
#define ASSERT(a)                                                                                                      \
    while (1) {                                                                                                        \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                    \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* SYS_ASSERT */

/* Local defines */

/* global variable */
QueueHandle_t g_sys_queue;
QueueHandle_t g_daemon_queue;
sys_ctrl_t g_sys_ctrl;

/* Private variables ---------------------------------------------------------*/
static sys_msg_t g_sys_msg;
static sys_msg_t g_daemon_msg;

//lv_mem_monitor_t mem_mon;

/* Forward functions */

static status_t prv_sys_led_on_from_panel(void);
static status_t prv_sys_led_on_from_screen(void);
static status_t prv_sys_led_off_from_panel(void);
static status_t prv_sys_led_off_from_screen(void);
static status_t prv_sys_led_set_brightness_from_panel(uint16_t brightness);
static status_t prv_sys_led_set_brightness_from_screen(uint16_t brightness);
static status_t prv_sys_led_set_color_temp_from_panel(uint16_t color_temperature);
static status_t prv_sys_led_set_color_temp_from_screen(uint16_t color_temperature);

static status_t prv_sys_night_light_on_from_panel(void);
static status_t prv_sys_night_light_off_from_panel(void);

static status_t prv_sys_fan_on_from_panel(void);
static status_t prv_sys_fan_on_from_screen(void);
static status_t prv_sys_fan_off_from_panel(void);
static status_t prv_sys_fan_off_from_screen(void);
static status_t prv_sys_fan_set_speed_from_panel(uint16_t speed);
static status_t prv_sys_fan_set_speed_from_screen(uint16_t speed);

static status_t prv_sys_timer_start_from_panel(void);
static status_t prv_sys_timer_start_from_screen(void);
static status_t prv_sys_timer_stop_from_panel(void);
static status_t prv_sys_timer_stop_from_screen(void);

static status_t prv_sys_slider_switch_target_from_panel(panel_slider_target_t slider_target);

/* daemon functions */
static status_t prv_daemon_led_on_from_panel(void);
static status_t prv_daemon_led_on_from_screen(void);
static status_t prv_daemon_led_off_from_panel(void);
static status_t prv_daemon_led_off_from_screen(void);
static status_t prv_daemon_led_set_brightness_from_panel(uint16_t brightness);
static status_t prv_daemon_led_set_brightness_from_screen(uint16_t brightness);
static status_t prv_daemon_led_set_color_temp_from_panel(uint16_t color_temperature);
static status_t prv_daemon_led_set_color_temp_from_screen(uint16_t color_temperature);

static status_t prv_daemon_night_light_on_from_panel(void);
static status_t prv_daemon_night_light_off_from_panel(void);

static status_t prv_daemon_fan_on_from_panel(void);
static status_t prv_daemon_fan_on_from_screen(void);
static status_t prv_daemon_fan_off_from_panel(void);
static status_t prv_daemon_fan_off_from_screen(void);
static status_t prv_daemon_fan_set_speed_from_panel(uint16_t speed);
static status_t prv_daemon_fan_set_speed_from_screen(uint16_t speed);

static status_t prv_daemon_slider_switch_target_from_panel(panel_slider_target_t slider_target);

static void sys_task(void* parameter);
static void daemon_task(void* parameter);
static void prv_proc(const sys_msg_t* msg);
static void prv_daemon_proc(const sys_msg_t* msg);

/* Functions */
status_t
sys_init(void) {
    /* charge init */
    g_sys_ctrl.led.brightness = th_led_brightness;
    g_sys_ctrl.led.last_brightness = th_led_brightness;
    g_sys_ctrl.led.color_temperature = th_led_color_temperature;
    g_sys_ctrl.led.last_color_temperature = th_led_color_temperature;
    g_sys_ctrl.fan.speed = th_fan_speed;
    g_sys_ctrl.fan.last_speed = th_fan_speed;

    g_sys_queue = xQueueCreate(20, sizeof(sys_msg_t));
    g_daemon_queue = xQueueCreate(20, sizeof(sys_msg_t));

    xTaskCreate(sys_task, "sys_task", 256, NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(daemon_task, "daemon_task", 128, NULL, tskIDLE_PRIORITY + 1, NULL);

    return status_ok;
}

status_t
sys_led_on(sys_enent_source_t source) {
    TRACE("sys led on\n");
    sys_msg_t msg;
    msg.source = source;
    msg.type = SYS_EVENT_LED_ON;
    msg.mode = SYS_MODE_LED_BRIGHT;

    BaseType_t sys_queue_status = xQueueSend(g_sys_queue, &msg, portMAX_DELAY);
    BaseType_t daemon_queue_status = xQueueSend(g_daemon_queue, &msg, portMAX_DELAY);

    if (sys_queue_status == pdPASS && daemon_queue_status == pdPASS) {
        return status_ok;
    }

    return status_err;
}

status_t
sys_led_off(sys_enent_source_t source) {
    TRACE("sys led off\n");
    sys_msg_t msg;
    msg.source = source;
    msg.type = SYS_EVENT_LED_OFF;
    msg.mode = SYS_MODE_LED_BRIGHT;

    BaseType_t sys_queue_status = xQueueSend(g_sys_queue, &msg, portMAX_DELAY);
    BaseType_t daemon_queue_status = xQueueSend(g_daemon_queue, &msg, portMAX_DELAY);

    if (sys_queue_status == pdPASS && daemon_queue_status == pdPASS) {
        return status_ok;
    }

    return status_err;
}

status_t
sys_night_light_on(sys_enent_source_t source) {
    TRACE("sys night light on\n");
    sys_msg_t msg;
    msg.source = source;
    msg.type = SYS_EVENT_NIGHT_LIGHT_ON;
    msg.mode = SYS_MODE_NIGHT_LIGHT;

    BaseType_t sys_queue_status = xQueueSend(g_sys_queue, &msg, portMAX_DELAY);
    BaseType_t daemon_queue_status = xQueueSend(g_daemon_queue, &msg, portMAX_DELAY);

    if (sys_queue_status == pdPASS && daemon_queue_status == pdPASS) {
        return status_ok;
    }

    return status_err;
}

status_t
sys_night_light_off(sys_enent_source_t source) {
    TRACE("sys night light off\n");
    sys_msg_t msg;
    msg.source = source;
    msg.type = SYS_EVENT_NIGHT_LIGHT_OFF;
    msg.mode = SYS_MODE_NIGHT_LIGHT;

    BaseType_t sys_queue_status = xQueueSend(g_sys_queue, &msg, portMAX_DELAY);
    BaseType_t daemon_queue_status = xQueueSend(g_daemon_queue, &msg, portMAX_DELAY);

    if (sys_queue_status == pdPASS && daemon_queue_status == pdPASS) {
        return status_ok;
    }

    return status_err;
}

status_t
sys_led_set_brightness(sys_enent_source_t source, uint16_t brightness) {
    sys_msg_t msg;
    msg.source = source;
    msg.type = SYS_EVENT_LED_SET_BRIGHTNESS;
    msg.mode = SYS_MODE_LED_BRIGHT;
    msg.value_data = brightness;

    BaseType_t sys_queue_status = xQueueSend(g_sys_queue, &msg, portMAX_DELAY);
    BaseType_t daemon_queue_status = xQueueSend(g_daemon_queue, &msg, portMAX_DELAY);

    if (sys_queue_status == pdPASS && daemon_queue_status == pdPASS) {
        return status_ok;
    }

    return status_err;
}

status_t
sys_led_set_color_temp(sys_enent_source_t source, uint16_t color_temperature) {
    sys_msg_t msg;
    msg.source = source;
    msg.type = SYS_EVENT_LED_SET_COLOR_TEMP;
    msg.mode = SYS_MODE_LED_COLOR;
    msg.value_data = color_temperature;

    BaseType_t sys_queue_status = xQueueSend(g_sys_queue, &msg, portMAX_DELAY);
    BaseType_t daemon_queue_status = xQueueSend(g_daemon_queue, &msg, portMAX_DELAY);

    if (sys_queue_status == pdPASS && daemon_queue_status == pdPASS) {
        return status_ok;
    }

    return status_err;
}

status_t
sys_fan_on(sys_enent_source_t source) {
    sys_msg_t msg;
    msg.source = source;
    msg.type = SYS_EVENT_FAN_ON;
    msg.mode = SYS_MODE_FAN;

    BaseType_t sys_queue_status = xQueueSend(g_sys_queue, &msg, portMAX_DELAY);
    BaseType_t daemon_queue_status = xQueueSend(g_daemon_queue, &msg, portMAX_DELAY);

    if (sys_queue_status == pdPASS && daemon_queue_status == pdPASS) {
        TRACE("sys fan on queue send successfully.\n");
        return status_ok;
    }

    TRACE("sys fan on queue send failed.\n");
    return status_err;
}

status_t
sys_fan_off(sys_enent_source_t source) {
    sys_msg_t msg;
    msg.source = source;
    msg.type = SYS_EVENT_FAN_OFF;
    msg.mode = SYS_MODE_FAN;

    BaseType_t sys_queue_status = xQueueSend(g_sys_queue, &msg, portMAX_DELAY);
    BaseType_t daemon_queue_status = xQueueSend(g_daemon_queue, &msg, portMAX_DELAY);

    if (sys_queue_status == pdPASS && daemon_queue_status == pdPASS) {
        TRACE("sys fan off queue send successfully.\n");
        return status_ok;
    }

    TRACE("sys fan off queue send failed.\n");
    return status_err;
}

status_t
sys_fan_set_speed(sys_enent_source_t source, uint16_t speed) {
    sys_msg_t msg;
    msg.source = source;
    msg.type = SYS_EVENT_FAN_SET_SPEED;
    msg.mode = SYS_MODE_FAN;
    msg.value_data = speed;

    BaseType_t sys_queue_status = xQueueSend(g_sys_queue, &msg, portMAX_DELAY);
    BaseType_t daemon_queue_status = xQueueSend(g_daemon_queue, &msg, portMAX_DELAY);

    if (sys_queue_status == pdPASS && daemon_queue_status == pdPASS) {
        return status_ok;
    }

    return status_err;
}

status_t
sys_timer_start(sys_enent_source_t source) {
    sys_msg_t msg;
    msg.source = source;
    msg.type = SYS_EVENT_TIMER_START;
    msg.mode = SYS_MODE_TIMER;

    BaseType_t sys_queue_status = xQueueSend(g_sys_queue, &msg, portMAX_DELAY);
    BaseType_t daemon_queue_status = xQueueSend(g_daemon_queue, &msg, portMAX_DELAY);

    if (sys_queue_status == pdPASS && daemon_queue_status == pdPASS) {
        return status_ok;
    }

    return status_err;
}

status_t
sys_timer_stop(sys_enent_source_t source) {
    sys_msg_t msg;
    msg.source = source;
    msg.type = SYS_EVENT_TIMER_STOP;
    msg.mode = SYS_MODE_TIMER;

    BaseType_t sys_queue_status = xQueueSend(g_sys_queue, &msg, portMAX_DELAY);
    BaseType_t daemon_queue_status = xQueueSend(g_daemon_queue, &msg, portMAX_DELAY);

    if (sys_queue_status == pdPASS && daemon_queue_status == pdPASS) {
        return status_ok;
    }

    return status_err;
}

status_t
sys_slider_switch_target(sys_enent_source_t source, panel_slider_target_t slider_target) {
    sys_msg_t msg;
    msg.source = source;
    msg.slider_target = slider_target;
    msg.type = SYS_EVENT_SLIDER_SWITCH_TARGET;
    msg.mode = SYS_MODE_SWITCH;

    BaseType_t sys_queue_status = xQueueSend(g_sys_queue, &msg, portMAX_DELAY);
    BaseType_t daemon_queue_status = xQueueSend(g_daemon_queue, &msg, portMAX_DELAY);

    if (sys_queue_status == pdPASS && daemon_queue_status == pdPASS) {
        return status_ok;
    }

    return status_err;
}

static void
sys_task(void* parameter) {
    sys_msg_t sys_msg;
    sys_msg.type = SYS_EVENT_IDLE;
    g_sys_msg.type = SYS_EVENT_IDLE;

    while (1) {
        if (xQueueReceive(g_sys_queue, &sys_msg, 0) == pdPASS) {
            if (sys_msg.mode != g_sys_msg.mode && g_sys_msg.type != SYS_EVENT_IDLE) {
                while (g_sys_msg.type != SYS_EVENT_IDLE) {
                    prv_proc(&g_sys_msg);
                    osDelay(SYS_TASK_DELAY);
                }
            }
            g_sys_msg = sys_msg;
            TRACE("g_sys_msg.type : %d\n", g_sys_msg.type);
        }

        if (g_sys_msg.type != SYS_EVENT_IDLE) {
            prv_proc(&g_sys_msg);
        }

        wdog_feed();
        osDelay(SYS_TASK_DELAY);
    }
}

static void
daemon_task(void* parameter) {
    sys_msg_t msg;
    while (1) {
        if (xQueueReceive(g_daemon_queue, &msg, 0) == pdPASS) {
            if (msg.mode != g_daemon_msg.mode && g_daemon_msg.type != SYS_EVENT_IDLE) {
                while (g_daemon_msg.type != SYS_EVENT_IDLE) {
                    prv_daemon_proc(&g_daemon_msg);
                    osDelay(7);
                }
            }
            g_daemon_msg = msg;
            TRACE("g_daemon_msg.type : %d\n", g_daemon_msg.type);
        }

        if (g_daemon_msg.type != SYS_EVENT_IDLE) {
            prv_daemon_proc(&g_daemon_msg);
        }

        osDelay(7);
    }
}

static void
prv_proc(const sys_msg_t* msg) {
    status_t status = status_err;

    switch (msg->type) {
        case SYS_EVENT_IDLE:
            /* do nothing */
            status = status_ok;
            break;
        case SYS_EVENT_LED_ON:
            if (msg->source == SOURCE_PANEL) {
                status = prv_sys_led_on_from_panel();
            } else if (msg->source == SOURCE_SCREEN) {
                status = prv_sys_led_on_from_screen();
            }
            break;

        case SYS_EVENT_LED_OFF:
            if (msg->source == SOURCE_PANEL) {
                status = prv_sys_led_off_from_panel();
            } else if (msg->source == SOURCE_SCREEN) {
                status = prv_sys_led_off_from_screen();
            }
            break;

        case SYS_EVENT_NIGHT_LIGHT_ON:
            if (msg->source == SOURCE_PANEL) {
                status = prv_sys_night_light_on_from_panel();
            } else if (msg->source == SOURCE_SCREEN) {
                /* do noghting */
            }
            break;

        case SYS_EVENT_NIGHT_LIGHT_OFF:
            if (msg->source == SOURCE_PANEL) {
                status = prv_sys_night_light_off_from_panel();
            } else if (msg->source == SOURCE_SCREEN) {
                /* do noghting */
            }
            break;

        case SYS_EVENT_LED_SET_BRIGHTNESS:
            if (msg->source == SOURCE_PANEL) {
                status = prv_sys_led_set_brightness_from_panel(msg->value_data);
            } else if (msg->source == SOURCE_SCREEN) {
                status = prv_sys_led_set_brightness_from_screen(msg->value_data);
            }
            break;

        case SYS_EVENT_LED_SET_COLOR_TEMP:
            if (msg->source == SOURCE_PANEL) {
                status = prv_sys_led_set_color_temp_from_panel(msg->value_data);
            } else if (msg->source == SOURCE_SCREEN) {
                status = prv_sys_led_set_color_temp_from_screen(msg->value_data);
            }
            break;

        case SYS_EVENT_FAN_ON:
            if (msg->source == SOURCE_PANEL) {
                status = prv_sys_fan_on_from_panel();
            } else if (msg->source == SOURCE_SCREEN) {
                status = prv_sys_fan_on_from_screen();
            }
            break;

        case SYS_EVENT_FAN_OFF:
            if (msg->source == SOURCE_PANEL) {
                status = prv_sys_fan_off_from_panel();
            } else if (msg->source == SOURCE_SCREEN) {
                status = prv_sys_fan_off_from_screen();
            }
            break;

        case SYS_EVENT_FAN_SET_SPEED:
            if (msg->source == SOURCE_PANEL) {
                status = prv_sys_fan_set_speed_from_panel(msg->value_data);
            } else if (msg->source == SOURCE_SCREEN) {
                status = prv_sys_fan_set_speed_from_screen(msg->value_data);
            }
            break;

        case SYS_EVENT_TIMER_START:
            if (msg->source == SOURCE_PANEL) {
                status = prv_sys_timer_start_from_panel();
            } else if (msg->source == SOURCE_SCREEN) {
                status = prv_sys_timer_start_from_screen();
            }
            break;

        case SYS_EVENT_TIMER_STOP:
            if (msg->source == SOURCE_PANEL) {
                status = prv_sys_timer_stop_from_panel();
            } else if (msg->source == SOURCE_SCREEN) {
                status = prv_sys_timer_stop_from_screen();
            }
            break;

        case SYS_EVENT_SLIDER_SWITCH_TARGET:
            if (msg->source == SOURCE_PANEL) {
                status = prv_sys_slider_switch_target_from_panel(msg->slider_target);
            } else if (msg->source == SOURCE_SCREEN) {
                /* do nothing */
            }
            break;

        default: status = status_err; break;
    }

    if (status != status_ok) {
        TRACE("sys proc error\n");
    }
}

static void
prv_daemon_proc(const sys_msg_t* msg) {
    status_t status = status_err;

    switch (msg->type) {
        case SYS_EVENT_IDLE:
            /* do nothing */
            break;
        case SYS_EVENT_LED_ON:
            if (msg->source == SOURCE_PANEL) {
                prv_daemon_led_on_from_panel();
            } else if (msg->source == SOURCE_SCREEN) {
            }
            break;

        case SYS_EVENT_LED_OFF:
            if (msg->source == SOURCE_PANEL) {
                prv_daemon_led_off_from_panel();
            } else if (msg->source == SOURCE_SCREEN) {
            }
            break;

        case SYS_EVENT_NIGHT_LIGHT_ON:
            if (msg->source == SOURCE_PANEL) {
                prv_daemon_night_light_on_from_panel();
            } else if (msg->source == SOURCE_SCREEN) {
                /* do noghting */
            }
            break;

        case SYS_EVENT_NIGHT_LIGHT_OFF:
            if (msg->source == SOURCE_PANEL) {
                prv_daemon_night_light_off_from_panel();
            } else if (msg->source == SOURCE_SCREEN) {
                /* do noghting */
            }
            break;

        case SYS_EVENT_LED_SET_BRIGHTNESS:
            if (msg->source == SOURCE_PANEL) {
                prv_daemon_led_set_brightness_from_panel(msg->value_data);
            } else if (msg->source == SOURCE_SCREEN) {
                prv_daemon_led_set_brightness_from_screen(msg->value_data);
            }
            break;

        case SYS_EVENT_LED_SET_COLOR_TEMP:
            if (msg->source == SOURCE_PANEL) {
                prv_daemon_led_set_color_temp_from_panel(msg->value_data);
            } else if (msg->source == SOURCE_SCREEN) {
                prv_daemon_led_set_color_temp_from_screen(msg->value_data);
            }
            break;

        case SYS_EVENT_FAN_ON:
            if (msg->source == SOURCE_PANEL) {
                prv_daemon_fan_on_from_panel();
            } else if (msg->source == SOURCE_SCREEN) {
                prv_daemon_fan_on_from_screen();
            }
            break;

        case SYS_EVENT_FAN_OFF:
            if (msg->source == SOURCE_PANEL) {
                prv_daemon_fan_off_from_panel();
            } else if (msg->source == SOURCE_SCREEN) {
                prv_daemon_fan_off_from_screen();
            }
            break;

        case SYS_EVENT_FAN_SET_SPEED:
            if (msg->source == SOURCE_PANEL) {
                prv_daemon_fan_set_speed_from_panel(msg->value_data);
            } else if (msg->source == SOURCE_SCREEN) {
                prv_daemon_fan_set_speed_from_screen(msg->value_data);
            }
            break;

        case SYS_EVENT_TIMER_START:
            if (msg->source == SOURCE_PANEL) {

            } else if (msg->source == SOURCE_SCREEN) {
            }
            break;

        case SYS_EVENT_TIMER_STOP:
            if (msg->source == SOURCE_PANEL) {

            } else if (msg->source == SOURCE_SCREEN) {
            }
            break;

        case SYS_EVENT_SLIDER_SWITCH_TARGET:
            if (msg->source == SOURCE_PANEL) {
                status = prv_daemon_slider_switch_target_from_panel(msg->slider_target);
            } else if (msg->source == SOURCE_SCREEN) {
                /* do nothing */
            }
            break;

        default: status = status_err; break;
    }

    if (status != status_ok) {}
}

static status_t
prv_sys_led_on_from_panel(void) {
    if (g_sys_ctrl.led.state != LED_ON) {
        TRACE("prv sys led on from panel.\n");
        g_sys_ctrl.led.state = LED_ON;
    }
    prv_sys_led_set_brightness_from_panel(g_sys_ctrl.led.last_brightness);
    return status_ok;
}

static status_t
prv_sys_led_on_from_screen(void) {
    if (g_sys_ctrl.led.state != LED_ON) {
        TRACE("prv sys led on from screen.\n");
        g_sys_ctrl.led.state = LED_ON;
    }
    prv_sys_led_set_brightness_from_panel(g_sys_ctrl.led.last_brightness);

    return status_ok;
}

static status_t
prv_sys_led_off_from_panel(void) {
    if (g_sys_ctrl.led.state != LED_OFF) {
        TRACE("prv sys led off from panel.\n");
        g_sys_ctrl.led.state = LED_OFF;
        g_sys_ctrl.led.last_brightness = g_sys_ctrl.led.brightness;
    }
    prv_sys_led_set_brightness_from_panel(0);
    return status_ok;
}

static status_t
prv_sys_led_off_from_screen(void) {
    if (g_sys_ctrl.led.state != LED_OFF) {
        TRACE("prv sys led off from screen.\n");
        g_sys_ctrl.led.state = LED_OFF;
        g_sys_ctrl.led.last_brightness = g_sys_ctrl.led.brightness;
    }
    prv_sys_led_set_brightness_from_panel(0);
    return status_ok;
}

static status_t
prv_sys_led_set_brightness_from_panel(uint16_t brightness) {
    if (brightness != 0) {
        g_sys_ctrl.led.state = LED_ON;
    } else {
        g_sys_ctrl.led.state = LED_OFF;
    }

    if (led_set_brightness_smooth(brightness) == status_ok) {
        TRACE("prv sys led set brightness smooth from panel status ok.\n");
        if (g_sys_msg.type == SYS_EVENT_LED_SET_BRIGHTNESS) {
            g_sys_ctrl.led.brightness = brightness;
            if (brightness == 0) {
                g_sys_ctrl.led.last_brightness = g_sys_ctrl.led.brightness;
            }
        }
        g_sys_msg.type = SYS_EVENT_IDLE;
    }

    return status_ok;
}

static status_t
prv_sys_led_set_brightness_from_screen(uint16_t brightness) {
    if (brightness != 0) {
        g_sys_ctrl.led.state = LED_ON;
    } else {
        g_sys_ctrl.led.state = LED_OFF;
    }

    if (led_set_brightness_smooth(brightness) == status_ok) {
        TRACE("prv sys led set brightness smooth from screen status ok.\n");
        if (g_sys_msg.type == SYS_EVENT_LED_SET_BRIGHTNESS) {
            g_sys_ctrl.led.brightness = brightness;
            if (brightness == 0) {
                g_sys_ctrl.led.last_brightness = g_sys_ctrl.led.brightness;
            }
        }
        g_sys_msg.type = SYS_EVENT_IDLE;
    }
    return status_ok;
}

static status_t
prv_sys_led_set_color_temp_from_panel(uint16_t color_temperature) {
    if (led_set_color_temperature_smooth(color_temperature) == status_ok) {
        TRACE("prv sys led set color temperature smooth from panel status ok.\n");
        g_sys_msg.type = SYS_EVENT_IDLE;
        g_sys_ctrl.led.color_temperature = color_temperature;
    }
    return status_ok;
}

static status_t
prv_sys_led_set_color_temp_from_screen(uint16_t color_temperature) {
    if (led_set_color_temperature_smooth(color_temperature) == status_ok) {
        TRACE("prv sys led set color temperature smooth from screen status ok.\n");
        g_sys_msg.type = SYS_EVENT_IDLE;
        g_sys_ctrl.led.color_temperature = color_temperature;
    }
    return status_ok;
}

static status_t
prv_sys_night_light_on_from_panel(void) {
    TRACE("prv sys night light on from panel.\n");
    night_light_set_status(1);
    panel_set_sw_led_status(night_light, panel_led_on);
    g_sys_msg.type = SYS_EVENT_IDLE;
    return status_ok;
}

static status_t
prv_sys_night_light_off_from_panel(void) {
    TRACE("prv sys night light off from panel.\n");
    night_light_set_status(0);
    g_sys_msg.type = SYS_EVENT_IDLE;
    return status_ok;
}

static status_t
prv_sys_fan_on_from_panel(void) {
    if (g_sys_ctrl.fan.state != FAN_ON) {
        TRACE("prv sys fan on from panel.\n");
        g_sys_ctrl.fan.state = FAN_ON;
    }
    prv_sys_fan_set_speed_from_panel(g_sys_ctrl.fan.last_speed);
    return status_ok;
}

static status_t
prv_sys_fan_on_from_screen(void) {
    if (g_sys_ctrl.fan.state != FAN_ON) {
        TRACE("prv sys fan on from screen.\n");
        g_sys_ctrl.fan.state = FAN_ON;
    }
    prv_sys_fan_set_speed_from_panel(g_sys_ctrl.fan.last_speed);
    return status_ok;
}

static status_t
prv_sys_fan_off_from_panel(void) {
    if (g_sys_ctrl.fan.state != FAN_OFF) {
        TRACE("prv sys fan off from panel.\n");
        g_sys_ctrl.fan.state = FAN_OFF;
        g_sys_ctrl.fan.last_speed = g_sys_ctrl.fan.speed;
    }
    prv_sys_fan_set_speed_from_panel(0);
    return status_ok;
}

static status_t
prv_sys_fan_off_from_screen(void) {
    if (g_sys_ctrl.fan.state != FAN_OFF) {
        TRACE("prv sys fan off from screen.\n");
        g_sys_ctrl.fan.state = FAN_OFF;
        g_sys_ctrl.fan.last_speed = g_sys_ctrl.fan.speed;
    }
    prv_sys_fan_set_speed_from_panel(0);
    return status_ok;
}

static status_t
prv_sys_fan_set_speed_from_panel(uint16_t speed) {
    if (speed != 0) {
        g_sys_ctrl.fan.state = FAN_ON;
    } else {
        g_sys_ctrl.fan.state = FAN_OFF;
    }

    if (fan_set_speed(speed) == status_ok) {
        TRACE("fan set speed smooth status ok\n");
        if (g_sys_msg.type == SYS_EVENT_FAN_SET_SPEED) {
            g_sys_ctrl.fan.speed = speed;
        }
        g_sys_msg.type = SYS_EVENT_IDLE;
    }
    return status_ok;
}

static status_t
prv_sys_fan_set_speed_from_screen(uint16_t speed) {
    if (speed != 0) {
        g_sys_ctrl.fan.state = FAN_ON;
    } else {
        g_sys_ctrl.fan.state = FAN_OFF;
    }

    if (fan_set_speed(speed) == status_ok) {
        TRACE("fan set speed smooth status ok\n");
        if (g_sys_msg.type == SYS_EVENT_FAN_SET_SPEED) {
            g_sys_ctrl.fan.speed = speed;
        }
        g_sys_msg.type = SYS_EVENT_IDLE;
    }
    return status_ok;
}

static status_t
prv_sys_timer_start_from_panel(void) {
    g_sys_ctrl.timer.state = TIMER_RUNNING;
    return status_ok;
}

static status_t
prv_sys_timer_start_from_screen(void) {
    g_sys_ctrl.timer.state = TIMER_RUNNING;
    return status_ok;
}

static status_t
prv_sys_timer_stop_from_panel(void) {
    g_sys_ctrl.timer.state = TIMER_STOPPED;
    return status_ok;
}

static status_t
prv_sys_timer_stop_from_screen(void) {
    g_sys_ctrl.timer.state = TIMER_STOPPED;
    return status_ok;
}

static status_t
prv_sys_slider_switch_target_from_panel(panel_slider_target_t slider_target) {
    g_sys_ctrl.slider_target = slider_target;
    panel_set_key_mode_sw(slider_target);
    g_sys_msg.type = SYS_EVENT_IDLE;
    return status_ok;
}

/* daemon functions */
static status_t
prv_daemon_led_on_from_panel(void) {
    prv_daemon_led_set_brightness_from_panel(g_sys_ctrl.led.last_brightness);
    return status_ok;
}

static status_t
prv_daemon_led_on_from_screen(void) {
    prv_daemon_led_set_brightness_from_panel(g_sys_ctrl.led.last_brightness);
    return status_ok;
}

static status_t
prv_daemon_led_off_from_panel(void) {
    prv_daemon_led_set_brightness_from_panel(0);
    return status_ok;
}

static status_t
prv_daemon_led_off_from_screen(void) {
    prv_daemon_led_set_brightness_from_panel(0);
    return status_ok;
}

static status_t
prv_daemon_led_set_brightness_from_panel(uint16_t brightness) {
    if (brightness != 0) {
        panel_set_sw_led_status(main_sw, panel_led_on);
    } else {
        panel_set_sw_led_status(main_sw, panel_led_off);
    }
    if (slider_set_led_line_smooth(MODE_LED_BRIGHT, brightness) == status_ok) {
        g_daemon_msg.type = SYS_EVENT_IDLE;
    }
    return status_ok;
}

static status_t
prv_daemon_led_set_brightness_from_screen(uint16_t brightness) {
    if (slider_set_led_line_smooth(MODE_LED_BRIGHT, brightness) == status_ok) {
        g_daemon_msg.type = SYS_EVENT_IDLE;
    }
    return status_ok;
}

static status_t
prv_daemon_led_set_color_temp_from_panel(uint16_t color_temperature) {
    if (slider_set_led_line_smooth(MODE_LED_COLOR, color_temperature) == status_ok) {
        TRACE("prv sys led set color temperature smooth from panel status ok.\n");
        g_daemon_msg.type = SYS_EVENT_IDLE;
    }
    return status_ok;
}

static status_t
prv_daemon_led_set_color_temp_from_screen(uint16_t color_temperature) {
    if (slider_set_led_line_smooth(MODE_LED_COLOR, color_temperature) == status_ok) {
        TRACE("prv sys led set color temperature smooth from screen status ok.\n");
        g_daemon_msg.type = SYS_EVENT_IDLE;
    }
    return status_ok;
}

static status_t
prv_daemon_night_light_on_from_panel(void) {
    panel_set_sw_led_status(night_light, panel_led_on);
    g_daemon_msg.type = SYS_EVENT_IDLE;
    return status_ok;
}

static status_t
prv_daemon_night_light_off_from_panel(void) {
    panel_set_sw_led_status(night_light, panel_led_off);
    g_daemon_msg.type = SYS_EVENT_IDLE;
    return status_ok;
}

static status_t
prv_daemon_fan_on_from_panel(void) {
    prv_daemon_fan_set_speed_from_panel(g_sys_ctrl.fan.last_speed);
    return status_ok;
}

static status_t
prv_daemon_fan_on_from_screen(void) {
    prv_daemon_fan_set_speed_from_panel(g_sys_ctrl.fan.last_speed);
    return status_ok;
}

static status_t
prv_daemon_fan_off_from_panel(void) {
    prv_daemon_fan_set_speed_from_panel(0);
    return status_ok;
}

static status_t
prv_daemon_fan_off_from_screen(void) {
    prv_daemon_fan_set_speed_from_panel(0);
    return status_ok;
}

static status_t
prv_daemon_fan_set_speed_from_panel(uint16_t speed) {
    if (speed != 0) {
        panel_set_sw_led_status(fan, panel_led_on);
    } else {
        panel_set_sw_led_status(fan, panel_led_off);
    }

    if (slider_set_led_line_smooth(MODE_FAN, speed) == status_ok) {
        TRACE("fan set speed smooth status ok\n");
        g_daemon_msg.type = SYS_EVENT_IDLE;
    }
    return status_ok;
}

static status_t
prv_daemon_fan_set_speed_from_screen(uint16_t speed) {
    if (speed != 0) {
        panel_set_sw_led_status(fan, panel_led_on);
    } else {
        panel_set_sw_led_status(fan, panel_led_off);
    }

    if (slider_set_led_line_smooth(MODE_FAN, speed) == status_ok) {
        TRACE("fan set speed smooth status ok\n");
        g_daemon_msg.type = SYS_EVENT_IDLE;
    }
    return status_ok;
}

static status_t
prv_daemon_slider_switch_target_from_panel(panel_slider_target_t slider_target) {
    switch (slider_target) {
        case MODE_LED_BRIGHT:
            panel_set_sw_led_status(idx_brightness, panel_led_on);
            panel_set_sw_led_status(idx_color, panel_led_off);
            panel_set_sw_led_status(idx_fan, panel_led_off);
            if (slider_set_led_line_smooth(MODE_LED_BRIGHT, g_sys_ctrl.led.brightness) == status_ok) {
                g_daemon_msg.type = SYS_EVENT_IDLE;
            }
            break;

        case MODE_LED_COLOR:
            panel_set_sw_led_status(idx_brightness, panel_led_off);
            panel_set_sw_led_status(idx_color, panel_led_on);
            panel_set_sw_led_status(idx_fan, panel_led_off);
            if (slider_set_led_line_smooth(MODE_LED_COLOR, g_sys_ctrl.led.color_temperature) == status_ok) {
                g_daemon_msg.type = SYS_EVENT_IDLE;
            }
            break;

        case MODE_FAN:
            panel_set_sw_led_status(idx_brightness, panel_led_off);
            panel_set_sw_led_status(idx_color, panel_led_off);
            panel_set_sw_led_status(idx_fan, panel_led_on);
            if (slider_set_led_line_smooth(MODE_FAN, g_sys_ctrl.fan.speed) == status_ok) {
                g_daemon_msg.type = SYS_EVENT_IDLE;
            }
            break;
        default:
            /* do nothing */
            break;
    }
    return status_ok;
}

void
charge_set_sattus(uint8_t charge_status) {
    if (charge_status != 0) {
        th_charge_en = 1;
        HAL_GPIO_WritePin(USB_POWER_EN_GPIO_Port, USB_POWER_EN_Pin, GPIO_PIN_SET);
    } else {
        th_charge_en = 0;
        HAL_GPIO_WritePin(USB_POWER_EN_GPIO_Port, USB_POWER_EN_Pin, GPIO_PIN_RESET);
    }
}

void
charge_save_sattus(void) {
    data_save_direct();
}

static void
cli_cmd_charge_save_status(cli_printf cliprintf, int argc, char** argv) {
    if (1 == argc) {
        charge_save_sattus();
        cliprintf("led_save_status ok\r\n");
    } else {
        cliprintf("parameter length error\r\n");
    }
}
CLI_CMD_EXPORT(charge_save, charge save status, cli_cmd_charge_save_status)

static void
cli_cmd_fan_en(cli_printf cliprintf, int argc, char** argv) {
    if (2 == argc) {
        if (atoi(argv[1]) != 0) {
            sys_fan_on(SOURCE_PANEL);
        } else {
            sys_fan_off(SOURCE_PANEL);
        }

        cliprintf("fan_en EN ok\r\n");
    } else {
        cliprintf("parameter length error\r\n");
    }
}
CLI_CMD_EXPORT(fan_en, fan set status, cli_cmd_fan_en)

static void
cli_cmd_fan_set_speed(cli_printf cliprintf, int argc, char** argv) {
    if (2 == argc) {
        uint16_t speed = atoi(argv[1]);

        sys_fan_set_speed(SOURCE_PANEL, speed);

        cliprintf("fan_set_speed SPEED ok\r\n");
    } else {
        cliprintf("parameter length error\r\n");
    }
}
CLI_CMD_EXPORT(fan_set_speed, fan set speed, cli_cmd_fan_set_speed)

static void
cli_cmd_led_set_bright(cli_printf cliprintf, int argc, char** argv) {
    if (2 == argc) {
        uint16_t bright = atoi(argv[1]);
        cliprintf("bright : %d\n", bright);

        status_t status = sys_led_set_brightness(SOURCE_PANEL, bright);

        if (status == status_ok) {
            TRACE("status_ok\n");

        } else if (status == status_err) {
            TRACE("status_err\n");
        }

        cliprintf("led_set_bright bright ok\r\n");
    } else {
        cliprintf("parameter length error\r\n");
    }
}
CLI_CMD_EXPORT(led_set_bright, fan set speed, cli_cmd_led_set_bright)
