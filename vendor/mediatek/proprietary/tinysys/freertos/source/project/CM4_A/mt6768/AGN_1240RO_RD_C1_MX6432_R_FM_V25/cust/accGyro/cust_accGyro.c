#include "cust_accGyro.h"

struct accGyro_hw cust_accGyro_hw[] __attribute__((section(".cust_accGyro"))) = {
#ifdef CFG_SC7A20_SUPPORT
		{
		.name = "sc7a20",
		.i2c_num = 1,
		.direction = 2,
		.i2c_addr = {0x18, 0},
		.eint_num = 2,
		},
#endif

#ifdef CFG_STK832X_SUPPORT
    {
        .name = "stk832x",
        .i2c_num = 1,
        .direction = 0,
        .i2c_addr = {0x68, 0},
        .eint_num = 2,
    },
#endif

};
