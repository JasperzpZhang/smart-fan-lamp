/**
 * \file            drv_sc.c
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

#include "drv/peri/sc/drv_sc.h"
#include "lib/debug/lib_debug.h"
#include "lib/type/lib_type.h"
#include "main.h"

/* Debug config */
#if SCREEN_DEBUG
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* SCREEN_DEBUG */
#if SCREEN_ASSERT
#undef ASSERT
#define ASSERT(a)                                                                                                      \
    while (!(a)) {                                                                                                     \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                    \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* SCREEN_ASSERT */

status_t
sc_init(sc_id_t sc_id) {
    switch (sc_id) {
        case dwin01: break;

        case atk_lcd_4_3: break;

        case hxc_lcd_1_8: lcd_init();
            sc_tp_init();
        break;

        default: break;
    }
    return status_ok;
}
