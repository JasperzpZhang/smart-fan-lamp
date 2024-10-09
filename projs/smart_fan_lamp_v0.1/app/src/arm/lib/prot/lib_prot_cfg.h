/*
    prot_cfg.h

    Configuration File for Prot Module
*/

/* Copyright 2019 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 13Jul19, Karl Created
*/

#ifndef __PROT_CONFIG_H__
#define __PROT_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/* Includes */
#include "app/config.h"

/* Defines */
#ifndef PROT_ENABLE
#define PROT_ENABLE             (0)
#endif
#ifndef PROT_RTOS
#define PROT_RTOS               (1)
#endif
#ifndef PROT_DEBUG
#define PROT_DEBUG              (0)
#endif
#ifndef PROT_TEST
#define PROT_TEST               (0)
#endif
#ifndef PROT_ASSERT
#define PROT_ASSERT             (0)
#endif
#ifndef PROT_MAX_MARK_SIZE
#define PROT_MAX_MARK_SIZE      (4)
#endif
#ifndef PROT_SHOW_CONT
#define PROT_SHOW_CONT          (0)
#endif

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __PROT_CONFIG_H__ */
