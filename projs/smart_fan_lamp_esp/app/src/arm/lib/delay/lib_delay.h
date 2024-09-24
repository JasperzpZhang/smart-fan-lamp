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

void delay_us(uint32_t us);
void os_delay_ms(uint32_t ms);
void hal_delay_ms(uint32_t ms);
void os_delay_s(uint32_t s);
void hal_delay_s(uint32_t s);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LIB_DELAY_H__ */
