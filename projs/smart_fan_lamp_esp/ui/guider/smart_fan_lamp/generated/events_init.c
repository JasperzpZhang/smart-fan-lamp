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

#include "app/include.h"
/* Debug config */
#if LVGL_DEBUG || 1
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* LVGL_DEBUG */



static void home_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch(dir) {
        case LV_DIR_BOTTOM:
        {
            lv_indev_wait_release(lv_indev_get_act());
			ui_load_scr_animation(&guider_ui, &guider_ui.control, guider_ui.control_del, &guider_ui.home_del, setup_scr_control, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 200, 0, false, true);
            break;
        }
        case LV_DIR_TOP:
        {
            lv_indev_wait_release(lv_indev_get_act());
			ui_load_scr_animation(&guider_ui, &guider_ui.config, guider_ui.config_del, &guider_ui.home_del, setup_scr_config, LV_SCR_LOAD_ANIM_OVER_TOP, 200, 0, false, true);
            break;
        }
        case LV_DIR_RIGHT:
        {
            lv_indev_wait_release(lv_indev_get_act());
			ui_load_scr_animation(&guider_ui, &guider_ui.clock, guider_ui.clock_del, &guider_ui.home_del, setup_scr_clock, LV_SCR_LOAD_ANIM_OVER_RIGHT, 200, 0, false, true);
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

static void home_digital_clock_1_event_handler (lv_event_t *e)
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

void events_init_home (lv_ui *ui)
{
	lv_obj_add_event_cb(ui->home, home_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->home_digital_clock_1, home_digital_clock_1_event_handler, LV_EVENT_ALL, ui);
}

static void control_slider_2_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
	case LV_EVENT_VALUE_CHANGED:
	{
		
		lv_obj_t * slider = lv_event_get_target(e);
		uint16_t slider_value = lv_slider_get_value(slider);
		
		sys_led_set_brightness(SOURCE_SCREEN, slider_value);
		        
		
		
		
		break;
	}
    default:
        break;
    }
}

static void control_img_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
	case LV_EVENT_CLICKED:
	{
		ui_load_scr_animation(&guider_ui, &guider_ui.home, guider_ui.home_del, &guider_ui.control_del, setup_scr_home, LV_SCR_LOAD_ANIM_OVER_TOP, 200, 0, false, true);
		break;
	}
    default:
        break;
    }
}

void events_init_control (lv_ui *ui)
{
	lv_obj_add_event_cb(ui->control_slider_2, control_slider_2_event_handler, LV_EVENT_ALL, ui);
	lv_obj_add_event_cb(ui->control_img_1, control_img_1_event_handler, LV_EVENT_ALL, ui);
}

static void config_img_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
	case LV_EVENT_CLICKED:
	{
		ui_load_scr_animation(&guider_ui, &guider_ui.home, guider_ui.home_del, &guider_ui.config_del, setup_scr_home, LV_SCR_LOAD_ANIM_OVER_TOP, 200, 0, false, true);
		break;
	}
    default:
        break;
    }
}

void events_init_config (lv_ui *ui)
{
	lv_obj_add_event_cb(ui->config_img_1, config_img_1_event_handler, LV_EVENT_ALL, ui);
}

static void clock_img_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
	case LV_EVENT_CLICKED:
	{
		ui_load_scr_animation(&guider_ui, &guider_ui.home, guider_ui.home_del, &guider_ui.clock_del, setup_scr_home, LV_SCR_LOAD_ANIM_OVER_LEFT, 200, 0, false, true);
		break;
	}
    default:
        break;
    }
}

void events_init_clock (lv_ui *ui)
{
	lv_obj_add_event_cb(ui->clock_img_1, clock_img_1_event_handler, LV_EVENT_ALL, ui);
}


void events_init(lv_ui *ui)
{

}
