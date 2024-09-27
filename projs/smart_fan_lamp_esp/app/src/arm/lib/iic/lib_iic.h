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
 * @file      lib_iic.h
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

#ifndef __LIB_IIC_H__
#define __LIB_IIC_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes */
#include "app/config.h"
#include "lib/type/lib_type.h"
#include "main.h"

#if SORTWARE_I2C_ENABLE

/* Defines */
/**
 * @brief iic gpio definition
 */
#ifndef IIC_SCL_Pin
#define IIC_SCL_Pin GPIO_PIN_6
#endif
#ifndef IIC_SCL_GPIO_Port
#define IIC_SCL_GPIO_Port GPIOB
#endif
#ifndef IIC_SDA_Pin
#define IIC_SDA_Pin GPIO_PIN_7
#endif
#ifndef IIC_SDA_GPIO_Port
#define IIC_SDA_GPIO_Port GPIOB
#endif

/**
 * @brief iic gpio contral function
 */
#define IIC_SCL(state)        HAL_GPIO_WritePin(IIC_SCL_GPIO_Port, IIC_SCL_Pin, (state ? GPIO_PIN_SET : GPIO_PIN_RESET))
#define IIC_SDA(state)        HAL_GPIO_WritePin(IIC_SDA_GPIO_Port, IIC_SDA_Pin, (state ? GPIO_PIN_SET : GPIO_PIN_RESET))
#define IIC_READ_SDA          HAL_GPIO_ReadPin(IIC_SDA_GPIO_Port, IIC_SDA_Pin)

typedef enum {
    high = 1,
    low = 0,
} i2c_bus_status_t;

/**
 * @brief  iic bus init
 * @return status code
 *         - status_err error
 *         - status_ok  success
 * @note   none
 */
status_t
iic_init(void);

/**
 * @brief  iic bus deinit
 * @return status code
 *         - status_err error
 *         - status_ok  success
 * @note   none
 */
status_t
iic_deinit(void);

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
status_t iic_write_cmd(uint8_t addr, uint8_t *buf, uint16_t len);

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
iic_write_addr8(uint8_t addr, uint8_t reg, uint8_t* buf, uint8_t len);

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
status_t iic_write_addr16(uint8_t addr, uint16_t reg, uint8_t *buf, uint16_t len);

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
status_t iic_read_cmd(uint8_t addr, uint8_t *buf, uint16_t len);

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
iic_read_addr8(uint8_t addr, uint8_t reg, uint8_t* buf, uint8_t len);

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
status_t iic_read_addr16(uint8_t addr, uint16_t reg, uint8_t *buf, uint16_t len);


status_t iic_write_data(uint8_t device_address, uint8_t *data, uint16_t length);
status_t iic_read_data(uint8_t device_address, uint8_t *data, uint16_t length, uint8_t ack);


#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LIB_IIC_H__ */
