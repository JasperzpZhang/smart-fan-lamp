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

#include "drv/peri/voice/drv_voice.h"
#include "lib/cli/lib_cli.h"
#include "lib/debug/lib_debug.h"
#include "lib/iic/lib_iic.h"
#include "lib/uart/lib_uart.h"
#include "usart.h"

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

QueueHandle_t g_queue_voice;
void test_task(void* para);
uint8_t buf[32] = {0};

status_t
drv_voice_init(void) {

    g_queue_voice = xQueueCreate(10, sizeof(buf));

    HAL_UARTEx_ReceiveToIdle_DMA(&huart3, buf, 32);

    xTaskCreate(test_task, "test_task", 128, NULL, tskIDLE_PRIORITY + 2, NULL);

    TRACE("uart3 receive ok;\n");

    return status_ok;
}

void
uart3_rx_event_callback(UART_HandleTypeDef* huart, uint16_t size) {

    xQueueSendFromISR(g_queue_voice, &buf, NULL);

    HAL_UARTEx_ReceiveToIdle_DMA(&huart3, buf, 32);
}

void
test_task(void* para) {
    uint8_t msg_buf[32] = {0};

    TRACE("test task run\n");

    while (1) {
        if (xQueueReceive(g_queue_voice, &msg_buf, portMAX_DELAY) == pdPASS) {

            TRACE("msg_buf : %s\n", msg_buf);
        }

        osDelay(100);
    }
}
