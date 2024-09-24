/**
 * \file            lib_debug_cfg.h
 * \brief           Debug library
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

#ifndef __LIB_DEBUG_CONFIG_H__
#define __LIB_DEBUG_CONFIG_H__

#ifdef __cplusplus
extern "C"
{
#endif /*__cplusplus */

/* Includes */
#include "app/config.h"

/* Configuration defines */
#ifndef DEBUG_ENABLE
#define DEBUG_ENABLE (0)
#endif
#ifndef DEBUG_ENABLE_UART
#define DEBUG_ENABLE_UART (0)
#endif
#ifndef DEBUG_ENABLE_TCP
#define DEBUG_ENABLE_TCP (0)
#endif
#ifndef DEBUG_ENABLE_UDP
#define DEBUG_ENABLE_UDP (0)
#endif
#ifndef DEBUG_RTOS
#define DEBUG_RTOS (1)
#endif
#ifndef DEBUG_DEBUG
#define DEBUG_DEBUG (0)
#endif
#ifndef DEBUG_TEST
#define DEBUG_TEST (0)
#endif
#ifndef DEBUG_ASSERT
#define DEBUG_ASSERT (0)
#endif
#ifndef DEBUG_BUF_SIZE
#define DEBUG_BUF_SIZE (128)
#endif
#ifndef DEBUG_UART_ENABLE_MSP
#define DEBUG_UART_ENABLE_MSP (0)
#endif
#ifndef DEBUG_UART_ENABLE_PRINTF
#define DEBUG_UART_ENABLE_PRINTF (0)
#endif
#ifndef DEBUG_UART_SEND_TIMEOUT
#define DEBUG_UART_SEND_TIMEOUT (1000) /* Ms */
#endif
#ifndef DEBUG_UDP_ENABLE_STAMP
#define DEBUG_UDP_ENABLE_STAMP (0)
#endif
#ifndef DEBUG_UDP_PKT_HEAD
#define DEBUG_UDP_PKT_HEAD (0x7E7E)
#endif
#ifndef DEBUG_UDP_PKT_TAIL
#define DEBUG_UDP_PKT_TAIL (0x0D0A)
#endif
#ifndef DEBUG_UDP_SVR_ADDR
#define DEBUG_UDP_SVR_ADDR "192.168.1.255"
#endif
#ifndef DEBUG_UDP_SVR_PORT
#define DEBUG_UDP_SVR_PORT (6000)
#endif
#ifndef DEBUG_UDP_CLI_ADDR
#define DEBUG_UDP_CLI_ADDR IPADDR_ANY
#endif
#ifndef DEBUG_UDP_CLI_PORT
#define DEBUG_UDP_CLI_PORT (8000)
#endif

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /*__LIB_DEBUG_CONFIG_H__*/
