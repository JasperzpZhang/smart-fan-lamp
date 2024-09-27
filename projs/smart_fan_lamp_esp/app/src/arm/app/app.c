/**
 * @                                                    __----~~~~~~~~~~~------___
 * @                                   .  .   ~~//====......          __--~ ~~
 * @                   -.            \_|//     |||\\  ~~~~~~::::... /~
 * @                ___-==_       _-~o~  \/    |||  \\            _/~~-
 * @        __---~~~.==~||\=_    -_--~/_-~|-   |\\   \\        _/~
 * @    _-~~     .=~    |  \\-_    '-~7  /-   /  ||    \      /
 * @  .~       .~       |   \\ -_    /  /-   /   ||      \   /
 * @ /  ____  /         |     \\ ~-_/  /|- _/   .||       \ /
 * @ |~~    ~~|--~~~~--_ \     ~==-/   | \~--===~~        .\
 * @          '         ~-|      /|    |-~\~~       __--~~
 * @                      |-~~-_/ |    |   ~\_   _-~            /\
 * @                           /  \     \__   \/~                \__
 * @                       _--~ _/ | .-~~____--~-/                  ~~==.
 * @                      ((->/~   '.|||' -_|    ~~-/ ,              . _||
 * @                                 -_     ~\      ~~---l__i__i__i--~~_/
 * @                                 _-~-__   ~)  \----______________--~~
 * @                               //.-~~~-~_--~- |-------~~~~~~~~
 * @                                      //.-~~~--\
 * @                      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * @
 * @                              神兽保佑            永无BUG
 **/

/**
 * \file            app.c
 * \brief           Application file
 */

/*
 * Copyright (c) 2024 Jasper
 *
 * This file includes all the initialization applications.
 *
 * Author:          Jasper <jasperzhangse@gmail.com>
 * Version:         v1.0.0
 */

#include "app/include.h"

/* Debug config */
#if APP_DEBUG
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* APP_DEBUG */

/* Functions */
void
app_init(void) {
    wdog_start(10000);
    debug_init();
    debug_uart_cfg(&huart1);
    debug_channel_set(DEBUG_CHAN_UART);
    cli_init(&huart1, UART_DMA_ENABLE);
    delay_init();
    iic_init();
    time_init();
    mem_init();
    data_init();
    fan_init();
    tp_init();
    led_init();
    ctrl_init();
    sys_init();
}

/* Bootloader */
void
bootloader_run(void) {
#if RUN_WITH_BOOTLOADER
#warning "Please config IROM1: Start 0x8010000, Size 0x90000 in Options->Target"
    SCB->VTOR = 0x8000000 | ((0x10000) & (uint32_t)0x1FFFFF80);
    __enable_irq();
#else
#warning "Please config IROM1: Start 0x8000000, Size 0x100000 in Options->Target"
#endif /* RUN_WITH_BOOTLOADER */
}

/*
user:
    company : D:\Program Files\MDK\Keil5\ARM\ARM_Complier_5.06\bin\fromelf.exe --bin -o ..\..\src\script\app.bin smart_fan_lamp_esp_app\smart_fan_lamp_esp_app.axf

    jasper : D:\ProgramData\Mdk\ARM\ARM_Compiler_506\bin\fromelf.exe --bin -o ..\..\src\script\app.bin smart_fan_lamp_esp_app\smart_fan_lamp_esp_app.axf
    
    ..\..\src\script\auto_copy.bat
*/
