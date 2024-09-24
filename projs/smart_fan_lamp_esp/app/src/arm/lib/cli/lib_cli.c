/**
 * \file            lib_cli.c
 * \brief           Command line interface library
 */

/*
 * Copyright (c) 2024 Jasper
 *
 * This file is part of the cli library.
 *
 * Author:          Jasper <jasperzhangse@gmail.com>
 * Version:         v1.0.0
 */

/* Includes. */
#include "lib/cli/lib_cli.h"
#include "FreeRTOS.h"
#include "lib/debug/lib_debug.h"
#include "task.h"
#include <stdio.h>
#include <string.h>


#if CLI_ENABLE

/* Debug config */
#if CLI_DEBUG
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* CLI_DEBUG */
#if CLI_ASSERT
#undef ASSERT
#define ASSERT(a)                                                                                                      \
    while (!(a)) {                                                                                                     \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                    \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* CLI_ASSERT */

/* Functions */
status_t lib_cli_init(void) {
    /* Do nothing */
    return status_ok;
}

status_t lib_cli_terminal(void) {
    /* Do nothing */
    return status_ok;
}

status_t lib_cli_load_cmd(void* pvCmd) {
#if (CLI_ENABLE_UART || CLI_ENABLE_UDP)
    FreeRTOS_CLIRegisterCommand(pvCmd);
#endif /* (CLI_ENABLE_UART || CLI_ENABLE_UDP) */
    return status_ok;
}

#endif /* CLI_ENABLE */
