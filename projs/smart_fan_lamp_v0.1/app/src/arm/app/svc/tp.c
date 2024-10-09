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

#include "app/include.h"

/* Debug config */
#if TP_DEBUG || 0
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

QueueHandle_t g_queue_panel;

static void tp_task(void* parameter);

void
tp_init(void) {
    drv_tp_init();
    osDelay(100);
    g_queue_panel = xQueueCreate(20, sizeof(msg_panel_t));
    xTaskCreate(tp_task, "tp_task", 128, NULL, tskIDLE_PRIORITY + 2, NULL);
}

static void
tp_task(void* parameter) {

    uint8_t buf[2] = {0};
    msg_tp_t msg_tp;
    msg_panel_t msg_ctrl;
    static uint8_t tp_slider_en = 0;

    while (1) {
        wdog_feed();
        if (xQueueReceive(g_queue_tp, &msg_tp, portMAX_DELAY) == pdPASS) {

            if ((msg_tp.buf[0] & 0x10) != 0) {

                if (HAL_GPIO_ReadPin(KEY_INT_GPIO_Port, KEY_INT_Pin) == GPIO_PIN_RESET) {
                    /* wt == 1 */
                    do {
                        if (tp_slider_en == 0) {
                            tp_slider_en = 1;
                        }
                        tp_read_data(buf);
                        TRACE("tp_slider on : \n");
                        TRACE("buf[1] : %d\n", buf[1]);

                        msg_ctrl.slider_en = 1;
                        msg_ctrl.slider_value = buf[1];
                        xQueueSend(g_queue_panel, &msg_ctrl, portMAX_DELAY);

                        osDelay(200);
                    } while ((buf[0] & 0x10) != 0);
                }

            } else {
                /* wt == 0 */
                if ((HAL_GPIO_ReadPin(KEY_INT_GPIO_Port, KEY_INT_Pin) == GPIO_PIN_SET)) {

                    if (tp_slider_en == 1) {
                        TRACE("tp_slider off : \n");
                        TRACE("buf[1] : %d\n", msg_tp.buf[1]);

                        msg_ctrl.slider_en = 1;
                        msg_ctrl.slider_value = msg_tp.buf[1];
                        xQueueReset(g_queue_panel);
                        xQueueSend(g_queue_panel, &msg_ctrl, portMAX_DELAY);

                        tp_slider_en = 0;
                    } else {

                        msg_ctrl.panel._TP_KEY1 = 0;
                    }
                }

                if ((HAL_GPIO_ReadPin(KEY_INT_GPIO_Port, KEY_INT_Pin) == GPIO_PIN_RESET)) {

                    msg_ctrl.panel._TP_KEY1 = 1;
                    msg_ctrl.panel._TP_KEY4 = ((msg_tp.buf[0] >> 0) & 0x01);
                    msg_ctrl.panel._TP_KEY5 = ((msg_tp.buf[0] >> 1) & 0x01);
                    msg_ctrl.panel._TP_KEY6 = ((msg_tp.buf[0] >> 2) & 0x01);
                    msg_ctrl.panel._TP_KEY7 = ((msg_tp.buf[0] >> 3) & 0x01);
                    msg_ctrl.panel._TP_KEY8 = ((msg_tp.buf[0] >> 5) & 0x01);
                    msg_ctrl.panel._TP_KEY9 = ((msg_tp.buf[0] >> 6) & 0x01);

                    msg_ctrl.slider_en = 0;
                    xQueueSend(g_queue_panel, &msg_ctrl, portMAX_DELAY);
                }
            }
        }
        osDelay(20);
    }
}
