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
extern "C" {
#endif /* __cplusplus */

/* Defines */

#define PIN(a)  (a##_Pin)
#define PORT(a) (a##_GPIO_Port)

/* Types */
typedef enum {
    WAVE_DETC,

} gpio_in_t;

typedef enum {
    SYS_LED,
    VBUS_EN,
    WAVE_EN,
    NIGHT_LIGHT_EN,
    USB_PWR_EN,
    AUDIO_EN,
    KEY_LED1,
    KEY_LED2,
    KEY_LED3,
    KEY_LED4,
    KEY_LED5,
    KEY_LED6,
    KEY_LED7,
    KEY_LED8,
    KEY_LED9,
    KEY_LED10,
    KEY_LED11,
    KEY_LED12,
    KEY_LED13,
    KEY_LED14,
    KEY_LED15,
    KEY_LED16,
    KEY_LED17,
    SPI3_FLASH_CS,
    SPI3_LCD_CS,

} gpio_out_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __DEFINE_H__ */
