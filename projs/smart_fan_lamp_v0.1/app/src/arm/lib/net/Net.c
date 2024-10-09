/*
    Net.c

    Implementation File for Net Module
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

/* Includes */
#include <stdlib.h>
#include <string.h>
#include <cmsis_os.h>
#include <stm32f1xx_hal.h>
#include <netdb.h>
#include <dhcp.h>
#include <sockets.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include "Prot/Prot.h"
#include "Rbuf/Rbuf.h"
#include "Net/LwIP.h"
#include "Net/Net.h"

#if (NET_ENABLE && NET_ENABLE_SUITE)

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

/* Local defines */
#define ETH_GET_CTRL()             ((EthCtrl_t*)(&s_xEthCtrl))
#define NET_GET_CTRL(handle)       ((NetCtrl_t*)(handle))

/* Local types */
typedef enum {
    ETH_STATUS_PHY_DISCONNECTED,
    ETH_STATUS_PHY_CONNECTED,
    ETH_STATUS_GOT_LOCAL_IP,
    ETH_STATUS_GOT_SERVER_IP,
}EthStatus_t;

typedef enum {
    TCP_STATUS_DISCONNECTED,
    TCP_STATUS_CONNECTING,
    TCP_STATUS_CONNECTED,
}TcpStatus_t;

typedef enum {
    TCP_CONN_STATUS_SUCCESS,
    TCP_CONN_STATUS_FAIL_TIMEOUT,
    TCP_CONN_STATUS_FAIL_REFUSED,
}TcpConnStatus_t;

typedef struct {
    Bool_t          bConfig;
    EthStatus_t     xStatus;
    Bool_t          bEnDhcp;
    uint16_t        usDhcpTimeout;
    uint16_t        usDhcpTimeoutCnt;
    uint32_t        ulServerIp;
    uint16_t        usServerPort;
    uint32_t        ulLocalIp;
    uint32_t        ulLocalNetMask;
    uint32_t        ulLocalGwAddr;
    struct netif*   pxNetIf;
    char            cSvrName[NET_MAX_SVR_NAME];
    osThreadId      xEthTask;
}EthCtrl_t;

typedef struct {
    Bool_t          bConfig;
    /* Status */
    NetHandle_t     xNet;
    SOCKET          xSocket;
    Bool_t          bConnect;
    uint16_t        usSeqNum;
    TcpStatus_t     xTcpStatus;
    TcpStatus_t     xTcpLastStatus;
    EthCtrl_t       *pxEth;
    /* Server */
    uint32_t        ulServerIp;
    uint16_t        usServerPort;
    /* Callback */
    void            *pvRecvPara;
    NetRecvProcFunc_t pxNetRecvProc;
    NetEventFunc_t  pxEventFunc[NET_EVENT_NUM];
    /* Recv ctrl */
    ProtHandle_t    xProt;
    int             lRecvd;
    uint16_t        usRecvIndex;
    uint8_t         ucProcBuf[NET_RECV_BUF_SIZE];
    uint8_t         ucRecvBuf[NET_RECV_BUF_SIZE];
    /* Send */
    SemaphoreHandle_t xSendMutex;
    uint8_t         ucSendBuf[NET_SEND_BUF_SIZE];
#if NET_ENABLE_ASYNC
    RbufHandle_t    xRbuf;
    uint32_t        ulRbufSize;
    uint8_t         ucSendRBuf[NET_SEND_BUF_SIZE*NET_SEND_RBUF_CNT];
#endif /* NET_ENABLE_ASYNC */
    /* Test */
#if NET_TEST
    uint32_t        ulTotalSend;
    uint32_t        ulTotalRecv;
    uint32_t        ulSendErrCnt;
    uint32_t        ulRecvErrCnt;
#endif /* NET_TEST */
    osThreadId      xNetTask;
}NetCtrl_t;

/* Forward declaration */
static TcpConnStatus_t  prvTcpConnect   (NetCtrl_t *pxCtrl);
static Status_t         prvEthChk       (EthCtrl_t *pxCtrl);
static Status_t         prvNetChk       (NetCtrl_t *pxCtrl);
static void             prvTaskEth      (void const *pvPara);
static void             prvTaskNet      (void const *pvPara);
static Status_t         prvShowIp       (uint32_t ulIP);
static Status_t         prvShowDhcpIp   (struct netif* pxNetIf);
static Status_t         prvShowStaticIp (struct netif* pxNetIf);
static Status_t         prvRstStaticIp  (struct netif* pxNetIf, uint32_t ulLocalIp, uint32_t ulLocalNetMask, uint32_t ulLocalGwAddr);
extern ETH_HandleTypeDef heth;

/* Local variables */
static EthCtrl_t        s_xEthCtrl;

/* Functions */
Status_t NetInit(void)
{
    EthCtrl_t *pxEthCtrl = ETH_GET_CTRL();
    
    ASSERT(NULL != pxEthCtrl);
    /* LwIP init */
    LwIPInit();
    LwIPConfigDhcp(pxEthCtrl->bEnDhcp);
    if (!pxEthCtrl->bEnDhcp) {
        LwIPConfigIpAddr(pxEthCtrl->ulLocalIp, pxEthCtrl->ulLocalNetMask, pxEthCtrl->ulLocalGwAddr);
    }
    LwIPStart();
    
    /* Create Eth monitor task */
    osThreadDef(prvTaskEth, prvTaskEth, ETH_TASK_PRIORITY, 0, ETH_TASK_STACK_SIZE);
    pxEthCtrl->xEthTask = osThreadCreate(osThread(prvTaskEth), (void*)pxEthCtrl);
    if (NULL == pxEthCtrl->xEthTask) {
        TRACE("NetInit: create prvTaskEth task failed\n");
        return STATUS_ERR;
    }
    
    return STATUS_OK;
}

Status_t NetTerm(void)
{
    /* Do nothing */
    return STATUS_OK;
}

NetHandle_t NetCreate(void)
{
    NetCtrl_t *pxCtrl = NULL;
    pxCtrl = (NetCtrl_t*)malloc(sizeof(NetCtrl_t));
    memset(pxCtrl, 0, sizeof(NetCtrl_t));
    return (NetHandle_t)pxCtrl;
}

Status_t NetDelete(NetHandle_t xHandle)
{
    if(xHandle) {
        /* TODO: Clear up */
        free((void*)xHandle);
    }
    return STATUS_OK;
}

Status_t NetConfigEth(NET_CONFIG_ETH_PARA)
{
    EthCtrl_t *pxCtrl = ETH_GET_CTRL();
    pxCtrl->bConfig           = TRUE;
    pxCtrl->xStatus           = ETH_STATUS_PHY_DISCONNECTED;
    pxCtrl->bEnDhcp           = bDhcp;
    pxCtrl->usDhcpTimeout     = usDhcpTimeout;
    pxCtrl->usDhcpTimeoutCnt  = 0;
    pxCtrl->ulServerIp        = 0;
    pxCtrl->usServerPort      = usServerPort;
    pxCtrl->ulLocalIp         = ulLocalIp;        /* Only valid when DHCP is disabled  */
    pxCtrl->ulLocalNetMask    = ulLocalNetMask;   /* Only valid when DHCP is disabled  */
    pxCtrl->ulLocalGwAddr     = ulLocalGwAddr;    /* Only valid when DHCP is disabled  */
    pxCtrl->pxNetIf           = pxNetIf;
    ASSERT(strlen(cSvrName) < NET_MAX_SVR_NAME);
    strcpy(pxCtrl->cSvrName, cSvrName);
    return STATUS_OK;
}

Status_t  NetConfigTcpClient(NetHandle_t xHandle, ProtHandle_t xProt, NetRecvProcFunc_t pxNetRecvProc, void* pvPara)
{
    NetCtrl_t *pxCtrl = NET_GET_CTRL(xHandle);
    
    ASSERT(NULL != pxCtrl);
    pxCtrl->bConfig         = TRUE;
    pxCtrl->xNet            = xHandle;
    pxCtrl->xSocket         = INVALID_SOCKET;
    pxCtrl->bConnect        = FALSE;
    pxCtrl->usSeqNum        = 0;
    pxCtrl->xTcpStatus      = TCP_STATUS_DISCONNECTED;
    pxCtrl->xTcpLastStatus  = TCP_STATUS_DISCONNECTED;
    pxCtrl->pxEth           = ETH_GET_CTRL();
    /* Server */
    pxCtrl->ulServerIp      = 0;    /* Get the para from eth ctrl */
    pxCtrl->usServerPort    = 0;    /* Get the para from eth ctrl */
    /* Recv & proc */
    pxCtrl->pvRecvPara      = pvPara;
    pxCtrl->pxNetRecvProc   = pxNetRecvProc;
    /* Recv ctrl */
    pxCtrl->xProt           = xProt;
    pxCtrl->lRecvd          = 0;
    pxCtrl->usRecvIndex     = 0;
    memset(pxCtrl->ucProcBuf, 0, NET_RECV_BUF_SIZE);
    memset(pxCtrl->ucRecvBuf, 0, NET_RECV_BUF_SIZE);
    /* Send */
    pxCtrl->xSendMutex      = xSemaphoreCreateRecursiveMutex();
    memset(pxCtrl->ucSendBuf, 0, NET_SEND_BUF_SIZE);
#if NET_ENABLE_ASYNC
    pxCtrl->xRbuf           = RbufCreate();
    pxCtrl->ulRbufSize      = NET_SEND_BUF_SIZE*NET_SEND_RBUF_CNT;
    memset(pxCtrl->ucSendRBuf, 0, NET_SEND_BUF_SIZE*NET_SEND_RBUF_CNT);
#endif /* NET_ENABLE_ASYNC */
#if NET_TEST
    pxCtrl->ulTotalSend     = 0;
    pxCtrl->ulTotalRecv     = 0;
    pxCtrl->ulSendErrCnt    = 0;
    pxCtrl->ulRecvErrCnt    = 0;
#endif /* NET_TEST */
    
    if (NULL == pxCtrl->xSendMutex) {
        TRACE("NetConfigTcp create SendMutex failed\n");
        return STATUS_ERR;
    }
    
#if NET_ENABLE_ASYNC
    /* Rbuf Config */
    RbufConfig(pxCtrl->xRbuf, pxCtrl->ucSendRBuf, pxCtrl->ulRbufSize, 50, 100);
#endif /* NET_ENABLE_ASYNC */
    
    return STATUS_OK;
}

Status_t NetConfigTcpEvent(NetHandle_t xHandle, NetEventType_t xEvType, NetEventFunc_t pxEventFunc)
{
    NetCtrl_t *pxCtrl = NET_GET_CTRL(xHandle);
    ASSERT(NULL != pxCtrl);
    pxCtrl->pxEventFunc[xEvType] = pxEventFunc;
    return STATUS_OK;
}

Status_t NetStart(NetHandle_t xHandle)
{
    NetCtrl_t *pxCtrl = NET_GET_CTRL(xHandle);
    
    ASSERT(NULL != pxCtrl);
    /* Create tcp client monitor task */    
    if (pxCtrl->bConfig) {
        osThreadDef(prvTaskNet, prvTaskNet, NET_TASK_PRIORITY, 0, NET_TASK_STACK_SIZE);
        pxCtrl->xNetTask = osThreadCreate(osThread(prvTaskNet), (void*)pxCtrl);
        if (NULL == pxCtrl->xNetTask) {
            TRACE("NetStart: create prvTaskNet task failed\n");
            return STATUS_ERR;
        }
        return STATUS_OK;
    }
    else {
        TRACE("NetStart: module hasn't been inited\n");
        return STATUS_ERR;
    }
}

Status_t NetSendBlk(NetHandle_t xHandle, uint8_t* pucBuffer, uint16_t usSize)
{
    uint16_t usSent = 0;
    uint16_t usLeft = usSize;
    NetCtrl_t *pxCtrl = NET_GET_CTRL(xHandle);
    
    ASSERT(NULL != pxCtrl);
    if (!pxCtrl->bConnect) {
        TRACE("NetSendBlk error: connection hasn't been established!\n");
        return STATUS_ERR;
    }
    
    xSemaphoreTakeRecursive(pxCtrl->xSendMutex, portMAX_DELAY);
    
    while (usSent < usSize) {
        int lRet = 0;
        int lErr = 0;
        uint16_t usCnt = 0;
        
        usCnt++;
        if (usCnt > 1000) {
            TRACE("NetSendBlk error: try timeout!\n");
            break;
        }
        socklen_t xOptLen = sizeof(lErr);
        lRet = send(pxCtrl->xSocket, pucBuffer + usSent, usLeft, 0);
        getsockopt(pxCtrl->xSocket, SOL_SOCKET, SO_ERROR, &lErr, &xOptLen);
        if ((-1 == lRet) && (ERR_WOULDBLOCK != lErr) && (EAGAIN != lErr)) {
            close(pxCtrl->xSocket);
            pxCtrl->bConnect       = FALSE;
            pxCtrl->xTcpLastStatus = pxCtrl->xTcpStatus;
            pxCtrl->xTcpStatus     = TCP_STATUS_DISCONNECTED;
            pxCtrl->xSocket        = INVALID_SOCKET;
            TRACE("NetSendBlk error: send failed with errno(%d)!\n", lErr);
            xSemaphoreGiveRecursive(pxCtrl->xSendMutex);
            return STATUS_ERR;
        }
        if (-1 != lRet) {
            usSent += lRet;
            usLeft -= lRet;
        }
    }
    
    if (usSent == usSize) {
    #if NET_TEST
        pxCtrl->ulTotalSend += usSent;
    #endif /* NET_TEST */
        xSemaphoreGiveRecursive(pxCtrl->xSendMutex);
        return STATUS_OK;
    }
    else {
    #if NET_TEST
        pxCtrl->ulSendErrCnt = 0;
    #endif /* NET_TEST */
        xSemaphoreGiveRecursive(pxCtrl->xSendMutex);
        TRACE("NetSendBlk error: length that has been sent is not equal to size!\n");
        return STATUS_ERR;
    }
}

#if NET_ENABLE_ASYNC
Status_t NetSendAsync(NetHandle_t xHandle, uint8_t* pucBuffer, uint16_t usSize)
{
    NetCtrl_t *pxCtrl = NET_GET_CTRL(xHandle);
    ASSERT(NULL != pxCtrl);
    if (!pxCtrl->bConnect) {
        TRACE("NetSendAsync error: connection hasn't been established!\n");
        return STATUS_ERR;
    }
    return ((usSize == RbufWrite(pxCtrl->xRbuf, (const void*)pucBuffer, (uint32_t)usSize)) ? STATUS_OK : STATUS_ERR);
}
#endif /* NET_ENABLE_ASYNC */

static TcpConnStatus_t prvTcpConnect(NetCtrl_t *pxCtrl)
{
    ASSERT(NULL != pxCtrl);
    if (!pxCtrl->bConnect) {
        socklen_t xOptLen;
        struct timeval xTm;
        struct sockaddr_in xServerAddr;
        int lConnRet, lKeepAlive, lKeepIdle, lKeepInterval, lKeepCount;
        unsigned long bNBlock;  /* TODO: check me ULONG */

        /* Create socket */
        pxCtrl->xSocket = socket(AF_INET, SOCK_STREAM, 0);
        /* Recv timeout */
        xTm.tv_sec = 1; 
        xTm.tv_usec = 0;
        xOptLen = sizeof(struct timeval);
        setsockopt(pxCtrl->xSocket, SOL_SOCKET, SO_RCVTIMEO, &xTm, xOptLen);
        /* Send timeout */
        xTm.tv_sec = 1;
        xTm.tv_usec = 0;
        xOptLen = sizeof(struct timeval);
        setsockopt(pxCtrl->xSocket, SOL_SOCKET, SO_SNDTIMEO, &xTm, xOptLen);
        /* Conn timeout(XXX: Not implemented in LwIP) */
        xTm.tv_sec = NET_CONN_TIMEOUT;
        xTm.tv_usec = 0;
        xOptLen = sizeof(struct timeval);
        setsockopt(pxCtrl->xSocket, SOL_SOCKET, SO_CONTIMEO, &xTm, xOptLen);
        /* Enable keepalive */
        lKeepAlive = 1;
        setsockopt(pxCtrl->xSocket, SOL_SOCKET, SO_KEEPALIVE, (void*)&lKeepAlive, sizeof(lKeepAlive));
        /* Probe will be enabled when without data traffic within 90s */
        lKeepIdle = 90;
        setsockopt(pxCtrl->xSocket, IPPROTO_TCP, TCP_KEEPIDLE, (void*)&lKeepIdle, sizeof(lKeepIdle));
        /* Probe packet interval is 5s */
        lKeepInterval = 5;
        setsockopt(pxCtrl->xSocket, IPPROTO_TCP, TCP_KEEPINTVL, (void*)&lKeepInterval, sizeof(lKeepInterval));
        /* Probe packet transmission is 3 */
        lKeepCount = 3;
        setsockopt(pxCtrl->xSocket, IPPROTO_TCP, TCP_KEEPCNT, (void *)&lKeepCount, sizeof(lKeepCount));
        /* Set socket blocking */
        bNBlock = FALSE;
        ioctlsocket(pxCtrl->xSocket, FIONBIO, (void*)&bNBlock);
        /* Init ip addr struct */
        memset(&xServerAddr, 0, sizeof(xServerAddr));
        xServerAddr.sin_family      = AF_INET;
        xServerAddr.sin_addr.s_addr = pxCtrl->ulServerIp;
        xServerAddr.sin_port        = htons(pxCtrl->usServerPort);
        /* Connect the server */
        lConnRet = connect(pxCtrl->xSocket, (struct sockaddr *)&xServerAddr, sizeof(struct sockaddr));
        /* Connection succeed? */
        if (0 == lConnRet) {
            bNBlock = TRUE; /* Set the tcp to non-blocking mode! */
            ioctlsocket(pxCtrl->xSocket, FIONBIO, (void*)&bNBlock);
            pxCtrl->bConnect = TRUE;
            return TCP_CONN_STATUS_SUCCESS;
        }
        else {
            return TCP_CONN_STATUS_FAIL_TIMEOUT;
        }
    }
    
    return TCP_CONN_STATUS_SUCCESS;
}

static Status_t prvEthChk(EthCtrl_t *pxCtrl)
{
    uint32_t ulRegValue;
    
    ASSERT(pxCtrl != NULL);
    /* Check PHY status */
    HAL_ETH_ReadPHYRegister(&heth, PHY_BSR, &ulRegValue);
    if ((ulRegValue & PHY_LINKED_STATUS) != PHY_LINKED_STATUS) {
        netif_set_down(pxCtrl->pxNetIf);
        pxCtrl->xStatus          = ETH_STATUS_PHY_DISCONNECTED;
        pxCtrl->usDhcpTimeoutCnt = 0;
        pxCtrl->ulServerIp       = 0;
        TRACE("prvNetChk: PHY unlink\n");
        return STATUS_OK;
    }
    
    /* Eth status machine */
    switch(pxCtrl->xStatus)
    {
        case ETH_STATUS_PHY_DISCONNECTED:
        {
            netif_set_up(pxCtrl->pxNetIf);
            TRACE("prvEthChk - ETH_STATUS_PHY_DISCONNECTED: PHY linked\n");
            pxCtrl->xStatus = ETH_STATUS_PHY_CONNECTED;
            if (pxCtrl->bEnDhcp) {
                dhcp_stop(pxCtrl->pxNetIf);
                dhcp_start(pxCtrl->pxNetIf);
            }
            /* Execute the PHY_LINKED code directly! */
        }
        case ETH_STATUS_PHY_CONNECTED:
        {
            if (pxCtrl->bEnDhcp) {
                /* DHCP */
                if (pxCtrl->usDhcpTimeoutCnt < pxCtrl->usDhcpTimeout) {
                    /* Get local IP through DHCP */
                    if (pxCtrl->pxNetIf->dhcp->offered_ip_addr.addr == 0) {
                        pxCtrl->usDhcpTimeoutCnt++;
                        TRACE("prvEthChk - ETH_STATUS_PHY_CONNECTED: try to get local IP through dhcp(%d)\n", pxCtrl->usDhcpTimeoutCnt);
                        break;
                    }
                    else {
                        TRACE("prvEthChk - ETH_STATUS_PHY_CONNECTED: get local IP through dhcp\n");
                        prvShowDhcpIp(pxCtrl->pxNetIf);
                    }
                }
                else {
                    TRACE("prvEthChk - ETH_STATUS_PHY_CONNECTED: get host IP through dhcp timeout\n");
                    /* Release DHCP resource */ 
                    dhcp_stop(pxCtrl->pxNetIf);
                    prvRstStaticIp(pxCtrl->pxNetIf, pxCtrl->ulLocalIp, pxCtrl->ulLocalNetMask, pxCtrl->ulLocalGwAddr);
                    prvShowStaticIp(pxCtrl->pxNetIf);
                }
            }
            else {
                /* Static allocated */
                prvShowStaticIp(pxCtrl->pxNetIf);
                TRACE("prvEthChk: get local IP statically\n");
            }
            pxCtrl->xStatus = ETH_STATUS_GOT_LOCAL_IP;
            /* Execute the ETH_STATUS_GOT_LOCAL_IP code directly! */
        }
        case ETH_STATUS_GOT_LOCAL_IP:
        {
            struct hostent *pxHostent = NULL;
            /* gethostbyname will block 7 sec if parse failed, modify DNS_TMR_INTERVAL from 143 to 1 */
            pxHostent = gethostbyname((char*)pxCtrl->cSvrName);
            if ((NULL == pxHostent->h_addr_list[0]) || (AF_INET != pxHostent->h_addrtype)) {
                TRACE("prvNetChk: try to get server IP through gethostbyname\n");
                break;
            }
            else {
                /* Print server IP address */
                pxCtrl->xStatus    = ETH_STATUS_GOT_SERVER_IP;
                pxCtrl->ulServerIp = *(uint32_t *)pxHostent->h_addr_list[0];
                TRACE("prvNetChk: get server IP\n");
                prvShowIp(pxCtrl->ulServerIp);
                break;
            }
        }
        case ETH_STATUS_GOT_SERVER_IP:
        {
            /* Do nothing */
            break;
        }
    }

    return STATUS_OK;
}

static Status_t prvNetChk(NetCtrl_t *pxCtrl)
{
    ASSERT(NULL != pxCtrl);
    ASSERT(NULL != pxCtrl->pxEth);
    
    xSemaphoreTakeRecursive(pxCtrl->xSendMutex, portMAX_DELAY);
    
    /* Check PHY status */
    if (ETH_STATUS_PHY_DISCONNECTED == pxCtrl->pxEth->xStatus) {
        pxCtrl->bConnect       = FALSE;
        pxCtrl->xTcpLastStatus = pxCtrl->xTcpStatus;
        pxCtrl->xTcpStatus     = TCP_STATUS_DISCONNECTED;
        if (INVALID_SOCKET != pxCtrl->xSocket) {
            close(pxCtrl->xSocket);
            pxCtrl->xSocket = INVALID_SOCKET;
        }
    }
    
    /* Net status machine */
    if (ETH_STATUS_GOT_SERVER_IP == pxCtrl->pxEth->xStatus) {
        switch(pxCtrl->xTcpStatus)
        {
            case TCP_STATUS_DISCONNECTED:   
            {
                /* Execute the disconnected event callback function */
                if (TCP_STATUS_CONNECTED == pxCtrl->xTcpLastStatus) {
                    if (pxCtrl->pxEventFunc[NET_EVENT_TCP_DISCONNECTED]) {
                            (pxCtrl->pxEventFunc[NET_EVENT_TCP_DISCONNECTED])(pxCtrl->xNet);
                        }
                        TRACE("prvNetChk - TCP_STATUS_DISCONNECTED: tcp disconnect\n");
                }
                pxCtrl->bConnect       = FALSE;
                pxCtrl->xTcpLastStatus = pxCtrl->xTcpStatus;
                pxCtrl->xTcpStatus     = TCP_STATUS_CONNECTING;
                if (INVALID_SOCKET != pxCtrl->xSocket) {
                    close(pxCtrl->xSocket);
                    pxCtrl->xSocket = INVALID_SOCKET;
                }
                /* Execute the TCP_STATUS_CONNECTING code directly! */
            }
            case TCP_STATUS_CONNECTING:
            {
                pxCtrl->ulServerIp   = pxCtrl->pxEth->ulServerIp;
                pxCtrl->usServerPort = pxCtrl->pxEth->usServerPort;
                switch(prvTcpConnect(pxCtrl))
                {
                    case TCP_CONN_STATUS_SUCCESS:
                        pxCtrl->bConnect       = TRUE;
                        pxCtrl->xTcpLastStatus = pxCtrl->xTcpStatus;
                        pxCtrl->xTcpStatus     = TCP_STATUS_CONNECTED;
                        /* Execute the connected event callback function */
                        if (pxCtrl->pxEventFunc[NET_EVENT_TCP_CONNECTED]) {
                            (pxCtrl->pxEventFunc[NET_EVENT_TCP_CONNECTED])(pxCtrl->xNet);
                        }
                        TRACE("prvNetChk - TCP_STATUS_CONNECTING: tcp connect succeed\n");
                        break;
                    case TCP_CONN_STATUS_FAIL_TIMEOUT:
                        pxCtrl->bConnect       = FALSE;
                        pxCtrl->xTcpLastStatus = pxCtrl->xTcpStatus;
                        pxCtrl->xTcpStatus     = TCP_STATUS_DISCONNECTED;
                        TRACE("prvNetChk - TCP_STATUS_CONNECTING: tcp connect timeout\n");
                        break;
                    case TCP_CONN_STATUS_FAIL_REFUSED:
                        pxCtrl->bConnect       = FALSE;
                        pxCtrl->xTcpLastStatus = pxCtrl->xTcpStatus;
                        pxCtrl->xTcpStatus     = TCP_STATUS_DISCONNECTED;
                        TRACE("prvNetChk - TCP_STATUS_CONNECTING: tcp connect refused\n");
                        break;
                }
                break;
            }
            case TCP_STATUS_CONNECTED:
            {
                /* Check connection status */
                if (!pxCtrl->bConnect) {
                    TRACE("prvNetChk - TCP_STATUS_CONNECTED: connection hasn't been established!\n");
                    pxCtrl->xTcpLastStatus = pxCtrl->xTcpStatus;
                    pxCtrl->xTcpStatus     = TCP_STATUS_DISCONNECTED;
                    break;
                }
                /* Read net data */
                {
                    uint16_t usNRead;
                    /* Get the data to be read */
                    ioctlsocket(pxCtrl->xSocket, FIONREAD, (void*)(&usNRead));
                    if (usNRead > NET_RECV_BUF_SIZE) {
                        usNRead = NET_RECV_BUF_SIZE;
                    }
                    {
                        int lErr = 0;
                        socklen_t xOptLen = sizeof(int);
                        /* Recv the socket data, the socket should set recv timeout first */
                        pxCtrl->lRecvd = recv(pxCtrl->xSocket, pxCtrl->ucRecvBuf, usNRead, 0);
                        getsockopt(pxCtrl->xSocket, SOL_SOCKET, SO_ERROR, &lErr, &xOptLen);
                        if ((-1 == pxCtrl->lRecvd) && (ERR_WOULDBLOCK != lErr) && (EAGAIN != lErr)) {
                        #if NET_TEST
                            pxCtrl->ulRecvErrCnt++;
                        #endif /* NET_TEST */
                            close(pxCtrl->xSocket);
                            pxCtrl->xSocket = INVALID_SOCKET;
                            TRACE("prvNetChk - TCP_STATUS_CONNECTED: recv error and close the socket!\n");
                            pxCtrl->xTcpLastStatus = pxCtrl->xTcpStatus;
                            pxCtrl->xTcpStatus     = TCP_STATUS_DISCONNECTED;
                            break;
                        }
                        /* Connection has been closed by the remote */
                        if (0 == pxCtrl->lRecvd) {
                            pxCtrl->bConnect    = FALSE;
                            pxCtrl->usRecvIndex = 0;
                            TRACE("prvNetChk - TCP_STATUS_CONNECTED: connection has been closed by the remote terminal peacefully!\n");
                            pxCtrl->xTcpLastStatus = pxCtrl->xTcpStatus;
                            pxCtrl->xTcpStatus     = TCP_STATUS_DISCONNECTED;
                            break;
                        }
                        
                        if (-1 != pxCtrl->lRecvd) {
                        #if NET_TEST
                            pxCtrl->ulTotalRecv += pxCtrl->lRecvd;
                        #endif /* NET_TEST */
                            TRACE("prvNetChk - TCP_STATUS_CONNECTED: handle-%08x recv %d bytes\n", pxCtrl->xNet, pxCtrl->lRecvd);
                            /* Use PROT utility to parse the data */
                            if (pxCtrl->xProt) {
                                ProtProc(pxCtrl->xProt, pxCtrl->ucRecvBuf, pxCtrl->lRecvd, &(pxCtrl->usRecvIndex), pxCtrl->ucProcBuf, (void*)pxCtrl->xNet);
                            }
                            /* Recv data callback */
                            if (pxCtrl->pxNetRecvProc) {
                                (pxCtrl->pxNetRecvProc)(pxCtrl->xNet, pxCtrl->ucRecvBuf, pxCtrl->lRecvd, pxCtrl->pvRecvPara);
                            }
                        }
                    }
                }
            #if NET_ENABLE_ASYNC
                /* Send net data */
                {
                    uint32_t ulNRead = 0;
                    uint32_t ulRbufRead = 0;
                    ulNRead = RbufGetFull(pxCtrl->xRbuf);
                    if (ulNRead > NET_SEND_BUF_SIZE) {
                        ulNRead = NET_SEND_BUF_SIZE;
                    }
                    /* There is sth need to be sent */
                    if (ulNRead) {
                        ulRbufRead = RbufReadDirect(pxCtrl->xRbuf, pxCtrl->ucSendBuf, ulNRead);
                        ASSERT(ulRbufRead <= NET_SEND_BUF_SIZE);
                        if (STATUS_OK == NetSendBlk(pxCtrl->xNet, pxCtrl->ucSendBuf, ulRbufRead)) {
                            TRACE("prvNetChk - TCP_STATUS_CONNECTED: send %d bytes\n", ulRbufRead);
                        }
                        else {
                            TRACE("prvNetChk - TCP_STATUS_CONNECTED: send failed\n");
                        }
                    }
                }
            #endif /* NET_ENABLE_ASYNC */
                break;
            }
        }
    }

    xSemaphoreGiveRecursive(pxCtrl->xSendMutex);
    return STATUS_OK;
}

static void prvTaskEth(void const *pvPara)
{
    ASSERT(NULL != pvPara);
    while (1) {
        prvEthChk((EthCtrl_t*)pvPara);
        osDelay(ETH_TASK_DELAY);
    }
    /* We should never get here! */
}

static void prvTaskNet(void const *pvPara)
{
    ASSERT(NULL != pvPara);
    while (1) {
        prvNetChk((NetCtrl_t*)pvPara);
        osDelay(NET_TASK_DELAY);
    }
    /* We should never get here! */
}

static Status_t prvShowIp(uint32_t ulIP)
{
    TRACE("IP         :%d.%d.%d.%d\n", ulIP&0xFF, (ulIP>>8)&0xFF, (ulIP>>16)&0xFF, (ulIP>>24)&0xFF);
    return STATUS_OK;
}

static Status_t prvShowDhcpIp(struct netif* pxNetIf)
{
    ASSERT(NULL != pxNetIf);
    TRACE("MAC        :%02X-%02X-%02X-%02X-%02X-%02X\n", pxNetIf->hwaddr[0], pxNetIf->hwaddr[1], pxNetIf->hwaddr[2], pxNetIf->hwaddr[3], pxNetIf->hwaddr[4], pxNetIf->hwaddr[5]);
    TRACE("IP         :%s\n", ipaddr_ntoa(&(pxNetIf->dhcp->offered_ip_addr)));
    TRACE("Netmask    :%s\n", ipaddr_ntoa(&(pxNetIf->dhcp->offered_sn_mask)));
    TRACE("Gateway    :%s\n", ipaddr_ntoa(&(pxNetIf->dhcp->offered_gw_addr)));
    TRACE("DHCP       :%s\n", ipaddr_ntoa(&(pxNetIf->dhcp->server_ip_addr)));
    return STATUS_OK;
}

static Status_t prvShowStaticIp(struct netif* pxNetIf)
{
    ASSERT(NULL != pxNetIf);
    TRACE("MAC        :%02X-%02X-%02X-%02X-%02X-%02X\n", pxNetIf->hwaddr[0], pxNetIf->hwaddr[1], pxNetIf->hwaddr[2], pxNetIf->hwaddr[3], pxNetIf->hwaddr[4], pxNetIf->hwaddr[5]);
    TRACE("IP         :%s\n", ipaddr_ntoa(&(pxNetIf->ip_addr)));
    TRACE("Netmask    :%s\n", ipaddr_ntoa(&(pxNetIf->netmask)));
    TRACE("Gateway    :%s\n", ipaddr_ntoa(&(pxNetIf->gw)));
    return STATUS_OK;
}

static Status_t prvRstStaticIp(struct netif* pxNetIf, uint32_t ulLocalIp, uint32_t ulLocalNetMask, uint32_t ulLocalGwAddr)
{
    netif_set_addr(pxNetIf, (ip_addr_t*)&ulLocalIp, (ip_addr_t*)&ulLocalNetMask, (ip_addr_t*)&ulLocalGwAddr);   
    return STATUS_OK;
}

#endif /* (NET_ENABLE && NET_ENABLE_SUITE) */
