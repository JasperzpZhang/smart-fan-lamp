/*
    MemFram.c

    Implementation File for Mem Fram Module
*/

/* Copyright 2019 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 01Jul19, Ziv Created
    01b, 24Jul19, Karl Reconstructured Mem lib
    01c, 09Mar20, Karl Added address width & address mask
    01d, 11May20, Karl Fixed merge errors
*/

/* Includes */
#include <main.h>
#include "drv/peri/mem/drv_mem_fram.h"

#if (MEM_ENABLE && MEM_ENABLE_FRAM)

/* Pragmas */
#pragma diag_suppress 550 /* warning: #550-D: variable was set but never used */
    
/* Debug config */
#if MEM_DEBUGx
    #undef TRACE
    #define TRACE(...)  DebugPrintf(__VA_ARGS__)
#else
    #undef TRACE
    #define TRACE(...)
#endif /* MEM_DEBUGx */
#if MEM_ASSERT
    #undef ASSERT
    #define ASSERT(a)   while(!(a)){DebugPrintf("ASSERT failed: %s %d\n", __FILE__, __LINE__);}
#else
    #undef ASSERT
    #define ASSERT(...)
#endif /* MEM_ASSERT */

/* Local defines */
/* Fram opcode */
#define REG_WREN            (0x06)      /* Set write enable latch */
#define REG_WRDI            (0x04)      /* Reset write enable latch */
#define REG_RDSR            (0x05)      /* Read Status Register */
#define REG_WRSR            (0x01)      /* Write Status Register */
#define REG_READ            (0x03)      /* Read memory data */
#define REG_FSTRD           (0x0B)      /* Fast read memory data */
#define REG_WRITE           (0x02)      /* Write memory data */
#define REG_SLEEP           (0xB9)      /* Enter sleep mode */
#define REG_RDID            (0x9F)      /* Read device ID */
#define REG_SNR             (0xC3)      /* Read S/N */
#define DUMMY_DATA          (0x00)      /* Empty Data Use For Fast Read And Write*/

#define FRAM_CS_H(pxCtrl)   HAL_GPIO_WritePin(pxCtrl->xConfig.xCs.pxPinPort, pxCtrl->xConfig.xCs.usPin, GPIO_PIN_SET)
#define FRAM_CS_L(pxCtrl)   HAL_GPIO_WritePin(pxCtrl->xConfig.xCs.pxPinPort, pxCtrl->xConfig.xCs.usPin, GPIO_PIN_RESET)

/* Local types */
typedef enum {
    FRAM_WP_NONE,
    FRAM_WP_UPPER_QUARTER,
    FRAM_WP_UPPER_HALF,
    FRAM_WP_UPPER_WHOLE,
}FramWriteProtect_t;

/* Forward declaration */
static uint8_t      prvReadByte             (MemCtrl_t *pxCtrl, uint32_t ulAddr);
static status_t     prvWriteByte            (MemCtrl_t *pxCtrl, uint32_t ulAddr, uint8_t ucData);
static status_t     prvReadBulk             (MemCtrl_t *pxCtrl, uint32_t ulAddr, uint32_t ulLength, OUT uint8_t *pucData);
static status_t     prvWriteBulk            (MemCtrl_t *pxCtrl, uint32_t ulAddr, uint32_t ulLength, IN uint8_t *pucData);
static uint8_t      prvReadStatus           (MemCtrl_t *pxCtrl);
static status_t     prvWriteStatus          (MemCtrl_t *pxCtrl, uint8_t ucData);
static status_t     prvWriteProtect         (MemCtrl_t *pxCtrl, FramWriteProtect_t xWp);
static uint8_t      prvSpiReadWriteByte     (MemCtrl_t *pxCtrl, uint8_t ucData);

/* Functions */
status_t FramInit(void)
{
    /* Do nothing */
    return status_ok;
}

status_t FramTerm(void)
{
    /* Do nothing */
    return status_ok;
}

status_t FramConfig(MemHandle_t xHandle, MemConfig_t *pxConfig)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    MemCtrl_t *pxCtrl = MEM_GET_CTRL(xHandle);
    ASSERT(NULL != pxCtrl);
    
    /* Config paras */
    pxCtrl->xConfig = *pxConfig;
    pxCtrl->bConfig = TRUE;
    
    /* Init spi */
    ASSERT(pxCtrl != NULL);
    pxCtrl->hSpi.Instance                = pxConfig->pxSpi;
    pxCtrl->hSpi.Init.Mode               = SPI_MODE_MASTER;
    pxCtrl->hSpi.Init.Direction          = SPI_DIRECTION_2LINES;
    pxCtrl->hSpi.Init.DataSize           = SPI_DATASIZE_8BIT;
    pxCtrl->hSpi.Init.CLKPolarity        = SPI_POLARITY_LOW;
    pxCtrl->hSpi.Init.CLKPhase           = SPI_PHASE_1EDGE;
    pxCtrl->hSpi.Init.NSS                = SPI_NSS_SOFT;
    pxCtrl->hSpi.Init.BaudRatePrescaler  = SPI_BAUDRATEPRESCALER_8;
    pxCtrl->hSpi.Init.FirstBit           = SPI_FIRSTBIT_MSB;
    pxCtrl->hSpi.Init.TIMode             = SPI_TIMODE_DISABLE;
    pxCtrl->hSpi.Init.CRCCalculation     = SPI_CRCCALCULATION_DISABLE;
    pxCtrl->hSpi.Init.CRCPolynomial      = 10;
    if (HAL_SPI_Init(&(pxCtrl->hSpi)) != HAL_OK) {
        ASSERT(0); /* We should never get here */
    }
    pxCtrl->phSpi = &(pxCtrl->hSpi);
    
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
    GPIO_InitStruct.Pin   = pxCtrl->xConfig.xCs.usPin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(pxCtrl->xConfig.xCs.pxPinPort, &GPIO_InitStruct);
    
    return status_ok;
}

status_t FramCtrl(MemHandle_t xHandle, MemCtrlOperation_t xCtrlOp, uint32_t ulExtraPara)
{
    status_t xRet;
    MemCtrl_t *pxCtrl = MEM_GET_CTRL(xHandle);
    ASSERT(NULL != pxCtrl);
    switch (xCtrlOp) {
    case MEM_CTRL_OP_FRAM_WP_NONE:
        xRet = prvWriteProtect(pxCtrl, FRAM_WP_NONE);
        break;
    case MEM_CTRL_OP_FRAM_WP_QUARTER:
        xRet = prvWriteProtect(pxCtrl, FRAM_WP_UPPER_QUARTER);
        break;
    case MEM_CTRL_OP_FRAM_WP_HALF:
        xRet = prvWriteProtect(pxCtrl, FRAM_WP_UPPER_HALF);
        break;
    case MEM_CTRL_OP_FRAM_WP_WHOLE:
        xRet = prvWriteProtect(pxCtrl, FRAM_WP_UPPER_WHOLE);
        break;
    default:
        xRet = status_err;
        break;
    }
    return xRet;
}

status_t FramRead(MemHandle_t xHandle, uint32_t ulAddr, uint32_t ulLength, OUT uint8_t *pucData)
{
    MemCtrl_t *pxCtrl = MEM_GET_CTRL(xHandle);
    ASSERT(NULL != pxCtrl);
    if ((ulAddr < pxCtrl->xConfig.ulStAddr) || ((ulAddr + ulLength) > pxCtrl->xConfig.ulEdAddr)) {
        return status_err;
    }
    else {
        if (1 == ulLength) {
            *pucData = prvReadByte(pxCtrl, ulAddr);
        }
        else {
            prvReadBulk(pxCtrl, ulAddr, ulLength, pucData);
        }
        return status_ok;
    }
}

status_t FramWrite(MemHandle_t xHandle, uint32_t ulAddr, uint32_t ulLength, IN uint8_t *pucData)
{
    MemCtrl_t *pxCtrl = MEM_GET_CTRL(xHandle);
    ASSERT(NULL != pxCtrl);
    if ((ulAddr < pxCtrl->xConfig.ulStAddr) || ((ulAddr + ulLength) > pxCtrl->xConfig.ulEdAddr)) {
        return status_err;
    }
    else {
        if (1 == ulLength) {
            prvWriteByte(pxCtrl, ulAddr, *pucData);
        }
        else {
            prvWriteBulk(pxCtrl, ulAddr, ulLength, pucData);
        }
        return status_ok;
    }
}

uint8_t FramReadStatus(MemHandle_t xHandle)
{
    MemCtrl_t *pxCtrl = MEM_GET_CTRL(xHandle);
    ASSERT(NULL != pxCtrl);
    return prvReadStatus(pxCtrl);
}

status_t FramShowStatus(uint8_t ucStatus)
{
    Framstatus_t xStatus;
    xStatus.ucData = ucStatus;
    TRACE("FramShowStatus:\n");
    TRACE("    WPEN : %d\n", xStatus.xBit.WPEN);
    TRACE("    BP   : %d\n", xStatus.xBit.BP);
    TRACE("    WEL  : %d\n", xStatus.xBit.WEL);
    return status_ok;
}

static uint8_t prvReadByte(MemCtrl_t *pxCtrl, uint32_t ulAddr)
{
    uint8_t ucData;
#if (MEM_FRAM_ADDR_WIDTH > 24)
    uint8_t ucAddrT;
#endif /* (MEM_FRAM_ADDR_WIDTH > 24) */
#if (MEM_FRAM_ADDR_WIDTH > 16)
    uint8_t ucAddrH;
#endif /* (MEM_FRAM_ADDR_WIDTH > 16) */
    uint8_t ucAddrM, ucAddrL;
    
    /* Setup address */
    ulAddr &= MEM_FRAM_ADDR_MASK;
#if (MEM_FRAM_ADDR_WIDTH > 24)
    ucAddrT = (uint8_t)((ulAddr & 0xff000000)>>24);
#endif /* (MEM_FRAM_ADDR_WIDTH > 24) */
#if (MEM_FRAM_ADDR_WIDTH > 16)
    ucAddrH = (uint8_t)((ulAddr & 0x00ff0000)>>16);
#endif /* (MEM_FRAM_ADDR_WIDTH > 16) */
    ucAddrM = (uint8_t)((ulAddr & 0x0000ff00)>>8);
    ucAddrL = (uint8_t)((ulAddr & 0x000000ff));

    FRAM_CS_L(pxCtrl);
    /* Execute read command */
    prvSpiReadWriteByte(pxCtrl, REG_READ);
    /* Write address */
#if (MEM_FRAM_ADDR_WIDTH > 24)
    prvSpiReadWriteByte(pxCtrl, ucAddrT);
#endif /* (MEM_FRAM_ADDR_WIDTH > 24) */
#if (MEM_FRAM_ADDR_WIDTH > 16)
    prvSpiReadWriteByte(pxCtrl, ucAddrH);
#endif /* (MEM_FRAM_ADDR_WIDTH > 16) */
    prvSpiReadWriteByte(pxCtrl, ucAddrM);
    prvSpiReadWriteByte(pxCtrl, ucAddrL);
    /* Read data */
    ucData = prvSpiReadWriteByte(pxCtrl, DUMMY_DATA);
    FRAM_CS_H(pxCtrl);

    return ucData;
}

static status_t prvWriteByte(MemCtrl_t *pxCtrl, uint32_t ulAddr, uint8_t ucData)
{
#if (MEM_FRAM_ADDR_WIDTH > 24)
    uint8_t ucAddrT;
#endif /* (MEM_FRAM_ADDR_WIDTH > 24) */
#if (MEM_FRAM_ADDR_WIDTH > 16)
    uint8_t ucAddrH;
#endif /* (MEM_FRAM_ADDR_WIDTH > 16) */
    uint8_t ucAddrM, ucAddrL;
 
    /* Setup address */
    ulAddr &= MEM_FRAM_ADDR_MASK;
#if (MEM_FRAM_ADDR_WIDTH > 24)
    ucAddrT = (uint8_t)((ulAddr & 0xff000000)>>24);
#endif /* (MEM_FRAM_ADDR_WIDTH > 24) */
#if (MEM_FRAM_ADDR_WIDTH > 16)
    ucAddrH = (uint8_t)((ulAddr & 0x00ff0000)>>16);
#endif /* (MEM_FRAM_ADDR_WIDTH > 16) */
    ucAddrM = (uint8_t)((ulAddr & 0x0000ff00)>>8);
    ucAddrL = (uint8_t)((ulAddr & 0x000000ff));
    
    FRAM_CS_L(pxCtrl);
    /* Enable write */
    prvSpiReadWriteByte(pxCtrl, REG_WREN);
    FRAM_CS_H(pxCtrl);
    
    FRAM_CS_L(pxCtrl);
    /* Execute write command */
    prvSpiReadWriteByte(pxCtrl, REG_WRITE);
    /* Write address */
#if (MEM_FRAM_ADDR_WIDTH > 24)
    prvSpiReadWriteByte(pxCtrl, ucAddrT);
#endif /* (MEM_FRAM_ADDR_WIDTH > 24) */
#if (MEM_FRAM_ADDR_WIDTH > 16)
    prvSpiReadWriteByte(pxCtrl, ucAddrH);
#endif /* (MEM_FRAM_ADDR_WIDTH > 16) */
    prvSpiReadWriteByte(pxCtrl, ucAddrM);
    prvSpiReadWriteByte(pxCtrl, ucAddrL);
    /* Write data */
    prvSpiReadWriteByte(pxCtrl, ucData);
    FRAM_CS_H(pxCtrl);
    
    FRAM_CS_L(pxCtrl);
    /* Disable write */
    prvSpiReadWriteByte(pxCtrl, REG_WRDI);
    FRAM_CS_H(pxCtrl);
    
    return status_ok;
}

static status_t prvReadBulk(MemCtrl_t *pxCtrl, uint32_t ulAddr, uint32_t ulLength, OUT uint8_t *pucData)
{
    uint32_t n;
#if (MEM_FRAM_ADDR_WIDTH > 24)
    uint8_t ucAddrT;
#endif /* (MEM_FRAM_ADDR_WIDTH > 24) */
#if (MEM_FRAM_ADDR_WIDTH > 16)
    uint8_t ucAddrH;
#endif /* (MEM_FRAM_ADDR_WIDTH > 16) */
    uint8_t ucAddrM, ucAddrL;

    /* Setup address */
    ulAddr &= MEM_FRAM_ADDR_MASK;
#if (MEM_FRAM_ADDR_WIDTH > 24)
    ucAddrT = (uint8_t)((ulAddr & 0xff000000)>>24);
#endif /* (MEM_FRAM_ADDR_WIDTH > 24) */
#if (MEM_FRAM_ADDR_WIDTH > 16)
    ucAddrH = (uint8_t)((ulAddr & 0x00ff0000)>>16);
#endif /* (MEM_FRAM_ADDR_WIDTH > 16) */
    ucAddrM = (uint8_t)((ulAddr & 0x0000ff00)>>8);
    ucAddrL = (uint8_t)((ulAddr & 0x000000ff));
    
    FRAM_CS_L(pxCtrl);
    /* Execute read command */
    prvSpiReadWriteByte(pxCtrl, REG_READ);
    /* Write address */
#if (MEM_FRAM_ADDR_WIDTH > 24)
    prvSpiReadWriteByte(pxCtrl, ucAddrT);
#endif /* (MEM_FRAM_ADDR_WIDTH > 24) */
#if (MEM_FRAM_ADDR_WIDTH > 16)
    prvSpiReadWriteByte(pxCtrl, ucAddrH);
#endif /* (MEM_FRAM_ADDR_WIDTH > 16) */
    prvSpiReadWriteByte(pxCtrl, ucAddrM);
    prvSpiReadWriteByte(pxCtrl, ucAddrL);
    /* Read datas */
    for (n = 0; n < ulLength; n++) {
        *pucData++ = prvSpiReadWriteByte(pxCtrl, DUMMY_DATA);
    }
    FRAM_CS_H(pxCtrl);
    
    return status_ok;
}

static status_t prvWriteBulk(MemCtrl_t *pxCtrl, uint32_t ulAddr, uint32_t ulLength, IN uint8_t *pucData)
{
    uint32_t n;
#if (MEM_FRAM_ADDR_WIDTH > 24)
    uint8_t ucAddrT;
#endif /* (MEM_FRAM_ADDR_WIDTH > 24) */
#if (MEM_FRAM_ADDR_WIDTH > 16)
    uint8_t ucAddrH;
#endif /* (MEM_FRAM_ADDR_WIDTH > 16) */
    uint8_t ucAddrM, ucAddrL;

    /* Setup address */
    ulAddr &= MEM_FRAM_ADDR_MASK;
#if (MEM_FRAM_ADDR_WIDTH > 24)
    ucAddrT = (uint8_t)((ulAddr & 0xff000000)>>24);
#endif /* (MEM_FRAM_ADDR_WIDTH > 24) */
#if (MEM_FRAM_ADDR_WIDTH > 16)
    ucAddrH = (uint8_t)((ulAddr & 0x00ff0000)>>16);
#endif /* (MEM_FRAM_ADDR_WIDTH > 16) */
    ucAddrM = (uint8_t)((ulAddr & 0x0000ff00)>>8);
    ucAddrL = (uint8_t)((ulAddr & 0x000000ff));

    FRAM_CS_L(pxCtrl);
    /* Enable write */
    prvSpiReadWriteByte(pxCtrl, REG_WREN);
    FRAM_CS_H(pxCtrl);
    
    FRAM_CS_L(pxCtrl);
    /* Execute write command */
    prvSpiReadWriteByte(pxCtrl, REG_WRITE);
    /* Write address */
#if (MEM_FRAM_ADDR_WIDTH > 24)
    prvSpiReadWriteByte(pxCtrl, ucAddrT);
#endif /* (MEM_FRAM_ADDR_WIDTH > 24) */
#if (MEM_FRAM_ADDR_WIDTH > 16)
    prvSpiReadWriteByte(pxCtrl, ucAddrH);
#endif /* (MEM_FRAM_ADDR_WIDTH > 16) */
    prvSpiReadWriteByte(pxCtrl, ucAddrM);
    prvSpiReadWriteByte(pxCtrl, ucAddrL);
    /* Write datas */
    for (n = 0; n < ulLength; n++) {
        prvSpiReadWriteByte(pxCtrl, *pucData++);
    }
    FRAM_CS_H(pxCtrl);
    
    FRAM_CS_L(pxCtrl);
    /* Disable write */
    prvSpiReadWriteByte(pxCtrl, REG_WRDI);
    FRAM_CS_H(pxCtrl);
    
    return status_ok;
}

static uint8_t prvReadStatus(MemCtrl_t *pxCtrl)
{
    uint8_t ucData;

    FRAM_CS_L(pxCtrl);
    /* Execute read command */
    prvSpiReadWriteByte(pxCtrl, REG_RDSR);
    ucData = prvSpiReadWriteByte(pxCtrl, DUMMY_DATA);
    FRAM_CS_H(pxCtrl);

    return ucData;
}

static status_t prvWriteStatus(MemCtrl_t *pxCtrl, uint8_t ucData)
{
    FRAM_CS_L(pxCtrl);
    /* Enable write */
    prvSpiReadWriteByte(pxCtrl, REG_WREN);
    FRAM_CS_H(pxCtrl);

    FRAM_CS_L(pxCtrl);
    /* Execute write command */
    prvSpiReadWriteByte(pxCtrl, REG_WRSR);
    ucData = prvSpiReadWriteByte(pxCtrl, ucData);
    FRAM_CS_H(pxCtrl);

    FRAM_CS_L(pxCtrl);
    /* Disable write */
    prvSpiReadWriteByte(pxCtrl, REG_WRDI);
    FRAM_CS_H(pxCtrl);

    return status_ok;
}

static status_t prvWriteProtect(MemCtrl_t *pxCtrl, FramWriteProtect_t xWp)
{
    Framstatus_t xStatus;
    xStatus.ucData = prvReadStatus(pxCtrl);
    switch (xWp) {
    case FRAM_WP_NONE:
        xStatus.xBit.BP = 0;
        break;
    case FRAM_WP_UPPER_QUARTER:
        xStatus.xBit.BP = 1;
        break;
    case FRAM_WP_UPPER_HALF:
        xStatus.xBit.BP = 2;
        break;
    case FRAM_WP_UPPER_WHOLE:
        xStatus.xBit.BP = 3;
        break;
    default:
        ASSERT(0);  /* We should never get here! */
        break;
    }
    /* Disable the function of Write Protect Pin */
    xStatus.xBit.WPEN = 0;
    prvWriteStatus(pxCtrl, xStatus.ucData);
    return status_ok;
}

static uint8_t prvSpiReadWriteByte(MemCtrl_t *pxCtrl, uint8_t ucData)
{
    uint8_t ucRecvd = 0;
    HAL_SPI_TransmitReceive(pxCtrl->phSpi, &ucData, &ucRecvd, 1, 1000);
    return ucRecvd;
}

#if MEM_FRAM_ENABLE_MSP

/* XXX: MemFram.c -> HAL_SPI_MspInit only applies to 50-0211-034-X1.0 Board! */
void HAL_SPI_MspInit(SPI_HandleTypeDef* pxSpiHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    if (SPI3 == pxSpiHandle->Instance) {
        /* Peripheral clock enable */
        __HAL_RCC_SPI3_CLK_ENABLE();

        /*
         * SPI3 GPIO Configuration
         * PB3 -> SPI3_SCK
         * PB4 -> SPI3_MISO
         * PB5 -> SPI3_MOSI
         */
        GPIO_InitStruct.Pin   = GPIO_PIN_3|GPIO_PIN_5;
        GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        GPIO_InitStruct.Pin  = GPIO_PIN_4;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(Flash_MISO_Pin_Port, &GPIO_InitStruct);
    }
}

/* XXX: MemFram.c -> HAL_SPI_MspDeInit only applies to 50-0211-034-X1.0 Board! */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* pxSpiHandle)
{
    if (SPI3 == pxSpiHandle->Instance) {
        /* Peripheral clock disable */
        __HAL_RCC_SPI3_CLK_DISABLE();

        /*
         * SPI3 GPIO Configuration
         * PB3 -> SPI3_SCK
         * PB4 -> SPI3_MISO
         * PB5 -> SPI3_MOSI
         */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5);
    }
}

#endif /* #if MEM_FRAM_ENABLE_MSP */

#endif /* (MEM_ENABLE && MEM_ENABLE_FRAM) */
