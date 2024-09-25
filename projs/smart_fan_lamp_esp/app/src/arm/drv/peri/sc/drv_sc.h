/**
 * \file            drv_screen.h
 * \brief           Screen driver file
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
 * This file is part of the screen driver.
 *
 * Author:          Jasper <jasperzhangse@gmail.com>
 * Version:         v1.0.0-dev
 * Last edit:       2024-06-18
 */
 
#ifndef __DRV_SC_H__
#define __DRV_SC_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "main.h"
#include "lib/type/lib_type.h"
#include "drv/peri/sc/drv_lcd.h"
#include "drv/peri/sc/drv_tp.h"

/* screen device driver include */
#include "drv/peri/sc/st7789/drv_st7789.h"

    typedef enum
    {
        dwin01 = 0,
        atk_lcd_4_3,
        hxc_lcd_1_8,
        
    } sc_id_t;

    status_t sc_init(sc_id_t sc_id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __DRV_SC_H__ */
