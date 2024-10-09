/**
 * \file            lib_delay.h
 * \brief           Delay library
 */

/*
 * Copyright (c) 2024 Jasper
 *
 * This file includes all the delay functions.
 *
 * Author:          Jasper <jasperzpzhang@gmail.com>
 * Version:         v0.0.1
 */


#ifndef __LIB_DELAY_H__
#define __LIB_DELAY_H__



#include "main.h"
#include "app/config.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if DELAY_ENABLE

/**
 * @defgroup delay delay function
 * @brief    delay function modules
 * @{
 */

/**
 * @brief  delay clock init
 * @return status code
 *         - 0 success
 * @note   none
 */
uint8_t delay_init(void);

/**
 * @brief     delay us
 * @param[in] us
 * @note      none
 */
void delay_us(uint32_t us);

/**
  * @brief  Robust delay function delay_ms.
  *         Automatically selects between FreeRTOS's vTaskDelay or HAL_Delay 
  *         depending on the presence of FreeRTOS and scheduler state.
  * @param  ms: Delay duration in milliseconds
  * @retval None
  */
void delay_ms(uint32_t ms);

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* __LIB_DELAY_H__ */


