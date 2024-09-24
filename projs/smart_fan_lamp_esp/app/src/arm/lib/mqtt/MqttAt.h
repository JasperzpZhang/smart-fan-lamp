#ifndef __AT_SOCKET_H__
#define __AT_SOCKET_H__

#include "Mqtt/MqttConfig.h"

#if MQTT_AT

/* Opening a non-blocking lwip socket. */
int open_nb_socket(const char* addr, const char* port);

#endif /* MQTT_AT */

#endif /* __AT_SOCKET_H__ */
