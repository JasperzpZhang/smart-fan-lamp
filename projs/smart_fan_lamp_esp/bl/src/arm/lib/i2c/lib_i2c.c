/**
 * \file            lib_i2c.c
 * \brief           Software i2c library
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
 * This file contrains all the software i2c functions.
 *
 * Author:          Jasper <JasperZhangSE@gmail.com>
 * Version:         v1.0.0-dev
 */

#include "FreeRTOS.h"
#include <cmsis_os.h>
#include "lib/delay/lib_delay_us.h"
#include "lib/i2c/lib_i2c.h"
#include "lib/type/lib_type.h"
#include "lib/debug/lib_debug.h"
#include "app/arm_application/config.h"

/* Debug config */
#if I2C_DEBUG
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* I2C_DEBUG */
#if I2C_ASSERT
#undef ASSERT
#define ASSERT(a)                                                   \
    while (!(a))                                                    \
    {                                                               \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__); \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* I2C_ASSERT */

#if SORTWARE_I2C_ENABLE

static void i2c_delay(void)
{
#if USE_SYS_TICK_DELAY_US
    delay_us(45);
#else
    // for (volatile uint32_t i = 0; i < 3000; i++); // 根据系统频率调整
    osDelay(1);
#endif
}

status_t i2c_start(void)
{
    /*    __________
     *SCL           \________
     *    ______
     *SDA       \_____________
     */
    I2C_SDA(high);
    I2C_SCL(high);
    i2c_delay();
    if (I2C_SDA_READ() == low)
        return status_err; /* 检查总线忙 */
    I2C_SDA(low);
    i2c_delay();
    I2C_SCL(low);
    i2c_delay();
    return status_ok;
}

status_t i2c_stop(void)
{
    /*          ____________
     *SCL _____/
     *               _______
     *SDA __________/
     */
    I2C_SCL(low);
    i2c_delay();
    I2C_SDA(low);
    i2c_delay();
    I2C_SCL(high);
    i2c_delay();
    I2C_SDA(high);
    i2c_delay();
    return status_ok;
}

status_t i2c_write_byte(uint8_t byte)
{
    TRACE("i2c write byte = ");
    for (uint8_t i = 0; i < 8; i++)
    {
        if (byte & 0x80)
        {
            I2C_SDA(high);
        }
        else
        {
            I2C_SDA(low);
        }
        TRACE("%d ", (byte & 0x80) >> 7);
        i2c_delay();
        I2C_SCL(high);
        i2c_delay();
        I2C_SCL(low);
        i2c_delay();
        byte <<= 1;
    }
    TRACE("\r\n");

    /* 等待 ACK */
    I2C_SDA(high);
    i2c_delay();
    I2C_SCL(high);
    i2c_delay();
    if (I2C_SDA_READ() == high)
    {
        TRACE("i2c_write_byte error\r\n");
        return status_err;
    }
    I2C_SCL(low);
    return status_ok;
}

status_t i2c_read_byte(uint8_t *byte, uint8_t ack)
{
    /* 检查空指针 */
    if (!byte)
        return status_err;

    TRACE("i2c read byte = ");

    *byte = 0;
    i2c_delay();

    for (uint8_t i = 0; i < 8; i++)
    {
        *byte <<= 1;
        I2C_SCL(high);
        i2c_delay();
        if (I2C_SDA_READ() == high)
        {
            *byte |= 0x01;
        }
        TRACE("%d ", (*byte & 0x01));

        I2C_SCL(low);
        i2c_delay();
    }
    TRACE("\n");

    /* 发送 ACK 或 NACK */
    if (ack)
    {
        I2C_SDA(high);
    }
    else
    {
        I2C_SDA(low);
    }
    i2c_delay();
    I2C_SCL(high);
    i2c_delay();
    I2C_SCL(low);
    i2c_delay();
    I2C_SDA(high);

    return status_ok;
}

status_t i2c_write_data(uint8_t device_address, uint8_t *data, uint16_t length)
{
    /* 检查空指针 */
    if (!data)
    {
        return status_err;
    }

    /* 发送启动信号 */
    if (i2c_start() != status_ok)
    {
        return status_err;
    }

    /* 发送设备地址(写模式) */
    if (i2c_write_byte(device_address << 1 & 0xFE) != status_ok)
    {
        i2c_stop();
        return status_err;
    }

    /* 发送数据 */
    for (uint16_t i = 0; i < length; i++)
    {
        if (i2c_write_byte(data[i]) != status_ok)
        {
            i2c_stop();
            return status_err;
        }
        TRACE("data[%d] = %d\r\n", i, data[i]);
    }
    TRACE("\r\n");

    /* 发送停止信号 */
    i2c_stop();
    return status_ok;
}

status_t i2c_read_data(uint8_t device_address, uint8_t *data, uint16_t length, uint8_t ack)
{
    /* 检查空指针 */
    if (!data)
    {
        TRACE("data is NULL\r\n");
        return status_err;
    }

    /* 发送启动信号 */
    if (i2c_start() != status_ok)
    {
        TRACE("start error\r\n");
        return status_err;
    }

    /* 发送设备地址(读模式) */
    if (i2c_write_byte(device_address << 1 | 0x01) != status_ok)
    {
        TRACE("write address error\r\n");
        i2c_stop();
        return status_err;
    }

    /* 接收数据 */
    for (uint16_t i = 0; i < length; i++)
    {
        if (i2c_read_byte(&data[i], (i < length - 1) ? ack : 1) != status_ok)
        {
            TRACE("i2c_read_data error\r\n");
            i2c_stop();
            return status_err;
        }
        TRACE("data[%d] = %d\r\n", i, data[i]);
    }
    TRACE("\r\n");

    /* 发送停止信号 */
    i2c_stop();
    return status_ok;
}

status_t i2c_write_register(uint8_t device_address, uint8_t register_address, uint8_t *data, uint8_t length)
{
    /* 检查空指针 */
    if (!data)
    {
        return status_err;
    }

    /* 发送启动信号 */
    if (i2c_start() != status_ok)
    {
        return status_err;
    }

    /* 发送设备地址(写模式) */
    if (i2c_write_byte(device_address << 1 & 0xFE) != status_ok)
    {
        i2c_stop();
        return status_err;
    }

    /* 发送寄存器地址 */
    if (i2c_write_byte(register_address) != status_ok)
    {
        i2c_stop();
        return status_err;
    }

    /* 发送数据 */
    for (uint8_t i = 0; i < length; i++)
    {
        if (i2c_write_byte(data[i]) != status_ok) /* 使用数组下标访问数据 */
        {
            i2c_stop();
            return status_err;
        }
    }

    /* 发送停止信号 */
    i2c_stop();
    return status_ok;
}

status_t i2c_read_register(uint8_t device_address, uint8_t register_address, uint8_t *data, uint8_t length)
{
    /* 检查空指针 */
    if (!data)
        return status_err;

    /* 发送启动信号 */
    if (i2c_start() != status_ok)
        return status_err;

    /* 发送设备地址和写位(左移一位并将最低位清零) */
    if (i2c_write_byte((device_address << 1) & 0xFE) != status_ok)
    {
        i2c_stop();
        return status_err;
    }

    /* 发送寄存器地址 */
    if (i2c_write_byte(register_address) != status_ok)
    {
        i2c_stop();
        return status_err;
    }

    /* 发送起始条件(重复开始) */
    if (i2c_start() != status_ok)
    {
        return status_err;
    }

    /* 发送设备地址和读位 (左移一位并将最低位置 1) */
    if (i2c_write_byte((device_address << 1) | 0x01) != status_ok)
    {
        i2c_stop();
        return status_err;
    }

    for (uint8_t i = 0; i < length; i++)
    {
        if (i == length - 1)
        {
            // 最后一个字节，发送NACK
            if (i2c_read_byte(&data[i], 1) != status_ok) // 第二个参数1表示NACK
            {
                i2c_stop();
                return status_err;
            }
        }
        else
        {
            // 不是最后一个字节，发送ACK
            if (i2c_read_byte(&data[i], 0) != status_ok) // 第二个参数0表示ACK
            {
                i2c_stop();
                return status_err;
            }
        }
        
    }

    /* 发送停止条件 */
    i2c_stop();
    return status_ok;
}


#endif
