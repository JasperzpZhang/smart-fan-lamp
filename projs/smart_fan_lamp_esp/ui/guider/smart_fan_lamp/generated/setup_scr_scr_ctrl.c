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



void setup_scr_scr_ctrl(lv_ui *ui)
{
	//Write codes scr_ctrl
	ui->scr_ctrl = lv_obj_create(NULL);
	lv_obj_set_size(ui->scr_ctrl, 280, 240);
	lv_obj_set_scrollbar_mode(ui->scr_ctrl, LV_SCROLLBAR_MODE_OFF);

	//Write style for scr_ctrl, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->scr_ctrl, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->scr_ctrl, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->scr_ctrl, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes scr_ctrl_slider_1
	ui->scr_ctrl_slider_1 = lv_slider_create(ui->scr_ctrl);
	lv_slider_set_range(ui->scr_ctrl_slider_1, 0, 100);
	lv_slider_set_mode(ui->scr_ctrl_slider_1, LV_SLIDER_MODE_NORMAL);
	lv_slider_set_value(ui->scr_ctrl_slider_1, 50, LV_ANIM_OFF);
	lv_obj_set_pos(ui->scr_ctrl_slider_1, 19, 99);
	lv_obj_set_size(ui->scr_ctrl_slider_1, 241, 23);

	//Write style for scr_ctrl_slider_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->scr_ctrl_slider_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->scr_ctrl_slider_1, 50, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_outline_width(ui->scr_ctrl_slider_1, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_outline_opa(ui->scr_ctrl_slider_1, 78, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_outline_color(ui->scr_ctrl_slider_1, lv_color_hex(0xff6500), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->scr_ctrl_slider_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for scr_ctrl_slider_1, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->scr_ctrl_slider_1, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->scr_ctrl_slider_1, lv_color_hex(0xfffffe), LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->scr_ctrl_slider_1, LV_GRAD_DIR_HOR, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_color(ui->scr_ctrl_slider_1, lv_color_hex(0xff6500), LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_main_stop(ui->scr_ctrl_slider_1, 0, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_stop(ui->scr_ctrl_slider_1, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->scr_ctrl_slider_1, 50, LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//Write style for scr_ctrl_slider_1, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->scr_ctrl_slider_1, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->scr_ctrl_slider_1, 50, LV_PART_KNOB|LV_STATE_DEFAULT);

	//Write codes scr_ctrl_sw_1
	ui->scr_ctrl_sw_1 = lv_switch_create(ui->scr_ctrl);
	lv_obj_set_pos(ui->scr_ctrl_sw_1, 161, 171);
	lv_obj_set_size(ui->scr_ctrl_sw_1, 84, 38);

	//Write style for scr_ctrl_sw_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->scr_ctrl_sw_1, 16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->scr_ctrl_sw_1, lv_color_hex(0x00ffc4), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->scr_ctrl_sw_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->scr_ctrl_sw_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->scr_ctrl_sw_1, 153, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->scr_ctrl_sw_1, lv_color_hex(0x00bdff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->scr_ctrl_sw_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->scr_ctrl_sw_1, 38, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->scr_ctrl_sw_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for scr_ctrl_sw_1, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
	lv_obj_set_style_bg_opa(ui->scr_ctrl_sw_1, 199, LV_PART_INDICATOR|LV_STATE_CHECKED);
	lv_obj_set_style_bg_color(ui->scr_ctrl_sw_1, lv_color_hex(0x5cf4ff), LV_PART_INDICATOR|LV_STATE_CHECKED);
	lv_obj_set_style_bg_grad_dir(ui->scr_ctrl_sw_1, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_CHECKED);
	lv_obj_set_style_border_width(ui->scr_ctrl_sw_1, 0, LV_PART_INDICATOR|LV_STATE_CHECKED);

	//Write style for scr_ctrl_sw_1, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->scr_ctrl_sw_1, 68, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->scr_ctrl_sw_1, lv_color_hex(0x001dff), LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->scr_ctrl_sw_1, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->scr_ctrl_sw_1, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->scr_ctrl_sw_1, 27, LV_PART_KNOB|LV_STATE_DEFAULT);

	//Write codes scr_ctrl_btn_1
	ui->scr_ctrl_btn_1 = lv_btn_create(ui->scr_ctrl);
	ui->scr_ctrl_btn_1_label = lv_label_create(ui->scr_ctrl_btn_1);
	lv_label_set_text(ui->scr_ctrl_btn_1_label, "Button");
	lv_label_set_long_mode(ui->scr_ctrl_btn_1_label, LV_LABEL_LONG_WRAP);
	lv_obj_align(ui->scr_ctrl_btn_1_label, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_pad_all(ui->scr_ctrl_btn_1, 0, LV_STATE_DEFAULT);
	lv_obj_set_width(ui->scr_ctrl_btn_1_label, LV_PCT(100));
	lv_obj_set_pos(ui->scr_ctrl_btn_1, 31, 165);
	lv_obj_set_size(ui->scr_ctrl_btn_1, 96, 43);

	//Write style for scr_ctrl_btn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->scr_ctrl_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->scr_ctrl_btn_1, lv_color_hex(0x009ea9), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->scr_ctrl_btn_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->scr_ctrl_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->scr_ctrl_btn_1, 25, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->scr_ctrl_btn_1, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_color(ui->scr_ctrl_btn_1, lv_color_hex(0x0d4b3b), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_opa(ui->scr_ctrl_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_spread(ui->scr_ctrl_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_ofs_x(ui->scr_ctrl_btn_1, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_ofs_y(ui->scr_ctrl_btn_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->scr_ctrl_btn_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->scr_ctrl_btn_1, &lv_font_montserratMedium_18, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->scr_ctrl_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->scr_ctrl_btn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes scr_ctrl_btn_2
	ui->scr_ctrl_btn_2 = lv_btn_create(ui->scr_ctrl);
	ui->scr_ctrl_btn_2_label = lv_label_create(ui->scr_ctrl_btn_2);
	lv_label_set_text(ui->scr_ctrl_btn_2_label, "Home");
	lv_label_set_long_mode(ui->scr_ctrl_btn_2_label, LV_LABEL_LONG_WRAP);
	lv_obj_align(ui->scr_ctrl_btn_2_label, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_pad_all(ui->scr_ctrl_btn_2, 0, LV_STATE_DEFAULT);
	lv_obj_set_width(ui->scr_ctrl_btn_2_label, LV_PCT(100));
	lv_obj_set_pos(ui->scr_ctrl_btn_2, 18, 12);
	lv_obj_set_size(ui->scr_ctrl_btn_2, 79, 50);

	//Write style for scr_ctrl_btn_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->scr_ctrl_btn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->scr_ctrl_btn_2, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->scr_ctrl_btn_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->scr_ctrl_btn_2, lv_color_hex(0x5482a9), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->scr_ctrl_btn_2, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->scr_ctrl_btn_2, 25, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->scr_ctrl_btn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->scr_ctrl_btn_2, lv_color_hex(0x12548b), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->scr_ctrl_btn_2, &lv_font_montserratMedium_18, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->scr_ctrl_btn_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->scr_ctrl_btn_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

	//The custom code of scr_ctrl.
	

	//Update current screen layout.
	lv_obj_update_layout(ui->scr_ctrl);

	//Init events for screen.
	events_init_scr_ctrl(ui);
}
