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

void
fan_init(void) {
    if (th_mem_en == 0) {
        return;
    }

    /* You must initalize all parameters before you can operate a function */
    fan_set_speed(th_fan_polarity, th_fan_speed);

    if (1 == th_fan_status) {
        HAL_TIM_PWM_Start(FAN_FORWARD_TIM, FAN_FORWARD_CHANNEL);
        // HAL_TIM_PWM_Start(FAN_REVERSE_TIM, FAN_REVERSE_CHANNEL);
    }
}

/* set pwm duty */
void
prv_fan_set_pwm_duty(uint16_t fan_polarity, uint16_t fan_pwm_duty) {
    if (fan_polarity == 1) {
        __HAL_TIM_SET_COMPARE(FAN_FORWARD_TIM, FAN_FORWARD_CHANNEL, fan_pwm_duty);
    }
}

/* set fan speed */
void
fan_set_speed(uint16_t fan_polarity, uint16_t fan_speed) {
    if (th_fan_speed != fan_speed) {
        th_fan_polarity = fan_polarity;
        th_fan_speed = fan_speed;
        // data_save_direct();
    }

    /* fan speed -> fan pwm duty */
    uint16_t fan_pwm_duty = fan_speed;

    /* set fan pwm duty */
    prv_fan_set_pwm_duty(fan_polarity, fan_pwm_duty);
}

status_t
fan_stop(void) {
    HAL_TIM_PWM_Stop(FAN_FORWARD_TIM, FAN_FORWARD_CHANNEL);
    return status_ok;
}

status_t
fan_start(void) {
    HAL_TIM_PWM_Start(FAN_FORWARD_TIM, FAN_FORWARD_CHANNEL);
    return status_ok;
}

status_t
fan_set_status(uint16_t on_off) {
    if (on_off != 0) {
        fan_start();
        th_fan_status = 1;
    } else {
        fan_stop();
        th_fan_status = 0;
    }
//    data_save_direct();
    return status_ok;
}

status_t
fan_set_level(uint16_t fan_level) {
    switch (fan_level) {
        case 1: fan_set_speed(1, 50); break;
        case 2: fan_set_speed(1, 70); break;
        case 3: fan_set_speed(1, 100); break;
    }
//    data_save_direct();
    return status_ok;
}

static void
cli_fan_set_speed(cli_printf cliprintf, int argc, char** argv) {
    if (2 == argc) {
        uint16_t fan_speed = atoi(argv[1]);

        if (fan_speed < 0 || fan_speed > 100) {
            cliprintf("fan_speed : 0 - 100");
            return;
        }

        fan_set_speed(1, fan_speed);
        cliprintf("fan_set_speed ok\r\n");
    } else {
        cliprintf("parameter length error\r\n");
    }
}
CLI_CMD_EXPORT(fan_set_speed, set fan speed, cli_fan_set_speed)

static void
cli_fan_set_status(cli_printf cliprintf, int argc, char** argv) {
    if (2 == argc) {
        th_fan_status = atoi(argv[1]);

        if (th_fan_status == 1) {
            fan_start();
        } else {
            fan_stop();
        }

        data_save_direct();

        cliprintf("fan_set_status ok\r\n");
    } else {
        cliprintf("parameter length error\r\n");
    }
}
CLI_CMD_EXPORT(fan_set_status, set fan status, cli_fan_set_status)
