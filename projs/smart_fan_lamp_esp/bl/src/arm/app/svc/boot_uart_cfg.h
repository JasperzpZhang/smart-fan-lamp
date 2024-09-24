/*
    BootUartConfig.h

    Configuration File for Boot Uart Module
*/

/* Copyright 2022 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 06Apr22, Karl Created
*/

#ifndef __BOOT_UART_CONFIG_H__
#define __BOOT_UART_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes */
#include "app/config.h"

/* Defines */
#ifndef BOOT_UART_ENABLE
#define BOOT_UART_ENABLE             (0)
#endif
#ifndef BOOT_UART_FLASH_PAGE_SIZE
#define BOOT_UART_FLASH_PAGE_SIZE    (0x800)
#endif
#ifndef BOOT_UART_APP_ADDR
#define BOOT_UART_APP_ADDR           (0xFFFFFFFF)
#endif
#ifndef BOOT_UART_APP_SIZE
#define BOOT_UART_APP_SIZE           (0xFFFFFFFF)
#endif
#ifndef BOOT_UART_RTOS
#define BOOT_UART_RTOS               (0)
#endif
#ifndef BOOT_UART_DEBUG
#define BOOT_UART_DEBUG              (0)
#endif
#ifndef BOOT_UART_ASSERT
#define BOOT_UART_ASSERT             (0)
#endif
#ifndef BOOT_UART_TEST
#define BOOT_UART_TEST               (0)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __BOOT_UART_CONFIG_H__ */
