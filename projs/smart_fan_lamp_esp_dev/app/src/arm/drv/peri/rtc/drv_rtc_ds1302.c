/*
    Ds1302.c

    Implementation File for Ds1302 Module
*/

/* Copyright 2018 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 20Sep18, David Created
    01b, 13Nov18, Karl Modified
*/

/* Includes */
#include <cmsis_os.h>
#include "drv/rtc/drv_rtc_ds1302.h"

#if (RTC_ENABLE && RTC_DS1302_ENABLE)

/* Pragmas */
#pragma diag_suppress 177 /* warning: #177-D: function "FUNC" was set but never used */
#pragma diag_suppress 550 /* warning: #550-D: variable was set but never used */

/* Debug config */
#if RTC_DEBUG
#undef TRACE
#define TRACE(...) DebugPrintf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* RTC_DEBUG */
#if RTC_ASSERT
#undef ASSERT
#define ASSERT(a)                                                  \
    while (!(a))                                                   \
    {                                                              \
        DebugPrintf("ASSERT failed: %s %d\n", __FILE__, __LINE__); \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* RTC_ASSERT */

/* Local defines */
/* Register definition */
#define REG_SEC_WO (0x80)
#define REG_SEC_RO (0x81)
#define REG_MIN_WO (0x82)
#define REG_MIN_RO (0x83)
#define RET_HOUR_WO (0x84)
#define REG_HOUR_RO (0x85)
#define REG_DAY_WO (0x86)
#define REG_DAY_RO (0x87)
#define REG_MON_WO (0x88)
#define REG_MON_RO (0x89)
#define REG_WEEK_WO (0x8A)
#define REG_WEEK_RO (0x8B)
#define REG_YEAR_WO (0x8C)
#define REG_YEAR_RO (0x8D)
#define REG_CTRL_WO (0x8E)
#define REG_CTRL_RO (0x8F)
#define REG_CLK_BURST_WO (0xBE)
#define REG_CLK_BURST_RO (0xBF)
#define REG_RAM_BURST_WO (0xFE)
#define REG_RAM_BURST_RO (0xFF)

/* Local types */
typedef struct
{
    uint8_t ucYear; /* Bcd: 0~99 */
    uint8_t ucMon;  /* Bcd: 1~12 */
    uint8_t ucDay;  /* Bcd: 1~31 */
    uint8_t ucWeek; /* Bcd: 1~7 */
    uint8_t ucHour; /* Bcd: 1~12 / 0~23 */
    uint8_t ucMin;  /* Bcd: 0~59 */
    uint8_t ucSec;  /* Bcd: 0~59 */
} DsTime_t;

/* Forward declaration */
static Status_t prvDs1302SetIoInput(void);
static Status_t prvDs1302SetIoOutput(void);
static uint8_t prvDs1302ReadByte(void);
static uint8_t prvDs1302Read(uint8_t ucReg);
static Status_t prvDS1302WriteByte(uint8_t ucData);
static Status_t prvDs1302Write(uint8_t ucReg, uint8_t ucData);
static Time_t prvDs1302ToStdCTime(DsTime_t *pxDsTm);
static DsTime_t prvStdCToDs1302Time(Time_t *pxTm);
static uint8_t prvBcdToDec(uint8_t ucBcd);
static uint8_t prvDecToBcd(uint8_t ucDec);
static Status_t prvPrintDs1302Time(DsTime_t *pxDsTm);
static Status_t prvPrintStdCTime(Time_t *pxTm);

/* Local variables */
static Bool_t s_bInit = FALSE;
static Bool_t s_bConfig = FALSE;
static Ds1302Config_t s_xConfig;
#if RTC_RTOS
static osMutexId s_xMutex;
#endif /* RTC_RTOS */

/* Local defines */
#if RTC_RTOS
#define DS1302_MUTEX_INIT()                             \
    do                                                  \
    {                                                   \
        osMutexDef(Ds1302Mutex);                        \
        s_xMutex = osMutexCreate(osMutex(Ds1302Mutex)); \
    } while (0)
#define DS1302_LOCK() osMutexWait(s_xMutex, osWaitForever)
#define DS1302_UNLOCK() osMutexRelease(s_xMutex)
#else
#define DS1302_MUTEX_INIT()
#define DS1302_LOCK()
#define DS1302_UNLOCK()
#endif /* RTC_RTOS */
#define DS1302_RAM_SIZE (31)
#define IO_PIN s_xConfig.xIo.usPin
#define RST_PIN s_xConfig.xRst.usPin
#define CLK_PIN s_xConfig.xClk.usPin
#define IO_PIN_PORT s_xConfig.xIo.pxPort
#define RST_PIN_PORT s_xConfig.xRst.pxPort
#define CLK_PIN_PORT s_xConfig.xClk.pxPort

/* Functions */
Status_t RtcDs1302Init(void)
{
    if (s_bInit == FALSE)
    {
        DS1302_MUTEX_INIT();
#if RTC_RTOS
        if (NULL == s_xMutex)
        {
            TRACE("RtcDs1302Init create mutex failed\n");
            return STATUS_ERR;
        }
#endif /* RTC_RTOS */
        s_bInit = TRUE;
        return STATUS_OK;
    }
    else
    {
        return STATUS_ERR;
    }
}

Status_t RtcDs1302Term(void)
{
    /* Do nothing */
    return STATUS_OK;
}

Status_t RtcDs1302Config(Ds1302Config_t *pxConfig)
{
    /* Config pin port & pin */
    s_xConfig = *pxConfig;

    /* ATTENTION: Enable GPIO Ports Clock outside the function */
    /* Init GPIO port */
    {
        GPIO_InitTypeDef GPIO_InitStruct;

        /* Config rst pin */
        GPIO_InitStruct.Pin = RST_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(RST_PIN_PORT, &GPIO_InitStruct);

        /* Config clk pin */
        GPIO_InitStruct.Pin = CLK_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(CLK_PIN_PORT, &GPIO_InitStruct);
    }

    s_bConfig = TRUE;

    return STATUS_OK;
}

Time_t RtcDs1302ReadTime(void)
{
    DsTime_t xDsTm;

    ASSERT((TRUE == s_bInit) && (TRUE == s_bConfig));
    DS1302_LOCK();

    xDsTm.ucSec = prvDs1302Read(REG_SEC_RO);
    xDsTm.ucMin = prvDs1302Read(REG_MIN_RO);
    xDsTm.ucHour = prvDs1302Read(REG_HOUR_RO);
    xDsTm.ucDay = prvDs1302Read(REG_DAY_RO);
    xDsTm.ucMon = prvDs1302Read(REG_MON_RO);
    xDsTm.ucWeek = prvDs1302Read(REG_WEEK_RO);
    xDsTm.ucYear = prvDs1302Read(REG_YEAR_RO);

    DS1302_UNLOCK();

    return prvDs1302ToStdCTime(&xDsTm);
}

Time_t RtcDs1302ReadTimeBurst(void)
{
    DsTime_t xDsTm;

    ASSERT((TRUE == s_bInit) && (TRUE == s_bConfig));
    DS1302_LOCK();

    HAL_GPIO_WritePin(RST_PIN_PORT, RST_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CLK_PIN_PORT, CLK_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RST_PIN_PORT, RST_PIN, GPIO_PIN_SET);

    prvDS1302WriteByte(REG_CLK_BURST_RO);
    xDsTm.ucSec = prvDs1302ReadByte();
    xDsTm.ucMin = prvDs1302ReadByte();
    xDsTm.ucHour = prvDs1302ReadByte();
    xDsTm.ucDay = prvDs1302ReadByte();
    xDsTm.ucMon = prvDs1302ReadByte();
    xDsTm.ucWeek = prvDs1302ReadByte();
    xDsTm.ucYear = prvDs1302ReadByte();

    HAL_GPIO_WritePin(CLK_PIN_PORT, CLK_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RST_PIN_PORT, RST_PIN, GPIO_PIN_RESET);

    DS1302_UNLOCK();

    return prvDs1302ToStdCTime(&xDsTm);
}

Status_t RtcDs1302WriteTime(Time_t xTm)
{
    DsTime_t xDsTm;
    uint8_t ucFlag = 0;
    uint8_t ucAddr = 0x80;

    ASSERT((TRUE == s_bInit) && (TRUE == s_bConfig));
    DS1302_LOCK();

    xDsTm = prvStdCToDs1302Time(&xTm);
    prvDs1302Write(REG_CTRL_WO, 0x00);
    prvDs1302Write(ucAddr, 0x80);
    prvDs1302Write(REG_MIN_WO, xDsTm.ucMin);
    prvDs1302Write(RET_HOUR_WO, xDsTm.ucHour);
    prvDs1302Write(REG_DAY_WO, xDsTm.ucDay);
    prvDs1302Write(REG_MON_WO, xDsTm.ucMon);
    prvDs1302Write(REG_WEEK_WO, xDsTm.ucWeek);
    prvDs1302Write(REG_YEAR_WO, xDsTm.ucYear);

    do
    {
        prvDs1302Write(REG_SEC_WO, xDsTm.ucSec);
        ucFlag = prvDs1302Read(REG_SEC_RO);
    } while (ucFlag & 0x80);
    prvDs1302Write(REG_CTRL_WO, 0x80);

    DS1302_UNLOCK();

    return STATUS_OK;
}

Status_t RtcDs1302WriteTimeBurst(Time_t xTm)
{
    DsTime_t xDsTm;

    ASSERT((TRUE == s_bInit) && (TRUE == s_bConfig));
    xDsTm = prvStdCToDs1302Time(&xTm);
    DS1302_LOCK();

    prvDs1302Write(REG_CTRL_WO, 0x00);
    HAL_GPIO_WritePin(RST_PIN_PORT, RST_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CLK_PIN_PORT, CLK_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RST_PIN_PORT, RST_PIN, GPIO_PIN_SET);

    prvDS1302WriteByte(REG_CLK_BURST_WO);
    prvDS1302WriteByte(xDsTm.ucSec);
    prvDS1302WriteByte(xDsTm.ucMin);
    prvDS1302WriteByte(xDsTm.ucHour);
    prvDS1302WriteByte(xDsTm.ucDay);
    prvDS1302WriteByte(xDsTm.ucMon);
    prvDS1302WriteByte(xDsTm.ucWeek);
    prvDS1302WriteByte(xDsTm.ucYear);
    prvDS1302WriteByte(0x80);

    HAL_GPIO_WritePin(CLK_PIN_PORT, CLK_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RST_PIN_PORT, RST_PIN, GPIO_PIN_RESET);
    prvDs1302Write(REG_CTRL_WO, 0x80);

    DS1302_UNLOCK();

    return STATUS_OK;
}

uint8_t RtcDs1302ReadRam(uint8_t ucAddr)
{
    uint8_t ucData = 0xFF;

    ASSERT((TRUE == s_bInit) && (TRUE == s_bConfig));
    DS1302_LOCK();

    if (ucAddr < DS1302_RAM_SIZE)
    {
        ucData = prvDs1302Read(0xC1 + ucAddr * 2);
    }
    else
    {
        TRACE("RtcDs1302ReadRam addr range error\n");
    }

    DS1302_UNLOCK();

    return ucData;
}

Status_t RtcDs1302ReadRamBurst(uint8_t ucData[31])
{
    uint8_t n = 0;

    ASSERT((TRUE == s_bInit) && (TRUE == s_bConfig));
    DS1302_LOCK();

    HAL_GPIO_WritePin(RST_PIN_PORT, RST_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CLK_PIN_PORT, CLK_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RST_PIN_PORT, RST_PIN, GPIO_PIN_SET);
    prvDS1302WriteByte(REG_RAM_BURST_RO);

    for (n = 0; n < 31; n++)
    {
        ucData[n] = prvDs1302ReadByte();
    }

    HAL_GPIO_WritePin(CLK_PIN_PORT, CLK_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RST_PIN_PORT, RST_PIN, GPIO_PIN_RESET);

    DS1302_UNLOCK();

    return STATUS_OK;
}

Status_t RtcDs1302WriteRam(uint8_t ucAddr, uint8_t ucData)
{
    Status_t nRet = STATUS_OK;

    ASSERT((TRUE == s_bInit) && (TRUE == s_bConfig));
    DS1302_LOCK();

    if (ucAddr < DS1302_RAM_SIZE)
    {
        prvDs1302Write(REG_CTRL_WO, 0x00);
        prvDs1302Write(0xC0 + ucAddr * 2, ucData);
        prvDs1302Write(REG_CTRL_WO, 0x80);
    }
    else
    {
        nRet = STATUS_ERR;
        TRACE("RtcDs1302WriteRam addr range error\n");
    }

    DS1302_UNLOCK();

    return nRet;
}

Status_t RtcDs1302WriteRamBurst(uint8_t ucData[31])
{
    uint8_t n;

    ASSERT((TRUE == s_bInit) && (TRUE == s_bConfig));
    DS1302_LOCK();

    prvDs1302Write(REG_CTRL_WO, 0x00);
    HAL_GPIO_WritePin(RST_PIN_PORT, RST_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CLK_PIN_PORT, CLK_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RST_PIN_PORT, RST_PIN, GPIO_PIN_SET);
    prvDS1302WriteByte(REG_RAM_BURST_WO);

    for (n = 0; n < 31; n++)
    {
        prvDS1302WriteByte(ucData[n]);
    }

    HAL_GPIO_WritePin(CLK_PIN_PORT, CLK_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RST_PIN_PORT, RST_PIN, GPIO_PIN_RESET);
    prvDs1302Write(REG_CTRL_WO, 0x80);

    DS1302_UNLOCK();

    return STATUS_OK;
}

#if (RTC_STDC_TIME == 2)

time_t time(time_t *timer)
{
    Time_t tm;
    time_t t;

    tm = RtcDs1302ReadTimeBurst();
    t = mktime(&tm);

    if (NULL == timer)
    {
        return t;
    }
    else
    {
        *timer = t;
        return t;
    }
}

#endif /* (RTC_STDC_TIME == 2) */

static Status_t prvDs1302SetIoInput()
{
    GPIO_InitTypeDef GPIO_InitStruct;

    ASSERT((TRUE == s_bInit) && (TRUE == s_bConfig));
    GPIO_InitStruct.Pin = IO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;

    HAL_GPIO_Init(IO_PIN_PORT, &GPIO_InitStruct);

    return STATUS_OK;
}

static Status_t prvDs1302SetIoOutput()
{
    GPIO_InitTypeDef GPIO_InitStruct;

    ASSERT((TRUE == s_bInit) && (TRUE == s_bConfig));
    GPIO_InitStruct.Pin = IO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(IO_PIN_PORT, &GPIO_InitStruct);

    return STATUS_OK;
}

static uint8_t prvDs1302ReadByte(void)
{
    uint8_t n;
    uint8_t ucData = 0;

    ASSERT((TRUE == s_bInit) && (TRUE == s_bConfig));
    prvDs1302SetIoInput();
    for (n = 0; n < 8; n++)
    {
        ucData >>= 1;

        if (GPIO_PIN_SET == HAL_GPIO_ReadPin(IO_PIN_PORT, IO_PIN))
        {
            ucData |= 0x80;
        }
        HAL_GPIO_WritePin(CLK_PIN_PORT, CLK_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(CLK_PIN_PORT, CLK_PIN, GPIO_PIN_RESET);
    }

    return ucData;
}

static uint8_t prvDs1302Read(uint8_t ucReg)
{
    uint8_t ucData;

    ASSERT((TRUE == s_bInit) && (TRUE == s_bConfig));
    HAL_GPIO_WritePin(RST_PIN_PORT, RST_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CLK_PIN_PORT, CLK_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RST_PIN_PORT, RST_PIN, GPIO_PIN_SET);

    prvDS1302WriteByte(ucReg | 0x01);
    ucData = prvDs1302ReadByte();

    HAL_GPIO_WritePin(CLK_PIN_PORT, CLK_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RST_PIN_PORT, RST_PIN, GPIO_PIN_RESET);

    return ucData;
}

static Status_t prvDS1302WriteByte(uint8_t ucData)
{
    uint8_t n;

    ASSERT((TRUE == s_bInit) && (TRUE == s_bConfig));
    prvDs1302SetIoOutput();
    for (n = 0; n < 8; n++)
    {
        if (ucData & 0x01)
        {
            HAL_GPIO_WritePin(IO_PIN_PORT, IO_PIN, GPIO_PIN_SET);
        }
        else
        {
            HAL_GPIO_WritePin(IO_PIN_PORT, IO_PIN, GPIO_PIN_RESET);
        }
        HAL_GPIO_WritePin(CLK_PIN_PORT, CLK_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(CLK_PIN_PORT, CLK_PIN, GPIO_PIN_RESET);
        ucData >>= 1;
    }

    return STATUS_OK;
}

static Status_t prvDs1302Write(uint8_t ucReg, uint8_t ucData)
{
    ASSERT((TRUE == s_bInit) && (TRUE == s_bConfig));
    HAL_GPIO_WritePin(RST_PIN_PORT, RST_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CLK_PIN_PORT, CLK_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RST_PIN_PORT, RST_PIN, GPIO_PIN_SET);

    prvDS1302WriteByte(ucReg);
    prvDS1302WriteByte(ucData);

    HAL_GPIO_WritePin(CLK_PIN_PORT, CLK_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RST_PIN_PORT, RST_PIN, GPIO_PIN_RESET);

    return STATUS_OK;
}

static Time_t prvDs1302ToStdCTime(DsTime_t *pxDsTm)
{
    Time_t cTm;

    /* Bcd -> Dec */
    cTm.tm_sec = prvBcdToDec(pxDsTm->ucSec);         /* seconds after the minute, 0 to 60 (0 - 60 allows for the occasional leap second) */
    cTm.tm_min = prvBcdToDec(pxDsTm->ucMin);         /* minutes after the hour, 0 to 59 */
    cTm.tm_hour = prvBcdToDec(pxDsTm->ucHour);       /* hours since midnight, 0 to 23 */
    cTm.tm_mday = prvBcdToDec(pxDsTm->ucDay);        /* day of the month, 1 to 31 */
    cTm.tm_mon = prvBcdToDec(pxDsTm->ucMon) - 1;     /* months since January, 0 to 11 */
    cTm.tm_year = prvBcdToDec(pxDsTm->ucYear) + 100; /* years since 1900 */
    cTm.tm_wday = prvBcdToDec(pxDsTm->ucWeek) - 1;   /* days since Sunday, 0 to 6 */
    cTm.tm_yday = prvBcdToDec(0);                    /* days since January 1, 0 to 365 */
    cTm.tm_isdst = prvBcdToDec(0);                   /* Daylight Savings Time flag */
#if RTC_SHOW_TIME
    prvPrintStdCTime(&cTm);
#endif /* RTC_SHOW_TIME */

    return cTm;
}

static DsTime_t prvStdCToDs1302Time(Time_t *pxTm)
{
    DsTime_t xDsTm;

    /* Dec -> Bcd */
    xDsTm.ucYear = prvDecToBcd(pxTm->tm_year - 100); /* 0~99 */
    xDsTm.ucMon = prvDecToBcd(pxTm->tm_mon + 1);     /* 1~12 */
    xDsTm.ucDay = prvDecToBcd(pxTm->tm_mday);        /* 1~31 */
    xDsTm.ucWeek = prvDecToBcd(pxTm->tm_wday + 1);   /* 1~7 */
    xDsTm.ucHour = prvDecToBcd(pxTm->tm_hour);       /* 1~12 / 0~23 */
    xDsTm.ucMin = prvDecToBcd(pxTm->tm_min);         /* 0~59 */
    xDsTm.ucSec = prvDecToBcd(pxTm->tm_sec);         /* 0~59 */
#if RTC_SHOW_TIME
    prvPrintDs1302Time(&xDsTm);
#endif /* RTC_SHOW_TIME */

    return xDsTm;
}

static uint8_t prvBcdToDec(uint8_t ucBcd)
{
    uint8_t ucDec;
    ucDec = ucBcd / 16 * 10 + ucBcd % 16;
    return ucDec;
}

static uint8_t prvDecToBcd(uint8_t ucDec)
{
    uint8_t ucBcd;
    ucBcd = ucDec / 10 * 16 + ucDec % 10;
    return ucBcd;
}

static Status_t prvPrintDs1302Time(DsTime_t *pxDsTm)
{
    TRACE("prvPrintDs1302Time:\n");
    TRACE("    ucYear:   %02X\n", pxDsTm->ucYear);
    TRACE("    ucMon:    %02X\n", pxDsTm->ucMon);
    TRACE("    ucDay:    %02X\n", pxDsTm->ucDay);
    TRACE("    ucWeek:   %02X\n", pxDsTm->ucWeek);
    TRACE("    ucHour:   %02X\n", pxDsTm->ucHour);
    TRACE("    ucMin:    %02X\n", pxDsTm->ucMin);
    TRACE("    ucSec:    %02X\n", pxDsTm->ucSec);
    return STATUS_OK;
}

static Status_t prvPrintStdCTime(Time_t *pxTm)
{
    TRACE("prvPrintStdCTime:\n");
    TRACE("    tm_year:   %d\n", pxTm->tm_year);
    TRACE("    tm_mon:    %d\n", pxTm->tm_mon);
    TRACE("    tm_mday:   %d\n", pxTm->tm_mday);
    TRACE("    tm_wday:   %d\n", pxTm->tm_wday);
    TRACE("    tm_yday:   %d\n", pxTm->tm_yday);
    TRACE("    tm_hour:   %d\n", pxTm->tm_hour);
    TRACE("    tm_min:    %d\n", pxTm->tm_min);
    TRACE("    tm_sec:    %d\n", pxTm->tm_sec);
    TRACE("    tm_isdst:  %d\n", pxTm->tm_isdst);
    return STATUS_OK;
}

#endif /* (RTC_ENABLE && RTC_DS1302_ENABLE) */
