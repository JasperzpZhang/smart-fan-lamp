/*
    MemSpiFlash.c

    Implementation File for Mem Spi Flash Module
*/

/* Copyright 2019 Shanghai Master Inc. */

/*
    modification history
    --------------------
    01a, 20Sep18, David Created
    01b, 06Oct18, Karl Modified
    01c, 24Jul19, Karl Reconstructured Mem lib
*/

/* Includes */
#include "drv/peri/mem/drv_mem_spi_flash.h"

#if (MEM_ENABLE && MEM_ENABLE_SPIFLASH)

/* Debug config */
#if MEM_DEBUGx
    #undef TRACE
    #define TRACE(...)  debug_printf(__VA_ARGS__)
#else
    #undef TRACE
    #define TRACE(...)
#endif /* MEM_DEBUGx */
#if MEM_ASSERT
    #undef ASSERT
    #define ASSERT(a)   while(!(a)){DebugPrintf("ASSERT failed: %s %d\n", __FILE__, __LINE__);}
#else
    #undef ASSERT
    #define ASSERT(...)
#endif /* MEM_ASSERT */

/* Forward declaration */
/* Read and write SPI device */
static Flash_Err Spi_Write_Read(uint8_t *write_buf, size_t write_size, uint8_t *read_buf, size_t read_size);
#ifdef USING_SFDP
/* 读SFDP数据 */
static Flash_Err read_sfdp_data(uint32_t addr, uint8_t *read_buf, size_t size);
/* 读SFDP参数头部，并验证是否可用 */
static bool read_sfdp_header(void);
/* 读JEDEC基本参数头部，并验证是否可用 */
static bool read_basic_header(sfdp_para_header *basic_header);
/* 读JEDEC Flash参数表 */
static bool read_basic_table(sfdp_para_header *basic_header);
/* 读SFDP头部参数 */
static bool flash_read_sfdp(void);
/* 获取最合适的擦除方式 */
static size_t flash_sfdp_get_suitable_eraser(uint32_t addr, size_t erase_size);
#endif
/* read the device ID(Mannufacture ID、Type ID、capacity ID) */
static Flash_Err Read_Jedec_Id(void);
static Flash_Err Wait_Busy(void);
static Flash_Err Flash_Reset(void);
static Flash_Err Set_Write_Enabled(bool enabled);
static Flash_Err Set_4_Byte_Address_Mode(bool enabled);
static status_t Make_Adress_Byte_Array(uint32_t addr, uint8_t *array);
static Flash_Err Page256_or_1_Byte_Write(uint32_t addr, size_t size, uint16_t write_gran, const uint8_t *data);
static Flash_Err Aai_Write(uint32_t addr, size_t size, const uint8_t *data);
Flash_Err Spi_Flash_Write_Status(bool is_volatile, uint8_t status);

/* Local variables */
static Flash_Spi spi = {.spix = NULL, .cs_gpiox = NULL, .cs_gpio_pin = NULL};
static Flash_t flash;
static Bool_t s_bConfig = FALSE;

/* Functions */
Flash_Err Spi_Flash_Init()
{
    Flash_Err result;

    if(!s_bConfig)
    {
        return FLASH_ERR_SPI_CONFIG;
    }
    
    result = Flash_Reset();
    if(result != FLASH_SUCCESS) 
        return result;      
    result = Read_Jedec_Id();
    if(result != FLASH_SUCCESS)
        return result;

#ifdef USING_SFDP
    /* 读SFDP参数 */
    if(flash_read_sfdp() == false)
        return FLASH_ERR_NOT_FOUND;
    else
    {
        /* FLASH容量 */
        flash.chip.capacity = flash.sfdp.capacity;
        
        /* FLASH写粒度，1字节或者256字节页编程 */
        if(flash.sfdp.write_gran == 1) 
            flash.chip.write_mode = WM_BYTE;
        else 
            flash.chip.write_mode = WM_PAGE_256B;

        /* 最小的擦除单元大小和命令 */
        flash.chip.erase_gran = flash.sfdp.eraser[0].size;
        flash.chip.erase_gran_cmd = flash.sfdp.eraser[0].cmd;
        for(size_t i = 1; i < SFDP_ERASE_TYPE_MAX_NUM; i++) 
        {
            if(flash.sfdp.eraser[i].size != 0 && flash.chip.erase_gran > flash.sfdp.eraser[i].size) 
            {
                flash.chip.erase_gran = flash.sfdp.eraser[i].size;
                flash.chip.erase_gran_cmd = flash.sfdp.eraser[i].cmd;
            }
        }
    }
#endif

#ifdef USING_FLASH_INFO_TABLE
    /* Read the information from the static information table，Name | Capacity | Write mode |  Erasure coarse-grained | Erasure command */
    for(size_t i = 0; i < sizeof(flash_chip_table) / sizeof(Flash_Chip); i++)
    {
        if((flash_chip_table[i].mf_id == flash.chip.mf_id) && 
             (flash_chip_table[i].type_id == flash.chip.type_id) && 
             (flash_chip_table[i].capacity_id == flash.chip.capacity_id))
        {
            flash.chip.capacity = flash_chip_table[i].capacity;
            flash.chip.write_mode = flash_chip_table[i].write_mode;
            flash.chip.erase_gran = flash_chip_table[i].erase_gran;
            flash.chip.erase_gran_cmd = flash_chip_table[i].erase_gran_cmd;
            break;
        }
    }
#endif

    /* Self-growing flash must be unprotected before it can be written and erased */
    if(flash.chip.write_mode & WM_AAI) 
    {
        /* write status register,set to unprotected */
        result = Spi_Flash_Write_Status(true, 0x00);
        if(result != FLASH_SUCCESS) 
            return result;
    }

    /* If larger than 16M, the address is in 4-byte mode */
    if(flash.chip.capacity > (1 << 24))
        result = Set_4_Byte_Address_Mode(true);
    else 
        flash.addr_in_4_byte = false;   

    flash.init_ok = true;
    TRACE("flash is initialize success.\r\n");

    return result;
}

Flash_Err Spi_Flash_Term(void)
{
    /* Do nothing */
    return FLASH_SUCCESS;
}

Flash_Err Spi_Flash_Config(SPI_HandleTypeDef *pSpi, GPIO_TypeDef *pCsPinPort, uint16_t nCsPin)
{
    spi.spix = pSpi;
    spi.cs_gpiox = pCsPinPort;
    spi.cs_gpio_pin = nCsPin;
    s_bConfig = TRUE;
    return FLASH_SUCCESS;
}

/* Read the status register */
Flash_Err Spi_Flash_Read_Status(uint8_t *status)
{
    uint8_t cmd = CMD_READ_STATUS_REGISTER;

    assert_param(status);

    return Spi_Write_Read(&cmd, 1, status, 1);
}

/* Write the status register */
Flash_Err Spi_Flash_Write_Status(bool is_volatile, uint8_t status)
{
    Flash_Err result;
    uint8_t cmd_data[2];

    /* Enable the volatile writing */
    if(is_volatile) 
    {
        cmd_data[0] = VOLATILE_SR_WRITE_ENABLE;
        result = Spi_Write_Read(cmd_data, 1, NULL, 0);
    }
    /* Enable the Novolatile writing */
    else
        result = Set_Write_Enabled(true);
    if(result !=FLASH_SUCCESS)
        return result;
    
    cmd_data[0] = CMD_WRITE_STATUS_REGISTER;
    cmd_data[1] = status;
    result = Spi_Write_Read(cmd_data, 2, NULL, 0);
    if(result != FLASH_SUCCESS) 
        TRACE("Error: Write_status register failed.\r\n");

    return result;
}

/* Read data from flash */
Flash_Err Spi_Flash_Read(uint32_t addr, size_t size, uint8_t *data)
{
    Flash_Err result;
    uint8_t cmd_data[5], cmd_size;

    /* Check the validity of the parameter */
    assert_param(data);
    assert_param(flash->init_ok);

    /* Check for excess capacity */
    if(addr + size > flash.chip.capacity) 
    {
        TRACE("Error: Flash address is out of bound.");
        return FLASH_ERR_ADDR_OUT_OF_BOUND;
    }

    /* Wait for flash to be free */
    result = Wait_Busy();
    if(result != FLASH_SUCCESS)
        return result;
    
    /* Read the data */
    cmd_data[0] = CMD_READ_DATA;
    Make_Adress_Byte_Array(addr, &cmd_data[1]);
    cmd_size = flash.addr_in_4_byte ? 5 : 4;
    result = Spi_Write_Read(cmd_data, cmd_size, data, size);

    return result;
}

/* Write flash data */
Flash_Err Spi_Flash_Write(uint32_t addr, size_t size, const uint8_t *data)
{
    Flash_Err result;

    /* Page programming mode */
    if(flash.chip.write_mode & WM_PAGE_256B) 
        result = Page256_or_1_Byte_Write(addr, size, 256, data);
    /* self growth model */
    else if(flash.chip.write_mode & WM_AAI) 
        result = Aai_Write(addr, size, data);
    /* Double buffer mode */
    else if(flash.chip.write_mode & WM_DUAL_BUFFER);
    
    TRACE("Spi_Flash_Write\r\n");
    return result;
}

/* Erase the whole piece of flash */
Flash_Err Spi_Flash_Chip_Erase(void)
{
    Flash_Err result;
    uint8_t cmd_data[4];

    assert_param(flash);
    assert_param(flash->init_ok);

    /* Enable write */
    result = Set_Write_Enabled(true);
    if(result != FLASH_SUCCESS)
        return result;

    cmd_data[0] = CMD_ERASE_CHIP;
    if(flash.chip.write_mode & WM_DUAL_BUFFER) 
    {
        cmd_data[1] = 0x94;
        cmd_data[2] = 0x80;
        cmd_data[3] = 0x9A;
        result = Spi_Write_Read(cmd_data, 4, NULL, 0);
    } 
    else
        result = Spi_Write_Read(cmd_data, 1, NULL, 0);
    if(result == FLASH_SUCCESS) 
        result = Wait_Busy();
    else
        TRACE("Error: Flash chip erase SPI communicate error.");

    Set_Write_Enabled(false);

    return result;
}

/* Erase flash*/
Flash_Err Spi_Flash_Erase(uint32_t addr, size_t size)
{
    Flash_Err result;
    uint8_t cmd_data[5], cmd_size, cur_erase_cmd;
#ifdef USING_SFDP
    size_t eraser_index;
#endif  
    size_t cur_erase_size;

    assert_param(flash->init_ok);
    
    /* Check for excess memory */
    if(addr + size > flash.chip.capacity) 
    {
        TRACE("Error: Flash address is out of bound.");
        return FLASH_ERR_ADDR_OUT_OF_BOUND;
    }

    if(addr == 0 && size == flash.chip.capacity)
        return Spi_Flash_Chip_Erase();

    /* Loop erasure, each erasure with the appropriate erasure instruction, as few times as possible */
    while(size) 
    {
    #ifdef USING_SFDP       
        /* 算出最合适的擦除指令 */
        if(flash.sfdp.available) 
        {
            eraser_index = flash_sfdp_get_suitable_eraser(addr, size);
            cur_erase_cmd = flash.sfdp.eraser[eraser_index].cmd;
            cur_erase_size = flash.sfdp.eraser[eraser_index].size;
        }
        else
    #endif
        {
            cur_erase_cmd = flash.chip.erase_gran_cmd;
            cur_erase_size = flash.chip.erase_gran;
        }
        
        /* write enable */
        result = Set_Write_Enabled(true);
        if(result != FLASH_SUCCESS) 
            return result;

        /* erase */
        cmd_data[0] = cur_erase_cmd;
        Make_Adress_Byte_Array(addr, &cmd_data[1]);
        cmd_size = flash.addr_in_4_byte ? 5 : 4;
        result = Spi_Write_Read(cmd_data, cmd_size, NULL, 0);
        if(result != FLASH_SUCCESS) 
        {
            TRACE("Error: Flash erase SPI communicate error.");
            break;
        }
        
        /* Wait for flash be free */
        result = Wait_Busy();
        if(result != FLASH_SUCCESS) 
            break;
        
        /* Align the next erase address and calculate the erase size */
        if(addr % cur_erase_size != 0) 
        {
            if(size > cur_erase_size - (addr % cur_erase_size)) 
            {
                size -= cur_erase_size - (addr % cur_erase_size);
                addr += cur_erase_size - (addr % cur_erase_size);
            } 
            else 
                break;
        } 
        else 
        {
            if (size > cur_erase_size) 
            {
                size -= cur_erase_size;
                addr += cur_erase_size;
            } 
            else 
                break;
        }
    }

    Set_Write_Enabled(false);

    return result;
}

/* Erasure before writing data */
Flash_Err Spi_Flash_Erase_Write(uint32_t addr, size_t size, const uint8_t *data)
{
    Flash_Err result;

    result = Spi_Flash_Erase(addr, size);
    if(result == FLASH_SUCCESS) 
        result = Spi_Flash_Write(addr, size, data);
    TRACE("Spi_Flash_Erase_Write\r\n");
    return result;
}

static Flash_Err Read_Jedec_Id(void)
{
    Flash_Err result;
    uint8_t cmd_data[1];
    uint8_t recv_data[3];
    
    cmd_data[0] = CMD_JEDEC_ID;
    result = Spi_Write_Read(cmd_data, sizeof(cmd_data), recv_data, sizeof(recv_data));
    if(result != FLASH_SUCCESS)
    {
        TRACE("Error: Read JEDEC ID.\r\n");
        return result;
    }
    
    flash.chip.mf_id = recv_data[0];
    flash.chip.type_id = recv_data[1];
    flash.chip.capacity_id = recv_data[2];

    return result;
}

/* Waiting until the flash device is free */
static Flash_Err Wait_Busy(void)
{
    Flash_Err result;
    uint8_t status;
    size_t retry_times = 1000;

    /* The busy position 0 in the waiting status register */
    do
    {

        result = Spi_Flash_Read_Status(&status);
        if(result == FLASH_SUCCESS && (status & STATUS_REGISTER_BUSY) == 0)
            break;

        HAL_Delay(1);
    }while(--retry_times);
    

    if(retry_times == 0)
        result = FLASH_ERR_TIMEOUT;
    
    return result;
}

/* Software reset FLASH device */
static Flash_Err Flash_Reset(void)
{
    Flash_Err result;
    uint8_t cmd_data[2];

    /* Write reset instruction */
    cmd_data[0] = CMD_ENABLE_RESET;
    cmd_data[1] = CMD_RESET;
    result = Spi_Write_Read(cmd_data, 2, NULL, 0);
    if(result != FLASH_SUCCESS)
        return result;
    
    /* Wait for reset to complete */
    result = Wait_Busy();
    if(result != FLASH_SUCCESS)
        TRACE("Error: Reset failed.");
    
    return result;
}


static Flash_Err Set_Write_Enabled(bool enabled)
{
    Flash_Err result;
    uint8_t cmd, register_status;

    if(enabled) 
        cmd = CMD_WRITE_ENABLE;
    else 
        cmd = CMD_WRITE_DISABLE;
    result = Spi_Write_Read(&cmd, 1, NULL, 0);
    if(result != FLASH_SUCCESS)
        return result;

    result = Spi_Flash_Read_Status(&register_status);
    if(result != FLASH_SUCCESS)
        return result;
    
    /* Check the write success or failure */
    if(enabled && (register_status & STATUS_REGISTER_WEL) == 0) 
    {
        TRACE("Error: enable write status.\r\n");
        return FLASH_ERR_WRITE;
    } 
    else if(!enabled && (register_status & STATUS_REGISTER_WEL) == 1) 
    {
        TRACE("Error: disable write status.\r\n");
        return FLASH_ERR_WRITE;
    }
    
    return result;
}

static Flash_Err Set_4_Byte_Address_Mode(bool enabled)
{
    Flash_Err result;
    uint8_t cmd;


    result = Set_Write_Enabled(true);
    if(result != FLASH_SUCCESS) 
        return result;

    if(enabled) 
        cmd = CMD_ENTER_4B_ADDRESS_MODE;
    else 
        cmd = CMD_EXIT_4B_ADDRESS_MODE;
    result = Spi_Write_Read(&cmd, 1, NULL, 0);
    if(result == FLASH_SUCCESS) 
        flash.addr_in_4_byte = enabled ? true : false;
    else 
        TRACE("Error: %s 4-Byte addressing mode failed.\r\n", enabled ? "Enter" : "Exit");


    Set_Write_Enabled(false);
    
    return result;
}

/* Put the address into array */
static status_t Make_Adress_Byte_Array(uint32_t addr, uint8_t *array)
{
    uint8_t len, i;

    assert_param(array);

    len = flash.addr_in_4_byte ? 4 : 3;
    for(i = 0; i < len; i++) 
        array[i] = (addr >> ((len - (i + 1)) * 8)) & 0xFF;
    return status_ok;
}

/* Writes data through page programming instructions */
static Flash_Err Page256_or_1_Byte_Write(uint32_t addr, size_t size, uint16_t write_gran, const uint8_t *data)
{
    Flash_Err result;
    uint8_t cmd_data[5 + WRITE_MAX_PAGE_SIZE], cmd_size;
    size_t data_size;

    assert_param(write_gran == 1 || write_gran == 256);
    assert_param(flash->init_ok);
    
    if(addr + size > flash.chip.capacity) 
    {
        TRACE("Error: Address is out of bound.\r\n");
        return FLASH_ERR_ADDR_OUT_OF_BOUND;
    }
    
    /* Loop written */
    while(size) 
    {
        result = Set_Write_Enabled(true);
        if(result != FLASH_SUCCESS)
            return result;
        cmd_data[0] = CMD_PAGE_PROGRAM;
        Make_Adress_Byte_Array(addr, &cmd_data[1]);
        cmd_size = flash.addr_in_4_byte ? 5 : 4;
        if(addr % write_gran != 0) 
        {
            if(size > write_gran - (addr % write_gran)) 
                data_size = write_gran - (addr % write_gran);
            else 
                data_size = size;
        } 
        else
        {
            if(size > write_gran) 
                data_size = write_gran;
            else 
                data_size = size;
        }
        size -= data_size;
        addr += data_size;
        memcpy(&cmd_data[cmd_size], data, data_size);
        result = Spi_Write_Read(cmd_data, cmd_size + data_size, NULL, 0);
        if(result != FLASH_SUCCESS) 
        {
            TRACE("Error: Flash write SPI communicate error.");
            break;
        }
        
        result = Wait_Busy();
        if(result != FLASH_SUCCESS) 
            break;
        
        /* Reverse-offset data_size bytes */
        data += data_size;
    }
    Set_Write_Enabled(false);

    return result;
}

/* Address self - growth mode writes data */
static Flash_Err Aai_Write(uint32_t addr, size_t size, const uint8_t *data)
{
    Flash_Err result;
    uint8_t cmd_data[6], cmd_size;
    bool first_write = true;

    assert_param(flash->init_ok);
    
    if(addr + size > flash.chip.capacity) 
    {
        TRACE("Error: Flash address is out of bound.");
        return FLASH_ERR_ADDR_OUT_OF_BOUND;
    }
    
    /* The first byte is written when the first address is odd */
    if(addr % 2 != 0) 
    {
        result = Page256_or_1_Byte_Write(addr++, 1, 1, data++);
        if(result != FLASH_SUCCESS) 
            return result;
        size--;
    }

    result = Set_Write_Enabled(true);
    if(result != FLASH_SUCCESS) 
        return result;

    cmd_data[0] = CMD_AAI_WORD_PROGRAM;
    while(size >= 2) 
    {
        if(first_write) 
        {
            Make_Adress_Byte_Array(addr, &cmd_data[1]);
            cmd_size = flash.addr_in_4_byte ? 5 : 4;
            cmd_data[cmd_size] = *data;
            cmd_data[cmd_size + 1] = *(data + 1);
            first_write = false;
        }
        else 
        {
            cmd_size = 1;
            cmd_data[1] = *data;
            cmd_data[2] = *(data + 1);
        }
        result = Spi_Write_Read(cmd_data, cmd_size + 2, NULL, 0);
        if(result != FLASH_SUCCESS) 
        {
            TRACE("Error: Flash write SPI communicate error.");
            Set_Write_Enabled(false);
            return result;
        }

        result = Wait_Busy();
        if(result != FLASH_SUCCESS) 
        {
            Set_Write_Enabled(false);
            return result;
        }

        size -= 2;
        addr += 2;
        data += 2;
    }

    result = Set_Write_Enabled(false);

    if(result == FLASH_SUCCESS && size == 1) 
        result = Page256_or_1_Byte_Write(addr, 1, 1, data);

    if(result != FLASH_SUCCESS) 
        Set_Write_Enabled(false);

    return result;
}

#ifdef USING_SFDP
/* 读SFDP参数 */
static bool flash_read_sfdp(void)
{
    /* JEDEC基本参数头部 */
    sfdp_para_header basic_header;
    
    /* 读SFDP参数头部和JEDEC基本参数头部，并验证是否可用 */
    if(read_sfdp_header() != true || read_basic_header(&basic_header) != true)
        return false;
    
    /* 读JEDEC Flash参数表的内容 */
    return read_basic_table(&basic_header); 
}

/* 读SFDP数据 */
static Flash_Err read_sfdp_data(uint32_t addr, uint8_t *read_buf, size_t size)
{
    /* 读SFDP命令为0x5A */
    uint8_t cmd[] = {CMD_READ_SFDP_REGISTER, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF, DUMMY_DATA};

    /* 检查参数合法性 */
    assert_param(addr < 1 << 24);
    assert_param(read_buf);

    /* 读取数据 */
    return Spi_Write_Read(cmd, sizeof(cmd), read_buf, size);
}

/* 读SFDP参数头部，并验证是否可用 */
static bool read_sfdp_header(void)
{
    uint32_t header_addr = 0;
    uint8_t header[2 * 4] = {0};

    /* 初始化available */
    flash.sfdp.available = false;
    
    /* 读取sfdp头部 */
    if(read_sfdp_data(header_addr, header, sizeof(header)) != FLASH_SUCCESS) 
    {
        TRACE("Error: Read SFDP header.\r\n");
        return false;
    }
    
    /* 检查头部4个字节是否为签名档SFDP */
    if(!(header[0] == 'S' && header[1] == 'F' && header[2] == 'D' && header[3] == 'P')) 
    {
        TRACE("Error: Check SFDP signature.\r\n");
        return false;
    }

    /* SFDP版本号为V1.0 */
    flash.sfdp.minor_rev = header[4];
    flash.sfdp.major_rev = header[5];
    if(flash.sfdp.major_rev > SUPPORT_MAX_SFDP_MAJOR_REV) 
    {
        TRACE("Error: SFDP_V%d.%d is not supported.\r\n", 
                                 flash.sfdp.major_rev, flash.sfdp.minor_rev);
        return false;
    }

    return true;
}

/* 读JEDEC基本参数头部，并验证是否可用 */
static bool read_basic_header(sfdp_para_header *basic_header)
{
    uint32_t header_addr = 8;
    uint8_t header[2 * 4] = {0};

    /* 检查参数合法性 */
    assert_param(basic_header);

    /* 读取sfdp头部 */
    if(read_sfdp_data(header_addr, header, sizeof(header)) != FLASH_SUCCESS) 
    {
        TRACE("Error: Read JEDEC basic parameter header.\r\n");
        return false;
    }
    
    /* JEDEC ID标示 */
    basic_header->id = header[0];
    /* JEDEC参数表版本 */
    basic_header->minor_rev = header[1];
    basic_header->major_rev = header[2];
    /* JEDEC Flash参数表的长度为9(单位:双字) */
    basic_header->len = header[3];
    /* 地址指针为0x000080，即JEDEC Flash参数表存放在该地址下 */
    basic_header->ptp = header[4] | header[5] << 8 | header[6] << 16;
    
    /* JEDEC参数表版本为V1.0 */
    if(basic_header->major_rev > SUPPORT_MAX_SFDP_MAJOR_REV) 
    {
        TRACE("Error: JEDEC_V%d.%d basic parameter header is not supported.\r\n", 
                                 basic_header->major_rev, basic_header->minor_rev);
        return false;
    }
    
    /* JEDEC Flash参数表的长度为9(单位:双字) */
    if(basic_header->len < BASIC_TABLE_LEN)
    {
        TRACE("Error: JEDEC basic parameter table length is %d.\r\n", basic_header->len);
        return false;
    }

    return true;
}

/* 读JEDEC Flash参数表 */
static bool read_basic_table(sfdp_para_header *basic_header)
{
    uint32_t table_addr = basic_header->ptp;
    uint8_t table[BASIC_TABLE_LEN * 4] = {0}, i, j;

    /* 检查参数合法性 */
    assert_param(basic_header);

    /* 读JEDEC Flash参数表 */
    if(read_sfdp_data(table_addr, table, sizeof(table)) != FLASH_SUCCESS)
    {
        TRACE("Error: Read JEDEC basic parameter table.\r\n");
        return false;
    }

    /* 查看是否支持4K擦除(块/扇区擦除)，擦除命令是多少 */
    flash.sfdp.erase_4k_cmd = table[1];
    switch(table[0] & 0x03) 
    {
        /* 支持4K擦除 */
        case 1:
            flash.sfdp.erase_4k = true;
            break;
        
        /* 不支持4K擦除 */
        case 3:
            flash.sfdp.erase_4k = false;
            break;
        
        default:
            TRACE("Error: Uniform 4 KB erase supported information.\r\n");
            return false;
    }
    
    /* 获取写粒度 */
    switch((table[0] & (0x01 << 2)) >> 2) 
    {
        /* 写粒度为1字节 */
        case 0:
            flash.sfdp.write_gran = 1;
            break;
        
        /* 写粒度为64字节或更大(一般为页编程256字节) */
        case 1:
            flash.sfdp.write_gran = 256;
            break;
    }
    
    /* 易失性状态寄存器块保护位 */
    switch((table[0] & (0x01 << 3)) >> 3) 
    {
        /* 状态寄存器是非易失性的 */
        case 0:
            flash.sfdp.sr_is_non_vola = true;
            break;
        
        /* 状态寄存器是易失性的 */
        case 1:
            flash.sfdp.sr_is_non_vola = false;
            /* 写使能指令 */
            switch((table[0] & (0x01 << 4)) >> 4) 
            {
                /* 写使能指令为0x50 */
                case 0:
                    flash.sfdp.vola_sr_we_cmd = VOLATILE_SR_WRITE_ENABLE;
                    break;
                
                /* 写使能指令为0x06 */
                case 1:
                    flash.sfdp.vola_sr_we_cmd = CMD_WRITE_ENABLE;
                    break;
            }
            break;
    }
    
    /* 获取读取/写入/擦除时地址字节数 */
    switch((table[2] & (0x03 << 1)) >> 1) 
    {
        /* 3字节 */
        case 0:
            flash.sfdp.addr_3_byte = true;
            flash.sfdp.addr_4_byte = false;
            break;
        
        /* 3/4字节 */
        case 1:
            flash.sfdp.addr_3_byte = true;
            flash.sfdp.addr_4_byte = true;
            break;
        
        /* 4字节 */
        case 2:
            flash.sfdp.addr_3_byte = false;
            flash.sfdp.addr_4_byte = true;
            break;
        
        /* 错误 */
        default:
            flash.sfdp.addr_3_byte = false;
            flash.sfdp.addr_4_byte = false;
            TRACE("Error: Read address bytes.\r\n");
            return false;
    }
    
    /* 获取flash存储容量 */
    uint32_t table2_temp = (table[7] << 24) | (table[6] << 16) | (table[5] << 8) | table[4];
    switch ((table[7] & (0x01 << 7)) >> 7) 
    {
        case 0:
            flash.sfdp.capacity = 1 + (table2_temp >> 3);
            break;
        
        case 1:
            table2_temp &= 0x7FFFFFFF;
            if(table2_temp > sizeof(flash.sfdp.capacity) * 8 + 3) 
            {
                flash.sfdp.capacity = 0;
                TRACE("Error: Capacity is grater than 32 Gb/ 4 GB! Not Supported.\r\n");
                return false;
            }
            flash.sfdp.capacity = 1 << (table2_temp - 3);
            break;
    }
    
    /* 获取支持的擦除大小和命令 */
    for(i = 0, j = 0; i < SFDP_ERASE_TYPE_MAX_NUM; i++) 
    {
        if (table[28 + 2 * i] != 0x00) 
        {
            flash.sfdp.eraser[j].size = 1 << table[28 + 2 * i];
            flash.sfdp.eraser[j].cmd = table[28 + 2 * i + 1];
            j++;
        }
    }
    
    /* 按照擦除空间大小从小到大进行排序 */
    for(i = 0, j = 0; i < SFDP_ERASE_TYPE_MAX_NUM; i++) 
    {
        if(flash.sfdp.eraser[i].size) 
        {
            for(j = i + 1; j < SFDP_ERASE_TYPE_MAX_NUM; j++) 
            {
                if(flash.sfdp.eraser[j].size != 0 && flash.sfdp.eraser[i].size > flash.sfdp.eraser[j].size) 
                {
                    uint32_t temp_size = flash.sfdp.eraser[i].size;
                    uint8_t temp_cmd = flash.sfdp.eraser[i].cmd;
                    flash.sfdp.eraser[i].size = flash.sfdp.eraser[j].size;
                    flash.sfdp.eraser[i].cmd = flash.sfdp.eraser[j].cmd;
                    flash.sfdp.eraser[j].size = temp_size;
                    flash.sfdp.eraser[j].cmd = temp_cmd;
                }
            }
        }
    }

    flash.sfdp.available = true;
    return true;
}

/* 获取最合适的擦除方式 */
static size_t flash_sfdp_get_suitable_eraser(uint32_t addr, size_t erase_size)
{
    size_t index = SMALLEST_ERASER_INDEX, i;
    
    /* 检查参数合法性 */
    assert_param(flash.sfdp.available);
    
    /* 检查最小擦除方式是否地址对齐 */
    if(addr % flash.sfdp.eraser[SMALLEST_ERASER_INDEX].size)
        return SMALLEST_ERASER_INDEX;
    
    /* 找出擦除次数最少的擦除方式 */
    for(i = SFDP_ERASE_TYPE_MAX_NUM - 1; ; i--) 
    {
        if((flash.sfdp.eraser[i].size != 0) && 
             (erase_size >= flash.sfdp.eraser[i].size) && 
             (addr % flash.sfdp.eraser[i].size == 0)) 
        {
            index = i;
            break;
        }
        
        if(i == SMALLEST_ERASER_INDEX) 
            break;
    }
    
    return index;
}
#endif

/* Read-write SPI equipment */
static Flash_Err Spi_Write_Read(uint8_t *write_buf, size_t write_size, uint8_t *read_buf, size_t read_size)
{
    HAL_StatusTypeDef status;

    /* Determine the validity of parameters */
    if((write_size != 0 && write_buf == NULL) ||
         (read_size != 0 && read_buf == NULL))
        assert_param(0);
    
    /* Piece of selected */
    HAL_GPIO_WritePin(spi.cs_gpiox, spi.cs_gpio_pin, GPIO_PIN_RESET);
    
    if(write_size != 0)
    {
        status = HAL_SPI_Transmit(spi.spix, write_buf, write_size, 1000);
        if(status != HAL_OK)
        {
            HAL_GPIO_WritePin(spi.cs_gpiox, spi.cs_gpio_pin, GPIO_PIN_SET);
            return FLASH_ERR_TIMEOUT;
        }
    }

    if(read_size != 0)
    {
        status = HAL_SPI_Receive(spi.spix, read_buf, read_size, 1000);
        if(status != HAL_OK)
        {
            HAL_GPIO_WritePin(spi.cs_gpiox, spi.cs_gpio_pin, GPIO_PIN_SET);
            return FLASH_ERR_TIMEOUT;       
        }
    }
    
    HAL_GPIO_WritePin(spi.cs_gpiox, spi.cs_gpio_pin, GPIO_PIN_SET);
    
    return FLASH_SUCCESS;   
}

#endif /* (MEM_ENABLE && MEM_ENABLE_SPIFLASH) */
