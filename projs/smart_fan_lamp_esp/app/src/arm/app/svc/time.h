/**
 * \file            rtc_time.h
 * \brief           rtc_time application file
 */

/*
 * Copyright (c) 2024 Jasper
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is the entire rtc_time application.
 *
 * Author:          Jasper <jasperzhangse@gmail.com>
 * Version:         v1.0.0-dev
 */

#ifndef __RTC_TIME_H__
#define __RTC_TIME_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "lib/type/lib_type.h"

typedef struct {
    uint32_t start_time;
    uint32_t duration;
    uint32_t remaining_time;
    uint8_t rm_hour;
    uint8_t rm_min;
    uint8_t rm_sec;
} time_hdl_t;

#define TIME_CTRL_INIT {.tick_count = 0, .year = 0, .mon = 0, .week = 0, .day = 0, .hour = 0, .min = 0, .sec = 0}

#define TIME_HDL_INIT  {.start_time = 0, .duration = 0, .remaining_time = 0, .rm_hour = 0, .rm_min = 0, .rm_sec = 0}

/* Functions */
status_t time_init(void);
status_t DrvTimeTerm(void);
status_t rtc_get_time(void);

status_t timing_start(time_hdl_t* time_hdl, uint8_t hour, uint8_t min, uint8_t sec);
status_t timing_update(time_hdl_t* time_hdl);
uint8_t is_timing_remaining(time_hdl_t* time_hdl);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __RTC_TIME_H__ */
