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
#include "lib/debug/lib_debug.h"
#ifdef DELAY_RTOS 
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"  // Include xPortIsInsideInterrupt definition
#endif

#if DELAY_ENABLE

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

/**
  * @brief  Robust delay function delay_ms.
  *         Automatically selects between FreeRTOS's vTaskDelay or HAL_Delay 
  *         depending on the presence of FreeRTOS and scheduler state.
  * @param  ms: Delay duration in milliseconds
  * @retval None
  */
void delay_ms(uint32_t ms) {
    /* Check for invalid input: if ms is 0, do nothing */
    if (ms == 0) {
        return;
    }

#ifdef DELAY_RTOS
    /* Detect if we are inside an interrupt context */
    if (xPortIsInsideInterrupt()) {
        /* vTaskDelay cannot be used in interrupt context, use busy-wait instead */
        volatile uint32_t counter = ms * 1000;  // Simple busy-wait loop
        while (counter--) {
            __NOP();  // Prevent compiler optimization
        }
    } else {
        /* Get the current state of the scheduler */
        BaseType_t schedulerState = xTaskGetSchedulerState();
        
        if (schedulerState == taskSCHEDULER_RUNNING) {
            /* If the scheduler is running, use vTaskDelay for task-aware delay */
            vTaskDelay(pdMS_TO_TICKS(ms));  // Convert milliseconds to system ticks
        } else {
            /* If the scheduler is not started or suspended, use HAL_Delay */
            HAL_Delay(ms);
        }
    }
#else
    /* If FreeRTOS is not available, always use HAL_Delay */
    HAL_Delay(ms);
#endif
}

#endif /* DELAY_ENABLE */

