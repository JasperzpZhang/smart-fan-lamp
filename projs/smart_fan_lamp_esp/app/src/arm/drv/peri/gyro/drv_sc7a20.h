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
 * @file      drv_sc7a20.h
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

#ifndef __DRV_SC7A20_H__
#define __DRV_SC7A20_H__

#include "lib/type/lib_type.h"
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// sc7a20 句柄
typedef struct {
    status_t (*iic_read)(uint8_t addr, uint8_t reg, uint8_t* buf, uint8_t len);  // IIC 读取寄存器接口
    status_t (*iic_write)(uint8_t addr, uint8_t reg, uint8_t* buf, uint8_t len); // IIC 写入寄存器接口
    uint8_t addr;
    uint8_t mode_config_data; // 记录模式配置值
} sc7a20_hdl_t;

// sc7a20 初始化
status_t sc7a20_init(sc7a20_hdl_t* hdl, uint8_t addr,
                     status_t (*iic_read_reg)(uint8_t addr, uint8_t reg, uint8_t* buf, uint8_t len),
                     status_t (*iic_write_reg)(uint8_t addr, uint8_t reg, uint8_t* buf, uint8_t len));

status_t sc7a20_read_acceleration(sc7a20_hdl_t* hdl, int16_t* p_xa, int16_t* p_ya,
                                  int16_t* p_za); // sc7a20 读取加速度值
status_t sc7a20_get_z_axis_angle(sc7a20_hdl_t* hdl, int16_t acce_buff[3], float* p_angle_z); // sc7a20 获取 Z 轴倾角
int sc7a20_get_orientation(sc7a20_hdl_t* hdl);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __DRV_SC7A20_H__ */
