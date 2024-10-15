#include "hyn_core.h"
#include "cst3240_fw.h"

#ifdef chip_cst3240

#define BOOT_I2C_ADDR   (0x5A)
#define MAIN_I2C_ADDR   (0x5A)

#define CST3240_BIN_SIZE   (31*512 + 480)

static struct hyn_ts_data *hyn_3240data = NULL;
static const u8 gest_map_tbl[33] = {0xff,4,1,3,2,5,12,6,7,7,9,11,10,13,12,7,7,6,10,6,5,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,14};

static int cst3240_updata_judge(u8 *p_fw, u16 len);
static u32 cst3240_read_checksum(void);
static int cst3240_updata_tpinfo(void);
static int cst3240_enter_boot(void);
static void cst3240_rst(void);

static int cst3240_init(struct hyn_ts_data* ts_data)
{
    int ret = 0;
    HYN_ENTER();
    hyn_3240data = ts_data;
    hyn_set_i2c_addr(hyn_3240data,BOOT_I2C_ADDR);
    ret = cst3240_enter_boot();
    if(ret){
        HYN_INFO("cst3240_enter_boot failed");
        return -1;
    }
    hyn_3240data->fw_updata_addr = (u8*)fw_bin;
    hyn_3240data->fw_updata_len = CST3240_BIN_SIZE;
    hyn_3240data->hw_info.ic_fw_checksum = cst3240_read_checksum();
    if(hyn_3240data->need_updata_fw ==0){
        //hyn_wr_reg(hyn_3240data,0xA006EE,3,buf,0); //exit boot
        cst3240_rst();
        mdelay(50);
        hyn_set_i2c_addr(hyn_3240data,MAIN_I2C_ADDR);
        ret = cst3240_updata_tpinfo();
        hyn_3240data->need_updata_fw = cst3240_updata_judge((u8*)fw_bin,CST3240_BIN_SIZE);
    }
    if(hyn_3240data->need_updata_fw){
        HYN_INFO("need updata FW !!!");
    }
    return 0;
}

static int cst3240_report(void)
{
    u8 buf[80]={0};
    u8 finger_num = 0,len = 0,write_tail_end = 0,key_state=0,key_id = 0,tmp_dat;
    struct hyn_plat_data *dt = &hyn_3240data->plat_data;
    int ret = 0;
    hyn_3240data->rp_buf.report_need = REPORT_NONE;

    switch(hyn_3240data->work_mode){
        case NOMAL_MODE:
            write_tail_end = 1;
            ret = hyn_wr_reg(hyn_3240data,0xD000,2,buf,7);
            if(ret || buf[6] != 0xAB || buf[0] == 0xAB){
                break;
            }
            len = buf[5] & 0x7F;
            if(len > dt->max_touch_num){
                len = dt->max_touch_num;
            }

            if(buf[5]&0x80){ //key
                if(buf[5]==0x80){
                    key_id = (buf[1]>>4)-1;
                    key_state = buf[0];
                }
                else{
                    finger_num = len;
                    len = (len-1)*5+3;
                    ret = hyn_wr_reg(hyn_3240data,0xD007,2,&buf[5],len);
                    len += 5;
                    key_id = (buf[len-2]>>4)-1;
                    key_state = buf[len-3];
                }
                if(key_state&0x80){
                    hyn_3240data->rp_buf.report_need |= REPORT_KEY;
                    if((key_id == hyn_3240data->rp_buf.key_id || 0 == hyn_3240data->rp_buf.key_state)&& key_state == 0x83){
                        hyn_3240data->rp_buf.key_id = key_id;
                        hyn_3240data->rp_buf.key_state = 1;
                    }
                    else{
                        hyn_3240data->rp_buf.key_state = 0;
                    }
                }
            }
            else{ //pos
                u16 index = 0;
                u8 i = 0;
                finger_num = len;
                hyn_3240data->rp_buf.report_need |= REPORT_POS;
                if(finger_num > 1){
                    len = (len-1)*5 + 1;
                    ret = hyn_wr_reg(hyn_3240data,0xD007,2,&buf[5],len);
                }
                hyn_3240data->rp_buf.rep_num = finger_num;
                for(i = 0; i < finger_num; i++){
                    index = i*5;
                    hyn_3240data->rp_buf.pos_id[i] =  (buf[index]>>4)&0x0F;
                    hyn_3240data->rp_buf.event[i] =  (buf[index]&0x0F) == 0x06 ? 1 : 0;
                    hyn_3240data->rp_buf.pos_x[i] = ((u16)buf[index + 1]<<4) + ((buf[index + 3] >> 4) & 0x0F);
                    hyn_3240data->rp_buf.pos_y[i] = ((u16)buf[index + 2]<<4) + (buf[index + 3] & 0x0F);
                    hyn_3240data->rp_buf.pres_z[i] = buf[index + 4];
                    
                    HYN_INFO("report_id = %d, xy = %d,%d",hyn_3240data->rp_buf.pos_id[i],hyn_3240data->rp_buf.pos_x[i],hyn_3240data->rp_buf.pos_y[i]);
                }
            }
            break;
        case GESTURE_MODE:
            ret = hyn_wr_reg(hyn_3240data,0xD04C,2,&tmp_dat,1);
            if((tmp_dat&0x7F) <= 32){
                tmp_dat = tmp_dat&0x7F;
                hyn_3240data->gesture_id = gest_map_tbl[tmp_dat];
                hyn_3240data->rp_buf.report_need |= REPORT_GES;
            }
            break;
        default:
            break;
    }
    if(write_tail_end){
        hyn_wr_reg(hyn_3240data,0xD000AB,3,buf,0);
    }
    return 0;
}

static int cst3240_prox_handle(u8 cmd)
{
    /*int ret = 0;
    switch(cmd){
        case 1: //enable
            hyn_3240data->prox_mode_en = 1;
            hyn_3240data->prox_state = 0;
            ret = hyn_wr_reg(hyn_3240data,0xD004B01,3,NULL,0);
            break;
        case 0: //disable
            hyn_3240data->prox_mode_en = 0;
            hyn_3240data->prox_state = 0;
            ret = hyn_wr_reg(hyn_3240data,0xD004B00,3,NULL,0);
            break;
        case 2: //read
            ret = hyn_wr_reg(hyn_3240data,0xD004B,2,&hyn_3240data->prox_state,1);
            break;
    }
    return ret;*/
    return 0;
}

static int cst3240_set_workmode(enum work_mode mode,u8 enable)
{
    if(hyn_3240data->work_mode != mode){
        hyn_3240data->work_mode = mode;
        if(mode != NOMAL_MODE)
            hyn_esdcheck_switch(hyn_3240data,DISABLE);
        switch(mode){
            case NOMAL_MODE:
                hyn_irq_set(hyn_3240data,ENABLE);
                hyn_esdcheck_switch(hyn_3240data,ENABLE);
                hyn_wr_reg(hyn_3240data,0xD109,2,NULL,0);
                break;

            case GESTURE_MODE:
                hyn_wr_reg(hyn_3240data,0xD104,2,NULL,0);
                break;

            case LP_MODE:
                break;
            case DIFF_MODE:
                hyn_wr_reg(hyn_3240data,0xD10D,2,NULL,0);
                break;
            case RAWDATA_MODE:
                hyn_wr_reg(hyn_3240data,0xD10A,2,NULL,0);
                break;
            case FAC_TEST_MODE:
                hyn_wr_reg(hyn_3240data,0xD119,2,NULL,0);
                break;


            case DEEPSLEEP:
                hyn_irq_set(hyn_3240data,DISABLE);
                hyn_wr_reg(hyn_3240data,0xD105,2,NULL,0);
                break;
            default :
                hyn_esdcheck_switch(hyn_3240data,ENABLE);
                hyn_3240data->work_mode = NOMAL_MODE;
                break;
        }
    }
    return 0;
}


static int cst3240_supend(void)
{
    HYN_ENTER();
    cst3240_set_workmode(DEEPSLEEP,0);
    return 0;
}

static int cst3240_resum(void)
{
    HYN_ENTER();
    cst3240_rst();
    msleep(50);
    cst3240_set_workmode(NOMAL_MODE,0);
    return 0;
}

static void cst3240_rst(void)
{
    gpio_set_value(hyn_3240data->plat_data.reset_gpio,0);
    msleep(5);
    gpio_set_value(hyn_3240data->plat_data.reset_gpio,1);
}

static int cst3240_enter_boot(void)
{
    int retry = 0,ret = 0;//, erro_cnt = 0;
    u8 buf[4];
    while(++retry<20){
        cst3240_rst();
        mdelay(5+retry);
        ret = hyn_wr_reg(hyn_3240data,0xA001AA,3,buf,0);
        if(ret < 0){
            continue;
        }
        ret = hyn_wr_reg(hyn_3240data,0xA002,2,buf,1);
        if(ret < 0){
            continue;
        }
        if(buf[0] != 0x55 ){ //buf[0] != 0xAC
            continue;
        }

        mdelay(2);
        ret = hyn_wr_reg(hyn_3240data,0xA003,2,buf,1);
        if(buf[0] != 0xA8)continue;

        ret = hyn_wr_reg(hyn_3240data,0xA00100,3,buf,0); 
        if(ret) continue;

        break;
    }

    if(retry >=20)return -1;

    return 0;
}

static int cst3240_updata_tpinfo(void)
{
    u8 buf[28];
    u32 chip_ic_checkcode =0;
    struct tp_info *ic = &hyn_3240data->hw_info;
    int ret = 0;
    ret = hyn_wr_reg(hyn_3240data,0xD101,2,buf,0);
    mdelay(1);
    ret |= hyn_wr_reg(hyn_3240data,0xD1FC,2,buf,4);

    chip_ic_checkcode = (buf[3]<<24)|(buf[2]<<16)|(buf[1]<<8)|buf[0];

    if((ret ||(chip_ic_checkcode & 0xffff0000)!=0xCACA0000)){
        HYN_ERROR("cst3240_updata_tpinfo failed");
        return -1;
    }

    mdelay(10);

    ret = hyn_wr_reg(hyn_3240data,0xD204,2,buf,4);
    if(ret){
        return -1;
    }
    ic->fw_chip_type = (buf[3]<<8)|buf[2];
    ic->fw_project_id = (buf[1]<<8)|buf[0];

    ret = hyn_wr_reg(hyn_3240data,0xD208,2,buf,4);
    if(ret){
        return -1;
    }
    ic->fw_ver = (buf[3]<<24)|(buf[2]<<16)|(buf[1]<<8)|buf[0];


    ret = hyn_wr_reg(hyn_3240data,0xD1F4,2,buf,8);
    if(ret){
        return -1;
    }

    ic->fw_sensor_txnum = buf[0];
    ic->fw_sensor_rxnum = buf[2];
    ic->fw_key_num = buf[3];
    ic->fw_res_y = (buf[7]<<8)|buf[6];
    ic->fw_res_x = (buf[5]<<8)|buf[4];


    HYN_INFO("IC_info fw_project_id:%04x ictype:%04x fw_ver:%x checksum:%#x",ic->fw_project_id,ic->fw_chip_type,ic->fw_ver,ic->ic_fw_checksum);
    return 0;
}

static u32 cst3240_read_checksum(void)
{
    int ret;
    u8 buf[4],retry = 5;
    while(retry--){
        ret = hyn_wr_reg(hyn_3240data,0xA000,2,buf,1);
        if(ret){
			mdelay(2);
			continue;
        }
        if(buf[0]!=0) break;
        mdelay(2);
    }
    mdelay(1);
    if(buf[0] == 0x01){
        memset(buf,0,sizeof(buf));
        ret = hyn_wr_reg(hyn_3240data,0xA008,2,buf,4);
    }
    else{
        hyn_3240data->need_updata_fw = 1;
    }
    return buf[0] + (buf[1]<<8) + (buf[2]<<16) + (buf[3]<<24);
}

static int cst3240_updata_judge(u8 *p_fw, u16 len)
{
    u32 f_checksum,f_fw_ver,f_ictype,f_fw_project_id;
    u8 *p_data = p_fw + len- 16; 

    struct tp_info *ic = &hyn_3240data->hw_info;

	p_data = &p_fw[CST3240_BIN_SIZE-16+4];
    f_fw_project_id = U8TO16(p_data[1],p_data[0]);
    f_ictype = U8TO16(p_data[3],p_data[2]);
    f_fw_ver = U8TO16(p_data[7],p_data[6]);
    f_fw_ver = (f_fw_ver<<16)|U8TO16(p_data[5],p_data[4]);

    f_checksum = U8TO16(p_data[11],p_data[10]);
    f_checksum = (f_checksum<<16)|U8TO16(p_data[9],p_data[8]);

    HYN_INFO("Bin_info fw_project_id:%04x ictype:%04x fw_ver:%x checksum:%#x",f_fw_project_id,f_ictype,f_fw_ver,f_checksum);
   
    if(f_ictype != ic->fw_chip_type || f_fw_project_id != ic->fw_project_id){
        return 0; //not updata
    }
    if(f_checksum != ic->ic_fw_checksum && f_fw_ver > ic->fw_ver){
        return 1; //need updata
    }
    return 0;
}

static int cst3240_updata_fw(u8 *bin_addr, u16 len)
{
    int i,ret, k,retry = 4;
	u8 i2c_buf[512+2];
	u16 eep_addr = 0, total_kbyte = 31;
    u32 fw_checksum = 0;

    u8  *pData = bin_addr +31*512+476;   
    fw_checksum = pData[0] + (pData[1]<<8) + (pData[2]<<16) + (pData[3]<<24);


#if HYN_TRANSFER_LIMIT
	unsigned char temp_buf[8];
	unsigned short iic_addr;
	int  j;
#endif
    HYN_ENTER();
    hyn_irq_set(hyn_3240data,DISABLE);
    hyn_set_i2c_addr(hyn_3240data,BOOT_I2C_ADDR);

    while(--retry){
        ret = cst3240_enter_boot();
        if(ret){
            HYN_INFO("cst3240_enter_boot faill");
            continue;
        }

        ret = hyn_wr_reg(hyn_3240data,0xA01000,3,i2c_buf,0); 
        if(ret) continue;

        //start trans fw
        for (i=0; i<total_kbyte; i++) {
            i2c_buf[0] = 0xA0;
            i2c_buf[1] = 0x14;
            eep_addr = i << 9;		//i * 512
            i2c_buf[2] = eep_addr;
            i2c_buf[3] = eep_addr>>8;
            ret = hyn_write_data(hyn_3240data, i2c_buf, 4);

#if HYN_TRANSFER_LIMIT
            memcpy(i2c_buf, bin_addr + eep_addr, 512);
            for(j=0; j<128; j++) {
                iic_addr = (j<<2);
                temp_buf[0] = (iic_addr+0xA018)>>8;
                temp_buf[1] = (iic_addr+0xA018)&0xFF;
                temp_buf[2] = i2c_buf[iic_addr+0];
                temp_buf[3] = i2c_buf[iic_addr+1];
                temp_buf[4] = i2c_buf[iic_addr+2];
                temp_buf[5] = i2c_buf[iic_addr+3];
                ret =   hyn_write_data(hyn_3240data, i2c_buf, 6);
                if (ret < 0){
                    ret = -1;
                    break;
                }
            }
#else
            i2c_buf[0] = 0xA0;
            i2c_buf[1] = 0x18;
            memcpy(i2c_buf + 2, bin_addr + eep_addr, 512);
            ret |= hyn_write_data(hyn_3240data, i2c_buf, 514);
#endif
            ret |= hyn_wr_reg(hyn_3240data, 0xA004EE, 3,i2c_buf,0);
            if(ret) continue;

            mdelay(10); //wait finsh

            for(k=0;k<200;k++){
                mdelay(10);
                ret = hyn_wr_reg(hyn_3240data,0xA005,2,i2c_buf,1);
                if (ret < 0 || i2c_buf[0] != 0x55){
                    continue;
                }            
                if(i2c_buf[0] == 0x55){
                    break;
                }
            }
            if(k==200) continue;
        }

        if(ret) continue;

        ret = hyn_wr_reg(hyn_3240data, 0xA01002, 3,i2c_buf,0);
        if(ret) continue;


        i2c_buf[0] = 0xA0;
        i2c_buf[1] = 0x0C;
        i2c_buf[2] = 0x1E0&0xff;//480
        i2c_buf[3] = 0x1E0>>8;
        ret =  hyn_write_data(hyn_3240data, i2c_buf, 4);
        if (ret )  continue;

        i2c_buf[0] = 0xA0;
        i2c_buf[1] = 0x14;
        eep_addr = 31 << 9;		
        i2c_buf[2] = eep_addr;
        i2c_buf[3] = eep_addr>>8;
        ret =  hyn_write_data(hyn_3240data, i2c_buf, 4);
        if (ret )  continue;

#if HYN_TRANSFER_LIMIT
        memcpy(i2c_buf, bin_addr + eep_addr, 480);
        for(j=0; j<120; j++) {
            iic_addr = (j<<2);
            temp_buf[0] = (iic_addr+0xA018)>>8;
            temp_buf[1] = (iic_addr+0xA018)&0xFF;
            temp_buf[2] = i2c_buf[iic_addr+0];
            temp_buf[3] = i2c_buf[iic_addr+1];
            temp_buf[4] = i2c_buf[iic_addr+2];
            temp_buf[5] = i2c_buf[iic_addr+3];
            ret =   hyn_write_data(hyn_3240data, i2c_buf, 6);
            if (ret < 0){
                ret = -1;
                break;
            }
        }
#else
        i2c_buf[0] = 0xA0;
        i2c_buf[1] = 0x18;
        memcpy(i2c_buf + 2, bin_addr + eep_addr, 480);
        ret |= hyn_write_data(hyn_3240data, i2c_buf, 482);
#endif
        ret |= hyn_wr_reg(hyn_3240data, 0xA004EE, 3,i2c_buf,0);
        if(ret) continue;

        mdelay(10); //wait finsh

        for(k=0;k<200;k++){
            mdelay(10);
            ret = hyn_wr_reg(hyn_3240data,0xA005,2,i2c_buf,1);

            if (ret < 0 || i2c_buf[0] != 0x55){
                continue;
            }            
            if(i2c_buf[0] == 0x55){
                break;
            }
        }
        if(k==200) continue;



        ret = hyn_wr_reg(hyn_3240data,0xA00300,3,i2c_buf,0);
        if(ret) continue;
        hyn_3240data->hw_info.ic_fw_checksum = cst3240_read_checksum();
        if(fw_checksum != hyn_3240data->hw_info.ic_fw_checksum){
            HYN_INFO("updata fw err %d ic_fw_checksum != fw_checksum\r\n",retry);
            continue;
        }
        break;
    }
	
    hyn_wr_reg(hyn_3240data,0xA006EE,3,i2c_buf,0); //exit boot
    mdelay(2);
    cst3240_rst();
    mdelay(50);
    hyn_set_i2c_addr(hyn_3240data,MAIN_I2C_ADDR);
		
    if(retry){

        cst3240_updata_tpinfo();
        HYN_INFO("updata_fw success");
    }
    else{
        HYN_INFO("updata_fw failed");
    }

    hyn_irq_set(hyn_3240data,ENABLE);
    return !retry;
}

static u32 cst3240_check_esd(void)
{
    int ret = 0;
    u8 buf[6];
    ret = hyn_wr_reg(hyn_3240data,0xD040,2,buf,6);
    if(ret ==0){
        u16 checksum = buf[0]+buf[1]+buf[2]+buf[3]+0xA5;
        if(checksum != ( (buf[4]<<8)+ buf[5])){
            ret = -1;
        }
    }
    return ret ? 0:(buf[3]+(buf[2]<<8)+(buf[1]<<16)+(buf[0]<<24));
}

static int cst3240_get_rawdata(u8 *buf, u16 len)
{
    int ok = 0;   
    uint16_t node_num = 0;

    if (hyn_3240data->hw_info.fw_key_num == 0) {
        node_num = hyn_3240data->hw_info.fw_sensor_txnum * hyn_3240data->hw_info.fw_sensor_rxnum;
    } else {
        node_num = (hyn_3240data->hw_info.fw_sensor_txnum - 1) * hyn_3240data->hw_info.fw_sensor_rxnum + hyn_3240data->hw_info.fw_key_num;
    }

    ok = hyn_wr_reg(hyn_3240data,0x1000,2,buf,node_num*2);
    if (ok == FALSE) {
        return FALSE;
    }
        
    ok = hyn_wr_reg(hyn_3240data,0x000500,3,buf,0);
    if (ok == FALSE) {
        return FALSE;
    }

    return TRUE;
}

static int cst3240_get_differ(u8 *buf, u16 len)
{
    int ok = 0;   
    uint16_t node_num = 0;

    if (hyn_3240data->hw_info.fw_key_num == 0) {
        node_num = hyn_3240data->hw_info.fw_sensor_txnum * hyn_3240data->hw_info.fw_sensor_rxnum;
    } else {
        node_num = (hyn_3240data->hw_info.fw_sensor_txnum - 1) * hyn_3240data->hw_info.fw_sensor_rxnum + hyn_3240data->hw_info.fw_key_num;
    }

    ok = hyn_wr_reg(hyn_3240data,0x1000,2,buf,node_num*2);
    if (ok == FALSE) {
        return FALSE;
    }
        
    ok = hyn_wr_reg(hyn_3240data,0x000500,3,buf,0);
    if (ok == FALSE) {
        return FALSE;
    }

    return TRUE;
}

static int cst3240_get_test_result(u8 *buf, u16 len)
{
    return 0;
}


const struct hyn_ts_fuc cst3240_fuc = {
    .tp_rest = cst3240_rst,
    .tp_report = cst3240_report,
    .tp_supend = cst3240_supend,
    .tp_resum = cst3240_resum,
    .tp_chip_init = cst3240_init,
    .tp_updata_fw = cst3240_updata_fw,
    .tp_set_workmode = cst3240_set_workmode,
    .tp_check_esd = cst3240_check_esd,
    .tp_prox_handle = cst3240_prox_handle,
    .tp_get_rawdata = cst3240_get_rawdata,
    .tp_get_differ = cst3240_get_differ,
    .tp_get_test_result = cst3240_get_test_result
};


#endif