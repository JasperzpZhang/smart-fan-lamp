/**
 * \file            led.c
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

#include "app/include.h"

/* Debug config */
#if LED_DEBUG || 1
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* LED_DEBUG */
#if LED_ASSERT
#undef ASSERT
#define ASSERT(a)                                                                                                      \
    while (!(a)) {                                                                                                     \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                    \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* LED_ASSERT */

/* global variables */
led_ctrl_t led_ctrl;

/* Local Defines */
#define LIGHT_DEEP 100

void led_task(void* para);

/* functions */
status_t
led_init(void) {
    /* You must initalize all parameters before you can operate a function */
    led_ctrl.last_led_brightness = th_led_brightness;
    led_ctrl.last_led_color_temperature = th_led_color_temperature;
    led_set_brightness(th_led_brightness);
    led_set_color_temperature(th_led_color_temperature);
    if (1 == th_led_status) {
        HAL_TIM_PWM_Start(LED_COLD_TIM, LED_COLD_TIM_CHANNEL);
        HAL_TIM_PWM_Start(LED_WARM_TIM, LED_WARM_TIM_CHANNEL);
    }

    xTaskCreate(led_task, "led task", 128, NULL, tskIDLE_PRIORITY + 2, NULL);

    return status_ok;
}

void
led_task(void* para) {
    led_msg_t led_msg;
    uint8_t bright = 0;
    uint8_t bright_lock = 0;
    while (1) {
        if (xQueueReceive(g_led_queue, &led_msg, 0) == pdPASS) {
            TRACE("buf[1] : %d\n", led_msg.slider_value * 100 / 180);
            bright = (uint8_t)(led_msg.slider_value * 100 / 180);
            bright_lock = 1;
        }

        if (bright_lock == 1) {
            led_set_brightness_smooth(bright);
            if (led_ctrl.led_brightness == bright) {
                bright_lock = 0;
            }
        }

        osDelay(2);
    }
}

/* change pwm duty */
static status_t
prv_led_pwm_duty_ctrl(uint16_t led_cold_pwm_duty, uint16_t led_warm_pwm_duty) {
    __HAL_TIM_SET_COMPARE(LED_COLD_TIM, LED_COLD_TIM_CHANNEL, led_cold_pwm_duty);
    __HAL_TIM_SET_COMPARE(LED_WARM_TIM, LED_WARM_TIM_CHANNEL, led_warm_pwm_duty);
    return status_ok;
}

/* change brightness */
status_t
led_set_brightness(uint16_t led_brightness) {
    if (led_brightness > 100) {
        return status_err;
    }
    led_ctrl.last_led_brightness = led_brightness;
    led_ctrl.status._LED_STATUS = 1;
    uint16_t led_cold_brightness = 0, led_warm_brightness = 0;
    /* You can't change the color temperature when you adjust the brightness */
    led_warm_brightness = led_ctrl.last_led_color_temperature * led_brightness / LIGHT_DEEP;
    led_cold_brightness = (LIGHT_DEEP - led_ctrl.last_led_color_temperature) * led_brightness / LIGHT_DEEP;
    prv_led_pwm_duty_ctrl(led_cold_brightness, led_warm_brightness);

    return status_ok;
}

/* change color temperature */
status_t
led_set_color_temperature(uint16_t led_color_temperature) {
    if (led_color_temperature > 100) {
        return status_err;
    }
    led_ctrl.last_led_color_temperature = led_color_temperature;
    uint16_t led_cold_brightness = 0, led_warm_brightness = 0;
    /* You can't change the brightness when you adjust the color temperature */
    led_warm_brightness = led_color_temperature * led_ctrl.last_led_brightness / LIGHT_DEEP;
    led_cold_brightness = (LIGHT_DEEP - led_color_temperature) * led_ctrl.last_led_brightness / LIGHT_DEEP;
    prv_led_pwm_duty_ctrl(led_cold_brightness, led_warm_brightness);
    return status_ok;
}

status_t
led_save_status(void) {
    th_led_brightness = led_ctrl.last_led_brightness;
    th_led_color_temperature = led_ctrl.last_led_color_temperature;
    th_led_status = led_ctrl.status._LED_STATUS;
    data_save_direct();
    return status_ok;
}

status_t
led_stop_pwm(void) {
    HAL_TIM_PWM_Stop(LED_COLD_TIM, LED_COLD_TIM_CHANNEL);
    HAL_TIM_PWM_Stop(LED_WARM_TIM, LED_WARM_TIM_CHANNEL);
    return status_ok;
}

status_t
led_start_pwm(void) {
    HAL_TIM_PWM_Start(LED_COLD_TIM, LED_COLD_TIM_CHANNEL);
    HAL_TIM_PWM_Start(LED_WARM_TIM, LED_WARM_TIM_CHANNEL);
    return status_ok;
}

status_t
led_set_status(uint16_t on_off) {
    if (on_off != 0) {
        led_start_pwm();
        led_ctrl.status._LED_STATUS = 1;
    } else {
        led_stop_pwm();
        led_ctrl.status._LED_STATUS = 0;
    }
    led_save_status();
    return status_ok;
}

status_t
night_light_set_status(uint16_t on_off) {
    if (on_off != 1 && on_off != 0) {
        TRACE("night light on_off num error\r\n");
        return status_err;
    }
    HAL_GPIO_WritePin(LED_NIGHT_LIGHT_GPIO_PORT, LED_NIGHT_LIGHT_GPIO_PIN, (on_off ? GPIO_PIN_SET : GPIO_PIN_RESET));
    led_ctrl.status._NIGHT_LIGHT_STATUS = on_off;
    led_save_status();
    return status_ok;
}

status_t
led_set_brightness_smooth(uint16_t target_led_brightness) {
    if (led_ctrl.led_brightness != target_led_brightness) {
        if (led_ctrl.led_brightness < target_led_brightness) {
            led_ctrl.led_brightness++;
            led_set_brightness(led_ctrl.led_brightness);
        }

        if (led_ctrl.led_brightness > target_led_brightness) {
            led_ctrl.led_brightness--;
            led_set_brightness(led_ctrl.led_brightness);
        }
        osDelay(8);
    }
    led_ctrl.status._LED_STATUS = 1;
    //    led_save_status();
    return status_ok;
}

status_t
led_set_color_temperature_smooth(uint16_t target_led_color_temperature) {
    while (led_ctrl.led_color_temperature != target_led_color_temperature) {
        if (led_ctrl.led_color_temperature < target_led_color_temperature) {
            led_ctrl.led_color_temperature++;
            led_set_color_temperature(led_ctrl.led_color_temperature);
        }

        if (led_ctrl.led_color_temperature > target_led_color_temperature) {
            led_ctrl.led_color_temperature--;
            led_set_color_temperature(led_ctrl.led_color_temperature);
        }
        osDelay(8);
    }
//    led_save_status();
    return status_ok;
}

/* cli commond */
static void
cli_led_set_brightness(cli_printf cliprintf, int argc, char** argv) {
    if (2 == argc) {
        int led_brightness;
        led_brightness = atoi(argv[1]);
        if (led_brightness < 0 || led_brightness > LIGHT_DEEP) {
            cliprintf("brightness : 0 - 100\r\n");
            return;
        }
        led_set_brightness(led_brightness);
        led_save_status();
        cliprintf("led_set_brightness ok\r\n");
    } else {
        cliprintf("parameter length error\r\n");
    }
}
CLI_CMD_EXPORT(led_set_bright, set led brightness, cli_led_set_brightness)

static void
cli_led_set_color_temperature(cli_printf cliprintf, int argc, char** argv) {
    if (2 == argc) {
        int led_color_temperature;
        led_color_temperature = atoi(argv[1]);
        if (led_color_temperature < 0 || led_color_temperature > LIGHT_DEEP) {
            cliprintf("color_temperature : 0 - 100\r\n");
            return;
        }
        led_set_color_temperature(led_color_temperature);
        led_save_status();
        cliprintf("led_color_temperature ok\r\n");
    } else {
        cliprintf("parameter length error\r\n");
    }
}
CLI_CMD_EXPORT(led_set_color, set led color temperature, cli_led_set_color_temperature)

static void
cli_led_set_status(cli_printf cliprintf, int argc, char** argv) {
    if (2 == argc) {
        int led_status;
        led_status = atoi(argv[1]);

        if (led_status == 0) {
            led_set_status(0);
        } else {
            led_set_status(1);
        }

        cliprintf("led_set_status ok\r\n");
    } else {
        cliprintf("parameter length error\r\n");
    }
}
CLI_CMD_EXPORT(led_set, set led status, cli_led_set_status)

static void
cli_night_light_set_status(cli_printf cliprintf, int argc, char** argv) {
    if (2 == argc) {
        int night_light_status;
        night_light_status = atoi(argv[1]);
        if (night_light_status != 1 && night_light_status != 0) {
            cliprintf("night_light_status : 0 / 1\r\n");
            return;
        }
        night_light_set_status(night_light_status);
        cliprintf("night_light_set_status ok\r\n");
    } else {
        cliprintf("parameter length error\r\n");
    }
}
CLI_CMD_EXPORT(nl_set, set night light status, cli_night_light_set_status)
