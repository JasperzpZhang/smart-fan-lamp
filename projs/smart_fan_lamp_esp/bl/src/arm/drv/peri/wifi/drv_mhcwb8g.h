/**
 * \file            drv_mhcwb8g.h
 * \brief           Xiaomi MHCWB8G driver file
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
 * This file is part of the Xioami MHCWB8G driver.
 *
 * Author:          Jasper <jasperzhangse@gmail.com>
 * Version:         v1.0.0-dev
 * Last edit:       2024-06-18
 */

#ifndef __DRV_MHCWB8G_H__
#define __DRV_MHCWB8G_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "FreeRTOS.h"
#include "queue.h"
#include "stm32f1xx_hal.h"
#include "lib/type/lib_type.h"
#include "lib/uart/lib_uart.h"

    /* 定义消息结构体，用于传递数据 */
    #define WIFI_RX_BUFFER_SIZE 40
    typedef struct {
        uint8_t data[WIFI_RX_BUFFER_SIZE];
        uint8_t length;
    } wifi_msg_t;

    extern uart_ctrl_t g_wifi_ctrl;
    extern QueueHandle_t wifi_uart_queue;

    status_t mhcwb8g_init(UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma_rx, DMA_HandleTypeDef *hdma_tx);
    status_t wifi_send_cmd(const char *cmd);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __DRV_MHCWB8G_H__ */
