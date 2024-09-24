/**
 * \file            lib_delay.c
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

#include "FreeRTOS.h"      
#include "task.h"   
#include "lib/delay/lib_delay.h"
#include "lib/debug/lib_debug.h"

/* Debug config */
#if DELAY_DEBUG
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* DELAY_DEBUG */
#if DELAY_ASSERT
#undef ASSERT
#define ASSERT(a)                                                                                                      \
    while (1) {                                                                                                        \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                    \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* DELAY_ASSERT */

void delay_us(uint32_t us) {
    uint32_t ticks = (SystemCoreClock / 1000000) * us;      /* 每微秒的Ticks */
    SysTick->LOAD = ticks - 1;                              /* 设定加载值 */
    SysTick->VAL = 0;                                       /* 清除当前计数值 */
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;               /* 启动计时器 */

    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));  /* 等待计时完成 */

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;              /* 关闭计时器 */
}

void os_delay_ms(uint32_t ms){
    vTaskDelay(pdMS_TO_TICKS(ms));
}

void hal_delay_ms(uint32_t ms){
    HAL_Delay(ms);
}

void os_delay_s(uint32_t s) {
    os_delay_ms(s * 1000); 
}

void hal_delay_s(uint32_t s) {
    hal_delay_ms(s * 1000); 
}
