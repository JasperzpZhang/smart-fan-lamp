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

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

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

void os_delay_ms(uint32_t ms);
void hal_delay_ms(uint32_t ms);
void os_delay_s(uint32_t s);
void hal_delay_s(uint32_t s);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LIB_DELAY_H__ */
