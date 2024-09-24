/*
    LwIPOpts.h

    Head File for Custom LwIP Option Module
*/

/* Copyright 2019 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 26Sep18, Karl Created
    01b, 05Dec18, Karl Modified
    01b, 03Aug19, Karl Reconstructured Net lib
*/

#ifndef __LWIP_OPTS_H__
#define __LWIP_OPTS_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/* XXX: LwIPOpts.h is originally created by CubeMX tools! */

/* STM32CubeMX specific parameters (not defined in opt.h) */
/* Parameters set in STM32CubeMX LwIP Configuration GUI */
#define WITH_RTOS                   (1)
#define CHECKSUM_BY_HARDWARE        (0)

/* LwIP stack parameters (modified compared to initialization value in opt.h) */
/* Parameters set in STM32CubeMX LwIP Configuration GUI */
#define MEM_ALIGNMENT               (4)
#define LWIP_ETHERNET               (1)
#define LWIP_DNS                    (1)
#define TCP_QUEUE_OOSEQ             (0)
#define TCP_SND_QUEUELEN            (9)
#define TCP_SNDLOWAT                (1071)
#define TCP_SNDQUEUELOWAT           (5)
#define TCP_WND_UPDATE_THRESHOLD    (536)
#define TCPIP_THREAD_STACKSIZE      (1024)
#define TCPIP_THREAD_PRIO           (3)
#define SLIPIF_THREAD_STACKSIZE     (1024)
#define SLIPIF_THREAD_PRIO          (3)
#define DEFAULT_THREAD_STACKSIZE    (1024)
#define DEFAULT_THREAD_PRIO         (3)
#define LWIP_STATS                  (0)
#define CHECKSUM_GEN_IP             (0)
#define CHECKSUM_GEN_UDP            (0)
#define CHECKSUM_GEN_TCP            (0)
#define CHECKSUM_GEN_ICMP           (0)
#define CHECKSUM_CHECK_IP           (0)
#define CHECKSUM_CHECK_UDP          (0)
#define CHECKSUM_CHECK_TCP          (0)
 
/* XXX: LwIPOpts.h user config parameters */
/* Parameters not set in STM32CubeMX LwIP Configuration GUI */
/* LwIP Parameters not in opt.h */
#define LWIP_PROVIDE_ERRNO          (1)
#define LWIP_SO_RCVTIMEO            (1)
#define LWIP_SO_SNDTIMEO            (1)
#define LWIP_DHCP                   (1)
#define LWIP_TCP_KEEPALIVE          (1)
#define LWIP_SO_RCVBUF              (1)
#define RECV_BUFSIZE_DEFAULT        (256)
#define MEMP_NUM_TCP_PCB            (2)
#define MEMP_NUM_NETCONN            (5)
#undef  MEMP_NUM_SYS_TIMEOUT

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __LWIP_OPTS_H__ */
