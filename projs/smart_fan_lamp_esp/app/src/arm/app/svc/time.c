/**
 * \file            rtc_time.c
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

/* Includes */
#include "app/include.h"

/* Debug config */
#if TIME_DEBUG || 1
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* TIME_DEBUG */
#if TIME_ASSERT
#undef ASSERT
#define ASSERT(a)                                                                                                      \
    while (!(a)) {                                                                                                     \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                    \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* TIME_ASSERT */

/* Local defines */
#if TIME_RTOS || 1
static osMutexId time_mutex;

#define TIME_MUTEX_INIT()                                                                                              \
    do {                                                                                                               \
        time_mutex = osMutexNew(NULL);                                                                                 \
        if (time_mutex == NULL) {                                                                                      \
            /* Handle error */                                                                                         \
        }                                                                                                              \
    } while (0)
#define TIME_LOCK()   osMutexAcquire(time_mutex, osWaitForever)
#define TIME_UNLOCK() osMutexRelease(time_mutex)
#else
#define TIME_MUTEX_INIT()
#define TIME_LOCK()
#define TIME_UNLOCK()
#endif /* RTC_RTOS */

static uint32_t tick_count = 0;

QueueHandle_t g_queue_time;
time_ctrl_t g_time_ctrl;

void time_task(void* para);
time_ctrl_t calculate_time_from_seconds(uint32_t total_seconds);
uint32_t calculate_time_to_timestamp(time_ctrl_t* time_ctrl);

/* Functions */
status_t
time_init(void) {
#if RTC_DS1302_ENABLE
    Ds1302Config_t xConfig;

    xConfig.xRst.pxPort = PORT(DS1302_RST);
    xConfig.xRst.usPin = PIN(DS1302_RST);
    xConfig.xClk.pxPort = PORT(DS1302_CLK);
    xConfig.xClk.usPin = PIN(DS1302_CLK);
    xConfig.xIo.pxPort = PORT(DS1302_IO);
    xConfig.xIo.usPin = PIN(DS1302_IO);

    RtcDs1302Init();
    RtcDs1302Config(&xConfig);
#endif
#if RTC_DS1338_ENABLE
    TIME_MUTEX_INIT();
    RtcDs1338Init();
#endif

#if TIMER_TIME_ENABLE
    // 在初始化中启动 TIM5
    TIME_MUTEX_INIT();
    g_queue_time = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(time_task, "time task", 128, NULL, tskIDLE_PRIORITY + 2, NULL);
    HAL_TIM_Base_Start_IT(&htim5); // 启动 TIM5 并使能中断

#endif

    return status_ok;
}

status_t
DrvTimeTerm(void) {
    /* Do nothing */
    return status_ok;
}

status_t
rtc_get_time(void) {
    Time_t xTm = RtcReadTime(RTC_TYPE_DS1338);
    TRACE("RTC time: %d, ", mktime(&xTm));
    TRACE("%s\r\n", asctime(&xTm));
    return status_ok;
}

uint32_t
rtc_get_timestamp(void) {
    Time_t tm_ctrl = RtcReadTime(RTC_TYPE_DS1338);
    return mktime(&tm_ctrl);
}

uint32_t
time_get_current_timestamp(time_ctrl_t* time_ctrl) {
    uint32_t timestamp = rtc_get_timestamp();

    return timestamp;
}

void
time_task(void* para) {

    static uint32_t msg_tick_count = 0;

    while (1) {
        if (xQueueReceive(g_queue_time, &msg_tick_count, portMAX_DELAY) == pdPASS) {}

        TRACE("time_count : %d s\n", msg_tick_count);

        // 调用函数计算并更新时间
        g_time_ctrl = calculate_time_from_seconds(msg_tick_count);

        // 打印时间信息
        TRACE("Time: %02d days, %02d:%02d:%02d\n", g_time_ctrl.day, g_time_ctrl.hour, g_time_ctrl.minute,
              g_time_ctrl.second);

        TRACE("\n\n");
    }
}

time_ctrl_t
calculate_time_from_seconds(uint32_t total_seconds) {

    time_ctrl_t time_ctrl;

    const uint32_t seconds_per_minute = 60;
    const uint32_t seconds_per_hour = 60 * seconds_per_minute;
    const uint32_t seconds_per_day = 24 * seconds_per_hour;

    TIME_LOCK();

    time_ctrl.tick_count = total_seconds;

    // 计算秒、分钟、小时、天
    time_ctrl.second = total_seconds % seconds_per_minute;
    uint32_t total_minutes = total_seconds / seconds_per_minute;
    time_ctrl.minute = total_minutes % 60;
    uint32_t total_hours = total_minutes / 60;
    time_ctrl.hour = total_hours % 24;
    time_ctrl.day = total_hours / 24; // 计算经过了多少天

    TIME_UNLOCK();

    return time_ctrl;
}

uint32_t
time_to_seconds(uint16_t hour, uint16_t minute, uint16_t second) {
    const uint32_t seconds_per_minute = 60;
    const uint32_t seconds_per_hour = 60 * seconds_per_minute;
    // 将小时、分钟、秒转换为总秒数
    uint32_t total_seconds = (hour * seconds_per_hour) + (minute * seconds_per_minute) + second;
    return total_seconds;
}

status_t
read_tick_time(time_ctrl_t* time_ctrl) {
    TIME_LOCK();
    time_ctrl->tick_count = g_time_ctrl.tick_count;
    time_ctrl->year = g_time_ctrl.year;
    time_ctrl->month = g_time_ctrl.month;
    time_ctrl->day = g_time_ctrl.day;
    time_ctrl->hour = g_time_ctrl.hour;
    time_ctrl->minute = g_time_ctrl.minute;
    time_ctrl->second = g_time_ctrl.second;
    TIME_UNLOCK();
    return status_ok;
}

uint32_t
calculate_time_to_timestamp(time_ctrl_t* time_ctrl) {

    uint32_t timestamp;
    Time_t tm_ctrl;

    Time_t xTm;
    tm_ctrl.tm_year = time_ctrl->year - 1900;
    tm_ctrl.tm_mon = time_ctrl->month - 1;
    tm_ctrl.tm_mday = time_ctrl->day;
    tm_ctrl.tm_hour = time_ctrl->hour;
    tm_ctrl.tm_min = time_ctrl->minute;
    tm_ctrl.tm_sec = time_ctrl->second;

    timestamp = mktime(&tm_ctrl);
    return timestamp;
}

/**
  * @brief  TIM9 Period Elapsed Callback (Timer event notification).
  *         This function is called when TIM9 reaches the end of the period.
  * @param  htim TIM handle
  * @retval None
  */
void
tim5_period_elapsed_cb(TIM_HandleTypeDef* htim) {
    tick_count++;
    // TRACE("time_count : %d s\n", time_count);
    xQueueSendFromISR(g_queue_time, &tick_count, NULL);
}

static void
prvCliCmdRtcGet(cli_printf cliprintf, int argc, char** argv) {
    CHECK_CLI();
    TIME_LOCK();
    Time_t xTm = RtcReadTime(RTC_TYPE_DS1338);
    // cliprintf("RTC time: %d, %s\r\n", mktime(&xTm), asctime(&xTm));
    cliprintf("RTC time: %d, ", mktime(&xTm));
    cliprintf("%s\r\n", asctime(&xTm));
    TIME_UNLOCK();
}
CLI_CMD_EXPORT(rtc_get, show rtc time, prvCliCmdRtcGet)

static void
prvCliCmdRtcSet(cli_printf cliprintf, int argc, char** argv) {
    CHECK_CLI();
    TIME_LOCK();
    if (argc != 7) {
        cliprintf("rtc_set YYYY MM DD hh mm ss\n");
        return;
    }
    int lYear = atoi(argv[1]); /* YYYY */
    int lMon = atoi(argv[2]);
    int lDay = atoi(argv[3]);
    int lHour = atoi(argv[4]);
    int lMin = atoi(argv[5]);
    int lSec = atoi(argv[6]);

    Time_t xTm;
    xTm.tm_year = lYear - 1900;
    xTm.tm_mon = lMon - 1;
    xTm.tm_mday = lDay;
    xTm.tm_hour = lHour;
    xTm.tm_min = lMin;
    xTm.tm_sec = lSec;
    RtcWriteTime(RTC_TYPE_DS1338, xTm);

    cliprintf("RTC time: %d, %s\r", mktime(&xTm), asctime(&xTm));
    // cliprintf("RTC time: %d, ", mktime(&xTm));
    // cliprintf("%s\r\n", asctime(&xTm));
    TIME_UNLOCK();
}
CLI_CMD_EXPORT(rtc_set, set rtc time, prvCliCmdRtcSet)
