#include <Mqtt/Mqtt.h>

#if MQTT_ENABLE

/* Debug config */
#if MQTT_DEBUG
    #undef TRACE
    #define TRACE(...)  DebugPrintf(__VA_ARGS__)
#else
    #undef TRACE
    #define TRACE(...)
#endif /* MQTT_DEBUG */
#if MQTT_ASSERT
    #undef ASSERT
    #define ASSERT(a)   while(!(a)){DebugPrintf("ASSERT failed: %s %d\n", __FILE__, __LINE__);}
#else
    #undef ASSERT
    #define ASSERT(...)
#endif /* MQTT_ASSERT */

/** 
 * @file 
 * @brief Implements @ref mqtt_pal_sendall and @ref mqtt_pal_recvall and 
 *        any platform-specific helpers you'd like.
 * @cond Doxygen_Suppress
 */



#ifdef __FREERTOS_LWIP__

ssize_t mqtt_pal_sendall(mqtt_pal_socket_handle fd, const void* buf, size_t len, int flags) {
    size_t sent = 0;
    while(sent < len) {
        ssize_t tmp = send(fd, (uint8_t*)buf + sent, len - sent, flags);
        if (tmp < 1) {
            return MQTT_ERROR_SOCKET_ERROR;
        }
        sent += (size_t) tmp;
    }
    return sent;
}

ssize_t mqtt_pal_recvall(mqtt_pal_socket_handle fd, void* buf, size_t bufsz, int flags) {
    const void const *start = buf;
    ssize_t rv;
    int err = 0;
    socklen_t optlen = sizeof(socklen_t);
    do {
        rv = recv(fd, buf, bufsz, flags);
        /* get socket error number */
        getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &optlen);
        if (rv > 0) {
            /* successfully read bytes from the socket */
            buf = (uint8_t*)buf + rv;
            bufsz -= rv;
        } else if (rv < 0 && err != EWOULDBLOCK) {
            /* an error occurred that wasn't "nothing to read". */
            return MQTT_ERROR_SOCKET_ERROR;
        }
    } while (rv > 0);

    return (uint8_t*)buf - (uint8_t*)start;
}

#endif /* __FREERTOS_LWIP__ */

#ifdef __FREERTOS_AT__

ssize_t mqtt_pal_sendall(mqtt_pal_socket_handle fd, const void* buf, size_t len, int flags) {
    size_t sent = 0;
    while(sent < len) {
        ssize_t tmp = at_send(fd, (uint8_t*)buf + sent, len - sent, flags);
        if (tmp < 1) {
            return MQTT_ERROR_SOCKET_ERROR;
        }
        sent += (size_t) tmp;
    }
    return sent;
}

ssize_t mqtt_pal_recvall(mqtt_pal_socket_handle fd, void* buf, size_t bufsz, int flags) {
    const void const *start = buf;
    ssize_t rv;
    int err = 0;
    socklen_t optlen = sizeof(socklen_t);
    do {
        rv = at_recv(fd, buf, bufsz, flags | MSG_DONTWAIT);
        /* get socket error number */
        at_getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &optlen);
        if (rv > 0) {
            /* successfully read bytes from the socket */
            buf = (uint8_t*)buf + rv;
            bufsz -= rv;
        } else if (rv < 0 && err != EAGAIN/*EWOULDBLOCK*/) {
            /* an error occurred that wasn't "nothing to read". */
            return MQTT_ERROR_SOCKET_ERROR;
        }
    } while (rv > 0);

    return (uint8_t*)buf - (uint8_t*)start;
}

#endif /* __FREERTOS_AT__ */

/** @endcond */

#endif /* MQTT_ENABLE */
