/*****************************************************************************
* | File      	:   DEV_Config.h
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*                Used to shield the underlying layers of each master 
*                and enhance portability
*----------------
* |	This version:   V1.0
* | Date        :   2018-11-22
* | Info        :

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_

#include <stdint.h>
#include <stdio.h>
#include "lib/debug/lib_debug.h"
#include "main.h"
#include "spi.h"
#include "tim.h"

//#define uint8_t                           uint8_t
//#define uint16_t                           uint16_t
//#define uint32_t                         uint32_t

/**
 * GPIO config
**/
#define DEV_RST_PIN                     LCD_RESET_GPIO_Port, LCD_RESET_Pin         //PD2
#define DEV_DC_PIN                      LCD_DC_GPIO_Port, LCD_DC_Pin               //PD0
#define DEV_CS_PIN                      SPI3_FLASH_CS_GPIO_Port, SPI3_FLASH_CS_Pin //PA15
#define DEV_BL_PIN                      TIM10->CCR1                                //PB8

/**
 * GPIO read and write
**/
#define DEV_Digital_Write(_pin, _value) HAL_GPIO_WritePin(_pin, _value == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET)
#define DEV_Digital_Read(_pin)          HAL_GPIO_ReadPin(_pin)

/**
 * SPI 
**/
#define DEV_SPI_WRITE(_dat)             DEV_SPI_WRite(_dat);

/**
 * delay x ms
**/
#define DEV_DELAY_MS(__xms)             HAL_Delay(__xms)

/**
 * PWM_BL
**/

#define DEV_SET_PWM(_Value)             DEV_BL_PIN = _Value

/*-----------------------------------------------------------------------------*/
void DEV_SPI_WRite(uint8_t _dat);
int DEV_Module_Init(void);
void DEV_Module_Exit(void);
#endif
