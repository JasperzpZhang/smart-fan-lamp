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
 * @file      drv_sc7a20.c
 * @brief     Implementation File for TouchPad Module
 * @version   1.0.0
 * @author    Jasper
 * @date      2024-09-26
 */

/**
   modification history
   --------------------
   01a, 19Oct24, Jasper Created
 */
#include "drv/peri/gyro/drv_sc7a20.h"

/*************************************************************************************************************
 * 文件名:            SC7A20.c
 * 功能:              SC7A20 三轴加速度传感器支持
 * 详细:              https://www.doc88.com/p-80659792307153.html?r=1
 *                    如果想一次读写多个寄存器，需要将寄存器地址最高位置1，也就是寄存器地址|0x80
*************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h> // 用于 abs()
#include <string.h>
#include "lib/debug/lib_debug.h"
#include "lib/delay/lib_delay.h"
#include "math.h"

/* Debug config */
#if SC7A20_DEBUG || 1
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* SC7A20_DEBUG */
#if SC7A20_ASSERT
#undef ASSERT
#define ASSERT(a)                                                                                                      \
    while (1) {                                                                                                        \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                    \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* SC7A20_ASSERT */

#define SC7A20_REG_WHO_AM_I 0x0F
#define SC7A20_REG_CTRL_1   0x20
#define SC7A20_REG_CTRL_2   0x21
#define SC7A20_REG_CTRL_3   0x22
#define SC7A20_REG_CTRL_4   0x23
#define SC7A20_REG_X_L      0x28
#define SC7A20_REG_X_H      0x29
#define SC7A20_REG_Y_L      0x2A
#define SC7A20_REG_Y_H      0x2B
#define SC7A20_REG_Z_L      0x2C
#define SC7A20_REG_Z_H      0x2D
#define SC7A20_REG_STATUS   0x27

#define BIT7                0x80

/*************************************************************************************************************************
*函数          :    status_t sc7a20_init(sc7a20_hdl_t *hdl, uint8_t addr, 
*                        status_t (*iic_read)(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len), 
*                        status_t (*iic_write)(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len))
*功能          :    SC7A20 初始化
*参数          :    hdl: 句柄；addr: 芯片 IIC 写地址；iic_read: IIC 读取接口；iic_write: IIC 写入接口；
*返回          :    status_ok: 初始化成功；status_err: 初始化失败      :    
*************************************************************************************************************************/
status_t
sc7a20_init(sc7a20_hdl_t* hdl, uint8_t addr, status_t (*iic_read)(uint8_t addr, uint8_t reg, uint8_t* buf, uint8_t len),
            status_t (*iic_write)(uint8_t addr, uint8_t reg, uint8_t* buf, uint8_t len)) {
    uint16_t i;
    uint8_t reg_temp;
    uint8_t buff[16];

    if (hdl == NULL) {
        TRACE("无效的句柄\r\n");
        delay_ms(5000);
        return status_err;
    }

    hdl->addr = addr;           // 记录地址
    hdl->iic_read = iic_read;   // IIC 读取接口
    hdl->iic_write = iic_write; // IIC 写入接口

    reg_temp = 0;

    delay_ms(50);

    for (i = 0; i < 10; i++) {
        buff[0] = buff[1] = 0;
        if (hdl->iic_read(hdl->addr, SC7A20_REG_WHO_AM_I, buff, 1) == status_err) {
            TRACE("Sensor通讯失败\r\n");
        } else if (buff[0] == 0x11) {
            break;
        } else {
            TRACE("无效的 id 0x%02X\r\n", buff[0]);
        }

        delay_ms(100);
    }

    reg_temp = 0x47;
    hdl->iic_write(hdl->addr, SC7A20_REG_CTRL_1, &reg_temp, 1); // 50Hz + 正常模式 xyz 使能
    reg_temp = 0x00;
    hdl->iic_write(hdl->addr, SC7A20_REG_CTRL_2, &reg_temp, 1); // 关闭滤波器
    reg_temp = 0x00;
    hdl->iic_write(hdl->addr, SC7A20_REG_CTRL_3, &reg_temp, 1); // 关闭中断
    reg_temp = 0x88;
    hdl->iic_write(hdl->addr, SC7A20_REG_CTRL_4, &reg_temp, 1); // 2g + 高精度模式

    if (i < 10) {
        return status_ok;
    }
    return status_err;
}

int16_t
sc7a20_12bit_complement(uint8_t msb, uint8_t lsb) {
    int16_t temp;

    temp = msb << 8 | lsb;
    temp = temp >> 4; // 只有高 12 位有效
    temp = temp & 0x0fff;
    if (temp & 0x0800) {      // 负数 补码 ==> 原码
        temp = temp & 0x07ff; // 屏蔽最高位
        temp = ~temp;
        temp = temp + 1;
        temp = temp & 0x07ff;
        temp = -temp; // 还原最高位
    }
    return temp;
}

/*************************************************************************************************************************
*函数          :    status_t sc7a20_read_acceleration(sc7a20_hdl_t* hdl, int16_t* a_x, int16_t* a_y, int16_t* a_z)
*功能          :    SC7A20 读取加速度值
*参数          :    hdl: 句柄；a_x, a_y, a_z: 三轴加速度值
*返回          :    status_ok: 成功；status_err: 失败      :
*************************************************************************************************************************/
status_t
sc7a20_read_acceleration(sc7a20_hdl_t* hdl, int16_t* a_x, int16_t* a_y, int16_t* a_z) {
    uint8_t buff[6];
    uint8_t i;

    memset(buff, 0, 6);
    if (hdl->iic_read(hdl->addr, SC7A20_REG_X_L | BIT7, buff, 6) == status_err) {
        return status_err;
    } else {
        hdl->iic_read(hdl->addr, SC7A20_REG_STATUS, &i, 1);

        // X 轴
        *a_x = buff[1] << 8 | buff[0];
        *a_x >>= 4; // 取 12 位精度

        // Y 轴
        *a_y = buff[3] << 8 | buff[2];
        *a_y >>= 4; // 取 12 位精度

        // Z 轴
        *a_z = buff[5] << 8 | buff[4];
        *a_z >>= 4; // 取 12 位精度

        return status_ok;
    }
}

#define PI 3.1415926535898

/*************************************************************************************************************************
*函数          :    status_t sc7a20_get_z_axis_angle(sc7a20_hdl_t* hdl, int16_t a_buf[3], float* angle_z)
*功能          :    SC7A20 获取 Z 轴倾角
*参数          :    hdl: 句柄；a_buf: 三轴加速度；angle_z: Z 方向倾角
*返回          :    status_ok: 成功；status_err: 失败
*依赖          :    底层宏定义
*************************************************************************************************************************/
status_t
sc7a20_get_z_axis_angle(sc7a20_hdl_t* hdl, int16_t a_buf[3], float* angle_z) {
    double fx, fy, fz;
    double A;
    int16_t xa, ya, za;

    if (sc7a20_read_acceleration(hdl, &xa, &ya, &za) == status_err) {
        return status_err;
    }

    a_buf[0] = xa; // X 轴加速度
    a_buf[1] = ya; // Y 轴加速度
    a_buf[2] = za; // Z 轴加速度

    fx = xa * (2.0 / 4096);
    fy = ya * (2.0 / 4096);
    fz = za * (2.0 / 4096);

    // 计算 Z 方向的倾角
    A = sqrt(fx * fx + fy * fy) / fz;
    A = atan(A) * (180 / PI);

    *angle_z = A;
    // TRACE("=======角度：%.04f\r\n", *angle_z);

    return status_ok;
}

int
sc7a20_get_orientation(sc7a20_hdl_t* hdl) {
    int16_t xa, ya, za;

    // 调用现有的加速度读取函数
    if (sc7a20_read_acceleration(hdl, &xa, &ya, &za) == status_err) {
        return status_err;
    }

    // 将三轴加速度值转换为正值
    xa = abs(xa);
    ya = abs(ya);
    za = abs(za);

    /* 滞回滤波算法 （Hysteresis Filtering/Comparison）*/
    // 判断横屏或竖屏状态
    if (xa < 300) {
        return 1; // 横屏
    } else if (xa > 850) {
        return 2; // 竖屏
    }

    return 0;
}
