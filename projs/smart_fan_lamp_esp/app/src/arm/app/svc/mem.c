/**
 * \file            mem.c
 * \brief           Memory file
 */

/*
 * Copyright (c) 2024 Jasper
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is the entire memory application.
 *
 * Author:          Jasper <jasperzhangse@gmail.com>
 * Version:         v1.0.0-dev
 */

/* Includes */
#include "app/include.h"

/* Debug config */
#if MEM_DEBUG
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* MEM_DEBUG */
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

/* Local variables */
static MemHandle_t s_xMem;

/* Functions */
status_t
mem_init(void) {
    MemInit(MEM_DEVICE_SPIFLASH);
    s_xMem = MemCreate(MEM_DEVICE_SPIFLASH);

    MemConfig_t xConfig;
    MemConfigInit(&xConfig);
    MemConfigSpi(&xConfig, MEM_FLASH_SPI);
    MemConfigCsPin(&xConfig, PORT(SPI3_FLASH_CS), PIN(SPI3_FLASH_CS));
    MemConfigCapacity(&xConfig, 0 /*Start*/, MEM_FLASH_CAPACITY /*End*/, MEM_FLASH_CAPACITY /*Capacity*/);
    MemConfig(s_xMem, &xConfig);

    return status_ok;
}

status_t
mem_term(void) {
    /* Do nothing */
    return status_ok;
}

status_t
mem_flash_read(uint32_t ulAddr, uint32_t ulLength, OUT uint8_t* pucData) {
    return MemRead(s_xMem, ulAddr, ulLength, pucData);
}

status_t
mem_flash_write(uint32_t ulAddr, uint32_t ulLength, IN uint8_t* pucData) {
    return MemEraseWrite(s_xMem, ulAddr, ulLength, pucData);
}

static uint8_t*
prvParseHexStr(const char* pcStr, uint8_t* pucLength) {
    static uint8_t ucBuffer[80];
    uint8_t ucData = 0;
    uint8_t ucCnt = 0;
    uint8_t ucIndex = 0;
    Bool_t bProc = FALSE;
    while ((0 != *pcStr) && (ucIndex < 80)) {
        char cTmp;
        cTmp = *pcStr++;
        if ((cTmp >= 'a') && (cTmp <= 'f')) {
            ucData *= 16;
            ucData += 10 + (cTmp - 'a');
            ucCnt++;
            bProc = TRUE;
        }
        if ((cTmp >= 'A') && (cTmp <= 'F')) {
            ucData *= 16;
            ucData += 10 + (cTmp - 'A');
            ucCnt++;
            bProc = TRUE;
        }
        if ((cTmp >= '0') && (cTmp <= '9')) {
            ucData *= 16;
            ucData += cTmp - '0';
            ucCnt++;
            bProc = TRUE;
        }
        if ((TRUE == bProc) && ((' ' == cTmp) || (',' == cTmp) || ('|' == cTmp) || (2 == ucCnt) || (0 == *pcStr))) {
            ucBuffer[ucIndex++] = ucData;
            ucData = 0;
            ucCnt = 0;
            bProc = FALSE;
        }
    }
    *pucLength = ucIndex;
    return ucBuffer;
}

static void
prvCliCmdMemRead(cli_printf cliprintf, int argc, char** argv) {
#define CLI_MEM_SIZE (16)
    CHECK_CLI();

    if (argc != 3) {
        cliprintf("mem_read ADDRESS LENGTH\n");
    }

    int lBaseAddr = atoi(argv[1]);
    int lSize = atoi(argv[2]);

    if (lSize >= 0) {
        int lCnt, lEachSize, lAddr;
        uint8_t ucData[CLI_MEM_SIZE];
        lCnt = lSize / CLI_MEM_SIZE + ((lSize % CLI_MEM_SIZE) ? 1 : 0);
        for (int n = 0; n < lCnt; n++) {
            if ((n + 1) == lCnt) {
                lEachSize = lSize - CLI_MEM_SIZE * n;
            } else {
                lEachSize = CLI_MEM_SIZE;
            }
            lAddr = lBaseAddr + CLI_MEM_SIZE * n;
            MemRead(s_xMem, lAddr, lEachSize, ucData);
            cliprintf("0x%05X: ", lAddr);
            for (int m = 0; m < lEachSize; m++) {
                cliprintf("%02X ", ucData[m]);
            }
            cliprintf("\n");
        }
    }
}
CLI_CMD_EXPORT(mem_read, read from fram, prvCliCmdMemRead)

static void
prvCliCmdMemWrite(cli_printf cliprintf, int argc, char** argv) {
    CHECK_CLI();

    if (argc != 3) {
        cliprintf("mem_write ADDRESS DATA\n");
    }

    int lAddr = atoi(argv[1]);
    uint8_t ucLength = 0;
    uint8_t* ucData = prvParseHexStr(argv[2], &ucLength);

    MemEraseWrite(s_xMem, lAddr, ucLength, ucData);
}
CLI_CMD_EXPORT(mem_write, write to fram, prvCliCmdMemWrite)


#if MEM_TEST
static void
prvCliCmdMemTest(cli_printf cliprintf, int argc, char** argv) {
#define SPIF_PAGE_SIZE 256
    MemSpiFlashErase_t xErase;
    static uint8_t ucData[4096];
    uint32_t n, m, ulCapacity, ulAddr, ulErrCnt;
    ;

    /* FM25Q16A: 16M-BIT 2MB, PAGE_SIZE: 256, SECTOR_SIZE: 4096; */
    /* API test 1 */
    cliprintf("SPI flash API test 1: ");
    ulErrCnt = 0;
    for (m = 0; m < sizeof(ucData); m++) {
        ucData[m] = m;
    }
    xErase.ulAddr = 0;
    xErase.ulLength = sizeof(ucData);
    MemCtrl(s_xMem, MEM_CTRL_OP_SPIFLASH_ERASE, (uint32_t)(&xErase));
    MemWrite(s_xMem, 0, sizeof(ucData), ucData);
    memset(ucData, 0, sizeof(ucData));
    MemRead(s_xMem, 0, sizeof(ucData), ucData);
    for (m = 0; m < sizeof(ucData); m++) {
        if (ucData[m] != (m & 0xFF)) {
            ulErrCnt++;
        }
    }
    if (0 == ulErrCnt) {
        cliprintf("Success\n");
    } else {
        cliprintf("Failure\n");
    }

    /* API test 2 */
    cliprintf("SPI flash API test 2: ");
    ulErrCnt = 0;
    for (m = 0; m < sizeof(ucData); m++) {
        ucData[m] = m;
    }
    MemEraseWrite(s_xMem, 0, sizeof(ucData), ucData);
    memset(ucData, 0, sizeof(ucData));
    MemRead(s_xMem, 0, sizeof(ucData), ucData);
    for (m = 0; m < sizeof(ucData); m++) {
        if (ucData[m] != (m & 0xFF)) {
            ulErrCnt++;
        }
    }
    if (0 == ulErrCnt) {
        cliprintf("Success\n");
    } else {
        cliprintf("Failure\n");
    }

    /* API test 3 */
    uint8_t ucWtBuf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    uint8_t ucRdBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    cliprintf("SPI flash API test 3\n");
    ulErrCnt = 0;
    MemEraseWrite(s_xMem, 0, 8, ucWtBuf);
    cliprintf("    EraseWrite: %02X %02X %02X %02X %02X %02X %02X %02X\n", ucWtBuf[0], ucWtBuf[1], ucWtBuf[2],
              ucWtBuf[3], ucWtBuf[4], ucWtBuf[5], ucWtBuf[6], ucWtBuf[7]);
    MemRead(s_xMem, 0, 8, ucRdBuf);
    cliprintf("    Read      : %02X %02X %02X %02X %02X %02X %02X %02X\n", ucRdBuf[0], ucRdBuf[1], ucRdBuf[2],
              ucRdBuf[3], ucRdBuf[4], ucRdBuf[5], ucRdBuf[6], ucRdBuf[7]);
    ucWtBuf[0] = 0xAA;
    ucWtBuf[1] = 0xBB;
    MemEraseWrite(s_xMem, 0, 2, ucWtBuf);
    cliprintf("    EraseWrite: %02X %02X\n", ucWtBuf[0], ucWtBuf[1]);
    MemRead(s_xMem, 0, 8, ucRdBuf);
    cliprintf("    Read      : %02X %02X %02X %02X %02X %02X %02X %02X\n", ucRdBuf[0], ucRdBuf[1], ucRdBuf[2],
              ucRdBuf[3], ucRdBuf[4], ucRdBuf[5], ucRdBuf[6], ucRdBuf[7]);
    ucWtBuf[0] = 0x55;
    MemWrite(s_xMem, 0, 1, ucWtBuf);
    cliprintf("    Write     : %02X\n", ucWtBuf[0]);
    MemRead(s_xMem, 0, 8, ucRdBuf);
    cliprintf("    Read      : %02X %02X %02X %02X %02X %02X %02X %02X\n", ucRdBuf[0], ucRdBuf[1], ucRdBuf[2],
              ucRdBuf[3], ucRdBuf[4], ucRdBuf[5], ucRdBuf[6], ucRdBuf[7]);
    MemEraseWrite(s_xMem, 0, 1, ucWtBuf);
    cliprintf("    EraseWrite: %02X\n", ucWtBuf[0]);
    MemRead(s_xMem, 0, 8, ucRdBuf);
    cliprintf("    Read      : %02X %02X %02X %02X %02X %02X %02X %02X\n", ucRdBuf[0], ucRdBuf[1], ucRdBuf[2],
              ucRdBuf[3], ucRdBuf[4], ucRdBuf[5], ucRdBuf[6], ucRdBuf[7]);
    ucWtBuf[0] = 0x11;
    ucWtBuf[1] = 0x22;
    ucWtBuf[2] = 0x33;
    MemEraseWrite(s_xMem, 0, 3, ucWtBuf);
    cliprintf("    EraseWrite: %02X %02X %02X\n", ucWtBuf[0], ucWtBuf[1], ucWtBuf[2]);
    MemRead(s_xMem, 0, 8, ucRdBuf);
    cliprintf("    Read      : %02X %02X %02X %02X %02X %02X %02X %02X\n", ucRdBuf[0], ucRdBuf[1], ucRdBuf[2],
              ucRdBuf[3], ucRdBuf[4], ucRdBuf[5], ucRdBuf[6], ucRdBuf[7]);
    ucWtBuf[0] = 0xAA;
    ucWtBuf[1] = 0xBB;
    MemWrite(s_xMem, 0, 2, ucWtBuf);
    cliprintf("    Write     : %02X %02X\n", ucWtBuf[0], ucWtBuf[1]);
    MemRead(s_xMem, 0, 8, ucRdBuf);
    cliprintf("    Read      : %02X %02X %02X %02X %02X %02X %02X %02X\n", ucRdBuf[0], ucRdBuf[1], ucRdBuf[2],
              ucRdBuf[3], ucRdBuf[4], ucRdBuf[5], ucRdBuf[6], ucRdBuf[7]);
    ucWtBuf[0] = 0xCC;
    ucWtBuf[1] = 0xDD;
    MemEraseWrite(s_xMem, 0, 2, ucWtBuf);
    cliprintf("    EraseWrite: %02X %02X\n", ucWtBuf[0], ucWtBuf[1]);
    MemRead(s_xMem, 0, 8, ucRdBuf);
    cliprintf("    Read      : %02X %02X %02X %02X %02X %02X %02X %02X\n", ucRdBuf[0], ucRdBuf[1], ucRdBuf[2],
              ucRdBuf[3], ucRdBuf[4], ucRdBuf[5], ucRdBuf[6], ucRdBuf[7]);

    /* Total flash read write test */
    uint32_t ulOffset = 0;
    uint32_t ulCnt = ulCapacity / sizeof(ucData);
    Bool_t bEraseChip = FALSE;
    if (bEraseChip) {
        cliprintf("SPI flash all memory read write test:\n");
        ulErrCnt = 0;
        MemCtrl(s_xMem, MEM_CTRL_OP_SPIFLASH_ERASE_CHIP, 0); /* XXX: Attention - need to wait a long time */
        HAL_Delay(100000);                                   /* At least 43 s */
        for (n = 0; n < ulCnt; n++) {
            cliprintf("    Write to 0x%08X: ", ulOffset);
            for (m = 0; m < sizeof(ucData); m++) {
                ucData[m] = m;
            }
            MemWrite(s_xMem, ulOffset, sizeof(ucData), ucData);
            memset(ucData, 0, sizeof(ucData));
            MemRead(s_xMem, ulOffset, sizeof(ucData), ucData);
            for (m = 0; m < sizeof(ucData); m++) {
                if (ucData[m] != (m & 0xFF)) {
                    ulErrCnt++;
                    break;
                }
            }
            if (0 == ulErrCnt) {
                cliprintf("Success\n");
            } else {
                break;
            }
            ulOffset += sizeof(ucData);
        }
        if (0 == ulErrCnt) {
            cliprintf("    Chip Ok\n");
        } else {
            cliprintf("    Chip Error\n");
        }
    }

    /* Page test */
    cliprintf("SPI flash page test\n");
    memset(ucData, 0x41, SPIF_PAGE_SIZE);
    ucData[SPIF_PAGE_SIZE - 1] = 0x40;
    MemEraseWrite(s_xMem, 0, SPIF_PAGE_SIZE, ucData);
    memset(ucData, 0x42, SPIF_PAGE_SIZE);
    ucData[SPIF_PAGE_SIZE - 1] = 0x40;
    MemEraseWrite(s_xMem, SPIF_PAGE_SIZE, SPIF_PAGE_SIZE, ucData);
    /* Read & check the page 1 memory */
    MemRead(s_xMem, 0, SPIF_PAGE_SIZE, ucData);
    /* Read & check the page 1 memory */
    MemRead(s_xMem, SPIF_PAGE_SIZE, SPIF_PAGE_SIZE, ucData);

    /* Erase test */
    cliprintf("SPI flash erase test\n");
    memset(ucData, 0x41, 4096);
    ucData[4096 - 1] = 0x40;
    MemEraseWrite(s_xMem, 0, 4096, ucData);
    memset(ucData, 0x42, 4096);
    ucData[4096 - 1] = 0x40;
    MemEraseWrite(s_xMem, 4096, 4096, ucData);
    /* Read & check the sector 1 memory */
    MemRead(s_xMem, 0, 4096, ucData);
    /* Read & check the sector 1 memory */
    MemRead(s_xMem, 4096, 4096, ucData);

    /* Range test */
    cliprintf("SPI flash range test: ");
    ulErrCnt = 0;
    if (status_ok == MemRead(s_xMem, ulCapacity - 1, 2, ucData)) {
        ulErrCnt++;
    }
    if (status_ok == MemRead(s_xMem, ulCapacity, 1, ucData)) {
        ulErrCnt++;
    }
    if (status_ok == MemWrite(s_xMem, ulCapacity - 1, 2, ucData)) {
        ulErrCnt++;
    }
    if (status_ok == MemWrite(s_xMem, ulCapacity, 1, ucData)) {
        ulErrCnt++;
    }
    if (0 == ulErrCnt) {
        cliprintf("Success\n");
    } else {
        cliprintf("Failure(%d)\n", ulErrCnt);
    }
}
CLI_CMD_EXPORT(mem_test, memory flash test, prvCliCmdMemTest)
#endif /* MEM_TEST */
