/*
    Net.h

    Head File for Net Module
*/

/* Copyright 2020 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 10Sep18, Karl Created
    01b, 03Dec18, Karl Modified
    01c, 03Aug19, Karl Reconstructured Net lib
    01d, 03Jun20, Karl Fixed warning enumerated type mixed with another type
*/

#ifndef __NET_H__
#define __NET_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/* Includes */
#include <stdint.h>
#include "Include/Include.h"
#include "Prot/Prot.h"
#include "Net/NetConfig.h"

/* Defines */
#ifndef NET_ENABLE_ASYNC
#define NET_ENABLE_ASYNC        (1)
#endif
#ifndef NET_TASK_STACK_SIZE
#define NET_TASK_STACK_SIZE     (512)
#endif
#ifndef NET_TASK_PRIORITY
#define NET_TASK_PRIORITY       (osPriorityNormal)
#endif
#ifndef NET_TASK_DELAY
#define NET_TASK_DELAY          (25)    /* ms */
#endif
#ifndef NET_RECV_BUF_SIZE
#define NET_RECV_BUF_SIZE       (128)
#endif
#ifndef NET_SEND_BUF_SIZE
#define NET_SEND_BUF_SIZE       (128)
#endif
#ifndef NET_SEND_RBUF_CNT
#define NET_SEND_RBUF_CNT       (3)
#endif
#ifndef NET_DHCP_TIMEOUT
#define NET_DHCP_TIMEOUT        (3)     /* Second */
#endif
#ifndef NET_CONN_TIMEOUT
#define NET_CONN_TIMEOUT        (3)     /* Second */
#endif
#ifndef NET_MAX_SVR_NAME
#define NET_MAX_SVR_NAME        (64)
#endif
#ifndef ETH_TASK_STACK_SIZE
#define ETH_TASK_STACK_SIZE     (128)
#endif
#ifndef ETH_TASK_PRIORITY
#define ETH_TASK_PRIORITY       (osPriorityNormal)
#endif
#ifndef ETH_TASK_DELAY
#define ETH_TASK_DELAY          (1000)  /* ms */
#endif

/* Byte order */
#define NET_LITTLE_ENDIAN       (0)
#define NET_BIG_ENDIAN          (1)
#define NET_ENDIAN              NET_LITTLE_ENDIAN
#if (NEG_ENDIAN == NET_LITTLE_ENDIAN)
#define MAKE_IPADDR(a,b,c,d)    ((uint32_t)((d) & 0xff) << 24) | \
                                ((uint32_t)((c) & 0xff) << 16) | \
                                ((uint32_t)((b) & 0xff) << 8)  | \
                                ((uint32_t)((a) & 0xff))
#endif /* (NEG_ENDIAN == NET_LITTLE_ENDIAN) */
#if (NEG_ENDIAN == NET_BIG_ENDIAN)
#define MAKE_IPADDR(a,b,c,d)    ((uint32_t)((a) & 0xff) << 24) | \
                                ((uint32_t)((b) & 0xff) << 16) | \
                                ((uint32_t)((c) & 0xff) << 8)  | \
                                ((uint32_t)((d) & 0xff))
#endif /* (NEG_ENDIAN == NET_BIG_ENDIAN) */

/* Types */
typedef uint32_t NetHandle_t;
typedef Status_t(*NetRecvProcFunc_t)(NetHandle_t, uint8_t *pucData, uint16_t usSize, void* pvPara);
typedef Status_t(*NetEventFunc_t)(NetHandle_t);

typedef enum {
    NET_EVENT_TCP_CONNECTED,
    NET_EVENT_TCP_DISCONNECTED,
    NET_EVENT_NUM
}NetEventType_t;

/* Functions */
Status_t        NetInit(void);
Status_t        NetTerm(void);

NetHandle_t     NetCreate(void);
Status_t        NetDelete(NetHandle_t xHandle);

#define         NET_CONFIG_ETH_PARA Bool_t bDhcp, uint32_t usDhcpTimeout, uint32_t ulLocalIp, uint32_t ulLocalNetMask, uint32_t ulLocalGwAddr, const char* cSvrName, uint16_t usServerPort, void *pxNetIf
Status_t        NetConfigEth(NET_CONFIG_ETH_PARA);
Status_t        NetConfigTcpClient(NetHandle_t xHandle, ProtHandle_t xProt, NetRecvProcFunc_t pxNetRecvProc, void* pvPara);
Status_t        NetConfigTcpEvent(NetHandle_t xHandle, NetEventType_t xEvType, NetEventFunc_t pxEventFunc);
Status_t        NetStart(NetHandle_t xHandle);

Status_t        NetSendBlk(NetHandle_t xHandle, uint8_t* pucBuffer, uint16_t usSize);
#if NET_ENABLE_ASYNC
Status_t        NetSendAsync(NetHandle_t xHandle, uint8_t* pucBuffer, uint16_t usSize);
#endif /* NET_ENABLE_ASYNC */

#if NET_TEST
Status_t NetTest(void);
#endif /* NET_TEST */

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __NET_H__ */
