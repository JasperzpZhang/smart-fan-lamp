/*
* Copyright 2024 NXP
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"



int scr_clock_analog_clock_1_hour_value = 3;
int scr_clock_analog_clock_1_min_value = 34;
int scr_clock_analog_clock_1_sec_value = 50;
void setup_scr_scr_clock(lv_ui *ui)
{
	//Write codes scr_clock
	ui->scr_clock = lv_obj_create(NULL);
	lv_obj_set_size(ui->scr_clock, 280, 240);
	lv_obj_set_scrollbar_mode(ui->scr_clock, LV_SCROLLBAR_MODE_OFF);

	//Write style for scr_clock, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->scr_clock, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->scr_clock, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->scr_clock, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes scr_clock_analog_clock_1
	static bool scr_clock_analog_clock_1_timer_enabled = false;
	ui->scr_clock_analog_clock_1 = lv_analogclock_create(ui->scr_clock);
	lv_analogclock_hide_digits(ui->scr_clock_analog_clock_1, false);
	lv_analogclock_set_major_ticks(ui->scr_clock_analog_clock_1, 2, 10, lv_color_hex(0x25a248), 10);
	lv_analogclock_set_ticks(ui->scr_clock_analog_clock_1, 2, 5, lv_color_hex(0x5c5959));
	lv_analogclock_set_hour_needle_line(ui->scr_clock_analog_clock_1, 2, lv_color_hex(0x00ff00), -40);
	lv_analogclock_set_min_needle_line(ui->scr_clock_analog_clock_1, 2, lv_color_hex(0x705db5), -30);
	lv_analogclock_set_sec_needle_line(ui->scr_clock_analog_clock_1, 2, lv_color_hex(0xaf4f0f), -10);
	lv_analogclock_set_time(ui->scr_clock_analog_clock_1, scr_clock_analog_clock_1_hour_value, scr_clock_analog_clock_1_min_value,scr_clock_analog_clock_1_sec_value);
	// create timer
	if (!scr_clock_analog_clock_1_timer_enabled) {
		lv_timer_create(scr_clock_analog_clock_1_timer, 1000, NULL);
		scr_clock_analog_clock_1_timer_enabled = true;
	}
	lv_obj_set_pos(ui->scr_clock_analog_clock_1, 40, 20);
	lv_obj_set_size(ui->scr_clock_analog_clock_1, 200, 200);

	//Write style for scr_clock_analog_clock_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->scr_clock_analog_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->scr_clock_analog_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->scr_clock_analog_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for scr_clock_analog_clock_1, Part: LV_PART_TICKS, State: LV_STATE_DEFAULT.
	lv_obj_set_style_text_color(ui->scr_clock_analog_clock_1, lv_color_hex(0x0ace10), LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->scr_clock_analog_clock_1, &lv_font_montserratMedium_12, LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->scr_clock_analog_clock_1, 255, LV_PART_TICKS|LV_STATE_DEFAULT);

	//Write style for scr_clock_analog_clock_1, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->scr_clock_analog_clock_1, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->scr_clock_analog_clock_1, lv_color_hex(0x10251d), LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->scr_clock_analog_clock_1, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//The custom code of scr_clock.
	

	//Update current screen layout.
	lv_obj_update_layout(ui->scr_clock);

	//Init events for screen.
	events_init_scr_clock(ui);
}
