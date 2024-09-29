/**
 * \file            include.h
 * \brief           Include file
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
 * This file contains all the header files
 *
 * Author:          Jasper <JasperZhangSE@gmail.com>
 * Version:         v1.0.0-dev
 */

#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes */
/* Std C */
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


/* RTOS */
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "event_groups.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"


/* HAL */
#include "main.h"

/* Config */
#include "app/config.h"
#include "app/define.h"

/* Bsp */
#include "gpio.h"
#include "iwdg.h"
#include "tim.h"
#include "usart.h"
#include "adc.h"

/* Library */
#include "lib/cli/lib_cli.h"
#include "lib/debug/lib_debug.h"
#include "lib/delay/lib_delay.h"
#include "lib/iic/lib_iic.h"
#include "lib/rbuf/lib_rbuf.h"
#include "lib/type/lib_type.h"
#include "lib/uart/lib_uart.h"
#include "lib/wdog/lib_wdog.h"


/* Driver moudle */
#include "drv/peri/mem/drv_mem.h"
#include "drv/peri/rtc/drv_rtc.h"
#include "drv/peri/tp/drv_tp.h"
//#include "drv/peri/humiture/drv_aht20_basic.h"

/* Application */
#include "app/app.h"
#include "app/svc/cli.h"
#include "app/svc/data.h"
#include "app/svc/fan.h"
#include "app/svc/mem.h"
#include "app/svc/time.h"
#include "app/svc/tp.h"
#include "app/sys.h"
#include "app/svc/led.h"
#include "app/svc/panel.h"
#include "app/svc/wave.h"

#if 0
#include "Gpio/Gpio.h"
#include "Mem/Mem.h"
#include "Mem/MemFram.h"
#include "Mqtt/Mqtt.h"
#include "Net/Net.h"
#include "Prot/Prot.h"
#include "Rbuf/Rbuf.h"
#include "Rtc/Rtc.h"
#include "Uart/Uart.h"
#include "Wdog/Wdog.h"
#include "debug/debug.h"


/* Bsp library */
#include "BspGpio.h"
#include "BspI2C.h"
#include "BspSpi.h"
#include "BspTim.h"
#include "BspUart.h"


/* User application */
#include "User/Cli.h"
#include "User/Com.h"
#include "User/Data.h"
#include "User/Drv/Adc.h"
#include "User/Drv/Can.h"
#include "User/Drv/Dac.h"
#include "User/Drv/Gpio.h"
#include "User/Drv/Mem.h"
#include "User/Drv/Net.h"
#include "User/Drv/Pwr.h"
#include "User/Drv/Pwr/Pwr1Prot.h"
#include "User/Drv/Pwr/Pwr2Prot.h"
#include "User/Drv/Stc.h"
#include "User/Drv/Time.h"
#include "User/Sys.h"
#include "User/_Config.h"
#include "User/_Include.h"
#include "User/_Type.h"

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCLUDE_H__ */
