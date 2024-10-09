/*
    Mem.c

    Implementation File for Mem Module
*/

/* Copyright 2019 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 24Jul19, Karl Created
*/

/* Includes */
#include "drv/peri/mem/drv_mem.h"
#include <cmsis_os.h>
#include <stdlib.h>
#include <string.h>
#include "drv/peri/mem/drv_mem_fram.h"
#include "drv/peri/mem/drv_mem_spi_flash_if.h"
#include "drv/peri/mem/drv_mem_stm32_flash_if.h"


#if MEM_ENABLE

/* Pragmas */
#pragma diag_suppress 177 /* warning: #177-D: variable was declared but never referenced */

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
        DebugPrintf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                     \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* MEM_ASSERT */

/* Local defines */
#if MEM_RTOS
#undef MUTEX_NAME
#define MUTEX_NAME s_xMemMutex
static osMutexId_t MUTEX_NAME; // ÉùÃ÷»¥³âËø±äÁ¿
#define MEM_MUTEX_INIT()                                                                                               \
    do {                                                                                                               \
        osMutexAttr_t mutex_attr = {0};                                                                                \
        s_xMemMutex = osMutexNew(&mutex_attr);                                                                         \
    } while (0)
#define MEM_LOCK()   osMutexAcquire(MUTEX_NAME, osWaitForever)
#define MEM_UNLOCK() osMutexRelease(MUTEX_NAME)
#else
#define MEM_MUTEX_INIT()
#define MEM_LOCK()
#define MEM_UNLOCK()
#endif /* MEM_RTOS */

static Bool_t s_bMutexInit = FALSE;

/* Functions */
status_t
MemInit(MemDevice_t xDevice) {
    status_t xRet = status_err;
    switch (xDevice) {
#if MEM_ENABLE_FRAM
        case MEM_DEVICE_FRAM: xRet = FramInit(); break;
#endif /* MEM_ENABLE_FRAM */
#if MEM_ENABLE_SPIFLASH
        case MEM_DEVICE_SPIFLASH: xRet = SpiFlashIfInit(); break;
#endif /* MEM_ENABLE_SPIFLASH */
#if MEM_ENABLE_STM32FLASH
        case MEM_DEVICE_STM32FLASH: xRet = Stm32FlashIfInit(); break;
#endif /* MEM_ENABLE_STM32FLASH */
        default:
            xRet = status_err;
            ASSERT(0); /* We should never get here! */
            break;
    }
    if (!s_bMutexInit) {
        MEM_MUTEX_INIT();
        s_bMutexInit = TRUE;
    }
    return xRet;
}

status_t
MemTerm(MemDevice_t xDevice) {
    status_t xRet = status_err;
    switch (xDevice) {
#if MEM_ENABLE_FRAM
        case MEM_DEVICE_FRAM: xRet = FramTerm(); break;
#endif /* MEM_ENABLE_FRAM */
#if MEM_ENABLE_SPIFLASH
        case MEM_DEVICE_SPIFLASH: xRet = SpiFlashIfTerm(); break;
#endif /* MEM_ENABLE_SPIFLASH */
#if MEM_ENABLE_STM32FLASH
        case MEM_DEVICE_STM32FLASH: xRet = Stm32FlashIfTerm(); break;
#endif /* MEM_ENABLE_STM32FLASH */
        default:
            ASSERT(0); /* We should never get here! */
            break;
    }
    return xRet;
}

MemHandle_t
MemCreate(MemDevice_t xDevice) {
    MemCtrl_t* pxCtrl = NULL;
    //    pxCtrl = (MemCtrl_t*)malloc(sizeof(MemCtrl_t));
    pxCtrl = (MemCtrl_t*)pvPortMalloc(sizeof(MemCtrl_t));
    memset(pxCtrl, 0, sizeof(MemCtrl_t));
    pxCtrl->xDevice = xDevice;
    return (MemHandle_t)pxCtrl;
}

status_t
MemDelete(MemHandle_t xHandle) {
    if (xHandle) {
        free((void*)xHandle);
    }
    return status_ok;
}

status_t
MemConfigInit(MemConfig_t* pxConfig) {
    ASSERT(NULL != pxConfig);
    memset(pxConfig, 0, sizeof(MemConfig_t));
    return status_ok;
}

status_t
MemConfigSpi(MemConfig_t* pxConfig, SPI_TypeDef* pxSpi) {
    ASSERT(NULL != pxConfig);
    pxConfig->pxSpi = pxSpi;
    return status_ok;
}

status_t
MemConfigCsPin(MemConfig_t* pxConfig, GPIO_TypeDef* pxPinPort, uint16_t usPin) {
    ASSERT(NULL != pxConfig);
    pxConfig->xCs.pxPinPort = pxPinPort;
    pxConfig->xCs.usPin = usPin;
    return status_ok;
}

status_t
MemConfigCapacity(MemConfig_t* pxConfig, uint32_t ulStAddr, uint32_t ulEdAddr, uint32_t ulCapacity) {
    ASSERT(NULL != pxConfig);
    pxConfig->ulStAddr = ulStAddr;
    pxConfig->ulEdAddr = ulEdAddr;
    pxConfig->ulCapacity = ulCapacity;
    return status_ok;
}

status_t
MemConfig(MemHandle_t xHandle, MemConfig_t* pxConfig) {
    status_t xRet = status_err;
    MemCtrl_t* pxCtrl = MEM_GET_CTRL(xHandle);

    ASSERT(NULL != pxCtrl);
    pxCtrl->xConfig = *pxConfig;
    pxCtrl->bConfig = TRUE;
    switch (pxCtrl->xDevice) {
#if MEM_ENABLE_FRAM
        case MEM_DEVICE_FRAM: xRet = FramConfig(xHandle, pxConfig); break;
#endif /* MEM_ENABLE_FRAM */
#if MEM_ENABLE_SPIFLASH
        case MEM_DEVICE_SPIFLASH: xRet = SpiFlashIfConfig(xHandle, pxConfig); break;
#endif /* MEM_ENABLE_SPIFLASH */
#if MEM_ENABLE_STM32FLASH
        case MEM_DEVICE_STM32FLASH: xRet = Stm32FlashIfConfig(xHandle, pxConfig); break;
#endif /* MEM_ENABLE_STM32FLASH */
        default:
            xRet = status_err;
            ASSERT(0); /* We should never get here! */
            break;
    }

    return xRet;
}

status_t
MemCtrl(MemHandle_t xHandle, MemCtrlOperation_t xCtrlOp, uint32_t ulExtraPara) {
    status_t xRet = status_err;
    MemCtrl_t* pxCtrl = MEM_GET_CTRL(xHandle);

    MEM_LOCK();
    ASSERT(NULL != pxCtrl);
    switch (pxCtrl->xDevice) {
#if MEM_ENABLE_FRAM
        case MEM_DEVICE_FRAM: xRet = FramCtrl(xHandle, xCtrlOp, ulExtraPara); break;
#endif /* MEM_ENABLE_FRAM */
#if MEM_ENABLE_SPIFLASH
        case MEM_DEVICE_SPIFLASH: xRet = SpiFlashIfCtrl(xHandle, xCtrlOp, ulExtraPara); break;
#endif /* MEM_ENABLE_SPIFLASH */
#if MEM_ENABLE_STM32FLASH
        case MEM_DEVICE_STM32FLASH: xRet = Stm32FlashIfCtrl(xHandle, xCtrlOp, ulExtraPara); break;
#endif /* MEM_ENABLE_STM32FLASH */
        default:
            ASSERT(0); /* We should never get here! */
            xRet = status_err;
            break;
    }
    MEM_UNLOCK();

    return xRet;
}

status_t
MemRead(MemHandle_t xHandle, uint32_t ulAddr, uint32_t ulLength, OUT uint8_t* pucData) {
    status_t xRet = status_err;
    MemCtrl_t* pxCtrl = MEM_GET_CTRL(xHandle);

    MEM_LOCK();
    ASSERT(NULL != pxCtrl);
    switch (pxCtrl->xDevice) {
#if MEM_ENABLE_FRAM
        case MEM_DEVICE_FRAM: xRet = FramRead(xHandle, ulAddr + pxCtrl->xConfig.ulStAddr, ulLength, pucData); break;
#endif /* MEM_ENABLE_FRAM */
#if MEM_ENABLE_SPIFLASH
        case MEM_DEVICE_SPIFLASH:
            xRet = SpiFlashIfRead(xHandle, ulAddr + pxCtrl->xConfig.ulStAddr, ulLength, pucData);
            break;
#endif /* MEM_ENABLE_SPIFLASH */
#if MEM_ENABLE_STM32FLASH
        case MEM_DEVICE_STM32FLASH:
            xRet = Stm32FlashIfRead(xHandle, ulAddr + pxCtrl->xConfig.ulStAddr, ulLength, pucData);
            break;
#endif /* MEM_ENABLE_STM32FLASH */
        default:
            ASSERT(0); /* We should never get here! */
            xRet = status_err;
            break;
    }
    MEM_UNLOCK();

    return xRet;
}

status_t
MemWrite(MemHandle_t xHandle, uint32_t ulAddr, uint32_t ulLength, IN uint8_t* pucData) {
    status_t xRet = status_err;
    MemCtrl_t* pxCtrl = MEM_GET_CTRL(xHandle);

    MEM_LOCK();
    ASSERT(NULL != pxCtrl);
    switch (pxCtrl->xDevice) {
#if MEM_ENABLE_FRAM
        case MEM_DEVICE_FRAM: xRet = FramWrite(xHandle, ulAddr + pxCtrl->xConfig.ulStAddr, ulLength, pucData); break;
#endif /* MEM_ENABLE_FRAM */
#if MEM_ENABLE_SPIFLASH
        case MEM_DEVICE_SPIFLASH:
            xRet = SpiFlashIfWrite(xHandle, ulAddr + pxCtrl->xConfig.ulStAddr, ulLength, pucData);
            break;
#endif /* MEM_ENABLE_SPIFLASH */
#if MEM_ENABLE_STM32FLASH
        case MEM_DEVICE_STM32FLASH:
            xRet = stm32_flash_if_write(xHandle, ulAddr + pxCtrl->xConfig.ulStAddr, ulLength, pucData);
            break;
#endif /* MEM_ENABLE_STM32FLASH */
        default:
            ASSERT(0); /* We should never get here! */
            xRet = status_err;
            break;
    }
    MEM_UNLOCK();

    return xRet;
}

status_t
MemEraseWrite(MemHandle_t xHandle, uint32_t ulAddr, uint32_t ulLength, IN uint8_t* pucData) {
    status_t xRet = status_err;
    MemCtrl_t* pxCtrl = MEM_GET_CTRL(xHandle);

    MEM_LOCK();
    ASSERT(NULL != pxCtrl);
    switch (pxCtrl->xDevice) {
#if MEM_ENABLE_SPIFLASH
        case MEM_DEVICE_SPIFLASH:

            xRet = SpiFlashIfEraseWrite(xHandle, ulAddr + pxCtrl->xConfig.ulStAddr, ulLength, pucData);
            break;
#endif /* MEM_ENABLE_SPIFLASH */
        default:
            ASSERT(0); /* We should never get here! */
            xRet = status_err;
            break;
    }
    MEM_UNLOCK();

    return xRet;
}

status_t
MemShow(MemHandle_t xHandle) {
    MemCtrl_t* pxCtrl = MEM_GET_CTRL(xHandle);

    ASSERT(NULL != pxCtrl);
    TRACE("MemShow [%08X]\n", pxCtrl);
    TRACE("    bConfig          : %d\n", pxCtrl->bConfig);
    TRACE("    bInit            : %d\n", pxCtrl->bInit);
    TRACE("    xDevice          : %d\n", pxCtrl->xDevice);
    TRACE("    xConfig          :\n");
    TRACE("        pxSpi        : %08X\n", pxCtrl->xConfig.pxSpi);
    TRACE("        xCs.Pin      : %d\n", pxCtrl->xConfig.xCs.usPin);
    TRACE("        xCs.Port     : %08X\n", pxCtrl->xConfig.xCs.pxPinPort);
    TRACE("        ulStAddr     : %08X\n", pxCtrl->xConfig.ulStAddr);
    TRACE("        ulEdAddr     : %08X\n", pxCtrl->xConfig.ulEdAddr);
    TRACE("        ulCapacity   : %d B, %.1f KB, %.1f MB\n", pxCtrl->xConfig.ulCapacity,
          (float)(pxCtrl->xConfig.ulCapacity) / 1024.f, (float)(pxCtrl->xConfig.ulCapacity) / 1024.f / 1024.f);
    TRACE("    hSpi             : %08X\n", pxCtrl->hSpi);
    TRACE("    phSpi            : %08X\n", pxCtrl->phSpi);

    return status_ok;
}

#endif /* MEM_ENABLE */
