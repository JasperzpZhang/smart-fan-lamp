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

#include "lib/delay/lib_delay.h"
#include "FreeRTOS.h"
#include "lib/debug/lib_debug.h"
#include "task.h"

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

static volatile uint32_t gs_fac_us = 0;        /**< fac cnt */

/**
 * @brief  delay clock init
 * @return status code
 *         - 0 success
 * @note   none
 */
uint8_t delay_init(void)
{
    /* usr HCLK */
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    
    /* set fac */
    gs_fac_us = 168;
    
    return 0;
}

/**
 * @brief     delay us
 * @param[in] us
 * @note      none
 */
void delay_us(uint32_t us)
{
    uint32_t ticks;
    uint32_t told;
    uint32_t tnow;
    uint32_t tcnt;
    uint32_t reload;
    
    /* set the used param */
    tcnt = 0;
    reload = SysTick->LOAD;
    ticks = us * gs_fac_us;
    told = SysTick->VAL;
    
    /* delay */
    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow;
            }
            else 
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks)
            {
                break;
            }
        }
    }
}

void
os_delay_ms(uint32_t ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}

void
hal_delay_ms(uint32_t ms) {
    HAL_Delay(ms);
}

void
os_delay_s(uint32_t s) {
    os_delay_ms(s * 1000);
}

void
hal_delay_s(uint32_t s) {
    hal_delay_ms(s * 1000);
}
