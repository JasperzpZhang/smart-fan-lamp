#include "hyn_core.h"


#define HYN_DRIVER_NAME  "hyn_ts"
static struct hyn_ts_data *hyn_data = NULL;
static const struct hyn_ts_fuc* hyn_fun = NULL;
static const struct hyn_ts_fuc* fuc_tbl[] = {
#ifdef chip_cst3xx
    &cst3xx_fuc,
#endif
#ifdef chip_cst1xx
    &cst1xx_fuc,
#endif
#ifdef chip_cst8xxS
    &cst8xxS_fuc,
#endif
#ifdef chip_cst8xxT
    &cst8xxT_fuc,
#endif
#if defined(chip_cst36xx) || defined(chip_cst66xx)
    &cst66xx_fuc,
#endif
#ifdef chip_cst92xx
    &cst92xx_fuc,
#endif
#ifdef chip_cst7xx
    &cst7xx_fuc,
#endif
#ifdef chip_cst3240
    &cst3240_fuc,
#endif
    NULL,
};

static int hyn_detect_each_ic(struct hyn_ts_data *ts_data)
{
    int ret = -1,index = 0;
    const struct  hyn_ts_fuc *hyn_fuc;
    while(1){
        hyn_fuc = fuc_tbl[index++];
        if(hyn_fuc != NULL)
            ret = hyn_fuc->tp_chip_init(ts_data);
        if(ret ==0 || hyn_fuc == NULL){
            if(ret == 0) {
                ts_data->hyn_fuc_used = hyn_fuc;
                return 0;
            }
            break;
        }
    }
    return -1;
}

static int hyn_parse_dt(struct hyn_ts_data *ts_data)
{
    int ret = 0;
    struct device *dev = ts_data->dev;
    struct hyn_plat_data* dt = &ts_data->plat_data;
    if(dev->of_node){
        u32 buf[8];
        struct device_node *np = dev->of_node;
        dt->vdd_i2c = regulator_get(dev, "vdd_ana");
        dt->vdd_ana = regulator_get(dev, "vcc_i2c");
        if(IS_ERR_OR_NULL(dt->vdd_i2c) || IS_ERR_OR_NULL(dt->vdd_ana)){
            HYN_ERROR("regulator_get failed");
            return -ENODEV;
        }

        dt->reset_gpio = of_get_named_gpio_flags(np, "reset-gpio", 0, &dt->reset_gpio_flags);
        dt->irq_gpio = of_get_named_gpio_flags(np, "irq-gpio", 0, &dt->irq_gpio_flags);
        if(dt->reset_gpio < 0 || dt->irq_gpio < 0){
            HYN_ERROR("dts get gpio failed");
            return -ENODEV;
        }
        else{
            HYN_INFO("reset_gpio:%d irq_gpio:%d",dt->reset_gpio,dt->irq_gpio);
        }

        dt->pinctl = devm_pinctrl_get(dev);
        if (IS_ERR_OR_NULL(dt->pinctl)) {
            HYN_ERROR("devm_pinctrl_get failed");
            return -ENODEV;
        }
        dt->pin_active = pinctrl_lookup_state(dt->pinctl, "ts_active");
        dt->pin_suspend= pinctrl_lookup_state(dt->pinctl, "ts_suspend");
        if(IS_ERR_OR_NULL(dt->pin_active) || IS_ERR_OR_NULL(dt->pin_suspend)){
            HYN_ERROR("dts get \"ts-active\" \"ts_suspend\" failed");
            return -EINVAL;
        }

        ret = of_property_read_u32(np, "max-touch-number", &dt->max_touch_num);
        ret |= of_property_read_u32(np, "pos-swap", &dt->swap_xy);
        ret |= of_property_read_u32(np, "posx-reverse", &dt->reverse_x);
        ret |= of_property_read_u32(np, "posy-reverse", &dt->reverse_y);
        ret |= of_property_read_u32_array(np, "display-coords", buf, 4);
        if(0 == dt->swap_xy){
            dt->x_resolution = buf[2];
            dt->y_resolution = buf[3];
        }
        else{
            dt->x_resolution = buf[3];
            dt->y_resolution = buf[2];
        }
        HYN_INFO("dts x_res = %d,y_res = %d,touch-number = %d",dt->x_resolution,dt->y_resolution,dt->max_touch_num);

        ret |= of_property_read_u32(np, "key-number", &dt->key_num);
        if(dt->key_num && dt->key_num<=8){
            ret |= of_property_read_u32(np, "key-y-coord", &dt->key_y_coords);
            ret |= of_property_read_u32_array(np, "key-x-coords", dt->key_x_coords, dt->key_num);
            ret |= of_property_read_u32_array(np, "keys", dt->key_code, dt->key_num);
        }

        if(ret < 0){
            HYN_ERROR("dts get pos or key failed");
            return -EINVAL;
        }
        return 0;
    }
    else{
        HYN_ERROR("dts match failed");
        return -ENODEV;
    }
}

static int hyn_poweron(struct hyn_ts_data *ts_data)
{
    int ret = 0;

    ret = pinctrl_select_state(ts_data->plat_data.pinctl, ts_data->plat_data.pin_active);
    if(ret){
        HYN_ERROR("pin active set failed");
        return -1;
    }

    ret = gpio_request(ts_data->plat_data.irq_gpio, "hyn_irq_gpio");
    ret |= gpio_request(ts_data->plat_data.reset_gpio, "hyn_reset_gpio");
    if(ret < 0){
        HYN_ERROR("gpio_request failed");
        return -1;
    }
    ret = gpio_direction_input(ts_data->plat_data.irq_gpio);
    ret |= gpio_direction_output(ts_data->plat_data.reset_gpio, 0);

    if(IS_ERR_OR_NULL(ts_data->plat_data.vdd_ana)){
        ret |= regulator_set_voltage(ts_data->plat_data.vdd_ana, 2800000, 3300000);
    }
    if(IS_ERR_OR_NULL(ts_data->plat_data.vdd_i2c)){
        ret |= regulator_set_voltage(ts_data->plat_data.vdd_i2c, 1800000, 1800000);
    }

    mdelay(5);
    gpio_set_value(ts_data->plat_data.reset_gpio, 1);

    if(ret) {
		gpio_free(ts_data->plat_data.irq_gpio);
        gpio_free(ts_data->plat_data.reset_gpio);
        HYN_ERROR("gpio set failed");
    }
    return ret;
}

static int hyn_input_dev_init(struct hyn_ts_data *ts_data)
{
    int ret = 0;
    int key_num = 0;
    struct hyn_plat_data *dt = &ts_data->plat_data;
    struct input_dev *input_dev;

     HYN_INFO("hyn_input_dev_init enter ");

    input_dev = input_allocate_device();
    if (!input_dev) {
        HYN_ERROR("Failed to allocate memory for input device");
        return -ENOMEM;
    }
    input_dev->name = HYN_DRIVER_NAME;
    input_dev->id.bustype = ts_data->bus_type;
    input_dev->dev.parent = ts_data->dev;
    input_set_drvdata(input_dev, ts_data);

    __set_bit(EV_SYN, input_dev->evbit);
    __set_bit(EV_ABS, input_dev->evbit);
    __set_bit(EV_KEY, input_dev->evbit);
    __set_bit(BTN_TOUCH, input_dev->keybit);
    __set_bit(INPUT_PROP_DIRECT, input_dev->propbit);

    for (key_num = 0; key_num < dt->key_num; key_num++)
			input_set_capability(input_dev, EV_KEY, dt->key_code[key_num]);
#if HYN_MT_PROTOCOL_B_EN
    set_bit(BTN_TOOL_FINGER,input_dev->keybit);
    //input_mt_init_slots(input_dev, dt->max_touch_num);
    input_mt_init_slots(input_dev, dt->max_touch_num, INPUT_MT_DIRECT);
#else
    input_dev->keybit[BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH);
#endif

	input_set_abs_params(input_dev, ABS_MT_TRACKING_ID, 0,  dt->max_touch_num, 0, 0); 
	input_set_abs_params(input_dev, ABS_MT_POSITION_X, 0, dt->x_resolution,0, 0);
	input_set_abs_params(input_dev, ABS_MT_POSITION_Y, 0, dt->y_resolution,0, 0);
	input_set_abs_params(input_dev, ABS_MT_TOUCH_MAJOR, 0, 255, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_WIDTH_MAJOR, 0, 200, 0, 0);
    input_set_abs_params(input_dev, ABS_MT_PRESSURE, 0, 0xFF, 0, 0);

    ret = input_register_device(input_dev);
    if(ret){
        input_set_drvdata(input_dev, NULL);
        input_free_device(input_dev);
        HYN_ERROR("input_register_device failed");
        return ret;
    }
    
    ts_data->input_dev = input_dev;
    return 0;
}

static void release_all_finger(struct hyn_ts_data *ts_data)
{
#if HYN_MT_PROTOCOL_B_EN
    u8 i;
	for(i=0; i< ts_data->plat_data.max_touch_num; i++) {	
		input_mt_slot(ts_data->input_dev, i);
		input_report_abs(ts_data->input_dev, ABS_MT_TRACKING_ID, -1);
		input_mt_report_slot_state(ts_data->input_dev, MT_TOOL_FINGER, false);
	}
	input_report_key(ts_data->input_dev, BTN_TOUCH, 0);
#else
    input_report_key(ts_data->input_dev, BTN_TOUCH, 0);
	input_mt_sync(ts_data->input_dev);
#endif
    HYN_INFO("release_all_finger");
}

static void touch_updata(u8 idx,u8 event)
{
    struct ts_frame *rep_frame = &hyn_data->rp_buf;
    struct input_dev *dev = hyn_data->input_dev;
    u16 zpress = rep_frame->pres_z[idx];
#if HYN_MT_PROTOCOL_B_EN
    if(event){
        input_mt_slot(dev, rep_frame->pos_id[idx]);
        input_mt_report_slot_state(dev, MT_TOOL_FINGER, 1);
        input_report_abs(dev, ABS_MT_TRACKING_ID, rep_frame->pos_id[idx]);
        input_report_abs(dev, ABS_MT_POSITION_X, rep_frame->pos_x[idx]);
        input_report_abs(dev, ABS_MT_POSITION_Y, rep_frame->pos_y[idx]);
        input_report_abs(dev, ABS_MT_TOUCH_MAJOR, zpress>>3);
        input_report_abs(dev, ABS_MT_WIDTH_MAJOR, zpress>>3);
        input_report_abs(dev, ABS_MT_PRESSURE, zpress);
    }
    else{
        input_mt_slot(dev, rep_frame->pos_id[idx]);
        input_report_abs(dev, ABS_MT_TRACKING_ID, -1);
        input_mt_report_slot_state(dev, MT_TOOL_FINGER, 0);
    }
#else
    if(event){
        input_report_key(dev, BTN_TOUCH, 1);
        input_report_abs(dev, ABS_MT_PRESSURE, zpress);
        input_report_abs(dev, ABS_MT_TRACKING_ID, rep_frame->pos_id[idx]);
        input_report_abs(dev, ABS_MT_TOUCH_MAJOR, zpress>>3);
        // input_report_abs(dev, ABS_MT_WIDTH_MAJOR, zpress>>3);
        input_report_abs(dev, ABS_MT_POSITION_X, rep_frame->pos_x[idx]);
        input_report_abs(dev, ABS_MT_POSITION_Y, rep_frame->pos_y[idx]);
        input_mt_sync(dev);
    }
    else{
        input_report_key(dev, BTN_TOUCH, 0);
        input_mt_sync(dev);
    }
#endif
}

static void hyn_irq_report_work(struct work_struct *work)
{
    struct hyn_ts_data *ts_data = hyn_data;
    struct ts_frame *rep_frame = &hyn_data->rp_buf;
    struct input_dev *dev = hyn_data->input_dev;
    struct hyn_plat_data *dt = &hyn_data->plat_data;
    u16 xpos,ypos;
    hyn_fun->tp_report();
    mutex_lock(&ts_data->mutex_report);
    if(rep_frame->report_need & REPORT_KEY){ //key
#if KEY_USED_POS_REPORT
        rep_frame->pos_id[0] = 0;
        rep_frame->pos_x[0] = dt->key_x_coords[rep_frame->key_id];
        rep_frame->pos_y[0] = dt->key_y_coords;
        rep_frame->pres_z[0] = 100;
        touch_updata(0,rep_frame->key_state ? 1:0);
#else
        input_report_key(dev,dt->key_code[rep_frame->key_id],rep_frame->key_state ? 1:0); 
#endif
        input_sync(dev);
        // HYN_INFO("report key");
    }

    if(rep_frame->report_need & REPORT_POS){ //pos
        u8 i;
        if(rep_frame->rep_num == 0){
            release_all_finger(ts_data);
        }
        else{
            u8 touch_down = 0;
            for(i = 0; i < rep_frame->rep_num; i++){
                if(dt->swap_xy){
                    xpos = rep_frame->pos_y[i];
                    ypos = rep_frame->pos_x[i];
                }
                else{
                    xpos = rep_frame->pos_x[i];
                    ypos = rep_frame->pos_y[i];
                }
                if(ypos > dt->y_resolution || xpos > dt->x_resolution){
                    HYN_ERROR("ypos or xpos to big");
                }
                if(dt->reverse_x){
                    xpos = dt->x_resolution-xpos;
                }
                if(dt->reverse_y){
                    ypos = dt->y_resolution-ypos;
                }
                rep_frame->pos_x[i] = xpos;
                rep_frame->pos_y[i] = ypos;
                touch_updata(i,rep_frame->event[i]? 1:0);
                if(rep_frame->event[i]) touch_down++;
            }
#if HYN_MT_PROTOCOL_B_EN
            input_report_key(dev, BTN_TOUCH, touch_down ? 1:0);
#endif
        }
        input_sync(dev);
    }
#if (HYN_GESTURE_EN)
    else if(rep_frame->report_need & REPORT_GES){
        if(ts_data->gesture_id != 0xFF){
            input_report_key(dev, ts_data->gesture_id, 1);
            input_sync(dev);
            input_report_key(dev, ts_data->gesture_id, 0);
            input_sync(dev);
        }
    }
#endif 

    if(rep_frame->report_need == REPORT_NONE){
        release_all_finger(ts_data);
    }
    mutex_unlock(&ts_data->mutex_report);
}

static void hyn_esdcheck_work(struct work_struct *work)
{
    HYN_ENTER(); 
#if ESD_CHECK_EN
    int ret;
    ret = hyn_fun->tp_check_esd();
    if(hyn_data->esd_last_value != ret){
        hyn_data->esd_fail_cnt = 0;
        hyn_data->esd_last_value = ret;
    }
    else{
        hyn_data->esd_fail_cnt++;
        if(hyn_data->esd_fail_cnt > 2){
            hyn_data->esd_fail_cnt = 0;
            hyn_fun->tp_rest();
        }
    }
    queue_delayed_work(hyn_data->hyn_workqueue, &hyn_data->esdcheck_work,
                           msecs_to_jiffies(1000));
#endif
}

static void hyn_resum(struct device *dev)
{
    int ret = 0;
    HYN_ENTER();
    pinctrl_select_state(hyn_data->plat_data.pinctl, hyn_data->plat_data.pin_active);
    hyn_fun->tp_resum();
    if(hyn_data->prox_mode_en){
        hyn_fun->tp_prox_handle(1);
    }
    else if(hyn_data->gesture_is_enable){
        hyn_irq_set(hyn_data,DISABLE);
        ret = disable_irq_wake(hyn_data->client->irq);
        ret |= irq_set_irq_type(hyn_data->client->irq,hyn_data->plat_data.irq_gpio_flags); 
        if(ret < 0){
            HYN_ERROR("gesture irq_set_irq failed");
        }  
        hyn_irq_set(hyn_data,ENABLE);
    }
}

static void hyn_suspend(struct device *dev)
{
    int ret = 0;
    HYN_ENTER();
    release_all_finger(hyn_data);
    input_sync(hyn_data->input_dev);
    if(hyn_data->prox_mode_en ==1){
    }
    else if(hyn_data->gesture_is_enable){
        hyn_irq_set(hyn_data,DISABLE);
        ret = enable_irq_wake(hyn_data->client->irq);
        ret |= irq_set_irq_type(hyn_data->client->irq,IRQF_TRIGGER_LOW|IRQF_NO_SUSPEND); 
        if(ret < 0){
            HYN_ERROR("gesture irq_set_irq failed");
        }  
        hyn_fun->tp_set_workmode(GESTURE_MODE,0);
        hyn_irq_set(hyn_data,ENABLE);
    }
    else{
        hyn_fun->tp_supend();
        pinctrl_select_state(hyn_data->plat_data.pinctl, hyn_data->plat_data.pin_suspend);
    }
}

static void hyn_updata_fw_work(struct work_struct *work)
{
    int ret = 0;
    ret = hyn_fun->tp_updata_fw(hyn_data->fw_updata_addr,hyn_data->fw_updata_len);
}

static void hyn_resum_work(struct work_struct *work)
{
    hyn_resum(hyn_data->dev);
}

static irqreturn_t hyn_irq_handler(int irq, void *data)
{
    if(hyn_data->work_mode < DIFF_MODE){
        queue_work(hyn_data->hyn_workqueue,&hyn_data->work_report);
    }
    else{
        atomic_set(&hyn_data->hyn_irq_flg,1);
        wake_up(&hyn_data->wait_irq);
    }
    return IRQ_HANDLED;
}

#if defined(CONFIG_FB)
static int fb_notifier_callback(struct notifier_block *self, unsigned long event, void *data)
{
    struct fb_event *evdata = data;
    int *blank = NULL;

    if (!(event == FB_EARLY_EVENT_BLANK || event == FB_EVENT_BLANK)) {
        HYN_INFO("event(%lu) do not need process\n", event);
        return 0;
    }

    blank = evdata->data;
    HYN_INFO("FB event:%lu,blank:%d", event, *blank);
    switch (*blank) {
    case FB_BLANK_UNBLANK:
        if (FB_EARLY_EVENT_BLANK == event) {
            HYN_INFO("resume: event = %lu, not care\n", event);
        } else if (FB_EVENT_BLANK == event) {
            queue_work(hyn_data->hyn_workqueue, &hyn_data->work_resume);
        }
        break;
    case FB_BLANK_POWERDOWN:
        if (FB_EARLY_EVENT_BLANK == event) {
            cancel_work_sync(&hyn_data->work_resume);
            hyn_suspend(hyn_data->dev);
        } else if (FB_EVENT_BLANK == event) {
            HYN_INFO("suspend: event = %lu, not care\n", event);
        }
        break;
    default:
        HYN_INFO("FB BLANK(%d) do not need process\n", *blank);
        break;
    }
    return 0;
}
#elif defined(CONFIG_HAS_EARLYSUSPEND)
static void hyn_ts_early_suspend(struct early_suspend *handler)
{
    hyn_suspend(hyn_data->dev);
}
static void hyn_ts_late_resume(struct early_suspend *handler)
{
    hyn_resum(hyn_data->dev);
}
#endif


#ifdef I2C_PORT
static int hyn_ts_probe(struct i2c_client *client, const struct i2c_device_id *id)
#else
static int hyn_ts_probe(struct spi_device *client)
#endif
{
    int ret = 0;
    u16 bus_type;
    struct hyn_ts_data *ts_data = 0;

    HYN_INFO("enter:%s", __func__);

#ifdef I2C_PORT
    if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
        HYN_INFO("I2C not supported");
        return -ENODEV;
    }
    bus_type = BUS_I2C;
#else
    client->mode = SPI_MODE;
	client->max_speed_hz = SPI_CLOCK_FREQ;
	client->bits_per_word = 8;
    if (spi_setup(client) < 0){
        HYN_INFO("SPI not supported");
        return -ENODEV;
    }
    bus_type = BUS_SPI;
#endif

    ts_data = kzalloc(sizeof(*ts_data), GFP_KERNEL);
    if (!ts_data) {
        HYN_INFO("alloc ts data failed");
        return -ENOMEM;
    }
    ts_data->bus_type = bus_type;
    ts_data->rp_buf.key_id = 0xFF;
    ts_data->work_mode = NOMAL_MODE;
    
    hyn_data = ts_data;
    ts_data->client = client;
    ts_data->dev = &client->dev;
    dev_set_drvdata(ts_data->dev, ts_data);

    ret = hyn_parse_dt(ts_data);
    if(ret){
        HYN_ERROR("hyn_parse_dt failed");
        goto ERRO_DTS_INIT;
    }

    ret = hyn_poweron(ts_data);
    if(ret){
        HYN_ERROR("hyn_poweron failed");
        goto ERRO_DTS_INIT;
    }
    // spin_lock_init(&ts_data->irq_lock);
    mutex_init(&ts_data->mutex_report);
    mutex_init(&ts_data->mutex_bus);
    mutex_init(&ts_data->mutex_fs);
    init_waitqueue_head(&ts_data->wait_irq);

    ret = hyn_detect_each_ic(ts_data);
    if(ret){
        HYN_ERROR("hyn_detect_each_ic failed,use default fuc tbl[0]");
        // ts_data->hyn_fuc_used = (void*)fuc_tbl[0];
        goto ERROR_CREATE_WQ;
    }
    hyn_fun = ts_data->hyn_fuc_used;

    ts_data->hyn_workqueue = create_singlethread_workqueue("hyn_wq");
    if (IS_ERR_OR_NULL(ts_data->hyn_workqueue)) {
        HYN_ERROR("create work queue failed");
        goto ERROR_CREATE_WQ;
    }

    ret = hyn_input_dev_init(ts_data);
    if(ret){
        HYN_ERROR("hyn_input_dev_init failed");
        goto ERRO_REG_INPUT_DEV;
    }

    ts_data->gpio_irq =  gpio_to_irq(ts_data->plat_data.irq_gpio);
    ret = request_threaded_irq(ts_data->gpio_irq, NULL, hyn_irq_handler,
                                (IRQF_TRIGGER_FALLING | IRQF_ONESHOT), HYN_DRIVER_NAME, ts_data);
    if(ret){
        HYN_ERROR("request_threaded_irq failed");
        goto ERRO_REQUEST_IRQ;
    }
    atomic_set(&ts_data->irq_is_disable,ENABLE);
    hyn_irq_set(ts_data , DISABLE);

    INIT_WORK(&ts_data->work_report,hyn_irq_report_work);
    INIT_WORK(&ts_data->work_updata_fw,hyn_updata_fw_work);
    INIT_WORK(&ts_data->work_resume,hyn_resum_work);
    INIT_DELAYED_WORK(&ts_data->esdcheck_work,hyn_esdcheck_work);

#if defined(CONFIG_FB)
    ts_data->fb_notif.notifier_call = fb_notifier_callback;
    ret = fb_register_client(&ts_data->fb_notif);
    if (ret) {
        HYN_ERROR("register fb_notifier failed: %d", ret);
    }
#elif defined(CONFIG_HAS_EARLYSUSPEND)
    ts_data->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
    ts_data->early_suspend.suspend = hyn_ts_early_suspend;
    ts_data->early_suspend.resume = hyn_ts_late_resume;
    register_early_suspend(&ts_data->early_suspend);
#endif
#if (HYN_APK_DEBUG_EN)
    hyn_tool_fs_int(ts_data);
#endif
    hyn_create_sysfs(ts_data);
#if (HYN_GESTURE_EN)
    hyn_gesture_init(ts_data);
#endif

    hyn_irq_set(ts_data,ENABLE);
    hyn_esdcheck_switch(ts_data,ENABLE);
    if(ts_data->need_updata_fw){
        queue_work(ts_data->hyn_workqueue,&ts_data->work_updata_fw);
        // flush_workqueue(ts_data->hyn_workqueue);
    }
    return 0;

ERRO_REQUEST_IRQ:
    input_unregister_device(ts_data->input_dev);
ERRO_REG_INPUT_DEV:
    destroy_workqueue(ts_data->hyn_workqueue);
ERROR_CREATE_WQ:
    mutex_destroy(&ts_data->mutex_fs);
    mutex_destroy(&ts_data->mutex_bus);
    mutex_destroy(&ts_data->mutex_report);
    gpio_free(ts_data->plat_data.irq_gpio);
    gpio_free(ts_data->plat_data.reset_gpio);
ERRO_DTS_INIT:
    kfree(ts_data);

    return -1;
}

#ifdef I2C_PORT
static int hyn_ts_remove(struct i2c_client *client)
#else
static int hyn_ts_remove(struct spi_device *client)
#endif
{
    struct hyn_ts_data *ts_data = hyn_data;
    HYN_ENTER();    
    if(!IS_ERR_OR_NULL(ts_data)){
        if(ts_data->gpio_irq != 0)
            free_irq(ts_data->gpio_irq, ts_data);
        if(ts_data->input_dev != NULL)
            input_unregister_device(ts_data->input_dev);
        if (ts_data->hyn_workqueue)
            destroy_workqueue(ts_data->hyn_workqueue);
#if defined(CONFIG_FB) 
        fb_unregister_client(&ts_data->fb_notif);
#elif defined(CONFIG_HAS_EARLYSUSPEND)
        unregister_early_suspend(&ts_data->early_suspend);
#endif
        mutex_destroy(&ts_data->mutex_fs);
        mutex_destroy(&ts_data->mutex_bus);
        mutex_destroy(&ts_data->mutex_report);
        if(gpio_is_valid(ts_data->plat_data.irq_gpio))
		    gpio_free(ts_data->plat_data.irq_gpio);
        if(gpio_is_valid(ts_data->plat_data.irq_gpio))
            gpio_free(ts_data->plat_data.reset_gpio);
#if (HYN_APK_DEBUG_EN)
        hyn_tool_fs_exit();
#endif
#if (HYN_GESTURE_EN)
        hyn_gesture_exit();
#endif
        hyn_release_sysfs();
        kfree(ts_data);
    }
    return 0;
}


static const struct of_device_id hyn_of_match_table[] = {
    {.compatible = HYN_DRIVER_NAME,},
    {},
};

#ifdef I2C_PORT
static const struct i2c_device_id hyn_id_table[] = {
    {.name = HYN_DRIVER_NAME, .driver_data = 0,},
    {},
};

static struct i2c_driver hyn_ts_driver = {
    .probe = hyn_ts_probe,
    .remove = hyn_ts_remove,
    .driver = {
        .name = HYN_DRIVER_NAME,
        .owner = THIS_MODULE,
        .of_match_table = hyn_of_match_table,
    },
    .id_table = hyn_id_table,
};
#else
static struct spi_driver hyn_ts_driver = {
	.driver = {
		   .name = HYN_DRIVER_NAME,
		   .of_match_table = hyn_of_match_table,
		   .owner = THIS_MODULE,
		   },
	.probe = hyn_ts_probe,
	.remove = hyn_ts_remove,
};
#endif

static int __init hyn_ts_init(void)
{
    int ret = 0;
    HYN_ENTER();
#ifdef I2C_PORT  
    ret = i2c_add_driver(&hyn_ts_driver);
#else
    ret = spi_register_driver(&hyn_ts_driver);
#endif
    if (ret) {
        HYN_INFO("add i2c driver failed");
        return -ENODEV;
    }
    return 0;
}

static void __exit hyn_ts_exit(void)
{
    HYN_INFO("enter:%s", __func__);
#ifdef I2C_PORT  
    i2c_del_driver(&hyn_ts_driver);
#else
    spi_unregister_driver(&hyn_ts_driver);
#endif
}

module_init(hyn_ts_init);
module_exit(hyn_ts_exit);

MODULE_AUTHOR("Hynitron Driver Team");
MODULE_DESCRIPTION("Hynitron Touchscreen Driver");
MODULE_LICENSE("GPL v2");

