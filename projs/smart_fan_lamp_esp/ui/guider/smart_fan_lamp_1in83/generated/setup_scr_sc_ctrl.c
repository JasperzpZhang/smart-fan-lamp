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



void setup_scr_sc_ctrl(lv_ui *ui)
{
	//Write codes sc_ctrl
	ui->sc_ctrl = lv_obj_create(NULL);
	lv_obj_set_size(ui->sc_ctrl, 280, 240);
	lv_obj_set_scrollbar_mode(ui->sc_ctrl, LV_SCROLLBAR_MODE_OFF);

	//Write style for sc_ctrl, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->sc_ctrl, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->sc_ctrl, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->sc_ctrl, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes sc_ctrl_slider_1
	ui->sc_ctrl_slider_1 = lv_slider_create(ui->sc_ctrl);
	lv_slider_set_range(ui->sc_ctrl_slider_1, 0, 100);
	lv_slider_set_mode(ui->sc_ctrl_slider_1, LV_SLIDER_MODE_NORMAL);
	lv_slider_set_value(ui->sc_ctrl_slider_1, 50, LV_ANIM_OFF);
	lv_obj_set_pos(ui->sc_ctrl_slider_1, 15, 188);
	lv_obj_set_size(ui->sc_ctrl_slider_1, 239, 29);

	//Write style for sc_ctrl_slider_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->sc_ctrl_slider_1, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->sc_ctrl_slider_1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->sc_ctrl_slider_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->sc_ctrl_slider_1, 50, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_outline_width(ui->sc_ctrl_slider_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->sc_ctrl_slider_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for sc_ctrl_slider_1, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->sc_ctrl_slider_1, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->sc_ctrl_slider_1, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->sc_ctrl_slider_1, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->sc_ctrl_slider_1, 50, LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//Write style for sc_ctrl_slider_1, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->sc_ctrl_slider_1, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->sc_ctrl_slider_1, 50, LV_PART_KNOB|LV_STATE_DEFAULT);

	//Write codes sc_ctrl_btn_2
	ui->sc_ctrl_btn_2 = lv_btn_create(ui->sc_ctrl);
	ui->sc_ctrl_btn_2_label = lv_label_create(ui->sc_ctrl_btn_2);
	lv_label_set_text(ui->sc_ctrl_btn_2_label, "home");
	lv_label_set_long_mode(ui->sc_ctrl_btn_2_label, LV_LABEL_LONG_WRAP);
	lv_obj_align(ui->sc_ctrl_btn_2_label, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_pad_all(ui->sc_ctrl_btn_2, 0, LV_STATE_DEFAULT);
	lv_obj_set_width(ui->sc_ctrl_btn_2_label, LV_PCT(100));
	lv_obj_set_pos(ui->sc_ctrl_btn_2, 24, 21);
	lv_obj_set_size(ui->sc_ctrl_btn_2, 75, 26);

	//Write style for sc_ctrl_btn_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->sc_ctrl_btn_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->sc_ctrl_btn_2, lv_color_hex(0x63aaa3), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->sc_ctrl_btn_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->sc_ctrl_btn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->sc_ctrl_btn_2, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->sc_ctrl_btn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->sc_ctrl_btn_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->sc_ctrl_btn_2, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->sc_ctrl_btn_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->sc_ctrl_btn_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

	//The custom code of sc_ctrl.
	

	//Update current screen layout.
	lv_obj_update_layout(ui->sc_ctrl);

	//Init events for screen.
	events_init_sc_ctrl(ui);
}
