/*
    LwIP.h

    Head File for Custom LwIP Module
*/

/* Copyright 2019 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 12Sep18, Karl Created
    01b, 04Dec18, Karl Modified
    01c, 03Aug19, Karl Reconstructured Net lib
*/

#ifndef __LWIP_H__
#define __LWIP_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/* Includes */
#include <stdint.h>
#include "Include/Include.h"
#include "Net/NetConfig.h"

/* Defines */
#ifndef LWIP_DEFAULT_IPADDR
#define LWIP_DEFAULT_IPADDR     {192, 168, 1, 10}
#endif
#ifndef LWIP_DEFAULT_NETMASK
#define LWIP_DEFAULT_NETMASK    {255, 255, 255, 0}
#endif
#ifndef LWIP_DEFAULT_GATEWAY
#define LWIP_DEFAULT_GATEWAY    {192, 168, 1, 1}
#endif
#ifndef LWIP_DEFAULT_DHCP
#define LWIP_DEFAULT_DHCP       FALSE
#endif

/* Functions */
Status_t    LwIPInit(void);
Status_t    LwIPTerm(void);

Status_t    LwIPConfigDhcp(Bool_t bDhcp);
Status_t    LwIPConfigIpAddr(uint32_t ulIpAddr, uint32_t ulNetMask, uint32_t ulGwAddr);
Status_t    LwIPStart(void);

void*       LwIPGetNetIf(void);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __LWIP_H_ */
