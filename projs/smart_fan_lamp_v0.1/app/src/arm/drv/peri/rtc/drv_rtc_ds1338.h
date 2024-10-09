/**
 * \file            drv_rtc_ds1338.h
 * \brief           Ds1338 driver file
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
 * This file is part of the ds1338 driver.
 *
 * Author:          Jasper <jasperzhangse@gmail.com>
 * Version:         v1.0.0-dev
 * Last edit:       2024-06-18
 */

#ifndef __RTC_DS1338_H__
#define __RTC_DS1338_H__

#ifdef __cplusplus
extern "C"
{
#endif /*__cplusplus */

/* Includes */
#include <stdint.h>
#include <main.h>
#include "lib/type/lib_type.h"
#include "drv/peri/rtc/drv_rtc_config.h"

/* Device address */
#define DS1338_ADDR 0x68

/**
If set, in an hour register (DS1338_REG_HOURS, DS1338_REG_A1_HOUR,
DS1338_REG_A2_HOUR, the hour is between 0 and 12, and the
(!AM)/PM bit indicates AM or PM.

If not set, the hour is between 0 and 23.
*/
#define DS1338_HOUR_12 (0x01 << 6)

/**
If DS1338_HOUR_12 is set:
- If set, indicates PM
- If not set, indicates AM
*/
#define DS1338_PM_MASK (0x01 << 5)

// If set, the oscillator has stopped since the last time
// this bit was cleared.
#define DS1338_OSC_STOP_FLAG (0x01 << 5)

// Set to disable the oscillator
#define DS1338_OSC_DISABLE (0x01 << 7)

/**
These options control the behavior of the SQW/(!INTB) pin.
*/
#define DS1338_SQWE_FLAG (0x01 << 4)
#define DS1338_SQW_MASK (0x03)
#define DS1338_SQW_32768HZ (0x03)
#define DS1338_SQW_8192HZ (0x02)
#define DS1338_SQW_4096HZ (0x01)
#define DS1338_SQW_1HZ (0x00)

// Occurs when the number of I2C bytes available is less than the number requested.
#define READ_ERROR 5

#define decode_bcd(x) ((x >> 4) * 10 + (x & 0x0F))
#define encode_bcd(x) ((((x / 10) & 0x0F) << 4) + (x % 10))

    /* Functions */
    Time_t RtcDs1338ReadTime(void);
    status_t RtcDs1338WriteTime(Time_t xTm);
    status_t RtcDs1338Init(void);

#if (RTC_STDC_TIME == 2)
    time_t time(time_t *timer);
#endif /* (RTC_STDC_TIME == 2) */

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __RTC_DS1338_H__ */
