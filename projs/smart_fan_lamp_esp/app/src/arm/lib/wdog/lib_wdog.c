/*
    Wdog.c

    Implementation File for Wdog Module
*/

/* Copyright 2018 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 08Oct18, Karl Created
    01b, 11Dec18, Karl Modified distinguish os & no-os wdog facility
    01c, 02Sep24, Jasper Restructured Wdog lib
*/

/* Includes */
#include "lib/wdog/lib_wdog.h"
#include <main.h>
#include <string.h>
#include "iwdg.h"
#include "lib/debug/lib_debug.h"


#pragma diag_suppress 1035 /* warning: #1035-D: single-precision operand implicitly converted to double-precision */

#if WDOG_ENABLE

/* Debug config */
#if WDOG_DEBUG
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* WDOG_DEBUG */
#if WDOG_ASSERT
#undef ASSERT
#define ASSERT(a)                                                                                                      \
    while (!(a)) {                                                                                                     \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                    \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* WDOG_ASSERT */

status_t
wdog_set_timeout(uint32_t us) {
    /*
     *  Min/max IWDG timeout period (in ms) at 40 kHz (LSI)
     *  Prescaler   PR[2:0]     Min timeout     Max timeout
     *  /4          0           0.1             409.6
     *  /8          1           0.2             819.2
     *  /16         2           0.4             1638.4
     *  /32         3           0.8             3276.8
     *  /64         4           1.6             6553.6
     *  /128        5           3.2             13107.2
     *  /256        6 (or 7)    6.4             26214.4
     */
    uint32_t reload = 0;
    uint8_t prescaler = 0;
    float fMs = 0.f;

    fMs = (float)us / 1000.f;
    if (fMs < 409.6) {
        reload = (uint32_t)(fMs / 0.1);
        prescaler = IWDG_PRESCALER_4;
    } else if (fMs < 819.2) {
        reload = (uint32_t)(fMs / 0.2);
        prescaler = IWDG_PRESCALER_8;
    } else if (fMs < 1638.4) {
        reload = (uint32_t)(fMs / 0.4);
        prescaler = IWDG_PRESCALER_16;
    } else if (fMs < 3276.8) {
        reload = (uint32_t)(fMs / 0.8);
        prescaler = IWDG_PRESCALER_32;
    } else if (fMs < 6553.6) {
        reload = (uint32_t)(fMs / 1.6);
        prescaler = IWDG_PRESCALER_64;
    } else if (fMs < 13107.2) {
        reload = (uint32_t)(fMs / 3.2);
        prescaler = IWDG_PRESCALER_128;
    } else if (fMs < 26214.4) {
        reload = (uint32_t)(fMs / 6.4);
        prescaler = IWDG_PRESCALER_256;
    } else {
        /* We should never get here */
        ASSERT(0);
    }

    ASSERT(NULL != pxWdg);
    hiwdg.Instance = IWDG;
    hiwdg.Init.Reload = reload;
    hiwdg.Init.Prescaler = prescaler;
    TRACE("wdog_set_timeout(LSI-40KHz): \n");
    TRACE("    Prescaler: %d\n", prescaler);
    TRACE("    Reload:    %d\n", reload);
    TRACE("    Delay:     %d\n", us);
    if (HAL_IWDG_Init(&hiwdg) != HAL_OK) {
        /* We should never get here */
        ASSERT(0);
    }

    return status_ok;
}

status_t
wdog_start(uint32_t ms) {
    wdog_set_timeout(ms * 1000L);
    return status_ok;
}

status_t
wdog_feed(void) {
    HAL_IWDG_Refresh(&hiwdg);
    return status_ok;
}

#endif

#if WDOG_ENABLE && 0

/* Debug config */
#if WDOG_DEBUG
#undef TRACE
#define TRACE(...) DebugPrintf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* WDOG_DEBUG */
#if WDOG_ASSERT
#undef ASSERT
#define ASSERT(a)                                                                                                      \
    while (!(a)) {                                                                                                     \
        DebugPrintf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                     \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* WDOG_ASSERT */

/* Local defines */
#define WDOG_START_SIGNAL (0x01)
#define WDOG_GET_CTRL()   ((WdogCtrl_t*)(&s_xWdogCtrl))

/* Local types */
typedef struct {
    uint16_t usWdogNum;
    EventBits_t xEventGroupBits;
    EventGroupHandle_t xEventGroupHandle;

    struct {
        EventBits_t xEventBits;
        osThreadId xTaskId;
    } xTaskWdog[WDOG_MAX_TASK_NUM];
} WdogCtrl_t;

/* Forward declaration */
static Status_t prvWdgConfig(IWDG_HandleTypeDef* pxWdg, uint32_t ulUs);
static Status_t prvWdogCheck(void);
static void prvWdogTask(void const* pvData);

/* Local variables */
static WdogCtrl_t s_xWdogCtrl;
static osThreadId s_xWdogTask = NULL;
static IWDG_HandleTypeDef s_xWdog;
static IWDG_HandleTypeDef* s_pxWdog = NULL;

/* Functions */
Status_t
WdogInit(void) {
    WdogCtrl_t* pxCtrl = WDOG_GET_CTRL();

    ASSERT(NULL != pxCtrl);
    memset(&s_xWdogCtrl, 0, sizeof(WdogCtrl_t));
    pxCtrl->xEventGroupHandle = xEventGroupCreate();
    prvWdgConfig(&s_xWdog, WDOG_TIMEOUT_US);
    osThreadDef(prvWdogTask, prvWdogTask, WDOG_TASK_PRIORITY, 0, WDOG_TASK_STKSIZE);
    s_xWdogTask = osThreadCreate(osThread(prvWdogTask), NULL);
    ASSERT(NULL != s_xWdogTask);

    return STATUS_OK;
}

Status_t
WdogTerm(void) {
    /* Do nothing */
    return STATUS_OK;
}

static Status_t
prvWdgConfig(IWDG_HandleTypeDef* pxWdg, uint32_t ulUs) {
    /*
     *  Min/max IWDG timeout period (in ms) at 40 kHz (LSI)
     *  Prescaler   PR[2:0]     Min timeout     Max timeout
     *  /4          0           0.1             409.6
     *  /8          1           0.2             819.2
     *  /16         2           0.4             1638.4
     *  /32         3           0.8             3276.8
     *  /64         4           1.6             6553.6
     *  /128        5           3.2             13107.2
     *  /256        6 (or 7)    6.4             26214.4
     */
    uint32_t ulReload = 0;
    uint8_t ucPrescaler = 0;
    float fMs = 0.f;

    fMs = (float)ulUs / 1000.f;
    if (fMs < 409.6) {
        ulReload = (uint32_t)(fMs / 0.1);
        ucPrescaler = IWDG_PRESCALER_4;
    } else if (fMs < 819.2) {
        ulReload = (uint32_t)(fMs / 0.2);
        ucPrescaler = IWDG_PRESCALER_8;
    } else if (fMs < 1638.4) {
        ulReload = (uint32_t)(fMs / 0.4);
        ucPrescaler = IWDG_PRESCALER_16;
    } else if (fMs < 3276.8) {
        ulReload = (uint32_t)(fMs / 0.8);
        ucPrescaler = IWDG_PRESCALER_32;
    } else if (fMs < 6553.6) {
        ulReload = (uint32_t)(fMs / 1.6);
        ucPrescaler = IWDG_PRESCALER_64;
    } else if (fMs < 13107.2) {
        ulReload = (uint32_t)(fMs / 3.2);
        ucPrescaler = IWDG_PRESCALER_128;
    } else if (fMs < 26214.4) {
        ulReload = (uint32_t)(fMs / 6.4);
        ucPrescaler = IWDG_PRESCALER_256;
    } else {
        /* We should never get here */
        ASSERT(0);
    }

    ASSERT(NULL != pxWdg);
    pxWdg->Instance = IWDG;
    pxWdg->Init.Reload = ulReload;
    pxWdg->Init.Prescaler = ucPrescaler;
    TRACE("prvWdgConfig(LSI-40KHz): \n");
    TRACE("    Prescaler: %d\n", ucPrescaler);
    TRACE("    Reload:    %d\n", ulReload);
    TRACE("    Delay:     %d\n", ulUs);
    s_pxWdog = pxWdg;
    if (HAL_IWDG_Init(pxWdg) != HAL_OK) {
        /* We should never get here */
        ASSERT(0);
    }

    return STATUS_OK;
}

Status_t
WdogTaskStart(void) {
    ASSERT(NULL != s_xWdogTask);
    ASSERT(NULL != s_pxWdog);
    HAL_IWDG_Start(s_pxWdog);
    osSignalSet(s_xWdogTask, WDOG_START_SIGNAL);
    return STATUS_OK;
}

Status_t
WdogTaskEnable(osThreadId xTaskId) {
    uint16_t n;
    WdogCtrl_t* pxCtrl = WDOG_GET_CTRL();

    ASSERT(NULL != pxCtrl);
    ASSERT(pxCtrl->usWdogNum < WDOG_MAX_TASK_NUM);

    if (pxCtrl->usWdogNum < WDOG_MAX_TASK_NUM) {
        n = pxCtrl->usWdogNum++;
        pxCtrl->xTaskWdog[n].xEventBits = 1 << n;
        pxCtrl->xTaskWdog[n].xTaskId = xTaskId;
        pxCtrl->xEventGroupBits |= pxCtrl->xTaskWdog[n].xEventBits;
        TRACE("WdogTaskEnable:\n");
        TRACE("    xTaskId:         0x%08X\n", pxCtrl->xTaskWdog[n].xTaskId);
        TRACE("    xEventBits:      0x%08X\n", pxCtrl->xTaskWdog[n].xEventBits);
        TRACE("    xEventGroupBits: 0x%08X\n", pxCtrl->xEventGroupBits);
        TRACE("    WdogNum:         %d\n", pxCtrl->usWdogNum);
        return STATUS_OK;
    } else {
        TRACE("WdogTaskEnable failed\n");
        return STATUS_ERR;
    }
}

Status_t
WdogTaskFeed(void) {
    uint16_t n;
    osThreadId xCurTaskId;
    uint8_t bFind = FALSE;
    WdogCtrl_t* pxCtrl = WDOG_GET_CTRL();

    ASSERT(NULL != pxCtrl);
    xCurTaskId = osThreadGetId();
    for (n = 0; n < pxCtrl->usWdogNum; n++) {
        if (xCurTaskId == pxCtrl->xTaskWdog[n].xTaskId) {
            bFind = TRUE;
            break;
        }
    }

    ASSERT(TRUE == bFind);
    if (bFind) {
        xEventGroupSetBits(pxCtrl->xEventGroupHandle, pxCtrl->xTaskWdog[n].xEventBits);
        TRACE("WdogFeed:\n");
        TRACE("    xTaskId:    0x%08X\n", pxCtrl->xTaskWdog[n].xTaskId);
        TRACE("    xEventBits: 0x%08X\n", pxCtrl->xTaskWdog[n].xEventBits);
        return STATUS_OK;
    } else {
        return STATUS_ERR;
    }
}

Status_t
WdogStart(uint32_t ulMs) {
    prvWdgConfig(&s_xWdog, ulMs * 1000L);
    ASSERT(NULL != s_pxWdog);
    HAL_IWDG_Start(s_pxWdog);
    return STATUS_OK;
}

Status_t
WdogFeed(void) {
    ASSERT(NULL != s_pxWdog);
    HAL_IWDG_Refresh(s_pxWdog);
    return STATUS_OK;
}

static Status_t
prvWdogCheck(void) {
    EventBits_t xWdEvBits;
    EventBits_t xWdEvBitsAll;
    WdogCtrl_t* pxCtrl = WDOG_GET_CTRL();

    ASSERT(NULL != pxCtrl);
    xWdEvBitsAll = pxCtrl->xEventGroupBits;
    xWdEvBits = xEventGroupWaitBits(pxCtrl->xEventGroupHandle, xWdEvBitsAll, pdTRUE, pdTRUE, 1);

    if ((xWdEvBits & xWdEvBitsAll) == xWdEvBitsAll) {
        TRACE("prvWdogCheck: feed hardware wdog succeeded\n");
        ASSERT(NULL != s_pxWdog);
        HAL_IWDG_Refresh(s_pxWdog);
    } else {
        TRACE("prvWdogCheck: feed hardware wdog failed\n");
    }

    osDelay(WDOG_TASK_DELAY_MS);

    return STATUS_OK;
}

static void
prvWdogTask(void const* pvData) {
    TRACE("Enter prvWdogTask\n");

    /* Wait for WdogSart function to trigger! */
    osSignalWait(WDOG_START_SIGNAL, osWaitForever);

    while (1) {
        prvWdogCheck();
    }
}

#endif /* WDOG_ENABLE */
