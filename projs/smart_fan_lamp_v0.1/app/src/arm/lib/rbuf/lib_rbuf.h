/*
    Rbuf.h

    Head File for Rbuf Module
*/

/* Copyright 2018 Shanghai BiNY Inc. */

/*
    modification history
    --------------------
    01a, 28Nov18, Karl Created
    01b, 04Dec18, Karl Added RbufReadDirect
    01c, 12Jul19, Karl Added rbuf_create and RbufDestroy
    01d, 12Jul19, Karl Reconstructured Rbuf library
*/

#ifndef __LIB_RBUF_H__
#define __LIB_RBUF_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/* Includes */
#include <stdint.h>
#include "lib/type/lib_type.h"
#include "lib/rbuf/lib_rbuf_cfg.h"

/* Types */
typedef uint32_t rbuf_handle_t;

typedef enum {
    RBUF_STATUS_OK,
    RBUF_STATUS_ERR,
    RBUF_STATUS_ERR_INIT,
    RBUF_STATUS_ERR_REINIT,
    RBUF_STATUS_ERR_UNINIT,
    RBUF_STATUS_ERR_MSGQ,
}rbuf_status_t;

/* Functions */
rbuf_status_t    rbuf_init(void);
rbuf_status_t    rbuf_term(void);

rbuf_handle_t    rbuf_create(void);
rbuf_status_t    rbuf_delete(rbuf_handle_t xHandle);
rbuf_status_t    rbuf_cfg(rbuf_handle_t xHandle, void* pvBuffer, uint32_t ulBufSize, uint32_t ulMsgQueueSize, uint32_t ulMsgQueueWaitMs);

uint32_t        RbufWrite(rbuf_handle_t xHandle, const void* pvData, uint32_t ulCount);
uint32_t        RbufRead(rbuf_handle_t xHandle, void* pvData, uint32_t ulCount);
uint32_t        RbufReadDirect(rbuf_handle_t xHandle, void* pvData, uint32_t ulCount);
uint32_t        RbufGetFree(rbuf_handle_t xHandle);
uint32_t        RbufGetFull(rbuf_handle_t xHandle);
rbuf_status_t    RbufShowStatus(rbuf_handle_t xHandle);

#if RBUF_TEST
Status_t        RbufTest(void);
#endif /* RBUF_TEST */

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __LIB_RBUF_H__ */
