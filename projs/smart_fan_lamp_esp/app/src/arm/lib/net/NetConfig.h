/*
    NetConfig.h

    Configuration File for Net Module
*/

/* Copyright 2019 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 03Aug19, Karl Created
*/

#ifndef __NET_CONFIG_H__
#define __NET_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/* Includes */
#include "Config.h"

/* Configuration defines */
#ifndef NET_ENABLE
#define NET_ENABLE              (0)
#endif
#ifndef NET_ENABLE_LWIP
#define NET_ENABLE_LWIP         (0)
#endif
#ifndef NET_ENABLE_SUITE
#define NET_ENABLE_SUITE        (0)
#endif
#ifndef NET_RTOS
#define NET_RTOS                (1)
#endif
#ifndef NET_DEBUG
#define NET_DEBUG               (0)
#endif
#ifndef NET_TEST
#define NET_TEST                (0)
#endif
#ifndef NET_ASSERT
#define NET_ASSERT              (0)
#endif

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __NET_CONFIG_H__ */
