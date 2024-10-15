#include "hyn_core.h"

#ifdef I2C_PORT

#if I2C_USE_DMA
int hyn_write_data(struct hyn_ts_data *ts_data, u8 *buf, u16 len)
{
    int ret = 0;
    struct i2c_msg i2c_msg;
    mutex_lock(&ts_data->mutex_bus);
    i2c_msg.addr = ts_data->client->addr;
    i2c_msg.flags = 0;
    i2c_msg.len = len;
    i2c_msg.buf = buf;
    ret = i2c_transfer(ts_data->client->adapter, &i2c_msg, 1);
    mutex_unlock(&ts_data->mutex_bus);
    if (ret < 0) {
        return -1;
    }
    return 0;
}

int hyn_read_data(struct hyn_ts_data *ts_data,u8 *buf, u16 len)
{
    int ret = 0;
    struct i2c_msg i2c_msg;
    mutex_lock(&ts_data->mutex_bus);
    i2c_msg.addr = ts_data->client->addr;
    i2c_msg.flags = I2C_M_RD;
    i2c_msg.len = len;
    i2c_msg.buf = buf;
    ret = i2c_transfer(ts_data->client->adapter, &i2c_msg, 1);
    mutex_unlock(&ts_data->mutex_bus);
    if (ret < 0) {
        return -1;
    }
    return 0;
}

int hyn_wr_reg(struct hyn_ts_data *ts_data, u32 reg_addr, u8 size, u8 *rbuf, u16 rlen)
{
    int ret = 0,i=0;
    struct i2c_msg i2c_msg[2];
    u8 wbuf[4];
    if(size==0 || size > 4) return -1;
    mutex_lock(&ts_data->mutex_bus);
    memset(wbuf,0,sizeof(wbuf));
    i = size;
    while(i){
        i--;
        wbuf[i] = reg_addr;
        reg_addr >>= 8;
    }

    i2c_msg[0].addr = ts_data->client->addr;
    i2c_msg[0].flags = 0;
    i2c_msg[0].len = size;
    i2c_msg[0].buf = wbuf;
    if(rlen){
        i2c_msg[1].addr = ts_data->client->addr;
        i2c_msg[1].flags = I2C_M_RD;
        i2c_msg[1].len = rlen;
        i2c_msg[1].buf = rbuf;
    }
    ret = i2c_transfer(ts_data->client->adapter, i2c_msg, rlen==0 ? 1:2);
    mutex_unlock(&ts_data->mutex_bus);
    if (ret < 0) {
        return -1;
    }
    return 0;
}
#else
int hyn_write_data(struct hyn_ts_data *ts_data, u8 *buf, u16 len)
{
    int ret = 0;
    mutex_lock(&ts_data->mutex_bus);
    ret = i2c_master_send(ts_data->client, buf, len);
    mutex_unlock(&ts_data->mutex_bus);
    if (ret < 0) {
        return -1;
    }
    return 0;
}

int hyn_read_data(struct hyn_ts_data *ts_data,u8 *buf, u16 len)
{
    int ret = 0;
    mutex_lock(&ts_data->mutex_bus);
    ret = i2c_master_recv(ts_data->client, buf, len);
    mutex_unlock(&ts_data->mutex_bus);
    if (ret < 0) {
        return -1;
    }
    
    return 0;
}

int hyn_wr_reg(struct hyn_ts_data *ts_data, u32 reg_addr, u8 size, u8 *rbuf, u16 rlen)
{
    int ret = 0,i=0;
    u8 wbuf[4];
    mutex_lock(&ts_data->mutex_bus);
    memset(wbuf,0,sizeof(wbuf));
    i = size;
    while(i){
        i--;
        wbuf[i] = reg_addr;
        reg_addr >>= 8;
    }
    ret = i2c_master_send(ts_data->client, wbuf, size);
    ret |= i2c_master_recv(ts_data->client, rbuf, rlen);
    mutex_unlock(&ts_data->mutex_bus);
    if (ret < 0) {
        return -1;
    }
    return 0;
}

#endif
#endif

