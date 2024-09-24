/**
 * \file            lib_cli_custom.h
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
#ifndef __LIB_CLI_CUSTOM_H__
#define __LIB_CLI_CUSTOM_H__

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include "lib/type/lib_type.h"
#include "lib/cli/lib_cli_config.h"


/**
 * \ingroup         CLI
 * \defgroup        CLI_CONFIG Configuration
 * \brief           Default CLI configuration
 * \{
 *
 * Configuration for command line interface (CLI).
 */

/**
 * \brief           CLI promet, printed on every NL
 */
#ifndef CLI_PROMPT
#define CLI_PROMPT                  "> "
#endif

/**
 * \brief           CLI NL, default is NL and CR
 */
#ifndef CLI_NL
#define CLI_NL                      "\r\n"
#endif

/**
 * \brief           Max CLI command length
 */
#ifndef CLI_MAX_CMD_LENGTH
#define CLI_MAX_CMD_LENGTH          256
#endif

/**
 * \brief           Max sotred CLI commands to history
 */
#ifndef CLI_CMD_HISTORY
#define CLI_CMD_HISTORY             10
#endif


/**
 * \brief           Max CLI arguments in a single command
 */
#ifndef CLI_MAX_NUM_OF_ARGS
#define CLI_MAX_NUM_OF_ARGS         16
#endif

/**
 * \brief           Max modules for CLI
 */
#ifndef CLI_MAX_MODULES
#define CLI_MAX_MODULES             16
#endif

/**
 * \}
 */

/**
 * \defgroup        CLI Command line interface
 * \brief           Command line interface
 * \{
 *
 * Functions to initialize everything needed for command line interface (CLI).
 */

typedef void cli_printf(const char* format, ...);
typedef void cli_function(cli_printf cliprintf, int argc, char** argv);

/**
 * \brief           CLI command structure
 */
typedef struct {                                
    const char* name;                           /*!< Command name */
    const char* help;                           /*!< Command help */
    cli_function* func;                         /*!< Command function */
} cli_command_t;

/**
 * \brief           List of commands
 */
typedef struct {
    const cli_command_t* commands;              /*!< Pointer to commands */
    size_t num_of_commands;                     /*!< Total number of commands */
} cli_commands_t;

/* Functions */
status_t    cli_custom_init(void);
status_t    cli_custom_terminal(void);

status_t    cli_custom_load_cmd(const cli_command_t *commands, size_t num_of_commands);
void        cli_custom_input(cli_printf cliprintf, char ch);

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif /* __LIB_CLI_CUSTOM_H__ */

