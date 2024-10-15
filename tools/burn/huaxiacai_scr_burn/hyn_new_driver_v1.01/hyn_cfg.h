
#ifndef _HYNITRON_CFG_H
#define _HYNITRON_CFG_H

// #define chip_cst1xx     /*suport 128 140 148 328 */
// #define chip_cst3xx     /*suport 348 340 */
// #define chip_cst7xx     /*suport 716 716se  830 836u */
// #define chip_cst8xxS    /*suport 816S 820S*/
#define chip_cst8xxT    /*suport 816t 816d 816te 820*/
// #define chip_cst36xx    /*suport 3640 */
// #define chip_cst66xx    /*suport 6656 6644 */
// #define chip_cst92xx    /*suport 9217 9220  */
// #define chip_cst3240    /*suport 3240  */


#define I2C_PORT
#ifdef I2C_PORT
    #define I2C_USE_DMA      (1)
#else
    #define SPI_MODE         (0)
    #define SPI_DELAY_CS     (10) //us
    #define SPI_CLOCK_FREQ   (8000000)
#endif

#define HYN_TRANSFER_LIMIT    (0)

#define HYN_UART_DEBUG_EN     (1)
#define HYN_APK_DEBUG_EN      (0)
#define HYN_TP0_TEST_LOG_SAVE  (1)

#define HYN_GESTURE_EN        (0)
#define HYN_PROX_EN           (0)

#define MAX_POINTS_REPORT     (5)

#define KEY_USED_POS_REPORT   (1)

#define ESD_CHECK_EN          (0)

#define HYN_MT_PROTOCOL_B_EN  (1)

#endif



