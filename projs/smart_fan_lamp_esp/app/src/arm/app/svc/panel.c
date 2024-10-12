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
 * @file      panel.c
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

#include "app/include.h"

/* Debug config */
#if CTRL_DEBUG || 0
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* CTRL_DEBUG */
#if CTRL_ASSERT
#undef ASSERT
#define ASSERT(a)                                                                                                      \
    while (1) {                                                                                                        \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                    \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* CTRL_ASSERT */

panel_ctrl_t g_panel_ctrl;

void panel_ctrl_task(void* para);
status_t tp_key_proc(msg_panel_t* msg);
status_t slider_set_target_value(uint8_t value);
void panel_status_init(void);
status_t slider_set_led_line(uint8_t value);

/* functions */
status_t
panel_init(void) {

    panel_status_init();
    xTaskCreate(panel_ctrl_task, "ctrl task", 128, NULL, tskIDLE_PRIORITY + 2, NULL);

    return status_ok;
}

void
panel_status_init(void) {
    g_panel_ctrl.slider_target = MODE_LED_BRIGHT;

    panel_set_led_status(mode_sw, panel_led_on);

    /* main sw led init */
    if (g_led_ctrl.status._LED_STATUS == 1 && g_led_ctrl.led_brightness != 0) {
        g_panel_ctrl.sw._SW_MAIN = 1;
        panel_set_led_status(main_sw, panel_led_on);
    } else {
        g_panel_ctrl.sw._SW_MAIN = 0;
        panel_set_led_status(main_sw, panel_led_off);
    }

    /* fan led init */
    if (g_fan_ctrl.fan_status == 1 && g_fan_ctrl.fan_speed != 0) {
        g_panel_ctrl.sw._SW_FAN = 1;
        panel_set_led_status(fan, panel_led_on);
    } else {
        g_panel_ctrl.sw._SW_FAN = 0;
        panel_set_led_status(fan, panel_led_off);
    }

    /* three index led */
    panel_set_led_status(idx_brightness, panel_led_on);
    panel_set_led_status(idx_color, panel_led_off);
    panel_set_led_status(idx_fan, panel_led_off);

    slider_set_led_line_smooth_blk(g_led_ctrl.led_brightness);
}

void
panel_ctrl_task(void* para) {
    msg_panel_t msg_panel;
    uint8_t value = 0;
    uint8_t slider_value_lock = 0;
    while (1) {
        wdog_feed();
        if (xQueueReceive(g_queue_panel, &msg_panel, 0)) {
            if (msg_panel.slider_en == 1) {
                value = (uint8_t)(msg_panel.slider_value * 100 / 190);
                slider_value_lock = 1;
                msg_panel.slider_en = 0;
                TRACE("msg queue panel is run\n");
            } else {
                /* process tp key */
                tp_key_proc(&msg_panel);
            }
        }

        /* set led brightness */
        if (slider_value_lock == 1) {
            TRACE("value : %d\n", value);
            slider_set_target_value(value);
            slider_set_led_line_smooth(value);
            if (((g_led_ctrl.led_brightness == value) && (g_panel_ctrl.slider_target == MODE_LED_BRIGHT))
                || ((g_led_ctrl.led_color_temperature == value) && g_panel_ctrl.slider_target == MODE_LED_COLOR)
                || ((g_fan_ctrl.fan_speed == value) && g_panel_ctrl.slider_target == MODE_FAN)

            ) {
                slider_value_lock = 0;
            }
        }

        osDelay(7);
    }
}

status_t
tp_key_proc(msg_panel_t* msg) {

    /* main sw */
    if (msg->panel._TP_KEY4 == 1) {
        TRACE("msg->tp._TP_KEY4 : %d\n", msg->panel._TP_KEY4);
        g_panel_ctrl.sw._SW_MAIN = !g_panel_ctrl.sw._SW_MAIN;
        if (g_panel_ctrl.sw._SW_MAIN == 1) {
            panel_set_led_status(main_sw, panel_led_on);
            led_set_status(1);
            if (g_panel_ctrl.slider_target == MODE_LED_BRIGHT) {
                slider_set_led_line_smooth_blk(g_led_ctrl.led_brightness);
            }

        } else {
            panel_set_led_status(main_sw, panel_led_off);
            led_set_status(0);
            if (g_panel_ctrl.slider_target == MODE_LED_BRIGHT) {
                slider_set_led_line_smooth_blk(0);
            }
        }
    }

    /* night light sw */
    if (msg->panel._TP_KEY5 == 1) {
        TRACE("msg->tp._TP_KEY5 : %d\n", msg->panel._TP_KEY5);
        g_panel_ctrl.sw._SW_NIGHT_LIGHT = !g_panel_ctrl.sw._SW_NIGHT_LIGHT;
        if (g_panel_ctrl.sw._SW_NIGHT_LIGHT == 1) {
            panel_set_led_status(night_light, panel_led_on);
            night_light_set_status(1);
        } else {
            panel_set_led_status(night_light, panel_led_off);
            night_light_set_status(0);
        }
    }

    /* fan sw */
    if (msg->panel._TP_KEY6 == 1) {
        TRACE("msg->tp._TP_KEY6 : %d\n", msg->panel._TP_KEY6);
        g_panel_ctrl.sw._SW_FAN = !g_panel_ctrl.sw._SW_FAN;
        if (g_panel_ctrl.sw._SW_FAN == 1) {
            panel_set_led_status(fan, panel_led_on);
            fan_set_status(1);
            if (g_panel_ctrl.slider_target == MODE_FAN) {
                slider_set_led_line_smooth_blk(g_fan_ctrl.last_fan_speed);
            }
        } else {
            panel_set_led_status(fan, panel_led_off);
            fan_set_status(0);
            if (g_panel_ctrl.slider_target == MODE_FAN) {
                slider_set_led_line_smooth_blk(0);
            }
        }
    }

    /* mode sw */
    if (msg->panel._TP_KEY7 == 1) {
        TRACE("msg->tp._TP_KEY7 : %d\n", msg->panel._TP_KEY7);
        if (g_panel_ctrl.slider_target == MODE_LED_BRIGHT) {
            g_panel_ctrl.slider_target = MODE_LED_COLOR;
        } else if (g_panel_ctrl.slider_target == MODE_LED_COLOR) {
            g_panel_ctrl.slider_target = MODE_FAN;
        } else if (g_panel_ctrl.slider_target == MODE_FAN) {
            g_panel_ctrl.slider_target = MODE_LED_BRIGHT;
        }

        switch ((uint8_t)g_panel_ctrl.slider_target) {
            case MODE_LED_BRIGHT:
                panel_set_led_status(idx_brightness, panel_led_on);
                panel_set_led_status(idx_color, panel_led_off);
                panel_set_led_status(idx_fan, panel_led_off);
                slider_set_led_line_smooth_blk(g_led_ctrl.led_brightness);
                break;
            case MODE_LED_COLOR:
                panel_set_led_status(idx_brightness, panel_led_off);
                panel_set_led_status(idx_color, panel_led_on);
                panel_set_led_status(idx_fan, panel_led_off);
                slider_set_led_line_smooth_blk(g_led_ctrl.led_color_temperature);
                break;

            case MODE_FAN:
                panel_set_led_status(idx_brightness, panel_led_off);
                panel_set_led_status(idx_color, panel_led_off);
                panel_set_led_status(idx_fan, panel_led_on);
                slider_set_led_line_smooth_blk(g_fan_ctrl.fan_speed);
                break;
            default:
                /* do nothing */
                break;
        }
    }

    return status_ok;
}

status_t
slider_set_target_value(uint8_t value) {
    static uint8_t fan_start_stop_lock = 0;
    static uint8_t led_start_stop_lock = 0;

    switch (g_panel_ctrl.slider_target) {
        case MODE_LED_BRIGHT:
            //            TRACE("bright : %d\n", value);
            led_set_brightness_smooth(value);
            g_led_ctrl.last_led_brightness = g_led_ctrl.led_brightness;
            if (value != 0) {
                if (led_start_stop_lock == 0) {
                    panel_set_led_status(main_sw, panel_led_on);
                    led_start_stop_lock = 1;
                }
            } else {
                if (led_start_stop_lock == 1) {
                    panel_set_led_status(main_sw, panel_led_off);
                    led_start_stop_lock = 0;
                }
            }
            break;
        case MODE_LED_COLOR:
            //            TRACE("color : %d\n", value);
            led_set_color_temperature_smooth(value);
            g_led_ctrl.last_led_color_temperature = g_led_ctrl.led_color_temperature;
            break;

        case MODE_FAN:
            //            TRACE("speed : %d\n", value);
            if (value != 0) {
                if (fan_start_stop_lock == 0) {
                    panel_set_led_status(fan, panel_led_on);
                    fan_start_stop_lock = 1;
                }
            } else {
                if (fan_start_stop_lock == 1) {
                    panel_set_led_status(fan, panel_led_off);
                    fan_start_stop_lock = 0;
                }
            }

            fan_set_speed_smooth(value);
            osDelay(1);
            break;

        default: break;
    }

    return status_ok;
}

status_t
panel_set_led_status(panel_led_target_t led_target, panel_led_status_t led_status) {

    switch ((uint16_t)led_target) {
        case main_sw:
            PANEL_SET_MAIN_SW_LED(led_status);
            g_panel_ctrl.sw._SW_MAIN = led_status;
            break;
        case night_light:
            PANEL_SET_NIGHT_LIGHT_LED(led_status);
            g_panel_ctrl.sw._SW_NIGHT_LIGHT = led_status;
            break;
        case fan:
            PANEL_SET_FAN_LED(led_status);
            g_panel_ctrl.sw._SW_FAN = led_status;
            break;
        case mode_sw:
            PANEL_SET_MODE_SW_LED(led_status);
            g_panel_ctrl.sw._SW_MODE = led_status;
            break;
        case idx_brightness: PANEL_SET_IDX_BRIGHTNESS_LED(led_status); break;
        case idx_color: PANEL_SET_IDX_COLOR_LED(led_status); break;
        case idx_fan: PANEL_SET_IDX_FAN_LED(led_status); break;
    }

    return status_ok;
}

status_t
slider_set_led_line_smooth_blk(uint8_t value) {
    while (g_panel_ctrl.slider_led_line_value != value) {
        if (g_panel_ctrl.slider_led_line_value < value) {
            g_panel_ctrl.slider_led_line_value++;
        } else if (g_panel_ctrl.slider_led_line_value > value) {
            g_panel_ctrl.slider_led_line_value--;
        }
        slider_set_led_line(g_panel_ctrl.slider_led_line_value);
        osDelay(8);
    }
    return status_ok;
}

status_t
slider_set_led_line_smooth(uint8_t value) {
    if (g_panel_ctrl.slider_led_line_value != value) {
        if (g_panel_ctrl.slider_led_line_value < value) {
            g_panel_ctrl.slider_led_line_value++;
            slider_set_led_line(g_panel_ctrl.slider_led_line_value);
        } else if (g_panel_ctrl.slider_led_line_value > value) {
            g_panel_ctrl.slider_led_line_value--;
            slider_set_led_line(g_panel_ctrl.slider_led_line_value);
        }
    }
    return status_ok;
}

status_t
slider_set_led_line(uint8_t value) {

    if (value > 0 && value < 10) {

        value = 1;
    } else {
        value /= 10;
    }

    switch (value) {
        case 0:
            HAL_GPIO_WritePin(KEY_LED1_GPIO_Port, KEY_LED1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED2_GPIO_Port, KEY_LED2_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED3_GPIO_Port, KEY_LED3_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED4_GPIO_Port, KEY_LED4_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED5_GPIO_Port, KEY_LED5_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED6_GPIO_Port, KEY_LED6_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED7_GPIO_Port, KEY_LED7_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED8_GPIO_Port, KEY_LED8_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED9_GPIO_Port, KEY_LED9_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED10_GPIO_Port, KEY_LED10_Pin, GPIO_PIN_SET);
            break;

        case 1:
            HAL_GPIO_WritePin(KEY_LED1_GPIO_Port, KEY_LED1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED2_GPIO_Port, KEY_LED2_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED3_GPIO_Port, KEY_LED3_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED4_GPIO_Port, KEY_LED4_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED5_GPIO_Port, KEY_LED5_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED6_GPIO_Port, KEY_LED6_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED7_GPIO_Port, KEY_LED7_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED8_GPIO_Port, KEY_LED8_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED9_GPIO_Port, KEY_LED9_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED10_GPIO_Port, KEY_LED10_Pin, GPIO_PIN_SET);
            break;

        case 2:
            HAL_GPIO_WritePin(KEY_LED1_GPIO_Port, KEY_LED1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED2_GPIO_Port, KEY_LED2_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED3_GPIO_Port, KEY_LED3_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED4_GPIO_Port, KEY_LED4_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED5_GPIO_Port, KEY_LED5_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED6_GPIO_Port, KEY_LED6_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED7_GPIO_Port, KEY_LED7_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED8_GPIO_Port, KEY_LED8_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED9_GPIO_Port, KEY_LED9_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED10_GPIO_Port, KEY_LED10_Pin, GPIO_PIN_SET);
            break;

        case 3:
            HAL_GPIO_WritePin(KEY_LED1_GPIO_Port, KEY_LED1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED2_GPIO_Port, KEY_LED2_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED3_GPIO_Port, KEY_LED3_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED4_GPIO_Port, KEY_LED4_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED5_GPIO_Port, KEY_LED5_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED6_GPIO_Port, KEY_LED6_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED7_GPIO_Port, KEY_LED7_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED8_GPIO_Port, KEY_LED8_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED9_GPIO_Port, KEY_LED9_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED10_GPIO_Port, KEY_LED10_Pin, GPIO_PIN_SET);
            break;

        case 4:
            HAL_GPIO_WritePin(KEY_LED1_GPIO_Port, KEY_LED1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED2_GPIO_Port, KEY_LED2_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED3_GPIO_Port, KEY_LED3_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED4_GPIO_Port, KEY_LED4_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED5_GPIO_Port, KEY_LED5_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED6_GPIO_Port, KEY_LED6_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED7_GPIO_Port, KEY_LED7_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED8_GPIO_Port, KEY_LED8_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED9_GPIO_Port, KEY_LED9_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED10_GPIO_Port, KEY_LED10_Pin, GPIO_PIN_SET);
            break;

        case 5:
            HAL_GPIO_WritePin(KEY_LED1_GPIO_Port, KEY_LED1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED2_GPIO_Port, KEY_LED2_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED3_GPIO_Port, KEY_LED3_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED4_GPIO_Port, KEY_LED4_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED5_GPIO_Port, KEY_LED5_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED6_GPIO_Port, KEY_LED6_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED7_GPIO_Port, KEY_LED7_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED8_GPIO_Port, KEY_LED8_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED9_GPIO_Port, KEY_LED9_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED10_GPIO_Port, KEY_LED10_Pin, GPIO_PIN_SET);
            break;

        case 6:
            HAL_GPIO_WritePin(KEY_LED1_GPIO_Port, KEY_LED1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED2_GPIO_Port, KEY_LED2_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED3_GPIO_Port, KEY_LED3_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED4_GPIO_Port, KEY_LED4_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED5_GPIO_Port, KEY_LED5_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED6_GPIO_Port, KEY_LED6_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED7_GPIO_Port, KEY_LED7_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED8_GPIO_Port, KEY_LED8_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED9_GPIO_Port, KEY_LED9_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED10_GPIO_Port, KEY_LED10_Pin, GPIO_PIN_SET);
            break;

        case 7:
            HAL_GPIO_WritePin(KEY_LED1_GPIO_Port, KEY_LED1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED2_GPIO_Port, KEY_LED2_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED3_GPIO_Port, KEY_LED3_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED4_GPIO_Port, KEY_LED4_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED5_GPIO_Port, KEY_LED5_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED6_GPIO_Port, KEY_LED6_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED7_GPIO_Port, KEY_LED7_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED8_GPIO_Port, KEY_LED8_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED9_GPIO_Port, KEY_LED9_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED10_GPIO_Port, KEY_LED10_Pin, GPIO_PIN_SET);
            break;

        case 8:
            HAL_GPIO_WritePin(KEY_LED1_GPIO_Port, KEY_LED1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED2_GPIO_Port, KEY_LED2_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED3_GPIO_Port, KEY_LED3_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED4_GPIO_Port, KEY_LED4_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED5_GPIO_Port, KEY_LED5_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED6_GPIO_Port, KEY_LED6_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED7_GPIO_Port, KEY_LED7_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED8_GPIO_Port, KEY_LED8_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED9_GPIO_Port, KEY_LED9_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(KEY_LED10_GPIO_Port, KEY_LED10_Pin, GPIO_PIN_SET);
            break;

        case 9:
            HAL_GPIO_WritePin(KEY_LED1_GPIO_Port, KEY_LED1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED2_GPIO_Port, KEY_LED2_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED3_GPIO_Port, KEY_LED3_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED4_GPIO_Port, KEY_LED4_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED5_GPIO_Port, KEY_LED5_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED6_GPIO_Port, KEY_LED6_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED7_GPIO_Port, KEY_LED7_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED8_GPIO_Port, KEY_LED8_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED9_GPIO_Port, KEY_LED9_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED10_GPIO_Port, KEY_LED10_Pin, GPIO_PIN_SET);
            break;

        case 10:
            HAL_GPIO_WritePin(KEY_LED1_GPIO_Port, KEY_LED1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED2_GPIO_Port, KEY_LED2_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED3_GPIO_Port, KEY_LED3_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED4_GPIO_Port, KEY_LED4_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED5_GPIO_Port, KEY_LED5_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED6_GPIO_Port, KEY_LED6_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED7_GPIO_Port, KEY_LED7_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED8_GPIO_Port, KEY_LED8_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED9_GPIO_Port, KEY_LED9_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(KEY_LED10_GPIO_Port, KEY_LED10_Pin, GPIO_PIN_RESET);
            break;
        default:
            /* do nothing */
            break;
    }

    return status_ok;
}
