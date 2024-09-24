/*
    Prot.h

    Head File for Prot Module
*/

/* Copyright 2019 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 22Nov18, Karl Created
    01b, 13Jul19, Karl Reconstructured Prot library
*/

#ifndef __PROT_H__
#define __PROT_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/* Includes */
#include <stdint.h>
#include "lib/type/lib_type.h"
#include "lib/prot/lib_prot_cfg.h"

/* Types */
typedef uint32_t prot_handle_t;

/* Packet head length type */
typedef enum {
    PROT_LENGTH_UINT8   = 1,
    PROT_LENGTH_UINT16  = 2,
    PROT_LENGTH_UINT32  = 4,
    PROT_LENGTH_TYPE_SIZE,
}prot_length_t;

/* Packet process callback function */
typedef uint8_t (*prot_chk_func_t)(const void*, uint32_t);
typedef status_t (*prot_proc_func_t)(const void*, const uint8_t*, uint32_t, void*);

/* Functions */
status_t        prot_init(void);
status_t        prot_term(void);

prot_handle_t   prot_create(void);
status_t        prot_delete(prot_handle_t xHandle);

status_t        prot_cfg_head(prot_handle_t xHandle, const uint8_t* pucHeadMark, uint8_t ucHeadMarkSize, uint8_t ucHeadSize);
status_t        prot_cfg_tail(prot_handle_t xHandle, const uint8_t* pucTailMark, uint8_t ucTailMarkSize, uint8_t ucTailSize);
status_t        prot_cfg_misc(prot_handle_t xHandle, uint32_t ulMaxMsgSize, uint32_t ulMsgLengthOffset, prot_length_t xLengthType, uint8_t ucLengthOffset);
status_t        prot_cfg_cb(prot_handle_t xHandle, prot_proc_func_t pxPktProc, prot_chk_func_t pxPktChk);
status_t        prot_cfg(prot_handle_t xHandle);

status_t        prot_proc(prot_handle_t xHandle, IN uint8_t* pucRecvBuf, IN uint16_t usRecvd, INOUT uint16_t *pusRecvIndex, INOUT uint8_t* pucProcBuf, IN void* pvPara);

#if PROT_TEST
status_t        ProtTest(void);
#endif /* PROT_TEST */

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __PROT_H__ */
