#include "cust_accGyro.h"

struct accGyro_hw cust_accGyro_hw[] __attribute__((section(".cust_accGyro"))) = {
#ifdef CFG_MXC4005X_SUPPORT
    {
        .name = "mxc4005x",
        .i2c_num = 0,
        .direction = 1,
        .i2c_addr = {0x15, 0},
        .eint_num = 2,
    },
#endif

};
