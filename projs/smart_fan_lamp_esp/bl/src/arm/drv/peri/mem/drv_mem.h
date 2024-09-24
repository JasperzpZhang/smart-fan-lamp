/*
    Mem.h
    
    Head File for Mem Module
*/

/* Copyright 2019 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 24Jul19, Karl Created
*/

#ifndef __DRV_MEM_H__
#define __DRV_MEM_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/* Includes */
#include <stdint.h>
#include <stm32f1xx_hal.h>
#include "lib/type/lib_type.h"
#include "drv/peri/mem/drv_mem_config.h"

/* Defines */
#define MEM_GET_CTRL(handle)       ((MemCtrl_t*)(handle))

/* Types */
typedef uint32_t MemHandle_t;

typedef enum {
    MEM_DEVICE_FRAM,
    MEM_DEVICE_SPIFLASH,
    MEM_DEVICE_STM32FLASH,
}MemDevice_t;

typedef struct {
    uint32_t ulAddr;
    uint32_t ulLength;
}MemSpiFlashErase_t;

typedef struct {
    uint32_t ulStartSector;
}MemStm32FlashErase_t;

typedef struct {
    uint32_t ulObWrpPages;
    uint32_t ulStatus;
}MemStm32FlashWp_t;

typedef enum {
    /* FRAM Operation */
    MEM_CTRL_OP_FRAM_WP_NONE,
    MEM_CTRL_OP_FRAM_WP_QUARTER,
    MEM_CTRL_OP_FRAM_WP_HALF,
    MEM_CTRL_OP_FRAM_WP_WHOLE,
    /* SPI-FLASH Operation */
    MEM_CTRL_OP_SPIFLASH_ERASE,
    MEM_CTRL_OP_SPIFLASH_ERASE_CHIP,
    /* STM32-FLASH Operation */
    MEM_CTRL_OP_STM32FLASH_ERASE,
    MEM_CTRL_OP_STM32FLASH_WP
}MemCtrlOperation_t;

typedef struct {
    SPI_TypeDef         *pxSpi;
    struct {
        uint16_t        usPin;
        GPIO_TypeDef    *pxPinPort;
    }xCs;
    uint32_t            ulStAddr;
    uint32_t            ulEdAddr;
    uint32_t            ulCapacity;
}MemConfig_t;

typedef struct {
    Bool_t              bConfig;
    Bool_t              bInit;
    MemDevice_t         xDevice;
    MemConfig_t         xConfig;
    SPI_HandleTypeDef   hSpi;
    SPI_HandleTypeDef   *phSpi;
}MemCtrl_t;

/* Functions */
status_t        MemInit(MemDevice_t xDevice);
status_t        MemTerm(MemDevice_t xDevice);

MemHandle_t     MemCreate(MemDevice_t xDevice);
status_t        MemDelete(MemHandle_t xHandle);

status_t        MemConfigInit(MemConfig_t *pxConfig);
status_t        MemConfigSpi(MemConfig_t *pxConfig, SPI_TypeDef *pxSpi);
status_t        MemConfigCsPin(MemConfig_t *pxConfig, GPIO_TypeDef *pxPinPort, uint16_t usPin);
status_t        MemConfigCapacity(MemConfig_t *pxConfig, uint32_t ulStAddr, uint32_t ulEdAddr, uint32_t ulCapacity);
status_t        MemConfig(MemHandle_t xHandle, MemConfig_t *pxConfig);

status_t        MemCtrl(MemHandle_t xHandle, MemCtrlOperation_t xCtrlOp, uint32_t ulExtraPara);
status_t        MemRead(MemHandle_t xHandle, uint32_t ulAddr, uint32_t ulLength, OUT uint8_t *pucData);
status_t        MemWrite(MemHandle_t xHandle, uint32_t ulAddr, uint32_t ulLength, IN uint8_t *pucData);
status_t        MemEraseWrite(MemHandle_t xHandle, uint32_t ulAddr, uint32_t ulLength, IN uint8_t *pucData);

status_t        MemShow(MemHandle_t xHandle);

#if MEM_TEST
status_t        MemTest(uint8_t ucSel);    
#endif /* MEM_TEST */

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __DRV_MEM_H__ */
