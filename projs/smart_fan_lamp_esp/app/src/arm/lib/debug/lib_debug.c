/**
 * \file            lib_debug.c
 * \brief           Debug library
 */

/*
 * Copyright (c) 2024 Jasper
 *
 * This file is part of the debug library.
 *
 * Author:          Jasper <jasperzpzhang@gmail.com>
 * Version:         v1.0.0
 */

#include "lib/debug/lib_debug.h"
#include <cmsis_os2.h>
#include <stdarg.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

#if DEBUG_ENABLE

/* Pragmas */
#pragma diag_suppress 550 /* warning: #550-D: variable was set but never used */

/* Local variables */
static debug_chan_t g_debug_chan = DEBUG_CHAN_NULL;
static osMutexId_t g_debug_mutex = NULL;
static Bool_t g_debug_init = FALSE;

/* Forward declaration */
static int prv_is_in_isr(void);
static void prv_debug_lock(void);
static void prv_debug_unlock(void);

/* Functions */
status_t
debug_init(void) {
    g_debug_mutex = osMutexNew(NULL);
    g_debug_init = TRUE;
    return status_ok;
}

status_t
debug_term(void) {
    return status_ok;
}

status_t
debug_channel_set(debug_chan_t debug_chan) {
    g_debug_chan = debug_chan;
    return status_ok;
}

debug_chan_t
debug_channel_get(void) {
    return g_debug_chan;
}

status_t
debug_printf(char* cFormat, ...) {
    uint16_t len = 0;
    status_t ret = status_ok;
    static char buf[DEBUG_BUF_SIZE];

    /* "TCP & UDP" does not support printf function in isr */
    if (((DEBUG_CHAN_TCP == g_debug_chan) || (DEBUG_CHAN_UDP == g_debug_chan)) && prv_is_in_isr()) {
        return status_err;
    }

    /* Enter critical section */
    prv_debug_lock();

    va_list va;
    va_start(va, cFormat);
    len = vsprintf(buf, cFormat, va);
    switch (g_debug_chan) {
#if DEBUG_ENABLE_UART
        case DEBUG_CHAN_UART: ret = debug_uart_printf_direct(buf, len); break;
#endif /* DEBUG_ENABLE_UART */
#if DEBUG_ENABLE_TCP
        case DEBUG_CHAN_TCP: xRet = DebugTcpPrintfDirect(cBuf, usLength); break;
#endif /* DEBUG_ENABLE_TCP */
#if DEBUG_ENABLE_UDP
        case DEBUG_CHAN_UDP: xRet = DebugUdpPrintfDirect(cBuf, usLength); break;
#endif /* DEBUG_ENABLE_UDP */
        default:
            /* Do nothing. */
            break;
    }
    va_end(va);

    /* Leave critical section */
    prv_debug_unlock();

    return ret;
}

static int
prv_is_in_isr(void) {
    return osKernelGetState() == osKernelRunning;
}

static void
prv_debug_lock(void) {
    if (prv_is_in_isr()) {
        taskDISABLE_INTERRUPTS();
    } else {
        osMutexAcquire(g_debug_mutex, osWaitForever);
    }
    return;
}

static void
prv_debug_unlock(void) {
    if (prv_is_in_isr()) {
        taskENABLE_INTERRUPTS();
    } else {
        osMutexRelease(g_debug_mutex);
    }
    return;
}

#endif /* DEBUG_ENABLE */
