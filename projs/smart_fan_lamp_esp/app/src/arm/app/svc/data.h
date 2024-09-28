/**
 * \file            data.h
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

#ifndef __DATA_H__
#define __DATA_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/* Includes */
#include "main.h"
#include "lib/type/lib_type.h"

/* Defines */
#define th_Data                     g_xData
//#define th_pid                      g_xData.pid
#define th_led_status               g_xData.led_status
#define th_led_brightness           g_xData.led_brightness
#define th_led_color_temperature    g_xData.led_color_temperature
#define th_fan_status               g_xData.fan_status
#define th_fan_polarity             g_xData.fan_polarity
#define th_fan_speed                g_xData.fan_speed
#define th_mem_en                   g_xData.mem_en


#define FLASH_DATA_HEAD     (0xA5)
#define FLASH_SAVE_PAGE1    (1024*64)
#define FLASH_SAVE_PAGE2    (1024*128)

#ifndef APP_DATA_INIT
#define APP_DATA_INIT       { \
                                .head = FLASH_DATA_HEAD, \
                                .led_status = 0, \
                                .led_brightness = 50, \
                                .led_color_temperature = 50, \
                                .fan_status = 0, \
                                .fan_polarity = 1, \
                                .fan_speed = 50, \
                                .mem_en = 1, \
                                .crc  = 0 \
                            }
//      .pid = "12345678901234", 
#endif /* APP_DATA_INIT */

/* Types */
#pragma pack(push)
#pragma pack(1)

typedef struct {
    uint8_t  head;              /* Head mark */
//    char     pid[20];       /* Pwr ID */ /* Use uint8_t will warnning */
    uint8_t  led_status;
    uint16_t led_brightness;
    uint16_t led_color_temperature;
    uint16_t fan_status;
    uint16_t fan_polarity;
    uint16_t fan_speed;
    uint16_t mem_en;
    uint8_t  crc;               /* Check code */
}Data_t;

#if 0
/* Defines */
#define th_Data             g_xData
#define th_OtWarnTh         g_xData.usOtWarnTh
#define th_OtCutTh          g_xData.usOtCutTh
#define th_MaxCur           g_xData.ulMaxCur
#define th_WorkCur          g_xData.ulWorkCur
#define th_CompRate         g_xData.lCompRate
#define th_PdWarnL1         g_xData.usPdWarnL1
#define th_PdWarnL2         g_xData.usPdWarnL2
#define th_AdVolPara        g_xData.ulAdVolPara
#define th_PwrType          g_xData.ucPwrType
#define th_TempNum          g_xData.ucTempNum
#define th_SysDebug         g_xData.ucSysDebug
#define th_Trial            g_xData.ucTrial
#define th_TrialDays        g_xData.usTrialDays
#define th_TrialStTime      g_xData.ulTrialStTime
#define th_ModEn            g_xData.xModEn.bit
#define th_ModEnAll         g_xData.xModEn.all
#define th_Dhcp             g_xData.bDhcp
#define th_DhcpTimeout      g_xData.ulDhcpTimeout
#define th_LocalIp          g_xData.ulLocalIp
#define th_LocalNetMask     g_xData.ulLocalNetMask
#define th_LocalGwAddr      g_xData.ulLocalGwAddr
#define th_MaxCurAd         g_usMaxCurAd
#define th_PwmDuty          g_xData.usduty
#define th_PwrId            g_xData.ucPwrId
#define th_TrialEn          g_xData.usTrialEn
#define th_OrderNum         g_xData.ulOrderNum

#define FLASH_DATA_HEAD     (0xA5)
#define FLASH_SAVE_PAGE1    (1024*64)
#define FLASH_SAVE_PAGE2    (1024*128)

#ifndef APP_DATA_INIT
#define APP_DATA_INIT       { \
                                .ucHead = FLASH_DATA_HEAD, \
                                .usOtWarnTh = 26,\
                                .usOtCutTh = 17,\
                                .ulMaxCur = 450,\
                                .ulWorkCur = 400,\
                                .lCompRate = 0/*0.001*/,\
                                .usPdWarnL1 = 1490/*1200mV*/,\
                                .usPdWarnL2 = 993/*800mV*/,\
                                .ulAdVolPara = 5800/*87/15=5.8*/,\
                                .ucPwrType = 1,\
                                .ucTempNum = 18,\
                                .usTrialEn = 1, \
                                .ucTrial = 0, \
                                .usTrialDays = 30, \
                                .ulTrialStTime = 0, \
                                .ucSysDebug = 0,\
                                .xModEn.all = 0xFFFF,\
                                .usduty = 0, \
                                .ucPwrId = "12345678901234", \
                                .ulOrderNum = 0001, \
                                .ucCrc  = 0 \
                            }
#endif /* APP_DATA_INIT */

/* Types */
#pragma pack(push)
#pragma pack(1)
typedef union {
    uint16_t all;
    struct {
        uint16_t MANUAL:1;          /* 手动 */
        uint16_t QBH:1;             /* QBH检测 */
        uint16_t PD:1;              /* PD检测 */
        uint16_t TEMP1:1;           /* 温度组1检测 */
        uint16_t TEMP2:1;           /* 温度组2检测 */
        uint16_t CHAN1_CUR:1;       /* 通道1电流检测 */
        uint16_t CHAN2_CUR:1;       /* 通道2电流检测 */
        uint16_t CHAN3_CUR:1;       /* 通道3电流检测 */
        uint16_t WATER_PRESS:1;     /* 水压检测 */
        uint16_t WATER_CHILLER:1;   /* 水冷机检测 */
        uint16_t CHAN_CTRL:1;       /* 通道开关 */
    }bit;
}ModEn_t;

typedef struct {
    uint8_t  ucHead;         /* Head mark */
    uint16_t usOtWarnTh;     /* Over temperature warn threshold (raw AD value) */
    uint16_t usOtCutTh;      /* Over temperature cut threshold (raw AD value) */
    uint32_t ulMaxCur;       /* Maximum current (0.1A) */
    uint32_t ulWorkCur;      /* Work current (0.1A) */
    int32_t  lCompRate;      /* Current compensation rate (0.01) */
    uint16_t usPdWarnL1;     /* PD warn threshold level 1 */
    uint16_t usPdWarnL2;     /* PD warn threshold level 2 */
    uint32_t ulAdVolPara;    /* AD voltage parameter */
    uint8_t  ucPwrType;      /* Power type(1-9.5KW, 2-18KW) */
    uint8_t  ucTempNum;      /* Temperature channel number */
    uint8_t  ucSysDebug;     /* System debug enable mark */
    uint8_t  usTrialEn;
    uint8_t  ucTrial;        /* Trial version mark */
    uint16_t usTrialDays;    /* Trial days */
    uint32_t ulTrialStTime;  /* Trial start time stamp */
    ModEn_t  xModEn;         /* Module enable */
    Bool_t   bDhcp;          /* DHCP mark */
    uint32_t ulDhcpTimeout;  /* DHCP timeout */
    uint32_t ulLocalIp;      /* Local IP address */
    uint32_t ulLocalNetMask; /* Local net mask */
    uint32_t ulLocalGwAddr;  /* Local gateway address */
    uint16_t usduty;         /* Pwm duty */
    char  ucPwrId[20];       /* Pwr ID */ /* Use uint8_t will warnning */
    uint32_t ulOrderNum;     /* Order Number  */
    uint8_t  ucCrc;          /* Check code */
}Data_t;

#endif

#pragma pack(pop)

/* Forward declarations */
extern Data_t g_xData;
extern uint16_t g_usMaxCurAd;

/* Functions */
status_t AppDataInit(void);
status_t AppDataTerm(void);
status_t data_init(void);

status_t DataLoad(OUT Data_t *pxData);
status_t DataSave(IN Data_t *pxData);
status_t DataSaveDirect(void);
status_t data_save_direct(void);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __DATA_H__ */
