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


/* ��FLASH֧��SFDP��׼ */
#undef USING_SFDP

/* ʹ��FLASH��̬��Ϣ�� */
#ifndef USING_SFDP
    #define USING_FLASH_INFO_TABLE
#endif

/* ���֧�ֵ�SFDP�汾��ΪV1.0 */
#define SUPPORT_MAX_SFDP_MAJOR_REV 1
/* JEDEC Flash������ĳ���Ϊ9(��λ:˫��) */
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

/* ҳ�ֽ� */
#define WRITE_MAX_PAGE_SIZE             256

/* ���ֽ� */
#define DUMMY_DATA                      0xFF

/* ����flash�����ʽ�� */
#define SFDP_ERASE_TYPE_MAX_NUM         4

/* �ڲ���������С������Ԫ��ƫ���� */
#define SMALLEST_ERASER_INDEX           0

/* ������ */
typedef enum {
    FLASH_SUCCESS = 0,                  /* �ɹ� */
    FLASH_ERR_NOT_FOUND = 1,            /* ��֧�� */
    FLASH_ERR_WRITE = 2,                /* д���� */
    FLASH_ERR_READ = 3,                 /* ������ */
    FLASH_ERR_TIMEOUT = 4,              /* ��ʱ */
    FLASH_ERR_ADDR_OUT_OF_BOUND = 5,    /* ��ַ�������� */
    FLASH_ERR_SPI_CONFIG = 6,           /* SPIδ���� */
}Flash_Err;

/* ״̬�Ĵ�����λ */
enum {
    STATUS_REGISTER_BUSY = (1 << 0),    /* æ/����״̬ */
    STATUS_REGISTER_WEL = (1 << 1),     /* дʹ�� */
    STATUS_REGISTER_SRP = (1 << 7),     /* ����״̬ */
};

/* FLASHдģʽ */
enum flash_write_mode {
    WM_PAGE_256B = 1 << 0,              /* ҳ���(256�ֽ�) */
    WM_BYTE = 1 << 1,                   /* ���ֽ� */
    WM_AAI = 1 << 2,                    /* �Զ����� */
    WM_DUAL_BUFFER = 1 << 3,            /* ˫������д */
};

/* spi�豸�ṹ�� */
typedef struct {
    SPI_HandleTypeDef *spix;
    GPIO_TypeDef      *cs_gpiox;
    uint16_t          cs_gpio_pin;
}Flash_Spi;

#ifdef USING_SFDP   
/* SFDP����ͷ���ṹ�� */
typedef struct {
    uint8_t id;                         /* JEDEC ID��ʾ */
    uint8_t minor_rev;                  /* JEDEC������汾 */
    uint8_t major_rev;                  /* JEDEC������汾 */
    uint8_t len;                        /* JEDEC Flash������ĳ���Ϊ9(��λ:˫��) */
    uint32_t ptp;                       /* ��ַָ��Ϊ0x000080����JEDEC Flash���������ڸõ�ַ�� */
}sfdp_para_header;

/* sfdp�ṹ�� */
typedef struct {
    bool available;                     /* ֧��SFDP */
    uint8_t major_rev;                  /* SFDP�汾�� */
    uint8_t minor_rev;                  /* SFDP�汾�� */
    uint16_t write_gran;                /* д������ */
    uint8_t erase_4k;                   /* ֧��4K���� */
    uint8_t erase_4k_cmd;               /* 4K�������� */
    bool sr_is_non_vola;                /* ֧�ַ���ʧ��״̬�Ĵ��� */
    uint8_t vola_sr_we_cmd;             /* ��ʧ��״̬�Ĵ���дʹ������ */
    bool addr_3_byte;                   /* ֧��3�ֽڵ�ַ */
    bool addr_4_byte;                   /* ֧��4�ֽڵ�ַ */
    uint32_t capacity;                  /* ���� */
    struct {
        uint32_t size;                  /* ������С */
        uint8_t cmd;                    /* �������� */
    }eraser[SFDP_ERASE_TYPE_MAX_NUM];   /* �������ͱ� */
}Flash_sfdp;
#endif

/* FLASH��Ϣ�ṹ�� */
typedef struct {
    uint8_t mf_id;                      /* ������ID */
    uint8_t type_id;                    /* ����ID */
    uint8_t capacity_id;                /* ����ID */
    uint32_t capacity;                  /* ���� */
    uint16_t write_mode;                /* дģʽ */
    uint32_t erase_gran;                /* д������ */
    uint8_t erase_gran_cmd;             /* д���������� */
}Flash_Chip;

#ifdef USING_FLASH_INFO_TABLE
/* ������ID */
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

/* FLASH��Ϣ�� */
static const Flash_Chip flash_chip_table[] = {
    /* ������ID        ����ID     ����ID   ����                дģʽ                      ����������     �������� */    
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

/* flash�ṹ�� */
typedef struct  {
    Flash_Chip chip;            /* FLASH��Ϣ */
    bool init_ok;               /* ��ʼ��OK��־ */
    bool addr_in_4_byte;        /* ֧��4�ֽڵ�ַ */
#ifdef USING_SFDP
    Flash_sfdp sfdp;            /* SFDP��Ϣ */
#endif
}Flash_t;

/* Flash��ʼ�� */
Flash_Err Spi_Flash_Init(void);
Flash_Err Spi_Flash_Term(void);
Flash_Err Spi_Flash_Config(SPI_HandleTypeDef *pSpi, GPIO_TypeDef *pCsPinPort, uint16_t nCsPin);
/* ��״̬�Ĵ��� */
Flash_Err Spi_Flash_Read_Status(uint8_t *status);
/* д״̬�Ĵ��� */
Flash_Err Spi_Flash_Write_Status(bool is_volatile, uint8_t status);
/* ��flash�ж�ȡ���� */
Flash_Err Spi_Flash_Read(uint32_t addr, size_t size, uint8_t *data);
/* дflash���� */
Flash_Err Spi_Flash_Write(uint32_t addr, size_t size, const uint8_t *data);
/* ������Ƭflash */
Flash_Err Spi_Flash_Chip_Erase(void);
/* ����flash�豸 */
Flash_Err Spi_Flash_Erase(uint32_t addr, size_t size);
/* �Ȳ�����д������ */
Flash_Err Spi_Flash_Erase_Write(uint32_t addr, size_t size, const uint8_t *data);

#ifdef __cplusplus
}
#endif /*__cplusplus */

#endif /* __MEM_SPI_FLASH_H__ */
