/*
* Copyright 2023 NXP
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include "app/include.h"
#include "custom.h"
#include "lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
void scr_status_init(lv_ui* ui);

/**********************
 *  STATIC VARIABLES
 **********************/

/**
 * Create a demo application
 */

void
custom_init(lv_ui* ui) {
    /* Add your codes here */
    setup_ui(ui);

    scr_status_init(ui);
}

void
scr_status_init(lv_ui* ui) {

    /* slider brightness */
    lv_slider_set_value(ui->control_slider_2, g_led_ctrl.led_brightness * 190 / 100, LV_ANIM_OFF);
}
