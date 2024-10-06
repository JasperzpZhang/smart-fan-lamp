/**
 * Copyright (c) 2024 Jasper All rights reserved
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 *
 * @file      tp.c
 * @brief     Implementation File for wave Module
 * @version   1.0.0
 * @author    Jasper
 * @date      2024-09-28
 */

/**
   modification history
   --------------------
   01a, 28Sep24, Jasper Created
 */

#include "app/include.h"
#include "lib/lvgl/lvgl.h"
#include "lv_port_disp.h"

/* Debug config */
#if WAVE_DEBUG || 1
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* WAVE_DEBUG */
#if WAVE_ASSERT
#undef ASSERT
#define ASSERT(a)                                                                                                      \
    while (1) {                                                                                                        \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                    \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* WAVE_ASSERT */

/* Local defines */
#if LVGL_RTOS || 0
#undef MUTEX_NAME
#define MUTEX_NAME lvgl_mutex
static osMutexId_t MUTEX_NAME;
#define LVGL_MUTEX_INIT()                                                                                              \
    do {                                                                                                               \
        osMutexAttr_t mutex_attr = {0};                                                                                \
        MUTEX_NAME = osMutexNew(&mutex_attr);                                                                          \
    } while (0)
#define LVGL_LOCK()   osMutexAcquire(MUTEX_NAME, osWaitForever)
#define LVGL_UNLOCK() osMutexRelease(MUTEX_NAME)
#else
#define LVGL_MUTEX_INIT()
#define LVGL_LOCK()
#define LVGL_UNLOCK()
#endif /* MEM_RTOS */

void lvgl_task(void* para);

status_t
lvgl_init(void) {

    LVGL_MUTEX_INIT();

    // sc_init(hxc_lcd_1_8);

    lv_init();
    lv_port_disp_init();

    lv_obj_t* sw = lv_switch_create(lv_scr_act()); // 在当前屏幕创建开关按钮
    // 设置开关按钮的位置为屏幕中间（大致位置）
    lv_obj_set_pos(sw, 120, 100);

    xTaskCreate(lvgl_task, "lvgl task", 1024, NULL, tskIDLE_PRIORITY + 2, NULL);
    return status_ok;
}

void
lvgl_task(void* para) {

    while (1) {
        LVGL_LOCK();
        lv_timer_handler();
        LVGL_UNLOCK();
        osDelay(5);
    }
}
