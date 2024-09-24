/*
    define.h

    Include file for whole project
*/

/*
    modification history
    --------------------
    01a, 18May24, Jasper Created
*/

#ifndef __DEFINE_H__
#define __DEFINE_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define PIN(a)      (a##_GPIO_PIN)
#define PORT(a)     (a##_GPIO_PORT)

/* Led moudle */
#define LED_COLD_TIM                    &htim3
#define LED_COLD_TIM_CHANNEL            TIM_CHANNEL_1
#define LED_WARM_TIM                    &htim3
#define LED_WARM_TIM_CHANNEL            TIM_CHANNEL_2
#define LED_NIGHT_LIGHT_GPIO_PORT       GPIOC
#define LED_NIGHT_LIGHT_GPIO_PIN        GPIO_PIN_13

/* Fan moudle */
#define FAN_FORWARD_TIM                 &htim2
#define FAN_FORWARD_CHANNEL             TIM_CHANNEL_2
#define FAN_REVERSE_TIM                 &htim2
#define FAN_REVERSE_CHANNEL             TIM_CHANNEL_1

/* Wave moudle */
#define WAVE_GPIO_PORT                  GPIOB
#define WAVE_GPIO_PIN                   GPIO_PIN_7

/* Usb moudle */
#define USB_SW_GPIO_PORT                GPIOB
#define USB_SW_GPIO_PIN                 GPIO_PIN_15

/* Spi flash moudle */
#define SPI3_CS_GPIO_PORT               GPIOD
#define SPI3_CS_GPIO_PIN                GPIO_PIN_7

/* wifi moudle */
#define WIFI_XIAOMI_EN_PORT             GPIOB
#define WIFI_XIAOMI_EN_PIN              GPIO_PIN_0
#define WIFI_XIAOMI_EN_ON               GPIO_PIN_RESET
#define WIFI_XIAOMI_EN_OFF              GPIO_PIN_SET

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __DEFINE_H__ */
