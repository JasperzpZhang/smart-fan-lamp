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



int home_digital_clock_1_min_value = 48;
int home_digital_clock_1_hour_value = 23;
int home_digital_clock_1_sec_value = 55;
void setup_scr_home(lv_ui *ui)
{
	//Write codes home
	ui->home = lv_obj_create(NULL);
	lv_obj_set_size(ui->home, 280, 240);
	lv_obj_set_scrollbar_mode(ui->home, LV_SCROLLBAR_MODE_OFF);

	//Write style for home, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->home, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->home, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->home, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes home_label_12
	ui->home_label_12 = lv_label_create(ui->home);
	lv_label_set_text(ui->home_label_12, "10月15日");
	lv_label_set_long_mode(ui->home_label_12, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->home_label_12, 23, 120);
	lv_obj_set_size(ui->home_label_12, 120, 27);

	//Write style for home_label_12, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->home_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->home_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->home_label_12, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->home_label_12, &lv_font_SourceHanSerifSC_Regular_24, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->home_label_12, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->home_label_12, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->home_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->home_label_12, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->home_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->home_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->home_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->home_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->home_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->home_label_12, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes home_digital_clock_1
	static bool home_digital_clock_1_timer_enabled = false;
	ui->home_digital_clock_1 = lv_dclock_create(ui->home, "23:48");
	if (!home_digital_clock_1_timer_enabled) {
		lv_timer_create(home_digital_clock_1_timer, 1000, NULL);
		home_digital_clock_1_timer_enabled = true;
	}
	lv_obj_set_pos(ui->home_digital_clock_1, 15, 38);
	lv_obj_set_size(ui->home_digital_clock_1, 159, 76);

	//Write style for home_digital_clock_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_radius(ui->home_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->home_digital_clock_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->home_digital_clock_1, &lv_font_Alatsi_Regular_60, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->home_digital_clock_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->home_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->home_digital_clock_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->home_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->home_digital_clock_1, 7, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->home_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->home_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->home_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->home_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes home_animimg_1
	ui->home_animimg_1 = lv_animimg_create(ui->home);
	lv_animimg_set_src(ui->home_animimg_1, (const void **) home_animimg_1_imgs, 11, false);
	lv_animimg_set_duration(ui->home_animimg_1, 1000*11);
	lv_animimg_set_repeat_count(ui->home_animimg_1, LV_ANIM_REPEAT_INFINITE);
	lv_animimg_start(ui->home_animimg_1);
	lv_obj_set_pos(ui->home_animimg_1, 185, 28);
	lv_obj_set_size(ui->home_animimg_1, 80, 80);

	//Write codes home_label_2
	ui->home_label_2 = lv_label_create(ui->home);
	lv_label_set_text(ui->home_label_2, "星期一");
	lv_label_set_long_mode(ui->home_label_2, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->home_label_2, 144, 120);
	lv_obj_set_size(ui->home_label_2, 88, 27);

	//Write style for home_label_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->home_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->home_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->home_label_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->home_label_2, &lv_font_SourceHanSerifSC_Regular_24, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->home_label_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->home_label_2, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->home_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->home_label_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->home_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->home_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->home_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->home_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->home_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->home_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes home_cont_1
	ui->home_cont_1 = lv_obj_create(ui->home);
	lv_obj_set_pos(ui->home_cont_1, 23, 165);
	lv_obj_set_size(ui->home_cont_1, 114, 50);
	lv_obj_set_scrollbar_mode(ui->home_cont_1, LV_SCROLLBAR_MODE_OFF);

	//Write style for home_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->home_cont_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->home_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->home_cont_1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->home_cont_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->home_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->home_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->home_cont_1, lv_color_hex(0x0d0d12), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->home_cont_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->home_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->home_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->home_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->home_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->home_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes home_img_1
	ui->home_img_1 = lv_img_create(ui->home_cont_1);
	lv_obj_add_flag(ui->home_img_1, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->home_img_1, &_temperature_alpha_30x30);
	lv_img_set_pivot(ui->home_img_1, 50,50);
	lv_img_set_angle(ui->home_img_1, 0);
	lv_obj_set_pos(ui->home_img_1, 3, 10);
	lv_obj_set_size(ui->home_img_1, 30, 30);

	//Write style for home_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_recolor_opa(ui->home_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_img_opa(ui->home_img_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->home_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_clip_corner(ui->home_img_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes home_label_6
	ui->home_label_6 = lv_label_create(ui->home_cont_1);
	lv_label_set_text(ui->home_label_6, "28 ℃");
	lv_label_set_long_mode(ui->home_label_6, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->home_label_6, 40, 17);
	lv_obj_set_size(ui->home_label_6, 58, 19);

	//Write style for home_label_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->home_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->home_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->home_label_6, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->home_label_6, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->home_label_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->home_label_6, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->home_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->home_label_6, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->home_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->home_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->home_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->home_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->home_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->home_label_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes home_cont_3
	ui->home_cont_3 = lv_obj_create(ui->home);
	lv_obj_set_pos(ui->home_cont_3, 15, 113);
	lv_obj_set_size(ui->home_cont_3, 121, 34);
	lv_obj_set_scrollbar_mode(ui->home_cont_3, LV_SCROLLBAR_MODE_OFF);

	//Write style for home_cont_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->home_cont_3, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->home_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->home_cont_3, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->home_cont_3, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->home_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->home_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->home_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->home_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->home_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->home_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->home_cont_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes home_cont_4
	ui->home_cont_4 = lv_obj_create(ui->home);
	lv_obj_set_pos(ui->home_cont_4, 144, 165);
	lv_obj_set_size(ui->home_cont_4, 114, 50);
	lv_obj_set_scrollbar_mode(ui->home_cont_4, LV_SCROLLBAR_MODE_OFF);

	//Write style for home_cont_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->home_cont_4, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->home_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->home_cont_4, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->home_cont_4, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->home_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->home_cont_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->home_cont_4, lv_color_hex(0x0d0d12), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->home_cont_4, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->home_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->home_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->home_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->home_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->home_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes home_img_2
	ui->home_img_2 = lv_img_create(ui->home_cont_4);
	lv_obj_add_flag(ui->home_img_2, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->home_img_2, &_Humidity_alpha_30x30);
	lv_img_set_pivot(ui->home_img_2, 50,50);
	lv_img_set_angle(ui->home_img_2, 0);
	lv_obj_set_pos(ui->home_img_2, 3, 10);
	lv_obj_set_size(ui->home_img_2, 30, 30);

	//Write style for home_img_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_recolor_opa(ui->home_img_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_img_opa(ui->home_img_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->home_img_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_clip_corner(ui->home_img_2, true, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes home_label_7
	ui->home_label_7 = lv_label_create(ui->home_cont_4);
	lv_label_set_text(ui->home_label_7, "63");
	lv_label_set_long_mode(ui->home_label_7, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->home_label_7, 40, 17);
	lv_obj_set_size(ui->home_label_7, 47, 19);

	//Write style for home_label_7, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->home_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->home_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->home_label_7, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->home_label_7, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->home_label_7, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->home_label_7, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->home_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->home_label_7, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->home_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->home_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->home_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->home_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->home_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->home_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes home_cont_5
	ui->home_cont_5 = lv_obj_create(ui->home);
	lv_obj_set_pos(ui->home_cont_5, 0, 0);
	lv_obj_set_size(ui->home_cont_5, 280, 30);
	lv_obj_set_scrollbar_mode(ui->home_cont_5, LV_SCROLLBAR_MODE_OFF);

	//Write style for home_cont_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->home_cont_5, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->home_cont_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->home_cont_5, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->home_cont_5, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->home_cont_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->home_cont_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->home_cont_5, lv_color_hex(0x0d0d12), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->home_cont_5, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->home_cont_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->home_cont_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->home_cont_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->home_cont_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->home_cont_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes home_label_8
	ui->home_label_8 = lv_label_create(ui->home_cont_5);
	lv_label_set_text(ui->home_label_8, "" LV_SYMBOL_WIFI " ");
	lv_label_set_long_mode(ui->home_label_8, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->home_label_8, 230, 5);
	lv_obj_set_size(ui->home_label_8, 20, 20);

	//Write style for home_label_8, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->home_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->home_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->home_label_8, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->home_label_8, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->home_label_8, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->home_label_8, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->home_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->home_label_8, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->home_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->home_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->home_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->home_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->home_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->home_label_8, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes home_label_10
	ui->home_label_10 = lv_label_create(ui->home_cont_5);
	lv_label_set_text(ui->home_label_10, "" LV_SYMBOL_VOLUME_MAX " ");
	lv_label_set_long_mode(ui->home_label_10, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->home_label_10, 200, 5);
	lv_obj_set_size(ui->home_label_10, 20, 20);

	//Write style for home_label_10, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->home_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->home_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->home_label_10, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->home_label_10, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->home_label_10, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->home_label_10, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->home_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->home_label_10, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->home_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->home_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->home_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->home_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->home_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->home_label_10, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes home_label_11
	ui->home_label_11 = lv_label_create(ui->home_cont_5);
	lv_label_set_text(ui->home_label_11, "" LV_SYMBOL_BELL " ");
	lv_label_set_long_mode(ui->home_label_11, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->home_label_11, 170, 5);
	lv_obj_set_size(ui->home_label_11, 20, 20);

	//Write style for home_label_11, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->home_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->home_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->home_label_11, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->home_label_11, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->home_label_11, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->home_label_11, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->home_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->home_label_11, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->home_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->home_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->home_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->home_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->home_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->home_label_11, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//The custom code of home.
	

	//Update current screen layout.
	lv_obj_update_layout(ui->home);

	//Init events for screen.
	events_init_home(ui);
}
