/**
 * \file            lib_uart.h
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
 * This file includes all the library functions for uart.
 *
 * Author:          Jasper <jasperzhangse@gmail.com>
 * Version:         v1.1.0
 */

#ifndef __LIB_UART_H__
#define __LIB_UART_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /* Includes */
    #include <stdint.h>
    #include "main.h"
    #include "lib/type/lib_type.h"
    #include "lib/uart/lib_uart_cfg.h"

    typedef enum {
        UART_DMA_ENABLE,
        UART_DMA_DISABLE
    }uart_dma_en_t;
    
   /* Types */
    
#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __LIB_UART_H__ */
