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

#include "app/arm_application/include.h"

/* Debug config */
#if LED_DEBUG
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* LED_DEBUG */
#if LED_ASSERT
#undef ASSERT
#define ASSERT(a)                                                   \
    while (!(a))                                                    \
    {                                                               \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__); \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* LED_ASSERT */

/* global variables */
uint16_t g_last_led_brightness = 0, g_last_led_color_temperature = 0;
uint16_t g_previous_led_brightness = 0, g_previous_led_color_temperature = 0;

/* functions */
status_t led_init(void)
{
    /* You must initalize all parameters before you can operate a function */
    g_last_led_brightness = th_led_brightness;
    g_last_led_color_temperature = th_led_color_temperature;
    adjust_led_brightness(th_led_brightness);
    adjust_led_color_temperature(th_led_color_temperature);
    if (1 == th_led_status)
    {
        HAL_TIM_PWM_Start(LED_COLD_TIM, LED_COLD_TIM_CHANNEL);
        HAL_TIM_PWM_Start(LED_WARM_TIM, LED_WARM_TIM_CHANNEL);
    }
    return status_ok;
}

/* change pwm duty */
static status_t prv_led_pwm_duty_ctrl(uint16_t led_cold_pwm_duty, uint16_t led_warm_pwm_duty)
{
    __HAL_TIM_SET_COMPARE(LED_COLD_TIM, LED_COLD_TIM_CHANNEL, led_cold_pwm_duty);
    __HAL_TIM_SET_COMPARE(LED_WARM_TIM, LED_WARM_TIM_CHANNEL, led_warm_pwm_duty);
    return status_ok;
}

/* change brightness */
status_t adjust_led_brightness(uint16_t led_brightness)
{
    g_last_led_brightness = led_brightness;
    uint16_t led_cold_brightness = 0, led_warm_brightness = 0;
    /* You can't change the color temperature when you adjust the brightness */
    led_warm_brightness = g_last_led_color_temperature * led_brightness / 1000;
    led_cold_brightness = (1000 - g_last_led_color_temperature) * led_brightness / 1000;
    prv_led_pwm_duty_ctrl(led_cold_brightness, led_warm_brightness);
    return status_ok;
}

/* change color temperature */
status_t adjust_led_color_temperature(uint16_t led_color_temperature)
{
    g_last_led_color_temperature = led_color_temperature;
    uint16_t led_cold_brightness = 0, led_warm_brightness = 0;
    /* You can't change the brightness when you adjust the color temperature */
    led_warm_brightness = led_color_temperature * g_last_led_brightness / 1000;
    led_cold_brightness = (1000 - led_color_temperature) * g_last_led_brightness / 1000;
    prv_led_pwm_duty_ctrl(led_cold_brightness, led_warm_brightness);
    return status_ok;
}

status_t save_led_data(void)
{
    th_led_brightness = g_last_led_brightness;
    th_led_color_temperature = g_last_led_color_temperature;
    data_save_direct();
    return status_ok;
}

status_t stop_led_pwm(void)
{
    HAL_TIM_PWM_Stop(LED_COLD_TIM, LED_COLD_TIM_CHANNEL);
    HAL_TIM_PWM_Stop(LED_WARM_TIM, LED_WARM_TIM_CHANNEL);
    return status_ok;
}

status_t start_led_pwm(void)
{
    HAL_TIM_PWM_Start(LED_COLD_TIM, LED_COLD_TIM_CHANNEL);
    HAL_TIM_PWM_Start(LED_WARM_TIM, LED_WARM_TIM_CHANNEL);
    return status_ok;
}

status_t toggle_led(uint16_t on_off)
{
    if (on_off != 0)
    {
        start_led_pwm();
        th_led_status = 1;
    }
    else
    {
        stop_led_pwm();
        th_led_status = 0;
    }
    data_save_direct();
    return status_ok;
}

status_t toggle_night_light(uint16_t on_off)
{
    if (on_off != 1 && on_off != 0)
    {
        TRACE("night light on_off num error\r\n");
        return status_err;
    }
    HAL_GPIO_WritePin(LED_NIGHT_LIGHT_GPIO_PORT, LED_NIGHT_LIGHT_GPIO_PIN, (on_off ? GPIO_PIN_SET : GPIO_PIN_RESET));
    return status_ok;
}

status_t smooth_adjust_led_brightness(uint16_t target_led_brightness)
{
    while (g_previous_led_brightness != target_led_brightness)
    {
        if (g_previous_led_brightness < target_led_brightness)
        {
            g_previous_led_brightness++;
            adjust_led_brightness(g_previous_led_brightness * 10);
        }

        if (g_previous_led_brightness > target_led_brightness)
        {
            g_previous_led_brightness--;
            adjust_led_brightness(g_previous_led_brightness * 10);
        }
        osDelay(15);
    }
    save_led_data();
    return status_ok;
}

status_t smooth_adjust_led_color_temperature(uint16_t target_led_color_temperature)
{
    while (g_previous_led_color_temperature != target_led_color_temperature)
    {
        if (g_previous_led_color_temperature < target_led_color_temperature)
        {
            g_previous_led_color_temperature++;
            adjust_led_color_temperature(g_previous_led_color_temperature * 10);
        }

        if (g_previous_led_color_temperature > target_led_color_temperature)
        {
            g_previous_led_color_temperature--;
            adjust_led_color_temperature(g_previous_led_color_temperature * 10);
        }
        osDelay(15);
    }
    save_led_data();
    return status_ok;
}

/* cli commond */
static void cli_adjust_led_brightness(cli_printf cliprintf, int argc, char **argv)
{
    if (2 == argc)
    {
        int led_brightness;
        led_brightness = atoi(argv[1]);
        if (led_brightness < 0 || led_brightness > 1000)
        {
            cliprintf("brightness : 0 - 1000\r\n");
            return;
        }
        adjust_led_brightness(led_brightness);
        save_led_data();
        refresh_screen(SET_LED_BR_ADDR, led_brightness / 10);
        cliprintf("adjust_led_brightness ok\r\n");
    }
    else
    {
        cliprintf("parameter length error\r\n");
    }
}
CLI_CMD_EXPORT(adjust_led_br, set led brightness, cli_adjust_led_brightness)

static void cli_adjust_led_color_temperature(cli_printf cliprintf, int argc, char **argv)
{
    if (2 == argc)
    {
        int led_color_temperature;
        led_color_temperature = atoi(argv[1]);
        if (led_color_temperature < 0 || led_color_temperature > 1000)
        {
            cliprintf("color_temperature : 0 - 1000\r\n");
            return;
        }
        adjust_led_color_temperature(led_color_temperature);
        save_led_data();
        refresh_screen(SET_LED_CT_ADDR, led_color_temperature / 10);
        cliprintf("led_color_temperature ok\r\n");
    }
    else
    {
        cliprintf("parameter length error\r\n");
    }
}
CLI_CMD_EXPORT(adjust_led_ct, set led color temperature, cli_adjust_led_color_temperature)

static void cli_toggle_led(cli_printf cliprintf, int argc, char **argv)
{
    if (2 == argc)
    {
        int led_status;
        led_status = atoi(argv[1]);
        
        if (led_status == 0)
        {
            toggle_led(0);
            refresh_screen(SET_LED_ONOFF_ADDR, 0x00);
        }
        else
        {
            toggle_led(1);
            refresh_screen(SET_LED_ONOFF_ADDR, 0x01);
        }
        
        refresh_screen(SET_LED_ONOFF_ADDR, (led_status == 0 ? 0 : 1));

        cliprintf("toggle_led ok\r\n");
    }
    else
    {
        cliprintf("parameter length error\r\n");
    }
}
CLI_CMD_EXPORT(toggle_led, set led status, cli_toggle_led)

static void cli_toggle_night_light(cli_printf cliprintf, int argc, char **argv)
{
    if (2 == argc)
    {
        int night_light_status;
        night_light_status = atoi(argv[1]);
        if (night_light_status != 1 && night_light_status != 0)
        {
            cliprintf("night_light_status : 0 / 1\r\n");
            return;
        }
        toggle_night_light(night_light_status);
        cliprintf("toggle_night_light ok\r\n");
    }
    else
    {
        cliprintf("parameter length error\r\n");
    }
}
CLI_CMD_EXPORT(toggle_nl, set night light status, cli_toggle_night_light)
