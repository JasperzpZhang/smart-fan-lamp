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



int scr_home_digital_clock_1_min_value = 30;
int scr_home_digital_clock_1_hour_value = 15;
int scr_home_digital_clock_1_sec_value = 50;
void setup_scr_scr_home(lv_ui *ui)
{
	//Write codes scr_home
	ui->scr_home = lv_obj_create(NULL);
	lv_obj_set_size(ui->scr_home, 280, 240);
	lv_obj_set_scrollbar_mode(ui->scr_home, LV_SCROLLBAR_MODE_OFF);

	//Write style for scr_home, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->scr_home, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->scr_home, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->scr_home, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes scr_home_digital_clock_1
	static bool scr_home_digital_clock_1_timer_enabled = false;
	ui->scr_home_digital_clock_1 = lv_dclock_create(ui->scr_home, "15:30");
	if (!scr_home_digital_clock_1_timer_enabled) {
		lv_timer_create(scr_home_digital_clock_1_timer, 1000, NULL);
		scr_home_digital_clock_1_timer_enabled = true;
	}
	lv_obj_set_pos(ui->scr_home_digital_clock_1, 71, 16);
	lv_obj_set_size(ui->scr_home_digital_clock_1, 143, 71);
	lv_obj_add_flag(ui->scr_home_digital_clock_1, LV_OBJ_FLAG_CLICKABLE);

	//Write style for scr_home_digital_clock_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_radius(ui->scr_home_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->scr_home_digital_clock_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->scr_home_digital_clock_1, &lv_font_Alatsi_Regular_61, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->scr_home_digital_clock_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->scr_home_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->scr_home_digital_clock_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->scr_home_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->scr_home_digital_clock_1, 7, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->scr_home_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->scr_home_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->scr_home_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->scr_home_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes scr_home_datetext_1
	ui->scr_home_datetext_1 = lv_label_create(ui->scr_home);
	lv_label_set_text(ui->scr_home_datetext_1, "2024/10/09");
	lv_obj_set_style_text_align(ui->scr_home_datetext_1, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_add_flag(ui->scr_home_datetext_1, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_add_event_cb(ui->scr_home_datetext_1, scr_home_datetext_1_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_set_pos(ui->scr_home_datetext_1, 76, 94);
	lv_obj_set_size(ui->scr_home_datetext_1, 130, 26);

	//Write style for scr_home_datetext_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_text_color(ui->scr_home_datetext_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->scr_home_datetext_1, &lv_font_Alatsi_Regular_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->scr_home_datetext_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->scr_home_datetext_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->scr_home_datetext_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->scr_home_datetext_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->scr_home_datetext_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->scr_home_datetext_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->scr_home_datetext_1, 7, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->scr_home_datetext_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->scr_home_datetext_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->scr_home_datetext_1, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_color(ui->scr_home_datetext_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_opa(ui->scr_home_datetext_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_spread(ui->scr_home_datetext_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_ofs_x(ui->scr_home_datetext_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_ofs_y(ui->scr_home_datetext_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes scr_home_animimg_1
	ui->scr_home_animimg_1 = lv_animimg_create(ui->scr_home);
	lv_animimg_set_src(ui->scr_home_animimg_1, (const void **) scr_home_animimg_1_imgs, 11, false);
	lv_animimg_set_duration(ui->scr_home_animimg_1, 1000*11);
	lv_animimg_set_repeat_count(ui->scr_home_animimg_1, LV_ANIM_REPEAT_INFINITE);
	lv_animimg_start(ui->scr_home_animimg_1);
	lv_obj_set_pos(ui->scr_home_animimg_1, 174, 139);
	lv_obj_set_size(ui->scr_home_animimg_1, 80, 80);

	//Write codes scr_home_cont_1
	ui->scr_home_cont_1 = lv_obj_create(ui->scr_home);
	lv_obj_set_pos(ui->scr_home_cont_1, 0, 240);
	lv_obj_set_size(ui->scr_home_cont_1, 280, 200);
	lv_obj_set_scrollbar_mode(ui->scr_home_cont_1, LV_SCROLLBAR_MODE_OFF);

	//Write style for scr_home_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->scr_home_cont_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->scr_home_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->scr_home_cont_1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->scr_home_cont_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->scr_home_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->scr_home_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->scr_home_cont_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->scr_home_cont_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->scr_home_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->scr_home_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->scr_home_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->scr_home_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->scr_home_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes scr_home_sw_1
	ui->scr_home_sw_1 = lv_switch_create(ui->scr_home_cont_1);
	lv_obj_set_pos(ui->scr_home_sw_1, 173, 40);
	lv_obj_set_size(ui->scr_home_sw_1, 59, 30);

	//Write style for scr_home_sw_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->scr_home_sw_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->scr_home_sw_1, lv_color_hex(0xe6e2e6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->scr_home_sw_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->scr_home_sw_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->scr_home_sw_1, 23, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->scr_home_sw_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for scr_home_sw_1, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
	lv_obj_set_style_bg_opa(ui->scr_home_sw_1, 255, LV_PART_INDICATOR|LV_STATE_CHECKED);
	lv_obj_set_style_bg_color(ui->scr_home_sw_1, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_CHECKED);
	lv_obj_set_style_bg_grad_dir(ui->scr_home_sw_1, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_CHECKED);
	lv_obj_set_style_border_width(ui->scr_home_sw_1, 0, LV_PART_INDICATOR|LV_STATE_CHECKED);

	//Write style for scr_home_sw_1, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->scr_home_sw_1, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->scr_home_sw_1, lv_color_hex(0xffffff), LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->scr_home_sw_1, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->scr_home_sw_1, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->scr_home_sw_1, 27, LV_PART_KNOB|LV_STATE_DEFAULT);

	//Write codes scr_home_sw_2
	ui->scr_home_sw_2 = lv_switch_create(ui->scr_home_cont_1);
	lv_obj_set_pos(ui->scr_home_sw_2, 176, 95);
	lv_obj_set_size(ui->scr_home_sw_2, 59, 30);

	//Write style for scr_home_sw_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->scr_home_sw_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->scr_home_sw_2, lv_color_hex(0xe6e2e6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->scr_home_sw_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->scr_home_sw_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->scr_home_sw_2, 23, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->scr_home_sw_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for scr_home_sw_2, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
	lv_obj_set_style_bg_opa(ui->scr_home_sw_2, 255, LV_PART_INDICATOR|LV_STATE_CHECKED);
	lv_obj_set_style_bg_color(ui->scr_home_sw_2, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_CHECKED);
	lv_obj_set_style_bg_grad_dir(ui->scr_home_sw_2, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_CHECKED);
	lv_obj_set_style_border_width(ui->scr_home_sw_2, 0, LV_PART_INDICATOR|LV_STATE_CHECKED);

	//Write style for scr_home_sw_2, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->scr_home_sw_2, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->scr_home_sw_2, lv_color_hex(0xffffff), LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->scr_home_sw_2, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->scr_home_sw_2, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->scr_home_sw_2, 27, LV_PART_KNOB|LV_STATE_DEFAULT);

	//Write codes scr_home_sw_3
	ui->scr_home_sw_3 = lv_switch_create(ui->scr_home_cont_1);
	lv_obj_set_pos(ui->scr_home_sw_3, 180, 149);
	lv_obj_set_size(ui->scr_home_sw_3, 59, 30);

	//Write style for scr_home_sw_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->scr_home_sw_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->scr_home_sw_3, lv_color_hex(0xe6e2e6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->scr_home_sw_3, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->scr_home_sw_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->scr_home_sw_3, 23, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->scr_home_sw_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for scr_home_sw_3, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
	lv_obj_set_style_bg_opa(ui->scr_home_sw_3, 255, LV_PART_INDICATOR|LV_STATE_CHECKED);
	lv_obj_set_style_bg_color(ui->scr_home_sw_3, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_CHECKED);
	lv_obj_set_style_bg_grad_dir(ui->scr_home_sw_3, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_CHECKED);
	lv_obj_set_style_border_width(ui->scr_home_sw_3, 0, LV_PART_INDICATOR|LV_STATE_CHECKED);

	//Write style for scr_home_sw_3, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->scr_home_sw_3, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->scr_home_sw_3, lv_color_hex(0xffffff), LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->scr_home_sw_3, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->scr_home_sw_3, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->scr_home_sw_3, 27, LV_PART_KNOB|LV_STATE_DEFAULT);

	//Write codes scr_home_label_2
	ui->scr_home_label_2 = lv_label_create(ui->scr_home);
	lv_label_set_text(ui->scr_home_label_2, "Label");
	lv_label_set_long_mode(ui->scr_home_label_2, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->scr_home_label_2, 29, 283);
	lv_obj_set_size(ui->scr_home_label_2, 100, 18);

	//Write style for scr_home_label_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->scr_home_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->scr_home_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->scr_home_label_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->scr_home_label_2, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->scr_home_label_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->scr_home_label_2, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->scr_home_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->scr_home_label_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->scr_home_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->scr_home_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->scr_home_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->scr_home_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->scr_home_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->scr_home_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes scr_home_label_3
	ui->scr_home_label_3 = lv_label_create(ui->scr_home);
	lv_label_set_text(ui->scr_home_label_3, "Label");
	lv_label_set_long_mode(ui->scr_home_label_3, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->scr_home_label_3, 29, 344);
	lv_obj_set_size(ui->scr_home_label_3, 100, 18);

	//Write style for scr_home_label_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->scr_home_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->scr_home_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->scr_home_label_3, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->scr_home_label_3, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->scr_home_label_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->scr_home_label_3, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->scr_home_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->scr_home_label_3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->scr_home_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->scr_home_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->scr_home_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->scr_home_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->scr_home_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->scr_home_label_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes scr_home_label_4
	ui->scr_home_label_4 = lv_label_create(ui->scr_home);
	lv_label_set_text(ui->scr_home_label_4, "Label");
	lv_label_set_long_mode(ui->scr_home_label_4, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->scr_home_label_4, 29, 392);
	lv_obj_set_size(ui->scr_home_label_4, 100, 18);

	//Write style for scr_home_label_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->scr_home_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->scr_home_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->scr_home_label_4, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->scr_home_label_4, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->scr_home_label_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->scr_home_label_4, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->scr_home_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->scr_home_label_4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->scr_home_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->scr_home_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->scr_home_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->scr_home_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->scr_home_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->scr_home_label_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//The custom code of scr_home.
	

	//Update current screen layout.
	lv_obj_update_layout(ui->scr_home);

	//Init events for screen.
	events_init_scr_home(ui);
}
