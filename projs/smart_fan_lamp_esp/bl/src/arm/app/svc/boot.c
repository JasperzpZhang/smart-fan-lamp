/*
    Boot.c
    
    Implementation File for Application Boot Module
*/

/* Copyright 2022 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 08Apr22, Karl Created
    01b, 07Sep24, Jasper Restructured
*/

/* Includes */
#include "app/svc/boot.h"
#include "app/svc/boot_uart.h"
#include "core_cm3.h"
#include "drv/peri/mem/drv_mem.h"
#include "lib/wdog/lib_wdog.h"
#include "stm32f1xx_hal.h"
#include "lib/tim/lib_tim.h"

/* Debug config */
#if APP_BOOT_DEBUG || 1
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* APP_BOOT_DEBUG */
#if APP_BOOT_ASSERT
#undef ASSERT
#define ASSERT(a)                                                                                                      \
    while (!(a)) {                                                                                                     \
        DebugPrintf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                     \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* APP_BOOT_ASSERT */

/* Defines */
/*            Uart bootloader
 * Please define these macros in Config.h
 * #define BOOT_UART_FLASH_PAGE_SIZE   (0x800)
 * #define BOOT_UART_APP_ADDR          (0x08010000)
 * #define BOOT_UART_APP_SIZE          (0x30000)
*/

#define NVIC_VECTTAB_RAM         ((uint32_t)0x20000000) /* RAM base address */
#define NVIC_VECTTAB_FLASH       ((uint32_t)0x08000000) /* Flash base address */
#define NVIC_VECTTAB_OFFSET_MASK ((uint32_t)0x1FFFFF80) /* Set the NVIC vector table offset mask */

/* Forward declarations */
#if APP_BOOT_ENABLE_WDOG
void prvStartTimer(void);
#endif /* APP_BOOT_ENABLE_WDOG */
#if BOOT_UART_ENABLE
static void prvBootUartDone(void);
#endif /* BOOT_UART_ENABLE */
static void prvLoadApplication(uint32_t ulAddr);
static void prvDisableInterrupt(void);
static void prvClockDeInit(void);
static void prvNvicVectorTableSet(uint32_t ulNvicVectorTab, uint32_t ulOffset);
//static __asm void prvJumpToAddress(uint32_t ulAddr);
void prvJumpToAddress(uint32_t ulAddr);


/* Global variables */
extern TIM_HandleTypeDef htim5;

/* Local variables */
#if APP_BOOT_ENABLE_WDOG
static TIM_HandleTypeDef s_hTimer;
#endif /* APP_BOOT_ENABLE_WDOG */
static MemHandle_t s_xMem;

/* Functions */

status_t BootRun(void) {
#if APP_BOOT_ENABLE_WDOG
    /* Start watch dog */
    wdog_start(2500 /*ms*/);
    /* Start 500ms timer */
//    prvStartTimer();
    tim_init();
#endif /* APP_BOOT_ENABLE_WDOG */

    /* Check bootmark */
    uint32_t ulBootMark = 0;
    mem_flash_read(0, sizeof(ulBootMark), (uint8_t*)&ulBootMark);
    TRACE("boot_mark : %x\n", ulBootMark);
    if (0x1234ABCD != ulBootMark) {
#if APP_BOOT_ENABLE_WDOG
        wdog_feed();
        wdog_start(10000 /*ms*/);
#endif /* APP_BOOT_ENABLE_WDOG */
        /* XXX: Disable SPI to save power */
        prvLoadApplication(BOOT_UART_APP_ADDR);
    }
    
    /* Config bootloader */
#if BOOT_UART_ENABLE
    BootUartConfig_t xConfig;
    xConfig.pxRxPinPort = GPIOA;
    xConfig.ulRxPin = GPIO_PIN_10;
    xConfig.pxTxPinPort = GPIOA;
    xConfig.ulTxPin = GPIO_PIN_9;
    xConfig.pxInstance = USART1;
    xConfig.ulWordLength = UART_WORDLENGTH_8B;
    xConfig.ulStopBits = UART_STOPBITS_1;
    xConfig.ulParity = UART_PARITY_NONE;
    xConfig.ulBaudRate = 115200;
    xConfig.pxDoneFunc = prvBootUartDone;
    BootUartConfig(&xConfig);
    TickInit();
#endif /* BOOT_UART_ENABLE */

#if APP_BOOT_ENABLE_WDOG
    wdog_feed();
    wdog_start(2500 /*ms*/);
#endif /* APP_BOOT_ENABLE_WDOG */

    /* Run bootloader */
#if BOOT_UART_ENABLE
    TRACE("boot uart running.\n");
    return BootUartRun();
#endif /* BOOT_UART_ENABLE */
}

#if APP_BOOT_ENABLE_WDOG
void tim5_period_elapsed_cb(TIM_HandleTypeDef* htim) {
    wdog_feed();
}
#endif /* APP_BOOT_ENABLE_WDOG */

#if BOOT_UART_ENABLE
/* Boot uart done callback function */
static void prvBootUartDone(void) {
    /* Clear boot mark */
    uint32_t ulBootMark = 0;
    MemEraseWrite(s_xMem, 0, sizeof(ulBootMark), (uint8_t*)&ulBootMark);
    /* Reset system */
    NVIC_SystemReset();
}
#endif /* BOOT_UART_ENABLE */

static void prvLoadApplication(uint32_t ulAddr) {
    if (((*(__IO uint32_t*)ulAddr) & 0x2FFE0000) == 0x20000000) {
        prvDisableInterrupt();
        prvClockDeInit();
        prvNvicVectorTableSet(NVIC_VECTTAB_FLASH, 0x10000);
        prvJumpToAddress(ulAddr);
    }
}

static void prvDisableInterrupt(void) {
    __disable_irq();
    for (IRQn_Type irq = (IRQn_Type)0; irq <= (IRQn_Type)67; irq++) {
        HAL_NVIC_DisableIRQ(irq);
        NVIC_ClearPendingIRQ(irq);
    }
    return;
}


static void prvClockDeInit(void) {
    HAL_RCC_DeInit();
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;
    HAL_DeInit();
    return;
}

static void prvNvicVectorTableSet(uint32_t ulNvicVectorTab, uint32_t ulOffset) {
    SCB->VTOR = ulNvicVectorTab | (ulOffset & NVIC_VECTTAB_OFFSET_MASK);
    return;
}

void prvJumpToAddress(uint32_t ulAddr) {
    // 设置主栈指针 (MSP)
    __set_MSP(*(volatile uint32_t*)ulAddr);
    
    // 跳转到复位处理程序
    void (*resetHandler)(void) = (void (*)(void))(*(volatile uint32_t*)(ulAddr + 4));
    resetHandler();
}
