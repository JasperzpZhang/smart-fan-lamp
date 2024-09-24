/**
 * \file            lib_uart_cfg.h
 * \brief           Uart library
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
 * This file if part of the uart library.
 *
 * Author:          Jasper <jasperzhangse@gmail.com>
 * Version:         v1.0.0
 */

#ifndef __LIB_UART_CFG_H__
#define __LIB_UART_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/* Includes */
#include "app/config.h"

/* Defines */
#ifndef UART_ENABLE
#define UART_ENABLE             (0)
#endif
#ifndef UART_RTOS
#define UART_RTOS               (1)
#endif
#ifndef UART_DEBUG
#define UART_DEBUG              (0)
#endif
#ifndef UART_ASSERT
#define UART_ASSERT             (0)
#endif
#ifndef UART_TEST
#define UART_TEST               (0)
#endif
#ifndef UART_ENABLE_MSP
#define UART_ENABLE_MSP         (0)
#endif
#ifndef UART_TXBUF_SIZE
#define UART_TXBUF_SIZE         (128)
#endif
#ifndef UART_RXBUF_SIZE
#define UART_RXBUF_SIZE         (128)
#endif

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __LIB_UART_CFG_H__ */
