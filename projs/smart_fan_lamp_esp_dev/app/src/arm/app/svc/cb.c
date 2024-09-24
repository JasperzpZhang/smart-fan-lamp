/**
 * \file            cb.c
 * \brief           callback
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
 * This file is all of callback functions.
 *
 * Author:          Jasper <jasperzhangse@gmail.com>
 * Version:         v1.0.0-dev
 */

/* Includes */
#include "app/include.h"

/* Debug config */
#if CB_DEBUG || 1
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* CB_DEBUG */
#if CB_ASSERT
#undef ASSERT
#define ASSERT(a)                                                  \
    while (!(a))                                                   \
    {                                                              \
        DebugPrintf("ASSERT failed: %s %d\n", __FILE__, __LINE__); \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* CB_ASSERT */

/* Local defines */
#define MAX_MSG_SIZE 128

/* Local types */

/* Forward declaration */
static void cb_task(void *parameter);

/* Local variables */
static rbuf_handle_t g_uart_cb_rbuf_handle;
static uint8_t       g_uart_cb_buffer[256];

/* Functions */
status_t cb_init(void)
{
    rbuf_init();
    g_uart_cb_rbuf_handle = rbuf_create();
    rbuf_cfg(g_uart_cb_rbuf_handle, g_uart_cb_buffer, sizeof(g_uart_cb_buffer), 3, 5);

    xTaskCreate(cb_task, "cb_task", 128, NULL, tskIDLE_PRIORITY + 2, NULL);
    return status_ok;
}

static void cb_task(void *parameter)
{
    static uint8_t  recv_buffer[MAX_MSG_SIZE];
    
    while (1) {
        uint16_t recvd = (uint16_t)RbufRead(g_uart_cb_rbuf_handle, recv_buffer, MAX_MSG_SIZE);
        if (recvd) {
            TRACE("recv_buffer : %s\n", recv_buffer);
        }
        osDelay(SYS_TASK_DELAY);
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == UART4) {
//        uart_idle_dma_cb(&cli_uart_ctrl.huart);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	/* re-enable rxne interrupt */
    if (huart->Instance == UART4) {
//        RbufWrite(g_uart_cb_rbuf_handle, pucBuf, usLength);
        uart_it_cb(&cli_uart_ctrl.huart);
    }
}
