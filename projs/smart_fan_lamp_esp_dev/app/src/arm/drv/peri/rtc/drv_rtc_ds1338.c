/**
 * \file            drv_rtc_ds1338.c
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

/* Includes */
#include "drv/peri/rtc/drv_rtc_ds1338.h"
#include "app/include.h"
#include "lib/debug/lib_debug.h"
#include "lib/i2c/lib_i2c.h"
#include <cmsis_os2.h>

#if (RTC_ENABLE && RTC_DS1338_ENABLE)

/* Pragmas */
#pragma diag_suppress 177 /* warning: #177-D: function "FUNC" was set but never used */
#pragma diag_suppress 550 /* warning: #550-D: variable was set but never used */

/* Debug config */
#if RTC_DEBUG
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* RTC_DEBUG */
#if RTC_ASSERT
#undef ASSERT
#define ASSERT(a)                                                                                                      \
    while (!(a)) {                                                                                                     \
        DebugPrintf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                     \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* RTC_ASSERT */

/* Local defines */
/* Register definition */
#define DS1338_REG_SECONDS   (0x00)
#define DS1338_REG_MINUTES   (0x01)
#define DS1338_REG_HOURS     (0x02)
#define DS1338_REG_DAY       (0x03)
#define DS1338_REG_DATE      (0x04)
#define DS1338_REG_MONTH     (0x05)
#define DS1338_REG_YEAR      (0x06)
#define DS1338_REG_CONTROL   (0x07)
#define DS1338_REG_RAM_BEGIN (0x08)
#define DS1338_REG_RAM_END   (0x3F)

/* Local types */
typedef struct {
    uint8_t ucYear; /* Bcd: 0~99 */
    uint8_t ucMon;  /* Bcd: 1~12 */
    uint8_t ucDay;  /* Bcd: 1~31 */
    uint8_t ucWeek; /* Bcd: 1~7 */
    uint8_t ucHour; /* Bcd: 1~12 / 0~23 */
    uint8_t ucMin;  /* Bcd: 0~59 */
    uint8_t ucSec;  /* Bcd: 0~59 */
} DsTime_t;

/* Forward declaration */
static uint8_t prvDs1338ReadTime(DsTime_t* time);
static uint8_t prvDs1338WriteTime(DsTime_t* time);
static Time_t prvDs1338ToStdCTime(DsTime_t* pxDsTm);
static DsTime_t prvStdCToDs1338Time(Time_t* pxTm);
static uint8_t prvBcdToDec(uint8_t ucBcd);
static uint8_t prvDecToBcd(uint8_t ucDec);
static status_t prvPrintDs1338Time(DsTime_t* pxDsTm);
static status_t prvPrintStdCTime(Time_t* pxTm);
static status_t prvPrintBcdCode(uint8_t* Buf, uint8_t Size);

/* Local variables */
static Bool_t s_bInit = FALSE;
static Bool_t s_bConfig = FALSE;
#if RTC_RTOS
static osMutexId s_xMutex;
#endif /* RTC_RTOS */

/* Local defines */
#if RTC_RTOS
#define DS1338_MUTEX_INIT()                                                                                            \
    do {                                                                                                               \
        s_xMutex = osMutexNew(NULL);                                                                                   \
        if (s_xMutex == NULL) {                                                                                        \
            /* Handle error */                                                                                         \
        }                                                                                                              \
    } while (0)
#define DS1338_LOCK()   osMutexAcquire(s_xMutex, osWaitForever)
#define DS1338_UNLOCK() osMutexRelease(s_xMutex)
#else
#define DS1338_MUTEX_INIT()
#define DS1338_LOCK()
#define DS1338_UNLOCK()
#endif /* RTC_RTOS */
#define DS1338_RAM_SIZE (31)

/* Functions */
status_t RtcDs1338Init(void) {
    if (s_bInit == FALSE) {
        DS1338_MUTEX_INIT();
#if RTC_RTOSS
        if (NULL == s_xMutex) {
            TRACE("RtcDs1302Init create mutex failed\n");
            return status_err;
        }
#endif /* RTC_RTOS */
        s_bInit = TRUE;
        return status_ok;
    } else {
        return status_err;
    }
}

Time_t RtcDs1338ReadTime(void) {
    DsTime_t xDsTm;
    ASSERT((TRUE == s_bInit) && (TRUE == s_bConfig));
    DS1338_LOCK();
    prvDs1338ReadTime(&xDsTm);
    DS1338_UNLOCK();
    return prvDs1338ToStdCTime(&xDsTm);
}

status_t RtcDs1338WriteTime(Time_t xTm) {
    DsTime_t xDsTm;
    ASSERT((TRUE == s_bInit) && (TRUE == s_bConfig));
    DS1338_LOCK();
    xDsTm = prvStdCToDs1338Time(&xTm);
    prvDs1338WriteTime(&xDsTm);
    DS1338_UNLOCK();
    return status_ok;
}

#if (RTC_STDC_TIME == 3)
time_t time(time_t* timer) {
    Time_t tm;
    time_t t;
    tm = RtcDs1338ReadTime();
    t = mktime(&tm);
    if (NULL == timer) {
        return t;
    } else {
        *timer = t;
        return t;
    }
}
#endif /* (RTC_STDC_TIME == 3) */

static uint8_t prvDs1338ReadTime(DsTime_t* time) {
    uint8_t buf[7];
#if 1

    //      i2c_read_data(DS1338_ADDR, buf, 7, 0);

    i2c_read_register(DS1338_ADDR, DS1338_REG_SECONDS, buf, 7);

#else
    i2c_read_data(DS1338_ADDR, DS1338_REG_SECONDS, buf, 7);
#endif

#if RTC_DEBUG
    prvPrintBcdCode(buf, 7);
#endif

    time->ucSec = prvBcdToDec(buf[0]);
    time->ucMin = prvBcdToDec(buf[1]);
    if (buf[2] & DS1338_HOUR_12) {
        time->ucHour = ((buf[2] >> 4) & 0x01) * 12 + ((buf[2] >> 5) & 0x01) * 12;
    } else {
        time->ucHour = prvBcdToDec(buf[2]);
    }
    time->ucDay = prvBcdToDec(buf[4] & 0x3F);
    time->ucMon = prvBcdToDec(buf[5] & 0x1F);
    time->ucYear = prvBcdToDec(buf[6]);

#if RTC_DEBUG
    prvPrintDs1338Time(time);
#endif
    return 0;
}

static uint8_t prvDs1338WriteTime(DsTime_t* time) {
    uint8_t buf[7];
    buf[0] = prvDecToBcd(time->ucSec);
    buf[1] = prvDecToBcd(time->ucMin);
    buf[2] = prvDecToBcd(time->ucHour); // Time always stored in 24-hour format
    buf[3] = 1;                         // Not used
    buf[4] = prvDecToBcd(time->ucDay);
    buf[5] = prvDecToBcd(time->ucMon);
    buf[6] = prvDecToBcd(time->ucYear);

#if RTC_DEBUG
    prvPrintBcdCode(buf, 7);
#endif
    //    i2c_write_data(DS1338_ADDR, buf, 7);
    i2c_write_register(DS1338_ADDR, DS1338_REG_SECONDS, buf, 7);

    return 0;
}

static Time_t prvDs1338ToStdCTime(DsTime_t* pxDsTm) {
    Time_t cTm;
    cTm.tm_sec = (pxDsTm->ucSec); /* seconds after the minute, 0 to 60 (0 - 60 allows for the occasional leap second) */
    cTm.tm_min = (pxDsTm->ucMin); /* minutes after the hour, 0 to 59 */
    cTm.tm_hour = (pxDsTm->ucHour);       /* hours since midnight, 0 to 23 */
    cTm.tm_mday = (pxDsTm->ucDay);        /* day of the month, 1 to 31 */
    cTm.tm_mon = (pxDsTm->ucMon) - 1;     /* months since January, 0 to 11 */
    cTm.tm_year = (pxDsTm->ucYear) + 100; /* years since 1900 */
    cTm.tm_wday = (pxDsTm->ucWeek) - 1;   /* days since Sunday, 0 to 6 */
    cTm.tm_yday = (0);                    /* days since January 1, 0 to 365 */
    cTm.tm_isdst = (0);                   /* Daylight Savings Time flag */

    return cTm;
}

static DsTime_t prvStdCToDs1338Time(Time_t* pxTm) {
    DsTime_t xDsTm;
    xDsTm.ucYear = (pxTm->tm_year - 100); /* 0~99 */
    xDsTm.ucMon = (pxTm->tm_mon + 1);     /* 1~12 */
    xDsTm.ucDay = (pxTm->tm_mday);        /* 1~31 */
    xDsTm.ucWeek = (pxTm->tm_wday + 1);   /* 1~7 */
    xDsTm.ucHour = (pxTm->tm_hour);       /* 1~12 / 0~23 */
    xDsTm.ucMin = (pxTm->tm_min);         /* 0~59 */
    xDsTm.ucSec = (pxTm->tm_sec);         /* 0~59 */

    return xDsTm;
}

static uint8_t prvBcdToDec(uint8_t ucBcd) { return ((ucBcd >> 4) * 10 + (ucBcd & 0x0F)); }

static uint8_t prvDecToBcd(uint8_t ucDec) { return ((((ucDec / 10) & 0x0F) << 4) + (ucDec % 10)); }

static status_t prvPrintDs1338Time(DsTime_t* pxDsTm) {
    TRACE("prvPrintDs1302Time:\n");
    TRACE("    ucYear:   %02X\n", pxDsTm->ucYear);
    TRACE("    ucMon:    %02X\n", pxDsTm->ucMon);
    TRACE("    ucDay:    %02X\n", pxDsTm->ucDay);
    TRACE("    ucWeek:   %02X\n", pxDsTm->ucWeek);
    TRACE("    ucHour:   %02X\n", pxDsTm->ucHour);
    TRACE("    ucMin:    %02X\n", pxDsTm->ucMin);
    TRACE("    ucSec:    %02X\n", pxDsTm->ucSec);
    return status_ok;
}

static status_t prvPrintStdCTime(Time_t* pxTm) {
    TRACE("prvPrintStdCTime:\n");
    TRACE("    tm_year:   %d\n", pxTm->tm_year);
    TRACE("    tm_mon:    %d\n", pxTm->tm_mon);
    TRACE("    tm_mday:   %d\n", pxTm->tm_mday);
    TRACE("    tm_wday:   %d\n", pxTm->tm_wday);
    TRACE("    tm_yday:   %d\n", pxTm->tm_yday);
    TRACE("    tm_hour:   %d\n", pxTm->tm_hour);
    TRACE("    tm_min:    %d\n", pxTm->tm_min);
    TRACE("    tm_sec:    %d\n", pxTm->tm_sec);
    TRACE("    tm_isdst:  %d\n", pxTm->tm_isdst);
    return status_ok;
}

static status_t prvPrintBcdCode(uint8_t* Buf, uint8_t Size) {
    uint8_t i, j, byte = 0;

    for (i = 0; i < Size; i++) {
        byte = Buf[i];
        for (j = 0; j < 8; j++) {
            TRACE("%d ", ((byte & 0x80) ? 1 : 0));
            byte <<= 1; // Shift byte to the left
        }
        TRACE("\n");
    }
    return status_ok;
}

#if 0
static void prv_cli_cmd_rtc_test(cli_printf cliprintf, int argc, char** argv)
{
    CHECK_CLI();
    
    uint8_t buf[7] = {0};
    uint8_t data = atoi(argv[1]);
    i2c_write_register(DS1338_ADDR, DS1338_REG_SECONDS, &data, 1);
    i2c_read_register(DS1338_ADDR, DS1338_REG_SECONDS, buf, 7);
//    i2c_write_data(DS1338_ADDR, &buf, 1);
//    i2c_read_data(DS1338_ADDR, &buf, 1, 0);
    
    TRACE("buf = %d\r\n", buf[0]);
}
CLI_CMD_EXPORT(rtc_test, rtc test, prv_cli_cmd_rtc_test)

#endif

#endif /* RTC_ENABLE && RTC_DS1338_ENABLE */
