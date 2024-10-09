/*
    LwIP.c

    Implementation File for Custom LwIP Module
*/

/* Copyright 2019 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 12Sep18, Karl Created
    01b, 04Dec18, Karl Modified
    01c, 03Aug19, Karl Reconstructured Net lib
*/

/* Includes */
#include <stm32f1xx_hal.h>
#include "lwip/init.h"
#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/dhcp.h"
#include "lwip/netif.h"
#include "lwip/lwip_timers.h"
#include "lwip/tcpip.h" /* Needed with used with RTOS */
#include "netif/etharp.h"
#include "EthernetIf.h"
#include "Net/LwIP.h"

#if (NET_ENABLE && NET_ENABLE_LWIP)

/* Debug config */
#if NET_DEBUG
    #undef TRACE
    #define TRACE(...)  DebugPrintf(__VA_ARGS__)
#else
    #undef TRACE
    #define TRACE(...)
#endif /* NET_DEBUG */
#if NET_ASSERT
    #undef ASSERT
    #define ASSERT(a)   while(!(a)){DebugPrintf("ASSERT failed: %s %d\n", __FILE__, __LINE__);}
#else
    #undef ASSERT
    #define ASSERT(...)
#endif /* NET_ASSERT */
  
/* Local variables */
static struct netif     s_xNetIf;
static ip_addr_t        s_xIpAddr       = {.addr = 0};
static ip_addr_t        s_xNetMask      = {.addr = 0};
static ip_addr_t        s_xGwAddr       = {.addr = 0};
static uint8_t          s_ucIpAddr[4]   = LWIP_DEFAULT_IPADDR;
static uint8_t          s_ucNetMask[4]  = LWIP_DEFAULT_NETMASK;
static uint8_t          s_ucGwAddr[4]   = LWIP_DEFAULT_GATEWAY;
static Bool_t           s_bDhcp         = LWIP_DEFAULT_DHCP;

/* Functions */
Status_t LwIPInit(void)
{
    /* Do nothing */
    return STATUS_OK;
}

Status_t LwIPTerm(void)
{
    /* Do nothing */
    return STATUS_OK;
}

Status_t LwIPConfigDhcp(Bool_t bDhcp)
{
    s_bDhcp = bDhcp;
    return STATUS_OK;
}

Status_t LwIPConfigIpAddr(uint32_t ulIpAddr, uint32_t ulNetMask, uint32_t ulGwAddr)
{
    uint16_t n;
    for (n = 0; n < 4; n++) {
        s_ucIpAddr[n]   = ulIpAddr & 0xFF;
        s_ucNetMask[n]  = ulNetMask & 0xFF;
        s_ucGwAddr[n]   = ulGwAddr & 0xFF;
        ulIpAddr        >>= 8;
        ulNetMask       >>= 8;
        ulGwAddr        >>= 8;
    }
    return STATUS_OK;
}

Status_t LwIPStart(void)
{
    tcpip_init(NULL, NULL);

    if (s_bDhcp) {
        /* IP addresses initialization with DHCP (IPv4) */
        s_xIpAddr.addr  = 0;
        s_xNetMask.addr = 0;
        s_xGwAddr.addr  = 0;  
    }
    else {
        /* IP addresses initialization with local (IPv4) */
        IP4_ADDR(&s_xIpAddr, s_ucIpAddr[0], s_ucIpAddr[1], s_ucIpAddr[2], s_ucIpAddr[3]);
        IP4_ADDR(&s_xGwAddr, s_ucGwAddr[0], s_ucGwAddr[1], s_ucGwAddr[2], s_ucGwAddr[3]);
        IP4_ADDR(&s_xNetMask, s_ucNetMask[0], s_ucNetMask[1], s_ucNetMask[2], s_ucNetMask[3]);
    }

    /* Add the network interface */
    netif_add(&s_xNetIf, &s_xIpAddr, &s_xNetMask, &s_xGwAddr, NULL, &ethernetif_init, &tcpip_input);

    /* Registers the default network interface */
    netif_set_default(&s_xNetIf);

    if (netif_is_link_up(&s_xNetIf)) {
        /* When the netif is fully configured this function must be called */
        netif_set_up(&s_xNetIf);
    }
    else {
        /* When the netif link is down this function must be called */
        netif_set_down(&s_xNetIf);
    }
    
    if (s_bDhcp) {
        /* Start DHCP negotiation for a network interface (IPv4) */
        dhcp_start(&s_xNetIf);
    }
    
    return STATUS_OK;
}

void* LwIPGetNetIf(void)
{
    return &s_xNetIf;
}

#endif /* (NET_ENABLE && NET_ENABLE_LWIP) */
