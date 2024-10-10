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
#if WAVE_DEBUG || 1
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

void radar_task(void* para);

status_t
wave_init(void) {
    xTaskCreate(wave_task, "wave task", 128, NULL, tskIDLE_PRIORITY + 1, NULL);
    return status_ok;
}

void
radar_task(void* para) {
    static uint32_t adc_light_value;

    while (1) {

        if (th_scence_mode == 0) {
            HAL_ADC_Start(&hadc1);
            HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
            adc_light_value = HAL_ADC_GetValue(&hadc1);
            //    TRACE("adc_value : %d mv [%d]\n", adc_light_value * 3300 / 4095, adc_light_value);
        }

        if ((adc_light_value < 100) && HAL_GPIO_ReadPin(WAVE_DETC_GPIO_Port, WAVE_DETC_Pin) == GPIO_PIN_RESET) {
            if (g_led_ctrl.status._NIGHT_LIGHT_STATUS != 1) {
                night_light_set_status(1);
                //                led_set_status(1);
                //                fan_set_status(1);
            }
        } else {
            if (g_led_ctrl.status._NIGHT_LIGHT_STATUS != 0) {
                night_light_set_status(0);
                //                led_set_status(0);
                //                fan_set_status(0);
            }
        }

        osDelay(1000);
    }
}

//        if (HAL_GPIO_ReadPin(WAVE_DETC_GPIO_Port, WAVE_DETC_Pin) == GPIO_PIN_RESET) {
//            if (g_led_ctrl.status._NIGHT_LIGHT_STATUS != 1) {
//                night_light_set_status(1);
//                led_set_status(1);
//                fan_set_status(1);
//            }
//        }
//        else {
//            if (g_led_ctrl.status._NIGHT_LIGHT_STATUS != 0) {
//                osDelay(5000);
//
//                if (HAL_GPIO_ReadPin(WAVE_DETC_GPIO_Port, WAVE_DETC_Pin) == GPIO_PIN_RESET) {
//                    continue;
//                }
//                night_light_set_status(0);
//                led_set_status(0);
//                fan_set_status(0);
//
//            }
//        }
