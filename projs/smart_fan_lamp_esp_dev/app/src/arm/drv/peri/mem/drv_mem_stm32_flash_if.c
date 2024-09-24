/*
    MemStm32FlashIf.c

    Implementation File for Mem Stm32 Flash Interface Module
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

/* Includes */
#include "drv/peri/mem/drv_mem_stm32_flash_if.h"

#if (MEM_ENABLE && MEM_ENABLE_STM32FLASH)

/* Pragmas */
#pragma diag_suppress 177 /* warning: #177-D: function was declared but never referenced */

/* Debug config */
#if MEM_DEBUGx
#undef TRACE
#define TRACE(...) DebugPrintf(__VA_ARGS__)
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

/* Forward declaration */
static uint32_t prv_stm32_flash_if_erace(uint32_t ulStartSector);
static uint32_t prv_stm32_flash_if_write_uint8(uint32_t ulAddr, uint8_t* pucSrc, uint32_t ulSizeInUint8);
static uint32_t prv_stm32_flash_if_write_uint16(uint32_t ulAddr, uint16_t* pusSrc, uint32_t ulSizeInUint16);
static uint32_t prv_stm32_flash_if_write_uint32(uint32_t ulAddr, uint32_t* pulSrc, uint32_t ulSizeInUint32);
static uint32_t prv_stm32_flash_if_read_uint8(uint32_t ulAddr, uint8_t* pucDest, uint32_t ulSizeInUint8);
static uint32_t prv_stm32_flash_if_read_uint16(uint32_t ulAddr, uint16_t* pusDest, uint32_t ulSizeInUint16);
static uint32_t prv_stm32_flash_if_read_uint32(uint32_t ulAddr, uint32_t* pulDest, uint32_t ulSizeInUint32);
static uint32_t prv_stm32_flash_if_get_wp_status(uint32_t ulObWrpPages);
/* Call HAL_FLASH_OB_Launch to make the config working, HAL_FLASH_OB_Launch will reset the MCU automatically!  */
static uint32_t prv_stm32_flash_if_set_wp_status(uint32_t ulObWrpPages, uint32_t ulStatus);
static uint32_t prv_write_uint32(uint32_t ulAddr, uint32_t* pulSrc, uint32_t ulSizeInUint32);

/* Functions */
status_t Stm32FlashIfInit(void) {
    /* Unlock the Program memory */
    HAL_FLASH_Unlock();
    /* Clear all FLASH flags */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
    /* Lock the Program memory */
    HAL_FLASH_Lock();
    return status_ok;
}

status_t Stm32FlashIfTerm(void) {
    /* Do nothing */
    return status_ok;
}

status_t Stm32FlashIfConfig(MemHandle_t xHandle, MemConfig_t* pxConfig) {
    MemCtrl_t* pxCtrl = MEM_GET_CTRL(xHandle);
    ASSERT(NULL != pxCtrl);

    /* Config paras */
    pxCtrl->xConfig = *pxConfig;
    pxCtrl->bConfig = TRUE;
    return status_ok;
}

status_t Stm32FlashIfCtrl(MemHandle_t xHandle, MemCtrlOperation_t xCtrlOp, uint32_t ulExtraPara) {
    status_t xRet;
    switch (xCtrlOp) {
        case MEM_CTRL_OP_STM32FLASH_ERASE: {
            MemStm32FlashErase_t xCtrl = *(MemStm32FlashErase_t*)ulExtraPara;
            xRet = ((FLASHIF_OK == prv_stm32_flash_if_erace(xCtrl.ulStartSector)) ? status_ok : status_err);
            break;
        }
        case MEM_CTRL_OP_STM32FLASH_WP: {
            MemStm32FlashWp_t xCtrl = *(MemStm32FlashWp_t*)ulExtraPara;
            xRet = ((FLASHIF_OK == prv_stm32_flash_if_set_wp_status(xCtrl.ulObWrpPages, xCtrl.ulStatus)) ? status_ok
                                                                                                         : status_err);
            break;
        }
        default:
            xRet = status_err;
            break;
    }
    return xRet;
}

status_t Stm32FlashIfRead(MemHandle_t xHandle, uint32_t ulAddr, uint32_t ulLength, OUT uint8_t* pucData) {
    MemCtrl_t* pxCtrl = MEM_GET_CTRL(xHandle);
    ASSERT(NULL != pxCtrl);

    /* XXX: Address should be 32-bit aligned! */
    if (0 != (ulAddr % 4)) {
        return status_err;
    }
    if ((ulAddr < pxCtrl->xConfig.ulStAddr) || ((ulAddr + ulLength) > pxCtrl->xConfig.ulEdAddr)) {
        return status_err;
    }

    prv_stm32_flash_if_read_uint8(ulAddr, pucData, ulLength);
    return status_ok;
}

status_t stm32_flash_if_write(MemHandle_t xHandle, uint32_t ulAddr, uint32_t ulLength, IN uint8_t* pucData) {
    MemCtrl_t* pxCtrl = MEM_GET_CTRL(xHandle);
    ASSERT(NULL != pxCtrl);

    /* XXX: Address should be 32-bit aligned! */
    if (0 != (ulAddr % 4)) {
        return status_err;
    }
    if ((ulAddr < pxCtrl->xConfig.ulStAddr) || ((ulAddr + ulLength) > pxCtrl->xConfig.ulEdAddr)) {
        return status_err;
    }

    prv_stm32_flash_if_write_uint8(ulAddr, pucData, ulLength);
    return status_ok;
}

uint32_t Stm32FlashIfGetWpStatus(MemHandle_t xHandle, uint32_t ulObWrpPages) {
    return prv_stm32_flash_if_get_wp_status(ulObWrpPages);
}

uint32_t prv_stm32_flash_if_write_uint8(uint32_t ulAddr, uint8_t* pucSrc, uint32_t ulSizeInUint8) {
    uint32_t ulRet = 0;
    uint32_t ulSizeInUint32 = 0;
    uint32_t ulExtraByte = 0;

    ulSizeInUint32 = ulSizeInUint8 / 4;
    ulExtraByte = ulSizeInUint8 % 4;

    ulRet = prv_stm32_flash_if_write_uint32(ulAddr, (uint32_t*)pucSrc, ulSizeInUint32);
    if (FLASHIF_OK != ulRet) {
        return ulRet;
    }

    if (ulExtraByte) {
        uint16_t n;
        uint8_t ucData[4];
        prv_stm32_flash_if_read_uint32(ulAddr + ulSizeInUint32 * 4, (uint32_t*)ucData, 1);
        for (n = 0; n < ulExtraByte; n++) {
            ucData[n] = pucSrc[ulSizeInUint32 * 4 + n];
        }
        ulRet = prv_stm32_flash_if_write_uint32(ulAddr + ulSizeInUint32 * 4, (uint32_t*)ucData, 1);
    }

    return ulRet;
}

uint32_t prv_stm32_flash_if_write_uint16(uint32_t ulAddr, uint16_t* pusSrc, uint32_t ulSizeInUint16) {
    uint32_t ulRet = 0;
    uint32_t ulSizeInUint32 = 0;
    uint32_t ulExtraByte = 0;

    ulSizeInUint32 = ulSizeInUint16 / 2;
    ulExtraByte = ulSizeInUint16 % 2;

    ulRet = prv_stm32_flash_if_write_uint32(ulAddr, (uint32_t*)pusSrc, ulSizeInUint32);
    if (FLASHIF_OK != ulRet) {
        return ulRet;
    }

    if (ulExtraByte) {
        uint16_t n;
        uint16_t usData[2];
        prv_stm32_flash_if_read_uint32(ulAddr + ulSizeInUint32 * 4, (uint32_t*)usData, 1);
        for (n = 0; n < ulExtraByte; n++) {
            usData[n] = pusSrc[ulSizeInUint32 * 2 + n];
        }
        ulRet = prv_stm32_flash_if_write_uint32(ulAddr + ulSizeInUint32 * 4, (uint32_t*)usData, 1);
    }

    return ulRet;
}

uint32_t prv_stm32_flash_if_write_uint32(uint32_t ulAddr, uint32_t* pulSrc, uint32_t ulSizeInUint32) {
    static uint32_t ulFlashBuffer[STM32_FLASH_PAGE_SIZE / 4];
    static FLASH_EraseInitTypeDef pEraseInit;
    uint32_t i = 0, ulSectorErr = 0, ulSectorOffset = 0, ulSectorLeft = 0, ulSectorNum = 0, ulAddrOffset = 0;

    ulAddrOffset = ulAddr - STM32_FLASH_BASE_ADDR;
    ulSectorNum = ulAddrOffset / STM32_FLASH_PAGE_SIZE;
    ulSectorOffset = (ulAddrOffset % STM32_FLASH_PAGE_SIZE) / 4;
    ulSectorLeft = STM32_FLASH_PAGE_SIZE / 4 - ulSectorOffset;
    if (ulSizeInUint32 <= ulSectorLeft) {
        ulSectorLeft = ulSizeInUint32;
    }

    HAL_FLASH_Unlock();
    while (1) {
        prv_stm32_flash_if_read_uint32((ulSectorNum * STM32_FLASH_PAGE_SIZE + STM32_FLASH_BASE_ADDR), ulFlashBuffer,
                                       (STM32_FLASH_PAGE_SIZE / 4));

        /* Get the unwritten position */
        for (i = 0; i < ulSectorLeft; i++) {
            if (0xFFFF != ulFlashBuffer[ulSectorOffset + i]) {
                break;
            }
        }

        /* Erase the sector? */
        if (i < ulSectorLeft) {
            pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
            pEraseInit.PageAddress = ulSectorNum * STM32_FLASH_PAGE_SIZE + STM32_FLASH_BASE_ADDR;
            pEraseInit.NbPages = 1;
            HAL_FLASHEx_Erase(&pEraseInit, &ulSectorErr);
            for (i = 0; i < ulSectorLeft; i++) {
                ulFlashBuffer[ulSectorOffset + i] = pulSrc[i];
            }
            prv_write_uint32((ulSectorNum * STM32_FLASH_PAGE_SIZE + STM32_FLASH_BASE_ADDR), ulFlashBuffer,
                             (STM32_FLASH_PAGE_SIZE / 4));
        } else {
            prv_write_uint32(ulAddr, pulSrc, ulSectorLeft);
        }

        if (ulSizeInUint32 == ulSectorLeft) {
            break;
        } else {
            ulSectorNum++;
            ulSectorOffset = 0;
            pulSrc += ulSectorLeft;
            ulAddr += ulSectorLeft;
            ulSizeInUint32 -= ulSectorLeft;
            if (ulSizeInUint32 > (STM32_FLASH_PAGE_SIZE / 4)) {
                ulSectorLeft = STM32_FLASH_PAGE_SIZE / 4;
            } else {
                ulSectorLeft = ulSizeInUint32;
            }
        }
    }
    HAL_FLASH_Lock();

    return (FLASHIF_OK);
}

uint32_t prv_stm32_flash_if_read_uint8(uint32_t ulAddr, uint8_t* pucDest, uint32_t ulSizeInUint8) {
    uint32_t ulNum = 0;
    ulAddr = (uint32_t)ulAddr;
    while (ulNum < ulSizeInUint8) {
        *(pucDest + ulNum) = *(__IO uint8_t*)ulAddr;
        ulAddr += 1;
        ulNum++;
    }
    return ulNum;
}

uint32_t prv_stm32_flash_if_read_uint16(uint32_t ulAddr, uint16_t* pusDest, uint32_t ulSizeInUint16) {
    uint32_t ulNum = 0;
    ulAddr = (uint32_t)ulAddr;
    while (ulNum < ulSizeInUint16) {
        *(pusDest + ulNum) = *(__IO uint16_t*)ulAddr;
        ulAddr += 2;
        ulNum++;
    }
    return ulNum;
}

uint32_t prv_stm32_flash_if_read_uint32(uint32_t ulAddr, uint32_t* pulDest, uint32_t ulSizeInUint32) {
    uint32_t ulNum = 0;
    ulAddr = (uint32_t)ulAddr;
    while (ulNum < ulSizeInUint32) {
        *(pulDest + ulNum) = *(__IO uint32_t*)ulAddr;
        ulAddr += 4;
        ulNum++;
    }
    return ulNum;
}

static uint32_t prv_stm32_flash_if_erace(uint32_t ulStartSector) {
    /* This function does an erase of all user flash area */
    uint32_t ulNbrOfPages = 0;
    uint32_t ulPageError = 0;
    FLASH_EraseInitTypeDef pEraseInit;
    HAL_StatusTypeDef status = HAL_OK;

    if (ulStartSector < STM32_FLASH_USER_ADDR_START) {
        return FLASHIF_ADDRRANGE_ERROR;
    }

    /* Unlock the Flash to enable the flash control register access */
    HAL_FLASH_Unlock();

    /* Get the sector where start the user flash area */
    ulNbrOfPages = (STM32_FLASH_USER_ADDR_END - ulStartSector) / FLASH_PAGE_SIZE;

    pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    pEraseInit.PageAddress = ulStartSector;
    pEraseInit.Banks = FLASH_BANK_1;
    pEraseInit.NbPages = ulNbrOfPages;
    status = HAL_FLASHEx_Erase(&pEraseInit, &ulPageError);

    /* Lock the Flash to disable the flash control register access */
    HAL_FLASH_Lock();

    if (status != HAL_OK) {
        /* Error occurred while page erase */
        return FLASHIF_ERASEKO;
    }

    return FLASHIF_OK;
}

uint32_t prv_stm32_flash_if_get_wp_status(uint32_t ulObWrpPages) {
    uint32_t ulProtectedPage = FLASHIF_PROTECTION_NONE;
    FLASH_OBProgramInitTypeDef OptionsBytesStruct;

    /* Unlock the Flash to enable the flash control register access */
    HAL_FLASH_Unlock();
    /* Check if there are write protected sectors inside the user flash area */
    HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct);
    /* Lock the Flash to disable the flash control register access */
    HAL_FLASH_Lock();
    /* Get pages already write protected */
    ulProtectedPage = ~(OptionsBytesStruct.WRPPage) & ulObWrpPages;
    /* Check if desired pages are already write protected */
    if (ulProtectedPage != 0) {
        /* Some sectors inside the user flash area are write protected */
        return FLASHIF_PROTECTION_WRPENABLED;
    } else {
        /* No write protected sectors inside the user flash area */
        return FLASHIF_PROTECTION_NONE;
    }
}

uint32_t prv_stm32_flash_if_set_wp_status(uint32_t ulObWrpPages, uint32_t ulStatus) {
    uint32_t ulProtectedPage = 0x0;
    FLASH_OBProgramInitTypeDef xCfgNew, xCfgOld;
    HAL_StatusTypeDef xResult = HAL_OK;

    /* Get pages write protection status */
    HAL_FLASHEx_OBGetConfig(&xCfgOld);
    /* The parameter says whether we turn the protection on or off */
    xCfgNew.WRPState = (ulStatus == FLASHIF_WRP_ENABLE ? OB_WRPSTATE_ENABLE : OB_WRPSTATE_DISABLE);
    /* We want to modify only the Write protection */
    xCfgNew.OptionType = OPTIONBYTE_WRP;
    /* No read protection, keep BOR and reset settings */
    xCfgNew.RDPLevel = OB_RDP_LEVEL_0;
    xCfgNew.USERConfig = xCfgOld.USERConfig;
    /* Get pages already write protected */
    ulProtectedPage = ~(xCfgOld.WRPPage) | ulObWrpPages;
    /* Unlock the Flash to enable the flash control register access */
    HAL_FLASH_Unlock();
    /* Unlock the Options Bytes */
    HAL_FLASH_OB_Unlock();
    /* Erase all the option Bytes */
    xResult = HAL_FLASHEx_OBErase();

    if (xResult == HAL_OK) {
        xCfgNew.WRPPage = ulProtectedPage;
        xResult = HAL_FLASHEx_OBProgram(&xCfgNew);
    }

    return ((xResult == HAL_OK) ? FLASHIF_OK : FLASHIF_PROTECTION_ERRROR);
}

static uint32_t prv_write_uint32(uint32_t ulAddr, uint32_t* pulSrc, uint32_t ulSizeInUint32) {
    uint32_t i = 0;

    for (i = 0; i < ulSizeInUint32; i++) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, ulAddr, *(uint32_t*)(pulSrc + i)) == HAL_OK) {
            /* Check the written value */
            if (*(uint32_t*)ulAddr != *(uint32_t*)(pulSrc + i)) {
                /* Flash content doesn't match SRAM content */
                return (FLASHIF_WRITINGCTRL_ERROR);
            }
            /* Increment FLASH destination address */
            ulAddr += 4;
        } else {
            /* Error occurred while writing data in Flash memory */
            return (FLASHIF_WRITING_ERROR);
        }
    }

    return (FLASHIF_OK);
}

#endif /* (MEM_ENABLE && MEM_ENABLE_STM32FLASH) */
