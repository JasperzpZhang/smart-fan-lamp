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
#if WAVE_DEBUG || 0
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

/* global defines */
QueueHandle_t g_queue_voice;
static msg_voice_t g_msg_voice;

/* forward functions */
void voice_task(void* para);
status_t voice_proc(msg_voice_t msg);

/* functions */
status_t
voice_init(void) {
    g_queue_voice = xQueueCreate(3, sizeof(g_msg_voice));
    HAL_UARTEx_ReceiveToIdle_IT(&huart3, g_msg_voice.buf, 10);
    xTaskCreate(voice_task, "voice task", 1024, NULL, tskIDLE_PRIORITY + 2, NULL);
    return status_ok;
}

void
voice_task(void* para) {
    msg_voice_t msg_voice;
    while (1) {
        if (xQueueReceive(g_queue_voice, &msg_voice, portMAX_DELAY) == pdPASS) {
            uint8_t i;
            TRACE("size    : %d\n", msg_voice.size);
            TRACE("msg_buf : ");
            for (i = 0; i < msg_voice.size; i++) {
                TRACE("%x ", msg_voice.buf[i]);
            }
            TRACE("\n");
        }
        voice_proc(msg_voice);
        if (HAL_UARTEx_ReceiveToIdle_IT(&huart3, g_msg_voice.buf, 10) == HAL_ERROR) {
            TRACE("HAL_ERROR\n");
        } else {
            TRACE("HAL_OK\n");
        }
        osDelay(5);
    }
}

status_t
voice_proc(msg_voice_t msg) {

    if (msg.buf[0] != 0xAA) {
        TRACE("error 0xAA\n");
        return status_err;
    }

    if (msg.buf[1] != 0x01) {
        TRACE("error 0x01\n");
        return status_err;
    }

    if (msg.buf[3] != 0xBB) {
        TRACE("error 0xBB\n");
        return status_err;
    }

    // TRACE("msg.buf[2] : %d\n", msg.buf[2]);

    switch (msg.buf[2]) {
        case (uint8_t)0x01:
            /* Turn on the main light */
            led_set_status(1);
            break;

        case (uint8_t)0x02:
            /* Turn off the main light */
            led_set_status(0);
            break;

        case (uint8_t)0x03:
            /* Turn on the night light */
            night_light_set_status(1);
            break;

        case (uint8_t)0x04:
            /* Turn off the night light */
            night_light_set_status(0);
            break;

        case (uint8_t)0x05:
            /* Restore normal lighting */
            break;

        case (uint8_t)0x06:
            /* Set brightness to 10% */
            led_set_brightness_smooth_blk(10);
            break;

        case (uint8_t)0x07:
            /* Set brightness to 20% */
            led_set_brightness_smooth_blk(20);
            break;

        case (uint8_t)0x08:
            /* Set brightness to 30% */
            led_set_brightness_smooth_blk(30);
            break;

        case (uint8_t)0x09:
            /* Set brightness to 40% */
            led_set_brightness_smooth_blk(40);
            break;

        case (uint8_t)0x0A:
            /* Set brightness to 50% */
            led_set_brightness_smooth_blk(50);
            break;

        case (uint8_t)0x0B:
            /* Set brightness to 60% */
            led_set_brightness_smooth_blk(60);
            break;

        case (uint8_t)0x0C:
            /* Set brightness to 70% */
            led_set_brightness_smooth_blk(70);
            break;

        case (uint8_t)0x0D:
            /* Set brightness to 80% */
            led_set_brightness_smooth_blk(80);
            break;

        case (uint8_t)0x0E:
            /* Set brightness to 90% */
            led_set_brightness_smooth_blk(90);
            break;

        case (uint8_t)0x0F:
            /* Set brightness to 100% */
            led_set_brightness_smooth_blk(100);
            break;

        case (uint8_t)0x10:
            /* Set color temperature to 10% */
            led_set_color_temperature_smooth_blk(10);
            break;

        case (uint8_t)0x11:
            /* Set color temperature to 20% */
            led_set_color_temperature_smooth_blk(20);
            break;

        case (uint8_t)0x12:
            /* Set color temperature to 30% */
            led_set_color_temperature_smooth_blk(30);
            break;

        case (uint8_t)0x13:
            /* Set color temperature to 40% */
            led_set_color_temperature_smooth_blk(40);
            break;

        case (uint8_t)0x14:
            /* Set color temperature to 50% */
            led_set_color_temperature_smooth_blk(50);
            break;

        case (uint8_t)0x15:
            /* Set color temperature to 60% */
            led_set_color_temperature_smooth_blk(60);
            break;

        case (uint8_t)0x16:
            /* Set color temperature to 70% */
            led_set_color_temperature_smooth_blk(70);
            break;

        case (uint8_t)0x17:
            /* Set color temperature to 80% */
            led_set_color_temperature_smooth_blk(80);
            break;

        case (uint8_t)0x18:
            /* Set color temperature to 90% */
            led_set_color_temperature_smooth_blk(90);
            break;

        case (uint8_t)0x19:
            /* Set color temperature to 100% */
            led_set_color_temperature_smooth_blk(100);
            break;

        case (uint8_t)0x1A:
            /* Turn on warm light mode */
            led_set_color_temperature_smooth_blk(0);
            break;

        case (uint8_t)0x1B:
            /* Turn on winter mode */
            led_set_color_temperature_smooth_blk(0);
            break;

        case (uint8_t)0x1C:
            /* Turn on cool light mode */
            led_set_color_temperature_smooth_blk(100);
            break;

        case (uint8_t)0x1D:
            /* Turn on summer mode */
            led_set_color_temperature_smooth_blk(100);
            break;

        case (uint8_t)0x1E:
            /* Turn on sunlight mode */
            led_set_status(1);
            fan_set_status(1);
            led_set_brightness_smooth_blk(100);
            led_set_color_temperature_smooth_blk(50);
            break;

        case (uint8_t)0x1F:
            /* Turn on tomato light mode */
            break;

        case (uint8_t)0x20:
            /* Turn on reading mode */
            break;

        case (uint8_t)0x21:
            /* Turn on holiday mode */
            break;

        case (uint8_t)0x22:
            /* Turn on sleep mode */
            led_set_status(0);
            fan_set_status(0);
            break;

        case (uint8_t)0x23:
            /* Turn on charging interface */
            HAL_GPIO_WritePin(USB_SW_GPIO_PORT, USB_SW_GPIO_PIN, GPIO_PIN_SET);
            break;

        case (uint8_t)0x24:
            /* Turn off charging interface */
            HAL_GPIO_WritePin(USB_SW_GPIO_PORT, USB_SW_GPIO_PIN, GPIO_PIN_RESET);
            break;

        case (uint8_t)0x25:
            /* Turn on fan */
            fan_set_status(1);
            break;

        case (uint8_t)0x26:
            /* Turn off fan */
            fan_set_status(0);
            break;

        case (uint8_t)0x27:
            /* Set fan speed to level 1 */
            fan_set_level(1);
            break;

        case (uint8_t)0x28:
            /* Set fan speed to level 2 */
            fan_set_level(2);
            break;

        case (uint8_t)0x29:
            /* Set fan speed to level 3 */
            fan_set_level(3);
            break;

        case (uint8_t)0x2A:
            /* Turn on seat occupancy sensor */
            break;

        case (uint8_t)0x2B:
            /* Turn off seat occupancy sensor */
            break;

        case (uint8_t)0x2C:
            /* Enter timer mode */
            break;

        case (uint8_t)0x2D:
            /* Start 5-minute timer */
            break;

        case (uint8_t)0x2E:
            /* Start 10-minute timer */
            break;

        case (uint8_t)0x2F:
            /* Start 30-minute timer */
            break;

        case (uint8_t)0x30:
            /* Start 60-minute timer */
            break;

        case (uint8_t)0x31:
            /* Start 2-hour timer */
            break;

        case (uint8_t)0x32:
            /* Start 4-hour timer */
            break;

        default: break;
    }

    return status_ok;
}

void
uart3_rx_event_callback(UART_HandleTypeDef* huart, uint16_t size) {
    g_msg_voice.size = size;
    xQueueSendFromISR(g_queue_voice, &g_msg_voice, NULL);
    memset(g_msg_voice.buf, 0, size);
    g_msg_voice.size = 0;
}

void
uart3_rx_cplt_callback(UART_HandleTypeDef* huart) {
    TRACE("uart3_rx_cplt_callback \n");
    HAL_UARTEx_ReceiveToIdle_IT(&huart3, g_msg_voice.buf, 10);
}

void
uart3_error_callback(UART_HandleTypeDef* huart) {
    TRACE("uart3_error_callback \n");
    HAL_UARTEx_ReceiveToIdle_IT(&huart3, g_msg_voice.buf, 10);
}
