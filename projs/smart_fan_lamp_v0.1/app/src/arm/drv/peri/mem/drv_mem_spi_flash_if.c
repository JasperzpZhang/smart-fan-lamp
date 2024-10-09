/*
    MemSpiFlashIf.c

    Implementation File for Mem Spi Flash Interface Module
*/

/* Copyright 2019 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 20Sep18, David Created
    01b, 06Oct18, Karl Modified
    01c, 24Jul19, Karl Reconstructured Mem lib
*/

/* Includes */
#include <cmsis_os.h>
#include "app/define.h"
#include "drv/peri/mem/drv_mem_spi_flash.h"
#include "drv/peri/mem/drv_mem_spi_flash_if.h"
#include "lib/debug/lib_debug.h"
#include "spi.h"

#if (MEM_ENABLE && MEM_ENABLE_SPIFLASH)

/* Debug config */
#if MEM_DEBUGx
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* MEM_DEBUGx */
#if MEM_ASSERT
#undef ASSERT
#define ASSERT(a)                                                                                                      \
    while (!(a)) {                                                                                                     \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                    \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* MEM_ASSERT */

#define SPI_CFG 1   

/* Forward declaration */
static status_t prvEraseWrite(MemHandle_t xHandle, uint32_t ulAddr, uint32_t ulLength, IN uint8_t* pucData);
static status_t prvChipErase(MemHandle_t xHandle);
static status_t prvErase(MemHandle_t xHandle, uint32_t ulAddr, uint32_t ulLength);

/* Functions */
status_t
SpiFlashIfInit(void) {
    /* Do nothing */
    return status_ok;
}

status_t
SpiFlashIfTerm(void) {
    /* Do nothing */
    return status_ok;
}

status_t
SpiFlashIfConfig(MemHandle_t xHandle, MemConfig_t* pxConfig) {


    GPIO_InitTypeDef GPIO_InitStruct;
    MemCtrl_t* pxCtrl = MEM_GET_CTRL(xHandle);
    ASSERT(NULL != pxCtrl);

    /* Config paras */
    pxCtrl->xConfig = *pxConfig;
    pxCtrl->bConfig = TRUE;  

    #if SPI_CFG && 0
    /* Init spi */
    ASSERT(pxCtrl != NULL);
    pxCtrl->hSpi.Instance = pxConfig->pxSpi;
    pxCtrl->hSpi.Init.Mode = SPI_MODE_MASTER;
    pxCtrl->hSpi.Init.Direction = SPI_DIRECTION_2LINES;
    pxCtrl->hSpi.Init.DataSize = SPI_DATASIZE_8BIT;
    pxCtrl->hSpi.Init.CLKPolarity = SPI_POLARITY_HIGH;
    pxCtrl->hSpi.Init.CLKPhase = SPI_PHASE_2EDGE;
    pxCtrl->hSpi.Init.NSS = SPI_NSS_SOFT;
    pxCtrl->hSpi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    pxCtrl->hSpi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    pxCtrl->hSpi.Init.TIMode = SPI_TIMODE_DISABLE;
    pxCtrl->hSpi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    pxCtrl->hSpi.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&(pxCtrl->hSpi)) != HAL_OK) {
        ASSERT(0); /* We should never get here */
    }

    pxCtrl->phSpi = &(pxCtrl->hSpi);
    
    #else
     pxCtrl->phSpi = &hspi3;
#endif
    
    #if SPI_CFG && 0
    /* Init gpio cs pin */
    switch ((uint32_t)(pxCtrl->xConfig.xCs.pxPinPort)) {
        case (uint32_t)GPIOA: __HAL_RCC_GPIOA_CLK_ENABLE(); break;
        case (uint32_t)GPIOB: __HAL_RCC_GPIOB_CLK_ENABLE(); break;
        case (uint32_t)GPIOC: __HAL_RCC_GPIOC_CLK_ENABLE(); break;
        case (uint32_t)GPIOD: __HAL_RCC_GPIOD_CLK_ENABLE(); break;
        case (uint32_t)GPIOE: __HAL_RCC_GPIOE_CLK_ENABLE(); break;
        default: ASSERT(0); break; /* We should never get here! */
    }

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(pxCtrl->xConfig.xCs.pxPinPort, pxCtrl->xConfig.xCs.usPin, GPIO_PIN_SET);
    /*Configure GPIO pins */
    GPIO_InitStruct.Pin = pxCtrl->xConfig.xCs.usPin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(pxCtrl->xConfig.xCs.pxPinPort, &GPIO_InitStruct);
    Spi_Flash_Config(&pxCtrl->hSpi, pxCtrl->xConfig.xCs.pxPinPort, pxCtrl->xConfig.xCs.usPin);
#else
    /* Spi driver init */
    Spi_Flash_Config(pxCtrl->phSpi, pxCtrl->xConfig.xCs.pxPinPort, pxCtrl->xConfig.xCs.usPin);
//    Spi_Flash_Config(&pxCtrl->hSpi, SPI3_FLASH_CS_GPIO_Port, SPI3_FLASH_CS_Pin);
#endif  


if (FLASH_SUCCESS == Spi_Flash_Init()) {
        return status_ok;
    } else {
        TRACE("Spi_Flash_Init() failed\n");
        return status_err;
    }
}

status_t
SpiFlashIfCtrl(MemHandle_t xHandle, MemCtrlOperation_t xCtrlOp, uint32_t ulExtraPara) {
    status_t xRet;
    switch (xCtrlOp) {
        case MEM_CTRL_OP_SPIFLASH_ERASE: {
            MemSpiFlashErase_t xCtrl = *(MemSpiFlashErase_t*)ulExtraPara;
            xRet = prvErase(xHandle, xCtrl.ulAddr, xCtrl.ulLength);
            break;
        }
        case MEM_CTRL_OP_SPIFLASH_ERASE_CHIP: xRet = prvChipErase(xHandle); break;
        default: xRet = status_err; break;
    }
    return xRet;
}

status_t
SpiFlashIfRead(MemHandle_t xHandle, uint32_t ulAddr, uint32_t ulLength, OUT uint8_t* pucData) {
    MemCtrl_t* pxCtrl = MEM_GET_CTRL(xHandle);
    ASSERT(NULL != pxCtrl);
    ASSERT(TRUE == pxCtrl->bConfig);
    if ((ulAddr < pxCtrl->xConfig.ulStAddr) || ((ulAddr + ulLength) > pxCtrl->xConfig.ulEdAddr)) {
        return status_err;
    } else {
        if (Spi_Flash_Read(ulAddr, ulLength, pucData) != FLASH_SUCCESS) {
            ASSERT(0); /* We should never get here! */
        }
        return status_ok;
    }
}

status_t
SpiFlashIfWrite(MemHandle_t xHandle, uint32_t ulAddr, uint32_t ulLength, IN uint8_t* pucData) {
    MemCtrl_t* pxCtrl = MEM_GET_CTRL(xHandle);
    ASSERT(NULL != pxCtrl);
    ASSERT(TRUE == pxCtrl->bConfig);
    if ((ulAddr < pxCtrl->xConfig.ulStAddr) || ((ulAddr + ulLength) > pxCtrl->xConfig.ulEdAddr)) {
        return status_err;
    } else {
        if (Spi_Flash_Write(ulAddr, ulLength, pucData) != FLASH_SUCCESS) {
            ASSERT(0); /* We should never get here! */
        }
        return status_ok;
    }
}

status_t
SpiFlashIfEraseWrite(MemHandle_t xHandle, uint32_t ulAddr, uint32_t ulLength, IN uint8_t* pucData) {
    return prvEraseWrite(xHandle, ulAddr, ulLength, pucData);
}

static status_t
prvChipErase(MemHandle_t xHandle) {
    Flash_Err xErr;
    MemCtrl_t* pxCtrl = MEM_GET_CTRL(xHandle);
    ASSERT(NULL != pxCtrl);
    ASSERT(TRUE == pxCtrl->bConfig);
    xErr = Spi_Flash_Chip_Erase();

    if (pxCtrl->bConfig == true) {
        /* Do nothing, just to eliminate the unused pxCtrl warnning. */
    }

    if ((xErr != FLASH_SUCCESS) && (xErr != FLASH_ERR_TIMEOUT)) {
        ASSERT(0); /* We should never get here! */
    }
    return status_ok;
}

static status_t
prvErase(MemHandle_t xHandle, uint32_t ulAddr, uint32_t ulLength) {
    MemCtrl_t* pxCtrl = MEM_GET_CTRL(xHandle);
    ASSERT(NULL != pxCtrl);
    ASSERT(TRUE == pxCtrl->bConfig);
    if ((ulAddr < pxCtrl->xConfig.ulStAddr) || ((ulAddr + ulLength) > pxCtrl->xConfig.ulEdAddr)) {
        return status_err;
    } else {
        if (Spi_Flash_Erase(ulAddr, ulLength) != FLASH_SUCCESS) {
            ASSERT(0); /* We should never get here! */
        }
        return status_ok;
    }
}

static status_t
prvEraseWrite(MemHandle_t xHandle, uint32_t ulAddr, uint32_t ulLength, IN uint8_t* pucData) {
    MemCtrl_t* pxCtrl = MEM_GET_CTRL(xHandle);
    ASSERT(NULL != pxCtrl);
    ASSERT(TRUE == pxCtrl->bConfig);

    if ((ulAddr < pxCtrl->xConfig.ulStAddr) || ((ulAddr + ulLength) > pxCtrl->xConfig.ulEdAddr)) {

        return status_err;
    } else {
        if (Spi_Flash_Erase_Write(ulAddr, ulLength, pucData) != FLASH_SUCCESS) {
            ASSERT(0); /* We should never get here! */
        }
        return status_ok;
    }
}

#if MEM_SPIFLASH_ENABLE_MSP

/* XXX: MemSpiFlashIf.c -> HAL_SPI_MspInit only applies to BY-SCWJ101-V1.1 Board! */
void
HAL_SPI_MspInit(SPI_HandleTypeDef* pxSpiHandle) {
    GPIO_InitTypeDef GPIO_InitStruct;
    if (pxSpiHandle->Instance == SPI3) {
        /* Peripheral clock enable */
        __HAL_RCC_SPI3_CLK_ENABLE();

        /* 
         * SPI3 GPIO Configuration    
         * PC10 ------> SPI3_SCK
         * PC11 ------> SPI3_MISO
         * PC12 ------> SPI3_MOSI 
         */
        GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_12;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_11;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        __HAL_AFIO_REMAP_SPI3_ENABLE();
    }
}

/* XXX: MemSpiFlashIf.c -> HAL_SPI_MspDeInit only applies to BY-SCWJ101-V1.1 Board! */
void
HAL_SPI_MspDeInit(SPI_HandleTypeDef* pxSpiHandle) {
    if (pxSpiHandle->Instance == SPI3) {
        /* Peripheral clock disable */
        __HAL_RCC_SPI3_CLK_DISABLE();

        /*
         * SPI3 GPIO Configuration    
         * PC10 ------> SPI3_SCK
         * PC11 ------> SPI3_MISO
         * PC12 ------> SPI3_MOSI 
         */
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);
    }
}

#endif /* MEM_SPIFLASH_ENABLE_MSP */

#endif /* (MEM_ENABLE && MEM_ENABLE_SPIFLASH) */
