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

typedef struct {
    uint32_t tick_count;
    uint8_t year;
    uint8_t mon;
    uint8_t week;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} time_ctrl_t;

static uint32_t tick_count = 0;

QueueHandle_t g_queue_time;
time_ctrl_t g_time_ctrl = TIME_CTRL_INIT;

void time_task(void* para);
time_ctrl_t sec_to_time(uint32_t sec);
uint32_t time_to_sec(uint8_t hour, uint8_t min, uint8_t sec);
status_t get_current_tick_time(time_ctrl_t* time_ctrl);

/*
    time_hdl_t g_time_radar;
    timing_start(&g_time_radar, 0, 15, 0);
    while(1){
        if(is_timing_ramaining(&g_time_radar) == 0){
            led_off();
        }
        if (radar_on == 1){
            timing_start(&g_time_radar, 0, 15, 0);
        }
        osDelay(500);
    }
 */

/* 
    time_hdl_t g_time_tomato;
    timing_start(&g_time_tomato, 0, 45, 0);

    while(1){
         if(is_timing_ramaining(&g_time_radar) == 0){
            TRACE("time : %d : %d : %d\n", g_time_tomato.rm_hour, g_time_tomato.rm_min, g_time_tomato.rm_sec)
        }
        osDelay(50);
    }
 */

/* 
    // �ݲ�����

    time_hdl_t g_time_clock;
    clock_create(&g_time_clock);

    while(1){

        TRACE("%d��%d�� %d : %d : %d\n", g_time_clock.tm_mon, g_time_clock.tm_day, g_time_clock.tm_hour, g_time_clock.tm_min, g_time_clock.tm_sec)
        osDelay(50);
    }
 */

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
    // �ڳ�ʼ�������� TIM5
    TIME_MUTEX_INIT();
    g_queue_time = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(time_task, "time task", 128, NULL, tskIDLE_PRIORITY + 2, NULL);
    HAL_TIM_Base_Start_IT(&htim5); // ���� TIM5 ��ʹ���ж�

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
get_current_timestamp(time_ctrl_t* time_ctrl) {
    uint32_t timestamp = rtc_get_timestamp();

    return timestamp;
}

void
time_task(void* para) {

    static uint32_t msg_tick_count = 0;

    while (1) {
        if (xQueueReceive(g_queue_time, &msg_tick_count, portMAX_DELAY) == pdPASS) {
            TIME_LOCK();
            g_time_ctrl = sec_to_time(msg_tick_count);
            TIME_UNLOCK();
        }
        // TRACE("time_count : %d s\n", g_time_ctrl.tick_count);
        // TRACE("Time: %02d days, %02d:%02d:%02d\n", g_time_ctrl.day, g_time_ctrl.hour, g_time_ctrl.min, g_time_ctrl.sec);
        // TRACE("\n");
    }
}

status_t
timing_start(time_hdl_t* time_hdl, uint8_t hour, uint8_t min, uint8_t sec) {
    time_ctrl_t time_ctrl;
    get_current_tick_time(&time_ctrl);
    time_hdl->start_time = time_ctrl.tick_count;
    time_hdl->duration = time_to_sec(hour, min, sec);
    // timing_update(time_hdl);
    return status_ok;
}

status_t
timing_update(time_hdl_t* time_hdl) {
    time_ctrl_t _time_ctrl;
    time_ctrl_t timing_time_ctrl;
    get_current_tick_time(&_time_ctrl);
    if ((time_hdl->start_time + time_hdl->duration) > _time_ctrl.tick_count) {
        time_hdl->remaining_time = (time_hdl->start_time + time_hdl->duration) - _time_ctrl.tick_count;
        // TRACE("time_hdl->start_time : %d\n", (int)time_hdl->start_time);
        // TRACE("time_hdl->duration   : %d\n", (int)time_hdl->duration);
        // TRACE("time_ctrl.tick_count : %d\n", (int)_time_ctrl.tick_count);

    } else {
        time_hdl->remaining_time = 0;
    }
    timing_time_ctrl = sec_to_time(time_hdl->remaining_time);
    time_hdl->rm_hour = timing_time_ctrl.hour;
    time_hdl->rm_min = timing_time_ctrl.min;
    time_hdl->rm_sec = timing_time_ctrl.sec;
    return status_ok;
}

uint8_t
is_timing_remaining(time_hdl_t* time_hdl) {
    timing_update(time_hdl);
    if (time_hdl->remaining_time > 0) {
        return 1;
    }
    return 0;
}

status_t
get_current_tick_time(time_ctrl_t* time_ctrl) {
    TIME_LOCK();
    time_ctrl->year = g_time_ctrl.year;
    time_ctrl->mon = g_time_ctrl.mon;
    time_ctrl->week = g_time_ctrl.week;
    time_ctrl->day = g_time_ctrl.day;
    time_ctrl->hour = g_time_ctrl.hour;
    time_ctrl->min = g_time_ctrl.min;
    time_ctrl->sec = g_time_ctrl.sec;
    time_ctrl->tick_count = g_time_ctrl.tick_count;
    TIME_UNLOCK();
    return status_ok;
}

uint32_t
time_to_sec(uint8_t hour, uint8_t min, uint8_t sec) {
    uint32_t total_sec;
    total_sec = (hour * 3600) + (min * 60) + sec;
    return total_sec;
}

time_ctrl_t
sec_to_time(uint32_t sec) {
    time_ctrl_t time_ctrl;
    time_ctrl.tick_count = sec;
    time_ctrl.day = sec / (24 * 3600); // ÿ���� 24 * 3600 ��
    sec = sec % (24 * 3600);           // ����ʣ������
    time_ctrl.hour = sec / 3600;       // ÿСʱ�� 3600 ��
    sec = sec % 3600;                  // ����ʣ������
    time_ctrl.min = sec / 60;          // ÿ������ 60 ��
    time_ctrl.sec = sec % 60;          // ʣ��ľ�������
    return time_ctrl;
}

#if 0

time_ctrl_t
calculate_time_from_seconds(uint32_t total_seconds) {
    time_ctrl_t time_ctrl;
    const uint32_t seconds_per_minute = 60;
    const uint32_t seconds_per_hour = 60 * seconds_per_minute;
    const uint32_t seconds_per_day = 24 * seconds_per_hour;
    TIME_LOCK();
    time_ctrl.tick_count = total_seconds;
    // �����롢���ӡ�Сʱ����
    time_ctrl.second = total_seconds % seconds_per_minute;
    uint32_t total_minutes = total_seconds / seconds_per_minute;
    time_ctrl.minute = total_minutes % 60;
    uint32_t total_hours = total_minutes / 60;
    time_ctrl.hour = total_hours % 24;
    time_ctrl.day = total_hours / 24; // ���㾭���˶�����
    TIME_UNLOCK();
    return time_ctrl;
}

uint32_t
time_to_seconds(uint16_t hour, uint16_t minute, uint16_t second) {
    const uint32_t seconds_per_minute = 60;
    const uint32_t seconds_per_hour = 60 * seconds_per_minute;
    // ��Сʱ�����ӡ���ת��Ϊ������
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

#endif

void
tim5_period_elapsed_cb(TIM_HandleTypeDef* htim) {
    tick_count++;
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
