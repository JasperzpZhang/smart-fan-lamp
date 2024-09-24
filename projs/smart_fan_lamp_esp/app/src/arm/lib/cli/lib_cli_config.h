/*
    CliConfig.h

    Configuration File for Cli Module
*/

/* Copyright 2019 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 01Aut19, Karl Created
*/

#ifndef __CLI_CONFIG_H__
#define __CLI_CONFIG_H__

#ifdef __cplusplus
extern "C"
{
#endif /*__cplusplus */

/* Includes */
#include "app/config.h"

/* Configuration defines */
#ifndef CLI_ENABLE
#define CLI_ENABLE (0)
#endif
#ifndef CLI_ENABLE_UART
#define CLI_ENABLE_UART (0)
#endif
#ifndef CLI_ENABLE_UDP
#define CLI_ENABLE_UDP (0)
#endif
#ifndef CLI_ENABLE_CUSTOM
#define CLI_ENABLE_CUSTOM (0)
#endif
#ifndef CLI_RTOS
#define CLI_RTOS (0)
#endif
#ifndef CLI_DEBUG
#define CLI_DEBUG (0)
#endif
#ifndef CLI_TEST
#define CLI_TEST (0)
#endif
#ifndef CLI_ASSERT
#define CLI_ASSERT (0)
#endif

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __CLI_CONFIG_H__ */
