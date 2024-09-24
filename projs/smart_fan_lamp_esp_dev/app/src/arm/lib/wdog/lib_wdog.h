/*
    Wdog.h

    Head File for Wdog Module
*/

/* Copyright 2018 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 08Oct18, Karl Created
    01b, 11Dec18, Karl Modified distinguish os & no-os wdog facility
*/

#ifndef __WDOG_H__
#define __WDOG_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/* Includes */
#include "iwdg.h"
#include "lib/type/lib_type.h"
#include "lib/wdog/lib_wdog_cfg.h"
//#include <cmsis_os.h>
#include <stdint.h>

/* Defines */
#ifndef WDOG_TASK_PRIORITY
#define WDOG_TASK_PRIORITY osPriorityAboveNormal
#endif
#ifndef WDOG_TASK_STKSIZE
#define WDOG_TASK_STKSIZE (128)
#endif
#ifndef WDOG_MAX_TASK_NUM
#define WDOG_MAX_TASK_NUM (10)
#endif
#ifndef WDOG_TASK_DELAY_MS
#define WDOG_TASK_DELAY_MS (1000) /* 1S */
#endif
#ifndef WDOG_TIMEOUT_US
#define WDOG_TIMEOUT_US (5000000) /* 5S */
#endif

/* Functions */

status_t wdog_set_timeout(uint32_t us);
status_t wdog_start(uint32_t ms);
status_t wdog_feed(void);

//Status_t WdogInit(void);
//Status_t WdogTerm(void);

///* Os-related wdog functions */
//Status_t WdogTaskEnable(osThreadId TaskId);
//Status_t WdogTaskFeed(void);
//Status_t WdogTaskStart(void);

///* No-os-related wdg functions */
//Status_t WdogStart(uint32_t ulMs);
//Status_t WdogFeed(void);

#if WDOG_TEST
Status_t WdogTest(void);
#endif /* WDOG_TEST */

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __WDOG_H__ */
