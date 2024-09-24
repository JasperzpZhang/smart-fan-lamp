/**
 * \file            lib_cli_custom.c
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

#include "lib/cli/lib_cli_custom.h"
#include "lib/debug/lib_debug.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


#if (CLI_ENABLE && CLI_ENABLE_CUSTOM)

/* Pragmas */
#pragma diag_suppress 177 /* warning: #177-D: variable "VAR" was set but never used */

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
        DebugPrintf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                     \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* CLI_ASSERT */

static cli_commands_t cli_command_table[CLI_MAX_MODULES];
static size_t num_of_modules;

static void cli_list(cli_printf cliprintf, int argc, char** argv);
static void cli_help(cli_printf cliprintf, int argc, char** argv);

/**
 * \brief           List of commands
 */
static const cli_command_t commands[] = {
    {"help", "Displays helptext for given command", cli_help},
    {"list", "Lists available commands", cli_list},
};

/**
 * \brief           Find the CLI command that matches the input string
 * \param[in]       command: pointer to command string for which we are searching
 * \return          pointer of the command if we found a match, else NULL
 */
const cli_command_t* cli_lookup_command(char* command) {
    uint32_t module_index, command_index;
    for (module_index = 0; module_index < num_of_modules; module_index++) {
        for (command_index = 0; command_index < cli_command_table[module_index].num_of_commands; command_index++) {
            if (strcmp(command, cli_command_table[module_index].commands[command_index].name) == 0) {
                return &cli_command_table[module_index].commands[command_index];
            }
        }
    }
    return NULL;
}

/**
 * \brief           CLI auto completion function
 * \param[in]       cliprintf: Pointer to CLI printf function
 * \param[in]       cmd_buffer: CLI command buffer
 * \param[in]       cmd_pos: pointer to current courser postion in command buffer
 * \param[in]       print_options: additional prints in case of double tab
 */
void cli_tab_auto_complete(cli_printf cliprintf, char* cmd_buffer, uint32_t* cmd_pos, bool print_options) {
    const char* matched_command = NULL;
    uint32_t module_index, command_index;
    uint32_t num_of_matched_commands = 0;
    uint32_t common_command_len = 0;

    for (module_index = 0; module_index < num_of_modules; module_index++) {
        for (command_index = 0; command_index < cli_command_table[module_index].num_of_commands; command_index++) {
            const cli_command_t* command = &cli_command_table[module_index].commands[command_index];
            if (strncmp(cmd_buffer, cli_command_table[module_index].commands[command_index].name, *cmd_pos) == 0) {
                /* Found a new command which matches the string */
                if (num_of_matched_commands == 0) {
                    /*
                     * Save the first match for later tab completion in case
                     * print_option is true (double tab)
                     */
                    matched_command = command->name;
                    common_command_len = strlen(matched_command);
                } else {
                    /*
                     * More then one match
                     * in case of print_option we need to print all options
                     */
                    if (print_options) {
                        /*
                         * Because we want to print help options only when we
                         * have multiple matches, print also the first one.
                         */
                        if (num_of_matched_commands == 1) {
                            cliprintf(CLI_NL "%s" CLI_NL, matched_command);
                        }
                        cliprintf("%s" CLI_NL, command->name);
                    }

                    /* 
                     * Find the common prefix of all the matched commands for
                     * partial completion
                     */
                    int last_common_command_len = common_command_len;
                    common_command_len = 0;
                    while (matched_command[common_command_len] == command->name[common_command_len]
                           && matched_command[common_command_len] != '\0' && command->name[common_command_len] != '\0'
                           && common_command_len < last_common_command_len) {
                        common_command_len++;
                    }
                }
                num_of_matched_commands++;
            }
        }
    }

    /* Do the full/partial tab completion */
    if (matched_command != NULL) {
        strncpy(cmd_buffer, matched_command, common_command_len);
        *cmd_pos = strlen(cmd_buffer);
        cmd_buffer[*cmd_pos] = '\0';
        cliprintf("\r" CLI_PROMPT "%s", cmd_buffer);
    }
}

/**
 * \brief           Register new CLI commands
 * \param[in]       commands: Pointer to commands table
 * \param[in]       num_of_commands: Number of new commands
 * \return          true when new commands where succesfully added, else false
 */
status_t cli_custom_load_cmd(const cli_command_t* commands, size_t num_of_commands) {
    ASSERT(num_of_modules < CLI_MAX_MODULES);
    if (num_of_modules >= CLI_MAX_MODULES) {
        TRACE("Exceeded the maximum number of CLI modules\n\r");
        return status_err;
    }

    /*
     * Warning: Not threadsafe!
     * TODO: add mutex that is initialized in init function
     */
    cli_command_table[num_of_modules].commands = commands;
    cli_command_table[num_of_modules].num_of_commands = num_of_commands;
    num_of_modules++;

    return status_ok;
}

/**
 * \brief           CLI Init function
 */
status_t cli_custom_init(void) {
    cli_custom_load_cmd(commands, sizeof(commands) / sizeof(commands[0]));
    return status_ok;
}

/**
 * \brief           CLI Term function
 */
status_t cli_custom_terminal(void) {
    /* Do nothing */
    return status_ok;
}

/**
 * \brief           CLI command for printing help text of specific command
 * \param[in]       cliprintf: Pointer to CLI printf function
 * \param[in]       argc: Number fo arguments in argv
 * \param[in]       argv: Pointer to the commands arguments
 */
static void cli_help(cli_printf cliprintf, int argc, char** argv) {
    const cli_command_t* command;

    if (argc < 2) {
        cliprintf("Error: No function specified (try `list`)\n");
        return;
    }

    if ((command = cli_lookup_command(argv[1])) == NULL) {
        cliprintf("Error, command `%s` not found\n", argv[1]);
        return;
    }

    if (command->help != NULL) {
        cliprintf("%s\n", command->help);
    }
}

/**
 * \brief           CLI command for listing all available commands
 * \param[in]       cliprintf: Pointer to CLI printf function
 * \param[in]       argc: Number fo arguments in argv
 * \param[in]       argv: Pointer to the commands arguments
 */
static void cli_list(cli_printf cliprintf, int argc, char** argv) {
    uint32_t module_index, command_index;

    cliprintf("%-20s%s" CLI_NL, "Command", "Description");
    cliprintf("-----------------------------------------------------------" CLI_NL);

    for (module_index = 0; module_index < num_of_modules; module_index++) {
        for (command_index = 0; command_index < cli_command_table[module_index].num_of_commands; command_index++) {
            cliprintf("%-20s%s" CLI_NL, cli_command_table[module_index].commands[command_index].name,
                      cli_command_table[module_index].commands[command_index].help);
        }
    }
}

/* Statically allocate CLI buffer to eliminate overhead of using heap*/
static char cmd_buffer[CLI_MAX_CMD_LENGTH];
static uint32_t cmd_pos;
static uint32_t cmd_cur_pos;

static char cmd_history_buffer[CLI_CMD_HISTORY][CLI_MAX_CMD_LENGTH];
static uint32_t cmd_history_pos;
static uint32_t cmd_history_full;

/**
 * \brief           Clear the command buffer and reset the position
 */
static void clear_cmd_buffer(void) {
    memset(cmd_buffer, 0x0, sizeof(cmd_buffer));
    cmd_pos = 0;
    cmd_cur_pos = 0;
}

/**
 * \brief           Stores the command to history
 */
static void store_command_to_history(void) {
    uint32_t hist_count;
    if (strcmp(cmd_history_buffer[0], cmd_buffer)) {
        for (hist_count = CLI_CMD_HISTORY - 1; hist_count > 0; hist_count--) {
            memcpy(cmd_history_buffer[hist_count], cmd_history_buffer[hist_count - 1], CLI_MAX_CMD_LENGTH);
        }
        cmd_history_full++;
        if (cmd_history_full > CLI_CMD_HISTORY) {
            cmd_history_full = CLI_CMD_HISTORY;
        }
        memcpy(cmd_history_buffer[0], cmd_buffer, CLI_MAX_CMD_LENGTH);
        cmd_history_buffer[0][CLI_MAX_CMD_LENGTH - 1] = '\0';
    }
}

/**
 * \brief           Special key sequence check
 *                      ^[3~ : Delete (TODO)
 *                      ^[A  : Up
 *                      ^[B  : Down
 *                      ^[C  : Right
 *                      ^[D  : Left
 *                      ^[1~ : Home (TODO)
 *                      ^OF  : End (TODO)
 * \param[in]       cliprintf: Pointer to CLI printf function
 * \param[in]       ch: input char from CLI
 * \return          true when special key sequence is active, else false
 */
static bool cli_special_key_check(cli_printf cliprintf, char ch) {
    static uint32_t key_sequence;
    static char last_ch;
    bool special_key_found = false;

    if (key_sequence == 0 && ch == 27) {
        special_key_found = true;
        key_sequence = 1;
    } else if (key_sequence == 1 && (ch == '[' || ch == 'O')) {
        special_key_found = true;
        key_sequence = 2;
    } else if (key_sequence == 2 && ch >= 'A' && ch <= 'D') {
        special_key_found = true;
        key_sequence = 0;
        switch (ch) {
            case 'A': /* Up */
                if (cmd_history_pos < cmd_history_full) {
                    /* Clear the line */
                    memset(cmd_buffer, ' ', cmd_pos);
                    cliprintf("\r%s       \r" CLI_PROMPT, cmd_buffer);

                    strcpy(cmd_buffer, cmd_history_buffer[cmd_history_pos]);
                    cmd_cur_pos = cmd_pos = strlen(cmd_buffer);
                    cliprintf("%s", cmd_buffer);

                    cmd_history_pos++;
                } else {
                    cliprintf("\a");
                }
                break;
            case 'B': /* Down */
                if (cmd_history_pos) {
                    /* Clear the line */
                    memset(cmd_buffer, ' ', cmd_pos);
                    cliprintf("\r%s       \r" CLI_PROMPT, cmd_buffer);

                    if (--cmd_history_pos != 0) {
                        strcpy(cmd_buffer, cmd_history_buffer[cmd_history_pos]);
                        cmd_cur_pos = cmd_pos = strlen(cmd_buffer);
                        cliprintf("%s", cmd_buffer);
                    } else {
                        clear_cmd_buffer();
                    }
                } else {
                    cliprintf("\a");
                }

                break;
            case 'D': /* Left */
                if (cmd_cur_pos > 0) {
                    cliprintf("\b");
                    cmd_cur_pos--;
                }
                break;
            case 'C': /* Right */
                if (cmd_cur_pos < cmd_pos) {
                    cliprintf("%c", cmd_buffer[cmd_cur_pos]);
                    cmd_cur_pos++;
                }
                break;
        }
    } else if (key_sequence == 2 && (ch == 'F')) {
        /* End*/
        /* TODO: for now just return invalid key */
        cliprintf("\a");
    } else if (key_sequence == 2 && (ch == '1' || ch == '3')) {
        /* Home or Delete, we need to check one more character */
        special_key_found = true;
        key_sequence = 3;
    } else if (key_sequence == 3) {
        /* TODO Home and Delete: for now just return invalid key */
        cliprintf("\a");
        special_key_found = true;
    } else {
        /* Unknown sequence */
        key_sequence = 0;
    }

    /* Store the last character */
    last_ch = ch;

    (void)last_ch; /* Prevent compiler warnings */

    return special_key_found;
}

/**
 * \brief           parse and execute the given command
 * \param[in]       cliprintf: Pointer to CLI printf function
 * \param[in]       input: input string to parse
 * \return          `true` when command is found and parsed, else `false`
 */
static bool cli_parse_and_execute_command(cli_printf cliprintf, char* input) {
    const cli_command_t* command;
    char* argv[CLI_MAX_NUM_OF_ARGS];
    uint32_t argc = 0;

    argv[argc] = strtok(input, " ");
    while (argv[argc] != NULL) {
        argv[++argc] = strtok(NULL, " ");
    }

    if ((command = cli_lookup_command(argv[0])) == NULL) {
        cliprintf("Unknown command: %s\n", argv[0]);
        return false;
    } else {
        command->func(cliprintf, argc, argv);
    }

    return true;
}

/**
 * \brief           parse new characters to the CLI
 * \param[in]       cliprintf: Pointer to CLI printf function
 * \param[in]       ch: new character to CLI
 */
void cli_custom_input(cli_printf cliprintf, char ch) {
    static char last_ch;

    if (!cli_special_key_check(cliprintf, ch)) {
        /* Parse the characters only if they are not part of the special key sequence */
        switch (ch) {
            /* Backspace */
            case '\b':
            case 127:
                if (cmd_cur_pos != 0) {
                    cmd_pos--;
                    cmd_cur_pos--;
                    if (cmd_pos > cmd_cur_pos) {
                        memmove(&cmd_buffer[cmd_cur_pos], &cmd_buffer[cmd_cur_pos + 1], cmd_pos - cmd_cur_pos);
                        cmd_buffer[cmd_pos] = '\0';
                        cliprintf("\b%s  \b", &cmd_buffer[cmd_cur_pos]);
                        /* move the cursor to the origin position */
                        for (int i = cmd_cur_pos; i <= cmd_pos; i++) {
                            cliprintf("\b");
                        }
                    } else {
                        cliprintf("\b \b");
                        cmd_buffer[cmd_pos] = '\0';
                    }
                }
                break;
            /* Tab for autocomplete */
            case '\t':
                cli_tab_auto_complete(cliprintf, cmd_buffer, &cmd_pos, (last_ch == '\t'));
                cmd_cur_pos = cmd_pos;
                break;
            /* New line -> new command */
            case '\n':
            case '\r':
                cmd_history_pos = 0;
                if (strlen(cmd_buffer) == 0) {
                    clear_cmd_buffer();
                    cliprintf(CLI_NL CLI_PROMPT);
                    return;
                }

                cliprintf(CLI_NL);
                store_command_to_history();
                cli_parse_and_execute_command(cliprintf, cmd_buffer);

                clear_cmd_buffer();
                cliprintf(CLI_NL CLI_PROMPT);
                break;
            /* All other chars */
            default:
                if (cmd_pos < CLI_MAX_CMD_LENGTH) {
                    /* normal character */
                    if (cmd_cur_pos < cmd_pos) {
                        memmove(&cmd_buffer[cmd_cur_pos + 1], &cmd_buffer[cmd_cur_pos], cmd_pos - cmd_cur_pos);
                        cmd_buffer[cmd_cur_pos] = ch;
                        cliprintf("%s", &cmd_buffer[cmd_cur_pos]);
                        /* move the cursor to new position */
                        for (int i = cmd_cur_pos; i < cmd_pos; i++) {
                            cliprintf("\b");
                        }
                    } else {
                        cmd_buffer[cmd_pos] = ch;
                        cliprintf("%c", ch);
                    }
                    cmd_pos++;
                    cmd_cur_pos++;
                } else {
                    clear_cmd_buffer();
                    cliprintf(CLI_NL "\aERR: Command too long" CLI_NL CLI_PROMPT);
                    return;
                }
        }
    }

    /* Store last character for double tab detection */
    last_ch = ch;
}

#endif /* (CLI_ENABLE && CLI_ENABLE_CUSTOM) */
