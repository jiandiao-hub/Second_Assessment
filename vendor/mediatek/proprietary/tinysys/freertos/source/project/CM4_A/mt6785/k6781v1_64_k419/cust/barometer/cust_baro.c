#include "cust_baro.h"

struct baro_hw cust_baro_hw[] __attribute__((section(".cust_baro"))) = {
#ifdef CFG_BMP280_SUPPORT
    {
        .name = "bmp280",
        .i2c_num = 0,
        .direction = 0,
        .i2c_addr = {0x77, 0},
    },
#endif
#ifdef CFG_BMP380_SUPPORT
	{
		.name = "bmp380",
		.i2c_num = 0,
		.direction = 0,
		.i2c_addr = {0x76, 0},
	},
#endif
};
