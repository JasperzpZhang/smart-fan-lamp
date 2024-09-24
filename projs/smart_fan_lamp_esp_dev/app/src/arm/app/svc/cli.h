/**
 * \file            cli.h
 * \brief           Command line interface
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
 * This file is the entire cli application.
 *
 * Author:          Jasper <jasperzhangse@gmail.com>
 * Version:         v1.0.0-dev
 */

#ifndef __CLI_H__
#define __CLI_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "FreeRTOS.h"
#include "queue.h"
#include "lib/type/lib_type.h"
#include "lib/uart/lib_uart.h"

    /* Defines */
    #ifndef CLI_ENABLE_SECURITY
    #define CLI_ENABLE_SECURITY 1
    #endif /* CLI_ENABLE_SECURITY */
    
    #ifndef CLI_RX_BUF_SIZE
    #define CLI_RX_BUF_SIZE 128
    #endif /* CLI_RX_BUF_SIZE */

    typedef struct
    {
        Bool_t init;
        UART_HandleTypeDef *huart;
        uint8_t uart_rx_buf[CLI_RX_BUF_SIZE];
        uart_dma_en_t uart_dma_en;
        xQueueHandle rx_queue;
        char buf[64];
        char fmt_buf[128];
    } cli_uart_ctrl_t;

    extern cli_uart_ctrl_t cli_uart_ctrl;

    
    

#define CHECK_CLI()                             \
    if (!cli_is_enabled())                        \
    {                                           \
        cliprintf("Please enable cli first\n"); \
        return;                                 \
    }

    /* Functions */
    status_t cli_init(UART_HandleTypeDef *huart, uart_dma_en_t uart_dma_en);
    status_t cli_term(void);

    Bool_t cli_is_enabled(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CLI_H__ */
