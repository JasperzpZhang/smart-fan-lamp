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
 * @file      drv_voice.c
 * @brief     Implementation File for voice Module
 * @version   1.0.0
 * @author    Jasper
 * @date      2024-10-06
 */

/**
   modification history
   --------------------
   01a, 26Sep24, Jasper Created
 */

#include "drv/peri/sc/lcd_1in83/drv_cst816t_if.h"
#include <stdint.h>
#include "lib/cli/lib_cli.h"
#include "lib/debug/lib_debug.h"
#include "lib/iic/lib_iic.h"

/* debug config */
#if CST816T_DEBUG || 1
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* CST816T_DEBUG */
#if CST816T_ASSERT
#undef ASSERT
#define ASSERT(a)                                                                                                      \
    while (1) {                                                                                                        \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                    \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* CST816T_ASSERT */

/* global variables */
cst816t_hdl_t cst816t_hdl;
SemaphoreHandle_t g_binary_semaphore_tp_event;

/* functions */
status_t
cst816t_init(void) {
    cst816t_hdl._ctrl.tp_event = false;
    g_binary_semaphore_tp_event = xSemaphoreCreateBinary();

    cst816t_hdl.iic_read = iic_read_addr8;
    cst816t_hdl.iic_write = iic_write_addr8;
    cst816t_hdl._ctrl.rst_port = LCD_TP_RST_GPIO_Port;
    cst816t_hdl._ctrl.rst_pin = LCD_TP_RST_Pin;
    cst816t_hdl._ctrl.irq_port = LCD_TP_INT_GPIO_Port;
    cst816t_hdl._ctrl.irq_pin = LCD_TP_INT_Pin;
    drv_cst816t_init(&cst816t_hdl, EN_CHANGE);
    return status_ok;
}

bool
cst816t_read(void) {
    if (xSemaphoreTake(g_binary_semaphore_tp_event, 0) == pdTRUE) {
        cst816t_hdl._ctrl.tp_event = !cst816t_hdl._ctrl.tp_event;
        if (cst816t_hdl._ctrl.tp_event == true)
        {
            TRACE("Touch down\n");
        }
        else
        {
            TRACE("Touch up\n");
        }
            
    }

    if (cst816t_hdl._ctrl.tp_event == true) {
        // TRACE("tp_event true\n");
        cst816t_available(&cst816t_hdl);
        
        if (cst816t_hdl._ctrl.finger_num == 0){
            cst816t_hdl._ctrl.tp_event = false;
            TRACE("Touch up\n");
            return false;
        }

        /* -90 */
        uint16_t tmp = cst816t_hdl._ctrl.x;
        cst816t_hdl._ctrl.x = cst816t_hdl._ctrl.y;
        cst816t_hdl._ctrl.y = 240 - tmp;

        TRACE("зјБъ: %d , %d\n", cst816t_hdl._ctrl.x, cst816t_hdl._ctrl.y);

        return true;
    } else {
        // TRACE("tp_event false\n");
        return false;
    }
}

void
gpio_4_exti_cb(uint16_t gpio_pin) {
    TRACE("gpio_4_exti_cb \n");
    BaseType_t higher_priority_task_woken = pdFALSE;
    
    xSemaphoreGiveFromISR(g_binary_semaphore_tp_event, &higher_priority_task_woken);
    portYIELD_FROM_ISR(higher_priority_task_woken);
}

static void
cli_cmd_sc_tp_get(cli_printf cliprintf, int argc, char** argv) {

    if (1 == argc) {

        bool press = cst816t_read();
        cliprintf("pass = %d\n", press);

        if (press == true) {
            cliprintf("x : %d\n", cst816t_hdl._ctrl.x);
            cliprintf("y : %d\n", cst816t_hdl._ctrl.y);
        }

    } else {
        cliprintf("parameter length error\r\n");
    }
}
CLI_CMD_EXPORT(sc_tp_get, get screen touchpad info, cli_cmd_sc_tp_get)

static void
cli_cmd_sc_tp_get_info(cli_printf cliprintf, int argc, char** argv) {

    if (1 == argc) {

        bool press = cst816t_read();
        cliprintf("pass = %d\n", press);

        cliprintf("chip_id          : %d\n", cst816t_hdl._ctrl.chip_id);
        cliprintf("finger_num       : %d\n", cst816t_hdl._ctrl.finger_num);
        cliprintf("gesture_id       : %d\n", cst816t_hdl._ctrl.gesture_id);
        cliprintf("firmware_version : %d\n", cst816t_hdl._ctrl.firmware_version);
        cliprintf("x                : %d\n", cst816t_hdl._ctrl.x);
        cliprintf("y                : %d\n", cst816t_hdl._ctrl.y);

    } else {
        cliprintf("parameter length error\r\n");
    }
}
CLI_CMD_EXPORT(sc_tp_get_info, get screen touchpad info, cli_cmd_sc_tp_get_info)
