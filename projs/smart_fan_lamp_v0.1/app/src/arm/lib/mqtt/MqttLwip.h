#ifndef __LWIP_SOCKET_H__
#define __LWIP_SOCKET_H__

#include "Mqtt/MqttConfig.h"

/* Opening a non-blocking lwip socket. */
int open_nb_socket(const char* addr, const char* port);

#endif /* __LWIP_SOCKET_H__ */
