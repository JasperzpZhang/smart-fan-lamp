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
 * @file      ctrl.c
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
#if CTRL_DEBUG || 1
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

#define KEY_LED_ON  GPIO_PIN_RESET
#define KEY_LED_OFF GPIO_PIN_SET

ctrl_t g_ctrl;

static uint8_t slider_led_line_value;

void ctrl_task(void* para);
status_t tp_key_proc(ctrl_msg_t* msg);
status_t slider_set_target_value(uint8_t value);
void ctrl_status_init(void);
status_t slider_set_led_line(uint8_t value);
status_t slider_set_led_line_smooth(uint8_t value);

/* functions */
status_t
ctrl_init(void) {

    ctrl_status_init();
    xTaskCreate(ctrl_task, "ctrl task", 128, NULL, tskIDLE_PRIORITY + 2, NULL);
    
    
    
    /* main sw led init */
    if(th_led_status == 1){
        g_ctrl.sw._SW_MAIN = 1;
        HAL_GPIO_WritePin(KEY_LED12_GPIO_Port, KEY_LED12_Pin, KEY_LED_ON);
    }
    else {
        g_ctrl.sw._SW_MAIN = 0;
        HAL_GPIO_WritePin(KEY_LED12_GPIO_Port, KEY_LED12_Pin, KEY_LED_OFF);
    }
    
//    /* night light led init */
//    if(th_led_status == 1){
//        g_ctrl.sw._SW_NIGHT_LIGHT = 1;
//        HAL_GPIO_WritePin(KEY_LED13_GPIO_Port, KEY_LED13_Pin, KEY_LED_ON);
//    }
//    else {
//        g_ctrl.sw._SW_NIGHT_LIGHT = 0;
//        HAL_GPIO_WritePin(KEY_LED13_GPIO_Port, KEY_LED13_Pin, KEY_LED_OFF);
//    }
    
    /* fan led init */
    if (th_fan_status == 1){
        g_ctrl.sw._SW_FAN = 1;
        HAL_GPIO_WritePin(KEY_LED14_GPIO_Port, KEY_LED14_Pin, KEY_LED_ON);
    }else {
        g_ctrl.sw._SW_FAN = 0;
        HAL_GPIO_WritePin(KEY_LED14_GPIO_Port, KEY_LED14_Pin, KEY_LED_OFF);
    }

    /* three index led */
    HAL_GPIO_WritePin(KEY_LED11_GPIO_Port, KEY_LED11_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(KEY_LED16_GPIO_Port, KEY_LED16_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(KEY_LED17_GPIO_Port, KEY_LED17_Pin, GPIO_PIN_SET);

    return status_ok;
}

void
ctrl_status_init(void) {
    g_ctrl.slider_target = MODE_LED_BRIGHT;
    HAL_GPIO_WritePin(KEY_LED15_GPIO_Port, KEY_LED15_Pin, KEY_LED_ON);
}

void
ctrl_task(void* para) {

    ctrl_msg_t ctrl_msg;
    uint8_t value = 0;
    uint8_t slider_value_lock = 0;
    while (1) {
        wdog_feed();
        if (xQueueReceive(g_ctrl_queue, &ctrl_msg, 0)) {
            if (ctrl_msg.slider_en == 1) {
                value = (uint8_t)(ctrl_msg.slider_value * 100 / 190);
                slider_value_lock = 1;
                ctrl_msg.slider_en = 0;
            } else {
                /* process tp key */
                tp_key_proc(&ctrl_msg);
            }
        }

        /* set led brightness */
        if (slider_value_lock == 1) {
            TRACE("value : %d\n", value);
            slider_set_target_value(value);
            // slider_set_led_line(value);
            slider_set_led_line_smooth(value);
            if (((led_ctrl.led_brightness == value) && (g_ctrl.slider_target == MODE_LED_BRIGHT))
                || ((led_ctrl.led_color_temperature == value) && g_ctrl.slider_target == MODE_LED_COLOR)

            ) {
                slider_value_lock = 0;
            }
        }

        osDelay(3);
    }
}

status_t
tp_key_proc(ctrl_msg_t* msg) {

    /* main sw */
    if (msg->tp._TP_KEY4 == 1) {
        TRACE("msg->tp._TP_KEY4 : %d\n", msg->tp._TP_KEY4);
        g_ctrl.sw._SW_MAIN = !g_ctrl.sw._SW_MAIN;
        if (g_ctrl.sw._SW_MAIN == 1) {
            HAL_GPIO_WritePin(KEY_LED12_GPIO_Port, KEY_LED12_Pin, KEY_LED_ON);
            led_set_status(1);
        } else {
            HAL_GPIO_WritePin(KEY_LED12_GPIO_Port, KEY_LED12_Pin, KEY_LED_OFF);
            led_set_status(0);
        }
    }

    /* night light sw */
    if (msg->tp._TP_KEY5 == 1) {
        TRACE("msg->tp._TP_KEY5 : %d\n", msg->tp._TP_KEY5);
        g_ctrl.sw._SW_NIGHT_LIGHT = !g_ctrl.sw._SW_NIGHT_LIGHT;
        if (g_ctrl.sw._SW_NIGHT_LIGHT == 1) {
            HAL_GPIO_WritePin(KEY_LED13_GPIO_Port, KEY_LED13_Pin, KEY_LED_ON);
            night_light_set_status(1);
        } else {
            HAL_GPIO_WritePin(KEY_LED13_GPIO_Port, KEY_LED13_Pin, KEY_LED_OFF);
            night_light_set_status(0);
        }
    }

    /* fan sw */
    if (msg->tp._TP_KEY6 == 1) {
        TRACE("msg->tp._TP_KEY6 : %d\n", msg->tp._TP_KEY6);
        g_ctrl.sw._SW_FAN = !g_ctrl.sw._SW_FAN;
        if (g_ctrl.sw._SW_FAN == 1) {
            HAL_GPIO_WritePin(KEY_LED14_GPIO_Port, KEY_LED14_Pin, KEY_LED_ON);
            fan_set_status(1);
        } else {
            HAL_GPIO_WritePin(KEY_LED14_GPIO_Port, KEY_LED14_Pin, KEY_LED_OFF);
            fan_set_status(0);
        }
    }
    
    /* mode sw */
    if (msg->tp._TP_KEY7 == 1) {
        TRACE("msg->tp._TP_KEY7 : %d\n", msg->tp._TP_KEY7);
        if (g_ctrl.slider_target == MODE_LED_BRIGHT) {
            g_ctrl.slider_target = MODE_LED_COLOR;
        } else if (g_ctrl.slider_target == MODE_LED_COLOR) {
            g_ctrl.slider_target = MODE_FAN;
        } else if (g_ctrl.slider_target == MODE_FAN) {
            g_ctrl.slider_target = MODE_LED_BRIGHT;
        }

        switch ((uint8_t)g_ctrl.slider_target) {
            case MODE_LED_BRIGHT:
                HAL_GPIO_WritePin(KEY_LED11_GPIO_Port, KEY_LED11_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(KEY_LED16_GPIO_Port, KEY_LED16_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(KEY_LED17_GPIO_Port, KEY_LED17_Pin, GPIO_PIN_SET);
                break;
            case MODE_LED_COLOR:
                HAL_GPIO_WritePin(KEY_LED11_GPIO_Port, KEY_LED11_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(KEY_LED16_GPIO_Port, KEY_LED16_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(KEY_LED17_GPIO_Port, KEY_LED17_Pin, GPIO_PIN_SET);
                break;

            case MODE_FAN:
                HAL_GPIO_WritePin(KEY_LED11_GPIO_Port, KEY_LED11_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(KEY_LED16_GPIO_Port, KEY_LED16_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(KEY_LED17_GPIO_Port, KEY_LED17_Pin, GPIO_PIN_RESET);
                break;
            default:
                /* do nothing */
                break;
        }
    }

    return status_ok;
}

#if 0
status_t
tp_key_proc(ctrl_msg_t* msg) {
    /* main led sw */
    if (msg->tp._TP_KEY4 == 1) {
        g_ctrl.status._LED_SW = !g_ctrl.status._LED_SW;
        if (g_ctrl.status._LED_SW == 1) {
            g_ctrl.led._LED4 = 1;
            HAL_GPIO_WritePin(KEY_LED12_GPIO_Port, KEY_LED12_Pin, GPIO_PIN_RESET);
            led_set_status(1);
        } else {
            g_ctrl.led._LED4 = 0;
            HAL_GPIO_WritePin(KEY_LED12_GPIO_Port, KEY_LED12_Pin, GPIO_PIN_SET);
            led_set_status(0);
        }
    }

    /* night light sw */
    if (msg->tp._TP_KEY5 == 1) {
        g_ctrl.status._LED_CTRL = !g_ctrl.status._LED_CTRL;
        if (g_ctrl.status._LED_CTRL == 1) {
            g_ctrl.slider_target = LED_COLOR_CTRL;
            g_ctrl.led._LED5 = 1;
            HAL_GPIO_WritePin(KEY_LED13_GPIO_Port, KEY_LED13_Pin, GPIO_PIN_RESET);
        } else {
            g_ctrl.slider_target = LED_BRIGHT_CTRL;
            g_ctrl.led._LED5 = 0;
            HAL_GPIO_WritePin(KEY_LED13_GPIO_Port, KEY_LED13_Pin, GPIO_PIN_SET);
        }
        
    }

    /* fan sw */
    if (msg->tp._TP_KEY6 == 1) {
        g_ctrl.led._LED6 = !g_ctrl.led._LED6;
        HAL_GPIO_TogglePin(KEY_LED14_GPIO_Port, KEY_LED14_Pin);
        
    }

    /* fan */
    if (msg->tp._TP_KEY7 == 1) {
        g_ctrl.led._LED7 = !g_ctrl.led._LED7;
        HAL_GPIO_TogglePin(KEY_LED15_GPIO_Port, KEY_LED15_Pin);
    }

    /* night light */
    if (msg->tp._TP_KEY8 == 1) {

        g_ctrl.status._NIGHT_LIGHT = !g_ctrl.status._NIGHT_LIGHT;
        if (g_ctrl.status._NIGHT_LIGHT == 1) {
            g_ctrl.led._LED8 = 1;
            HAL_GPIO_WritePin(KEY_LED16_GPIO_Port, KEY_LED16_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(NIGHT_LIGHT_EN_GPIO_Port, NIGHT_LIGHT_EN_Pin, GPIO_PIN_SET);
        } else {
            g_ctrl.slider_target = LED_BRIGHT_CTRL;
            g_ctrl.led._LED8 = 0;
            HAL_GPIO_WritePin(KEY_LED16_GPIO_Port, KEY_LED16_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(NIGHT_LIGHT_EN_GPIO_Port, NIGHT_LIGHT_EN_Pin, GPIO_PIN_RESET);
        }
    }

    /* usb */
    if (msg->tp._TP_KEY9 == 1) {

        g_ctrl.status._USB = !g_ctrl.status._USB;
        if (g_ctrl.status._USB == 1) {
            g_ctrl.led._LED9 = 1;
            HAL_GPIO_WritePin(KEY_LED17_GPIO_Port, KEY_LED17_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(USB_POWER_EN_GPIO_Port, USB_POWER_EN_Pin, GPIO_PIN_SET);
        } else {
            g_ctrl.slider_target = LED_BRIGHT_CTRL;
            g_ctrl.led._LED9 = 0;
            HAL_GPIO_WritePin(KEY_LED17_GPIO_Port, KEY_LED17_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(USB_POWER_EN_GPIO_Port, USB_POWER_EN_Pin, GPIO_PIN_RESET);
        }
    }

    return status_ok;
}

#endif

status_t
slider_set_target_value(uint8_t value) {

    switch (g_ctrl.slider_target) {
        case MODE_LED_BRIGHT:
            //            TRACE("bright : %d\n", value);
            led_set_brightness_smooth(value);
            break;
        case MODE_LED_COLOR:
            //            TRACE("color : %d\n", value);
            led_set_color_temperature_smooth(value);
            break;

        case MODE_FAN:
            //            TRACE("speed : %d\n", value);
            fan_set_speed(1, value);
            osDelay(4);
            break;

        default: break;
    }

    return status_ok;
}

status_t
slider_set_led_line_smooth(uint8_t value) {
    if (slider_led_line_value != value) {
        if (slider_led_line_value < value) {
            slider_led_line_value++;
            slider_set_led_line(slider_led_line_value);
        } else if (slider_led_line_value > value) {
            slider_led_line_value--;
            slider_set_led_line(slider_led_line_value);
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
