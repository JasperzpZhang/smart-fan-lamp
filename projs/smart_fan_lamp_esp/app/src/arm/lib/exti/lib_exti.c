/*
    lib_exti.c

    Implementation File for exti Module
*/

/* Copyright (c) 2024 Jasper. */

/*
    modification history
    --------------------
    01a, 25Sep24, Jasper Created
*/

/* Includes */

#include "lib/exti/lib_exti.h"

/* Pragmas */
#pragma diag_suppress 111 /* warning: #111-D: statement is unreachable */

/* Debug config */
#if EXTI_DEBUG
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* EXTI_DEBUG */
#if EXTI_ASSERT
#undef ASSERT
#define ASSERT(a)                                                                                                      \
    while (!(a)) {                                                                                                     \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                    \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* EXTI_ASSERT */

/* Local defines */

/* Local types */
typedef struct {
    uint16_t data;

} exti_ctrl_t;

/* Forward declaration */

/* Functions */

status_t
exti_init(void) {

    return status_ok;
}

__weak void
gpio_0_exti_cb(uint16_t gpio_pin) {
    /* Do nothing */
}

__weak void
gpio_1_exti_cb(uint16_t gpio_pin) {
    /* Do nothing */
}

__weak void
gpio_2_exti_cb(uint16_t gpio_pin) {
    /* Do nothing */
}

__weak void
gpio_3_exti_cb(uint16_t gpio_pin) {
    /* Do nothing */
}

__weak void
gpio_4_exti_cb(uint16_t gpio_pin) {
    /* Do nothing */
}

__weak void
gpio_5_exti_cb(uint16_t gpio_pin) {
    /* Do nothing */
}

__weak void
gpio_6_exti_cb(uint16_t gpio_pin) {
    /* Do nothing */
}

void
HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

    switch (GPIO_Pin) {
        case GPIO_PIN_0: gpio_0_exti_cb(GPIO_Pin); break;
        case GPIO_PIN_1: gpio_1_exti_cb(GPIO_Pin); break;
        case GPIO_PIN_2: gpio_2_exti_cb(GPIO_Pin); break;
        case GPIO_PIN_3: gpio_3_exti_cb(GPIO_Pin); break;
        case GPIO_PIN_4: gpio_4_exti_cb(GPIO_Pin); break;
        case GPIO_PIN_5: gpio_5_exti_cb(GPIO_Pin); break;
        case GPIO_PIN_6: gpio_6_exti_cb(GPIO_Pin); break;
    }
}
