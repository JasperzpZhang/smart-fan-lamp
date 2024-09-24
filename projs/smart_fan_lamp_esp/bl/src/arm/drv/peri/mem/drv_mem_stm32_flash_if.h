/*
    MemStm32FlashIf.h

    Head File for Mem Stm32 Flash Interface Module
*/

/* Copyright 2020 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 26Sep18, Karl Created
    01b, 01Dec18, Karl Modified
    01c, 24Jul19, Karl Reconstructured Mem lib
    01d, 04Jun20, Karl Fixed warnings
*/

#ifndef __MEM_STM32_FLASH_IF_H__
#define __MEM_STM32_FLASH_IF_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/* Includes */
#include <stdint.h>
#include <stm32f1xx_hal.h>
#include "lib/type/lib_type.h"
#include "drv/peri/mem/drv_mem.h"
#include "drv/peri/mem/drv_mem_config.h"


/* Defines */
/* Base address of the Flash sectors */
#define STM32_FLASH_BASE_ADDR           ((uint32_t)0x08000000)
#define STM32_FLASH_PAGE_SIZE           FLASH_PAGE_SIZE
#if STM32F107xC
    /* Capacity: 256KBytes */
    #define STM32_FLASH_USER_ADDR_START (0x08020000)    /* XXX: Change me according the user requiments */
    #define STM32_FLASH_USER_ADDR_END   (0x08040000)
#endif /* STM32F107xC */
#if STM32F103xB
    /* Capacity: 64KBytes */
    #define STM32_FLASH_USER_ADDR_START (0x08008000)    /* XXX: Change me according the user requiments */
    #define STM32_FLASH_USER_ADDR_END   (0x08010000)
#endif /* STM32F103xB */
#define SIZEOF08(a)                     sizeof(a)
#define SIZEOF16(a)                     sizeof(a)/2
#define SIZEOF32(a)                     sizeof(a)/4

/* Types */
enum  {
    FLASHIF_OK = 0,
    FLASHIF_ERASEKO,
    FLASHIF_WRITINGCTRL_ERROR,
    FLASHIF_WRITING_ERROR,
    FLASHIF_PROTECTION_ERRROR,
    FLASHIF_ADDRRANGE_ERROR,
};

/* Protection Type */
enum {
  FLASHIF_PROTECTION_NONE         = 0,
  FLASHIF_PROTECTION_PCROPENABLED = 0x1,
  FLASHIF_PROTECTION_WRPENABLED   = 0x2,
  FLASHIF_PROTECTION_RDPENABLED   = 0x4,
};

/* Protection Update */
enum {
    FLASHIF_WRP_ENABLE,
    FLASHIF_WRP_DISABLE
};

/* Functions */
status_t    Stm32FlashIfInit(void);
status_t    Stm32FlashIfTerm(void);
status_t    Stm32FlashIfConfig(MemHandle_t xHandle, MemConfig_t *pxConfig);

status_t    Stm32FlashIfCtrl(MemHandle_t xHandle, MemCtrlOperation_t xCtrlOp, uint32_t ulExtraPara);
status_t    Stm32FlashIfRead(MemHandle_t xHandle, uint32_t ulAddr, uint32_t ulLength, OUT uint8_t *pucData);
status_t    stm32_flash_if_write(MemHandle_t xHandle, uint32_t ulAddr, uint32_t ulLength, IN uint8_t *pucData);

uint32_t    Stm32FlashIfGetWpStatus(MemHandle_t xHandle, uint32_t ulObWrpPages);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif  /* __MEM_STM32_FLASH_IF_H__ */
