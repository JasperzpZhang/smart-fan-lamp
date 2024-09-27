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

QueueHandle_t g_led_queue;
QueueHandle_t g_ctrl_queue;

static void tp_task(void* parameter);

void
tp_init(void) {
    drv_tp_init();
    g_led_queue = xQueueCreate(10, sizeof(led_msg_t));
    g_ctrl_queue = xQueueCreate(10, sizeof(ctrl_msg_t));
    if (g_led_queue != NULL) {
        TRACE("g_led_queue create sucess.\n");
    }
    xTaskCreate(tp_task, "tp_task", 128, NULL, tskIDLE_PRIORITY + 2, NULL);
}

static void
tp_task(void* parameter) {

    uint8_t buf[2] = {0};
    tp_msg_t tp_msg;
//    led_msg_t led_msg;
    ctrl_msg_t ctrl_msg;
    uint8_t i;

    static uint8_t tp_slider_en = 0;

    while (1) {
        if (xQueueReceive(g_tp_queue, &tp_msg, portMAX_DELAY) == pdPASS) {

            if ((tp_msg.buf[0] & 0x10) != 0) {

                if (HAL_GPIO_ReadPin(KEY_INT_GPIO_Port, KEY_INT_Pin) == GPIO_PIN_RESET) {
                    /* wt == 1 */
                    do {
                        if (tp_slider_en == 0) {
                            tp_slider_en = 1;
                        }
                        tp_read_data(buf);
                        TRACE("tp_slider on : \n");

//                        led_msg.slider_value = buf[1];
//                        xQueueSend(g_led_queue, &led_msg, portMAX_DELAY);
                        
                        ctrl_msg.slider_en = 1;
                        ctrl_msg.slider_value = buf[1];
                        xQueueSend(g_ctrl_queue, &ctrl_msg, portMAX_DELAY);

                        osDelay(200);
                    } while ((buf[0] & 0x10) != 0);
                }

            } else {
                /* wt == 0 */
                if ((HAL_GPIO_ReadPin(KEY_INT_GPIO_Port, KEY_INT_Pin) == GPIO_PIN_SET)) {

                    if (tp_slider_en == 1) {
                        TRACE("tp_slider off : \n");
                        TRACE("buf[0] : ");
                        for (i = 7; i > 0; i--) {
                            TRACE("%d ", (tp_msg.buf[0] >> i) & 0x01);
                        }
                        TRACE("\n");
                        TRACE("buf[1] : %d\n\n", tp_msg.buf[1]);

                        // led_msg.slider_value = tp_msg.buf[1];
                        // xQueueReset(g_led_queue);
                        // xQueueSend(g_led_queue, &led_msg.slider_value, portMAX_DELAY);

                        ctrl_msg.slider_en = 1;
                        ctrl_msg.slider_value = tp_msg.buf[1];
                        xQueueReset(g_ctrl_queue);
                        xQueueSend(g_ctrl_queue, &ctrl_msg, portMAX_DELAY);

                        tp_slider_en = 0;
                    } else {

                        ctrl_msg.tp._TP_KEY1 = 0;
                    }
                }

                if ((HAL_GPIO_ReadPin(KEY_INT_GPIO_Port, KEY_INT_Pin) == GPIO_PIN_RESET)) {

                    TRACE("tp_key on : \n");
                    TRACE("buf[0] : ");
                    for (i = 7; i > 0; i--) {
                        TRACE("%d ", (tp_msg.buf[0] >> i) & 0x01);
                    }
                    TRACE("\n");
                    TRACE("buf[1] : %d\n\n", tp_msg.buf[1]);

                    ctrl_msg.tp._TP_KEY1 = 1;
                    ctrl_msg.tp._TP_KEY4 = ((tp_msg.buf[0] >> 0) & 0x01);
                    ctrl_msg.tp._TP_KEY5 = ((tp_msg.buf[0] >> 1) & 0x01);
                    ctrl_msg.tp._TP_KEY6 = ((tp_msg.buf[0] >> 2) & 0x01);
                    ctrl_msg.tp._TP_KEY7 = ((tp_msg.buf[0] >> 3) & 0x01);
                    ctrl_msg.tp._TP_KEY8 = ((tp_msg.buf[0] >> 5) & 0x01);
                    ctrl_msg.tp._TP_KEY9 = ((tp_msg.buf[0] >> 6) & 0x01);
                    
                    ctrl_msg.slider_en = 0;
                    xQueueSend(g_ctrl_queue, &ctrl_msg, portMAX_DELAY);
                }
            }
        }
        osDelay(20);
    }
}
