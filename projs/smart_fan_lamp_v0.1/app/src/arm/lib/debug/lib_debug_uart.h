/*
    lib_debug.h

    Implementation File for Debug Module
*/

/* Copyright (c) 2024 Jasper. */

/*
    modification history
    --------------------
    01a, 12Sep18, Karl Created
    01b, 15Nov18, Karl Modified
    01c, 15Jul19, Karl Reconstructured Debug module
    01d, 26Aug19, Karl Modified include files
    01e, 24Sep24, Jasper, Renamed functions from camel case to snake case.
*/

#ifndef __LIB_DEBUG_UART_H__
#define __LIB_DEBUG_UART_H__

#ifdef __cplusplus
extern "C"
{
#endif /*__cplusplus */

/* Includes */
#include <stdint.h>
#include "main.h"
#include "lib/type/lib_type.h"
#include "lib/debug/lib_debug_cfg.h"

    /* Functions */
    status_t debug_uart_init(void);
    status_t debug_uart_term(void);
    status_t debug_uart_cfg(UART_HandleTypeDef *huart);

    status_t debug_uart_printf(char *cFormat, ...);
    status_t debug_uart_printf_direct(char *cStr, uint16_t usLength);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /*__LIB_DEBUG_UART_H__*/
