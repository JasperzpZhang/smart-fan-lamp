/**
 * \file            lib_cli.c
 * \brief           Command line interface library
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
 * This file is part of the cli library.
 *
 * Author:          Jasper <jasperzhangse@gmail.com>
 * Version:         v1.0.0
 */

#ifndef __LIB_CLI_H__
#define __LIB_CLI_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Includes */
#include <stdint.h>
#include "lib/type/lib_type.h"
#include "lib/cli/lib_cli_config.h"
#include "lib/cli/lib_cli_custom.h"

/* Defines */
#define CLI_CMD_EXPORT(name, help, func)                         \
    const char __cli_##name##_name[] = #name;                    \
    const char __cli_##name##_help[] = #help;                    \
    USED const cli_command_t __cli_##name SECTION("CliCmdTab") = \
        {                                                        \
            __cli_##name##_name,                                 \
            __cli_##name##_help,                                 \
            func};

    /* Functions */
    status_t lib_cli_init(void);
    status_t lib_cli_terminal(void);
    status_t lib_cli_load_cmd(void *pvCmd);

#if CLI_TEST
    status_t CliTestFreeRtos(uint8_t ucSel);
    status_t CliTestCustom(uint8_t ucSel);
#endif /* CLI_TEST */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LIB_CLI_H__ */
