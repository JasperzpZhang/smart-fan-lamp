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
 * @brief     Implementation File for TouchPad Module
 * @version   1.0.0
 * @author    Jasper
 * @date      2024-09-26
 */

/**
   modification history
   --------------------
   01a, 26Sep24, Jasper Created
 */

#include "app/include.h"

/* Debug config */
#if GYRO_DEBUG || 1
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* GYRO_DEBUG */
#if GYRO_ASSERT
#undef ASSERT
#define ASSERT(a)                                                                                                      \
    while (1) {                                                                                                        \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                    \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* GYRO_ASSERT */

#define SC7A20_IIC_ADDR 0x18

sc7a20_hdl_t g_sc7a20_hdl;
static void gyro_task(void* parameter);

void
gyro_init(void) {

    sc7a20_init(&g_sc7a20_hdl, SC7A20_IIC_ADDR, iic_read_addr8, iic_write_addr8);

    xTaskCreate(gyro_task, "gyro_task", 128, NULL, tskIDLE_PRIORITY + 1, NULL);
}

static void
gyro_task(void* parameter) {

    uint8_t orientation = 0;
    uint8_t lock = 2;
    int16_t acc[3];
    float angle_z;
    uint16_t color = 0;

    while (1) {

        // if (status_err == sc7a20_get_z_axis_angle(&g_sc7a20_hdl, acc, &angle_z)) //SC7A20读取加速度值
        // {
        //     TRACE("加速度数据读取失败\r\n");
        // } else {
        //     TRACE("x        : %d\n", acc[0]);
        //     TRACE("y        : %d\n", acc[1]);
        //     TRACE("z        : %d\n", acc[2]);
        //     TRACE("angle_z  : %4f\n\n", angle_z);
        // }

        orientation = sc7a20_get_orientation(&g_sc7a20_hdl);

        if (orientation == 1) {
            color = 100;
            if (lock == 2) {
                lock = 1;
                sys_led_set_color_temp(SOURCE_PANEL, color);
            }

        } else if (orientation == 2) {
            color = 0;
            if (lock == 1) {
                lock = 2;
                sys_led_set_color_temp(SOURCE_PANEL, color);
            }
        }

        osDelay(500);
    }
}

/* apds9960 */
#include "drv/peri/apds9960/drv_apds9960_basic.h"
#include "drv/peri/apds9960/drv_apds9960_gesture.h"
#include "drv/peri/apds9960/drv_apds9960_interrupt.h"

/**
 * @brief global var definition
 */
uint8_t g_buf[256];                 /**< uart buffer */
volatile uint16_t g_len;            /**< uart buffer length */
uint8_t (*g_gpio_irq)(void) = NULL; /**< gpio irq */
static volatile uint8_t gs_flag;    /**< flag */

void
gpio_6_exti_cb(uint16_t gpio_pin) {
    TRACE("gesture int.\n");
    if (g_gpio_irq != NULL) {
        g_gpio_irq();
    }
}

/**
 * @brief     gesture callback
 * @param[in] type is the interrupt type
 * @note      none
 */
static void
a_gesture_callback(uint8_t type) {
    switch (type) {
        case APDS9960_INTERRUPT_STATUS_GESTURE_LEFT: {
            apds9960_interface_debug_print("apds9960: irq gesture left.\n");
            gs_flag = 1;

            break;
        }
        case APDS9960_INTERRUPT_STATUS_GESTURE_RIGHT: {
            apds9960_interface_debug_print("apds9960: irq gesture right.\n");
            gs_flag = 1;

            break;
        }
        case APDS9960_INTERRUPT_STATUS_GESTURE_UP: {
            apds9960_interface_debug_print("apds9960: irq gesture up.\n");
            gs_flag = 1;

            break;
        }
        case APDS9960_INTERRUPT_STATUS_GESTURE_DOWN: {
            apds9960_interface_debug_print("apds9960: irq gesture down.\n");
            gs_flag = 1;

            break;
        }
        case APDS9960_INTERRUPT_STATUS_GESTURE_NEAR: {
            apds9960_interface_debug_print("apds9960: irq gesture near.\n");
            gs_flag = 1;

            break;
        }
        case APDS9960_INTERRUPT_STATUS_GESTURE_FAR: {
            apds9960_interface_debug_print("apds9960: irq gesture far.\n");
            gs_flag = 1;

            break;
        }
        case APDS9960_INTERRUPT_STATUS_GFOV: {
            apds9960_interface_debug_print("apds9960: irq gesture fifo overflow.\n");

            break;
        }
        case APDS9960_INTERRUPT_STATUS_GVALID: {
            apds9960_interface_debug_print("apds9960: irq gesture fifo data.\n");

            break;
        }
        case APDS9960_INTERRUPT_STATUS_CPSAT: {
            apds9960_interface_debug_print("apds9960: irq clear photo diode saturation.\n");

            break;
        }
        case APDS9960_INTERRUPT_STATUS_PGSAT: {
            apds9960_interface_debug_print("apds9960: irq analog saturation.\n");

            break;
        }
        case APDS9960_INTERRUPT_STATUS_PINT: {
            apds9960_interface_debug_print("apds9960: irq proximity interrupt.\n");

            break;
        }
        case APDS9960_INTERRUPT_STATUS_AINT: {
            apds9960_interface_debug_print("apds9960: irq als interrupt.\n");

            break;
        }
        case APDS9960_INTERRUPT_STATUS_GINT: {
            break;
        }
        case APDS9960_INTERRUPT_STATUS_PVALID: {
            break;
        }
        case APDS9960_INTERRUPT_STATUS_AVALID: {
            apds9960_interface_debug_print("apds9960: irq als valid.\n");

            break;
        }
        default: {
            apds9960_interface_debug_print("apds9960: irq unknown.\n");

            break;
        }
    }
}

/**
 * @brief     interrupt receive callback
 * @param[in] type is the interrupt type
 * @note      none
 */
static void
a_interrupt_callback(uint8_t type) {
    switch (type) {
        case APDS9960_INTERRUPT_STATUS_GESTURE_LEFT: {
            apds9960_interface_debug_print("apds9960: irq gesture left.\n");

            break;
        }
        case APDS9960_INTERRUPT_STATUS_GESTURE_RIGHT: {
            apds9960_interface_debug_print("apds9960: irq gesture right.\n");

            break;
        }
        case APDS9960_INTERRUPT_STATUS_GESTURE_UP: {
            apds9960_interface_debug_print("apds9960: irq gesture up.\n");

            break;
        }
        case APDS9960_INTERRUPT_STATUS_GESTURE_DOWN: {
            apds9960_interface_debug_print("apds9960: irq gesture down.\n");

            break;
        }
        case APDS9960_INTERRUPT_STATUS_GESTURE_NEAR: {
            apds9960_interface_debug_print("apds9960: irq gesture near.\n");

            break;
        }
        case APDS9960_INTERRUPT_STATUS_GESTURE_FAR: {
            apds9960_interface_debug_print("apds9960: irq gesture far.\n");

            break;
        }
        case APDS9960_INTERRUPT_STATUS_GFOV: {
            apds9960_interface_debug_print("apds9960: irq gesture fifo overflow.\n");

            break;
        }
        case APDS9960_INTERRUPT_STATUS_GVALID: {
            apds9960_interface_debug_print("apds9960: irq gesture fifo data.\n");

            break;
        }
        case APDS9960_INTERRUPT_STATUS_CPSAT: {
            apds9960_interface_debug_print("apds9960: irq clear photo diode saturation.\n");

            break;
        }
        case APDS9960_INTERRUPT_STATUS_PGSAT: {
            apds9960_interface_debug_print("apds9960: irq analog saturation.\n");

            break;
        }
        case APDS9960_INTERRUPT_STATUS_PINT: {
            uint8_t res;
            uint8_t proximity;

            /* read proximity */
            res = apds9960_interrupt_read_proximity((uint8_t*)&proximity);
            if (res != 0) {
                apds9960_interface_debug_print("apds9960: read proximity failed.\n");
            }
            apds9960_interface_debug_print("apds9960: proximity is 0x%02X.\n", proximity);
            gs_flag = 1;

            break;
        }
        case APDS9960_INTERRUPT_STATUS_AINT: {
            uint8_t res;
            uint16_t red, green, blue, clear;

            /* read rgbc */
            res = apds9960_interrupt_read_rgbc((uint16_t*)&red, (uint16_t*)&green, (uint16_t*)&blue, (uint16_t*)&clear);
            if (res != 0) {
                apds9960_interface_debug_print("apds9960: read rgbc failed.\n");
            }
            /* output */
            apds9960_interface_debug_print("apds9960: red is 0x%04X.\n", red);
            apds9960_interface_debug_print("apds9960: green is 0x%04X.\n", green);
            apds9960_interface_debug_print("apds9960: blue is 0x%04X.\n", blue);
            apds9960_interface_debug_print("apds9960: clear is 0x%04X.\n", clear);
            gs_flag = 1;

            break;
        }
        case APDS9960_INTERRUPT_STATUS_GINT: {
            apds9960_interface_debug_print("apds9960: irq gesture interrupt.\n");

            break;
        }
        case APDS9960_INTERRUPT_STATUS_PVALID: {
            break;
        }
        case APDS9960_INTERRUPT_STATUS_AVALID: {
            break;
        }
        default: {
            apds9960_interface_debug_print("apds9960: irq unknown.\n");

            break;
        }
    }
}

void
app_apds9960_init(void) {

    uint8_t times = 3;
    uint8_t res;
    uint32_t i;

    /* set gpio irq */
    g_gpio_irq = apds9960_gesture_irq_handler;

    /* gesture init */
    res = apds9960_gesture_init(a_gesture_callback);

    /* loop */
    gs_flag = 0;
    for (i = 0; i < times; i++) {
        while (1) {
            if (gs_flag != 0) {
                gs_flag = 0;

                /* 1000 ms */
                apds9960_interface_delay_ms(100);

                break;
            } else {
                /* 1000 ms */
                apds9960_interface_delay_ms(100);

                continue;
            }
        }
    }
}
