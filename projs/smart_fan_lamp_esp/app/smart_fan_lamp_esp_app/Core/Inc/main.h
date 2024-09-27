/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define GYRO_INT1_Pin GPIO_PIN_2
#define GYRO_INT1_GPIO_Port GPIOE
#define GYRO_INT1_EXTI_IRQn EXTI2_IRQn
#define GYRO_INT2_Pin GPIO_PIN_3
#define GYRO_INT2_GPIO_Port GPIOE
#define GYRO_INT2_EXTI_IRQn EXTI3_IRQn
#define SYS_LED_Pin GPIO_PIN_4
#define SYS_LED_GPIO_Port GPIOE
#define BEEP_Pin GPIO_PIN_5
#define BEEP_GPIO_Port GPIOE
#define GES_INT_Pin GPIO_PIN_6
#define GES_INT_GPIO_Port GPIOE
#define GES_INT_EXTI_IRQn EXTI9_5_IRQn
#define VBUS_EN_Pin GPIO_PIN_3
#define VBUS_EN_GPIO_Port GPIOC
#define WAVE_EN_Pin GPIO_PIN_3
#define WAVE_EN_GPIO_Port GPIOA
#define NIGHT_LIGHT_EN_Pin GPIO_PIN_4
#define NIGHT_LIGHT_EN_GPIO_Port GPIOA
#define USB_POWER_EN_Pin GPIO_PIN_10
#define USB_POWER_EN_GPIO_Port GPIOE
#define KEY_TP_POWER_Pin GPIO_PIN_14
#define KEY_TP_POWER_GPIO_Port GPIOE
#define AUDIO_EN_Pin GPIO_PIN_15
#define AUDIO_EN_GPIO_Port GPIOE
#define KEY_LED15_Pin GPIO_PIN_12
#define KEY_LED15_GPIO_Port GPIOB
#define KEY_LED14_Pin GPIO_PIN_13
#define KEY_LED14_GPIO_Port GPIOB
#define KEY_LED13_Pin GPIO_PIN_14
#define KEY_LED13_GPIO_Port GPIOB
#define KEY_LED12_Pin GPIO_PIN_15
#define KEY_LED12_GPIO_Port GPIOB
#define KEY_LED11_Pin GPIO_PIN_8
#define KEY_LED11_GPIO_Port GPIOD
#define KEY_LED1_Pin GPIO_PIN_9
#define KEY_LED1_GPIO_Port GPIOD
#define KEY_LED2_Pin GPIO_PIN_10
#define KEY_LED2_GPIO_Port GPIOD
#define KEY_LED3_Pin GPIO_PIN_11
#define KEY_LED3_GPIO_Port GPIOD
#define KEY_LED4_Pin GPIO_PIN_12
#define KEY_LED4_GPIO_Port GPIOD
#define KEY_LED5_Pin GPIO_PIN_13
#define KEY_LED5_GPIO_Port GPIOD
#define KEY_LED6_Pin GPIO_PIN_14
#define KEY_LED6_GPIO_Port GPIOD
#define KEY_LED7_Pin GPIO_PIN_15
#define KEY_LED7_GPIO_Port GPIOD
#define KEY_LED8_Pin GPIO_PIN_8
#define KEY_LED8_GPIO_Port GPIOC
#define KEY_LED9_Pin GPIO_PIN_9
#define KEY_LED9_GPIO_Port GPIOC
#define KEY_LED10_Pin GPIO_PIN_8
#define KEY_LED10_GPIO_Port GPIOA
#define KEY_LED17_Pin GPIO_PIN_11
#define KEY_LED17_GPIO_Port GPIOA
#define KEY_LED16_Pin GPIO_PIN_12
#define KEY_LED16_GPIO_Port GPIOA
#define SPI3_FLASH_CS_Pin GPIO_PIN_15
#define SPI3_FLASH_CS_GPIO_Port GPIOA
#define SPI3_LCD_CS_Pin GPIO_PIN_1
#define SPI3_LCD_CS_GPIO_Port GPIOD
#define LCD_RESET_Pin GPIO_PIN_2
#define LCD_RESET_GPIO_Port GPIOD
#define LCD_TP_INT_Pin GPIO_PIN_4
#define LCD_TP_INT_GPIO_Port GPIOD
#define LCD_TP_INT_EXTI_IRQn EXTI4_IRQn
#define KEY_INT_Pin GPIO_PIN_5
#define KEY_INT_GPIO_Port GPIOB
#define KEY_INT_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
