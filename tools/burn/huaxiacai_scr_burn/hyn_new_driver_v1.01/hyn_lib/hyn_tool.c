
#include "hyn_core.h"

#if HYN_APK_DEBUG_EN

#define HYN_PROC_DIR_NAME		"hyn_apk_tool"
#define HYN_PROC_NODE_NAME		"fops_nod"

static struct hyn_ts_data *hyn_tool_data = NULL;
static const struct hyn_ts_fuc* hyn_tool_fun = NULL;

static struct proc_dir_entry *g_tool_dir = NULL;
static struct proc_dir_entry *g_tool_node = NULL;

#define RESET_IC 	0xCA
#define WRITE_IIC 	0x1A  //len
#define READ_REG 	0x2A
#define READ_IIC 	0x5A
#define READ_TPINF 	0x3A


static ssize_t hyn_proc_tool_read(struct file *page,char __user *user_buf, size_t count, loff_t *f_pos)
{
	unsigned char *buf = NULL;
	int ret_len = 0,ret = 0;	
	int max_len = hyn_tool_data->hw_info.fw_sensor_rxnum*hyn_tool_data->hw_info.fw_sensor_txnum*4
					+ (hyn_tool_data->hw_info.fw_sensor_rxnum + hyn_tool_data->hw_info.fw_sensor_txnum)*4;
	u8 *cmd = hyn_tool_data->host_cmd_save;
	mutex_lock(&hyn_tool_data->mutex_fs);

	if(*f_pos != 0)
		return -EFAULT;
		
    buf = kzalloc(max_len, GFP_KERNEL);
    if(buf == NULL){
        HYN_INFO("zalloc GFP_KERNEL memory failed.\n");
        return -ENOMEM;
    }
	if(hyn_tool_data->work_mode == DIFF_MODE
		|| hyn_tool_data->work_mode == RAWDATA_MODE
		|| hyn_tool_data->work_mode == FAC_TEST_MODE
	){
		// wait_event_interruptible(hyn_tool_data->wait_irq,(atomic_read(&hyn_tool_data->hyn_irq_flg)==ENABLE)); 
		wait_event_interruptible_timeout(hyn_tool_data->wait_irq,(atomic_read(&hyn_tool_data->hyn_irq_flg)==1),msecs_to_jiffies(120)); //  
		atomic_set(&hyn_tool_data->hyn_irq_flg,0);
		if(hyn_tool_data->work_mode == DIFF_MODE){
			ret_len = hyn_tool_fun->tp_get_differ(buf,max_len);
		}
		else if(hyn_tool_data->work_mode == RAWDATA_MODE){
			ret_len = hyn_tool_fun->tp_get_rawdata(buf,max_len);
		}
		else if(hyn_tool_data->work_mode == FAC_TEST_MODE){
			ret_len = hyn_tool_fun->tp_get_test_result(buf,max_len);
		}
		ret = copy_to_user(user_buf,buf,count);
	}
	else{
		if(cmd[0] == READ_REG){  //0:head 1~4:reg 5:reg_len  6~7:read_len
			u32 reg_addr = U8TO32(cmd[4],cmd[3],cmd[2],cmd[1]); //eg (read A1 A2 len 256):2A 00 00 A1 A2 02 FF 00
			ret_len = (cmd[7]<<8)|cmd[6];
			ret = hyn_wr_reg(hyn_tool_data,reg_addr,cmd[5],buf,ret_len);
			ret |= copy_to_user(user_buf,buf,ret_len);
		}
		else if(cmd[0] == READ_IIC){ //0:head 1~2:read_len
			ret_len = (cmd[2]<<8)|cmd[1];
			ret = hyn_read_data(hyn_tool_data,buf,ret_len);
			ret |= copy_to_user(user_buf,buf,ret_len);
		}
		else if(cmd[0] == READ_TPINF){
			ret_len = sizeof(hyn_tool_data->hw_info);
			ret = copy_to_user(user_buf,&hyn_tool_data->hw_info,ret_len);
		}
	}
	if(!IS_ERR_OR_NULL(buf)){	
		kfree(buf);
	}
	mutex_unlock(&hyn_tool_data->mutex_fs);
	HYN_INFO("count = %d copy len = %d ret = %d\r\n",(int)count,ret_len,ret);
	return ret < 0 ? -EFAULT:count;
}

static ssize_t hyn_proc_tool_write(struct file *file, const char __user *buffer,size_t count, loff_t *data)
{
    u8 *cmd = NULL,*pdata = NULL;
	int ret = 0;
	HYN_ENTER();
	mutex_lock(&hyn_tool_data->mutex_fs);
	#define MAX_W_SIZE (1024+8)
	if(count > MAX_W_SIZE){
		ret = -1;
		HYN_ERROR("count > MAX_W_SIZE\n");
		goto ERRO_WEND;
	}
	cmd = kzalloc(count, GFP_KERNEL);
	if(IS_ERR_OR_NULL(cmd)){
		ret = -1;
		HYN_ERROR("kzalloc failed\n");
		goto ERRO_WEND;
	}
	if(copy_from_user(cmd, buffer, count)){
		ret = -1;
		HYN_ERROR("copy data from user space failed.\n");
		goto ERRO_WEND;
	}
	memcpy(hyn_tool_data->host_cmd_save,cmd,count <sizeof(hyn_tool_data->host_cmd_save) ? count:sizeof(hyn_tool_data->host_cmd_save));
	HYN_INFO("cmd:0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x***len:%d ", cmd[0], cmd[1],cmd[2], cmd[3],cmd[4], cmd[5],cmd[6], cmd[7],(u16)count);

	switch(cmd[0]){
		case 0: // apk download bin
			 HYN_INFO("updata file_name =%s",&cmd[1]);
			 if(hyn_read_fw_file(&cmd[1], &pdata, &hyn_tool_data->fw_updata_len)){
				 ret = -1;
				 HYN_ERROR("hynitron_read %s failed.\n",&cmd[1]);
			 }
			 else if(!IS_ERR_OR_NULL(pdata)){
				ret = hyn_tool_fun->tp_updata_fw(pdata,hyn_tool_data->fw_updata_len);
				kfree(pdata);
			 }
			 break;
		case 2://
			atomic_set(&hyn_tool_data->hyn_irq_flg,0);
			if(cmd[1]==2){ //differ
				ret = hyn_tool_fun->tp_set_workmode(DIFF_MODE,0);
			}
			else if(cmd[1]==3){ //rawdata
				ret = hyn_tool_fun->tp_set_workmode(RAWDATA_MODE,0);
			}
			else if(cmd[1]==4){ //fac test
				ret = hyn_tool_fun->tp_set_workmode(FAC_TEST_MODE,0);
			}
			break;
		case 3: //nomal mode
			hyn_tool_fun->tp_set_workmode(NOMAL_MODE,0);
			break;
		case RESET_IC:
			hyn_tool_fun->tp_rest();
			break;
		case WRITE_IIC: //
			ret = hyn_write_data(hyn_tool_data,&cmd[1],count-1);
			break;
		default:
			break;
	}
ERRO_WEND:
	if(!IS_ERR_OR_NULL(cmd)){
		kfree(cmd);
	}
	mutex_unlock(&hyn_tool_data->mutex_fs);
	return ret == 0 ? count:-1;
}

static const struct file_operations proc_tool_fops = {
	.owner		= THIS_MODULE,
	.read	    = hyn_proc_tool_read,
	.write		= hyn_proc_tool_write, 	
};

int hyn_tool_fs_int(struct hyn_ts_data *ts_data)
{
	int ret = 0;
    hyn_tool_data = ts_data;
    hyn_tool_fun = ts_data->hyn_fuc_used;
	g_tool_dir = proc_mkdir(HYN_PROC_DIR_NAME, NULL);	
	if (IS_ERR_OR_NULL(g_tool_dir)){
		ret = -ENOMEM;
		HYN_INFO("proc_mkdir HYNITRON_PROC_DIR_NAME failed.\n");
		return -1;
	}
	g_tool_node = proc_create_data(HYN_PROC_NODE_NAME, 0777 | S_IFREG, g_tool_dir, &proc_tool_fops, (void *)ts_data->client);
	if (IS_ERR_OR_NULL(g_tool_node)) {
		ret = -ENOMEM;
		HYN_INFO("proc_create_data HYNITRON_PROC_FILE_NAME failed.\n");
		goto no_foo;
	}
	HYN_INFO("apk node creat success");
	return 0;
no_foo:
	remove_proc_entry(HYN_PROC_DIR_NAME, g_tool_dir);
	return ret;
}

void hyn_tool_fs_exit(void)
{
	if(!IS_ERR_OR_NULL(g_tool_node)){
		remove_proc_entry(HYN_PROC_DIR_NAME, g_tool_node);
		HYN_INFO("apk node remove");
	}
	if(!IS_ERR_OR_NULL(g_tool_dir)){
		proc_remove(g_tool_dir);
		HYN_INFO("apk dir remove");
	}
}

#endif

