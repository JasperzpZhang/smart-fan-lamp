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
    xTaskCreate(panel_ctrl_task, "ctrl task", 128, NULL, tskIDLE_PRIORITY + 1, NULL);
    return status_ok;
}

void
panel_status_init(void) {
    g_panel_ctrl.slider_target = MODE_LED_BRIGHT;
    panel_set_sw_led_status(mode_sw, panel_led_on);

    g_panel_ctrl.sw._SW_MAIN = 0;
    g_panel_ctrl.sw._SW_FAN = 0;
    g_panel_ctrl.sw._SW_NIGHT_LIGHT = 0;

    // /* main sw led init */
    // if (g_led_ctrl.status._LED_STATUS == 1 && g_led_ctrl.led_brightness != 0) {
    //     g_panel_ctrl.sw._SW_MAIN = 1;
    //     panel_set_sw_led_status(main_sw, panel_led_on);
    // } else {
    //     g_panel_ctrl.sw._SW_MAIN = 0;
    //     panel_set_sw_led_status(main_sw, panel_led_off);
    // }

    // /* fan led init */
    // if (g_fan_ctrl.fan_status == 1 && g_fan_ctrl.fan_speed != 0) {
    //     g_panel_ctrl.sw._SW_FAN = 1;
    //     panel_set_sw_led_status(fan, panel_led_on);
    // } else {
    //     g_panel_ctrl.sw._SW_FAN = 0;
    //     panel_set_sw_led_status(fan, panel_led_off);
    // }

    // /* three index led */
    // panel_set_sw_led_status(idx_brightness, panel_led_on);
    // panel_set_sw_led_status(idx_color, panel_led_off);
    // panel_set_sw_led_status(idx_fan, panel_led_off);

    // slider_set_led_line_smooth_blk(g_led_ctrl.led_brightness);
}

void
panel_ctrl_task(void* para) {
    msg_panel_t msg_panel;
    uint8_t value = 0;
    // uint8_t slider_value_lock = 0;
    while (1) {
        wdog_feed();
        if (xQueueReceive(g_queue_panel, &msg_panel, portMAX_DELAY)) {
            if (msg_panel.slider_en == 1) {
                value = (uint8_t)(msg_panel.slider_value * 100 / 190);
                msg_panel.slider_en = 0;
                slider_set_target_value(value);
                // slider_set_led_line_smooth(value);

            } else {
                /* process tp key */
                tp_key_proc(&msg_panel);
            }
        }

        osDelay(10);
    }
}

status_t
tp_key_proc(msg_panel_t* msg) {

    /* main sw */
    if (msg->panel._TP_KEY4 == 1) {
        TRACE("msg->tp._TP_KEY4 : %d\n", msg->panel._TP_KEY4);
        g_panel_ctrl.sw._SW_MAIN = !g_panel_ctrl.sw._SW_MAIN;
        TRACE("g_panel_ctrl.sw._SW_MAIN : %d\n", g_panel_ctrl.sw._SW_MAIN);
        if (g_panel_ctrl.sw._SW_MAIN != 0) {
            sys_led_on(SOURCE_PANEL);
        } else {
            sys_led_off(SOURCE_PANEL);
        }
    }

    /* night light sw */
    if (msg->panel._TP_KEY5 == 1) {
        TRACE("msg->tp._TP_KEY5 : %d\n", msg->panel._TP_KEY5);
        g_panel_ctrl.sw._SW_NIGHT_LIGHT = !g_panel_ctrl.sw._SW_NIGHT_LIGHT;
        TRACE("g_panel_ctrl.sw._SW_NIGHT_LIGHT : %d\n", g_panel_ctrl.sw._SW_NIGHT_LIGHT);
        if (g_panel_ctrl.sw._SW_NIGHT_LIGHT != 0) {
            sys_night_light_on(SOURCE_PANEL);
        } else {
            sys_night_light_off(SOURCE_PANEL);
        }
    }

    /* fan sw */
    if (msg->panel._TP_KEY6 == 1) {
        TRACE("msg->tp._TP_KEY6 : %d\n", msg->panel._TP_KEY6);
        g_panel_ctrl.sw._SW_FAN = !g_panel_ctrl.sw._SW_FAN;
        if (g_panel_ctrl.sw._SW_FAN != 0) {
            TRACE("fan on\n");
            sys_fan_on(SOURCE_PANEL);
        } else {
            TRACE("fan off\n");
            sys_fan_off(SOURCE_PANEL);
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
        // panel_set_key_mode_sw(g_panel_ctrl.slider_target);
        sys_slider_switch_target(SOURCE_PANEL, g_panel_ctrl.slider_target);
    }
    return status_ok;
}

status_t
panel_set_key_mode_sw(panel_slider_target_t slider_target) {
    switch (slider_target) {
        case MODE_LED_BRIGHT:
            g_panel_ctrl.slider_target = MODE_LED_BRIGHT;
            // panel_set_sw_led_status(idx_brightness, panel_led_on);
            // panel_set_sw_led_status(idx_color, panel_led_off);
            // panel_set_sw_led_status(idx_fan, panel_led_off);
            // slider_set_led_line_smooth_blk(g_led_ctrl.led_brightness);
            break;

        case MODE_LED_COLOR:
            g_panel_ctrl.slider_target = MODE_LED_COLOR;
            // panel_set_sw_led_status(idx_brightness, panel_led_off);
            // panel_set_sw_led_status(idx_color, panel_led_on);
            // panel_set_sw_led_status(idx_fan, panel_led_off);
            // slider_set_led_line_smooth_blk(g_led_ctrl.led_color_temperature);
            break;

        case MODE_FAN:
            g_panel_ctrl.slider_target = MODE_FAN;
            // panel_set_sw_led_status(idx_brightness, panel_led_off);
            // panel_set_sw_led_status(idx_color, panel_led_off);
            // panel_set_sw_led_status(idx_fan, panel_led_on);
            // slider_set_led_line_smooth_blk(g_fan_ctrl.fan_speed);
            break;
        default:
            /* do nothing */
            break;
    }
    return status_ok;
}

status_t
slider_set_target_value(uint8_t value) {

    switch (g_panel_ctrl.slider_target) {
        case MODE_LED_BRIGHT:
            if (value != 0) {
                g_panel_ctrl.sw._SW_MAIN = 1;
            } else {
                g_panel_ctrl.sw._SW_MAIN = 0;
            }
            sys_led_set_brightness(SOURCE_PANEL, value);
            break;
        case MODE_LED_COLOR: sys_led_set_color_temp(SOURCE_PANEL, value); break;
        case MODE_FAN:
            if (value != 0) {
                g_panel_ctrl.sw._SW_FAN = 1;
            } else {
                g_panel_ctrl.sw._SW_FAN = 0;
            }
            sys_fan_set_speed(SOURCE_PANEL, value);
            break;
        default: break;
    }
    return status_ok;
}

status_t
panel_set_sw_led_status(panel_led_target_t led_target, panel_led_status_t led_status) {

    switch ((uint16_t)led_target) {
        case main_sw:
            PANEL_SET_MAIN_SW_LED(led_status);
            if (led_status != 0) {
                if (g_panel_ctrl.sw._SW_MAIN == 0) {
                    g_panel_ctrl.sw._SW_MAIN = 1;
                }
            } else {
                if (g_panel_ctrl.sw._SW_MAIN == 1) {
                    g_panel_ctrl.sw._SW_MAIN = 0;
                }
            }
            break;
        case night_light:
            PANEL_SET_NIGHT_LIGHT_LED(led_status);
            if (led_status != 0) {
                if (g_panel_ctrl.sw._SW_NIGHT_LIGHT == 0) {
                    g_panel_ctrl.sw._SW_NIGHT_LIGHT = 1;
                }
            } else {
                if (g_panel_ctrl.sw._SW_NIGHT_LIGHT == 1) {
                    g_panel_ctrl.sw._SW_NIGHT_LIGHT = 0;
                }
            }
            break;
        case fan:
            PANEL_SET_FAN_LED(led_status);
            if (led_status != 0) {
                if (g_panel_ctrl.sw._SW_FAN == 0) {
                    g_panel_ctrl.sw._SW_FAN = 1;
                }
            } else {
                if (g_panel_ctrl.sw._SW_FAN == 1) {
                    g_panel_ctrl.sw._SW_FAN = 0;
                }
            }
            break;
        case mode_sw: PANEL_SET_MODE_SW_LED(led_status); break;
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
slider_set_led_line_smooth(panel_slider_target_t slider_target, uint8_t value) {
    if (slider_target != g_panel_ctrl.slider_target) {
        return status_ok;
    }

    if (g_panel_ctrl.slider_led_line_value != value) {
        if (g_panel_ctrl.slider_led_line_value < value) {
            g_panel_ctrl.slider_led_line_value++;
            slider_set_led_line(g_panel_ctrl.slider_led_line_value);
        } else if (g_panel_ctrl.slider_led_line_value > value) {
            g_panel_ctrl.slider_led_line_value--;
            slider_set_led_line(g_panel_ctrl.slider_led_line_value);
        }

        return status_busy;
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
