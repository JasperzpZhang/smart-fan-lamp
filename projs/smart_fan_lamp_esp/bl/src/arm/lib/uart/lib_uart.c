/**
 * \file            lib_uart.c
 * \brief           Uart library
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
 * This file includes all the library functions for uart.
 *
 * Author:          Jasper <jasperzhangse@gmail.com>
 * Version:         v1.1.0
 */

/* Includes */
//#include <cmsis_os.h>
#include "lib/uart/lib_uart.h"
#include "lib/debug/lib_debug.h"

#if UART_ENABLE

/* Pragmas */

/* Debug config */
#if UART_DEBUG
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* UART_DEBUG */
#if UART_ASSERT
#undef ASSERT
#define ASSERT(a)  while (!(a)) debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__);
#else
#undef ASSERT
#define ASSERT(...)
#endif /* UART_ASSERT */

/* Local defines */

/* Local types */

/* Functions */
/**
  * @brief  UART1 Reception Event Callback (Rx event notification).
  *         This function is called after data is received using advanced reception service.
  * @param  huart UART handle
  * @param  size  Number of bytes received in the reception buffer.
  *               Indicates the position in the buffer until which data is available.
  * @retval None
  */
__weak void uart1_rx_event_callback(UART_HandleTypeDef *huart, uint16_t size) 
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  UNUSED(size);

  /* NOTE : This function should not be modified. When the callback is needed,
            uart1_rx_event_callback can be implemented in the user file.
   */
}

/**
  * @brief  UART2 Reception Event Callback (Rx event notification).
  *         This function is called after data is received using advanced reception service.
  * @param  huart UART handle
  * @param  size  Number of bytes received in the reception buffer.
  *               Indicates the position in the buffer until which data is available.
  * @retval None
  */
__weak void uart2_rx_event_callback(UART_HandleTypeDef *huart, uint16_t size) {
    /* Prevent unused argument(s) compilation warning */
    UNUSED(huart);
    UNUSED(size);

    /* NOTE : This function should not be modified. When the callback is needed,
              uart2_rx_event_callback can be implemented in the user file.
     */
}

/**
  * @brief  UART3 Reception Event Callback (Rx event notification).
  *         This function is called after data is received using advanced reception service.
  * @param  huart UART handle
  * @param  size  Number of bytes received in the reception buffer.
  *               Indicates the position in the buffer until which data is available.
  * @retval None
  */
__weak void uart3_rx_event_callback(UART_HandleTypeDef *huart, uint16_t size) {
    /* Prevent unused argument(s) compilation warning */
    UNUSED(huart);
    UNUSED(size);

    /* NOTE : This function should not be modified. When the callback is needed,
              uart3_rx_event_callback can be implemented in the user file.
     */
}

/**
  * @brief  UART4 Reception Event Callback (Rx event notification).
  *         This function is called after data is received using advanced reception service.
  * @param  huart UART handle
  * @param  size  Number of bytes received in the reception buffer.
  *               Indicates the position in the buffer until which data is available.
  * @retval None
  */
__weak void uart4_rx_event_callback(UART_HandleTypeDef *huart, uint16_t size) {
    /* Prevent unused argument(s) compilation warning */
    UNUSED(huart);
    UNUSED(size);

    /* NOTE : This function should not be modified. When the callback is needed,
              uart4_rx_event_callback can be implemented in the user file.
     */
}

/**
  * @brief  UART5 Reception Event Callback (Rx event notification).
  *         This function is called after data is received using advanced reception service.
  * @param  huart UART handle
  * @param  size  Number of bytes received in the reception buffer.
  *               Indicates the position in the buffer until which data is available.
  * @retval None
  */
__weak void uart5_rx_event_callback(UART_HandleTypeDef *huart, uint16_t size) {
    /* Prevent unused argument(s) compilation warning */
    UNUSED(huart);
    UNUSED(size);

    /* NOTE : This function should not be modified. When the callback is needed,
              uart5_rx_event_callback can be implemented in the user file.
     */
}

/**
  * @brief  UART6 Reception Event Callback (Rx event notification).
  *         This function is called after data is received using advanced reception service.
  * @param  huart UART handle
  * @param  size  Number of bytes received in the reception buffer.
  *               Indicates the position in the buffer until which data is available.
  * @retval None
  */
__weak void uart6_rx_event_callback(UART_HandleTypeDef *huart, uint16_t size) {
    /* Prevent unused argument(s) compilation warning */
    UNUSED(huart);
    UNUSED(size);

    /* NOTE : This function should not be modified. When the callback is needed,
              uart6_rx_event_callback can be implemented in the user file.
     */
}

/**
  * @brief  UART7 Reception Event Callback (Rx event notification).
  *         This function is called after data is received using advanced reception service.
  * @param  huart UART handle
  * @param  size  Number of bytes received in the reception buffer.
  *               Indicates the position in the buffer until which data is available.
  * @retval None
  */
__weak void uart7_rx_event_callback(UART_HandleTypeDef *huart, uint16_t size) {
    /* Prevent unused argument(s) compilation warning */
    UNUSED(huart);
    UNUSED(size);

    /* NOTE : This function should not be modified. When the callback is needed,
              uart7_rx_event_callback can be implemented in the user file.
     */
}

/**
  * @brief  UART8 Reception Event Callback (Rx event notification).
  *         This function is called after data is received using advanced reception service.
  * @param  huart UART handle
  * @param  size  Number of bytes received in the reception buffer.
  *               Indicates the position in the buffer until which data is available.
  * @retval None
  */
__weak void uart8_rx_event_callback(UART_HandleTypeDef *huart, uint16_t size) {
    /* Prevent unused argument(s) compilation warning */
    UNUSED(huart);
    UNUSED(size);

    /* NOTE : This function should not be modified. When the callback is needed,
              uart8_rx_event_callback can be implemented in the user file.
     */
}

/**
  * @brief  UART Reception Event Callback.
  *         This function is called by the HAL when data is received using the advanced reception service.
  *         It determines the UART instance and calls the corresponding user-defined callback function.
  * @param  huart UART handle
  * @param  Size  Number of bytes received in the reception buffer.
  *               Indicates the position in the buffer until which data is available.
  * @retval None
  */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    switch ((uint32_t)huart->Instance)
    {
#ifdef USART1
        case (uint32_t)USART1:
            uart1_rx_event_callback(huart, Size);
            break;
#endif

#ifdef USART2
        case (uint32_t)USART2:
            uart2_rx_event_callback(huart, Size);
            break;
#endif

#ifdef USART3
        case (uint32_t)USART3:
            uart3_rx_event_callback(huart, Size);
            break;
#endif

#ifdef UART4
        case (uint32_t)UART4:
            uart4_rx_event_callback(huart, Size);
            break;
#endif

#ifdef UART5
        case (uint32_t)UART5:
            uart5_rx_event_callback(huart, Size);
            break;
#endif

#ifdef USART6
        case (uint32_t)USART6:
            uart6_rx_event_callback(huart, Size);
            break;
#endif

#ifdef UART7
        case (uint32_t)UART7:
            uart7_rx_event_callback(huart, Size);
            break;
#endif

#ifdef UART8
        case (uint32_t)UART8:
            uart8_rx_event_callback(huart, Size);
            break;
#endif

        default:
            /* Do nothing. */
            break;
    }
}

/**
  * @brief  UART1 Reception Complete Callback.
  *         This function is called when the UART1 reception is complete.
  * @param  huart UART handle
  * @retval None
  */
__weak void uart1_rx_cplt_callback(UART_HandleTypeDef *huart) {
    /* Prevent unused argument(s) compilation warning */
    UNUSED(huart);

    /* NOTE : This function should not be modified. When the callback is needed,
              uart1_rx_cplt_callback can be implemented in the user file.
     */
}

/**
  * @brief  UART2 Reception Complete Callback.
  *         This function is called when the UART2 reception is complete.
  * @param  huart UART handle
  * @retval None
  */
__weak void uart2_rx_cplt_callback(UART_HandleTypeDef *huart) {
    /* Prevent unused argument(s) compilation warning */
    UNUSED(huart);

    /* NOTE : This function should not be modified. When the callback is needed,
              uart2_rx_cplt_callback can be implemented in the user file.
     */
}

/**
  * @brief  UART3 Reception Complete Callback.
  *         This function is called when the UART3 reception is complete.
  * @param  huart UART handle
  * @retval None
  */
__weak void uart3_rx_cplt_callback(UART_HandleTypeDef *huart) {
    /* Prevent unused argument(s) compilation warning */
    UNUSED(huart);

    /* NOTE : This function should not be modified. When the callback is needed,
              uart3_rx_cplt_callback can be implemented in the user file.
     */
}

/**
  * @brief  UART4 Reception Complete Callback.
  *         This function is called when the UART4 reception is complete.
  * @param  huart UART handle
  * @retval None
  */
__weak void uart4_rx_cplt_callback(UART_HandleTypeDef *huart) {
    /* Prevent unused argument(s) compilation warning */
    UNUSED(huart);

    /* NOTE : This function should not be modified. When the callback is needed,
              uart4_rx_cplt_callback can be implemented in the user file.
     */
}

/**
  * @brief  UART5 Reception Complete Callback.
  *         This function is called when the UART5 reception is complete.
  * @param  huart UART handle
  * @retval None
  */
__weak void uart5_rx_cplt_callback(UART_HandleTypeDef *huart) {
    /* Prevent unused argument(s) compilation warning */
    UNUSED(huart);

    /* NOTE : This function should not be modified. When the callback is needed,
              uart5_rx_cplt_callback can be implemented in the user file.
     */
}

/**
  * @brief  UART6 Reception Complete Callback.
  *         This function is called when the UART6 reception is complete.
  * @param  huart UART handle
  * @retval None
  */
__weak void uart6_rx_cplt_callback(UART_HandleTypeDef *huart) {
    /* Prevent unused argument(s) compilation warning */
    UNUSED(huart);

    /* NOTE : This function should not be modified. When the callback is needed,
              uart6_rx_cplt_callback can be implemented in the user file.
     */
}

/**
  * @brief  UART7 Reception Complete Callback.
  *         This function is called when the UART7 reception is complete.
  * @param  huart UART handle
  * @retval None
  */
__weak void uart7_rx_cplt_callback(UART_HandleTypeDef *huart) {
    /* Prevent unused argument(s) compilation warning */
    UNUSED(huart);

    /* NOTE : This function should not be modified. When the callback is needed,
              uart7_rx_cplt_callback can be implemented in the user file.
     */
}

/**
  * @brief  UART8 Reception Complete Callback.
  *         This function is called when the UART8 reception is complete.
  * @param  huart UART handle
  * @retval None
  */
__weak void uart8_rx_cplt_callback(UART_HandleTypeDef *huart) {
    /* Prevent unused argument(s) compilation warning */
    UNUSED(huart);

    /* NOTE : This function should not be modified. When the callback is needed,
              uart8_rx_cplt_callback can be implemented in the user file.
     */
}

/**
  * @brief  UART Reception Complete Callback.
  *         This function is called by the HAL when UART reception is complete.
  *         It determines the UART instance and calls the corresponding user-defined callback function.
  * @param  huart UART handle
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    switch ((uint32_t)huart->Instance)
    {
#ifdef USART1
        case (uint32_t)USART1:
            uart1_rx_cplt_callback(huart);
            break;
#endif

#ifdef USART2
        case (uint32_t)USART2:
            uart2_rx_cplt_callback(huart);
            break;
#endif

#ifdef USART3
        case (uint32_t)USART3:
            uart3_rx_cplt_callback(huart);
            break;
#endif

#ifdef UART4
        case (uint32_t)UART4:
            uart4_rx_cplt_callback(huart);
            break;
#endif

#ifdef UART5
        case (uint32_t)UART5:
            uart5_rx_cplt_callback(huart);
            break;
#endif

#ifdef USART6
        case (uint32_t)USART6:
            uart6_rx_cplt_callback(huart);
            break;
#endif

#ifdef UART7
        case (uint32_t)UART7:
            uart7_rx_cplt_callback(huart);
            break;
#endif

#ifdef UART8
        case (uint32_t)UART8:
            uart8_rx_cplt_callback(huart);
            break;
#endif

        default:
            /* Do nothing. */
            break;
    }
}

#endif /* UART_ENABLE */
