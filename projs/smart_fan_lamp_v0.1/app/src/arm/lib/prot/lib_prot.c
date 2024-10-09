/*
    lib_prot.c

    Implementation File for Prot Module
*/

/* Copyright 2019 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 22Nov18, Karl Created
    01b, 13Jul19, Karl Reconstructured Prot library
    01c, 24Sep24, Jasper, Rename functions from camel case to snake case.
*/

/* Includes */
#include <stdlib.h>
#include <string.h>
#include "lib/prot/lib_prot.h"
#include "lib/debug/lib_debug.h"

#if PROT_ENABLE

/* Pragmas */
#pragma diag_suppress 111 /* warning: #111-D: statement is unreachable */

/* Debug config */
#if PROT_DEBUG
#undef TRACE
#define TRACE(...) debug_printf(__VA_ARGS__)
#else
#undef TRACE
#define TRACE(...)
#endif /* PROT_DEBUG */
#if PROT_ASSERT
#undef ASSERT
#define ASSERT(a)                                                                                                      \
    while (!(a)) {                                                                                                     \
        debug_printf("ASSERT failed: %s %d\n", __FILE__, __LINE__);                                                     \
    }
#else
#undef ASSERT
#define ASSERT(...)
#endif /* PROT_ASSERT */

/* Local defines */
#define PROT_GET_CTRL(handle) ((prot_ctrl_t*)(handle))
#define PROT_GET_LENGTH(type) ((uint16_t)(type))

/* Local types */
typedef struct {
    Bool_t init; /* Module init mark */

    uint8_t init_head;                     /* Head init mark */
    uint8_t head_mark_size;                /* Head mark size */
    uint8_t head_size;                     /* Total head size in byte */
    uint8_t head_mark[PROT_MAX_MARK_SIZE]; /* Head mark */

    Bool_t init_tail;                      /* Tail init mark */
    uint8_t tail_mark_size;                /* Tail mark size */
    uint8_t tail_size;                     /* Total tail size in byte */
    uint8_t tail_mark[PROT_MAX_MARK_SIZE]; /* Tail mark */

    uint8_t head_tail_size; /* Head tail size in byte */

    Bool_t init_misc;           /* Misc init mark */
    uint32_t max_msg_size;      /* Max message size in byte */
    prot_length_t length_type;  /* Length field type */
    uint8_t length_offset;      /* Length field offset */
    uint32_t msg_length_offset; /* Msg length offset */

    Bool_t init_cb;            /* Callback function init mark */
    prot_proc_func_t pkt_proc; /* Packet process callback function */
    prot_chk_func_t pkt_chk;   /* Packet check callback function */

#if PROT_TEST
    uint32_t rx_pkt_num;  /* Total recvd packet number */
    uint32_t chk_err_num; /* Check code error number */
    uint32_t len_err_num; /* Length or tail code error number */
#endif                    /* PROT_TEST */
} prot_ctrl_t;

/* Forward declaration */
static uint32_t prv_get_length(IN uint8_t* pucBuf, prot_length_t length_type, uint8_t length_offset);

/* Functions */
status_t
prot_init(void) {
    /* Do nothing */
    return status_ok;
}

status_t
prot_term(void) {
    /* Do nothing */
    return status_ok;
}

prot_handle_t
prot_create(void) {
    prot_ctrl_t* pxCtrl = NULL;
    pxCtrl = (prot_ctrl_t*)malloc(sizeof(prot_ctrl_t));
    memset(pxCtrl, 0, sizeof(prot_ctrl_t));
    return (prot_handle_t)pxCtrl;
}

status_t
prot_delete(prot_handle_t xHandle) {
    if (xHandle) {
        free((void*)xHandle);
    }
    return status_ok;
}

status_t
prot_cfg_head(prot_handle_t xHandle, const uint8_t* head_mark, uint8_t head_mark_size, uint8_t head_size) {
    prot_ctrl_t* pxCtrl = PROT_GET_CTRL(xHandle);

    ASSERT(NULL != pxCtrl);
    ASSERT(head_mark_size <= PROT_MAX_MARK_SIZE);
    memcpy(pxCtrl->head_mark, head_mark, head_mark_size);
    pxCtrl->head_mark_size = head_mark_size;
    pxCtrl->head_size = head_size;
    pxCtrl->init_head = TRUE;

    return status_ok;
}

status_t
prot_cfg_tail(prot_handle_t xHandle, const uint8_t* ptail_mark, uint8_t tail_mark_size, uint8_t tail_size) {
    prot_ctrl_t* pxCtrl = PROT_GET_CTRL(xHandle);

    ASSERT(NULL != pxCtrl);
    ASSERT(tail_mark_size <= PROT_MAX_MARK_SIZE);
    memcpy(pxCtrl->tail_mark, ptail_mark, tail_mark_size);
    pxCtrl->tail_mark_size = tail_mark_size;
    pxCtrl->tail_size = tail_size;
    pxCtrl->init_tail = TRUE;

    return status_ok;
}

status_t
prot_cfg_misc(prot_handle_t xHandle, uint32_t max_msg_size, uint32_t msg_length_offset, prot_length_t length_type,
              uint8_t length_offset) {
    prot_ctrl_t* pxCtrl = PROT_GET_CTRL(xHandle);

    ASSERT(NULL != pxCtrl);
    pxCtrl->max_msg_size = max_msg_size;
    pxCtrl->length_type = length_type;
    pxCtrl->length_offset = length_offset;
    pxCtrl->msg_length_offset = msg_length_offset;
    pxCtrl->init_misc = TRUE;

    return status_ok;
}

status_t
prot_cfg_cb(prot_handle_t xHandle, prot_proc_func_t pkt_proc, prot_chk_func_t pkt_chk) {
    prot_ctrl_t* pxCtrl = PROT_GET_CTRL(xHandle);

    ASSERT(NULL != pxCtrl);
    pxCtrl->pkt_proc = pkt_proc;
    pxCtrl->pkt_chk = pkt_chk;
    pxCtrl->init_cb = TRUE;

    return status_ok;
}

status_t
prot_cfg(prot_handle_t xHandle) {
    prot_ctrl_t* pxCtrl = PROT_GET_CTRL(xHandle);

    ASSERT(NULL != pxCtrl);
    if (pxCtrl->init_head && pxCtrl->init_tail && pxCtrl->init_misc && pxCtrl->init_cb) {
        pxCtrl->head_tail_size = pxCtrl->head_size + pxCtrl->tail_size;
        pxCtrl->init = TRUE;
        return status_ok;
    } else {
        pxCtrl->init = FALSE;
        return status_err;
    }
}

status_t
prot_proc(prot_handle_t xHandle, IN uint8_t* pucRecvBuf, IN uint16_t usRecvd, INOUT uint16_t* pusRecvIndex,
          INOUT uint8_t* pucProcBuf, IN void* pvPara) {
    uint16_t n = 0;
    uint8_t* pucBuf = NULL;
    uint32_t ulLength = 0;
    uint16_t ucRecvIndex = 0;
    prot_ctrl_t* pxCtrl = PROT_GET_CTRL(xHandle);

    ASSERT(NULL != pxCtrl);
    TRACE("Enter prot_proc - %08X\n", xHandle);
    if (!pxCtrl->init) {
        TRACE("    prot_ctrl_t hasn't been initialised\n");
        return status_err;
    }

    pucBuf = pucRecvBuf;
    ucRecvIndex = *pusRecvIndex;

    /* Find the user defined net packet and process it */
    for (n = 0; n < usRecvd; n++) {
#if PROT_SHOW_CONT
        TRACE("0x%02X\n", (*(pucBuf + n)) & 0xFF);
#endif /* PROT_SHOW_CONT */

        /* Check start mark */
        if (ucRecvIndex < pxCtrl->head_mark_size) {
            if (pxCtrl->head_mark[ucRecvIndex] == (*(pucBuf + n))) {
                TRACE("    Find the %02X\n", *(pucBuf + n) & 0xFF);
                pucProcBuf[ucRecvIndex++] = *(pucBuf + n);
                continue;
            } else {
                TRACE("    ERROR: head parse failed\n");
                ucRecvIndex = 0;
                continue;
            }
        }

        /* Accumulate the proc data buf */
        pucProcBuf[ucRecvIndex++] = *(pucBuf + n);
        
        /* Get the pkt data length */
        ASSERT(pxCtrl->length_type < PROT_LENGTH_TYPE_SIZE);
        Bool_t bGetLength = FALSE;
        if (ucRecvIndex >= (pxCtrl->length_offset + PROT_GET_LENGTH(pxCtrl->length_type))) {
            ulLength = prv_get_length(pucProcBuf, pxCtrl->length_type, pxCtrl->length_offset);
            ulLength += pxCtrl->msg_length_offset;
            bGetLength = TRUE;
            /* Verify the ulLength */
            if (ulLength > pxCtrl->max_msg_size) {
                /* The pkt is not correct, reset the receiving process! */
                ucRecvIndex = 0;
#if PROT_TEST
                pxCtrl->len_err_num++;
#endif /* PROT_TEST */
                TRACE("    ERROR: data length is greater than %d\n", pxCtrl->max_msg_size);
                continue;
            }
        }

        if (bGetLength) {
            /* Check the tail code */
            if ((ucRecvIndex == ulLength)
                && (0
                    == memcmp(pucProcBuf + ucRecvIndex - pxCtrl->tail_mark_size, pxCtrl->tail_mark,
                              pxCtrl->tail_mark_size))) {
                /* Finally, we got the data we want, now we can process the data immediately! */
                TRACE("    Begin to process the data\n");

                if (pxCtrl->pkt_proc != NULL) {
                    uint8_t bChk = TRUE;
                    if (pxCtrl->pkt_chk != NULL) {
                        bChk = (pxCtrl->pkt_chk)(pucProcBuf, ulLength);
                    }

                    if (bChk) {
#if PROT_TEST
                        pxCtrl->rx_pkt_num++;
#endif /* PROT_TEST */
                        /* Parse and execute the command */
                        (pxCtrl->pkt_proc)(pucProcBuf,                                       /* Head */
                                           (const uint8_t*)(pucProcBuf + pxCtrl->head_size), /* Content */
                                           ulLength - pxCtrl->head_tail_size,                /* Valid data length */
                                           pvPara /* Other para */);
                    } else {
#if PROT_TEST
                        pxCtrl->chk_err_num++;
#endif /* PROT_TEST */
                        TRACE("    ERROR: calc check wrong\n");
                    }
                }

                ucRecvIndex = 0;
                continue;
            } else if (ucRecvIndex >= ulLength) {
#if PROT_TEST
                pxCtrl->len_err_num++;
#endif /* PROT_TEST */
                TRACE("    ERROR: tail code or data length wrong\n");
                ucRecvIndex = 0;
                continue;
            }
        }
    }

    *pusRecvIndex = ucRecvIndex;
    TRACE("Leave prot_proc - %08X\n", xHandle);

    return status_ok;
}

static uint32_t
prv_get_length(IN uint8_t* pucBuf, prot_length_t length_type, uint8_t length_offset) {
    uint32_t ulLength;

    switch (length_type) {
        case PROT_LENGTH_UINT8: ulLength = *(uint8_t*)(pucBuf + length_offset); break;
        case PROT_LENGTH_UINT16: ulLength = *(uint16_t*)(pucBuf + length_offset); break;
        case PROT_LENGTH_UINT32: ulLength = *(uint32_t*)(pucBuf + length_offset); break;
        default: ASSERT(0); break; /* We shoule never get here! */
    }

    return ulLength;
}

#if PROT_DEBUG

status_t
prot_show_para(prot_handle_t xHandle) {
    uint16_t n = 0;
    prot_ctrl_t* pxCtrl = PROT_GET_CTRL(xHandle);

    ASSERT(NULL != pxCtrl);
    TRACE("ProtHandle %08X\n", xHandle);

    /********************************************************/
    TRACE("    init:             %d\n", pxCtrl->init);

    /********************************************************/
    TRACE("    init_head:         %d\n", pxCtrl->init_head);
    TRACE("    head_mark_size:    %d\n", pxCtrl->head_mark_size);
    TRACE("    head_mark:        ");
    for (n = 0; n < pxCtrl->head_mark_size; n++) {
        TRACE("%02X ", pxCtrl->head_mark[n] & 0xFF);
    }
    TRACE("\n");
    TRACE("    head_size:        %d\n", pxCtrl->head_size);

    /********************************************************/
    TRACE("    init_tail:         %d\n", pxCtrl->init_tail);
    TRACE("    tail_mark_size:    %d\n", pxCtrl->tail_mark_size);
    TRACE("    tail_mark:        ");
    for (n = 0; n < pxCtrl->tail_mark_size; n++) {
        TRACE("%02X ", pxCtrl->tail_mark[n]);
    }
    TRACE("\n");
    TRACE("    tail_size:        %d\n", pxCtrl->tail_size);

    /********************************************************/
    TRACE("    head_tail_size:    %d\n", pxCtrl->head_tail_size);

    /********************************************************/
    TRACE("    init_misc:         %d\n", pxCtrl->init_misc);
    TRACE("    max_msg_size:      %d\n", pxCtrl->max_msg_size);
    TRACE("    length_type:       %d\n", pxCtrl->length_type);
    TRACE("    length_offset:    %d\n", pxCtrl->length_offset);
    TRACE("    msg_length_offset: %d\n", pxCtrl->msg_length_offset);

    /********************************************************/
    TRACE("    init_cb:           %d\n", pxCtrl->init_cb);
    TRACE("    pkt_proc:         %08X\n", pxCtrl->pkt_proc);
    TRACE("    pkt_chk:          %08X\n", pxCtrl->pkt_chk);

    /********************************************************/
#if PROT_TEST
    TRACE("    rx_pkt_num:        %d\n", pxCtrl->rx_pkt_num);
    TRACE("    chk_err_num:       %d\n", pxCtrl->chk_err_num);
    TRACE("    len_err_num:       %d\n", pxCtrl->len_err_num);
#endif /* PROT_TEST */

    return status_ok;
}

#else

status_t
prot_show_para(prot_handle_t xHandle) {
    return status_ok;
}

#endif /* PROT_DEBUG */

#endif /* PROT_ENABLE */
