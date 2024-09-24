/*
    Ds1302.h

    Head File for Ds1302 Module
*/

/* Copyright 2018 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 20Sep18, David Created
    01b, 13Nov18, Karl Modified
*/

#ifndef __RTC_DS1302_H__
#define __RTC_DS1302_H__

#ifdef __cplusplus
extern "C"
{
#endif /*__cplusplus */

/* Includes */
#include <stdint.h>
#include <stm32f1xx_hal.h>
#include "lib/type/lib_type.h"
#include "drv/rtc/drv_rtc_config.h"

    /* Types */
    typedef struct
    {
        struct
        {
            GPIO_TypeDef *pxPort;
            uint16_t usPin;
        } xRst;
        struct
        {
            GPIO_TypeDef *pxPort;
            uint16_t usPin;
        } xClk;
        struct
        {
            GPIO_TypeDef *pxPort;
            uint16_t usPin;
        } xIo;
    } Ds1302Config_t;

    /* Functions */
    Status_t RtcDs1302Init(void);
    Status_t RtcDs1302Term(void);
    Status_t RtcDs1302Config(Ds1302Config_t *pxConfig);

    Time_t RtcDs1302ReadTime(void);
    Time_t RtcDs1302ReadTimeBurst(void);

    Status_t RtcDs1302WriteTime(Time_t xTm);
    Status_t RtcDs1302WriteTimeBurst(Time_t xTm);

    uint8_t RtcDs1302ReadRam(uint8_t ucAddr);
    Status_t RtcDs1302ReadRamBurst(uint8_t ucData[31]);

    Status_t RtcDs1302WriteRam(uint8_t ucAddr, uint8_t ucData);
    Status_t RtcDs1302WriteRamBurst(uint8_t ucData[31]);

#if (RTC_STDC_TIME == 2)
    time_t time(time_t *timer);
#endif /* (RTC_STDC_TIME == 2) */

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __RTC_DS1302_H__ */
