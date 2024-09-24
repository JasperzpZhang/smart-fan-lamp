/**
 * \file            cli.c
 * \brief           Command line interface
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
 * This file is the entire cli application.
 *
 * Author:          Jasper <jasperzhangse@gmail.com>
 * Version:         v1.0.0-dev
 */

/* Includes */
#include "app/include.h"

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
#define ASSERT(a)                                                  \
    while (!(a))                                                   \
    {                                                              \
        DebugPrintf("ASSERT failed: %s %d\n", __FILE__, __LINE__); \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* CLI_ASSERT */

/* Local types */

/* Forward declaration */
static void prv_cli_uart_task(void *para);
static void prv_cli_uart_printf(const char *cFormat, ...);
status_t prv_cli_uart_recv_cb(uint8_t *pucBuf, uint16_t usLength, void *isr_para);

/* Local variables */
#if CLI_ENABLE_SECURITY
static Bool_t g_cli_en = FALSE;
#else
static Bool_t g_cli_en = TRUE;
#endif /* CLI_ENABLE_SECURITY */
static const char *g_cfg_mark = "admin";
cli_uart_ctrl_t cli_uart_ctrl;

/* Functions */
status_t cli_init(UART_HandleTypeDef *huart, uart_dma_en_t uart_dma_en)
{
    extern const int CliCmdTab$$Base;
    extern const int CliCmdTab$$Limit;

    lib_cli_init();
    cli_custom_init();
    cli_custom_load_cmd((const cli_command_t *)(&CliCmdTab$$Base), ((uint32_t)&CliCmdTab$$Limit - (uint32_t)&CliCmdTab$$Base) / sizeof(cli_command_t));

    cli_uart_ctrl.huart = huart;
    cli_uart_ctrl.uart_dma_en = uart_dma_en;
    if (uart_dma_en == UART_DMA_ENABLE) {
        HAL_UARTEx_ReceiveToIdle_DMA(cli_uart_ctrl.huart, cli_uart_ctrl.uart_rx_buf, CLI_RX_BUF_SIZE);
    }
    else if (uart_dma_en == UART_DMA_DISABLE) {
        HAL_UARTEx_ReceiveToIdle_IT(cli_uart_ctrl.huart, cli_uart_ctrl.uart_rx_buf, CLI_RX_BUF_SIZE);
    }
        
    xTaskCreate(prv_cli_uart_task, "tCli", 256, (void *)&cli_uart_ctrl, tskIDLE_PRIORITY, NULL);

    cli_uart_ctrl.init = TRUE;
    return status_ok;
}

status_t cli_term(void)
{
    /* Do nothing */
    return status_ok;
}

Bool_t cli_is_enabled(void)
{
    return g_cli_en;
}

static void prv_cli_uart_task(void *para)
{
    cli_uart_ctrl_t *pub_ctrl = (cli_uart_ctrl_t *)para;
    
    /* Create the queues used to hold Rx characters. */
    pub_ctrl->rx_queue = xQueueCreate(256, (portBASE_TYPE)sizeof(char));

    /* Read and process data */
    while (1) {
        char chr;
        if (xQueueReceive(pub_ctrl->rx_queue, &chr, portMAX_DELAY) == pdPASS) {
            cli_custom_input(prv_cli_uart_printf, chr);
        }
    }
}

static void prv_cli_uart_printf(const char *cFormat, ...)
{
    cli_uart_ctrl_t *pub_ctrl = &cli_uart_ctrl;

    if (NULL != pub_ctrl) {
        va_list va;
        va_start(va, cFormat);
        uint16_t length = vsprintf(pub_ctrl->fmt_buf, cFormat, va);
        if (length) {
            HAL_UART_Transmit(pub_ctrl->huart, (uint8_t *)pub_ctrl->fmt_buf, length, 1000);
        }
    }
    return;
}

status_t prv_cli_uart_recv_cb(uint8_t *buffer, uint16_t length, void *isr_para)
{
    cli_uart_ctrl_t *pub_ctrl = (cli_uart_ctrl_t *)isr_para;
    ASSERT(NULL != pub_ctrl);

    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    for (uint16_t n = 0; n < length; n++) {
        xQueueSendFromISR(pub_ctrl->rx_queue, buffer + n, &xHigherPriorityTaskWoken);
    }

    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    return status_ok;
}

void uart1_rx_event_callback(UART_HandleTypeDef *huart, uint16_t size) {
    prv_cli_uart_recv_cb(cli_uart_ctrl.uart_rx_buf, size, &cli_uart_ctrl);
    if (cli_uart_ctrl.uart_dma_en == UART_DMA_ENABLE) {
        HAL_UARTEx_ReceiveToIdle_DMA(cli_uart_ctrl.huart, cli_uart_ctrl.uart_rx_buf, CLI_RX_BUF_SIZE);
    }
    else if (cli_uart_ctrl.uart_dma_en == UART_DMA_DISABLE) {
        HAL_UARTEx_ReceiveToIdle_IT(cli_uart_ctrl.huart, cli_uart_ctrl.uart_rx_buf, CLI_RX_BUF_SIZE);
    }
}

static void prv_cli_cmd_encli(cli_printf cliprintf, int argc, char **argv)
{
    if (3 == argc) {
        int lEnCli;
#if CLI_ENABLE_SECURITY
        if (0 != strcmp(argv[1], g_cfg_mark)) {
            cliprintf("CLI error: wrong config mark\n");
            return;
        }
        
#endif /* CLI_ENABLE_SECURITY */
        lEnCli = atoi(argv[2]);
        if (lEnCli > 0) {
            g_cli_en = TRUE;
            cliprintf("Enable cli ok\n");
        }
        else {
            g_cli_en = FALSE;
            cliprintf("Disable cli ok\r\n");
        }
    }
    else {
        cliprintf("CLI error: wrong parameters\r\n");
    }
    return;
}
CLI_CMD_EXPORT(encli, enable cli, prv_cli_cmd_encli)

static void prv_cli_cmd_reset(cli_printf cliprintf, int argc, char **argv)
{
    CHECK_CLI();

    NVIC_SystemReset();
}
CLI_CMD_EXPORT(reset, system reset, prv_cli_cmd_reset)

#if 0
static void prvCliCmdUpgrade(cli_printf cliprintf, int argc, char **argv)
{
    CHECK_CLI();

    uint32_t ulUpdateMark = 0x1234ABCD;
    mem_flash_write(0, sizeof(ulUpdateMark), (uint8_t *)&ulUpdateMark);

    cliprintf("enter upgrade mode after system reset\n");
}
CLI_CMD_EXPORT(upgrade, enter upgrade mode after system reset, prvCliCmdUpgrade)
#endif
