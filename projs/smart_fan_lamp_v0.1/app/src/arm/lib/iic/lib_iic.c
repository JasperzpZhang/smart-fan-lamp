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
#include "lib/delay/lib_delay.h"
#include "lib/iic/lib_iic.h"

/* Debug config */
#if IIC_DEBUG || 1
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* IIC_DEBUG */
#if IIC_ASSERT || 1
#undef ASSERT
#define ASSERT(a)                                                                                                      \
    while (!(a)) {                                                                                                     \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                    \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* IIC_ASSERT */

/* Local defines */
#if IIC_RTOS || 1
#undef MUTEX_NAME
#define MUTEX_NAME iic_mutex
static osMutexId_t MUTEX_NAME;
#define IIC_MUTEX_INIT()                                                                                               \
    do {                                                                                                               \
        osMutexAttr_t mutex_attr = {0};                                                                                \
        MUTEX_NAME = osMutexNew(&mutex_attr);                                                                          \
    } while (0)
#define IIC_LOCK()   osMutexAcquire(MUTEX_NAME, osWaitForever)
#define IIC_UNLOCK() osMutexRelease(MUTEX_NAME)
#else
#define IIC_MUTEX_INIT()
#define IIC_LOCK()
#define IIC_UNLOCK()
#endif /* MEM_RTOS */

#if SORTWARE_I2C_ENABLE

/**
 * @brief  iic bus init
 * @return status code
 *         - status_err error
 *         - status_ok  success
 * @note   none
 */
status_t
iic_init(void) {
    IIC_MUTEX_INIT();
    /* set sda high */
    IIC_SDA(high);
    /* set scl high */
    IIC_SCL(high);
    return status_ok;
}

/**
 * @brief  iic bus deinit
 * @return status code
 *         - status_err error
 *         - status_ok  success
 * @note   none
 */
status_t
iic_deinit(void) {
    /* do nothing */
    return status_ok;
}

/**
 * @brief iic bus delay
 * @note  none
 */
static void
prv_iic_delay(void) {
    delay_us(45);
    //    osDelay(1);
}

/**
 * @brief iic bus send start
 * @note  none
 */
static status_t
prv_iic_start(void) {
    /*    __________
     *SCL           \________
     *    ______
     *SDA       \_____________
     */
    IIC_SDA(high);
    IIC_SCL(high);
    prv_iic_delay();
    if (IIC_READ_SDA == low) {
        return status_err;
    }
    IIC_SDA(low);
    prv_iic_delay();
    IIC_SCL(low);
    prv_iic_delay();
    return status_ok;
}

/**
 * @brief iic bus send stop
 * @note  none
 */
static status_t
prv_iic_stop(void) {
    /*          ____________
     *SCL _____/
     *               _______
     *SDA __________/
     */
    IIC_SCL(low);
    prv_iic_delay();
    IIC_SDA(low);
    prv_iic_delay();
    IIC_SCL(high);
    prv_iic_delay();
    IIC_SDA(high);
    prv_iic_delay();
    return status_ok;
}

/**
 * @brief  iic wait ack
 * @return status code
 *         - status_ok  get ack
 *         - status_err no ack
 * @note   none
 */
static status_t
prv_iic_wait_ack(void) {
    uint16_t uc_err_time = 0;

    IIC_SDA(high);
    prv_iic_delay();
    IIC_SCL(high);
    prv_iic_delay();
    while (IIC_READ_SDA != 0) {
        uc_err_time++;
        if (uc_err_time > 250) {
            prv_iic_stop();

            return status_err;
        }
    }
    IIC_SCL(low);

    return status_ok;
}

/**
 * @brief iic bus send ack
 * @note  none
 */
static void
prv_iic_ack(void) {
    IIC_SCL(low);
    IIC_SDA(low);
    prv_iic_delay();
    IIC_SCL(high);
    prv_iic_delay();
    IIC_SCL(low);
    prv_iic_delay();
    IIC_SDA(high);
}

/**
 * @brief iic bus send nack
 * @note  none
 */
static void
prv_iic_nack(void) {
    IIC_SCL(low);
    IIC_SDA(high);
    prv_iic_delay();
    IIC_SCL(high);
    prv_iic_delay();
    IIC_SCL(low);
    IIC_SDA(high);
}

/**
 * @brief     iic send one byte
 * @param[in] txd is the sent byte
 * @note      none
 */
static status_t
prv_iic_send_byte(uint8_t txd) {
    for (uint8_t i = 0; i < 8; i++) {
        if (txd & 0x80) {
            IIC_SDA(high);
        } else {
            IIC_SDA(low);
        }
        prv_iic_delay();
        IIC_SCL(high);
        prv_iic_delay();
        IIC_SCL(low);
        prv_iic_delay();
        txd <<= 1;
    }

    if (prv_iic_wait_ack() == status_err) {
        return status_err;
    }

    return status_ok;
}

/** 
 * @brief     iic read one byte 
 * @param[in] ack is the sent ack
 * @return    status code
 *            - status_ok  success
 *            - status_err resd failed
 * @note      none
 */
static status_t
prv_iic_read_byte(uint8_t* byte, uint8_t ack) {

    if (!byte) {
        return status_err;
    }

    *byte = 0;
    prv_iic_delay();

    for (uint8_t i = 0; i < 8; i++) {
        *byte <<= 1;
        IIC_SCL(high);
        prv_iic_delay();
        if (IIC_READ_SDA == high) {
            *byte |= 0x01;
        }

        IIC_SCL(low);
        prv_iic_delay();
    }

    if (ack != 0) {
        prv_iic_ack();
    } else {
        prv_iic_nack();
    }

    return status_ok;
}

/**
 * @brief     iic bus write command
 * @param[in] addr is the iic device write address
 * @param[in] *buf points to a data buffer
 * @param[in] len is the len of the data buffer
 * @return    status code
 *            - status_ok  success
 *            - status_err write failed
 * @note      addr = addr_7bits
 */
status_t
iic_write_cmd(uint8_t addr, uint8_t* buf, uint16_t len) {
    IIC_LOCK();

    uint16_t i;

    /* send a start */
    prv_iic_start();

    /* send the write addr */
    prv_iic_send_byte(addr << 1);
    if (prv_iic_wait_ack() != 0) {
        prv_iic_stop();
        IIC_UNLOCK();
        return status_err;
    }

    /* write the data */
    for (i = 0; i < len; i++) {
        /* send one byte */
        prv_iic_send_byte(buf[i]);
        if (prv_iic_wait_ack() != 0) {
            prv_iic_stop();
            IIC_UNLOCK();
            return status_err;
        }
    }

    /* send a stop */
    prv_iic_stop();

    /* 这个时间短了会出现 start error */
    delay_us(100);

    IIC_UNLOCK();

    return status_ok;
}

/**
 * @brief     iic bus write
 * @param[in] addr is the iic device write address
 * @param[in] reg is the iic register address
 * @param[in] *buf points to a data buffer
 * @param[in] len is the length of the data buffer
 * @return    status code
 *            - status_ok  success
 *            - status_err write failed
 * @note      addr = device_address_7bits
 */
status_t
iic_write_addr8(uint8_t addr, uint8_t reg, uint8_t* buf, uint8_t len) {

    IIC_LOCK();

    if (!buf) {
        IIC_UNLOCK();
        return status_err;
    }

    if (prv_iic_start() != status_ok) {
        IIC_UNLOCK();
        return status_err;
    }

    if (prv_iic_send_byte(addr << 1 & 0xFE) != status_ok) {
        TRACE("iic send slave addr error\n");
        prv_iic_stop();
        IIC_UNLOCK();
        return status_err;
    }

    if (prv_iic_send_byte(reg) != status_ok) {
        prv_iic_stop();
        IIC_UNLOCK();
        return status_err;
    }

    for (uint8_t i = 0; i < len; i++) {
        if (prv_iic_send_byte(buf[i]) != status_ok) {
            prv_iic_stop();
            IIC_UNLOCK();
            return status_err;
        }
    }

    prv_iic_stop();

    /* 这个时间短了会出现 start error */
    delay_us(100);

    IIC_UNLOCK();

    return status_ok;
}

/**
 * @brief     iic bus write with 16 bits register address 
 * @param[in] addr is the iic device write address
 * @param[in] reg is the iic register address
 * @param[in] *buf points to a data buffer
 * @param[in] len is the len of the data buffer
 * @return    status code
 *            - status_ok  success
 *            - status_err write failed
 * @note      addr = addr_7bits
 */
status_t
iic_write_addr16(uint8_t addr, uint16_t reg, uint8_t* buf, uint16_t len) {
    IIC_LOCK();

    uint16_t i;

    /* send a start */
    if (prv_iic_start() != status_ok) {
        IIC_UNLOCK();
        return status_err;
    }

    /* send the write addr */
    if (prv_iic_send_byte(addr << 1) != status_ok) {
        prv_iic_stop();
        IIC_UNLOCK();
        return status_err;
    }

    /* send the reg high part */
    if (prv_iic_send_byte((reg >> 8) & 0xFF) != status_ok) {
        prv_iic_stop();
        IIC_UNLOCK();
        return status_err;
    }

    /* send the reg low part */
    if (prv_iic_send_byte(reg & 0xFF) != status_ok) {
        prv_iic_stop();
        IIC_UNLOCK();
        return status_err;
    }

    /* write the data */
    for (i = 0; i < len; i++) {
        /* send one byte */
        if (prv_iic_send_byte(buf[i]) != status_ok) {
            prv_iic_stop();
            IIC_UNLOCK();
            return status_err;
        }
    }
    /* send a stop */
    prv_iic_stop();

    /* 这个时间短了会出现 start error */
    delay_us(100);

    IIC_UNLOCK();
    return status_ok;
}

/**
 * @brief      iic bus read command
 * @param[in]  addr is the iic device write address
 * @param[out] *buf points to a data buffer
 * @param[in]  len is the length of the data buffer
 * @return     status code
 *             - status_ok  success
 *             - status_err read failed
 * @note       addr = device_address_7bits
 */
status_t
iic_read_cmd(uint8_t addr, uint8_t* buf, uint16_t len) {
    IIC_LOCK();

    /* send a start */
    prv_iic_start();

    /* send the read addr */

    if (prv_iic_send_byte((addr << 1) + 1) != status_ok) {
        prv_iic_stop();
        IIC_UNLOCK();
        return status_err;
    }
    /* read the data */
    while (len != 0) {
        /* if the last */
        if (len == 1) {
            /* send nack */
            prv_iic_read_byte(buf, 0);
        } else {
            /* send ack */
            prv_iic_read_byte(buf, 1);
        }
        len--;
        buf++;
    }

    /* send a stop */
    prv_iic_stop();

    /* 这个时间短了会出现 start error */
    delay_us(100);

    IIC_UNLOCK();

    return status_ok;
}

/**
 * @brief      iic bus read
 * @param[in]  addr is the iic device write address
 * @param[in]  reg is the iic register address
 * @param[out] *buf points to a data buffer
 * @param[in]  len is the length of the data buffer
 * @return     status code
 *             - status_ok  success
 *             - status_err read failed
 * @note       addr = device_address_7bits
 */
status_t
iic_read_addr8(uint8_t addr, uint8_t reg, uint8_t* buf, uint8_t len) {

    TRACE("iic_read_addr8 run\n");

    IIC_LOCK();

    if (!buf) {
        IIC_UNLOCK();
        return status_err;
    }

    if (prv_iic_start() != status_ok) {
        TRACE("iic read addr8 start error\n");
        IIC_UNLOCK();
        return status_err;
    }

    if (prv_iic_send_byte((addr << 1) & 0xFE) != status_ok) {
        TRACE("iic read addr8 send addr error\n");
        prv_iic_stop();
        IIC_UNLOCK();
        return status_err;
    }

    if (prv_iic_send_byte(reg) != status_ok) {
        TRACE("iic read addr8 send reg error\n");
        prv_iic_stop();
        IIC_UNLOCK();
        return status_err;
    }

    prv_iic_delay();

    if (prv_iic_start() != status_ok) {
        TRACE("iic read addr8 re-start error\n");
        prv_iic_stop();
        IIC_UNLOCK();
        return status_err;
    }

    if (prv_iic_send_byte((addr << 1) | 0x01) != status_ok) {
        TRACE("iic read addr8 re-send addr error\n");
        prv_iic_stop();
        IIC_UNLOCK();
        return status_err;
    }

    for (uint8_t i = 0; i < len; i++) {
        if (i == len - 1) {
            if (prv_iic_read_byte(&buf[i], 0) != status_ok) {
                prv_iic_stop();
                IIC_UNLOCK();
                return status_err;
            }
        } else {

            if (prv_iic_read_byte(&buf[i], 1) != status_ok) {
                prv_iic_stop();
                IIC_UNLOCK();
                return status_err;
            }
        }
        prv_iic_delay();
    }
    prv_iic_stop();

    /* 这个时间短了会出现 start error */
    delay_us(100);
    IIC_UNLOCK();
    return status_ok;
}

/**
 * @brief      iic bus read with 16 bits register address 
 * @param[in]  addr is the iic device write address
 * @param[in]  reg is the iic register address
 * @param[out] *buf points to a data buffer
 * @param[in]  len is the len of the data buffer
 * @return     status code
 *             - status_ok  success
 *             - status_err read failed
 * @note       addr = addr_7bits
 */
status_t
iic_read_addr16(uint8_t addr, uint16_t reg, uint8_t* buf, uint16_t len) {
    IIC_LOCK();

    /* send a start */
    prv_iic_start();

    /* send the write addr */

    if (prv_iic_send_byte(addr << 1) != status_ok) {
        prv_iic_stop();
        IIC_UNLOCK();
        return status_err;
    }

    /* send the reg high part */

    if (prv_iic_send_byte((reg >> 8) & 0xFF) != status_ok) {
        prv_iic_stop();
        IIC_UNLOCK();
        return status_err;
    }

    /* send the reg low part */

    if (prv_iic_send_byte(reg & 0xFF) != status_ok) {
        prv_iic_stop();
        IIC_UNLOCK();
        return status_err;
    }

    /* send a start */
    prv_iic_start();

    /* send the read addr */

    if (prv_iic_send_byte((addr << 1) + 1) != status_ok) {
        prv_iic_stop();
        IIC_UNLOCK();
        return status_err;
    }

    /* read the data */
    while (len != 0) {
        /* if the last */
        if (len == 1) {
            /* send nack */
            prv_iic_read_byte(buf, 0);
        } else {
            /* send ack */
            prv_iic_read_byte(buf, 1);
        }
        len--;
        buf++;
    }
    /* send a stop */
    prv_iic_stop();

    /* 这个时间短了会出现 start error */
    delay_us(100);

    IIC_UNLOCK();

    return status_ok;
}

status_t
iic_write_data(uint8_t device_address, uint8_t* data, uint16_t length) {

    /* 检查空指针 */
    if (!data) {
        return status_err;
    }

    IIC_LOCK();

    /* 发送启动信号 */
    if (prv_iic_start() != status_ok) {
        IIC_UNLOCK();
        return status_err;
    }

    /* 发送设备地址(写模式) */
    if (prv_iic_send_byte(device_address << 1 & 0xFE) != status_ok) {
        prv_iic_stop();
        IIC_UNLOCK();
        return status_err;
    }

    /* 发送数据 */
    for (uint16_t i = 0; i < length; i++) {
        if (prv_iic_send_byte(data[i]) != status_ok) {
            prv_iic_stop();
            IIC_UNLOCK();
            return status_err;
        }
        TRACE("data[%d] = %d\r\n", i, data[i]);
    }
    TRACE("\r\n");

    /* 这个时间短了会出现 start error */
    delay_us(100);
    /* 发送停止信号 */
    prv_iic_stop();
    IIC_UNLOCK();
    return status_ok;
}

status_t
iic_read_data(uint8_t device_address, uint8_t* data, uint16_t length, uint8_t ack) {

    /* 检查空指针 */
    if (!data) {
        TRACE("data is NULL\r\n");
        return status_err;
    }

    IIC_LOCK();

    /* 发送启动信号 */
    if (prv_iic_start() != status_ok) {
        TRACE("start error\r\n");
        IIC_UNLOCK();
        return status_err;
    }

    /* 发送设备地址(读模式) */
    if (prv_iic_send_byte(device_address << 1 | 0x01) != status_ok) {
        TRACE("write address error\r\n");
        prv_iic_stop();
        IIC_UNLOCK();
        return status_err;
    }

    /* 接收数据 */
    for (uint16_t i = 0; i < length; i++) {
        if (prv_iic_read_byte(&data[i], (i < length - 1) ? ack : 1) != status_ok) {
            TRACE("i2c_read_data error\r\n");
            prv_iic_stop();
            IIC_UNLOCK();
            return status_err;
        }
        prv_iic_delay();
        TRACE("data[%d] = %d\r\n", i, data[i]);
    }
    TRACE("\r\n");

    /* 这个时间短了会出现 start error */
    delay_us(100);

    /* 发送停止信号 */
    prv_iic_stop();
    IIC_UNLOCK();
    return status_ok;
}

#endif
