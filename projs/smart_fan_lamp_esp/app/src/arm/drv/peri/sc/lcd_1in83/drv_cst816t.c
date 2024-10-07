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
 * @file      drv_voice.c
 * @brief     Implementation File for voice Module
 * @version   1.0.0
 * @author    Jasper
 * @date      2024-10-06
 */

/**
   modification history
   --------------------
   01a, 26Sep24, Jasper Created
 */

#include "drv/peri/sc/lcd_1in83/drv_cst816t.h"
#include <stdint.h>

/* Debug config */
#if CST816T_DEBUG
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* CST816T_DEBUG */

#if CST816T_ASSERT
#undef ASSERT
#define ASSERT(a)                                                                                                      \
    while (1) {                                                                                                        \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                    \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* CST816T_ASSERT */

#define CST816T_ADDRESS          0x15

// 寄存器定义
#define REG_GESTURE_ID           0x01
#define REG_FINGER_NUM           0x02
#define REG_XPOS_H               0x03
#define REG_XPOS_L               0x04
#define REG_YPOS_H               0x05
#define REG_YPOS_L               0x06
#define REG_CHIP_ID              0xA7
#define REG_PROJ_ID              0xA8
#define REG_FW_VERSION_ID        0xA9
#define REG_FACTORY_ID           0xAA
#define REG_MOTION_MASK          0xEC
#define REG_IRQ_CTRL             0xFA
#define REG_AUTO_RST             0xFB
#define REG_LONG_PRESS_TIME      0xFC
#define REG_DIS_AUTOSLEEP        0xFE

// 中断和动作掩码定义
#define MOTION_MASK_DOUBLE_CLICK 0x07
#define IRQ_EN_TOUCH             0x40
#define IRQ_EN_CHANGE            0x20
#define IRQ_EN_MOTION            0x10
#define IRQ_EN_LONGPRESS         0x01

// 触摸屏初始化
status_t
drv_cst816t_init(cst816t_hdl_t* cst816t_hdl, uint8_t tp_mode) {

    ASSERT(cst816t_hdl);
    ASSERT(cst816t_hdl->iic_read);
    ASSERT(cst816t_hdl->iic_write);

    uint8_t data[4] = {0};
    if (HAL_GPIO_ReadPin(cst816t_hdl->_ctrl.rst_port, cst816t_hdl->_ctrl.rst_pin) == 1) {
        HAL_GPIO_WritePin(cst816t_hdl->_ctrl.rst_port, cst816t_hdl->_ctrl.rst_pin, GPIO_PIN_RESET);
        osDelay(100);
        HAL_GPIO_WritePin(cst816t_hdl->_ctrl.rst_port, cst816t_hdl->_ctrl.rst_pin, GPIO_PIN_SET);
        osDelay(500);
    }

    // 读取芯片ID和固件版本
    if (cst816t_hdl->iic_read(CST816T_ADDRESS, REG_CHIP_ID, data, 4) == 0) {
        cst816t_hdl->_ctrl.chip_id = data[0];
        cst816t_hdl->_ctrl.firmware_version = data[3];
    }

    // 配置中断控制和运动掩码
    uint8_t irq_en = 0, motion_mask = 0;
    switch (tp_mode) {
        case 0: irq_en = IRQ_EN_TOUCH; break;
        case 1: irq_en = IRQ_EN_CHANGE; break;
        case 2: irq_en = IRQ_EN_MOTION; break;
        case 3:
            irq_en = IRQ_EN_MOTION | IRQ_EN_LONGPRESS;
            motion_mask = MOTION_MASK_DOUBLE_CLICK;
            break;
    }

    cst816t_hdl->iic_write(CST816T_ADDRESS, REG_IRQ_CTRL, &irq_en, 1);
    cst816t_hdl->iic_write(CST816T_ADDRESS, REG_MOTION_MASK, &motion_mask, 1);

    // cst816t_hdl->iic_write(CST816T_ADDRESS, REG_AUTO_RST, &auto_rst, 1);

    /* 禁用自动休眠 */
    uint8_t dis_auto_sleep = 0xFF;
    cst816t_hdl->iic_write(CST816T_ADDRESS, REG_DIS_AUTOSLEEP, &dis_auto_sleep, 1);

    return status_ok;
}

// 检查触摸是否可用
bool
cst816t_available(cst816t_hdl_t* cst816t_hdl) {
    uint8_t data[6] = {0};
    if ((cst816t_hdl->iic_read(CST816T_ADDRESS, REG_GESTURE_ID, data, 6) == status_ok)) {
        cst816t_hdl->_ctrl.gesture_id = data[0];
        cst816t_hdl->_ctrl.finger_num = data[1];
        cst816t_hdl->_ctrl.x = ((data[2] & 0x0F) << 8) | data[3];
        cst816t_hdl->_ctrl.y = ((data[4] & 0x0F) << 8) | data[5];
        return true;
    }

    // if (cst816t_hdl->_ctrl.tp_event && (cst816t_hdl->iic_read(CST816T_ADDRESS, REG_GESTURE_ID, data, 6) == status_ok)) {
    //     cst816t_hdl->_ctrl.gesture_id = data[0];
    //     cst816t_hdl->_ctrl.finger_num = data[1];
    //     cst816t_hdl->_ctrl.x = ((data[2] & 0x0F) << 8) | data[3];
    //     cst816t_hdl->_ctrl.y = ((data[4] & 0x0F) << 8) | data[5];
    //     return true;
    // }
    return false;
}

#if 0

// 结构体定义
typedef struct {
    uint8_t chip_id;
    uint8_t firmware_version;
    uint8_t gesture_id;
    uint8_t finger_num;
    uint16_t x;
    uint16_t y;
    int8_t rst_pin;
    int8_t irq_pin;
    bool tp_event;
} cst816t_t;

// 函数原型声明
static void tp_isr(void);
void cst816t_init(cst816t_t *dev, int8_t rst_pin, int8_t irq_pin);
void cst816t_begin(cst816t_t *dev, uint8_t tp_mode);
bool cst816t_available(cst816t_t *dev);
uint8_t cst816t_iic_read(uint8_t addr, uint8_t reg_addr, uint8_t *data, uint32_t len);
uint8_t cst816t_iic_write(uint8_t addr, uint8_t reg_addr, const uint8_t *data, uint32_t len);
const char* cst816t_version(cst816t_t *dev);
const char* cst816t_state(cst816t_t *dev);

// ISR 回调
static void tp_isr(void) {
    // 设置触摸事件
}

// 初始化触摸屏
void cst816t_init(cst816t_t *dev, int8_t rst_pin, int8_t irq_pin) {
    dev->rst_pin = rst_pin;
    dev->irq_pin = irq_pin;
    dev->tp_event = false;
}

// 触摸屏初始化
void cst816t_begin(cst816t_t *dev, uint8_t tp_mode) {
    uint8_t data[4] = {0};
    if (dev->rst_pin >= 0) {
        // 这里省略引脚操作，如复位
    }

    // 读取芯片ID和固件版本
    if (cst816t_iic_read(CST816T_ADDRESS, REG_CHIP_ID, data, 4) == 0) {
        dev->chip_id = data[0];
        dev->firmware_version = data[3];
    }

    // 配置中断控制和运动掩码
    uint8_t irq_en = 0, motion_mask = 0;
    switch (tp_mode) {
        case 0: irq_en = IRQ_EN_TOUCH; break;
        case 1: irq_en = IRQ_EN_CHANGE; break;
        case 2: irq_en = IRQ_EN_MOTION; break;
        case 3: irq_en = IRQ_EN_MOTION | IRQ_EN_LONGPRESS;
                motion_mask = MOTION_MASK_DOUBLE_CLICK; break;
    }

    cst816t_iic_write(CST816T_ADDRESS, REG_IRQ_CTL, &irq_en, 1);
    cst816t_iic_write(CST816T_ADDRESS, REG_MOTION_MASK, &motion_mask, 1);

    // 禁用自动休眠
    uint8_t dis_auto_sleep = 0xFF;
    cst816t_iic_write(CST816T_ADDRESS, REG_DIS_AUTOSLEEP, &dis_auto_sleep, 1);
}

// 检查触摸是否可用
bool cst816t_available(cst816t_t *dev) {
    uint8_t data[6] = {0};
    if (dev->tp_event && (cst816t_iic_read(CST816T_ADDRESS, REG_GESTURE_ID, data, 6) == 0)) {
        dev->tp_event = false;
        dev->gesture_id = data[0];
        dev->finger_num = data[1];
        dev->x = ((data[2] & 0x0F) << 8) | data[3];
        dev->y = ((data[4] & 0x0F) << 8) | data[5];
        return true;
    }
    return false;
}

// IIC 读操作（空实现）
uint8_t cst816t_iic_read(uint8_t addr, uint8_t reg_addr, uint8_t *data, uint32_t len) {
    // 这里是 IIC 读取的空实现
    return 0;
}

// IIC 写操作（空实现）
uint8_t cst816t_iic_write(uint8_t addr, uint8_t reg_addr, const uint8_t *data, uint32_t len) {
    // 这里是 IIC 写入的空实现
    return 0;
}

// 返回版本信息
const char* cst816t_version(cst816t_t *dev) {
    static char version[32];
    snprintf(version, sizeof(version), "Chip ID: 0x%02X, Firmware: %u", dev->chip_id, dev->firmware_version);
    return version;
}

// 返回触摸状态
const char* cst816t_state(cst816t_t *dev) {
    static char state[64];
    snprintf(state, sizeof(state), "Gesture: 0x%02X, Fingers: %u, X: %u, Y: %u",
             dev->gesture_id, dev->finger_num, dev->x, dev->y);
    return state;
}

#endif
