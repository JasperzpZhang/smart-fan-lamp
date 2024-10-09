#include <stdio.h>
#include <stdlib.h>
#include <sockets.h>
#include <netdb.h>
#include <At/At.h>
#include <Mqtt/MqttAt.h>

#if (MQTT_ENABLE && MQTT_AT)

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

/*
    Opening a non-blocking lwip socket.
*/
int open_nb_socket(const char* addr, const char* port) {
    struct sockaddr_in addr_in;
    int sockfd = -1;
    struct hostent *host;
    int ret;

    host = at_gethostbyname(addr);
    if ((NULL == host) || (NULL == host->h_addr_list[0])) {
        TRACE("Failed to gethostbyname\n");
        return -1;
    }

    /* get address information */
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(atoi(port));
    addr_in.sin_addr.s_addr = *(u32_t*)host->h_addr_list[0];
    if (addr_in.sin_addr.s_addr == INADDR_NONE) {
        TRACE("Failed to get %s addr\n", addr);
        return -1;
    }

    /* create socket */
    sockfd = at_socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        TRACE("Failed to create socket\n");
        return -1;
    }

    /* connect to server */
    ret = at_connect(sockfd, (struct sockaddr*)&addr_in, sizeof(struct sockaddr));
    if (ret == SOCKET_ERROR) {
        at_closesocket(sockfd);
        TRACE("Failed to connect server\n");
        return -1;
    }

    /* make non-blocking */
    if (sockfd != -1) {
        /* XXX: Do nothing here */
    }

    /* return the new socket fd */
    return sockfd;
}

#endif /* (MQTT_ENABLE && MQTT_AT) */
