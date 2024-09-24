/**
 * \file            drv_rtc.h
 * \brief           Rtc driver file
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
 * This file is part of the rtc driver.
 *
 * Author:          Jasper <jasperzhangse@gmail.com>
 * Version:         v1.0.0-dev
 * Last edit:       2024-06-18
 */

#ifndef __DRV_RTC_H__
#define __DRV_RTC_H__

#ifdef __cplusplus
extern "C"
{
#endif /*__cplusplus */

/* Includes */
#include <stdint.h>
#include "lib/type/lib_type.h"
#include "drv/peri/rtc/drv_rtc_config.h"
#include "drv/peri/rtc/drv_rtc_ds1338.h"

    /* Types */
    typedef enum
    {
        RTC_TYPE_STM32,
        RTC_TYPE_DS1302,
        RTC_TYPE_DS1338,
        RTC_TYPE_SIZE
    } RtcType_t;

    /* Functions */
    status_t RtcInit(void);
    status_t RtcTerm(void);
    status_t RtcConfig(RtcType_t xRtcType, const void *pvConfig, uint16_t usConfigSize);

    Time_t RtcReadTime(RtcType_t xRtcType);
    status_t RtcWriteTime(RtcType_t xRtcType, Time_t xTm);

#if RTC_TEST
    status_t RtcTest(uint8_t ucSel);
#endif /* RTC_TEST */

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __DRV_RTC_H__ */
