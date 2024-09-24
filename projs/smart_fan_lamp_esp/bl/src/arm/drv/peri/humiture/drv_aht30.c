/**
 * \file            drv_aht30.c
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

#include "FreeRTOS.h"
#include <cmsis_os.h>
#include "stm32f1xx_hal.h"
#include "lib/i2c/lib_i2c.h"
#include "drv/humiture/drv_aht30.h"
#include "lib/debug/lib_debug.h"
#include "lib/cli/lib_cli.h"

/* Pragmas */
#pragma diag_suppress 177 /* warning: #177-D: variable was declared but never referenced */

/* Debug config */
#if AHT30_DEBUG
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* AHT30_DEBUG */
#if AHT30_ASSERT
#undef ASSERT
#define ASSERT(a)                                                   \
    while (!(a))                                                    \
    {                                                               \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__); \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* AHT30_ASSERT */



/* Local defines */
#if AHT30_RTOS
static osMutexId s_xAht30Mutex;

#define MEM_MUTEX_INIT()                                \
    do                                                  \
    {                                                   \
        s_xAht30Mutex = osMutexNew(NULL);                 \
        if (s_xAht30Mutex == NULL)                        \
        {                                               \
            /* Handle error */                          \
        }                                               \
    } while (0)
#define AHT30_LOCK()   osMutexAcquire(s_xAht30Mutex, osWaitForever)
#define AHT30_UNLOCK() osMutexRelease(s_xAht30Mutex)
#else
#define AHT30_MUTEX_INIT()
#define AHT30_LOCK()
#define AHT30_UNLOCK()
#endif /* AHT30_RTOS */ 
    

/* local define */
#define AHT30_I2C_ADDRESS 0x38
    
/**
 * @brief chip information definition
 */
#define CHIP_NAME                 "ASAIR AHT30"        /**< chip name */
#define MANUFACTURER_NAME         "ASAIR"              /**< manufacturer name */
#define SUPPLY_VOLTAGE_MIN        2.2f                 /**< chip min supply voltage */
#define SUPPLY_VOLTAGE_MAX        5.5f                 /**< chip max supply voltage */
#define MAX_CURRENT               0.60f                /**< chip max current */
#define TEMPERATURE_MIN           -40.0f               /**< chip min operating temperature */
#define TEMPERATURE_MAX           85.0f                /**< chip max operating temperature */
#define DRIVER_VERSION            1000                 /**< driver version */



static uint8_t a_aht30_calc_crc(uint8_t *data, uint8_t len);

uint8_t aht30_start_data[3] = {0xAC, 0x33, 0x00};

status_t aht30_init(void)
{
    return status_ok;
}

status_t aht30_read_data(uint8_t *rx_buffer)
{
    i2c_read_data(AHT30_I2C_ADDRESS, rx_buffer, 7, 0);

    return status_ok;
}

status_t aht30_read_humiture(int *temperature, uint16_t *humidity)
{
    TRACE("aht30_read_humiture run\r\n");

    osDelay(10);
    AHT30_LOCK();
    i2c_write_data(AHT30_I2C_ADDRESS, aht30_start_data, 3);
    
    osDelay(100);

    uint8_t rx_buffer[7] = {0};

    uint32_t temperature_raw = 0, humidity_raw = 0;
    

    aht30_read_data(rx_buffer);
    
    uint8_t crc = a_aht30_calc_crc(rx_buffer, 6);
    
    TRACE("CRC = %d\r\n", crc);
    TRACE("rx_buffer[6] = %d\r\n", rx_buffer[6]);
    
    if (crc != rx_buffer[6])
    {
        TRACE("i2c read error\r\n");
        AHT30_UNLOCK();
        return status_err;
    }

     TRACE("rx_buffer = ");
    for (uint8_t i = 0; i < 7; i++)
    {
        TRACE("%d\t", rx_buffer[i]);
    }
    TRACE("\r\n");

    temperature_raw = (((uint32_t)rx_buffer[3]) << 16) |
                      (((uint32_t)rx_buffer[4]) << 8) |
                      (((uint32_t)rx_buffer[5]) << 0); /* set the temperature */
    temperature_raw = (temperature_raw) & 0xFFFFF;     /* cut the temperature part */
    *temperature = (float)(temperature_raw) / 1048576.0f * 200.0f - 50.0f;

    humidity_raw = (((uint32_t)rx_buffer[1]) << 16) |
                   (((uint32_t)rx_buffer[2]) << 8) |
                   (((uint32_t)rx_buffer[3]) << 0);                     /* set the humidity */
    humidity_raw = (humidity_raw) >> 4;                                 /* right shift 4 */
    *humidity = (uint8_t)((float)(humidity_raw) / 1048576.0f * 100.0f); /* convert the humidity */
    AHT30_UNLOCK();
    return status_ok;
}

/**
 * @brief     calculate the crc
 * @param[in] *data points to a data buffer
 * @param[in] len is the data length
 * @return    crc
 * @note      none
 */
static uint8_t a_aht30_calc_crc(uint8_t *data, uint8_t len)
{
    uint8_t i;
    uint8_t byte;
    uint8_t crc = 0xFF;
    
    for (byte = 0; byte < len; byte++)          /* len times */
    {
        crc ^= data[byte];                      /* xor byte */
        for (i = 8; i > 0; --i)                 /* one byte */
        {
            if ((crc & 0x80) != 0)              /* if high*/
            {
                crc = (crc << 1) ^ 0x31;        /* xor 0x31 */
            }
            else
            {
                crc = crc << 1;                 /* skip */
            }
        }
    }
    
    return crc;                                 /* return crc */
}

static void cli_get_humiture(cli_printf cliprintf, int argc, char **argv)
{
    int temperature = 0;
    uint16_t humidity = 0;

    if (1 == argc)
    {
        aht30_read_humiture(&temperature, &humidity);

        if (temperature >= TEMPERATURE_MIN || temperature <= TEMPERATURE_MAX)
        {
            cliprintf("temperature = %d\r\n", temperature);
            cliprintf("humidity    = %d\r\n", humidity);
        }
        else
        {
            cliprintf("i2c communication error\r\n");
        }
    }
    else
    {
        cliprintf("parameter length error\r\n");
    }
}
CLI_CMD_EXPORT(get_humiture, set night light status, cli_get_humiture)
