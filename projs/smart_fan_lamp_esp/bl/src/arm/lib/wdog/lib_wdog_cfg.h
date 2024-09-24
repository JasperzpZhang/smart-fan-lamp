/*
    WdogConfig.h

    Configuration File for Wdog Module
*/

/* Copyright 2019 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 20Aug19, Karl Created
*/

#ifndef __WDOG_CONFIG_H__
#define __WDOG_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/* Includes */
#include "app/config.h"

/* Defines */
#ifndef WDOG_ENABLE
#define WDOG_ENABLE         (0)
#endif
#ifndef WDOG_RTOS
#define WDOG_RTOS           (1)
#endif
#ifndef WDOG_DEBUG
#define WDOG_DEBUG          (0)
#endif
#ifndef WDOG_ASSERT
#define WDOG_ASSERT         (0)
#endif
#ifndef WDOG_TEST
#define WDOG_TEST           (0)
#endif

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __WDOG_CONFIG_H__ */
