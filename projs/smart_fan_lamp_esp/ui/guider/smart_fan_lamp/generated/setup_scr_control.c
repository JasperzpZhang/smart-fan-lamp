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



void setup_scr_control(lv_ui *ui)
{
	//Write codes control
	ui->control = lv_obj_create(NULL);
	lv_obj_set_size(ui->control, 280, 240);
	lv_obj_set_scrollbar_mode(ui->control, LV_SCROLLBAR_MODE_OFF);

	//Write style for control, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->control, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->control, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->control, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes control_slider_2
	ui->control_slider_2 = lv_slider_create(ui->control);
	lv_slider_set_range(ui->control_slider_2, 0, 190);
	lv_slider_set_mode(ui->control_slider_2, LV_SLIDER_MODE_NORMAL);
	lv_slider_set_value(ui->control_slider_2, 100, LV_ANIM_OFF);
	lv_obj_set_pos(ui->control_slider_2, 23, 146);
	lv_obj_set_size(ui->control_slider_2, 231, 26);

	//Write style for control_slider_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->control_slider_2, 56, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->control_slider_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->control_slider_2, LV_GRAD_DIR_HOR, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_color(ui->control_slider_2, lv_color_hex(0xff6500), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_main_stop(ui->control_slider_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_stop(ui->control_slider_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->control_slider_2, 50, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_outline_width(ui->control_slider_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->control_slider_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for control_slider_2, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->control_slider_2, 0, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->control_slider_2, 50, LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//Write style for control_slider_2, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->control_slider_2, 131, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->control_slider_2, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->control_slider_2, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->control_slider_2, 50, LV_PART_KNOB|LV_STATE_DEFAULT);

	//Write codes control_img_1
	ui->control_img_1 = lv_img_create(ui->control);
	lv_obj_add_flag(ui->control_img_1, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->control_img_1, &_home_alpha_40x40);
	lv_img_set_pivot(ui->control_img_1, 50,50);
	lv_img_set_angle(ui->control_img_1, 0);
	lv_obj_set_pos(ui->control_img_1, 23, 17);
	lv_obj_set_size(ui->control_img_1, 40, 40);

	//Write style for control_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_recolor_opa(ui->control_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_img_opa(ui->control_img_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->control_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_clip_corner(ui->control_img_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes control_imgbtn_1
	ui->control_imgbtn_1 = lv_imgbtn_create(ui->control);
	lv_obj_add_flag(ui->control_imgbtn_1, LV_OBJ_FLAG_CHECKABLE);
	lv_imgbtn_set_src(ui->control_imgbtn_1, LV_IMGBTN_STATE_RELEASED, NULL, &_led_switch_alpha_40x40, NULL);
	ui->control_imgbtn_1_label = lv_label_create(ui->control_imgbtn_1);
	lv_label_set_text(ui->control_imgbtn_1_label, "");
	lv_label_set_long_mode(ui->control_imgbtn_1_label, LV_LABEL_LONG_WRAP);
	lv_obj_align(ui->control_imgbtn_1_label, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_pad_all(ui->control_imgbtn_1, 0, LV_STATE_DEFAULT);
	lv_obj_set_pos(ui->control_imgbtn_1, 115, 17);
	lv_obj_set_size(ui->control_imgbtn_1, 40, 40);

	//Write style for control_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_text_color(ui->control_imgbtn_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->control_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->control_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->control_imgbtn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->control_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_clip_corner(ui->control_imgbtn_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->control_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for control_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
	lv_obj_set_style_img_recolor_opa(ui->control_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_img_opa(ui->control_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_text_color(ui->control_imgbtn_1, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_text_font(ui->control_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_text_opa(ui->control_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_shadow_width(ui->control_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_PRESSED);

	//Write style for control_imgbtn_1, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
	lv_obj_set_style_img_recolor_opa(ui->control_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_img_opa(ui->control_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_text_color(ui->control_imgbtn_1, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_text_font(ui->control_imgbtn_1, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_text_opa(ui->control_imgbtn_1, 255, LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_shadow_width(ui->control_imgbtn_1, 0, LV_PART_MAIN|LV_STATE_CHECKED);

	//Write style for control_imgbtn_1, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
	lv_obj_set_style_img_recolor_opa(ui->control_imgbtn_1, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
	lv_obj_set_style_img_opa(ui->control_imgbtn_1, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

	//Write codes control_imgbtn_2
	ui->control_imgbtn_2 = lv_imgbtn_create(ui->control);
	lv_obj_add_flag(ui->control_imgbtn_2, LV_OBJ_FLAG_CHECKABLE);
	lv_imgbtn_set_src(ui->control_imgbtn_2, LV_IMGBTN_STATE_RELEASED, NULL, &_fan_select_alpha_37x38, NULL);
	ui->control_imgbtn_2_label = lv_label_create(ui->control_imgbtn_2);
	lv_label_set_text(ui->control_imgbtn_2_label, "");
	lv_label_set_long_mode(ui->control_imgbtn_2_label, LV_LABEL_LONG_WRAP);
	lv_obj_align(ui->control_imgbtn_2_label, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_pad_all(ui->control_imgbtn_2, 0, LV_STATE_DEFAULT);
	lv_obj_set_pos(ui->control_imgbtn_2, 195, 17);
	lv_obj_set_size(ui->control_imgbtn_2, 37, 38);

	//Write style for control_imgbtn_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_text_color(ui->control_imgbtn_2, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->control_imgbtn_2, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->control_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->control_imgbtn_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->control_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_clip_corner(ui->control_imgbtn_2, true, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->control_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for control_imgbtn_2, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
	lv_obj_set_style_img_recolor_opa(ui->control_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_img_opa(ui->control_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_text_color(ui->control_imgbtn_2, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_text_font(ui->control_imgbtn_2, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_text_opa(ui->control_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_shadow_width(ui->control_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_PRESSED);

	//Write style for control_imgbtn_2, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
	lv_obj_set_style_img_recolor_opa(ui->control_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_img_opa(ui->control_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_text_color(ui->control_imgbtn_2, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_text_font(ui->control_imgbtn_2, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_text_opa(ui->control_imgbtn_2, 255, LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_shadow_width(ui->control_imgbtn_2, 0, LV_PART_MAIN|LV_STATE_CHECKED);

	//Write style for control_imgbtn_2, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
	lv_obj_set_style_img_recolor_opa(ui->control_imgbtn_2, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
	lv_obj_set_style_img_opa(ui->control_imgbtn_2, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

	//Write codes control_imgbtn_4
	ui->control_imgbtn_4 = lv_imgbtn_create(ui->control);
	lv_obj_add_flag(ui->control_imgbtn_4, LV_OBJ_FLAG_CHECKABLE);
	lv_imgbtn_set_src(ui->control_imgbtn_4, LV_IMGBTN_STATE_RELEASED, NULL, &_led_unselect_alpha_50x50, NULL);
	lv_imgbtn_set_src(ui->control_imgbtn_4, LV_IMGBTN_STATE_PRESSED, NULL, &_led_unselect_alpha_50x50, NULL);
	lv_imgbtn_set_src(ui->control_imgbtn_4, LV_IMGBTN_STATE_CHECKED_RELEASED, NULL, &_led_select_alpha_50x50, NULL);
	lv_imgbtn_set_src(ui->control_imgbtn_4, LV_IMGBTN_STATE_CHECKED_PRESSED, NULL, &_led_select_alpha_50x50, NULL);
	ui->control_imgbtn_4_label = lv_label_create(ui->control_imgbtn_4);
	lv_label_set_text(ui->control_imgbtn_4_label, "");
	lv_label_set_long_mode(ui->control_imgbtn_4_label, LV_LABEL_LONG_WRAP);
	lv_obj_align(ui->control_imgbtn_4_label, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_pad_all(ui->control_imgbtn_4, 0, LV_STATE_DEFAULT);
	lv_obj_set_pos(ui->control_imgbtn_4, 115, 67);
	lv_obj_set_size(ui->control_imgbtn_4, 50, 50);

	//Write style for control_imgbtn_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_text_color(ui->control_imgbtn_4, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->control_imgbtn_4, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->control_imgbtn_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->control_imgbtn_4, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->control_imgbtn_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_clip_corner(ui->control_imgbtn_4, true, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->control_imgbtn_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for control_imgbtn_4, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
	lv_obj_set_style_img_recolor_opa(ui->control_imgbtn_4, 0, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_img_opa(ui->control_imgbtn_4, 255, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_text_color(ui->control_imgbtn_4, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_text_font(ui->control_imgbtn_4, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_text_opa(ui->control_imgbtn_4, 255, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_shadow_width(ui->control_imgbtn_4, 0, LV_PART_MAIN|LV_STATE_PRESSED);

	//Write style for control_imgbtn_4, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
	lv_obj_set_style_img_recolor_opa(ui->control_imgbtn_4, 0, LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_img_opa(ui->control_imgbtn_4, 255, LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_text_color(ui->control_imgbtn_4, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_text_font(ui->control_imgbtn_4, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_text_opa(ui->control_imgbtn_4, 255, LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_shadow_width(ui->control_imgbtn_4, 0, LV_PART_MAIN|LV_STATE_CHECKED);

	//Write style for control_imgbtn_4, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
	lv_obj_set_style_img_recolor_opa(ui->control_imgbtn_4, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
	lv_obj_set_style_img_opa(ui->control_imgbtn_4, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

	//Write codes control_imgbtn_5
	ui->control_imgbtn_5 = lv_imgbtn_create(ui->control);
	lv_obj_add_flag(ui->control_imgbtn_5, LV_OBJ_FLAG_CHECKABLE);
	lv_imgbtn_set_src(ui->control_imgbtn_5, LV_IMGBTN_STATE_RELEASED, NULL, &_fan_unselect_alpha_50x50, NULL);
	lv_imgbtn_set_src(ui->control_imgbtn_5, LV_IMGBTN_STATE_PRESSED, NULL, &_fan_unselect_alpha_50x50, NULL);
	lv_imgbtn_set_src(ui->control_imgbtn_5, LV_IMGBTN_STATE_CHECKED_RELEASED, NULL, &_fan_select_alpha_50x50, NULL);
	lv_imgbtn_set_src(ui->control_imgbtn_5, LV_IMGBTN_STATE_CHECKED_PRESSED, NULL, &_fan_select_alpha_50x50, NULL);
	ui->control_imgbtn_5_label = lv_label_create(ui->control_imgbtn_5);
	lv_label_set_text(ui->control_imgbtn_5_label, "");
	lv_label_set_long_mode(ui->control_imgbtn_5_label, LV_LABEL_LONG_WRAP);
	lv_obj_align(ui->control_imgbtn_5_label, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_pad_all(ui->control_imgbtn_5, 0, LV_STATE_DEFAULT);
	lv_obj_set_pos(ui->control_imgbtn_5, 190, 67);
	lv_obj_set_size(ui->control_imgbtn_5, 50, 50);

	//Write style for control_imgbtn_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_text_color(ui->control_imgbtn_5, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->control_imgbtn_5, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->control_imgbtn_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->control_imgbtn_5, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->control_imgbtn_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_clip_corner(ui->control_imgbtn_5, true, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->control_imgbtn_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for control_imgbtn_5, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
	lv_obj_set_style_img_recolor_opa(ui->control_imgbtn_5, 0, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_img_opa(ui->control_imgbtn_5, 255, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_text_color(ui->control_imgbtn_5, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_text_font(ui->control_imgbtn_5, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_text_opa(ui->control_imgbtn_5, 255, LV_PART_MAIN|LV_STATE_PRESSED);
	lv_obj_set_style_shadow_width(ui->control_imgbtn_5, 0, LV_PART_MAIN|LV_STATE_PRESSED);

	//Write style for control_imgbtn_5, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
	lv_obj_set_style_img_recolor_opa(ui->control_imgbtn_5, 0, LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_img_opa(ui->control_imgbtn_5, 255, LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_text_color(ui->control_imgbtn_5, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_text_font(ui->control_imgbtn_5, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_text_opa(ui->control_imgbtn_5, 255, LV_PART_MAIN|LV_STATE_CHECKED);
	lv_obj_set_style_shadow_width(ui->control_imgbtn_5, 0, LV_PART_MAIN|LV_STATE_CHECKED);

	//Write style for control_imgbtn_5, Part: LV_PART_MAIN, State: LV_IMGBTN_STATE_RELEASED.
	lv_obj_set_style_img_recolor_opa(ui->control_imgbtn_5, 0, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);
	lv_obj_set_style_img_opa(ui->control_imgbtn_5, 255, LV_PART_MAIN|LV_IMGBTN_STATE_RELEASED);

	//Write codes control_cont_1
	ui->control_cont_1 = lv_obj_create(ui->control);
	lv_obj_set_pos(ui->control_cont_1, 10, 190);
	lv_obj_set_size(ui->control_cont_1, 260, 40);
	lv_obj_set_scrollbar_mode(ui->control_cont_1, LV_SCROLLBAR_MODE_OFF);

	//Write style for control_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->control_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->control_cont_1, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->control_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->control_cont_1, lv_color_hex(0x393c41), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->control_cont_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->control_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->control_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->control_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->control_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->control_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes control_label_1
	ui->control_label_1 = lv_label_create(ui->control_cont_1);
	lv_label_set_text(ui->control_label_1, "照明：亮度80%");
	lv_label_set_long_mode(ui->control_label_1, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->control_label_1, 11, 7);
	lv_obj_set_size(ui->control_label_1, 195, 25);

	//Write style for control_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->control_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->control_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->control_label_1, lv_color_hex(0x199141), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->control_label_1, &lv_font_SourceHanSerifSC_Regular_24, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->control_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->control_label_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->control_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->control_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->control_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->control_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->control_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->control_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->control_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->control_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes control_sw_1
	ui->control_sw_1 = lv_switch_create(ui->control);
	lv_obj_set_pos(ui->control_sw_1, 23, 74);
	lv_obj_set_size(ui->control_sw_1, 68, 35);

	//Write style for control_sw_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->control_sw_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->control_sw_1, lv_color_hex(0xefefef), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->control_sw_1, LV_GRAD_DIR_HOR, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_color(ui->control_sw_1, lv_color_hex(0x5b4c4c), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_main_stop(ui->control_sw_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_stop(ui->control_sw_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->control_sw_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->control_sw_1, 35, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->control_sw_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for control_sw_1, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
	lv_obj_set_style_bg_opa(ui->control_sw_1, 206, LV_PART_INDICATOR|LV_STATE_CHECKED);
	lv_obj_set_style_bg_color(ui->control_sw_1, lv_color_hex(0x76ee59), LV_PART_INDICATOR|LV_STATE_CHECKED);
	lv_obj_set_style_bg_grad_dir(ui->control_sw_1, LV_GRAD_DIR_HOR, LV_PART_INDICATOR|LV_STATE_CHECKED);
	lv_obj_set_style_bg_grad_color(ui->control_sw_1, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_CHECKED);
	lv_obj_set_style_bg_main_stop(ui->control_sw_1, 0, LV_PART_INDICATOR|LV_STATE_CHECKED);
	lv_obj_set_style_bg_grad_stop(ui->control_sw_1, 255, LV_PART_INDICATOR|LV_STATE_CHECKED);
	lv_obj_set_style_border_width(ui->control_sw_1, 0, LV_PART_INDICATOR|LV_STATE_CHECKED);

	//Write style for control_sw_1, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->control_sw_1, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->control_sw_1, lv_color_hex(0xffffff), LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->control_sw_1, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->control_sw_1, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->control_sw_1, 35, LV_PART_KNOB|LV_STATE_DEFAULT);

	//The custom code of control.
	

	//Update current screen layout.
	lv_obj_update_layout(ui->control);

	//Init events for screen.
	events_init_control(ui);
}
