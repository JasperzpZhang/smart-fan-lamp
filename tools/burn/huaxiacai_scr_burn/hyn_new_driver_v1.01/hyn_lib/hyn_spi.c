
#include "hyn_core.h"

#ifndef I2C_PORT

int hyn_write_data(struct hyn_ts_data *ts_data, u8 *buf, u16 len)
{
	struct spi_message msg;
	struct spi_transfer transfer[1] = { {0} };
    int ret;
    mutex_lock(&ts_data->mutex_bus);

	spi_message_init(&msg);
	transfer[0].len = len;
	transfer[0].delay_usecs = SPI_DELAY_CS;
	transfer[0].tx_buf = buf;
	transfer[0].rx_buf = NULL;
	spi_message_add_tail(&transfer[0], &msg);
    ret = spi_sync(ts_data->client, &msg);

    mutex_unlock(&ts_data->mutex_bus);
    return ret < 0 ? ret:0;
}

int hyn_read_data(struct hyn_ts_data *ts_data,u8 *buf, u16 len)
{
	struct spi_message msg;
	struct spi_transfer transfer[1] = { {0} };
    int ret;
    mutex_lock(&ts_data->mutex_bus);

	spi_message_init(&msg);
	transfer[0].len = len;
	transfer[0].delay_usecs = SPI_DELAY_CS;
	transfer[0].tx_buf = NULL;
	transfer[0].rx_buf = buf;
	spi_message_add_tail(&transfer[0], &msg);
    ret = spi_sync(ts_data->client, &msg);

    mutex_unlock(&ts_data->mutex_bus);
    return ret < 0 ? ret:0;
}

int hyn_wr_reg(struct hyn_ts_data *ts_data, u32 reg_addr, u8 size, u8 *rbuf, u16 rlen)
{
    int ret = 0,i=0;
	struct spi_message msg;
	struct spi_transfer transfer[2] = { {0}, {0} };
    u8 cmd[4];
    mutex_lock(&ts_data->mutex_bus);

    memset(cmd,0,sizeof(cmd));
    i = size;
    while(i){
        i--;
        cmd[i] = reg_addr;
        reg_addr >>= 8;
    }
	spi_message_init(&msg);
	transfer[0].len = size;
	transfer[0].tx_buf = cmd;
	transfer[0].rx_buf = NULL;
	spi_message_add_tail(&transfer[0], &msg);

	transfer[1].len = rlen;
	transfer[1].delay_usecs = SPI_DELAY_CS;
	transfer[1].tx_buf = NULL;
	transfer[1].rx_buf = rbuf;
	spi_message_add_tail(&transfer[1], &msg);
    ret = spi_sync(ts_data->client, &msg);

    mutex_unlock(&ts_data->mutex_bus);

    return ret < 0 ? ret:0;
}

#endif

