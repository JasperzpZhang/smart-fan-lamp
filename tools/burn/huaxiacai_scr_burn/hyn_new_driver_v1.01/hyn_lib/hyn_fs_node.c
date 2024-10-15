
#include "hyn_core.h"
#include <linux/string.h>
#include <linux/kernel.h>


static struct hyn_ts_data *hyn_fs_data = NULL;

///cat hyntpfwver
static ssize_t hyn_tpfwver_show(struct device *dev,	struct device_attribute *attr,char *buf)
{
	ssize_t num_read = 0;
    HYN_ENTER();
	num_read = snprintf(buf, 128, "project_version:0x%02X,chip_type:0x%02X,checksum:0x%02X .\n",
			   hyn_fs_data->hw_info.fw_ver,
			   hyn_fs_data->hw_info.fw_chip_type,
			   hyn_fs_data->hw_info.ic_fw_checksum);
	return num_read;
}

static ssize_t hyn_tpfwver_store(struct device *dev,struct device_attribute *attr,const char *buf, size_t count)
{
	/*place holder for future use*/
	return -EPERM;
}

//echo fd /sdcard/app.bin
//echo w d1 01 
//echo w d1 01 r 2 
//echo w 1 2 3 4 5 
//echo rst
#define DEBUG_BUF_SIZE (256)

static ssize_t hyn_dbg_show(struct device *dev,struct device_attribute *attr,char *buf)
{
	ssize_t len = 0;
    int ret = -1;
	u8 *kbuf =NULL;
	u16 i;
	HYN_ENTER();
	mutex_lock(&hyn_fs_data->mutex_fs);
	kbuf = kzalloc(DEBUG_BUF_SIZE, GFP_KERNEL);

	if(hyn_fs_data->host_cmd_save[0]==0x5A){
		if(hyn_fs_data->host_cmd_save[1] > 4) hyn_fs_data->host_cmd_save[1] = 4;
		len = hyn_fs_data->host_cmd_save[6];
		if(len != 0 && hyn_fs_data->host_cmd_save[1] !=0){
			u32 reg;
			reg = hyn_fs_data->host_cmd_save[2];
			i = 0;
			while(++i < hyn_fs_data->host_cmd_save[1]){
				reg <<= 8;
				reg |= hyn_fs_data->host_cmd_save[2+i];
			}
			ret = hyn_wr_reg(hyn_fs_data,reg,hyn_fs_data->host_cmd_save[1],kbuf,(u16)len);
		}
	}
	else if(hyn_fs_data->host_cmd_save[0]==0x7A){
		len = hyn_fs_data->host_cmd_save[6];
		ret = hyn_read_data(hyn_fs_data,kbuf,(u16)len);
	}

	if(ret ==0){
		ssize_t cnt_char = 0;
		for(i = 0; i< len; i++){
			ret = sprintf(buf+cnt_char,"0x%02x ",*(kbuf+i));
			cnt_char += ret;
		}
		buf[cnt_char] = '\n';
		ret = cnt_char+1;
	}
	else{
		sprintf(buf,"fs read failed\n");
		ret = 13;
	}
	
	if(!IS_ERR_OR_NULL(kbuf)) kfree(kbuf);
	mutex_unlock(&hyn_fs_data->mutex_fs);

	return ret;
}

static ssize_t hyn_dbg_store(struct device *dev,struct device_attribute *attr,const char *buf, size_t count)
{
	u16 rec_len;
	u8 str[128];
	u8 *next_ptr;
	int val;
	const struct hyn_ts_fuc* hyn_fun = hyn_fs_data->hyn_fuc_used;
	int ret = 0;
	HYN_ENTER();
	mutex_lock(&hyn_fs_data->mutex_fs);
	rec_len = strlen(buf);
	HYN_INFO("rec_len =%d",rec_len);
	if(rec_len < 1 || rec_len > DEBUG_BUF_SIZE){
		HYN_INFO("erro write len");
		goto END;
	}
	next_ptr = (u8*)buf;
	str[0] = '\0';
	ret = get_word(&next_ptr,str);
	if(0 == strcmp(str,"fd")){ //updata fw
		u8 *pdata = NULL;
		ret = get_word(&next_ptr,str);
		if(strlen(str)<4){
			strcpy(str,"/sdcard/app.bin");
		}
		HYN_INFO("filename = %s",str);
		if(0 == hyn_read_fw_file(str, &pdata, &hyn_fs_data->fw_updata_len)){
			ret = hyn_fun->tp_updata_fw(pdata,hyn_fs_data->fw_updata_len);
			kfree(pdata);
		}
	}
	else if(0 == strcmp(str,"w")){
		u8 i = 0;
		u8 *kbuf = kzalloc(count/2, GFP_KERNEL);
		hyn_fs_data->host_cmd_save[0] = 0;
		hyn_fs_data->host_cmd_save[1] = 0xA5;
		hyn_fs_data->host_cmd_save[6] = 0;
		while(1){
			str[0] = '\0';
			ret = get_word(&next_ptr,str);
			// next_ptr += ret;
			HYN_INFO("read =%s ,len =%d",str,(int)strlen(str));
			if(0 == strcmp(str,"r")){
				ret = get_word(&next_ptr,str);
				val = str_2_num(str,16);
				if(ret < 0x100){
					hyn_fs_data->host_cmd_save[0] = 0x5A;
					hyn_fs_data->host_cmd_save[1] = i>4 ? 4:i;
					hyn_fs_data->host_cmd_save[6] = val;
					memcpy(&hyn_fs_data->host_cmd_save[2],kbuf,4);
					i = 0;
				}
				break;
			}
			if(ret == 0 || strlen(str)==0 || strlen(str)>2){
				break;
			}
			val = str_2_num(str,16);
			if(val > 0xFF) break;
			*(kbuf+i) = val;
			HYN_INFO(" %02x",val);
			i++;
		}
		if(i){
			hyn_write_data(hyn_fs_data,kbuf,i);
		}
		kfree(kbuf);
	}
	else if(0 == strcmp(str,"r")){
		ret = get_word(&next_ptr,str);
		val = str_2_num(str,16);
		if(val < 0x100){
			hyn_fs_data->host_cmd_save[0] = 0x7A;
			hyn_fs_data->host_cmd_save[6] = val;
		}
	}
	else if(0 == strcmp(str,"rst")){
		hyn_fun->tp_rest();
	}
	else if(0 == strcmp(str,"workmode")){
		ret = get_word(&next_ptr,str);
		if(ret){
			char mode = str[0]-'0';
			ret = get_word(&next_ptr,str);
			if(ret) hyn_fun->tp_set_workmode(mode,str[0]=='0'? 0:1);
		}
	}
END:
	mutex_unlock(&hyn_fs_data->mutex_fs);
	return count;
}

static DEVICE_ATTR(hyntpfwver, S_IRUGO | S_IWUSR, hyn_tpfwver_show,
			hyn_tpfwver_store);
static DEVICE_ATTR(hyntpdbg, S_IRUGO | S_IWUSR, hyn_dbg_show,
			hyn_dbg_store);

static struct attribute *hyn_attributes[] = {
	&dev_attr_hyntpfwver.attr,
	&dev_attr_hyntpdbg.attr,
	NULL
};

static struct attribute_group hyn_attribute_group = {
	.attrs = hyn_attributes
};

int hyn_create_sysfs(struct hyn_ts_data *ts_data)
{
	int ret = 0;
    hyn_fs_data = ts_data;
    ts_data->sys_node = kobject_create_and_add("hynitron_debug", NULL);
	if(IS_ERR_OR_NULL(ts_data->sys_node)){
		HYN_ERROR("kobject_create_and_add failed");
		return -1;
	}
    ret = sysfs_create_group(ts_data->sys_node, &hyn_attribute_group);
	if(ret){
		kobject_put(ts_data->sys_node);
		return -1;
	}
	HYN_INFO("sys_node creat success");
	return 0;
}

void hyn_release_sysfs(void)
{
    if(!IS_ERR_OR_NULL(hyn_fs_data->sys_node)){
		sysfs_remove_group(hyn_fs_data->sys_node, &hyn_attribute_group);
		kobject_put(hyn_fs_data->sys_node);
		hyn_fs_data->sys_node = NULL;
	}
}

