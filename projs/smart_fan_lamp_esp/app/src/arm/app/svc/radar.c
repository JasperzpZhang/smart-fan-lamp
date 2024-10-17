/**
 * Copyright (c) 2024 Jasper All rights reserved
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 *
 * @file      tp.c
 * @brief     Implementation File for wave Module
 * @version   1.0.0
 * @author    Jasper
 * @date      2024-09-28
 */

/**
   modification history
   --------------------
   01a, 28Sep24, Jasper Created
 */

#include "app/include.h"

/* Debug config */
#if RADAR_DEBUG || 0
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* WAVE_DEBUG */
#if WAVE_ASSERT
#undef ASSERT
#define ASSERT(a)                                                                                                      \
    while (1) {                                                                                                        \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                    \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* WAVE_ASSERT */

/* local defines */

radar_ctrl_t g_radar_ctrl;

/* Private function prototypes -----------------------------------------------*/
void radar_task(void* para);
status_t radar_send(uint8_t* cmd);
status_t radar_set(void);
status_t radar_strategy(void);

status_t
radar_init(void) {
    // radar_set();
    g_radar_ctrl.queue = xQueueCreate(RADAR_RX_BUF_SIZE, sizeof(char));
    xTaskCreate(radar_task, "radar task", 128, NULL, tskIDLE_PRIORITY + 1, NULL);
    g_radar_ctrl.radar_delay.hour = th_radar_delay.hour;
    g_radar_ctrl.radar_delay.min = th_radar_delay.min;
    g_radar_ctrl.radar_delay.sec = th_radar_delay.sec;
    g_radar_ctrl.radar_strategy_en = th_radar_strategy_en;
#if RADAR_MODE_INFO
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, g_radar_ctrl.msg.buf, RADAR_RX_BUF_SIZE);
#endif
    return status_ok;
}

void
radar_task(void* para) {
    // radar_msg_t msg_radar;
#if RADAR_MODE_INFO
    char chr;
#endif
    while (1) {
#if RADAR_MODE_INFO
        if (xQueueReceive(g_radar_ctrl.queue, &chr, 0) == pdPASS) {
            TRACE("%c", chr);
        }
#else
        if (g_radar_ctrl.radar_strategy_en != 0) {
            radar_strategy();
        }
        osDelay(1000);
#endif
    }
}

status_t
radar_strategy(void) {
    static time_hdl_t time_radar_hdl = TIME_HDL_INIT;
    static uint8_t radar_led_sw_lock = 0, radar_timing_sw_lock = 0;

    if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6) != 0) {
        TRACE("gpio_up\n");
        if (radar_led_sw_lock == 0) {
            TRACE("gpio_timing_start\n");
            timing_start(&time_radar_hdl, g_radar_ctrl.radar_delay.hour, g_radar_ctrl.radar_delay.min,
                         g_radar_ctrl.radar_delay.sec);
            radar_led_sw_lock = 1;
        }
    } else {
        TRACE("gpio_down\n");
        if (radar_led_sw_lock == 1) {
            radar_led_sw_lock = 0;
        }
    }

    if (is_timing_remaining(&time_radar_hdl) != 0) {
        TRACE("timing_start\n");
        TRACE("reamining_time : %d\n", time_radar_hdl.remaining_time);
        if (radar_timing_sw_lock == 0) {
            TRACE("timing_led_start\n");
            radar_timing_sw_lock = 1;
            sys_led_on(SOURCE_PANEL);
            sys_fan_on(SOURCE_PANEL);
        }
    } else {
        TRACE("timing_stop\n");
        if (radar_timing_sw_lock == 1) {
            TRACE("gpio_timing_stop\n");
            radar_timing_sw_lock = 0;
            sys_led_off(SOURCE_PANEL);
            sys_fan_off(SOURCE_PANEL);
        }
    }

    return status_ok;
}

status_t
radar_set(void) {
    radar_send((uint8_t*)"#set dl 5\n");
    osDelay(10);
    radar_send((uint8_t*)"#set rg 100\n");
    osDelay(10);
    radar_send((uint8_t*)"#set pt 0\n");
    osDelay(10);
    radar_send((uint8_t*)"#get al\n");
    osDelay(10);
    return status_ok;
}

status_t
radar_send(uint8_t* cmd) {
    /* Calculate the length of the string, excluding the '\0' */
    uint16_t len = strlen((const char*)cmd);
    // TRACE("len : %d\n", len);

    /* Use HAL_UART_Transmit_IT to send data in non-blocking mode */
    HAL_StatusTypeDef uart_status = HAL_UART_Transmit(&huart2, cmd, len, 0xffff);

    /* Return status based on UART transmission result */
    if (uart_status == HAL_OK) {
        return status_ok;
    } else {
        return status_err;
    }
}

static void
cli_cmd_redar_send_cmd(cli_printf cliprintf, int argc, char** argv) {
    char cmd_buffer[20] = {0}; // Buffer to store the final command string
    int total_length = 0;

    // Iterate over the arguments starting from argv[1]
    for (int i = 1; i < argc; i++) {
        int arg_length = strlen(argv[i]);

        // Check if adding this argument would exceed the buffer size
        if (total_length + arg_length + 1 >= 20) {
            cliprintf("Error: Command too long!\n");
            return;
        }

        // Append the argument to the buffer
        strcat(cmd_buffer, argv[i]);
        total_length += arg_length;

        // Add a space after each argument, except the last one
        if (i < argc - 1) {
            strcat(cmd_buffer, " ");
            total_length += 1;
        }
    }

    strcat(cmd_buffer, "\n");
    total_length += 1;

    // Now cmd_buffer contains the full command string
    cliprintf("Radar command:%s\n", cmd_buffer);
    radar_send(cmd_buffer);
}
CLI_CMD_EXPORT(radar_send, radar send cmd, cli_cmd_redar_send_cmd)

static void
cli_cmd_cfg_set_radar_delay(cli_printf cliprintf, int argc, char** argv) {
    if (argc == 4) {
        uint8_t hour = atoi(argv[1]);
        uint8_t min = atoi(argv[2]);
        uint8_t sec = atoi(argv[3]);

        if (hour > 24 || min > 60 || sec > 60) {
            cliprintf("hour : 0 - 24\n");
            cliprintf("min  : 0 - 60\n");
            cliprintf("sec  : 0 - 60\n");
            cliprintf("\n");
            return;
        }
        th_radar_delay.hour = hour;
        th_radar_delay.min = min;
        th_radar_delay.sec = sec;
        g_radar_ctrl.radar_delay.hour = hour;
        g_radar_ctrl.radar_delay.min = min;
        g_radar_ctrl.radar_delay.sec = sec;
        data_save_direct();

        cliprintf("cfg_set_radar_delay %d %d %d ok\n", g_radar_ctrl.radar_delay.hour, g_radar_ctrl.radar_delay.min,
                  g_radar_ctrl.radar_delay.sec);
    } else {
        cliprintf("error parameter\n\n");
        cliprintf("cfg_set_radar_delay hour mon sec\n");
    }
}
CLI_CMD_EXPORT(cfg_set_radar_delay, radar set delay, cli_cmd_cfg_set_radar_delay)

static void
cli_cmd_cfg_set_radar_strategy_en(cli_printf cliprintf, int argc, char** argv) {
    if (argc != 2) {
        cliprintf("error parameter\n\n");
        cliprintf("cfg_set_radar_en EN\n");
        return;
    }

    int radar_en = atoi(argv[1]);

    if (radar_en >= 0) {
        g_radar_ctrl.radar_strategy_en = 1;
    } else {
        g_radar_ctrl.radar_strategy_en = 0;
    }
    th_radar_strategy_en = g_radar_ctrl.radar_strategy_en;
    data_save_direct();
    cliprintf("cfg_set_radar_en %d ok\n", g_radar_ctrl.radar_strategy_en);
}
CLI_CMD_EXPORT(cfg_set_radar_en, radar set enable, cli_cmd_cfg_set_radar_strategy_en)

/**
  * @brief  UART2 Reception Event Callback (Rx event notification).
  *         This function is called after data is received using advanced reception service.
  * @param  huart UART handle
  * @param  size  Number of bytes received in the reception buffer.
  *               Indicates the position in the buffer until which data is available.
  * @retval None
  */
void
uart2_rx_event_callback(UART_HandleTypeDef* huart, uint16_t size) {

    /* Update the received size */
    g_radar_ctrl.msg.size = size;

    /* Variable to hold higher priority task notification status */
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // /* Send the data to the queue using FromISR function */
    // if (xQueueSendFromISR(g_radar_ctrl.queue, g_radar_ctrl.msg.buf, &xHigherPriorityTaskWoken) != pdPASS) {
    //     /* Handle error if queue is full or data could not be sent */
    //     TRACE("radar queue send from isr to task is error.\n");
    // }

    uint8_t i;
    for (i = 0; i < size; i++) {
        /* Send the data to the queue using FromISR function */
        if (xQueueSendFromISR(g_radar_ctrl.queue, (g_radar_ctrl.msg.buf + i), &xHigherPriorityTaskWoken) != pdPASS) {
            /* Handle error if queue is full or data could not be sent */
            TRACE("radar queue send from isr to task is error.\n");
        }
    }

    memset(g_radar_ctrl.msg.buf, 0, RADAR_RX_BUF_SIZE);

    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, g_radar_ctrl.msg.buf, RADAR_RX_BUF_SIZE);

    /* If sending to the queue caused a task to unblock and have a higher priority than the current task, yield */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void
uart2_error_callback(UART_HandleTypeDef* huart) {
    TRACE("uart2_error_callback \n");
    memset(g_radar_ctrl.msg.buf, 0, RADAR_RX_BUF_SIZE);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, g_radar_ctrl.msg.buf, RADAR_RX_BUF_SIZE);
}
