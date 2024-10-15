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



void setup_scr_clock(lv_ui *ui)
{
	//Write codes clock
	ui->clock = lv_obj_create(NULL);
	lv_obj_set_size(ui->clock, 280, 240);
	lv_obj_set_scrollbar_mode(ui->clock, LV_SCROLLBAR_MODE_OFF);

	//Write style for clock, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->clock, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->clock, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->clock, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes clock_img_1
	ui->clock_img_1 = lv_img_create(ui->clock);
	lv_obj_add_flag(ui->clock_img_1, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->clock_img_1, &_home_alpha_40x40);
	lv_img_set_pivot(ui->clock_img_1, 50,50);
	lv_img_set_angle(ui->clock_img_1, 0);
	lv_obj_set_pos(ui->clock_img_1, 20, 18);
	lv_obj_set_size(ui->clock_img_1, 40, 40);

	//Write style for clock_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_recolor_opa(ui->clock_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_img_opa(ui->clock_img_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->clock_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_clip_corner(ui->clock_img_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

	//The custom code of clock.
	

	//Update current screen layout.
	lv_obj_update_layout(ui->clock);

	//Init events for screen.
	events_init_clock(ui);
}
