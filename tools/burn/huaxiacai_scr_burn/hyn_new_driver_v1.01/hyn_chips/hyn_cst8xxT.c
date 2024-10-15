#include "hyn_core.h"
#include "cst8xxT_fw.h"

#ifdef chip_cst8xxT

#define CUSTOM_SENSOR_NUM  	(10)
#define CUSTOM_KEY_NUM  	(0)
#define CUSTOM_RES_X  	    (240)
#define CUSTOM_RES_Y  	    (240)

#define BOOT_I2C_ADDR   (0x6A)
#define MAIN_I2C_ADDR   (0x15)

#define CST8xxT_BIN_SIZE    (15*1024)
static struct hyn_ts_data *hyn_8xxTdata = NULL;

static int cst8xxT_updata_judge(u8 *p_fw, u16 len);
static u32 cst8xxT_read_checksum(void);
static int cst8xxT_updata_tpinfo(void);
static int cst8xxT_enter_boot(void);
static void cst8xxT_rst(void);

static int cst8xxT_init(struct hyn_ts_data* ts_data)
{
    int ret = 0;
    u8 buf[4];
    HYN_ENTER();
    hyn_8xxTdata = ts_data;
    hyn_set_i2c_addr(hyn_8xxTdata,BOOT_I2C_ADDR);
    ret = cst8xxT_enter_boot();
    if(ret == FALSE){
        HYN_INFO("cst8xxT_enter_boot failed");
        return FALSE;
    }
    hyn_8xxTdata->fw_updata_addr = (u8*)fw_bin;
    hyn_8xxTdata->fw_updata_len = CST8xxT_BIN_SIZE;

    hyn_8xxTdata->hw_info.ic_fw_checksum = cst8xxT_read_checksum();
    if(hyn_8xxTdata->need_updata_fw ==0){
        hyn_wr_reg(hyn_8xxTdata,0xA006EE,3,buf,0); //exit boot
        cst8xxT_rst();
        mdelay(50);
        hyn_set_i2c_addr(hyn_8xxTdata,MAIN_I2C_ADDR);
        ret = cst8xxT_updata_tpinfo();
        hyn_8xxTdata->need_updata_fw = cst8xxT_updata_judge((u8*)fw_bin,CST8xxT_BIN_SIZE);
    }
    if(hyn_8xxTdata->need_updata_fw){
        HYN_INFO("need updata FW !!!");
    }
    return TRUE;
}


static int  cst8xxT_enter_boot(void)
{
    for (uint8_t t = 5;; t += 2)
    {
        int ok = FALSE;
        uint8_t i2c_buf[4] = {0};

        if (t >= 15){
            return FALSE;
        }

        cst8xxT_rst();
        mdelay(5+t);

        ok = hyn_wr_reg(hyn_8xxTdata, 0xA001AB, 3, i2c_buf, 0);
        if(ok == FALSE){
            continue;
        }

        ok = hyn_wr_reg(hyn_8xxTdata, 0xA003,  2, i2c_buf, 1);
        if(ok == FALSE){
            continue;
        }

        if (i2c_buf[0] != 0xC1){
            continue;
        }
        break;
    }
    return TRUE;
}


static int write_code(u8 *bin_addr,uint8_t retry)
{
    uint16_t i;//,j;
    int ok = FALSE;
    u8 i2c_buf[512+2];
	
	bin_addr+=6;
	
    for ( i = 0;i < CST8xxT_BIN_SIZE; i += 512)
    {
        i2c_buf[0] = 0xA0;
        i2c_buf[1] = 0x14;
        i2c_buf[2] = i;
        i2c_buf[3] = i >> 8;
        ok = hyn_write_data(hyn_8xxTdata, i2c_buf, 4);
        if (ok == FALSE){
            break;
        }

#if HYN_TRANSFER_LIMIT
        for(j=0;j<16;j++)//512字节分成16组，每组32字节
        {
            i2c_buf[0] = ((0xA018 + (j*32)) >> 8);
            i2c_buf[1] = ((0xA018 + (j*32)) & 0x00ff);
            memcpy(i2c_buf + 2, bin_addr + i + (j*32), 32);            
            ok = hyn_write_data(hyn_8xxTdata, i2c_buf, 34);
            if (ok == FALSE){
                break;
            }            
        }
/*
        for(j=0;j<4;j++)//512字节分成4组，每组128字节
        {
            i2c_buf[0] = ((0xA018 + (j*128)) >> 8);
            i2c_buf[1] = ((0xA018 + (j*128)) & 0x00ff);
            memcpy(i2c_buf + 2, bin_addr + i + (j*128), 128);            
            ok = hyn_write_data(hyn_8xxTdata, i2c_buf, 130);
            if (ok == FALSE){
                break;
            }            
        }*/
#else
        i2c_buf[0] = 0xA0;
        i2c_buf[1] = 0x18;
        memcpy(i2c_buf + 2, bin_addr + i, 512);            
        ok = hyn_write_data(hyn_8xxTdata, i2c_buf, 514);
        if (ok == FALSE){
            break;
        }
#endif

        ok = hyn_wr_reg(hyn_8xxTdata, 0xA004EE, 3,i2c_buf,0);
        if (ok == FALSE){
            break;
        }

        mdelay(100 * retry);

        for (uint16_t t = 0;; t ++)
        {
            if (t >= 50){
                return FALSE;
            }
            mdelay(5);

            ok = hyn_wr_reg(hyn_8xxTdata,0xA005,2,i2c_buf,1);
            if (ok == FALSE){
                continue;
            }
            if (i2c_buf[0] != 0x55){
                continue;
            }
            break;
        }
    }
    return ok;
}


static uint32_t cst8xxT_read_checksum(void)
{
    int ok = FALSE;
    uint8_t i2c_buf[4] = {0};
    uint32_t value = 0;
    int chip_checksum_ok = FALSE;
    // firmware checksum
    ok = hyn_wr_reg(hyn_8xxTdata, 0xA00300,  3, i2c_buf, 0);
    if (ok == FALSE){
        return value;
    }

    mdelay(100);

    for (uint16_t t = 0;; t += 10)
    {
        if (t >= 1000){
            //return FALSE;
            break;
        }

        mdelay(10);

        ok = hyn_wr_reg(hyn_8xxTdata, 0xA000,  2, i2c_buf, 1);
        if (ok == FALSE){
            continue;
        }

        if (i2c_buf[0] == 1){
            chip_checksum_ok = TRUE;
            break;
        }
        else if (i2c_buf[0] == 2){
            chip_checksum_ok = FALSE;
            continue;
        }
    }

    if(chip_checksum_ok == FALSE){
        hyn_8xxTdata->need_updata_fw = 1;
    }
    else{
        ok = hyn_wr_reg(hyn_8xxTdata, 0xA008,  2, i2c_buf, 2);
        if (ok == FALSE){
            //return FALSE;
            return value;
        }
        value = i2c_buf[0];
        value |= (uint16_t)(i2c_buf[1]) << 8;
    }

    return value;
}


static int cst8xxT_updata_fw(u8 *bin_addr, u16 len)
{ 
    int retry = 0;
    int ok_copy = TRUE;
    int ok = FALSE;
    u8 i2c_buf[4];

    u32 fw_checksum = U8TO16(bin_addr[5],bin_addr[4]);
    len = len;

    HYN_ENTER();
    hyn_irq_set(hyn_8xxTdata,DISABLE);
    hyn_set_i2c_addr(hyn_8xxTdata,BOOT_I2C_ADDR);

    for(retry = 1; retry<10; retry++){
        ok = cst8xxT_enter_boot();
        if (ok == FALSE){
            continue;
        }

        ok = write_code(bin_addr,retry);
        if (ok == FALSE){
            continue;
        }

        hyn_8xxTdata->hw_info.ic_fw_checksum = cst8xxT_read_checksum();
        if(fw_checksum != hyn_8xxTdata->hw_info.ic_fw_checksum){
            continue;
        }
            
        if(retry>=5){
            ok_copy = FALSE;
            break;
        }
        break;
    }

    hyn_wr_reg(hyn_8xxTdata,0xA006EE,3,i2c_buf,0); //exit boot
    mdelay(2);
    cst8xxT_rst();
    mdelay(50);

    hyn_set_i2c_addr(hyn_8xxTdata,MAIN_I2C_ADDR);   

    if(ok_copy == TRUE){
        cst8xxT_updata_tpinfo();
        HYN_INFO("updata_fw success");
    }
    else{
        HYN_INFO("updata_fw failed");
    }

    hyn_irq_set(hyn_8xxTdata,ENABLE);

    return ok_copy;
}




static int cst8xxT_updata_tpinfo(void)
{
    u8 buf[8];
    struct tp_info *ic = &hyn_8xxTdata->hw_info;
    int ret = 0;

    ret = hyn_wr_reg(hyn_8xxTdata,0xA7,1,buf,4);
    if(ret == FALSE){
        HYN_ERROR("cst8xxT_updata_tpinfo failed");
        return FALSE;
    }

    ic->fw_sensor_txnum = CUSTOM_SENSOR_NUM;
    ic->fw_sensor_rxnum = 0;
    ic->fw_key_num = CUSTOM_KEY_NUM;
    ic->fw_res_y = CUSTOM_RES_Y;
    ic->fw_res_x = CUSTOM_RES_X;
    ic->fw_project_id = buf[1];
    ic->fw_chip_type = buf[0];
    ic->fw_ver = buf[2];

    HYN_INFO("IC_info fw_project_id:%04x ictype:%04x fw_ver:%x checksum:%#x",ic->fw_project_id,ic->fw_chip_type,ic->fw_ver,ic->ic_fw_checksum);
    return TRUE;
}

static int cst8xxT_updata_judge(u8 *p_fw, u16 len)
{
    u32 f_checksum,f_fw_ver,f_ictype,f_fw_project_id;
    u8 *p_data = p_fw ; 
    struct tp_info *ic = &hyn_8xxTdata->hw_info;

    f_checksum = U8TO16(p_data[5],p_data[4]);

    p_data = p_fw + 6 + CST8xxT_BIN_SIZE - 1 - 15 ; 

    f_ictype = p_data[0];
    f_fw_project_id = (p_data[1]<<24) + (p_data[2]<<16) + (p_data[3]<<8) + (p_data[4]<<0);
    f_fw_ver = p_data[5];

    HYN_INFO("Bin_info fw_project_id:%04x ictype:%04x fw_ver:%x checksum:%#x",f_fw_project_id,f_ictype,f_fw_ver,f_checksum);

    if(f_checksum != ic->ic_fw_checksum ){
        return 1; //need updata
    }
    return 0;
}

//------------------------------------------------------------------------------//

static int cst8xxT_set_workmode(enum work_mode mode,u8 enable)
{
    if(hyn_8xxTdata->work_mode != mode){
        hyn_8xxTdata->work_mode = mode;

        switch(mode){
            case NOMAL_MODE:
                hyn_irq_set(hyn_8xxTdata,ENABLE);
                //hyn_esdcheck_switch(hyn_8xxTdata,ENABLE);
                //hyn_wr_reg(hyn_8xxTdata,0xD100,2,NULL,0);
                hyn_wr_reg(hyn_8xxTdata,0xFE00,2,NULL,0);
                break;
            case GESTURE_MODE:
                //hyn_wr_reg(hyn_8xxTdata,0xD04C80,3,NULL,0);
                break;
            case LP_MODE:
                break;
            case DIFF_MODE:
            case RAWDATA_MODE:
                hyn_wr_reg(hyn_8xxTdata,0xFEF8,2,NULL,0);
                break;
            case FAC_TEST_MODE:
                //hyn_wr_reg(hyn_8xxTdata,0xD119,2,NULL,0);
                break;
            case DEEPSLEEP:
                hyn_irq_set(hyn_8xxTdata,DISABLE);
                hyn_wr_reg(hyn_8xxTdata,0xE503,2,NULL,0);
                break;
            default :
                //hyn_esdcheck_switch(hyn_8xxTdata,ENABLE);
                hyn_8xxTdata->work_mode = NOMAL_MODE;
                break;
        }
    }
    return 0;
}

static void cst8xxT_rst(void)
{
    gpio_set_value(hyn_8xxTdata->plat_data.reset_gpio,0);
    msleep(5);
    gpio_set_value(hyn_8xxTdata->plat_data.reset_gpio,1);
}



static int cst8xxT_supend(void)
{
    HYN_ENTER();
    cst8xxT_set_workmode(DEEPSLEEP,0);
    return 0;
}

static int cst8xxT_resum(void)
{
    cst8xxT_rst();
    msleep(50);
    cst8xxT_set_workmode(NOMAL_MODE,0);
    return 0;
}


static int cst8xxT_report(void)
{
    int ok = FALSE;
    uint8_t i = 0;
    uint8_t i2c_buf[3+6*MAX_POINTS_REPORT] = {0};
    uint16_t x,y;
    uint8_t id = 0,index = 0;
    struct hyn_plat_data *dt = &hyn_8xxTdata->plat_data;

    memset(&hyn_8xxTdata->rp_buf,0,sizeof(hyn_8xxTdata->rp_buf));
    hyn_8xxTdata->rp_buf.report_need = REPORT_NONE;

    ok = hyn_wr_reg(hyn_8xxTdata,0x00,1,i2c_buf,(3+6*MAX_POINTS_REPORT));
    if (ok == FALSE){
        return FALSE;
    }   

    hyn_8xxTdata->gesture_id  = i2c_buf[1];
    hyn_8xxTdata->rp_buf.rep_num  = i2c_buf[2];
    // HYN_INFO("rep_num = %d",hyn_8xxTdata->rp_buf.rep_num);
    for(i = 0 ; i < 2 ; i++)
    {
        id = (i2c_buf[5 + i*6] & 0xf0)>>4;
        if(id > 1) continue;

        x = (i2c_buf[3 + i*6] & 0x0f);
        x = (x<<8) + i2c_buf[4 + i*6];

        y = (i2c_buf[5 + i*6] & 0x0f);
        y = (y<<8) + i2c_buf[6 + i*6];

        hyn_8xxTdata->rp_buf.pos_id[index] = id;
        hyn_8xxTdata->rp_buf.event[index] = (i2c_buf[3 + i*6] & 0x40) ? 0:1;
        hyn_8xxTdata->rp_buf.pos_x[index] = x ;
        hyn_8xxTdata->rp_buf.pos_y[index] = y ;
        hyn_8xxTdata->rp_buf.pres_z[index] = (i2c_buf[7 + i*6] <<8) + i2c_buf[8 + i*6] ;
        hyn_8xxTdata->rp_buf.pres_z[index] += (x&0x03); //press mast chang
        index++;
    }
    if(index != 0) hyn_8xxTdata->rp_buf.report_need = REPORT_POS;
    if(dt->key_num){
        i = dt->key_num;
        while(i){
            i--;
            if(dt->key_y_coords ==hyn_8xxTdata->rp_buf.pos_y[0] && dt->key_x_coords[i] == hyn_8xxTdata->rp_buf.pos_x[0]){
                hyn_8xxTdata->rp_buf.key_id = i;
                hyn_8xxTdata->rp_buf.key_state = hyn_8xxTdata->rp_buf.event[0];
                hyn_8xxTdata->rp_buf.report_need = REPORT_KEY;
            }
        }
    }
    return TRUE;
}

static u32 cst8xxT_check_esd(void)
{
    return TRUE;
}

static int cst8xxT_prox_handle(u8 cmd)
{
    return TRUE;
}

static int cst8xxT_get_rawdata(u8 *buf, u16 len)
{
    int ok = 0;   
    ok = hyn_wr_reg(hyn_8xxTdata, 0x41, 1,buf,hyn_8xxTdata->hw_info.fw_sensor_txnum*2);
    if (ok == FALSE) {
        return FALSE;
    }
    return TRUE;
}

static int cst8xxT_get_differ(u8 *buf, u16 len)
{
    int ok = 0;
    ok = hyn_wr_reg(hyn_8xxTdata, 0x61, 1,buf,hyn_8xxTdata->hw_info.fw_sensor_txnum*2); 
    if (ok == FALSE) {
        return FALSE;
    }
    return TRUE;
}

static int cst8xxT_get_test_result(u8 *buf, u16 len)
{
    return 0;
}

const struct hyn_ts_fuc cst8xxT_fuc = {
    .tp_rest = cst8xxT_rst,
    .tp_report = cst8xxT_report,
    .tp_supend = cst8xxT_supend,
    .tp_resum = cst8xxT_resum,
    .tp_chip_init = cst8xxT_init,
    .tp_updata_fw = cst8xxT_updata_fw,
    .tp_set_workmode = cst8xxT_set_workmode,
    .tp_check_esd = cst8xxT_check_esd,
    .tp_prox_handle = cst8xxT_prox_handle,
    .tp_get_rawdata = cst8xxT_get_rawdata,
    .tp_get_differ = cst8xxT_get_differ,
    .tp_get_test_result = cst8xxT_get_test_result
};

#endif

