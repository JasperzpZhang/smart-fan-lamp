/*
    MemSpiFlashIf.h
    
    Head File for Mem Spi Flash Interface Module
*/

/* Copyright 2019 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 20Sep18, David Created
    01b, 06Oct18, Karl Modified
    01c, 24Jul19, Karl Reconstructured Mem lib
*/

#ifndef __MEM_SPI_FLASH_IF_H__
#define __MEM_SPI_FLASH_IF_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/* Includes */
#include <stdint.h>
#include <main.h>
#include "lib/type/lib_type.h"
#include "drv/peri/mem/drv_mem.h"
#include "drv/peri/mem/drv_mem_config.h"


/* Functions */
status_t    SpiFlashIfInit(void);
status_t    SpiFlashIfTerm(void);
status_t    SpiFlashIfConfig(MemHandle_t xHandle, MemConfig_t *pxConfig);

status_t    SpiFlashIfCtrl(MemHandle_t xHandle, MemCtrlOperation_t xCtrlOp, uint32_t ulExtraPara);
status_t    SpiFlashIfRead(MemHandle_t xHandle, uint32_t ulAddr, uint32_t ulLength, OUT uint8_t *pucData);
status_t    SpiFlashIfWrite(MemHandle_t xHandle, uint32_t ulAddr, uint32_t ulLength, IN uint8_t *pucData);
status_t    SpiFlashIfEraseWrite(MemHandle_t xHandle, uint32_t ulAddr, uint32_t ulLength, IN uint8_t *pucData);

#if MEM_SPIFLASH_ENABLE_MSP
void HAL_SPI_MspInit(SPI_HandleTypeDef *pxSpiHandle);
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *pxSpiHandle);
#endif /* MEM_SPIFLASH_ENABLE_MSP */

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __MEM_SPI_FLASH_IF_H__ */
