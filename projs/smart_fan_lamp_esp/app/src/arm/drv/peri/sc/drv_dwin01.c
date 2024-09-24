/**
 * \file            drv_dwin01.c
 * \brief           Dwin01 driver file
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
 * This file is part of the dwin01 driver.
 *
 * Author:          Jasper <jasperzhangse@gmail.com>
 * Version:         v1.0.0-dev
 * Last edit:       2024-06-18
 */

#include "stm32f1xx_hal.h"
#include "drv/screen/drv_dwin01.h"
#include "lib/uart/lib_uart.h"
#include "lib/debug/lib_debug.h"
#include "app/arm_application/user/sys.h"
#include "app/arm_application/user/drv/led.h"
#include "string.h"
#include "usart.h"
#include "lib/cli/lib_cli.h"
#include "stdlib.h"
#include "app/arm_application/user/drv/data.h"

/* Debug config */
#if SCREEN_DEBUG
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* SCREEN_DEBUG */
#if SCREEN_ASSERT
#undef ASSERT
#define ASSERT(a)                                                   \
    while (!(a))                                                    \
    {                                                               \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__); \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* SCREEN_ASSERT */



/* global variable */
uart_ctrl_t g_screen_ctrl;
uint8_t rx_buffer[10] = {0};
uint8_t tx_buffer[10] = {0};

QueueHandle_t xQueue; /* 声明消息队列句柄 */

/* forward function */
status_t prv_dmg_screen_uart_receive_callback(uint8_t *rx_buffer, uint16_t rx_buffer_length, void *isr_parameter);
status_t prv_dmg_screen_uart_receive_process(uint8_t *rx_buffer, uint16_t rx_buffer_length);
status_t prv_set_dwin_head(void);
status_t upgrade_screen_data(void);


status_t dwin01_init(UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma_rx, DMA_HandleTypeDef *hdma_tx)
{
    uart_config_callback(&g_screen_ctrl, prv_dmg_screen_uart_receive_callback, uart_isr_callback, NULL, NULL, NULL);
    uart_config_handle(&g_screen_ctrl, huart, hdma_rx, hdma_tx);
    prv_set_dwin_head();
    upgrade_screen_data();
    // 创建消息队列
    xQueue = xQueueCreate(10, sizeof(Message));
    return status_ok;
}

status_t upgrade_screen_data(void)
{
    refresh_screen(SET_LED_BR_ADDR, th_led_brightness / 10);
    refresh_screen(SET_LED_CT_ADDR, th_led_color_temperature / 10);
    refresh_screen(SET_LED_ONOFF_ADDR, th_led_status);
    refresh_screen(SET_FAN_ONOFF_ADDR, th_fan_status);
    
    uint8_t level = th_fan_speed;
    if (level < 55)
        level = 1;
    else if (level < 65)
        level = 2;
    else
        level = 3;
    refresh_screen(SET_FAN_LEVEL_ADDR, level);

    return status_ok;
}

status_t prv_dmg_screen_uart_receive_callback(uint8_t *rx_buffer, uint16_t rx_buffer_length, void *isr_parameter)
{
    /* process data */
    prv_dmg_screen_uart_receive_process(rx_buffer, rx_buffer_length);
    return status_ok;
}

status_t prv_dmg_screen_uart_receive_process(uint8_t *rx_buffer, uint16_t rx_buffer_length)
{

#if RX_BUFFER_PTINT
    TRACE("rx_buffer : ");
    for (uint8_t i = 0; i < rx_buffer_length; i++)
    {
        TRACE("%x ", rx_buffer[i]);
    }
    TRACE("%d", rx_buffer[7]);
    TRACE("\r\n");
#endif

    /* 发送消息到sys任务 */
    Message msg;
    memcpy(msg.data, rx_buffer, rx_buffer_length);
    msg.length = rx_buffer_length;

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(xQueue, &msg, &xHigherPriorityTaskWoken);

    /* 如果发送消息导致更高优先级的任务被唤醒，则执行任务切换 */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

    return status_ok;
}

status_t prv_set_dwin_head(void)
{
    tx_buffer[0] = 0x5A;
    tx_buffer[1] = 0xA5;
    tx_buffer[3] = 0x82;
    return status_ok;
}

status_t refresh_screen(uint16_t address, uint8_t value)
{
    tx_buffer[4] = address >> 8;
    tx_buffer[5] = address & 0xFF;
    
    switch (address)
    {
        /* system */
        case SET_BRIGHTNESS:
            tx_buffer[2] = 0x04;
            tx_buffer[6] = value;
            tx_buffer[7] = 0x00;
        break;
        
        /* user */
        case SET_LED_BR_ADDR:
        case SET_LED_CT_ADDR:
        case SET_FAN_LEVEL_ADDR:
            tx_buffer[2] = 0x05;
            tx_buffer[6] = value;
            tx_buffer[7] = 0x00;
            TRACE("progress bar run\r\n");
        break;
        
        case SET_LED_ONOFF_ADDR:
        case SET_FAN_ONOFF_ADDR:
            tx_buffer[2] = 0x05;
            tx_buffer[6] = 0x00;
            tx_buffer[7] = value;
            TRACE("button run\r\n");
        break;
        
        default:
            TRACE("address error\r\n");
    }
    
    TRACE("tx_buffer = ");
    for (uint8_t i = 0; i < tx_buffer[2] + 3; i++)
    {
        TRACE("%x ", tx_buffer[i]);
    }
    TRACE("\r\n");
    uart_block_send(&g_screen_ctrl, tx_buffer, tx_buffer[2] + 3, 10);

    return status_ok;
}

static void cli_refresh_screen(cli_printf cliprintf, int argc, char **argv)
{
    uint8_t cmd_length = 0;
    uint8_t *buffer = prvParseHexStr(argv[1], &cmd_length);

    refresh_screen(buffer[0] << 8 | buffer[1], buffer[2]);
    
    cliprintf("screen cmd send ok\r\n");
    
}
CLI_CMD_EXPORT(refresh_screen, refresh screen, cli_refresh_screen)

static void cli_screen_cmd_send(cli_printf cliprintf, int argc, char **argv)
{
    uint8_t cmd_length = 0;
    uint8_t *buffer = prvParseHexStr(argv[1], &cmd_length);
    
    cliprintf("buffer = ");
    for (uint8_t i = 0; i < cmd_length; i++)
    {
        cliprintf("%x ", *(buffer + i));
    }
    cliprintf("\r\n");
    uart_block_send(&g_screen_ctrl, buffer, cmd_length, 10);
    
    cliprintf("screen cmd send ok\r\n");
    
}
CLI_CMD_EXPORT(screen_cmd_send, send screen cmd, cli_screen_cmd_send)

#if 1
static uint8_t *prvParseHexStr(const char *pcStr, uint8_t *pucLength)
{
    static uint8_t ucBuffer[80];
    uint8_t ucData = 0;
    uint8_t ucCnt = 0;
    uint8_t ucIndex = 0;
    Bool_t bProc = FALSE;
    while ((0 != *pcStr) && (ucIndex < 80))
    {
        char cTmp;
        cTmp = *pcStr++;
        if ((cTmp >= 'a') && (cTmp <= 'f'))
        {
            ucData *= 16;
            ucData += 10 + (cTmp - 'a');
            ucCnt++;
            bProc = TRUE;
        }
        if ((cTmp >= 'A') && (cTmp <= 'F'))
        {
            ucData *= 16;
            ucData += 10 + (cTmp - 'A');
            ucCnt++;
            bProc = TRUE;
        }
        if ((cTmp >= '0') && (cTmp <= '9'))
        {
            ucData *= 16;
            ucData += cTmp - '0';
            ucCnt++;
            bProc = TRUE;
        }
        if ((TRUE == bProc) && ((' ' == cTmp) || (',' == cTmp) || ('|' == cTmp) || (2 == ucCnt) || (0 == *pcStr)))
        {
            ucBuffer[ucIndex++] = ucData;
            ucData = 0;
            ucCnt = 0;
            bProc = FALSE;
        }
    }
    *pucLength = ucIndex;
    return ucBuffer;
}
#endif


