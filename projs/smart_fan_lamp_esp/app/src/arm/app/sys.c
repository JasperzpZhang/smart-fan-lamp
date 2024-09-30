/**
 * \file            sys.c
 * \brief           System file
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
 * This file includes all the app functions for system.
 *
 * Author:          Jasper <jasperzhangse@gmail.com>
 * Version:         v1.0.0-dev
 */

#include "app/include.h"

#include "drv/peri/sc/lcd_1in83/drv_lcd_1in83.h"

/* Debug config */
#if SYS_DEBUG || 1
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* SYS_DEBUG */
#if SYS_ASSERT
#undef ASSERT
#define ASSERT(a)                                                                                                      \
    while (1) {                                                                                                        \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                    \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* SYS_ASSERT */

/* Local defines */

#define SYS_TASK_DELAY_TIME 300

/* global variable */

/* Forward functions */
static void sys_task(void* parameter);

/* Functions */
status_t
sys_init(void) {

    HAL_TIM_PWM_Start(&htim10, TIM_CHANNEL_1);
    __HAL_TIM_SET_COMPARE(&htim10, TIM_CHANNEL_1, 100);

    //    HAL_TIM_PWM_Start(FAN_FORWARD_TIM, FAN_FORWARD_CHANNEL);
    //    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 70);

    //    HAL_TIM_PWM_Start(LED_COLD_TIM, LED_COLD_TIM_CHANNEL);
    //    __HAL_TIM_SET_COMPARE(LED_COLD_TIM, LED_COLD_TIM_CHANNEL, 100);

    //    HAL_TIM_PWM_Start(LED_WARM_TIM, LED_WARM_TIM_CHANNEL);
    //    __HAL_TIM_SET_COMPARE(LED_WARM_TIM, LED_WARM_TIM_CHANNEL, 100);

    //    HAL_GPIO_WritePin(USB_POWER_EN_GPIO_Port, USB_POWER_EN_Pin, GPIO_PIN_SET);

    xTaskCreate(sys_task, "sys_task", 128, NULL, tskIDLE_PRIORITY + 2, NULL);

    return status_ok;
}

static void
sys_task(void* parameter) {
    
    
    
    while (1) {
        //        TRACE("sys wdog feed\n");

        lcd_1in83_clear(WHITE);
        osDelay(SYS_TASK_DELAY_TIME);
        lcd_1in83_clear(BLACK);
        osDelay(SYS_TASK_DELAY_TIME);
        lcd_1in83_clear(BLUE);
        osDelay(SYS_TASK_DELAY_TIME);
        lcd_1in83_clear(BRED);
        osDelay(SYS_TASK_DELAY_TIME);
        lcd_1in83_clear(GRED);
        osDelay(SYS_TASK_DELAY_TIME);
        lcd_1in83_clear(GBLUE);
        osDelay(SYS_TASK_DELAY_TIME);
        lcd_1in83_clear(RED);
        osDelay(SYS_TASK_DELAY_TIME);
        
        wdog_feed();
        lcd_1in83_clear(MAGENTA);
        osDelay(SYS_TASK_DELAY_TIME);
        lcd_1in83_clear(GREEN);
        osDelay(SYS_TASK_DELAY_TIME);
        lcd_1in83_clear(CYAN);
        osDelay(SYS_TASK_DELAY_TIME);
        lcd_1in83_clear(YELLOW);
        osDelay(SYS_TASK_DELAY_TIME);
        lcd_1in83_clear(BROWN);
        osDelay(SYS_TASK_DELAY_TIME);
        
        wdog_feed();
        lcd_1in83_clear(BRRED);
        osDelay(SYS_TASK_DELAY_TIME);
        lcd_1in83_clear(GRAY);
        osDelay(SYS_TASK_DELAY_TIME);
        lcd_1in83_clear(DARKBLUE);
        osDelay(SYS_TASK_DELAY_TIME);
        lcd_1in83_clear(LIGHTBLUE);
        osDelay(SYS_TASK_DELAY_TIME);
        lcd_1in83_clear(GRAYBLUE);
        osDelay(SYS_TASK_DELAY_TIME);

        wdog_feed();
        lcd_1in83_clear(LIGHTGREEN);
        osDelay(SYS_TASK_DELAY_TIME);
        lcd_1in83_clear(LGRAY);
        osDelay(SYS_TASK_DELAY_TIME);
        lcd_1in83_clear(LGRAYBLUE);
        osDelay(SYS_TASK_DELAY_TIME);
        lcd_1in83_clear(LBBLUE);
        osDelay(SYS_TASK_DELAY_TIME);
        
        wdog_feed();
        osDelay(500);
        osDelay(SYS_TASK_DELAY);
    }
}
