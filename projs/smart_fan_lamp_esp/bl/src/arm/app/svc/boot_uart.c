/*
    BootUart.c

    Implementation File for Boot Uart Module
*/

/* Copyright 2022 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 06Apr22, Karl Created
    01b, 10Apr22, Karl Added image verification utility
*/

/* Includes */
#include <string.h>
#include "app/svc/boot_uart.h"

#if BOOT_UART_ENABLE

/* Debug config */
#if BOOT_UART_DEBUG
    #undef TRACE
    #define TRACE(...)  DebugPrintf(__VA_ARGS__)
#else
    #undef TRACE
    #define TRACE(...)
#endif /* BOOT_UART_DEBUG */
#if BOOT_UART_ASSERT
    #undef ASSERT
    #define ASSERT(a)   while(!(a)){DebugPrintf("ASSERT failed: %s %d\n", __FILE__, __LINE__);}
#else
    #undef ASSERT
    #define ASSERT(...)
#endif /* BOOT_UART_ASSERT */

/* Pragmas */
#pragma diag_suppress 167   /* warning: #167-D: argument of type A is incompatible with parameter of type B */
#pragma diag_suppress 188   /* warning: #188-D: enumerated type mixed with another type */
#pragma diag_suppress 513   /* warning: #513-D: a value of type A cannot be assigned to an entity of type B */
#pragma diag_suppress 550   /* warning: #550-D: variable was set but never used */

/* Local defines */
#ifndef SECTOR_COUNT
#define SECTOR_COUNT            (128)
#endif /* SECTOR_COUNT */
#define PACKET_SEQNO_INDEX      (1)
#define PACKET_SEQNO_COMP_INDEX (2)
#define PACKET_HEADER           (3)     /* start, block, block-complement */
#define PACKET_TRAILER          (2)     /* CRC bytes */
#define PACKET_OVERHEAD         (PACKET_HEADER + PACKET_TRAILER)
#define PACKET_SIZE             (128)
#define PACKET_1K_SIZE          (1024)
#define PACKET_TIMEOUT          (1)
#define FILE_NAME_LENGTH        (64)
#define FILE_SIZE_LENGTH        (16)
/* ASCII control codes */
#define SOH                     (0x01)  /* start of 128-byte data packet */
#define STX                     (0x02)  /* start of 1024-byte data packet */
#define EOT                     (0x04)  /* end of transmission */
#define ACK                     (0x06)  /* receive OK */
#define NAK                     (0x15)  /* receiver error; retry */
#define CAN                     (0x18)  /* two of these in succession aborts transfer */
#undef  CRC
#define CRC                     (0x43)  /* use in place of first NAK for CRC mode */
/* Number of consecutive receive errors before giving up: */
#define MAX_ERRORS              (5)

/* Forward declarations */
static void prvBootUartDone(void);
static uint8_t prvFlashAddrToSector(uint32_t ulAddr);
static void prvFlashEraseSector(uint32_t page);
static int receive_packet(char *data, int *length);
static unsigned long ymodem_receive(void);
static void put_char(int c);
static int get_char(int timeout);
static void sleep(int delay/*s*/);
static unsigned short crc16(const unsigned char *buf, unsigned long count);
static unsigned long str_to_u32(char* str);

/* Local variables */
static UART_HandleTypeDef s_hUart;
static BootUartDone_t     s_pxDone = NULL;
static uint8_t            s_ucSectorErased[SECTOR_COUNT];
static uint32_t           s_ulErrorCnt = 0;

/* Functions */
status_t BootUartInit(void)
{
    /* Do nothing */
    return status_ok;
}

status_t BootUartTerm(void)
{
    /* Do nothing */
    return status_ok;
}

status_t BootUartConfig(const BootUartConfig_t *pxConfig)
{
    ASSERT(pxConfig);
    
    /* Enable Rx clock */
    switch((uint32_t)pxConfig->pxRxPinPort) {
    case (uint32_t)GPIOA: __HAL_RCC_GPIOA_CLK_ENABLE(); break;
    case (uint32_t)GPIOB: __HAL_RCC_GPIOB_CLK_ENABLE(); break;
    case (uint32_t)GPIOC: __HAL_RCC_GPIOC_CLK_ENABLE(); break;
    case (uint32_t)GPIOD: __HAL_RCC_GPIOD_CLK_ENABLE(); break;
    case (uint32_t)GPIOE: __HAL_RCC_GPIOE_CLK_ENABLE(); break;
    default: ASSERT(0); break;
    }
    /* Config Rx gpio pin */
    GPIO_InitTypeDef xGpioRxPin = {0};
    xGpioRxPin.Pin       = pxConfig->ulRxPin;
    xGpioRxPin.Mode      = GPIO_MODE_INPUT;
    xGpioRxPin.Pull      = GPIO_PULLUP;
    xGpioRxPin.Speed     = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(pxConfig->pxRxPinPort, &xGpioRxPin);
    
    /* Enable Tx clock */
    switch((uint32_t)pxConfig->pxTxPinPort) {
    case (uint32_t)GPIOA: __HAL_RCC_GPIOA_CLK_ENABLE(); break;
    case (uint32_t)GPIOB: __HAL_RCC_GPIOB_CLK_ENABLE(); break;
    case (uint32_t)GPIOC: __HAL_RCC_GPIOC_CLK_ENABLE(); break;
    case (uint32_t)GPIOD: __HAL_RCC_GPIOD_CLK_ENABLE(); break;
    case (uint32_t)GPIOE: __HAL_RCC_GPIOE_CLK_ENABLE(); break;
    default: ASSERT(0); break;
    }
    
    /* Config Tx gpio pin */
    GPIO_InitTypeDef xGpioTxPin = {0};
    xGpioTxPin.Pin       = pxConfig->ulTxPin;
    xGpioTxPin.Mode      = GPIO_MODE_AF_PP;
    xGpioTxPin.Pull      = GPIO_PULLUP;
    xGpioTxPin.Speed     = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(pxConfig->pxTxPinPort, &xGpioTxPin);
    
    /* Enable UART clock */
    switch ((uint32_t)pxConfig->pxInstance) {
    case (uint32_t)USART1: __HAL_RCC_USART1_CLK_ENABLE(); break;
    case (uint32_t)USART2: __HAL_RCC_USART2_CLK_ENABLE(); break;
    case (uint32_t)USART3: __HAL_RCC_USART3_CLK_ENABLE(); break;
    case (uint32_t)UART4:  __HAL_RCC_UART4_CLK_ENABLE();  break;
    case (uint32_t)UART5:  __HAL_RCC_UART5_CLK_ENABLE();  break;
    default: ASSERT(0); break;
    }
    
    /* Config UART parameter */
    s_hUart.Instance          = pxConfig->pxInstance;
    s_hUart.Init.BaudRate     = pxConfig->ulBaudRate;
    s_hUart.Init.WordLength   = pxConfig->ulWordLength;
    s_hUart.Init.StopBits     = pxConfig->ulStopBits;
    s_hUart.Init.Parity       = pxConfig->ulParity;
    s_hUart.Init.Mode         = UART_MODE_TX_RX;
    s_hUart.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    s_hUart.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&(s_hUart)) != HAL_OK) {
        /* We should never get here! */
        ASSERT(0);
    }
    
    /* Init members */
    if (pxConfig->pxDoneFunc) {
        s_pxDone = pxConfig->pxDoneFunc;
    }
    else {
        s_pxDone = prvBootUartDone;
    }
    
    /* Flash erase sectors */
    memset(s_ucSectorErased, 0, sizeof(s_ucSectorErased));
    HAL_FLASH_Unlock();
    for (uint32_t address = BOOT_UART_APP_ADDR; address < (BOOT_UART_APP_ADDR + BOOT_UART_APP_SIZE); address += BOOT_UART_FLASH_PAGE_SIZE) {
        prvFlashEraseSector(address);
    }
    HAL_FLASH_Lock();
    
    return status_ok;
}

status_t BootUartRun(void)
{
    ymodem_receive();
    /* Reset system */
    NVIC_SystemReset();
    /* Never get here */
    return status_ok;
}

static void prvBootUartDone(void)
{
    /* Default boot uart done callback function */
    while(1);
}

static uint8_t prvFlashAddrToSector(uint32_t ulAddr)
{
    return ((ulAddr & 0xFFFFF) / FLASH_PAGE_SIZE);
}

static void prvFlashEraseSector(uint32_t page)
{
    uint8_t sector = prvFlashAddrToSector(page);
    if (s_ucSectorErased[sector] == 0) {
        s_ucSectorErased[sector] = 1;
        HAL_FLASH_Unlock();
        FLASH_EraseInitTypeDef xEraseInit;
        uint32_t ulPageError   = 0;
        xEraseInit.TypeErase   = FLASH_TYPEERASE_PAGES;
        xEraseInit.PageAddress = page;
        xEraseInit.Banks       = FLASH_BANK_1;
        xEraseInit.NbPages     = 1;
        HAL_FLASHEx_Erase(&xEraseInit, &ulPageError);
        HAL_FLASH_Lock();
    }
}

/* Returns 0 on success, 1 on corrupt packet, -1 on error (timeout): */
static int receive_packet(char *data, int *length)
{
    int i, c;
    unsigned int packet_size;

    *length = 0;

    c = get_char(PACKET_TIMEOUT);
    if (c < 0) {
        return -1;
    }

    switch(c) {
    case SOH:
        packet_size = PACKET_SIZE;
        break;
    case STX:
        packet_size = PACKET_1K_SIZE;
        break;
    case EOT:
        return 0;
    case CAN:
        c = get_char(PACKET_TIMEOUT);
        if (c == CAN) {
            *length = -1;
            return 0;
        }
    default:
        /* This case could be the result of corruption on the first octet
        * of the packet, but it's more likely that it's the user banging
        * on the terminal trying to abort a transfer. Technically, the
        * former case deserves a NAK, but for now we'll just treat this
        * as an abort case.
        */
        *length = -1;
        return 0;
    }

    *data = (char)c;

    for(i = 1; i < (packet_size + PACKET_OVERHEAD); ++i) {
        c = get_char(PACKET_TIMEOUT);
        if (c < 0) {
            return -1;
        }
        data[i] = (char)c;
    }

    /* Just a sanity check on the sequence number/complement value.
     * Caller should check for in-order arrival.
     */
    if (data[PACKET_SEQNO_INDEX] != ((data[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff)) {
        return 1;
    }

    if (crc16(data + PACKET_HEADER, packet_size + PACKET_TRAILER) != 0) {
        return 1;
    }
    TRACE("receive_packet%d\n", packet_size);
    *length = packet_size;

    return 0;
}

static unsigned long ymodem_receive(void)
{
    static unsigned char packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD];
    int packet_length, i, file_done, session_done, crc_tries, crc_nak;
    unsigned int packets_received, errors, first_try = 1;
    char file_name[FILE_NAME_LENGTH], file_size[FILE_SIZE_LENGTH], *file_ptr;
    unsigned long size = 0;
    unsigned long offset = 0;

    TRACE("ymodem rcv:\n");
    file_name[0] = 0;

    for (session_done = 0, errors = 0; ; ) {
        crc_tries = crc_nak = 1;
        if (!first_try) {
            put_char(CRC);
        }
        first_try = 0;
        for (packets_received = 0, file_done = 0; ; ) {
            switch (receive_packet(packet_data, &packet_length)) {

            case 0:
                errors = 0;
                switch (packet_length) {
                    case -1:  /* abort */
                        put_char(ACK);
                        TRACE("\nreceive_packet timeout - aborted.\n");
                        return 0;
                    case 0:   /* end of transmission */
                        put_char(ACK);
                        /* Should add some sort of sanity check on the number of
                         * packets received and the advertised file length.
                         */
                        file_done = 1;
                        if (s_pxDone && (0 == s_ulErrorCnt)) {
                            s_pxDone();
                        }
                        break;
                    default:  /* normal packet */
                    if ((packet_data[PACKET_SEQNO_INDEX] & 0xff) != (packets_received & 0xff)) {
                        put_char(NAK);
                    }
                    else {
                        if (packets_received == 0) {
                            /* The spec suggests that the whole data section should
                             * be zeroed, but I don't think all senders do this. If
                             * we have a NULL filename and the first few digits of
                             * the file length are zero, we'll call it empty.
                             */
                            for (i = PACKET_HEADER; i < PACKET_HEADER + 4; i++) {
                                if (packet_data[i] != 0) {
                                    break;
                                }
                            }
                            if (i < PACKET_HEADER + 4) {  /* filename packet has data */
                                for (file_ptr = packet_data + PACKET_HEADER, i = 0; *file_ptr && i < FILE_NAME_LENGTH; ) {
                                    file_name[i++] = *file_ptr++;
                                }
                                file_name[i++] = '\0';
                                for (++file_ptr, i = 0; *file_ptr != ' ' && i < FILE_SIZE_LENGTH; ) {
                                    file_size[i++] = *file_ptr++;
                                }
                                file_size[i++] = '\0';
                                size = str_to_u32(file_size);
                                put_char(ACK);
                                put_char(crc_nak ? CRC : NAK);
                                crc_nak = 0;
                            }
                            else {  /* filename packet is empty; end session */
                                put_char(ACK);
                                file_done = 1;
                                session_done = 1;
                                break;
                            }
                        }
                        else {
                            /* get the data and write to the storage */
                            HAL_FLASH_Unlock();
                        #if 0
                            for (size_t i = 0; i < packet_length; i++) {
                                HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, (uint32_t)(BOOT_UART_APP_ADDR + offset + i), *(uint8_t*)(packet_data + PACKET_HEADER + i));
                            }
                        #else
                            packet_length += (packet_length % 4) ? (4 - (packet_length % 4)) : 0;
                            for (size_t i = 0; i < packet_length; i += 4) {
                                HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (uint32_t)(BOOT_UART_APP_ADDR + offset + i), *(uint32_t*)(packet_data + PACKET_HEADER + i));
                            }
                        #endif
                            HAL_FLASH_Lock();
                            if (0 != memcmp((void*)(BOOT_UART_APP_ADDR + offset), (packet_data + PACKET_HEADER), packet_length)) {
                                s_ulErrorCnt++;
                            }
                            offset += packet_length;
                            put_char(ACK);
                        }
                        ++packets_received;
                    }  /* sequence number ok */
                }
                break;

            default:
                if (packets_received != 0) {
                    if (++errors >= MAX_ERRORS) {
                        put_char(CAN);
                        put_char(CAN);
                        sleep(1);
                        TRACE("\ntoo many errors - aborted.\n");
                        return 0;
                    }
                }
                put_char(CRC);
            }
            if (file_done) {
                TRACE("file_done\n");
                break;
            }
        }  /* receive packets */

        if (session_done) {
            TRACE("session_done\n");
            break;
        }

    }  /* receive files */

    TRACE("\n");
    if (size > 0) {
        TRACE("read:%s\n", file_name);
        TRACE("len:0x%08x\n", size);
    }
    return size;
}

static void put_char(int c)
{
    HAL_UART_StateTypeDef state = HAL_UART_GetState(&s_hUart) & HAL_UART_STATE_BUSY_TX;
    while (state == HAL_UART_STATE_BUSY_TX) {
        state = HAL_UART_GetState(&s_hUart) & HAL_UART_STATE_BUSY_TX;
    }
    
    uint8_t data = (uint8_t)c;
    HAL_UART_Transmit(&s_hUart, (uint8_t *)&data, 1, 100);
    
    return;
}

static int get_char(int timeout)
{
    while (HAL_UART_STATE_BUSY_RX == (HAL_UART_GetState(&s_hUart) & HAL_UART_STATE_BUSY_RX)){};

    uint8_t data;
    HAL_StatusTypeDef state = HAL_UART_Receive(&s_hUart, &data, 1, 100);
    
    if (HAL_OK == state) {
        return (int)data;
    }
    else {
        return -1;
    }
}

static void sleep(int delay/*s*/)
{
    TickDelay(delay * 1000);
    return;
}

static unsigned short crc16(const unsigned char *buf, unsigned long count)
{
    unsigned short crc = 0;
    int i;

    while(count--) {
        crc = crc ^ *buf++ << 8;

        for (i=0; i<8; i++) {
            if (crc & 0x8000) {
                crc = crc << 1 ^ 0x1021;
            }
            else {
                crc = crc << 1;
            }
        }
    }
    return crc;
}

static unsigned long str_to_u32(char* str)
{
    const char *s = str;
    unsigned long acc;
    int c;

    /* strip leading spaces if any */
    do {
        c = *s++;
    } while (c == ' ');

    for (acc = 0; (c >= '0') && (c <= '9'); c = *s++) {
        c -= '0';
        acc *= 10;
        acc += c;
    }
    return acc;
}

#endif /* BOOT_UART_ENABLE */
