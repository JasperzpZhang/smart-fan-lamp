/**
 * \file            lib_type.h
 * \brief           Type library
 */

/*
 * Copyright (c) 2024 Jasper
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is all of the type library..
 *
 * Author:          Jasper <jasperzpzhang@gmail.com>
 * Version:         v1.0.0
 */

#ifndef __LIB_TYPE_H__
#define __LIB_TYPE_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Includes */
#include <time.h>
#include <stdint.h>

	/* Types */
	typedef enum
	{
        status_ok,
        status_err,
	} status_t;

	typedef uint8_t Bool_t;

	typedef struct tm Time_t;
    

	typedef int SOCKET;

/* Defines */
#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif
#ifndef NULL
#define NULL (0)
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET (-1)
#endif
#ifndef SOCKET_ERROR
#define SOCKET_ERROR (-1)
#endif

#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif
#ifndef INOUT
#define INOUT
#endif

#ifndef SECTION
#define SECTION(x) __attribute__((section(x)))
#endif
#ifndef USED
#define USED __attribute__((used))
#endif
#ifndef ALIGN
#define ALIGN(n) __attribute__((aligned(n)))
#endif
#ifndef WEAK
#define WEAK __attribute__((weak))
#endif
#ifndef INLINE
#define INLINE static __inline
#endif
#ifndef API
#define API
#endif

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /*__LIB_TYPE_H__*/
