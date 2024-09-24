/**
 * \file            drv_mhcwb8g.c
 * \brief           Xiaomi MHCWB8G driver file
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
 * This file is part of the Xioami MHCWB8G driver.
 *
 * Author:          Jasper <jasperzhangse@gmail.com>
 * Version:         v1.0.0-dev
 * Last edit:       2024-06-18
 */

#include "stm32f1xx_hal.h"
#include "drv/wifi/drv_mhcwb8g.h"
#include "lib/uart/lib_uart.h"
#include "lib/debug/lib_debug.h"
#include "app/arm_application/define.h"
#include "lib/cli/lib_cli.h"
#include "string.h"
#include "stdio.h"
#include "app/arm_application/define.h"
#include "tim.h"
#include "drv/screen/drv_screen.h"
#include "drv/screen/drv_dwin01.h"
#include "cmsis_os.h"
#include "app/arm_application/user/drv/data.h"

/* Debug config */
#if WIFI_DEBUG
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* WIFI_DEBUG */
#if WIFI_ASSERT
#undef ASSERT
#define ASSERT(a)                                                   \
    while (!(a))                                                    \
    {                                                               \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__); \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* WIFI_ASSERT */

/* Local defines */


/* global variable */
uart_ctrl_t g_wifi_ctrl;

/* create wifi uart message queue */
QueueHandle_t wifi_uart_queue;

/* forward function */
status_t prv_mhcwb8g_uart_receive_callback(uint8_t *rx_buffer, uint16_t rx_buffer_length, void *isr_parameter);
status_t prv_mhcwb8g_uart_receive_process(uint8_t *rx_buffer, uint16_t rx_buffer_length);
//status_t handle_set_properties(int var_index, int attr_index, uint8_t *value);

/* Functions */
status_t mhcwb8g_init(UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma_rx, DMA_HandleTypeDef *hdma_tx)
{
    uart_config_callback(&g_wifi_ctrl, prv_mhcwb8g_uart_receive_callback, uart_isr_callback, uart_dma_rx_isr_callback, uart_dma_tx_isr_callback, NULL);
    uart_config_handle(&g_wifi_ctrl, huart, hdma_rx, hdma_tx);
    HAL_GPIO_WritePin(WIFI_XIAOMI_EN_PORT, WIFI_XIAOMI_EN_PIN, WIFI_XIAOMI_EN_OFF);
    
    wifi_uart_queue = xQueueCreate(10, sizeof(wifi_msg_t));
    
    wifi_send_cmd("model axlon.light.lamp\r\n");
    osDelay(1);
    wifi_send_cmd("ble_config set 20624 0001\r\n");
    osDelay(1);
    wifi_send_cmd("mcu_version 0001\r\n");
    osDelay(1);
    wifi_send_cmd("ble_config dump\r\n");
    

    
    return status_ok;
}

status_t prv_mhcwb8g_uart_receive_callback(uint8_t *rx_buffer, uint16_t rx_buffer_length, void *isr_parameter)
{
    /* process data */
    // prv_mhcwb8g_uart_receive_process(rx_buffer, rx_buffer_length);
    
    TRACE("wifi rx_buffer : %s\r\n", rx_buffer);
    wifi_msg_t msg;

    const char *target_cmd = "down set_properties";
    const char *get_cmd = "down get_properties";
    /* find substring */
    char *match = strstr((const char *)rx_buffer, target_cmd);
    if (match != NULL)
    {
        TRACE("down set_properties matched\r\n");
        uint16_t cmd_len = strlen(target_cmd);
        match += cmd_len + 1; /* Delete the substring; +1 is to delete a blank */
        
        memcpy(msg.data, match, rx_buffer_length - cmd_len);
        msg.length = rx_buffer_length;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(wifi_uart_queue, &msg, &xHigherPriorityTaskWoken);
        /* 如果发送消息导致更高优先级的任务被唤醒，则执行任务切换 */
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    
    match = strstr((const char *)rx_buffer, get_cmd);
    if (match != NULL)
    {
        wifi_send_cmd("result\r\n");
    }

    return status_ok;
}

status_t wifi_send_cmd(const char *cmd)
{
    if (cmd == NULL)
    {
        return status_err;
    }

    size_t cmd_len = strlen(cmd);
    if (cmd_len == 0)
    {
        return status_err;
    }

    uart_block_send(&g_wifi_ctrl, (uint8_t *)cmd, cmd_len, 10);
    return status_ok;
}



static void cli_wifi_cmd(cli_printf cliprintf, int argc, char **argv)
{
    // 定义最大字符串数量和最大缓冲区长度
    const int max_args = 4; // argv[1] 到 argv[4]
    const int max_len = 64; // 假设一个合理的最大长度

    // 静态分配缓冲区
    char buffer[max_len];
    size_t total_len = 0;

    // 初始化缓冲区为空字符串
    buffer[0] = '\0';

    // 拼接所有字符串
    for (int i = 1; i < argc && i <= max_args; ++i)
    {
        size_t arg_len = strlen(argv[i]);
        // 检查是否会超出缓冲区长度
        if (total_len + arg_len >= max_len - 2)
        { // -2 to leave space for \r\n
            cliprintf("Total input length is too long\r\n");
            return;
        }
        strcat(buffer, argv[i]);
        if (i < argc - 1)
        {
            strcat(buffer, " ");
        }
        total_len += arg_len + 1;
    }

    // 添加 \r\n 到缓冲区末尾
    strcat(buffer, "\r\n");
    total_len += 2;

    cliprintf("%s\r\n", buffer);

    // 发送缓冲区内容
    uart_block_send(&g_wifi_ctrl, (uint8_t *)buffer, total_len, 10);

    // cliprintf("wifi cmd send ok\r\n");
}
CLI_CMD_EXPORT(wifi_cmd, send wifi cmd, cli_wifi_cmd)
