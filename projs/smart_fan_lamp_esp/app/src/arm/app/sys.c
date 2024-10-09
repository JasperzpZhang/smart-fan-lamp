/**
 * \file            sys.c
 * \brief           System file
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
 * This file includes all the app functions for system.
 *
 * Author:          Jasper <jasperzhangse@gmail.com>
 * Version:         v1.0.0-dev
 */

#include "app/include.h"

#include "drv/peri/sc/lcd_1in83/drv_lcd_1in83.h"

#include "lvgl/lvgl.h"

#include "task.h"

/* Debug config */
#if SYS_DEBUG || 1
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* SYS_DEBUG */
#if SYS_ASSERT
#undef ASSERT
#define ASSERT(a)                                                                                                      \
    while (1) {                                                                                                        \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                    \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* SYS_ASSERT */

/* Local defines */

/* global variable */

//lv_mem_monitor_t mem_mon;

/* Forward functions */
static void sys_task(void* parameter);

void print_heap_info(void);



/* Functions */
status_t
sys_init(void) {

    xTaskCreate(sys_task, "sys_task", 128, NULL, tskIDLE_PRIORITY + 1, NULL);

    return status_ok;
}

static void
sys_task(void* parameter) {

    while (1) {
        
//        BaseType_t stack_high_watermark = uxTaskGetStackHighWaterMark(g_lvgl_task_hdl);
//        TRACE("My Task Remaining Stack Size: %lu bytes\n", stack_high_watermark * sizeof(UBaseType_t));
        
        osDelay(500);
        wdog_feed();
        osDelay(SYS_TASK_DELAY);
    }
}

void print_heap_info(void) {
    size_t free_heap_size = xPortGetFreeHeapSize();
    size_t min_free_heap_size = xPortGetMinimumEverFreeHeapSize();

    TRACE("Free Heap Size: %u bytes\n", free_heap_size);
    TRACE("Minimum Ever Free Heap Size: %u bytes\n", min_free_heap_size);
}




