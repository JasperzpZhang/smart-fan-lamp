/**
 * \file            drv_aht30.h
 * \brief           Aht30 driver file
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
 * This file is part of the aht30 driver.
 *
 * Author:          Jasper <jasperzhangse@gmail.com>
 * Version:         v1.0.0
 * Last edit:       2024-06-18
 */

#ifndef __AHT30_H__
#define __AHT30_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "lib/type/lib_type.h"

    typedef enum
    {
        AHT30_STATUS_BUSY               = (1 << 7),        /**< busy bit */
        AHT30_STATUS_NOR_MODE           = (0 << 5),        /**< nor mode */
        AHT30_STATUS_CYC_MODE           = (1 << 5),        /**< cyc mode */
        AHT30_STATUS_CMD_MODE           = (2 << 5),        /**< cmd mode */
        AHT30_STATUS_CRC_FLAG           = (1 << 4),        /**< crc flag */
        AHT30_STATUS_CALIBRATION_ENABLE = (1 << 3),        /**< calibration enable */
        AHT30_STATUS_CMP_INT            = (1 << 2),        /**< comparation interrupt */
    } aht30_status_t;


    status_t aht30_init(void);
    status_t aht30_read_data(uint8_t *rx_buffer);
    status_t aht30_read_humiture(int *temperature, uint16_t *humidity);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AHT30_H__ */
