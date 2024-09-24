/**
 * \file            lib_tim.c
 * \brief           timer library
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
 * This file is all of the timer library..
 *
 * Author:          Jasper <jasperzhangse@gmail.com>
 * Version:         v1.0.0
 */

#include "lib/tim/lib_tim.h"
#include "tim.h"
#include "app/include.h"

/* Debug config */
#if LIB_TIM_DEBUG || 1
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* LIB_TIM_DEBUG */

void tim_init(void)
{
    
}

/**
  * @brief  TIM1 Period Elapsed Callback (Timer event notification).
  *         This function is called when TIM1 reaches the end of the period.
  * @param  htim TIM handle
  * @retval None
  */
__weak void tim1_period_elapsed_cb(TIM_HandleTypeDef* htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);

  /* NOTE : This function should not be modified. When the callback is needed,
            tim1_period_elapsed_cb can be implemented in the user file.
   */
}

/**
  * @brief  TIM2 Period Elapsed Callback (Timer event notification).
  *         This function is called when TIM2 reaches the end of the period.
  * @param  htim TIM handle
  * @retval None
  */
__weak void tim2_period_elapsed_cb(TIM_HandleTypeDef* htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);

  /* NOTE : This function should not be modified. When the callback is needed,
            tim2_period_elapsed_cb can be implemented in the user file.
   */
}

/**
  * @brief  TIM3 Period Elapsed Callback (Timer event notification).
  *         This function is called when TIM3 reaches the end of the period.
  * @param  htim TIM handle
  * @retval None
  */
__weak void tim3_period_elapsed_cb(TIM_HandleTypeDef* htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);

  /* NOTE : This function should not be modified. When the callback is needed,
            tim3_period_elapsed_cb can be implemented in the user file.
   */
}

/**
  * @brief  TIM4 Period Elapsed Callback (Timer event notification).
  *         This function is called when TIM4 reaches the end of the period.
  * @param  htim TIM handle
  * @retval None
  */
__weak void tim4_period_elapsed_cb(TIM_HandleTypeDef* htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);

  /* NOTE : This function should not be modified. When the callback is needed,
            tim4_period_elapsed_cb can be implemented in the user file.
   */
}

/**
  * @brief  TIM5 Period Elapsed Callback (Timer event notification).
  *         This function is called when TIM5 reaches the end of the period.
  * @param  htim TIM handle
  * @retval None
  */
__weak void tim5_period_elapsed_cb(TIM_HandleTypeDef* htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);

  /* NOTE : This function should not be modified. When the callback is needed,
            tim5_period_elapsed_cb can be implemented in the user file.
   */
}

/**
  * @brief  TIM6 Period Elapsed Callback (Timer event notification).
  *         This function is called when TIM6 reaches the end of the period.
  * @param  htim TIM handle
  * @retval None
  */
__weak void tim6_period_elapsed_cb(TIM_HandleTypeDef* htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);

  /* NOTE : This function should not be modified. When the callback is needed,
            tim6_period_elapsed_cb can be implemented in the user file.
   */
}

/**
  * @brief  TIM7 Period Elapsed Callback (Timer event notification).
  *         This function is called when TIM7 reaches the end of the period.
  * @param  htim TIM handle
  * @retval None
  */
__weak void tim7_period_elapsed_cb(TIM_HandleTypeDef* htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);

  /* NOTE : This function should not be modified. When the callback is needed,
            tim7_period_elapsed_cb can be implemented in the user file.
   */
}

/**
  * @brief  TIM8 Period Elapsed Callback (Timer event notification).
  *         This function is called when TIM8 reaches the end of the period.
  * @param  htim TIM handle
  * @retval None
  */
__weak void tim8_period_elapsed_cb(TIM_HandleTypeDef* htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);

  /* NOTE : This function should not be modified. When the callback is needed,
            tim8_period_elapsed_cb can be implemented in the user file.
   */
}

/**
  * @brief  TIM9 Period Elapsed Callback (Timer event notification).
  *         This function is called when TIM9 reaches the end of the period.
  * @param  htim TIM handle
  * @retval None
  */
__weak void tim9_period_elapsed_cb(TIM_HandleTypeDef* htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);

  /* NOTE : This function should not be modified. When the callback is needed,
            tim9_period_elapsed_cb can be implemented in the user file.
   */
}


void tim_period_elapsed_cb(TIM_HandleTypeDef* htim) {
    
    switch ((uint32_t)htim->Instance) {
#ifdef TIM1
        case (uint32_t)TIM1:
            tim1_period_elapsed_cb(htim);
            break;
#endif

#ifdef TIM2
        case (uint32_t)TIM2:
            tim2_period_elapsed_cb(htim);
            break;
#endif

#ifdef TIM3
        case (uint32_t)TIM3:
            tim3_period_elapsed_cb(htim);
            break;
#endif

#ifdef TIM4
        case (uint32_t)TIM4:
            tim4_period_elapsed_cb(htim);
            break;
#endif

#ifdef TIM5
        case (uint32_t)TIM5:
            tim5_period_elapsed_cb(htim);
            break;
#endif

#ifdef TIM6
        case (uint32_t)TIM6:
            tim5_period_elapsed_cb(htim);
            break;
#endif

#ifdef TIM7
        case (uint32_t)TIM7:
            tim5_period_elapsed_cb(htim);
            break;
#endif

#ifdef TIM8
        case (uint32_t)TIM8:
            tim5_period_elapsed_cb(htim);
            break;
#endif

#ifdef TIM9
        case (uint32_t)TIM9:
            tim5_period_elapsed_cb(htim);
            break;
#endif
    }
}
