/**
 * \file            lib_debug_uart.c
 * \brief           Debug uart library
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
 * This file is part of the debug library.
 *
 * Author:          Jasper <jasperzhangse@gmail.com>
 * Version:         v1.0.0
 */

/* Includes */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "lib/debug/lib_debug_uart.h"
// #include "usart.h"

#if (DEBUG_ENABLE && DEBUG_ENABLE_UART)

/* Pragmas */
#pragma diag_suppress 550 /* warning: #550-D: variable was set but never used */

/* Local variables */
static UART_HandleTypeDef *g_huart = NULL;
static Bool_t g_debug_cfg = FALSE;

/* Forward declaration */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Functions */
status_t debug_uart_init(void)
{
    /* Do nothing */
    return status_ok;
}

status_t debug_uart_term(void)
{
    /* Do nothing */
    return status_ok;
}

status_t debug_uart_cfg(UART_HandleTypeDef *huart)
{
    g_huart = huart;
    g_debug_cfg = TRUE;
    return status_ok;
}

status_t debug_uart_printf(char *cFormat, ...)
{
    static char tx_buf[DEBUG_BUF_SIZE];
    int vsp_cnt = 0;
    
    va_list va;
    va_start(va, cFormat);
    vsp_cnt = vsprintf(tx_buf, cFormat, va);
    if (vsp_cnt > 0) {
        while (HAL_UART_GetState(g_huart) == HAL_UART_STATE_BUSY_TX)
            ;
        HAL_UART_Transmit(g_huart, (uint8_t *)tx_buf, strlen(tx_buf), DEBUG_UART_SEND_TIMEOUT);
        va_end(va);
        return status_ok;
    }
    else {
        va_end(va);
        return status_err;
    }
}

status_t debug_uart_printf_direct(char *cStr, uint16_t usLength)
{
    while (HAL_UART_GetState(g_huart) == HAL_UART_STATE_BUSY_TX) {
        /* Do nothing. */
    };
    HAL_UART_Transmit(g_huart, (uint8_t *)cStr, usLength, DEBUG_UART_SEND_TIMEOUT);
    return status_ok;
}

#endif /* (DEBUG_ENABLE && DEBUG_ENABLE_UART) */
