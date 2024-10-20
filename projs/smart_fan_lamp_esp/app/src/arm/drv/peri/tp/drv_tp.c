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
 * @file      drv_tp.c
 * @brief     Implementation File for TouchPad Module
 * @version   1.0.0
 * @author    Jasper
 * @date      2024-09-26
 */

/**
   modification history
   --------------------
   01a, 26Sep24, Jasper Created
 */

#include "drv/peri/tp/drv_tp.h"
#include "app/svc/tp.h"
#include "lib/cli/lib_cli.h"
#include "lib/debug/lib_debug.h"
#include "lib/iic/lib_iic.h"


/* Debug config */
#if TP_DEBUG || 1
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* TP_DEBUG */
#if TP_ASSERT
#undef ASSERT
#define ASSERT(a)                                                                                                      \
    while (1) {                                                                                                        \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                    \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* TP_ASSERT */

#define TP_HY16009A_ADDR 0x53
#define TP_REG_ADDR      0x00

QueueHandle_t g_queue_tp;
static uint8_t tp_init_ok = 0;

status_t
drv_tp_init(void) {
    g_queue_tp = xQueueCreate(10, sizeof(msg_tp_t));
    
    tp_init_ok = 1;
    return status_ok;
}

status_t
tp_read_data(uint8_t* buf) {
    //    iic_read_addr8(TP_HY16009A_ADDR, TP_REG_ADDR, buf, 2);
    
    
    iic_read_data(TP_HY16009A_ADDR, buf, 2, 1);
    
    

    if (buf[1] >= 180 && buf[1] <= 223) {
        buf[1] = 190;
    } else if ((buf[1] >= 224 && buf[1] <= 255) || (buf[1] <= 15)) {
        buf[1] = 0;
    }
    buf[1] = 190 - buf[1];
    
    
    
//    TRACE("buf[1] : %d\n", buf[1]);
    return status_ok;
}

void
gpio_5_exti_cb(uint16_t gpio_pin) {
    TRACE("gpio_5_exti_cb run\n");
    if (tp_init_ok == 1)
    {
        msg_tp_t msg;
        tp_read_data(msg.buf);
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(g_queue_tp, &msg, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

static void
cli_cmd_tp_get(cli_printf cliprintf, int argc, char** argv) {

    uint8_t buf[2] = {0};
    uint8_t i = 0;
    
    if (1 == argc) {

        tp_read_data(buf);

        cliprintf("buf[0] : ");
        for (i = 0; i < 8; i++) {
            cliprintf("%d ", ((buf[0] & 0x80) >> 7));
            buf[0] <<= 1;
        }
        cliprintf("\n");

        cliprintf("buf[1] : %d", buf[1]);
    } else {
        cliprintf("parameter length error\r\n");
    }
}
CLI_CMD_EXPORT(tp_get, get tp info, cli_cmd_tp_get)
