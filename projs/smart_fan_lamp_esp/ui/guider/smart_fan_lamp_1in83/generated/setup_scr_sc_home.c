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



int sc_home_digital_clock_1_min_value = 28;
int sc_home_digital_clock_1_hour_value = 14;
int sc_home_digital_clock_1_sec_value = 52;
void setup_scr_sc_home(lv_ui *ui)
{
	//Write codes sc_home
	ui->sc_home = lv_obj_create(NULL);
	lv_obj_set_size(ui->sc_home, 280, 240);
	lv_obj_set_scrollbar_mode(ui->sc_home, LV_SCROLLBAR_MODE_OFF);

	//Write style for sc_home, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->sc_home, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->sc_home, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->sc_home, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes sc_home_img_1
	ui->sc_home_img_1 = lv_img_create(ui->sc_home);
	lv_obj_add_flag(ui->sc_home_img_1, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->sc_home_img_1, &_cloudy_alpha_50x50);
	lv_img_set_pivot(ui->sc_home_img_1, 50,50);
	lv_img_set_angle(ui->sc_home_img_1, 0);
	lv_obj_set_pos(ui->sc_home_img_1, 195, 161);
	lv_obj_set_size(ui->sc_home_img_1, 50, 50);

	//Write style for sc_home_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_recolor_opa(ui->sc_home_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_img_opa(ui->sc_home_img_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->sc_home_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_clip_corner(ui->sc_home_img_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes sc_home_digital_clock_1
	static bool sc_home_digital_clock_1_timer_enabled = false;
	ui->sc_home_digital_clock_1 = lv_dclock_create(ui->sc_home, "14:28");
	if (!sc_home_digital_clock_1_timer_enabled) {
		lv_timer_create(sc_home_digital_clock_1_timer, 1000, NULL);
		sc_home_digital_clock_1_timer_enabled = true;
	}
	lv_obj_set_pos(ui->sc_home_digital_clock_1, 51, 20);
	lv_obj_set_size(ui->sc_home_digital_clock_1, 168, 72);
	lv_obj_add_flag(ui->sc_home_digital_clock_1, LV_OBJ_FLAG_CLICKABLE);

	//Write style for sc_home_digital_clock_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_radius(ui->sc_home_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->sc_home_digital_clock_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->sc_home_digital_clock_1, &lv_font_Alatsi_Regular_61, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->sc_home_digital_clock_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->sc_home_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->sc_home_digital_clock_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->sc_home_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->sc_home_digital_clock_1, 7, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->sc_home_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->sc_home_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->sc_home_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->sc_home_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes sc_home_datetext_1
	ui->sc_home_datetext_1 = lv_label_create(ui->sc_home);
	lv_label_set_text(ui->sc_home_datetext_1, "2023/10/08");
	lv_obj_set_style_text_align(ui->sc_home_datetext_1, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_add_flag(ui->sc_home_datetext_1, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_add_event_cb(ui->sc_home_datetext_1, sc_home_datetext_1_event_handler, LV_EVENT_ALL, NULL);
	lv_obj_set_pos(ui->sc_home_datetext_1, 84, 101);
	lv_obj_set_size(ui->sc_home_datetext_1, 107, 28);

	//Write style for sc_home_datetext_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_radius(ui->sc_home_datetext_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->sc_home_datetext_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->sc_home_datetext_1, &lv_font_Alatsi_Regular_14, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->sc_home_datetext_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->sc_home_datetext_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->sc_home_datetext_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->sc_home_datetext_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->sc_home_datetext_1, 7, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->sc_home_datetext_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->sc_home_datetext_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->sc_home_datetext_1, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_color(ui->sc_home_datetext_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_opa(ui->sc_home_datetext_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_spread(ui->sc_home_datetext_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_ofs_x(ui->sc_home_datetext_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_ofs_y(ui->sc_home_datetext_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->sc_home_datetext_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes sc_home_btn_1
	ui->sc_home_btn_1 = lv_btn_create(ui->sc_home);
	ui->sc_home_btn_1_label = lv_label_create(ui->sc_home_btn_1);
	lv_label_set_text(ui->sc_home_btn_1_label, "ctrl");
	lv_label_set_long_mode(ui->sc_home_btn_1_label, LV_LABEL_LONG_WRAP);
	lv_obj_align(ui->sc_home_btn_1_label, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_style_pad_all(ui->sc_home_btn_1, 0, LV_STATE_DEFAULT);
	lv_obj_set_width(ui->sc_home_btn_1_label, LV_PCT(100));
	lv_obj_set_pos(ui->sc_home_btn_1, 32, 158);
	lv_obj_set_size(ui->sc_home_btn_1, 116, 59);

	//Write style for sc_home_btn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_radius(ui->sc_home_btn_1, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->sc_home_btn_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->sc_home_btn_1, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->sc_home_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->sc_home_btn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->sc_home_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->sc_home_btn_1, lv_color_hex(0x63aaa3), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->sc_home_btn_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->sc_home_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->sc_home_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//The custom code of sc_home.
	

	//Update current screen layout.
	lv_obj_update_layout(ui->sc_home);

	//Init events for screen.
	events_init_sc_home(ui);
}
