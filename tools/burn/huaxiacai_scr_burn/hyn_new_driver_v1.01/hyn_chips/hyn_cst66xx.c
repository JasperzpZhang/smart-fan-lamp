#include "hyn_core.h"
#if defined(chip_cst36xx) || defined(chip_cst66xx)

#include "cst3640_fw.h"
#include "cst6656_fw.h"
#include "cst6644_fw.h"
#include "cst6856_fw.h"

#define BOOT_I2C_ADDR   (0x5A)
#define MAIN_I2C_ADDR   (0x58)

#define FACTEST_PATH    "/sdcard/hyn_fac_test_cfg.ini"
#define FACTEST_LOG_PATH "/sdcard/hyn_fac_test.log"
#define cst66xx_BIN_SIZE    (40*1024)
#define MODULE_ID_ADDR      (0xA400)
#define PARTNUM_ADDR        (0xFF10)


static struct hyn_ts_data *hyn_66xxdata = NULL;
static const u8 gest_map_tbl[33] = {0xff,4,1,3,2,5,12,6,7,7,9,11,10,13,12,7,7,6,10,6,5,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,14};
static const struct hyn_chip_series hyn_6xx_fw[] = {
    {0xCACA2202,"cst3640",(u8*)fw_bin_3640}, //default chip
    {0xCACA2201,"cst6656",(u8*)fw_bin_6656},
    {0xCACA2203,"cst6644",(u8*)fw_bin_6644},
    {0xCACA2204,"cst6856",(u8*)fw_bin_6856},
    //0xCACA2205 //154
    //0xCACA2206 //148E
    {0,0,NULL}
};

static int cst66xx_updata_judge(u8 *p_fw, u16 len);
static u32 cst66xx_read_checksum(void);
static int cst66xx_enter_boot(void);
static u32 cst66xx_fread_word(u32 addr);
static void cst66xx_rst(void);

static int cst66xx_init(struct hyn_ts_data* ts_data)
{
    int ret = 0;
    u32 read_part_no;
    HYN_ENTER();
    hyn_66xxdata = ts_data;
    hyn_set_i2c_addr(hyn_66xxdata,BOOT_I2C_ADDR);
    ret = cst66xx_enter_boot();
    if(ret){
        HYN_INFO("cst66xx_enter_boot failed");
        return -1;
    }

    hyn_66xxdata->fw_updata_addr = hyn_6xx_fw[0].fw_bin;
    hyn_66xxdata->fw_updata_len = cst66xx_BIN_SIZE;
    read_part_no = cst66xx_fread_word(PARTNUM_ADDR);
    HYN_INFO("read_part_no:0x%08x",read_part_no);

    if((read_part_no & 0xCACA0000)==0xCACA0000){
        for(u8 i = 0; ;i++){
            if(hyn_6xx_fw[i].part_no == read_part_no){
                hyn_66xxdata->fw_updata_addr = hyn_6xx_fw[i].fw_bin;
                HYN_INFO("chip %s match success",hyn_6xx_fw[i].chip_name);
            }
            else if(hyn_6xx_fw[i].part_no == 0){
                break;
            }
        }
    }

    hyn_66xxdata->hw_info.ic_fw_checksum = cst66xx_read_checksum();
    cst66xx_rst(); //exit boot
    mdelay(50);
    hyn_set_i2c_addr(hyn_66xxdata,MAIN_I2C_ADDR);
    hyn_66xxdata->need_updata_fw = cst66xx_updata_judge(hyn_66xxdata->fw_updata_addr,cst66xx_BIN_SIZE);

    if(hyn_66xxdata->need_updata_fw){
        HYN_INFO("need updata FW !!!");
    }
    return 0;
}


static int cst66xx_report(void)
{
    u8 buf[80],i=0;
    u8 finger_num = 0,key_num=0,len = 0,report_typ= 0,key_state=0,key_id = 0,tmp_dat=0;
    struct hyn_plat_data *dt = &hyn_66xxdata->plat_data;
    int ret = 0,retry = 2;
    u16 crc_value = 0;

    hyn_66xxdata->rp_buf.report_need = REPORT_NONE;
    while(retry--){ //read point
        ret = hyn_wr_reg(hyn_66xxdata,0xD0070000,4,buf,9); 
        report_typ = buf[2];//FF:pos F0:ges E0:prox
        finger_num = buf[3]&0x0f;
        key_num    = buf[3]&0xf0;
        if(finger_num > dt->max_touch_num){
            finger_num = dt->max_touch_num;
        }
        if(finger_num > 0) len = (((key_num>0)?1:0) + finger_num -1)*5;
        // HYN_INFO("finger_num=%d,key_num=%d,len=%d",finger_num,key_num,len);
        if(len > 0){ 
            ret |= hyn_read_data(hyn_66xxdata,&buf[9],len);
        }
        ret |= hyn_wr_reg(hyn_66xxdata,0xD00002AB,4,buf,0);
        if(ret == 0){
            crc_value = 0x55;
            for(i=0;i<(len+5);i++)
            crc_value += buf[4+i];
            
            if(crc_value == (buf[0] | buf[1]<<8)) break;
            else ret = -1;
        }
    }

    if(ret){
        HYN_ERROR("read point failed");
        goto ERRO_WRITE_TAIL;
    }

    if((report_typ==0xff)&&((finger_num+key_num)>0)){
        if(key_num){
            key_id    = buf[8]&0xff;
            key_state = buf[8]>>8;
            hyn_66xxdata->rp_buf.report_need |= REPORT_KEY;
            if((key_id == hyn_66xxdata->rp_buf.key_id || 0 == hyn_66xxdata->rp_buf.key_state)&& key_state !=0){
                hyn_66xxdata->rp_buf.key_id = key_id;
                hyn_66xxdata->rp_buf.key_state = 1;
            }else{
                hyn_66xxdata->rp_buf.key_state = 0;
            }   
        }

        if(finger_num){ //pos
            u16 index = 0;            
            hyn_66xxdata->rp_buf.report_need |= REPORT_POS;
            hyn_66xxdata->rp_buf.rep_num = finger_num;
            for(i = 0; i < finger_num; i++){
                index = i*5;
                hyn_66xxdata->rp_buf.pos_id[i] = buf[index+8]&0x0F;
                hyn_66xxdata->rp_buf.event[i] =  buf[index+8]>>4;
                hyn_66xxdata->rp_buf.pos_y[i] =  hyn_66xxdata->hw_info.fw_res_x -( buf[index+4] + ((u16)(buf[index+7]&0x0F) <<8));
                hyn_66xxdata->rp_buf.pos_x[i] =  hyn_66xxdata->hw_info.fw_res_y -( buf[index+5] + ((u16)(buf[index+7]&0xf0) <<4));
                hyn_66xxdata->rp_buf.pres_z[i] = buf[index+6];
                //HYN_INFO("report_id = %d, xy = %d,%d",hyn_66xxdata->rp_buf.pos_id[i],hyn_66xxdata->rp_buf.pos_x[i],hyn_66xxdata->rp_buf.pos_y[i]);
            }
        }
    }else if(report_typ == 0xF0){
         tmp_dat = buf[8]&0xff;
        if((tmp_dat&0x7F) <= 32){  
            hyn_66xxdata->gesture_id = gest_map_tbl[tmp_dat];
            hyn_66xxdata->rp_buf.report_need |= REPORT_GES;
            HYN_INFO("gesture_id:%d",tmp_dat);
        }
    }
    return 0;
ERRO_WRITE_TAIL:
    ret = hyn_wr_reg(hyn_66xxdata,0xD00002AB,4,buf,0);
    return -1;
}

static int cst66xx_prox_handle(u8 cmd)
{
    int ret = 0;
    switch(cmd){
        case 1: //enable
            hyn_66xxdata->prox_mode_en = 1;
            hyn_66xxdata->prox_state = 0;
            //ret = hyn_wr_reg(hyn_66xxdata,0xD004B01,3,NULL,0);
            break;
        case 0: //disable
            hyn_66xxdata->prox_mode_en = 0;
            hyn_66xxdata->prox_state = 0;
           //ret = hyn_wr_reg(hyn_66xxdata,0xD004B00,3,NULL,0);
            break;
        case 2: //read
           //ret = hyn_wr_reg(hyn_66xxdata,0xD004B,2,&hyn_66xxdata->prox_state,1);
            break;
    }
    return ret;
}

static int cst66xx_set_workmode(enum work_mode mode,u8 enable)
{
    HYN_INFO("set_workmode:%d",mode);
    hyn_66xxdata->work_mode = mode;
    if(mode != NOMAL_MODE){
        hyn_esdcheck_switch(hyn_66xxdata,DISABLE);
    }
    hyn_wr_reg(hyn_66xxdata,0xD0000400,4,0,0); //disable lp i2c plu
    mdelay(1);
    hyn_wr_reg(hyn_66xxdata,0xD0000400,4,0,0);

    switch(mode){
        case NOMAL_MODE:
            hyn_irq_set(hyn_66xxdata,ENABLE);
            hyn_esdcheck_switch(hyn_66xxdata,ENABLE);
            hyn_wr_reg(hyn_66xxdata,0xD0000000,4,0,0);
            hyn_wr_reg(hyn_66xxdata,0xD0000400,4,0,0); //disable lp
            hyn_wr_reg(hyn_66xxdata,0xD0000C00,4,0,0);
            hyn_wr_reg(hyn_66xxdata,0xD0000100,4,0,0);
            break;
        case GESTURE_MODE:
            hyn_wr_reg(hyn_66xxdata,0xD0000C01,4,0,0);
            break;
        case LP_MODE:
            hyn_wr_reg(hyn_66xxdata,0xD00004AB,4,0,0);
            break;
        case DIFF_MODE:
        case RAWDATA_MODE:
            hyn_wr_reg(hyn_66xxdata,0xD00002AB,4,0,0); 
            hyn_wr_reg(hyn_66xxdata,0xD00001AB,4,0,0); //enter debug mode
            break;
        case FAC_TEST_MODE:
            cst66xx_rst();
            msleep(50);
            hyn_wr_reg(hyn_66xxdata,0xD00002AB,4,0,0); 
            hyn_wr_reg(hyn_66xxdata,0xD00000AB,4,0,0); //enter fac test
            break;
        case DEEPSLEEP:
            hyn_irq_set(hyn_66xxdata,DISABLE);
            hyn_wr_reg(hyn_66xxdata,0xD00022AB,4,0,0);
            break;
        default :
            hyn_esdcheck_switch(hyn_66xxdata,ENABLE);
            hyn_66xxdata->work_mode = NOMAL_MODE;
            break;
    }
    return 0;
}


static int cst66xx_supend(void)
{
    HYN_ENTER();
    cst66xx_set_workmode(DEEPSLEEP,0);
    return 0;
}

static int cst66xx_resum(void)
{
    HYN_ENTER();
    cst66xx_rst();
    msleep(50);
    cst66xx_set_workmode(NOMAL_MODE,0);
    return 0;
}

static void cst66xx_rst(void)
{
    HYN_ENTER();
    msleep(40);
    u8 temp = hyn_66xxdata->client->addr;
    hyn_set_i2c_addr(hyn_66xxdata,MAIN_I2C_ADDR);
    hyn_wr_reg(hyn_66xxdata,0xD00003AB,4,0,0); //soft rst
    hyn_set_i2c_addr(hyn_66xxdata,temp);
    // gpio_set_value(hyn_66xxdata->plat_data.reset_gpio,0);
    // msleep(2);
    // gpio_set_value(hyn_66xxdata->plat_data.reset_gpio,1);
}

static int cst66xx_enter_boot(void)
{
    int retry = 0,ret = 0,cnt_max = 20;
    u8 buf[4];
    while(++retry<cnt_max){
        cst66xx_rst();
        mdelay(12+retry);
        ret = hyn_wr_reg(hyn_66xxdata,0xA001A8,3,buf,0);
        if(ret < 0){
            continue;
        }
        if(cnt_max == 20){
            cnt_max = retry+4;
        }
        ret = hyn_wr_reg(hyn_66xxdata,0xA002,2,buf,2);
        if( ret==0 && buf[0] == 0x22 && buf[1] == 0xDD){
            return 0;
        }
    }
    return -1;
}

static int cst66xx_updata_tpinfo(void)
{
    u8 buf[60];
    struct tp_info *ic = &hyn_66xxdata->hw_info;
    int ret = 0;
    int retry = 4;

    while(--retry){
        ret = hyn_wr_reg(hyn_66xxdata,0xD0000400,4,buf,0);
        mdelay(2);
        ret |= hyn_wr_reg(hyn_66xxdata,0xD0000400,4,buf,0);
        //get all config info
        ret |= hyn_wr_reg(hyn_66xxdata,0xD0030000,4,buf,50);
        if(ret == 0) break; 
    }

    if(ret){
        HYN_ERROR("cst66xx_updata_tpinfo failed");
        return -1;
    }

    ic->fw_sensor_txnum = buf[48];
    ic->fw_sensor_rxnum = buf[49];
    ic->fw_key_num = buf[27];
    ic->fw_res_y = (buf[31]<<8)|buf[30];
    ic->fw_res_x = (buf[29]<<8)|buf[28];
    HYN_INFO("IC_info tx:%d rx:%d key:%d res-x:%d res-y:%d",ic->fw_sensor_txnum,ic->fw_sensor_rxnum,ic->fw_key_num,ic->fw_res_x,ic->fw_res_y);

    ic->fw_project_id = (buf[39]<<24)|(buf[38]<<16)|(buf[37]<<8)|buf[36];
    ic->fw_chip_type = (buf[3]<<24)|(buf[2]<<16)|(buf[1]<<8)|buf[0];
    ic->fw_ver = (buf[35]<<24)|(buf[34]<<16)|(buf[33]<<8)|buf[32];
    HYN_INFO("IC_info fw_project_id:%04x ictype:%04x fw_ver:%04x checksum:%04x",ic->fw_project_id,ic->fw_chip_type,ic->fw_ver,ic->ic_fw_checksum);

    return 0;
}

static u32 cst66xx_fread_word(u32 addr)
{
    int ret;
    u8 rec_buf[4],retry;
    u32 read_word = 0;

    retry = 3;
    while(retry--){
        ret = hyn_wr_reg(hyn_66xxdata,U8TO32(0xA0,0x06,(addr&0xFF),((addr>>8)&0xFF)),4,NULL,0); //set addr
        ret  |= hyn_wr_reg(hyn_66xxdata,0xA0080400,4,0,0); //set len
        ret  |= hyn_wr_reg(hyn_66xxdata,0xA00A0300,4,0,0); //?
        ret  |= hyn_wr_reg(hyn_66xxdata,0xA004D2,3,NULL,0);	//trig read
        if(ret ==0) break;
    }
    if(ret) return 0;

    retry = 20;
	while(retry--){
        ret = hyn_wr_reg(hyn_66xxdata,0xA020,2,rec_buf,2);	
		if(ret==0 && rec_buf[0]==0xD2 && rec_buf[1]==0x88){
            ret  |= hyn_wr_reg(hyn_66xxdata,0xA040,2,rec_buf,4);
            if(ret ==0){
                read_word = U8TO32(rec_buf[3],rec_buf[2],rec_buf[1],rec_buf[0]);
                break;
            }
		}
		mdelay(1);
	}

	return read_word;
}

static u32  read_checksum(u16 start_val,u16 start_addr,u16 len)
{
    int ret,retry = 3;
    u32 check_sum = 0;
    u8 buf[8] = {0};
    while(retry--){
        ret = hyn_wr_reg(hyn_66xxdata,U8TO32(0xA0,0x06,start_addr&0xFF,start_addr>>8),4,0,0);
        ret |= hyn_wr_reg(hyn_66xxdata,U8TO32(0xA0,0x08,len&0xFF,len>>8),4,0,0);
        ret |= hyn_wr_reg(hyn_66xxdata,U8TO32(0xA0,0x0A,start_val&0xFF,start_val>>8),4,0,0);
        ret |= hyn_wr_reg(hyn_66xxdata,0xA004D6,3,0,0);
        if(ret) continue;
        mdelay(len/0xc00 + 1);
        for(int j=0; j<20; j++){
            ret = hyn_wr_reg(hyn_66xxdata,0xA020,2,buf,2);
            if(ret ==0 && buf[0]==0xD6 && buf[1]==0x88){
                break;
            }
            mdelay(2);
        }
        ret = hyn_wr_reg(hyn_66xxdata,0xA040,2,buf,5);
        if(ret == 0 && buf[0] == 0xCA){
            check_sum = U8TO32(buf[4],buf[3],buf[2],buf[1]);
            break;
        }
        else{
            continue;
        }
    }
    return check_sum;
}

static u32 cst66xx_read_checksum(void)
{
    u32 chipSum1,chipSum2,totalSum;
    chipSum1=chipSum2=totalSum=0;

    chipSum1 = read_checksum(0,0,0x9000);
    chipSum2 = read_checksum(1,0xb000,0x0ffc);
    
    if(chipSum1 != 0 && chipSum2 != 0){
        totalSum = chipSum1 + chipSum2*2 - 0x55; 
    }
    // HYN_INFO("chipSum1:%04x chipSum2:%04x totalSum:%04x",chipSum1,chipSum2,totalSum);
    return totalSum;
}

static int cst66xx_updata_judge(u8 *p_fw, u16 len)
{
    u32 f_checksum,f_fw_ver,f_ictype,f_fw_project_id;
    u32 f_checksum_35k,f_checksum_1k,f_checksum_4k,f_check_all;
    u8 *p_data = p_fw + len- 5*1024; //35k 
    u32 *pdata_h;
    u16 i;
    int ret;
    struct tp_info *ic = &hyn_66xxdata->hw_info;

    f_fw_project_id = U8TO32(p_data[39],p_data[38],p_data[37],p_data[36]);
    f_ictype        = U8TO32(p_data[3],p_data[2],p_data[1],p_data[0]);
    f_fw_ver        = U8TO32(p_data[35],p_data[34],p_data[33],p_data[32]);
    
    p_data =  p_fw + len- 5*1024 -4; //35k 
    f_checksum_35k = U8TO32(p_data[3],p_data[2],p_data[1],p_data[0]);   
    p_data =  p_fw + len- 4*1024 -4; //1k 
    f_checksum_1k = U8TO32(p_data[3],p_data[2],p_data[1],p_data[0]);   
    p_data = p_fw + len- 4; //4k
    f_checksum_4k = U8TO32(p_data[3],p_data[2],p_data[1],p_data[0]);
    f_checksum=((f_checksum_35k*2) + (f_checksum_1k*2) -0x55);

    // HYN_INFO("f_checksum_35k:%04x f_checksum_1k:%04x f_checksum_4k:%04x f_checksum:%04x",f_checksum_35k,f_checksum_1k,f_checksum_4k,f_checksum);
    f_checksum=(f_checksum + (f_checksum_4k*2) -0x55);
    HYN_INFO("Bin_info fw_project_id:%04x ictype:%04x fw_ver:%04x checksum:%04x",f_fw_project_id,f_ictype,f_fw_ver,f_checksum);
    
    //check h file
    f_check_all = 0x55;
    pdata_h = (u32*)p_fw;
    i = len/4;
    while(i--){
        f_check_all += *pdata_h;
        pdata_h++;
    }
    if(f_check_all != f_checksum){
        HYN_INFO(".h file erro checksum:%04x",f_check_all);
        return 0; //no need updata
    }

    //read ic checksum failed
    if(ic->ic_fw_checksum ==0){
        return 1; //need updata
    } 

    ret = cst66xx_updata_tpinfo(); //boot checksum pass, communicate failed not updata
    if(ret == 0){
        if(f_ictype == ic->fw_chip_type 
        && f_fw_project_id == ic->fw_project_id
        && f_checksum != ic->ic_fw_checksum
        && f_fw_ver >= ic->fw_ver
        ){
            return 1; //need updata
        }
    }
    return 0;
}

static int cst66xx_updata_fw(u8 *bin_addr, u16 len)
{
    #define CHECKSUM_OFFECT  (40*1024-4)
    int i,ret, retry,retry_fw= 4;
	u8 i2c_buf[1024+10];
    u8 *p_bin_addr=bin_addr;
	u16 eep_addr = 0, total_kbyte = 40;
    u32 fw_checksum,f_checksum_35k,f_checksum_1k,f_checksum_4k;
      
    p_bin_addr =  bin_addr + len- 5*1024 -4; //35k 
    f_checksum_35k = U8TO32(p_bin_addr[3],p_bin_addr[2],p_bin_addr[1],p_bin_addr[0]);   
    p_bin_addr =  bin_addr + len- 4*1024 -4; //1k 
    f_checksum_1k = U8TO32(p_bin_addr[3],p_bin_addr[2],p_bin_addr[1],p_bin_addr[0]);   
    p_bin_addr = bin_addr + len- 4; //4k
    f_checksum_4k = U8TO32(p_bin_addr[3],p_bin_addr[2],p_bin_addr[1],p_bin_addr[0]);
 
    fw_checksum=((f_checksum_35k*2) + (f_checksum_1k*2) -0x55);
    fw_checksum=(fw_checksum + (f_checksum_4k*2) -0x55);
    HYN_INFO("fw_checksum:%04x",fw_checksum);
    

#if HYN_TRANSFER_LIMIT
	unsigned char temp_buf[8];
	unsigned short iic_addr;
	int  j;
#endif
    HYN_ENTER();
    hyn_irq_set(hyn_66xxdata,DISABLE);
    hyn_set_i2c_addr(hyn_66xxdata,BOOT_I2C_ADDR);
    while(--retry_fw){
        ret = cst66xx_enter_boot();
        if(ret){
            HYN_INFO("cst66xx_enter_boot faill");
            continue;
        }
        //erase 32k 0x0000~0x8000
        ret = hyn_wr_reg(hyn_66xxdata,0xA0060000,4,i2c_buf,0); 
        ret |= hyn_wr_reg(hyn_66xxdata,0xA0080080,4,i2c_buf,0); 
        ret |= hyn_wr_reg(hyn_66xxdata,0xA00A0200,4,i2c_buf,0); 
        ret |= hyn_wr_reg(hyn_66xxdata,0xA018CACA,4,i2c_buf,0); 
        ret |= hyn_wr_reg(hyn_66xxdata,0xA004E0,3,i2c_buf,0); 
        if(ret) continue;

        mdelay(40); //wait finsh earse flash
        for(retry=0; retry<20; retry++)
        {
            ret = hyn_wr_reg(hyn_66xxdata,0xA020,2,i2c_buf,2); 
            if(i2c_buf[0]==0xE0 && i2c_buf[1]==0x88) {
                break; //
            }
            mdelay(1);
        }

        //erase 4k 0x8000~0x9000
        ret = hyn_wr_reg(hyn_66xxdata,0xA0060080,4,i2c_buf,0); 
        ret |= hyn_wr_reg(hyn_66xxdata,0xA0080010,4,i2c_buf,0); 
        ret |= hyn_wr_reg(hyn_66xxdata,0xA00A0100,4,i2c_buf,0); 
        ret |= hyn_wr_reg(hyn_66xxdata,0xA018CACA,4,i2c_buf,0); 
        ret |= hyn_wr_reg(hyn_66xxdata,0xA004E0,3,i2c_buf,0); 
        if(ret) continue;

        mdelay(10); //wait finsh earse flash
        for(retry=0; retry<20; retry++)
        {
            ret = hyn_wr_reg(hyn_66xxdata,0xA020,2,i2c_buf,2); 
            if(i2c_buf[0]==0xE0 && i2c_buf[1]==0x88) {
                break; 
            }
            mdelay(1);
        }

         //erase 4k 0xB000~0xC000
        ret = hyn_wr_reg(hyn_66xxdata,0xA00600B0,4,i2c_buf,0); 
        ret |= hyn_wr_reg(hyn_66xxdata,0xA0080010,4,i2c_buf,0); 
        ret |= hyn_wr_reg(hyn_66xxdata,0xA00A0400,4,i2c_buf,0); 
        ret |= hyn_wr_reg(hyn_66xxdata,0xA018CACA,4,i2c_buf,0); 
        ret |= hyn_wr_reg(hyn_66xxdata,0xA004E0,3,i2c_buf,0); 
        if(ret) continue;

        mdelay(10); //wait finsh earse flash
        for(retry=0; retry<20; retry++)
        {
            ret = hyn_wr_reg(hyn_66xxdata,0xA020,2,i2c_buf,2); 
            if(i2c_buf[0]==0xE0 && i2c_buf[1]==0x88) {
                break; 
            }
            mdelay(1);
        }

        //start trans fw
        for (i=0; i<total_kbyte; i++) {

            eep_addr = i << 10;		//i * 1024
            p_bin_addr = bin_addr+eep_addr;
            if(i>=36){
                eep_addr =0xB000 + ( (i-36)<<10); //
            }
            i2c_buf[0] = 0xA0;
            i2c_buf[1] = 0x06;
            i2c_buf[2] = eep_addr;
            i2c_buf[3] = eep_addr>>8;
            ret = hyn_write_data(hyn_66xxdata, i2c_buf, 4);
            ret |= hyn_wr_reg(hyn_66xxdata,0xA0080004,4,i2c_buf,0); 
            if(i>=36){
                ret |= hyn_wr_reg(hyn_66xxdata,0xA00A0300,4,i2c_buf,0);
            }else{
                ret |= hyn_wr_reg(hyn_66xxdata,0xA00A0000,4,i2c_buf,0);
            }
            ret |= hyn_wr_reg(hyn_66xxdata,0xA018CACA,4,i2c_buf,0); 
            if(ret) continue;
#if HYN_TRANSFER_LIMIT
            memcpy(i2c_buf, pdata + eep_addr, 1024);
            for(j=0; j<256; j++) {
                iic_addr = (j<<2);
                temp_buf[0] = (iic_addr+0xA018)>>8;
                temp_buf[1] = (iic_addr+0xA018)&0xFF;
                temp_buf[2] = i2c_buf[iic_addr+0];
                temp_buf[3] = i2c_buf[iic_addr+1];
                temp_buf[4] = i2c_buf[iic_addr+2];
                temp_buf[5] = i2c_buf[iic_addr+3];
                ret = cst66xx_i2c_write(client, temp_buf, 6);
                    if (ret < 0)
                        goto error_out;
            }
#else
            i2c_buf[0] = 0xA0;
            i2c_buf[1] = 0x40;
            memcpy(i2c_buf + 2, p_bin_addr, 1024);
            ret |= hyn_write_data(hyn_66xxdata, i2c_buf, 1026);
#endif
            ret |= hyn_wr_reg(hyn_66xxdata, 0xA004E1, 3,i2c_buf,0);
            mdelay(30); //wait finsh

            for(retry=0; retry<20; retry++)
            {
                ret = hyn_wr_reg(hyn_66xxdata,0xA020,2,i2c_buf,2); 
                if(i2c_buf[0]==0xE1 && i2c_buf[1]==0x88) {
                    break; 
                }
                mdelay(1);
            }
           
        }
        if(ret) continue;
        hyn_66xxdata->hw_info.ic_fw_checksum = cst66xx_read_checksum();
        if(fw_checksum != hyn_66xxdata->hw_info.ic_fw_checksum){
            continue;
        }
        break;
    }

    if(retry_fw){
        cst66xx_rst();
        mdelay(50);
        cst66xx_updata_tpinfo();
        HYN_INFO("updata_fw success");
    }
    else{
        HYN_INFO("updata_fw failed");
    }
    hyn_set_i2c_addr(hyn_66xxdata,MAIN_I2C_ADDR);
    hyn_irq_set(hyn_66xxdata,ENABLE);
    return !retry_fw;
}

static u32 cst66xx_check_esd(void)
{
    int ret = 0;
    u8 buf[6];
    ret = hyn_wr_reg(hyn_66xxdata,0xD040,2,buf,6);
    if(ret ==0){
        u16 checksum = buf[0]+buf[1]+buf[2]+buf[3]+0xA5;
        if(checksum != ( (buf[4]<<8)+ buf[5])){
            ret = -1;
        }
    }
    return ret ? 0:(buf[3]+(buf[2]<<8)+(buf[1]<<16)+(buf[0]<<24));
}

static int cst66xx_get_rawdata(u8 *buf, u16 len)
{
    struct tp_info *ic = &hyn_66xxdata->hw_info;
    int ret = 0;
    u32 addr = 0xD0110000;
    u16 read_len = (ic->fw_sensor_rxnum*ic->fw_sensor_txnum)*2;
    u16 total_len = (ic->fw_sensor_rxnum*ic->fw_sensor_txnum + ic->fw_sensor_txnum + ic->fw_sensor_rxnum)*2;
    if(total_len > len){
        HYN_ERROR("buf too small");
        return -1;
    } 
    ret |= hyn_wr_reg(hyn_66xxdata,addr,4,buf,read_len); //m cap

    addr = 0xD0150000;
    buf += read_len;
    read_len = ic->fw_sensor_txnum*2;
    ret |= hyn_wr_reg(hyn_66xxdata,addr,4,buf,read_len); //s tx cap

    addr = 0xD0190000;
    buf += read_len;
    read_len = ic->fw_sensor_rxnum*2;
    ret |= hyn_wr_reg(hyn_66xxdata,addr,4,buf,read_len); //s rx cap

    ret |= hyn_wr_reg(hyn_66xxdata,0xD00002AB,4,0,0); //end

    return ret < 0 ? -1:total_len;
}

static int cst66xx_get_differ(u8 *buf, u16 len)
{
    struct tp_info *ic = &hyn_66xxdata->hw_info;
    int ret = 0;
    u32 addr = 0xD0120000;
    u16 read_len = (ic->fw_sensor_rxnum*ic->fw_sensor_txnum)*2;
    u16 total_len = (ic->fw_sensor_rxnum*ic->fw_sensor_txnum + ic->fw_sensor_txnum + ic->fw_sensor_rxnum)*2;
    if(total_len > len){
        HYN_ERROR("buf too small");
        return -1;
    } 
    ret |= hyn_wr_reg(hyn_66xxdata,addr,4,buf,read_len); //m cap

    addr = 0xD0160000;
    buf += read_len;
    read_len = ic->fw_sensor_txnum*2;
    ret |= hyn_wr_reg(hyn_66xxdata,addr,4,buf,read_len); //s tx cap

    addr = 0xD01A0000;
    buf += read_len;
    read_len = ic->fw_sensor_rxnum*2;
    ret |= hyn_wr_reg(hyn_66xxdata,addr,4,buf,read_len); //s rx cap

    ret |= hyn_wr_reg(hyn_66xxdata,0xD00002AB,4,0,0); //end

    return ret < 0 ? -1:total_len;
}

static int cst66xx_get_test_result(u8 *buf, u16 len)
{
    struct tp_info *ic = &hyn_66xxdata->hw_info;
    u16 total_len = (ic->fw_sensor_rxnum*ic->fw_sensor_txnum + ic->fw_sensor_txnum + ic->fw_sensor_rxnum)*2;
    u16 mt_len = ic->fw_sensor_rxnum*ic->fw_sensor_txnum*2,i=0;
    s16 test_th[30*30];
    s16 *raw_ptr,tmp;
    int ret = 0;
    if(total_len > len || mt_len==0){
        HYN_ERROR("buf too small");
        return FAC_GET_DATA_FAIL;
    } 
    //open high
    ret  = hyn_wr_reg(hyn_66xxdata,0xD0002000,4,0,0);
    ret |= hyn_wait_irq_timeout(hyn_66xxdata,100);
    ret  |= hyn_wr_reg(hyn_66xxdata,0xD0120000,4,(u8*)test_th,mt_len);
    ret  |= hyn_wr_reg(hyn_66xxdata,0xD00002AB,4,0,0);
    if(ret){
        ret = FAC_GET_DATA_FAIL;
        HYN_ERROR("read open high failed");
        goto TEST_ERRO;
    }
    //open low
    ret  = hyn_wr_reg(hyn_66xxdata,0xD0002100,4,0,0);
    ret |= hyn_wait_irq_timeout(hyn_66xxdata,100);
    ret  |= hyn_wr_reg(hyn_66xxdata,0xD0120000,4,(u8*)buf,mt_len);
    ret  |= hyn_wr_reg(hyn_66xxdata,0xD00002AB,4,0,0);
    if(ret){
        ret = FAC_GET_DATA_FAIL;
        HYN_ERROR("read open low failed");
        goto TEST_ERRO;
    }
    //cal det
    raw_ptr = (s16*)buf;
    for(u16 i = 0; i< ic->fw_sensor_rxnum*ic->fw_sensor_txnum; i++){
        *raw_ptr = test_th[i]-*raw_ptr;
        raw_ptr++;
    }
    //short test
    ret  = hyn_wr_reg(hyn_66xxdata,0xD0002300,4,0,0);
    ret |= hyn_wait_irq_timeout(hyn_66xxdata,200);
    ret  |= hyn_wr_reg(hyn_66xxdata,0xD0120000,4,&buf[mt_len],(ic->fw_sensor_rxnum+ic->fw_sensor_txnum)*2);
    ret  |= hyn_wr_reg(hyn_66xxdata,0xD00002AB,4,0,0);
    raw_ptr = (s16*)&buf[mt_len];
    HYN_INFO("fac%d %02x,%02x,%02x",mt_len,buf[mt_len],buf[mt_len+1],buf[mt_len+2]);
    for(i = 0; i< ic->fw_sensor_rxnum+ic->fw_sensor_txnum; i++){
        *raw_ptr = ((u16)*raw_ptr << 8)|((u16)*raw_ptr>> 8);
        raw_ptr++;
    }
    HYN_INFO("fac%d %02x,%02x,%02x",mt_len,buf[mt_len],buf[mt_len+1],buf[mt_len+2]);
    if(ret){
        ret = FAC_GET_DATA_FAIL;
        HYN_ERROR("read fac short failed");
        goto TEST_ERRO;
    }
    
    //read threshold & judge low TH
    mt_len  = ic->fw_sensor_rxnum*ic->fw_sensor_txnum;
    ret = hyn_get_threshold(FACTEST_PATH,"TX0OpenMin",test_th,mt_len);
    ret |= hyn_get_threshold(FACTEST_PATH,"FactoryTxShortTh",&tmp,1);
    if(ret){
        ret = FAC_GET_CFG_FAIL;
        HYN_ERROR("read threshold failed");
        goto TEST_ERRO;
    }
    for(i = mt_len; i< (mt_len+ic->fw_sensor_rxnum+ic->fw_sensor_txnum); i++){
        test_th[i] = tmp;
    } 
    raw_ptr = (s16*)buf;
    mt_len = mt_len+ic->fw_sensor_rxnum+ic->fw_sensor_txnum;
    for(i = 0; i< mt_len; i++){
        if(*raw_ptr++ < test_th[i]) break;
    }

    if(i == mt_len){
        HYN_INFO("open low pass");
        HYN_INFO("short test pass");
    }
    else if(i < ic->fw_sensor_rxnum*ic->fw_sensor_txnum){
        HYN_INFO("open low test failed");
        ret = FAC_TEST_OPENL_FAIL;
        goto TEST_ERRO;
    }
    else{
        HYN_INFO("short test failed");
        ret = -FAC_TEST_SHORT_FAIL;
        goto TEST_ERRO;
    }

    //read threshold & judge high TH
    raw_ptr = (s16*)buf;
    mt_len  = ic->fw_sensor_rxnum*ic->fw_sensor_txnum;
    ret = hyn_get_threshold(FACTEST_PATH,"TX0OpenMax",test_th,mt_len);
    if(ret){
        ret = FAC_GET_CFG_FAIL;
        HYN_ERROR("read threshold failed");
        goto TEST_ERRO;
    }
    for(i = 0; i< mt_len; i++){
        if(*raw_ptr++ > test_th[i]) break;
    }
    if(i == mt_len){
        HYN_INFO("open high pass");
    }
    else{
        HYN_INFO("open high test failed");
        ret = FAC_TEST_OPENH_FAIL;
        goto TEST_ERRO;
    }
    
TEST_ERRO:
    if(0 == fac_test_log_save(FACTEST_LOG_PATH,hyn_66xxdata,(s16*)buf,ret)) HYN_INFO("fac_test log save success");
    else  HYN_INFO("fac_test log save failed");

    cst66xx_resum();
    return ret;
}


const struct hyn_ts_fuc cst66xx_fuc = {
    .tp_rest = cst66xx_rst,
    .tp_report = cst66xx_report,
    .tp_supend = cst66xx_supend,
    .tp_resum = cst66xx_resum,
    .tp_chip_init = cst66xx_init,
    .tp_updata_fw = cst66xx_updata_fw,
    .tp_set_workmode = cst66xx_set_workmode,
    .tp_check_esd = cst66xx_check_esd,
    .tp_prox_handle = cst66xx_prox_handle,
    .tp_get_rawdata = cst66xx_get_rawdata,
    .tp_get_differ = cst66xx_get_differ,
    .tp_get_test_result = cst66xx_get_test_result
};

#endif


