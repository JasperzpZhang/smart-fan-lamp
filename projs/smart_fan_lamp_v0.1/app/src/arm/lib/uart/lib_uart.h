/*
    lib_debug.h

    Implementation File for Debug Module
*/

/* Copyright (c) 2024 Jasper. */

/*
    modification history
    --------------------
    01a, 25Sep24, Jasper Created
*/

#ifndef __LIB_UART_H__
#define __LIB_UART_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes */
#include <stdint.h>
#include "lib/type/lib_type.h"
#include "lib/uart/lib_uart_cfg.h"
#include "main.h"

typedef enum {
    UART_DMA_ENABLE,
    UART_DMA_DISABLE,
} uart_dma_en_t;

/* Types */

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __LIB_UART_H__ */
