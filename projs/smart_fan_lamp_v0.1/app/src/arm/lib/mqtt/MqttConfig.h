/*
    MqttConfig.h

    Configuration File for Mqtt Module
*/

/* Copyright 2019 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 15Jul19, Karl Created
    01b, 17Mar20, Karl Added MQTT_LWIP & MQTT_AT
*/

#ifndef __MQTT_CONFIG_H__
#define __MQTT_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/* Includes */
#include "Config.h"

/* Configuration defines */
#ifndef MQTT_ENABLE
#define MQTT_ENABLE         (0)
#endif
#ifndef MQTT_LWIP
#define MQTT_LWIP           (1)
#endif
#ifndef MQTT_AT
#define MQTT_AT             (0)
#endif
#ifndef MQTT_RTOS
#define MQTT_RTOS           (1)
#endif
#ifndef MQTT_DEBUG
#define MQTT_DEBUG          (0)
#endif
#ifndef MQTT_ASSERT
#define MQTT_ASSERT         (0)
#endif
#ifndef MQTT_TEST
#define MQTT_TEST           (0)
#endif

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __MQTT_CONFIG_H__ */
