/*
    RtcConfig.h

    Configuration File for Rtc Module
*/

/* Copyright 2019 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 12Jul18, Karl Created
*/

#ifndef __RTC_CONFIG_H__
#define __RTC_CONFIG_H__

#ifdef __cplusplus
extern "C"
{
#endif /*__cplusplus */

/* Includes */
#include "app/config.h"

/* Configuration defines */
#ifndef RTC_ENABLE
#define RTC_ENABLE (0)
#endif
#ifndef RTC_STM32_ENABLE
#define RTC_STM32_ENABLE (0)
#endif
#ifndef RTC_DS1302_ENABLE
#define RTC_DS1302_ENABLE (0)
#endif
#ifndef RTC_STDC_TIME
#define RTC_STDC_TIME (2) /* 0: No use; 1: STM32; 2: DS1302 */
#endif
#ifndef RTC_RTOS
#define RTC_RTOS (1)
#endif
#ifndef RTC_DEBUG
#define RTC_DEBUG (0)
#endif
#ifndef RTC_SHOW_TIME
#define RTC_SHOW_TIME (0)
#endif
#ifndef RTC_ASSERT
#define RTC_ASSERT (0)
#endif
#ifndef RTC_TEST
#define RTC_TEST (0)
#endif
#ifndef RTC_ENABLE_MSP
#define RTC_ENABLE_MSP (0)
#endif

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __RTC_CONFIG_H__ */
