/**
 * Copyright (c) 2024 Jasper All rights reserved
 *
 * @file      fan.c
 * @brief     Implementation File for Fan Module
 * @version   1.0.0
 * @author    Jasper
 * @date      2024-09-26
 *
 */

/**
   modification history
   --------------------
   01a, 26Sep24, Jasper Created
 */

/* Includes */
#include "app/include.h"

#pragma diag_suppress 186

fan_ctrl_t g_fan_ctrl;

status_t
fan_init(void) {
    /* You must initalize all parameters before you can operate a function */
    g_fan_ctrl.last_fan_speed = th_fan_speed;
    g_fan_ctrl.fan_status = th_fan_status;

    fan_start();

    if (g_fan_ctrl.fan_status == 1) {
        // fan_set_status(1);
    }

    return status_ok;
}

/* set pwm duty */
status_t
prv_fan_set_pwm_duty(uint16_t fan_pwm_duty) {
    __HAL_TIM_SET_COMPARE(FAN_FORWARD_TIM, FAN_FORWARD_CHANNEL, fan_pwm_duty);
    return status_ok;
}

/* set fan speed smooth*/
status_t
fan_set_speed_smooth_blk(uint16_t fan_speed) {
    while (g_fan_ctrl.fan_speed != fan_speed) {

        if (g_fan_ctrl.fan_speed < fan_speed) {
            g_fan_ctrl.fan_speed++;
        } else if (g_fan_ctrl.fan_speed > fan_speed) {
            g_fan_ctrl.fan_speed--;
        }
        fan_set_speed(g_fan_ctrl.fan_speed);
    }
    return status_ok;
}

/* set fan speed smooth*/
status_t
fan_set_speed_smooth(uint16_t fan_speed) {

    if (g_fan_ctrl.fan_speed < fan_speed) {
        g_fan_ctrl.fan_speed++;
    } else if (g_fan_ctrl.fan_speed > fan_speed) {
        g_fan_ctrl.fan_speed--;
    }
    fan_set_speed(g_fan_ctrl.fan_speed);
    return status_ok;
}

status_t
fan_set_speed(uint16_t fan_speed) {
    g_fan_ctrl.fan_speed = fan_speed;
    /* fan speed -> fan pwm duty */
    uint16_t fan_pwm_duty = fan_speed;
    /* set fan pwm duty */
    prv_fan_set_pwm_duty(fan_pwm_duty);
    return status_ok;
}

status_t
fan_start(void) {
    HAL_TIM_PWM_Start(FAN_FORWARD_TIM, FAN_FORWARD_CHANNEL);
    return status_ok;
}

status_t
fan_stop(void) {
    HAL_TIM_PWM_Stop(FAN_FORWARD_TIM, FAN_FORWARD_CHANNEL);
    return status_ok;
}

status_t
fan_set_status(uint16_t on_off) {
    if (on_off != 0) {
        //        fan_start();
        g_fan_ctrl.fan_status = 1;
        panel_set_led_status(fan, panel_led_on);
        if (g_fan_ctrl.last_fan_speed == 0) {
            g_fan_ctrl.last_fan_speed = 50;
        }
        fan_set_speed(g_fan_ctrl.last_fan_speed);
    } else {
        //        fan_stop();
        g_fan_ctrl.last_fan_speed = g_fan_ctrl.fan_speed;
        g_fan_ctrl.fan_status = 0;
        panel_set_led_status(fan, panel_led_off);
        fan_set_speed(0);
    }
    return status_ok;
}

status_t
fan_set_level(uint16_t fan_level) {
    switch (fan_level) {
        case 0:
            fan_set_speed(0);
            g_fan_ctrl.fan_speed = 0;
            break;
        case 1:
            fan_set_speed(50);
            g_fan_ctrl.fan_speed = 50;
            break;
        case 2:
            fan_set_speed(70);
            g_fan_ctrl.fan_speed = 70;
            break;
        case 3:
            fan_set_speed(100);
            g_fan_ctrl.fan_speed = 100;
            break;
    }
    return status_ok;
}

status_t
fan_save_status(void) {
    th_fan_speed = g_fan_ctrl.fan_speed;
    th_fan_status = g_fan_ctrl.fan_status;
    data_save_direct();
    return status_ok;
}

static void
cli_fan_set_speed(cli_printf cliprintf, int argc, char** argv) {
    if (2 == argc) {
        g_fan_ctrl.fan_speed = atoi(argv[1]);

        if (g_fan_ctrl.fan_speed < 0 || g_fan_ctrl.fan_speed > 100) {
            cliprintf("fan_speed : 0 - 100");
            return;
        }

        fan_set_speed(g_fan_ctrl.fan_speed);
        cliprintf("fan_set_speed ok\r\n");
    } else {
        cliprintf("parameter length error\r\n");
    }
}
CLI_CMD_EXPORT(fan_set_speed, set fan speed, cli_fan_set_speed)

static void
cli_fan_set_status(cli_printf cliprintf, int argc, char** argv) {
    if (2 == argc) {
        g_fan_ctrl.fan_status = atoi(argv[1]);

        if (g_fan_ctrl.fan_status != 0) {
            fan_set_status(1);
        } else {
            fan_set_status(0);
        }

        cliprintf("fan_set_status ok\r\n");
    } else {
        cliprintf("parameter length error\r\n");
    }
}
CLI_CMD_EXPORT(fan_set_status, set fan status, cli_fan_set_status)

static void
cli_cmd_fan_save_status(cli_printf cliprintf, int argc, char** argv) {
    if (1 == argc) {
        fan_save_status();

        cliprintf("fan_save_status ok\r\n");
    } else {
        cliprintf("parameter length error\r\n");
    }
}
CLI_CMD_EXPORT(fan_save, set fan save status, cli_cmd_fan_save_status)
