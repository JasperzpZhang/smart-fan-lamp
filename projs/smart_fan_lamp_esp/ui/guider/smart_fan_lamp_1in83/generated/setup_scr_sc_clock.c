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



int sc_clock_analog_clock_1_hour_value = 3;
int sc_clock_analog_clock_1_min_value = 20;
int sc_clock_analog_clock_1_sec_value = 50;
void setup_scr_sc_clock(lv_ui *ui)
{
	//Write codes sc_clock
	ui->sc_clock = lv_obj_create(NULL);
	lv_obj_set_size(ui->sc_clock, 280, 240);
	lv_obj_set_scrollbar_mode(ui->sc_clock, LV_SCROLLBAR_MODE_OFF);

	//Write style for sc_clock, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->sc_clock, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->sc_clock, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->sc_clock, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes sc_clock_analog_clock_1
	static bool sc_clock_analog_clock_1_timer_enabled = false;
	ui->sc_clock_analog_clock_1 = lv_analogclock_create(ui->sc_clock);
	lv_analogclock_hide_digits(ui->sc_clock_analog_clock_1, false);
	lv_analogclock_set_major_ticks(ui->sc_clock_analog_clock_1, 2, 10, lv_color_hex(0x555555), 10);
	lv_analogclock_set_ticks(ui->sc_clock_analog_clock_1, 2, 5, lv_color_hex(0x333333));
	lv_analogclock_set_hour_needle_line(ui->sc_clock_analog_clock_1, 2, lv_color_hex(0x00ff00), -40);
	lv_analogclock_set_min_needle_line(ui->sc_clock_analog_clock_1, 2, lv_color_hex(0xE1FF00), -30);
	lv_analogclock_set_sec_needle_line(ui->sc_clock_analog_clock_1, 2, lv_color_hex(0x6600FF), -10);
	lv_analogclock_set_time(ui->sc_clock_analog_clock_1, sc_clock_analog_clock_1_hour_value, sc_clock_analog_clock_1_min_value,sc_clock_analog_clock_1_sec_value);
	// create timer
	if (!sc_clock_analog_clock_1_timer_enabled) {
		lv_timer_create(sc_clock_analog_clock_1_timer, 1000, NULL);
		sc_clock_analog_clock_1_timer_enabled = true;
	}
	lv_obj_set_pos(ui->sc_clock_analog_clock_1, 39, 18);
	lv_obj_set_size(ui->sc_clock_analog_clock_1, 200, 200);

	//Write style for sc_clock_analog_clock_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->sc_clock_analog_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->sc_clock_analog_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->sc_clock_analog_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for sc_clock_analog_clock_1, Part: LV_PART_TICKS, State: LV_STATE_DEFAULT.
	lv_obj_set_style_text_color(ui->sc_clock_analog_clock_1, lv_color_hex(0xff0000), LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->sc_clock_analog_clock_1, &lv_font_montserratMedium_12, LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->sc_clock_analog_clock_1, 255, LV_PART_TICKS|LV_STATE_DEFAULT);

	//Write style for sc_clock_analog_clock_1, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->sc_clock_analog_clock_1, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->sc_clock_analog_clock_1, lv_color_hex(0x000000), LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->sc_clock_analog_clock_1, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//The custom code of sc_clock.
	

	//Update current screen layout.
	lv_obj_update_layout(ui->sc_clock);

	//Init events for screen.
	events_init_sc_clock(ui);
}
