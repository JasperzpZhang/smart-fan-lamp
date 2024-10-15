
#include "hyn_core.h"

void hyn_irq_set(struct hyn_ts_data *ts_data, u8 value)
{
	// HYN_ENTER();
    if(atomic_read(&ts_data->irq_is_disable) != value){
        if(value ==0)
            disable_irq(ts_data->gpio_irq);
        else
            enable_irq(ts_data->gpio_irq);
        atomic_set(&ts_data->irq_is_disable,value);
		// HYN_INFO("IRQ %d",value);
    }
}

void hyn_set_i2c_addr(struct hyn_ts_data *ts_data,u8 addr)
{
#ifdef I2C_PORT  
	ts_data->client->addr = addr;
#endif
}


void hyn_esdcheck_switch(struct hyn_ts_data *ts_data, u8 enable)
{
#if ESD_CHECK_EN
    if(enable){
        ts_data->esd_fail_cnt = 0;
        queue_delayed_work(ts_data->hyn_workqueue, &ts_data->esdcheck_work,
                           msecs_to_jiffies(1000));
    }
    else{
        cancel_delayed_work_sync(&ts_data->esdcheck_work);
    }
#endif
}


int hyn_read_fw_file(unsigned char *filename, unsigned char **pdata, int *plen)
{
	struct file *fp;
	mm_segment_t old_fs;
	//int size;
	//int length;
	int ret = -1;
	loff_t pos;
	off_t fsize;
	struct inode *inode;
	unsigned long magic;
	
	HYN_ENTER();

	if(strlen(filename) == 0) 
		return ret;
		
	fp = filp_open(filename, O_RDONLY, 0);
	if (IS_ERR(fp)) {
		HYN_INFO("error occured while opening file %s.\n", filename);
		return -EIO;
	}
    fp->f_op->llseek(fp, 0, 0);

	inode = fp->f_inode;
	magic = inode->i_sb->s_magic;
	fsize = inode->i_size;		
	*pdata = kzalloc(fsize, GFP_KERNEL);
	if(IS_ERR_OR_NULL(*pdata)){
		HYN_ERROR("kzalloc failed");
		filp_close(fp, NULL);
		return -1;
	}
	old_fs = get_fs();
	set_fs(KERNEL_DS);
	pos = 0;
	ret=vfs_read(fp, *pdata, fsize, &pos);

	if(ret==fsize){
		HYN_INFO("vfs_read success.ret:%d.\n",ret);
	}

	filp_close(fp, NULL);
	set_fs(old_fs);
	
	HYN_INFO("vfs_read done");
	return 0;
}

		
int hyn_wait_irq_timeout(struct hyn_ts_data *ts_data,int msec)
{
	atomic_set(&ts_data->hyn_irq_flg,0);
	while(msec--){
		msleep(1);
		if(atomic_read(&ts_data->hyn_irq_flg)==1){
			atomic_set(&ts_data->hyn_irq_flg,0);
			msec = -1;
			break;
		}
	}
	return msec == -1 ? 0:-1;
}

#define MAX_WROD_LEN	(64)
int get_word(u8**sc_str, u8* ds_str)
{
	u8 ch,cnt = 0,nul_flg = 0; 
	while(1){
		ch = **sc_str;
		*sc_str += 1;
		if((ch==' '&& nul_flg) || ch=='\t' || ch=='\0' || (ch=='\r' && **sc_str == '\n') || ch=='\n'|| ch==','|| ch=='=' || cnt>=MAX_WROD_LEN){
			*ds_str++ = '\0';
			break;
		}
		if(ch >= 'A' && ch <= 'Z') ch = ch + 'a' - 'A';
		if(ch!=' '){
			*ds_str++ = ch;
			nul_flg = 1;
		}
		cnt++;
	}
	return cnt;
}

int str_2_num(char *str,u8 type)
{
	int ret = 0,step = 0,flg = 0,cnt = 15;
	char ch;
	while(*str != '\0' && --cnt){
		ch = *str++;
		if(ch==' ') continue;
		else if(ch=='-' && step==0){
			flg = 1;
			continue;
		}
		if(type == 10){
			if(ch <= '9' && ch >= '0'){
				step++;
				ret *= 10;
				ret += (ch - '0');
			}
			else{
				cnt = 0;
				break;
			}
		}
		else{
			if(ch <= '9' && ch >= '0'){
				step++;
				ret *= 16;
				ret += (ch - '0');
			}
			else if(ch <= 'f' && ch >= 'a'){
				step++;
				ret *= 16;
				ret += (10 + ch - 'a');
			}
			else{
				cnt = 0;
				break;
			}
		}
	}
	// if(cnt == 0 || step==0)HYN_ERROR("failed");
	return (cnt == 0 || step==0) ? 0xAC5AC5AC : (flg ? -ret:ret);
}

int hyn_get_threshold(char *filename,char *match_string,s16 *pstore, u16 len)
{
	int ret = 0,cnt;
	u8 *buf = NULL,*ptr = NULL;
	u8 tmp_word[66];
	u16 i;
	int dec;
	off_t fsize;
	mm_segment_t old_fs;
	struct file *fp;
	loff_t pos;
	HYN_ENTER();

	if(strlen(filename) == 0) 
		return ret;
		
	fp = filp_open(filename, O_RDONLY, 0);
	if (IS_ERR(fp)) {
		HYN_INFO("error occured while opening file %s.\n", filename);
		return -EIO;
	}
	fp->f_op->llseek(fp, 0, 0);
	fsize = fp->f_inode->i_size;
	buf = vmalloc(fsize);
	if(IS_ERR_OR_NULL(buf)){
		HYN_ERROR("vmalloc failed");
		filp_close(fp, NULL);
		return -1;
	}
	old_fs = get_fs();
	set_fs(KERNEL_DS);
	pos = 0;
	ret=vfs_read(fp,buf, fsize, &pos);
	if(ret==fsize){
		HYN_INFO("read %s success.ret:%d.\n",filename,ret);
	}
	else{
		HYN_ERROR("read cfg failed ret = %d fsize = %d",ret,(int)fsize);
	}
	set_fs(old_fs);
	filp_close(fp, NULL);

////read finsh start match key string
	i = fsize;
	ptr = buf;
	while(--i){
		if(*ptr++ == '\n'){
			cnt = strlen(match_string);
			while(--cnt){
				if(ptr[cnt]!= match_string[cnt]) break;
			}
			if(cnt ==0) break; //ignor idx 0,1
		}
	}
	if(i == 0){
		ret = -1;
		goto MATCH_FAIL1;
	}

///////match key string end
	i = 0;
	while(i<10){
		cnt = get_word(&ptr,tmp_word);
		if(cnt==0){
			i++;
			continue;
		}
		// HYN_INFO("@@%s",tmp_word);
		dec = str_2_num(tmp_word,10);
		if(dec == 0xAC5AC5AC) continue;
		*pstore = dec;
		pstore++;
		i = 0;
		if(--len == 0) break;
	}
	ret = len ? -1:0;

MATCH_FAIL1:
	if(!IS_ERR_OR_NULL(buf)) vfree(buf);

	return ret;
}

int arry_to_string(s16 *from,u16 cnt,u8 *des,u16 maxlen)
{
	int str_cnt = 0,j;
	int ret;
	while(cnt--){
		ret = snprintf(&des[str_cnt],maxlen, "%d,",*from++);
		str_cnt += ret;
		if(str_cnt > maxlen-ret){
			str_cnt = maxlen;
			HYN_ERROR("str full");
			break;
		}
	}
	return str_cnt;
}

int fac_test_log_save(char *log_name,struct hyn_ts_data *ts_data,s16 *test_data, int uper_ret)
{
	mm_segment_t old_fs;
	struct file *fp;
	u8 w_buf[256];
	int ret = 0;
	u16 str_cnt;
	u8 i,j;

	HYN_ENTER();
	if(strlen(log_name) == 0) 
		return ret;
		
	fp = filp_open(log_name, O_RDWR | O_CREAT, 0644);
	if (IS_ERR(fp)) {
		HYN_INFO("error occured while opening file %s.\n", log_name);
		return -EIO;
	}
	old_fs = get_fs(); 
	set_fs(KERNEL_DS);
	ret = snprintf(w_buf,sizeof(w_buf), uper_ret ==0 ? "factory test pass\n":"factory test ng\n");
	vfs_write(fp, w_buf, ret, &fp->f_pos); 
	if(uper_ret == FAC_GET_DATA_FAIL){
		ret = snprintf(w_buf,sizeof(w_buf), "read fac_test data fail\n");
		vfs_write(fp, w_buf, ret, &fp->f_pos); 
	}
	else{
		ret = 0;
		if(uper_ret == FAC_GET_CFG_FAIL){
			ret = snprintf(w_buf,sizeof(w_buf), "read fac_test cfg fail\n");
		}
		else if(uper_ret == FAC_TEST_OPENL_FAIL){
			ret = snprintf(w_buf,sizeof(w_buf), "open low test fail\n");
		}
		else if(uper_ret == FAC_TEST_OPENH_FAIL){
			ret = snprintf(w_buf,sizeof(w_buf), "open high test fail\n");
		}
		else if(uper_ret == FAC_TEST_SHORT_FAIL){
			ret = snprintf(w_buf,sizeof(w_buf), "short test fail\n");
		}
		if(ret) vfs_write(fp, w_buf, ret, &fp->f_pos); 

		for(i = 0; i< ts_data->hw_info.fw_sensor_txnum; i++){
			ret = arry_to_string(test_data,ts_data->hw_info.fw_sensor_rxnum,w_buf,sizeof(w_buf)-2);
			w_buf[ret+1] = '\n';
			vfs_write(fp, w_buf, ret+2, &fp->f_pos); 
			test_data += ts_data->hw_info.fw_sensor_rxnum;
		}
		ret = arry_to_string(test_data,ts_data->hw_info.fw_sensor_txnum,w_buf,sizeof(w_buf)-2);
		w_buf[ret+1] = '\n';
		vfs_write(fp, w_buf, ret+2, &fp->f_pos); 
		test_data += ts_data->hw_info.fw_sensor_txnum;

		ret = arry_to_string(test_data,ts_data->hw_info.fw_sensor_rxnum,w_buf,sizeof(w_buf)-2);
		w_buf[ret+1] = '\n';
		vfs_write(fp, w_buf, ret+2, &fp->f_pos); 
	}
	set_fs(old_fs);
	filp_close(fp, NULL);
	return 0;
}






