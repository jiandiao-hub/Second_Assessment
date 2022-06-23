#include "cust_sar.h"
//D:\source\MTK_sensorhub\freertos\source\project\CM4_A\mt6765\k61v1_64_bsp\cust\sar\cust_sar.c
//D:\source\MTK_sensorhub\freertos\source\project\CM4_A\mt6765\k61v1_64_bsp\cust\sar\cust_sar.c

struct sar_hw cust_sar_hw[] __attribute__((section(".cust_sar"))) = {
#ifdef CFG_SAR_SUPPORT
    {
        .name = "aw9610x",
        .i2c_num = 0,
        .direction = 0,
        .i2c_addr = {0x12, 0},
    },
#endif
#ifdef CFG_HX9031A_SUPPORT
    {
        .name = "hx9031a",
        .i2c_num = 0,
        .direction = 0,//这是硬件layout方向，sar sensor不用关心
        .i2c_addr = {0x28, 0},
        .eint_num = 3,
    },
#endif
};
