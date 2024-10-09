/**
 * Copyright (c) 2024 Jasper All rights reserved
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 *
 * @file      lib_iic.c
 * @brief     Implementation File for Fan Module
 * @version   1.0.0
 * @author    Jasper
 * @date      2024-09-26
 */

/**
   modification history
   --------------------
   01a, 26Sep24, Jasper Created
 */

#include <cmsis_os.h>
#include "FreeRTOS.h"
#include "lib/debug/lib_debug.h"
#include "lib/spi/lib_spi.h"
#include "spi.h"

/* Debug config */
#if SPI_DEBUG
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* SPI_DEBUG */
#if SPI_ASSERT
#undef ASSERT
#define ASSERT(a)                                                                                                      \
    while (!(a)) {                                                                                                     \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                    \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* SPI_ASSERT */

/* Local defines */
#if SPI_RTOS
#undef MUTEX_NAME
#define MUTEX_NAME spi_mutex
static osMutexId_t MUTEX_NAME;
#define SPI_MUTEX_INIT()                                                                                               \
    do {                                                                                                               \
        osMutexAttr_t mutex_attr = {0};                                                                                \
        MUTEX_NAME = osMutexNew(&mutex_attr);                                                                          \
    } while (0)
#define SPI_LOCK()   osMutexAcquire(MUTEX_NAME, osWaitForever)
#define SPI_UNLOCK() osMutexRelease(MUTEX_NAME)
#else
#define SPI_MUTEX_INIT()
#define SPI_LOCK()
#define SPI_UNLOCK()
#endif /* SPI_RTOS */

#define SPI_CS(x) HAL_GPIO_WritePin(SPI3_FLASH_CS_GPIO_Port, SPI3_FLASH_CS_Pin, x ? GPIO_PIN_SET : GPIO_PIN_RESET);

/**
 * @brief     spi bus write command
 * @param[in] *buf points to a data buffer
 * @param[in] len is the length of the data buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t
spi_write_cmd(uint8_t* buf, uint16_t len) {
    uint8_t res;

    /* set cs low */
    SPI_CS(0);

    /* if len > 0 */
    if (len > 0) {
        /* transmit the buffer */
        res = HAL_SPI_Transmit(&hspi3, buf, len, 1000);
        if (res != HAL_OK) {
            /* set cs high */
            SPI_CS(1);

            return 1;
        }
    }

    /* set cs high */
    SPI_CS(1);

    return 0;
}

/**
 * @brief     spi bus write
 * @param[in] addr is the spi register address
 * @param[in] *buf points to a data buffer
 * @param[in] len is the length of the data buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t
spi_write(uint8_t addr, uint8_t* buf, uint16_t len) {
    uint8_t buffer;
    uint8_t res;

    /* set cs low */
    SPI_CS(0);

    /* transmit the addr */
    buffer = addr;
    res = HAL_SPI_Transmit(&hspi3, (uint8_t*)&buffer, 1, 1000);
    if (res != HAL_OK) {
        /* set cs high */
        SPI_CS(1);

        return 1;
    }

    /* if len > 0 */
    if (len > 0) {
        /* transmit the buffer */
        res = HAL_SPI_Transmit(&hspi3, buf, len, 1000);
        if (res != HAL_OK) {
            /* set cs high */
            SPI_CS(1);

            return 1;
        }
    }

    /* set cs high */
    SPI_CS(1);

    return 0;
}

/**
 * @brief     spi bus write address 16
 * @param[in] addr is the spi register address
 * @param[in] *buf points to a data buffer
 * @param[in] len is the length of the data buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t
spi_write_address16(uint16_t addr, uint8_t* buf, uint16_t len) {
    uint8_t buffer[2];
    uint8_t res;

    /* set cs low */
    SPI_CS(0);

    /* transmit the addr  */
    buffer[0] = (addr >> 8) & 0xFF;
    buffer[1] = addr & 0xFF;
    res = HAL_SPI_Transmit(&hspi3, (uint8_t*)buffer, 2, 1000);
    if (res != HAL_OK) {
        /* set cs high */
        SPI_CS(1);

        return 1;
    }

    /* if len > 0 */
    if (len > 0) {
        /* transmit the buffer */
        res = HAL_SPI_Transmit(&hspi3, buf, len, 1000);
        if (res != HAL_OK) {
            /* set cs high */
            SPI_CS(1);

            return 1;
        }
    }

    /* set cs high */
    SPI_CS(1);

    return 0;
}

/**
 * @brief      spi bus read command
 * @param[out] *buf points to a data buffer
 * @param[in]  len is the length of the data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t
spi_read_cmd(uint8_t* buf, uint16_t len) {
    uint8_t res;

    /* set cs low */
    SPI_CS(0);

    /* if len > 0 */
    if (len > 0) {
        /* receive to the buffer */
        res = HAL_SPI_Receive(&hspi3, buf, len, 1000);
        if (res != HAL_OK) {
            /* set cs high */
            SPI_CS(1);

            return 1;
        }
    }

    /* set cs high */
    SPI_CS(1);

    return 0;
}

/**
 * @brief      spi bus read
 * @param[in]  addr is the spi register address
 * @param[out] *buf points to a data buffer
 * @param[in]  len is the length of the data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t
spi_read(uint8_t addr, uint8_t* buf, uint16_t len) {
    uint8_t buffer;
    uint8_t res;

    /* set cs low */
    SPI_CS(0);

    /* transmit the addr */
    buffer = addr;
    res = HAL_SPI_Transmit(&hspi3, (uint8_t*)&buffer, 1, 1000);
    if (res != HAL_OK) {
        /* set cs high */
        SPI_CS(1);

        return 1;
    }

    /* if len > 0 */
    if (len > 0) {
        /* receive to the buffer */
        res = HAL_SPI_Receive(&hspi3, buf, len, 1000);
        if (res != HAL_OK) {
            /* set cs high */
            SPI_CS(1);

            return 1;
        }
    }

    /* set cs high */
    SPI_CS(1);

    return 0;
}

/**
 * @brief      spi bus read address 16
 * @param[in]  addr is the spi register address
 * @param[out] *buf points to a data buffer
 * @param[in]  len is the length of the data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t
spi_read_address16(uint16_t addr, uint8_t* buf, uint16_t len) {
    uint8_t buffer[2];
    uint8_t res;

    /* set cs low */
    SPI_CS(0);

    /* transmit the addr  */
    buffer[0] = (addr >> 8) & 0xFF;
    buffer[1] = addr & 0xFF;
    res = HAL_SPI_Transmit(&hspi3, (uint8_t*)buffer, 2, 1000);
    if (res != HAL_OK) {
        /* set cs high */
        SPI_CS(1);

        return 1;
    }

    /* if len > 0 */
    if (len > 0) {
        /* receive to the buffer */
        res = HAL_SPI_Receive(&hspi3, buf, len, 1000);
        if (res != HAL_OK) {
            /* set cs high */
            SPI_CS(1);

            return 1;
        }
    }

    /* set cs high */
    SPI_CS(1);

    return 0;
}

/**
 * @brief      spi transmit
 * @param[in]  *tx points to a tx buffer
 * @param[out] *rx points to a rx buffer
 * @param[in]  len is the length of the data buffer
 * @return     status code
 *             - 0 success
 *             - 1 transmit failed
 * @note       none
 */
uint8_t
spi_transmit(uint8_t* tx, uint8_t* rx, uint16_t len) {
    uint8_t res;

    /* set cs low */
    SPI_CS(0);

    /* if len > 0 */
    if (len > 0) {
        /* transmit */
        res = HAL_SPI_TransmitReceive(&hspi3, tx, rx, len, 1000);
        if (res != HAL_OK) {
            /* set cs high */
            SPI_CS(1);

            return 1;
        }
    }

    /* set cs high */
    SPI_CS(1);

    return 0;
}

/**
 * @brief      spi bus write read
 * @param[in]  *in_buf points to an input buffer
 * @param[in]  in_len is the input length
 * @param[out] *out_buf points to an output buffer
 * @param[in]  out_len is the output length
 * @return     status code
 *             - 0 success
 *             - 1 write read failed
 * @note       none
 */
uint8_t
spi_write_read(uint8_t* in_buf, uint32_t in_len, uint8_t* out_buf, uint32_t out_len) {
    uint8_t res;

    /* set cs low */
    SPI_CS(0);

    /* if in_len > 0 */
    if (in_len > 0) {
        /* transmit the input buffer */
        res = HAL_SPI_Transmit(&hspi3, in_buf, in_len, 1000);
        if (res != HAL_OK) {
            /* set cs high */
            SPI_CS(1);

            return 1;
        }
    }

    /* if out_len > 0 */
    if (out_len > 0) {
        /* transmit to the output buffer */
        res = HAL_SPI_Receive(&hspi3, out_buf, out_len, 1000);
        if (res != HAL_OK) {
            /* set cs high */
            SPI_CS(1);

            return 1;
        }
    }

    /* set cs high */
    SPI_CS(1);

    return 0;
}
