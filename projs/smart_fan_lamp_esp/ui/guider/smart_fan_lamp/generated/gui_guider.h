/*
* Copyright 2024 NXP
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

typedef struct
{
  
	lv_obj_t *home;
	bool home_del;
	lv_obj_t *home_label_12;
	lv_obj_t *home_digital_clock_1;
	lv_obj_t *home_animimg_1;
	lv_obj_t *home_label_2;
	lv_obj_t *home_cont_1;
	lv_obj_t *home_img_1;
	lv_obj_t *home_label_6;
	lv_obj_t *home_cont_3;
	lv_obj_t *home_cont_4;
	lv_obj_t *home_img_2;
	lv_obj_t *home_label_7;
	lv_obj_t *home_cont_5;
	lv_obj_t *home_label_8;
	lv_obj_t *home_label_10;
	lv_obj_t *home_label_11;
	lv_obj_t *home_win_bright_0;
	lv_obj_t *home_win_bright_0_item0;
	lv_obj_t *control;
	bool control_del;
	lv_obj_t *control_slider_2;
	lv_obj_t *control_img_1;
	lv_obj_t *control_imgbtn_1;
	lv_obj_t *control_imgbtn_1_label;
	lv_obj_t *control_imgbtn_2;
	lv_obj_t *control_imgbtn_2_label;
	lv_obj_t *control_imgbtn_3;
	lv_obj_t *control_imgbtn_3_label;
	lv_obj_t *control_imgbtn_4;
	lv_obj_t *control_imgbtn_4_label;
	lv_obj_t *control_imgbtn_5;
	lv_obj_t *control_imgbtn_5_label;
	lv_obj_t *control_cont_1;
	lv_obj_t *control_label_1;
	lv_obj_t *config;
	bool config_del;
	lv_obj_t *config_img_1;
	lv_obj_t *config_cont_1;
	lv_obj_t *config_cont_2;
	lv_obj_t *config_label_1;
	lv_obj_t *config_sw_1;
	lv_obj_t *config_cont_3;
	lv_obj_t *config_label_2;
	lv_obj_t *config_sw_2;
	lv_obj_t *config_cont_4;
	lv_obj_t *config_label_3;
	lv_obj_t *config_sw_3;
	lv_obj_t *config_cont_5;
	lv_obj_t *config_label_4;
	lv_obj_t *config_sw_4;
	lv_obj_t *clock;
	bool clock_del;
	lv_obj_t *clock_img_1;
}lv_ui;

typedef void (*ui_setup_scr_t)(lv_ui * ui);

void ui_init_style(lv_style_t * style);

void ui_load_scr_animation(lv_ui *ui, lv_obj_t ** new_scr, bool new_scr_del, bool * old_scr_del, ui_setup_scr_t setup_scr,
                           lv_scr_load_anim_t anim_type, uint32_t time, uint32_t delay, bool is_clean, bool auto_del);

void ui_animation(void * var, int32_t duration, int32_t delay, int32_t start_value, int32_t end_value, lv_anim_path_cb_t path_cb,
                       uint16_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                       lv_anim_exec_xcb_t exec_cb, lv_anim_start_cb_t start_cb, lv_anim_ready_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);


void init_scr_del_flag(lv_ui *ui);

void setup_ui(lv_ui *ui);


extern lv_ui guider_ui;


void setup_scr_home(lv_ui *ui);
void setup_scr_control(lv_ui *ui);
void setup_scr_config(lv_ui *ui);
void setup_scr_clock(lv_ui *ui);
#include "extra/widgets/animimg/lv_animimg.h"
LV_IMG_DECLARE(home_animimg_1cloudy);
LV_IMG_DECLARE(home_animimg_1sunny);
LV_IMG_DECLARE(home_animimg_1weather_13);
LV_IMG_DECLARE(home_animimg_1weather_12);
LV_IMG_DECLARE(home_animimg_1weather_18);
LV_IMG_DECLARE(home_animimg_1weather_2);
LV_IMG_DECLARE(home_animimg_1weather_22);
LV_IMG_DECLARE(home_animimg_1weather_24);
LV_IMG_DECLARE(home_animimg_1weather_3);
LV_IMG_DECLARE(home_animimg_1weather_4);
LV_IMG_DECLARE(home_animimg_1weather_7);
LV_IMG_DECLARE(_temperature_alpha_30x30);
LV_IMG_DECLARE(_Humidity_alpha_30x30);
LV_IMG_DECLARE(_home_alpha_40x40);
LV_IMG_DECLARE(_home_alpha_40x40);
LV_IMG_DECLARE(_home_alpha_40x40);

LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_24)
LV_FONT_DECLARE(lv_font_montserratMedium_16)
LV_FONT_DECLARE(lv_font_Alatsi_Regular_60)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_16)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_12)
LV_FONT_DECLARE(lv_font_montserratMedium_12)


#ifdef __cplusplus
}
#endif
#endif
