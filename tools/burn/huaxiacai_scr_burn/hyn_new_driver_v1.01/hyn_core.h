#ifndef HYNITRON_CORE_H
#define HYNITRON_CORE_H

#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/spi/spi.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/delay.h>
#include <linux/regulator/consumer.h>
#include <linux/pinctrl/consumer.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/fb.h>
#include <linux/mutex.h>
#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/time.h>
#include <linux/jiffies.h>
#include <linux/string.h>
#include <linux/vmalloc.h>

#include <linux/sysfs.h>
#include <linux/fcntl.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>
#include <linux/moduleparam.h>
#if defined(CONFIG_FB)
#include <linux/notifier.h>
#include <linux/fb.h>
#elif defined(CONFIG_HAS_EARLYSUSPEND)
#include <linux/earlysuspend.h>
#endif

#include "hyn_cfg.h"

#if HYN_UART_DEBUG_EN
#define HYN_ENTER()     printk(KERN_INFO "[HYN][enter]%s\n",__func__)
#define HYN_INFO(fmt, args...)  printk(KERN_INFO "[HYN]"fmt"\n", ##args)//printk(KERN_INFO "[HYN]"fmt"\n", ##args)
#define HYN_ERROR(fmt, args...) printk(KERN_ERR "[HYN][Error]%s:"fmt"\n",__func__,##args)
#else
#define HYN_ENTER(fmt, args...)
#define HYN_INFO(fmt, args...)
#define HYN_ERROR(fmt, args...)
#endif

#define DISABLE (0)
#define ENABLE  (1)
#define U8TO16(x,y) ((x<<8)|y)
#define U8TO32(x1,x2,x3,x4) (((x1)<<24)|((x2)<<16)|((x3)<<8)|(x4))
#define FALSE (-1)
#define TRUE  (0)

typedef signed char s8;
// typedef signed char bool;
typedef unsigned char u8;
typedef unsigned char uint8_t;
typedef signed short s16;
typedef unsigned short u16;
typedef unsigned short uint16_t;
typedef signed int s32;
typedef unsigned int u32;
typedef unsigned int uint32_t;
typedef signed long long s64;
typedef unsigned long long u64;

enum work_mode{
    NOMAL_MODE = 0,
    GESTURE_MODE = 1,
    LP_MODE = 2,
    DEEPSLEEP = 3,
    DIFF_MODE = 4,
    RAWDATA_MODE = 5,
    FAC_TEST_MODE = 6,
};

enum report_typ{
    REPORT_NONE = 0,
    REPORT_POS = 0x01,
    REPORT_KEY = 0x02,
    REPORT_GES = 0x04,
    REPORT_PROX = 0x08
};

enum fac_test_ero{
    FAC_TEST_PASS = 0,
    FAC_GET_DATA_FAIL = -1,
    FAC_GET_CFG_FAIL = -4,
    FAC_TEST_OPENL_FAIL = -5,
    FAC_TEST_OPENH_FAIL = -7,
    FAC_TEST_SHORT_FAIL = -6,
};

struct hyn_plat_data {
    struct regulator *vdd_ana;
    struct regulator *vdd_i2c;

    int reset_gpio;
    u32 reset_gpio_flags;
    int irq_gpio;
    u32 irq_gpio_flags;

    struct pinctrl *pinctl;
    struct pinctrl_state *pin_active;
    struct pinctrl_state *pin_suspend;

    u32 x_resolution;
	u32 y_resolution;
    int swap_xy;
	int reverse_x;
	int reverse_y;

    int max_touch_num;
    int key_num;
	u32  key_x_coords[8]; // max support 8 keys
	u32  key_y_coords;
	u32  key_code[8];
};

struct hyn_chip_series{
    u32 part_no;
    u8* chip_name;
    u8* fw_bin;
};

struct ts_frame{
    u16 pos_x[MAX_POINTS_REPORT];
    u16 pos_y[MAX_POINTS_REPORT];
    u16 pres_z[MAX_POINTS_REPORT];
    u8 pos_id[MAX_POINTS_REPORT];
    u8 event[MAX_POINTS_REPORT];
    u8 rep_num;
    enum report_typ report_need;
    u8 key_id;
    u8 key_state;
};

struct tp_info{
    u8  fw_sensor_txnum;
    u8  fw_sensor_rxnum;
    u8  fw_key_num;
    u8  reserve;
    u16 fw_res_y;
    u16 fw_res_x;
    u32 fw_boot_time;
    u32 fw_project_id;
    u32 fw_chip_type;
    u32 fw_ver;
    u32 ic_fw_checksum;
    u32 fw_module_id;
};


struct hyn_ts_data {
    u16 bus_type;
#ifdef I2C_PORT
    struct i2c_client *client;
#else
    struct spi_device *client;
#endif
    struct device *dev;
    struct input_dev  *input_dev;
    struct workqueue_struct *hyn_workqueue;
    int gpio_irq;
    int esd_fail_cnt;
    u32 esd_last_value;
    enum work_mode work_mode;

    atomic_t irq_is_disable;
    atomic_t hyn_irq_flg;
    // spinlock_t irq_lock;
    struct mutex mutex_report;
    struct mutex mutex_bus;
    struct mutex mutex_fs;

    struct hyn_plat_data plat_data;
    struct tp_info hw_info;
    struct ts_frame rp_buf;

    struct work_struct  work_report;
    struct work_struct  work_resume;
    struct delayed_work esdcheck_work;

    struct work_struct  work_updata_fw;
    int need_updata_fw;
    u8 *fw_updata_addr;
    int fw_updata_len;
    u8 host_cmd_save[16];
    wait_queue_head_t wait_irq;

    u8 prox_mode_en;
    u8 prox_state;
    
    u8 gesture_is_enable;
    u8 gesture_id;
    const void *hyn_fuc_used;
#if defined(CONFIG_FB)
    struct notifier_block fb_notif;
#elif defined(CONFIG_HAS_EARLYSUSPEND)
    struct early_suspend early_suspend;
#endif
    struct kobject *sys_node;
};

struct hyn_ts_fuc{
    void (*tp_rest)(void);
    int (*tp_report)(void);
    int (*tp_supend)(void);
    int (*tp_resum)(void);
    int (*tp_chip_init)(struct hyn_ts_data *ts_data);
    int (*tp_updata_fw)(u8 *bin_addr, u16 len);
    int (*tp_set_workmode)(enum work_mode mode,u8 enable);
    u32 (*tp_check_esd)(void);
    int (*tp_prox_handle)(u8 cmd);
    int (*tp_get_rawdata)(u8 *buf,u16 len);
    int (*tp_get_differ)(u8 *buf,u16 len);
    int (*tp_get_test_result)(u8 *buf,u16 len);
};


int hyn_write_data(struct hyn_ts_data *ts_data, u8 *buf, u16 len);
int hyn_read_data(struct hyn_ts_data *ts_data,u8 *buf, u16 len);
int hyn_wr_reg(struct hyn_ts_data *ts_data, u32 reg_addr, u8 size, u8 *rbuf, u16 rlen);

void hyn_irq_set(struct hyn_ts_data *ts_data, u8 value);
void hyn_esdcheck_switch(struct hyn_ts_data *ts_data, u8 enable);
int hyn_read_fw_file(unsigned char *filename, unsigned char **pdata, int *plen);
int get_word(u8 **sc_str, u8* ds_str);
void hyn_set_i2c_addr(struct hyn_ts_data *ts_data,u8 addr);

int hyn_proc_fs_int(struct hyn_ts_data *ts_data);
void hyn_proc_fs_exit(void);
int hyn_tool_fs_int(struct hyn_ts_data *ts_data);
void hyn_tool_fs_exit(void);

int hyn_create_sysfs(struct hyn_ts_data *ts_data);
void hyn_release_sysfs(void);
int hyn_gesture_init(struct hyn_ts_data *ts_data);
void hyn_gesture_exit(void);

int hyn_wait_irq_timeout(struct hyn_ts_data *ts_data,int msec);
int hyn_get_threshold(char *filename,char *match_string,s16 *pstore, u16 len);
int fac_test_log_save(char *log_name,struct hyn_ts_data *ts_data,s16 *test_data, int uper_ret);
int str_2_num(char *str,u8 type);


//ic type
extern const struct hyn_ts_fuc cst1xx_fuc;
extern const struct hyn_ts_fuc cst3xx_fuc;
extern const struct hyn_ts_fuc cst66xx_fuc;
extern const struct hyn_ts_fuc cst7xx_fuc;
extern const struct hyn_ts_fuc cst92xx_fuc;
extern const struct hyn_ts_fuc cst8xxS_fuc;
extern const struct hyn_ts_fuc cst8xxT_fuc;
extern const struct hyn_ts_fuc cst3240_fuc;

#endif
