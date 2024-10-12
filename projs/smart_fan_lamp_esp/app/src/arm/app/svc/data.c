/**
 * \file            data.c
 * \brief           Data file
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
 * This file is the entire data application.
 *
 * Author:          Jasper <jasperzhangse@gmail.com>
 * Version:         v1.0.0-dev
 */

/* Includes */
#include "app/include.h"

/* Debug config */
#if DATA_DEBUG
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* DATA_DEBUG */
#if DATA_ASSERT
#undef ASSERT
#define ASSERT(a)                                                                                                      \
    while (!(a)) {                                                                                                     \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                    \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* DATA_ASSERT */

/* Forward declaration */
static Bool_t prvChkCrc(uint8_t* pucData, uint16_t usLength);
static uint8_t prvCalcCrc(uint8_t* pucData, uint16_t usLength);

/* Global variables */
Data_t g_xData;
uint16_t g_usMaxCurAd;

/* Functions */
status_t
AppDataInit(void) {
    TRACE("AppDataInit\r\n");
    DataLoad(&g_xData);
    return status_ok;
}

status_t
data_init(void) {
    AppDataInit();
    return status_ok;
}

status_t
AppDataTerm(void) {
    /* Do nothing */
    return status_ok;
}

status_t
DataLoad(OUT Data_t* pxData) {
    Data_t xData;

    /* Read from page1 */
    mem_flash_read(FLASH_SAVE_PAGE1, sizeof(Data_t), (uint8_t*)&xData);
    TRACE("DataLoad = %x\r\n", (xData.head));
    if ((FLASH_DATA_HEAD == xData.head) && (prvChkCrc((uint8_t*)&xData, sizeof(xData)))) {
        if (pxData) {
            *pxData = xData;
        }
        return status_ok;
    }

    TRACE("data load ok1\r\n");

    /* Read from page2 */
    mem_flash_read(FLASH_SAVE_PAGE2, sizeof(Data_t), (uint8_t*)&xData);
    if ((FLASH_DATA_HEAD == xData.head) && (prvChkCrc((uint8_t*)&xData, sizeof(xData)))) {
        if (pxData) {
            *pxData = xData;
        }
        return status_ok;
    }
    TRACE("FLASH_SAVE_PAGE2\r\n");

#if 1
    /* Init FLASH data */
    Data_t xDataInit = APP_DATA_INIT;
    xDataInit.crc = prvCalcCrc((uint8_t*)&xDataInit, sizeof(Data_t) - 1);
    TRACE("APP_DATA_INIT ok\r\n");
    ASSERT(&xDataInit);
    mem_flash_write(FLASH_SAVE_PAGE1, sizeof(Data_t), (uint8_t*)&xDataInit);
    TRACE("MemFlashWrite FLASH_SAVE_PAGE1\r\n");
    mem_flash_write(FLASH_SAVE_PAGE2, sizeof(Data_t), (uint8_t*)&xDataInit);
    if (pxData) {
        *pxData = xDataInit;
    }
#endif

    TRACE("data load ok\r\n");

    return status_ok;
}

status_t
DataSave(IN Data_t* pxData) {
    /* Init FLASH data */
    Data_t xData;
    pxData->head = FLASH_DATA_HEAD;
    memcpy(&xData, pxData, sizeof(Data_t));
    xData.crc = prvCalcCrc((uint8_t*)&xData, sizeof(Data_t) - 1);
    mem_flash_write(FLASH_SAVE_PAGE1, sizeof(Data_t), (uint8_t*)&xData);
    mem_flash_write(FLASH_SAVE_PAGE2, sizeof(Data_t), (uint8_t*)&xData);

    return status_ok;
}

status_t
DataSaveDirect(void) {
    DataSave(&g_xData);
    return status_ok;
}

status_t
data_save_direct(void) {
    DataSave(&g_xData);
    return status_ok;
}

static Bool_t
prvChkCrc(uint8_t* pucData, uint16_t usLength) {
    /* CRC */
    uint8_t tmp = prvCalcCrc(pucData, usLength - 1);
    return (pucData[usLength - 1] == tmp) ? TRUE : FALSE;
}

static uint8_t
prvCalcCrc(uint8_t* pucData, uint16_t usLength) {
    /* REFERENCE: https://www.devcoons.com/crc8/ */
    /* CRC-8/MAXIM */

    uint8_t crc = 0x00;
    uint8_t ucTmp, ucSum;

    for (uint16_t n = 0; n < usLength; n++) {
        ucTmp = pucData[n];
        for (uint8_t m = 0; m < 8; m++) {
            ucSum = (crc ^ ucTmp) & 0x01;
            crc >>= 1;
            if (ucSum) {
                crc ^= 0x8C;
            }
            ucTmp >>= 1;
        }
    }
    return crc;
}

static void
prv_cli_cmd_cfg_show(cli_printf cliprintf, int argc, char** argv) {
    CHECK_CLI();
    cliprintf("mem_en                :  %d\n", th_mem_en);
    cliprintf("led_brightness        :  %d %%\n", th_led_brightness);
    cliprintf("led_color_temperature :  %d %%\n", th_led_color_temperature);
    cliprintf("led_status            :  %d\n", th_led_status);
    cliprintf("fan_speed             :  %d %%\n", th_fan_speed);
    cliprintf("fan_status            :  %d\n", th_fan_status);
    cliprintf("scence_mode           :  %d\n", th_scence_mode);
    cliprintf("charge_en             :  %d\n", th_charge_en);
    cliprintf("radar_delay.hour      :  %d\n", th_radar_delay.hour);
    cliprintf("radar_delay.min       :  %d\n", th_radar_delay.min);
    cliprintf("radar_delay.sec       :  %d\n", th_radar_delay.sec);
    cliprintf("radar_strategy_en     :  %d\n", th_radar_strategy_en);
}
CLI_CMD_EXPORT(cfg_show, show config parameters, prv_cli_cmd_cfg_show)

static void
prv_cli_cmd_cfg_mem_en(cli_printf cliprintf, int argc, char** argv) {
    CHECK_CLI();
    if (2 == argc) {
        uint8_t en = atoi(argv[1]);
        th_mem_en = en;
        data_save_direct();
        cliprintf("cfg_mem_en ok\r\n");
    } else {
        cliprintf("parameter length error\r\n");
    }
}
CLI_CMD_EXPORT(cfg_mem_en, memory enable, prv_cli_cmd_cfg_mem_en)

static void
prv_cli_cmd_mem_save(cli_printf cliprintf, int argc, char** argv) {
    CHECK_CLI();
    if (2 == argc) {
        uint8_t dev = atoi(argv[1]);

        if (dev == 1) {
            led_save_status();
        } else if (dev == 2) {
        }

    } else {
        cliprintf("parameter length error\r\n");
    }
}
CLI_CMD_EXPORT(mem_save, memory enable, prv_cli_cmd_mem_save)

#if 0
static void prv_cli_cmd_mem_init(cli_printf cliprintf, int argc, char **argv)
{
    CHECK_CLI();
    if (1 == argc)
    {
        /* Init FLASH data */
        Data_t xDataInit = APP_DATA_INIT;
        xDataInit.crc = prvCalcCrc((uint8_t *)&xDataInit, sizeof(Data_t) - 1);
        TRACE("APP_DATA_INIT ok\r\n");
        ASSERT(&xDataInit);
        MemFlashWrite(FLASH_SAVE_PAGE1, sizeof(Data_t), (uint8_t *)&xDataInit);
        TRACE("MemFlashWrite FLASH_SAVE_PAGE1\r\n");
        MemFlashWrite(FLASH_SAVE_PAGE2, sizeof(Data_t), (uint8_t *)&xDataInit);
        g_xData = xDataInit;
    }
    else
    {
        cliprintf("parameter length error\r\n");
    }
}
CLI_CMD_EXPORT(mem_init, memory init, prv_cli_cmd_mem_init)
#endif
