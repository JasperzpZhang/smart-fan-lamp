#include "hyn_core.h"
#include "cst92xx_fw.h"

#ifdef chip_cst92xx

#define BOOT_I2C_ADDR   (0x5A)
#define MAIN_I2C_ADDR   (0x5A)

#define CST92XX_BIN_SIZE    (0x7F80)



#if HYN_TRANSFER_LIMIT
#define HYNITRON_PROGRAM_PAGE_SIZE (128)
#else
#define HYNITRON_PROGRAM_PAGE_SIZE (64)
#endif

static struct hyn_ts_data *hyn_92xxdata = NULL;

static int cst92xx_updata_judge(u8 *p_fw, u16 len);
static u32 cst92xx_read_checksum(void);
static int cst92xx_updata_tpinfo(void);
static int cst92xx_enter_boot(void);
static void cst92xx_rst(void);

static int cst92xx_init(struct hyn_ts_data* ts_data)
{
    int ret = 0;
    u8 buf[4];
    HYN_ENTER();
    hyn_92xxdata = ts_data;
    hyn_set_i2c_addr(hyn_92xxdata,BOOT_I2C_ADDR);
    ret = cst92xx_enter_boot();
    if(ret == FALSE){
        HYN_INFO("cst92xx_enter_boot failed");
        return FALSE;
    }
    hyn_92xxdata->fw_updata_addr = (u8*)fw_bin;
    hyn_92xxdata->fw_updata_len = CST92XX_BIN_SIZE;

    hyn_92xxdata->hw_info.ic_fw_checksum = cst92xx_read_checksum();
    if(hyn_92xxdata->need_updata_fw ==0){
        hyn_wr_reg(hyn_92xxdata,0xA004E8,3,buf,0); //exit boot
        cst92xx_rst();
        mdelay(50);
        hyn_set_i2c_addr(hyn_92xxdata,MAIN_I2C_ADDR);
        ret = cst92xx_updata_tpinfo();
        hyn_92xxdata->need_updata_fw = cst92xx_updata_judge((u8*)fw_bin,CST92XX_BIN_SIZE);
    }
    if(hyn_92xxdata->need_updata_fw){
        HYN_INFO("need updata FW !!!");
    }
    return TRUE;
}


static int  cst92xx_enter_boot(void)
{
    int ok = FALSE;
    uint8_t i2c_buf[4] = {0};

    for (uint8_t t = 10;; t += 2)
    {
        if (t >= 30){
            return FALSE;
        }

        cst92xx_rst();
        mdelay(t);

        ok = hyn_wr_reg(hyn_92xxdata, 0xA001AA, 3, i2c_buf, 0);
        if(ok == FALSE){
            continue;
        }

        mdelay(1);

        ok = hyn_wr_reg(hyn_92xxdata, 0xA002,  2, i2c_buf, 2);
        if(ok == FALSE){
            continue;
        }

        if ((i2c_buf[0] == 0x55) && (i2c_buf[1] == 0xB0)) {
            break;
        }
    }

    ok = hyn_wr_reg(hyn_92xxdata, 0xA00100, 3, i2c_buf, 0);
    if(ok == FALSE){
        return FALSE;
    }

    return TRUE;
}

static int erase_all_mem(void)
{
    int ok = FALSE;
    u8 i2c_buf[8];

	//erase_all_mem
    ok = hyn_wr_reg(hyn_92xxdata, 0xA0140000, 4, i2c_buf, 0);
    if (ok == FALSE){
        return FALSE;
    }
    ok = hyn_wr_reg(hyn_92xxdata, 0xA00C807F, 4, i2c_buf, 0);
    if (ok == FALSE){
        return FALSE;
    }
    ok = hyn_wr_reg(hyn_92xxdata, 0xA004EC, 3, i2c_buf, 0);
    if (ok == FALSE){
        return FALSE;
    }
        
    mdelay(300);
    for (uint16_t t = 0;; t += 10) {
        if (t >= 1000) {
           return FALSE;
        }

        mdelay(10);

        ok = hyn_wr_reg(hyn_92xxdata, 0xA005, 2, i2c_buf, 1);
        if (ok == FALSE) {
            continue;
        }

        if (i2c_buf[0] == 0x88) {
            break;
        }
    }

    return TRUE;
}




static bool write_mem_page(uint16_t addr, uint8_t *buf, uint16_t len)
{
    int ok = FALSE;
    uint8_t i2c_buf[1024+2] = {0};

    i2c_buf[0] = 0xA0;
    i2c_buf[1] = 0x0C;
    i2c_buf[2] = len;
    i2c_buf[3] = len >> 8;
    //ok = hyn_i2c_write_r16(HYN_BOOT_I2C_ADDR, 0xA00C, i2c_buf, 2);
    ok = hyn_write_data(hyn_92xxdata, i2c_buf, 4);
    if(ok == FALSE){
         return FALSE;
    }


    i2c_buf[0] = 0xA0;
    i2c_buf[1] = 0x14;
    i2c_buf[2] = addr;
    i2c_buf[3] = addr >> 8;
    ok = hyn_write_data(hyn_92xxdata, i2c_buf, 4);
    if(ok == FALSE) {
        return FALSE;
    }


    i2c_buf[0] = 0xA0;
    i2c_buf[1] = 0x18;
    memcpy(i2c_buf + 2, buf, len);            
    ok = hyn_write_data(hyn_92xxdata, i2c_buf, len+2);
    if(ok == FALSE){
        return FALSE;
    }


    ok =  hyn_wr_reg(hyn_92xxdata,0xA004EE,3,i2c_buf,0);
    if(ok == FALSE){
        return FALSE;
    }

    for (uint16_t t = 0;; t += 10) {
        if (t >= 1000) {
            return FALSE;
        }

        mdelay(5);

        ok =  hyn_wr_reg(hyn_92xxdata,0xA005,2,i2c_buf,1);
        if(ok == FALSE){
            continue;
        }        

        if (i2c_buf[0] == 0x55) {
            break;
        }
    }

    return TRUE;
}


static int write_code(u8 *bin_addr,uint8_t retry)
{
    uint8_t *data = (uint8_t *)bin_addr;
    uint16_t addr = 0;
    uint16_t remain_len = CST92XX_BIN_SIZE;
    retry = retry;

    while (remain_len > 0) {
        uint16_t cur_len = remain_len;
        if (cur_len > HYNITRON_PROGRAM_PAGE_SIZE) {
            cur_len = HYNITRON_PROGRAM_PAGE_SIZE;
        }
        if (write_mem_page(addr, data, cur_len) ==  FALSE) {
             return FALSE;
        }
        data += cur_len;
        addr += cur_len;
        remain_len -= cur_len;
    }
    return TRUE;
}


static uint32_t cst92xx_read_checksum(void)
{
    int ok = FALSE;
    uint8_t i2c_buf[4] = {0};
    uint32_t chip_checksum = 0;
    uint8_t retry = 5;

    ok =  hyn_wr_reg(hyn_92xxdata,0xA00300,3,i2c_buf,0);
    if (ok == FALSE) {
        return FALSE;
    }      
    mdelay(2);    
    while(retry--){
        mdelay(5);
        ok =  hyn_wr_reg(hyn_92xxdata,0xA000,2,i2c_buf,1);
        if (ok == FALSE) {
            continue;
        }
        if(i2c_buf[0]!=0) break;
    }

    mdelay(1);
     if(i2c_buf[0] == 0x01){
        memset(i2c_buf,0,sizeof(i2c_buf));
        ok =  hyn_wr_reg(hyn_92xxdata,0xA008,2,i2c_buf,4);
        if (ok == FALSE) {
            return FALSE;
        }      

        chip_checksum = ((uint32_t)(i2c_buf[0])) |
            (((uint32_t)(i2c_buf[1])) << 8) |
            (((uint32_t)(i2c_buf[2])) << 16) |
            (((uint32_t)(i2c_buf[3])) << 24);
    }
    else{
        hyn_92xxdata->need_updata_fw = 1;
    }

    return chip_checksum;
}


static int cst92xx_updata_fw(u8 *bin_addr, u16 len)
{ 
    #define CHECKSUM_OFFECT  (0x7F6C)
    int retry = 0;
    int ok_copy = TRUE;
    int ok = FALSE;
    u8 i2c_buf[4];

    u32 fw_checksum = U8TO16(bin_addr[CHECKSUM_OFFECT+3],bin_addr[CHECKSUM_OFFECT+2]);
    fw_checksum = (fw_checksum<<16)+U8TO16(bin_addr[CHECKSUM_OFFECT+1],bin_addr[CHECKSUM_OFFECT]);

    len = len;

    HYN_ENTER();
    hyn_irq_set(hyn_92xxdata,DISABLE);
    hyn_set_i2c_addr(hyn_92xxdata,BOOT_I2C_ADDR);

    for(retry = 1; retry<10; retry++){
        ok = cst92xx_enter_boot();
        if (ok == FALSE){
            continue;
        }

        ok = erase_all_mem();
        if (ok == FALSE){
            continue;
        }

        ok = write_code(bin_addr,retry);
        if (ok == FALSE){
            continue;
        }

        hyn_92xxdata->hw_info.ic_fw_checksum = cst92xx_read_checksum();
        if(fw_checksum != hyn_92xxdata->hw_info.ic_fw_checksum){
            HYN_INFO("out data fw checksum err:0x%04x",hyn_92xxdata->hw_info.ic_fw_checksum);
            continue;
        }
            
        if(retry>=5){
            ok_copy = FALSE;
            break;
        }
        break;
    }

    hyn_wr_reg(hyn_92xxdata,0xA006EE,3,i2c_buf,0); //exit boot
    mdelay(2);
    cst92xx_rst();
    mdelay(50);

    hyn_set_i2c_addr(hyn_92xxdata,MAIN_I2C_ADDR);   

    if(ok_copy == TRUE){
        cst92xx_updata_tpinfo();
        HYN_INFO("updata_fw success");
    }
    else{
        HYN_INFO("updata_fw failed");
    }

    hyn_irq_set(hyn_92xxdata,ENABLE);

    return ok_copy;
}




static int cst92xx_updata_tpinfo(void)
{
    u8 buf[30];
    struct tp_info *ic = &hyn_92xxdata->hw_info;
    int ret = 0;
    ret = hyn_wr_reg(hyn_92xxdata,0xD101,2,buf,0);
    if(ret == FALSE){
        return FALSE;
    }
    mdelay(5);

    //firmware_project_id   firmware_ic_type
    ret = hyn_wr_reg(hyn_92xxdata,0xD1F4,2,buf,28);
    if(ret == FALSE){
        return FALSE;
    }
    ic->fw_project_id = ((uint16_t)buf[17] <<8) + buf[16];
    ic->fw_chip_type = ((uint16_t)buf[19] <<8) + buf[18];

    //firmware_version
    ic->fw_ver = (buf[23]<<24)|(buf[22]<<16)|(buf[21]<<8)|buf[20];

    //tx_num   rx_num   key_num
    ic->fw_sensor_txnum = ((uint16_t)buf[1]<<8) + buf[0];
    ic->fw_sensor_rxnum = buf[2];
    ic->fw_key_num = buf[3];

    ic->fw_res_y = (buf[7]<<8)|buf[6];
    ic->fw_res_x = (buf[5]<<8)|buf[4];

    HYN_INFO("IC_info fw_project_id:%04x ictype:%04x fw_ver:%x checksum:%#x",ic->fw_project_id,ic->fw_chip_type,ic->fw_ver,ic->ic_fw_checksum);
    return TRUE;
}

static int cst92xx_updata_judge(u8 *p_fw, u16 len)
{
    u32 f_checksum,f_fw_ver,f_ictype,f_fw_project_id;
    u8 *p_data = p_fw + len - 28;   //7F64
    struct tp_info *ic = &hyn_92xxdata->hw_info;

    f_fw_project_id = U8TO16(p_data[1],p_data[0]);
    f_ictype = U8TO16(p_data[3],p_data[2]);

    f_fw_ver = U8TO16(p_data[7],p_data[6]);
    f_fw_ver = (f_fw_ver<<16)|U8TO16(p_data[5],p_data[4]);

    f_checksum = U8TO16(p_data[11],p_data[10]);
    f_checksum = (f_checksum << 16)|U8TO16(p_data[9],p_data[8]);


    HYN_INFO("Bin_info fw_project_id:%04x ictype:%04x fw_ver:%x checksum:%#x",f_fw_project_id,f_ictype,f_fw_ver,f_checksum);
    
    
    if(f_ictype != ic->fw_chip_type || f_fw_project_id != ic->fw_project_id){
        return 0; //not updata
    }
    if(f_checksum != ic->ic_fw_checksum && f_fw_ver > ic->fw_ver){
        return 1; //need updata
    }
    return 0;
}

//------------------------------------------------------------------------------//

static int cst92xx_set_workmode(enum work_mode mode,u8 enable)
{
    int ok = FALSE;
    uint8_t i2c_buf[4] = {0};
    uint8_t i = 0;

    if(hyn_92xxdata->work_mode != mode){
        hyn_92xxdata->work_mode = mode;

        for(i=0;i<3;i++)
        {
            ok = hyn_wr_reg(hyn_92xxdata,0xD106,2,i2c_buf,0);
            if (ok == FALSE) {
                continue;
            }

            ok = hyn_wr_reg(hyn_92xxdata,0xD10B,2,i2c_buf,0);
            if (ok == FALSE) {
                continue;
            }

            break;
        }    

        switch(mode){
            case NOMAL_MODE:
                hyn_irq_set(hyn_92xxdata,ENABLE);
                ok = hyn_wr_reg(hyn_92xxdata,0xD109,2,i2c_buf,0);
                if (ok == FALSE) {
                    return FALSE;
                }
                break;
            case GESTURE_MODE:
                break;
            case LP_MODE:
                break;
            case DIFF_MODE:
                ok = hyn_wr_reg(hyn_92xxdata,0xD10D,2,i2c_buf,0);
                if (ok == FALSE) {
                    return FALSE;
                }
                break;

            case RAWDATA_MODE:
                ok = hyn_wr_reg(hyn_92xxdata,0xD10A,2,i2c_buf,0);
                if (ok == FALSE) {
                    return FALSE;
                }
                break;
            case FAC_TEST_MODE:

                break;
            case DEEPSLEEP:
                hyn_irq_set(hyn_92xxdata,DISABLE);
                hyn_wr_reg(hyn_92xxdata,0xD105,2,i2c_buf,0);
                break;
            default :
                //hyn_esdcheck_switch(hyn_92xxdata,ENABLE);
                hyn_92xxdata->work_mode = NOMAL_MODE;
                break;
        }
    }
    return TRUE;
}

static void cst92xx_rst(void)
{
    gpio_set_value(hyn_92xxdata->plat_data.reset_gpio,0);
    msleep(5);
    gpio_set_value(hyn_92xxdata->plat_data.reset_gpio,1);
}



static int cst92xx_supend(void)
{
    HYN_ENTER();
    cst92xx_set_workmode(DEEPSLEEP,0);
    return 0;
}

static int cst92xx_resum(void)
{
    cst92xx_rst();
    msleep(50);
    cst92xx_set_workmode(NOMAL_MODE,0);
    return 0;
}


static int cst92xx_report(void)
{
    int ok = FALSE;
    uint8_t i2c_buf[MAX_POINTS_REPORT*5+5] = {0};
    uint8_t finger_num = 0;
    uint8_t key_state=0,key_id = 0;

    hyn_92xxdata->rp_buf.report_need = REPORT_NONE;

    ok = hyn_wr_reg(hyn_92xxdata,0xD000,2,i2c_buf,sizeof(i2c_buf));
    if (ok == FALSE){
        return FALSE;
    }   
        
    ok = hyn_wr_reg(hyn_92xxdata,0xD000AB,3,i2c_buf,0);
    if (ok == FALSE){
        return FALSE;
    }   


    if (i2c_buf[6] != 0xAB) {
        HYN_INFO("fail buf[6]=0x%02x",i2c_buf[6]);
        return FALSE;
    }

    finger_num = i2c_buf[5] & 0x7F;
    if (finger_num > MAX_POINTS_REPORT) {
        HYN_INFO("fail finger_num=%d",finger_num);
        return TRUE;
    }
    hyn_92xxdata->rp_buf.rep_num = finger_num;

   
    if ((i2c_buf[5] & 0x80) == 0x80) { // button
        uint8_t *data = i2c_buf + finger_num * 5;
        if (finger_num > 0) {
            data += 2;
        }
        key_state = data[0];//0x83:按键有触摸  0x80:按键抬起
        key_id = data[1]; // data[1]; :0x17  0x27  0x37

        if(key_state&0x80){            
            hyn_92xxdata->rp_buf.report_need |= REPORT_KEY;
            if((key_id == hyn_92xxdata->rp_buf.key_id || 0 == hyn_92xxdata->rp_buf.key_state)&& key_state == 0x83){
                hyn_92xxdata->rp_buf.key_id = key_id;
                hyn_92xxdata->rp_buf.key_state = 1;
            }
            else{
                hyn_92xxdata->rp_buf.key_state = 0;
            }
        }
    }
    else//pos
    {
        uint8_t index = 0;
        hyn_92xxdata->rp_buf.report_need |= REPORT_POS;
        if (finger_num > 0) {
            uint8_t *data = i2c_buf;
            uint8_t *data_ges = i2c_buf + finger_num * 5 + 2;
            uint8_t id = data[0] >> 4;
            uint8_t switch_ = data[0] & 0x0F;
            uint16_t x = ((uint16_t)(data[1]) << 4) | (data[3] >> 4);
            uint16_t y = ((uint16_t)(data[2]) << 4) | (data[3] & 0x0F);
            uint16_t z = (data[4] & 0x7F);

            if (id < MAX_POINTS_REPORT) {
                hyn_92xxdata->rp_buf.pos_id[index] = id;
                hyn_92xxdata->rp_buf.event[index] = (switch_ == 0x06) ? 1 : 0;  
                hyn_92xxdata->rp_buf.pos_x[index] = x;
                hyn_92xxdata->rp_buf.pos_y[index] = y;
                hyn_92xxdata->rp_buf.pres_z[index] = z;
                index++;
            }
            
            hyn_92xxdata->gesture_id = (data[4] & 0x80);// palm
            if(finger_num > 1) data_ges++;
            hyn_92xxdata->gesture_id |= data_ges[0];//LIGHT_SCREEN_GESTURE
        }

        for (uint8_t i = 1; i < finger_num; i++) {
            uint8_t *data = i2c_buf+5*i+2;
            uint8_t id = data[0] >> 4;
            uint8_t switch_ = data[0] & 0x0F;
            uint16_t x = ((uint16_t)(data[1]) << 4) | (data[3] >> 4);
            uint16_t y = ((uint16_t)(data[2]) << 4) | (data[3] & 0x0F);
            uint16_t z = (data[4] & 0x7F);

            if (id < MAX_POINTS_REPORT) {
                hyn_92xxdata->rp_buf.pos_id[index] = id;
                hyn_92xxdata->rp_buf.event[index] = (switch_ == 0x06) ? 1 : 0;  
                hyn_92xxdata->rp_buf.pos_x[index] = x;
                hyn_92xxdata->rp_buf.pos_y[index] = y;
                hyn_92xxdata->rp_buf.pres_z[index] = z;
                index++;
            }
        }
    }

    return TRUE;
}

static u32 cst92xx_check_esd(void)
{
    return TRUE;
}

static int cst92xx_prox_handle(u8 cmd)
{
    return TRUE;
}

static int cst92xx_get_rawdata(u8 *buf, u16 len)
{
    int ok = 0;   
    uint16_t node_num = 0;

    if (hyn_92xxdata->hw_info.fw_key_num == 0) {
        node_num = hyn_92xxdata->hw_info.fw_sensor_txnum * hyn_92xxdata->hw_info.fw_sensor_rxnum;
    } else {
        node_num = (hyn_92xxdata->hw_info.fw_sensor_txnum - 1) * hyn_92xxdata->hw_info.fw_sensor_rxnum + hyn_92xxdata->hw_info.fw_key_num;
    }

    ok = hyn_wr_reg(hyn_92xxdata,0x1000,2,buf,node_num*2);
    if (ok == FALSE) {
        return FALSE;
    }
        
    ok = hyn_wr_reg(hyn_92xxdata,0x000500,3,buf,0);
    if (ok == FALSE) {
        return FALSE;
    }

    return TRUE;
}

static int cst92xx_get_differ(u8 *buf, u16 len)
{
    int ok = 0;   
    uint16_t node_num = 0;

    if (hyn_92xxdata->hw_info.fw_key_num == 0) {
        node_num = hyn_92xxdata->hw_info.fw_sensor_txnum * hyn_92xxdata->hw_info.fw_sensor_rxnum;
    } else {
        node_num = (hyn_92xxdata->hw_info.fw_sensor_txnum - 1) * hyn_92xxdata->hw_info.fw_sensor_rxnum + hyn_92xxdata->hw_info.fw_key_num;
    }

    ok = hyn_wr_reg(hyn_92xxdata,0x1000,2,buf,node_num*2);
    if (ok == FALSE) {
        return FALSE;
    }
        
    ok = hyn_wr_reg(hyn_92xxdata,0x000500,3,buf,0);
    if (ok == FALSE) {
        return FALSE;
    }

    return TRUE;
}

static int cst92xx_get_test_result(u8 *buf, u16 len)
{
    return 0;
}



const struct hyn_ts_fuc cst92xx_fuc = {
    .tp_rest = cst92xx_rst,
    .tp_report = cst92xx_report,
    .tp_supend = cst92xx_supend,
    .tp_resum = cst92xx_resum,
    .tp_chip_init = cst92xx_init,
    .tp_updata_fw = cst92xx_updata_fw,
    .tp_set_workmode = cst92xx_set_workmode,
    .tp_check_esd = cst92xx_check_esd,
    .tp_prox_handle = cst92xx_prox_handle,
    .tp_get_rawdata = cst92xx_get_rawdata,
    .tp_get_differ = cst92xx_get_differ,
    .tp_get_test_result = cst92xx_get_test_result
};

#endif

