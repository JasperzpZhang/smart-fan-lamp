#ifndef __MQTT_PAL_H__
#define __MQTT_PAL_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

#include "Mqtt/MqttConfig.h"

/**
 * @file
 * @brief Includes/supports the types/calls required by the MQTT-C client.
 * 
 * @note This is the \em only file included in mqtt.h, and mqtt.c. It is therefore 
 *       responsible for including/supporting all the required types and calls. 
 * 
 * @defgroup pal Platform abstraction layer
 * @brief Documentation of the types and calls required to port MQTT-C to a new platform.
 * 
 * mqtt_pal.h is the \em only header file included in mqtt.c. Therefore, to port MQTT-C to a 
 * new platform the following types, functions, constants, and macros must be defined in 
 * mqtt_pal.h:
 *  - Types:
 *      - \c size_t, \c ssize_t
 *      - \c uint8_t, \c uint16_t, \c uint32_t
 *      - \c va_list
 *      - \c mqtt_pal_time_t : return type of \c MQTT_PAL_TIME() 
 *      - \c mqtt_pal_mutex_t : type of the argument that is passed to \c MQTT_PAL_MUTEX_LOCK and 
 *        \c MQTT_PAL_MUTEX_RELEASE
 *  - Functions:
 *      - \c memcpy, \c strlen
 *      - \c va_start, \c va_arg, \c va_end
 *  - Constants:
 *      - \c INT_MIN
 * 
 * Additionally, three macro's are required:
 *  - \c MQTT_PAL_HTONS(s) : host-to-network endian conversion for uint16_t.
 *  - \c MQTT_PAL_NTOHS(s) : network-to-host endian conversion for uint16_t.
 *  - \c MQTT_PAL_TIME()   : returns [type: \c mqtt_pal_time_t] current time in seconds. 
 *  - \c MQTT_PAL_MUTEX_LOCK(mtx_pointer) : macro that locks the mutex pointed to by \c mtx_pointer.
 *  - \c MQTT_PAL_MUTEX_RELEASE(mtx_pointer) : macro that unlocks the mutex pointed to by 
 *    \c mtx_pointer.
 * 
 * Lastly, \ref mqtt_pal_sendall and \ref mqtt_pal_recvall, must be implemented in mqtt_pal.c 
 * for sending and receiving data using the platforms socket calls.
 */

#if MQTT_LWIP
    #define __FREERTOS_LWIP__
    #undef  __FREERTOS_AT__
#endif /* MQTT_LWIP */
#if MQTT_AT
    #define __FREERTOS_AT__
    #undef  __FREERTOS_LWIP__
#endif /* MQTT_AT */

/* FreeRTOS+LwIP platform support */
#ifdef __FREERTOS_LWIP__
    #include <string.h>
    #include <stdarg.h>
    #include <limits.h>
    #include <time.h>
    #include <FreeRTOS.h>
    #include <semphr.h>
    #include <sockets.h>
    #include <Net/LwIP.h>
    
    #define MQTT_PAL_HTONS(s) htons(s)
    #define MQTT_PAL_NTOHS(s) ntohs(s)

    #define MQTT_PAL_TIME() time(NULL)

    typedef time_t mqtt_pal_time_t;
    typedef SemaphoreHandle_t mqtt_pal_mutex_t;
    typedef int mqtt_pal_socket_handle;
    typedef signed int ssize_t;

    #define MQTT_PAL_MUTEX_INIT(mtx_ptr) *mtx_ptr=xSemaphoreCreateMutex()
    #define MQTT_PAL_MUTEX_LOCK(mtx_ptr)  xSemaphoreTake(*mtx_ptr, portMAX_DELAY)
    #define MQTT_PAL_MUTEX_UNLOCK(mtx_ptr) xSemaphoreGive(*mtx_ptr)
    
    #pragma diag_suppress 83    /* warning: #83-D: type qualifier specified more than once */
    #pragma diag_suppress 111   /* warning: #111-D: statement is unreachable */
    #pragma diag_suppress 188   /* warning: #188-D: enumerated type mixed with another type */

#endif /* __FREERTOS_LWIP__ */

/* FreeRTOS+At platform support */
#ifdef __FREERTOS_AT__
    #include <string.h>
    #include <stdarg.h>
    #include <limits.h>
    #include <time.h>
    #include <FreeRTOS.h>
    #include <semphr.h>
    #include <sockets.h>
    #include <Net/LwIP.h>
    #include <At/At.h>
    
    #define MQTT_PAL_HTONS(s) htons(s)
    #define MQTT_PAL_NTOHS(s) ntohs(s)

    #define MQTT_PAL_TIME() time(NULL)

    typedef time_t mqtt_pal_time_t;
    typedef SemaphoreHandle_t mqtt_pal_mutex_t;
    typedef int mqtt_pal_socket_handle;
    typedef signed int ssize_t;

    #define MQTT_PAL_MUTEX_INIT(mtx_ptr) *mtx_ptr=xSemaphoreCreateMutex()
    #define MQTT_PAL_MUTEX_LOCK(mtx_ptr)  xSemaphoreTake(*mtx_ptr, portMAX_DELAY)
    #define MQTT_PAL_MUTEX_UNLOCK(mtx_ptr) xSemaphoreGive(*mtx_ptr)
    
    #pragma diag_suppress 83    /* warning: #83-D: type qualifier specified more than once */
    #pragma diag_suppress 111   /* warning: #111-D: statement is unreachable */
    #pragma diag_suppress 188   /* warning: #188-D: enumerated type mixed with another type */

#endif /* __FREERTOS_AT__ */

/**
 * @brief Sends all the bytes in a buffer.
 * @ingroup pal
 * 
 * @param[in] fd The file-descriptor (or handle) of the socket.
 * @param[in] buf A pointer to the first byte in the buffer to send.
 * @param[in] len The number of bytes to send (starting at \p buf).
 * @param[in] flags Flags which are passed to the underlying socket.
 * 
 * @returns The number of bytes sent if successful, an \ref MQTTErrors otherwise.
 */
ssize_t mqtt_pal_sendall(mqtt_pal_socket_handle fd, const void* buf, size_t len, int flags);

/**
 * @brief Non-blocking receive all the byte available.
 * @ingroup pal
 * 
 * @param[in] fd The file-descriptor (or handle) of the socket.
 * @param[in] buf A pointer to the receive buffer.
 * @param[in] bufsz The max number of bytes that can be put into \p buf.
 * @param[in] flags Flags which are passed to the underlying socket.
 * 
 * @returns The number of bytes received if successful, an \ref MQTTErrors otherwise.
 */
ssize_t mqtt_pal_recvall(mqtt_pal_socket_handle fd, void* buf, size_t bufsz, int flags);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif
