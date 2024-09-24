/*
    MemConfig.h

    Configuration File for Mem Module
*/

/* Copyright 2019 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 24Jul19, Karl Created
    01b, 11May20, Karl Added MEM_FRAM_ADDR_MASK & MEM_FRAM_ADDR_WIDTH
*/

#ifndef __MEM_CONFIG_H__
#define __MEM_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/* Includes */
#include "app/config.h"

/* Configuration defines */
#ifndef MEM_ENABLE
#define MEM_ENABLE                  (0)
#endif
#ifndef MEM_ENABLE_FRAM
#define MEM_ENABLE_FRAM             (0)
#endif
#ifndef MEM_ENABLE_SPIFLASH
#define MEM_ENABLE_SPIFLASH         (0)
#endif
#ifndef MEM_ENABLE_STM32FLASH
#define MEM_ENABLE_STM32FLASH       (0)
#endif
#ifndef MEM_RTOS
#define MEM_RTOS                    (1)
#endif
#ifndef MEM_DEBUGx
#define MEM_DEBUGx                  (0)
#endif
#ifndef MEM_FRAM_ADDR_MASK
#define MEM_FRAM_ADDR_MASK          (0x1FFFF)
#endif
#ifndef MEM_FRAM_ADDR_WIDTH
#define MEM_FRAM_ADDR_WIDTH         (17)
#endif
#ifndef MEM_TEST
#define MEM_TEST                    (0)
#endif
#ifndef MEM_ASSERT
#define MEM_ASSERT                  (0)
#endif
#ifndef MEM_SPIFLASH_ENABLE_MSP
#define MEM_SPIFLASH_ENABLE_MSP     (0)
#endif
#ifndef MEM_FRAM_ENABLE_MSP
#define MEM_FRAM_ENABLE_MSP         (0)
#endif

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __MEM_CONFIG_H__ */
