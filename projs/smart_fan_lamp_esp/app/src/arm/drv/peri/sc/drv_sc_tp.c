
#include "drv/peri/sc/drv_sc_tp.h"

status_t sc_tp_init(void)
{
    cst816t_init();
    return status_ok;
}

