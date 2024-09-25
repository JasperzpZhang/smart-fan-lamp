/*
    lib_debug.c

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


/* Includes */
#include "lib/debug/lib_debug_uart.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// #include "usart.h"

#if (DEBUG_ENABLE && DEBUG_ENABLE_UART)

/* Pragmas */
#pragma diag_suppress 550 /* warning: #550-D: variable was set but never used */

/* Local variables */
static UART_HandleTypeDef* g_huart = NULL;
static Bool_t g_debug_cfg = FALSE;

/* Forward declaration */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE* f)
#endif /* __GNUC__ */

/* Functions */
status_t
debug_uart_init(void) {
    /* Do nothing */
    return status_ok;
}

status_t
debug_uart_term(void) {
    /* Do nothing */
    return status_ok;
}

status_t
debug_uart_cfg(UART_HandleTypeDef* huart) {
    g_huart = huart;
    g_debug_cfg = TRUE;
    return status_ok;
}

status_t
debug_uart_printf(char* cFormat, ...) {
    static char tx_buf[DEBUG_BUF_SIZE];
    int vsp_cnt = 0;

    va_list va;
    va_start(va, cFormat);
    vsp_cnt = vsprintf(tx_buf, cFormat, va);
    if (vsp_cnt > 0) {
        while (HAL_UART_GetState(g_huart) == HAL_UART_STATE_BUSY_TX)
            ;
        HAL_UART_Transmit(g_huart, (uint8_t*)tx_buf, strlen(tx_buf), DEBUG_UART_SEND_TIMEOUT);
        va_end(va);
        return status_ok;
    } else {
        va_end(va);
        return status_err;
    }
}

status_t
debug_uart_printf_direct(char* cStr, uint16_t usLength) {
    while (HAL_UART_GetState(g_huart) == HAL_UART_STATE_BUSY_TX) {
        /* Do nothing. */
    };
    HAL_UART_Transmit(g_huart, (uint8_t*)cStr, usLength, DEBUG_UART_SEND_TIMEOUT);
    return status_ok;
}

#endif /* (DEBUG_ENABLE && DEBUG_ENABLE_UART) */
