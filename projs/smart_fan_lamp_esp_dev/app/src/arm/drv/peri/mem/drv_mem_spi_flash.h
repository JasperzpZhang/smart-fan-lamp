/*
    MemSpiFlash.h
    
    Head File for Mem Spi Flash Module
*/

/* Copyright 2019 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 20Sep18, David Created
    01b, 06Oct18, Karl Modified
    01c, 24Jul19, Karl Reconstructured Mem lib
*/

#ifndef __MEM_SPI_FLASH_H__
#define __MEM_SPI_FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

/* Includes */
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <main.h>
#include "lib/type/lib_type.h"
#include "drv/peri/mem/drv_mem.h"
#include "drv/peri/mem/drv_mem_config.h"


/* 该FLASH支持SFDP标准 */
#undef USING_SFDP

/* 使用FLASH静态信息表 */
#ifndef USING_SFDP
    #define USING_FLASH_INFO_TABLE
#endif

/* 最大支持的SFDP版本号为V1.0 */
#define SUPPORT_MAX_SFDP_MAJOR_REV 1
/* JEDEC Flash参数表的长度为9(单位:双字) */
#define BASIC_TABLE_LEN 9

#define CMD_WRITE_ENABLE                0x06
#define CMD_WRITE_DISABLE               0x04
#define CMD_READ_STATUS_REGISTER        0x05
#define VOLATILE_SR_WRITE_ENABLE        0x50
#define CMD_WRITE_STATUS_REGISTER       0x01
#define CMD_PAGE_PROGRAM                0x02
#define CMD_AAI_WORD_PROGRAM            0xAD
#define CMD_ERASE_CHIP                  0xC7
#define CMD_READ_DATA                   0x03
#define CMD_MANUFACTURER_DEVICE_ID      0x90
#define CMD_JEDEC_ID                    0x9F
#define CMD_READ_UNIQUE_ID              0x4B
#define CMD_READ_SFDP_REGISTER          0x5A
#define CMD_ENABLE_RESET                0x66
#define CMD_RESET                       0x99
#define CMD_ENTER_4B_ADDRESS_MODE       0xB7
#define CMD_EXIT_4B_ADDRESS_MODE        0xE9

/* 页字节 */
#define WRITE_MAX_PAGE_SIZE             256

/* 空字节 */
#define DUMMY_DATA                      0xFF

/* 擦除flash的最大方式数 */
#define SFDP_ERASE_TYPE_MAX_NUM         4

/* 在擦除表中最小擦除单元的偏移量 */
#define SMALLEST_ERASER_INDEX           0

/* 错误码 */
typedef enum {
    FLASH_SUCCESS = 0,                  /* 成功 */
    FLASH_ERR_NOT_FOUND = 1,            /* 不支持 */
    FLASH_ERR_WRITE = 2,                /* 写错误 */
    FLASH_ERR_READ = 3,                 /* 读错误 */
    FLASH_ERR_TIMEOUT = 4,              /* 超时 */
    FLASH_ERR_ADDR_OUT_OF_BOUND = 5,    /* 地址超出容量 */
    FLASH_ERR_SPI_CONFIG = 6,           /* SPI未配置 */
}Flash_Err;

/* 状态寄存器的位 */
enum {
    STATUS_REGISTER_BUSY = (1 << 0),    /* 忙/空闲状态 */
    STATUS_REGISTER_WEL = (1 << 1),     /* 写使能 */
    STATUS_REGISTER_SRP = (1 << 7),     /* 保护状态 */
};

/* FLASH写模式 */
enum flash_write_mode {
    WM_PAGE_256B = 1 << 0,              /* 页编程(256字节) */
    WM_BYTE = 1 << 1,                   /* 单字节 */
    WM_AAI = 1 << 2,                    /* 自动增长 */
    WM_DUAL_BUFFER = 1 << 3,            /* 双缓冲区写 */
};

/* spi设备结构体 */
typedef struct {
    SPI_HandleTypeDef *spix;
    GPIO_TypeDef      *cs_gpiox;
    uint16_t          cs_gpio_pin;
}Flash_Spi;

#ifdef USING_SFDP   
/* SFDP参数头部结构体 */
typedef struct {
    uint8_t id;                         /* JEDEC ID标示 */
    uint8_t minor_rev;                  /* JEDEC参数表版本 */
    uint8_t major_rev;                  /* JEDEC参数表版本 */
    uint8_t len;                        /* JEDEC Flash参数表的长度为9(单位:双字) */
    uint32_t ptp;                       /* 地址指针为0x000080，即JEDEC Flash参数表存放在该地址下 */
}sfdp_para_header;

/* sfdp结构体 */
typedef struct {
    bool available;                     /* 支持SFDP */
    uint8_t major_rev;                  /* SFDP版本号 */
    uint8_t minor_rev;                  /* SFDP版本号 */
    uint16_t write_gran;                /* 写粗粒度 */
    uint8_t erase_4k;                   /* 支持4K擦除 */
    uint8_t erase_4k_cmd;               /* 4K擦除命令 */
    bool sr_is_non_vola;                /* 支持非易失性状态寄存器 */
    uint8_t vola_sr_we_cmd;             /* 易失性状态寄存器写使能命令 */
    bool addr_3_byte;                   /* 支持3字节地址 */
    bool addr_4_byte;                   /* 支持4字节地址 */
    uint32_t capacity;                  /* 容量 */
    struct {
        uint32_t size;                  /* 擦除大小 */
        uint8_t cmd;                    /* 擦除命令 */
    }eraser[SFDP_ERASE_TYPE_MAX_NUM];   /* 擦除类型表 */
}Flash_sfdp;
#endif

/* FLASH信息结构体 */
typedef struct {
    uint8_t mf_id;                      /* 制造商ID */
    uint8_t type_id;                    /* 类型ID */
    uint8_t capacity_id;                /* 容量ID */
    uint32_t capacity;                  /* 容量 */
    uint16_t write_mode;                /* 写模式 */
    uint32_t erase_gran;                /* 写粗粒度 */
    uint8_t erase_gran_cmd;             /* 写粗粒度命令 */
}Flash_Chip;

#ifdef USING_FLASH_INFO_TABLE
/* 制造商ID */
#define MF_ID_CYPRESS       0x01
#define MF_ID_FUJITSU       0x04
#define MF_ID_EON           0x1C
#define MF_ID_ATMEL         0x1F
#define MF_ID_MICRON        0x20
#define MF_ID_AMIC          0x37
#define MF_ID_SANYO         0x62
#define MF_ID_INTEL         0x89
#define MF_ID_ESMT          0x8C
#define MF_ID_FUDAN         0xA1
#define MF_ID_HYUNDAI       0xAD
#define MF_ID_SST           0xBF
#define MF_ID_GIGADEVICE    0xC8
#define MF_ID_ISSI          0xD5
#define MF_ID_WINBOND       0xEF

/* FLASH信息表 */
static const Flash_Chip flash_chip_table[] = {
    /* 制造商ID        类型ID     容量ID   容量                写模式                      擦除粗粒度     擦除命令 */    
    {MF_ID_ATMEL,       0x26,       0x00,   2 * 1024 * 1024,    WM_BYTE | WM_DUAL_BUFFER,   512,            0x81},      /* AT45DB161E */
    {MF_ID_WINBOND,     0x40,       0x13,   512 * 1024,         WM_PAGE_256B,               4096,           0x20},      /* W25Q40BV */
    {MF_ID_SST,         0x25,       0x41,   2 * 1024 * 1024,    WM_BYTE | WM_AAI,           4096,           0x20},      /* SST25VF016B */
    {MF_ID_MICRON,      0x20,       0x16,   4 * 1024 * 1024,    WM_PAGE_256B,               64*1024,        0xD8},      /* M25P32 */
    {MF_ID_EON,         0x30,       0x16,   4* 1024 * 1024,     WM_PAGE_256B,               4096,           0x20},      /* EN25Q32B */
    {MF_ID_GIGADEVICE,  0x40,       0x17,   8 * 1024 * 1024,    WM_PAGE_256B,               4096,           0x20},      /* GD25Q64B */
    {MF_ID_CYPRESS,     0x40,       0x15,   2 * 1024 * 1024,    WM_PAGE_256B,               4096,           0x20},      /* S25FL216K */
    {MF_ID_AMIC,        0x30,       0x14,   1 * 1024 * 1024,    WM_PAGE_256B,               4096,           0x20},      /* A25L080 */
    {MF_ID_ESMT,        0x20,       0x13,   512 * 1024,         WM_BYTE | WM_AAI,           4096,           0x20},      /* F25L004 */
    {MF_ID_SST,         0x25,       0x41,   2 * 1024 * 1024,    WM_BYTE | WM_AAI,           4096,           0x20},      /* PCT25VF016B */
    {MF_ID_WINBOND,     0x40,       0x18,   16 * 1024 * 1024,   WM_PAGE_256B,               4096,           0x20},      /* W25Q128FV */
    {MF_ID_WINBOND,     0x40,       0x19,   32 * 1024 * 1024,   WM_PAGE_256B,               4096,           0x20},      /* W25Q256FV */
    {MF_ID_FUDAN,       0x40,       0x15,   2 * 1024 * 1024,    WM_PAGE_256B,               4096,           0x20},      /* FM25Q16A */
    {MF_ID_FUDAN,       0x40,       0x17,   8 * 1024 * 1024,    WM_PAGE_256B,               4096,           0x20},      /* FM25Q64A */
};
#endif

/* flash结构体 */
typedef struct  {
    Flash_Chip chip;            /* FLASH信息 */
    bool init_ok;               /* 初始化OK标志 */
    bool addr_in_4_byte;        /* 支持4字节地址 */
#ifdef USING_SFDP
    Flash_sfdp sfdp;            /* SFDP信息 */
#endif
}Flash_t;

/* Flash初始化 */
Flash_Err Spi_Flash_Init(void);
Flash_Err Spi_Flash_Term(void);
Flash_Err Spi_Flash_Config(SPI_HandleTypeDef *pSpi, GPIO_TypeDef *pCsPinPort, uint16_t nCsPin);
/* 读状态寄存器 */
Flash_Err Spi_Flash_Read_Status(uint8_t *status);
/* 写状态寄存器 */
Flash_Err Spi_Flash_Write_Status(bool is_volatile, uint8_t status);
/* 从flash中读取数据 */
Flash_Err Spi_Flash_Read(uint32_t addr, size_t size, uint8_t *data);
/* 写flash数据 */
Flash_Err Spi_Flash_Write(uint32_t addr, size_t size, const uint8_t *data);
/* 擦除整片flash */
Flash_Err Spi_Flash_Chip_Erase(void);
/* 擦除flash设备 */
Flash_Err Spi_Flash_Erase(uint32_t addr, size_t size);
/* 先擦除后写入数据 */
Flash_Err Spi_Flash_Erase_Write(uint32_t addr, size_t size, const uint8_t *data);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __MEM_SPI_FLASH_H__ */
