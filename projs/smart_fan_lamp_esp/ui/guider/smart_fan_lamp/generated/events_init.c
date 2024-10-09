/*
* Copyright 2024 NXP
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"

#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif


static void scr_home_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch(dir) {
        case LV_DIR_LEFT:
        {
            lv_indev_wait_release(lv_indev_get_act());
			ui_load_scr_animation(&guider_ui, &guider_ui.scr_clock, guider_ui.scr_clock_del, &guider_ui.scr_home_del, setup_scr_scr_clock, LV_SCR_LOAD_ANIM_NONE, 200, 200, false, true);
            break;
        }
        case LV_DIR_RIGHT:
        {
            lv_indev_wait_release(lv_indev_get_act());
			ui_load_scr_animation(&guider_ui, &guider_ui.scr_ctrl, guider_ui.scr_ctrl_del, &guider_ui.scr_home_del, setup_scr_scr_ctrl, LV_SCR_LOAD_ANIM_NONE, 200, 200, false, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void scr_home_digital_clock_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
	case LV_EVENT_CLICKED:
	{
		break;
	}
    default:
        break;
    }
}

void events_init_scr_home (lv_ui *ui)
{
	lv_obj_add_event_cb(ui->scr_home, scr_home_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->scr_home_digital_clock_1, scr_home_digital_clock_1_event_handler, LV_EVENT_ALL, ui);
}

static void scr_ctrl_btn_2_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
	case LV_EVENT_CLICKED:
	{
		ui_load_scr_animation(&guider_ui, &guider_ui.scr_home, guider_ui.scr_home_del, &guider_ui.scr_ctrl_del, setup_scr_scr_home, LV_SCR_LOAD_ANIM_NONE, 200, 200, false, true);
		break;
	}
    default:
        break;
    }
}

void events_init_scr_ctrl (lv_ui *ui)
{
	lv_obj_add_event_cb(ui->scr_ctrl_btn_2, scr_ctrl_btn_2_event_handler, LV_EVENT_ALL, ui);
}

static void scr_clock_analog_clock_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
	case LV_EVENT_CLICKED:
	{
		ui_load_scr_animation(&guider_ui, &guider_ui.scr_home, guider_ui.scr_home_del, &guider_ui.scr_clock_del, setup_scr_scr_home, LV_SCR_LOAD_ANIM_NONE, 200, 200, false, true);
		break;
	}
    default:
        break;
    }
}

void events_init_scr_clock (lv_ui *ui)
{
	lv_obj_add_event_cb(ui->scr_clock_analog_clock_1, scr_clock_analog_clock_1_event_handler, LV_EVENT_ALL, ui);
}


void events_init(lv_ui *ui)
{

}
