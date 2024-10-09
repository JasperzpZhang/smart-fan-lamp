/**
 * \file            drv_dwin01.h
 * \brief           Dwin01 driver file
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
 * This file is part of the dwin01 driver.
 *
 * Author:          Jasper <jasperzhangse@gmail.com>
 * Version:         v1.0.0-dev
 * Last edit:       2024-06-18
 */

#ifndef __DRV_DWIN01_H__
#define __DRV_DWIN01_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "FreeRTOS.h"
#include "queue.h"
#include "stm32f1xx_hal.h"
#include "lib/type/lib_type.h"
#include "lib/uart/lib_uart.h"

    /* system */
    enum {
        SET_BRIGHTNESS      = 0x0082
    };

    /* user */
    enum {
        SET_LED_BR_ADDR      = 0x1001,
        SET_LED_CT_ADDR      = 0x1002,
        SET_LED_ONOFF_ADDR   = 0x1003,
        SET_FAN_ONOFF_ADDR   = 0x1004,
        SET_FAN_LEVEL_ADDR   = 0x1005
    };

    /* 定义消息结构体，用于传递数据 */
    #define RX_BUFFER_SIZE 10
    typedef struct {
        uint8_t data[RX_BUFFER_SIZE];
        uint8_t length;
    } Message;

    /* 声明消息队列句柄 */
    extern QueueHandle_t xQueue;
    extern uart_ctrl_t g_screen_ctrl;
    
    status_t dwin01_init(UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma_rx, DMA_HandleTypeDef *hdma_tx);
    static uint8_t *prvParseHexStr(const char *pcStr, uint8_t *pucLength);
    status_t refresh_screen(uint16_t address, uint8_t value);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __DRV_DWIN01_H__ */
