#include "cust_mag.h"

struct mag_hw cust_mag_hw[] __attribute__((section(".cust_mag"))) = {
#ifdef CFG_MMC5603_SUPPORT
    {

        .name = "mmc5603",
        .i2c_num = 1,
        .direction = 4,
        .i2c_addr = {0x30, 0},
    },
#endif
};
