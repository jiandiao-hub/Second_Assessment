#include "cust_alsps.h"

struct alsps_hw cust_alsps_hw[] __attribute__((section(".cust_alsps"))) = {
#ifdef CFG_CM36558_SUPPORT
    {
        .name = "cm36558",
        .i2c_num    = 0,
        .i2c_addr = {0x51, 0},
        .polling_mode_ps = 0,
        .polling_mode_als = 1,
        .als_level  = {0, 328, 861, 1377, 3125, 7721, 7767, 12621, 23062, 28430, 33274, 47116, 57694, 57694, 65535},
        .als_value  = {0, 200, 320, 502, 1004, 2005, 3058, 5005, 8008, 10010, 12000, 16000, 20000, 20000, 20000, 20000},
        .ps_threshold_high = 26,
        .ps_threshold_low = 21,
        .eint_num = 6,
    },
#endif

#ifdef CFG_LTR559_SUPPORT
    {
        .name = "ltr559",
        .i2c_num    = 0,
        .i2c_addr = {0x23, 0},
        .polling_mode_ps = 0,
        .polling_mode_als = 1,
        .als_level  = {0, 328, 861, 1377, 3125, 7721, 7767, 12621, 23062, 28430, 33274, 47116, 57694, 57694, 65535},
        .als_value  = {0, 200, 320, 502, 1004, 2005, 3058, 5005, 8008, 10010, 12000, 16000, 20000, 20000, 20000, 20000},
        .ps_threshold_high = 26,
        .ps_threshold_low = 21,
        .eint_num = 6,
    },
#endif

#ifdef CFG_LTR578_SUPPORT
    {
        .name = "ltr578",
        .i2c_num    = 0,
        .i2c_addr = {0x53, 0},
        .polling_mode_ps = 0,
        .polling_mode_als = 1,
        .als_level  = {0, 328, 861, 1377, 3125, 7721, 7767, 12621, 23062, 28430, 33274, 47116, 57694, 57694, 65535},
        .als_value  = {0, 200, 320, 502, 1004, 2005, 3058, 5005, 8008, 10010, 12000, 16000, 20000, 20000, 20000, 20000},
        .ps_threshold_high = 480,//200
        .ps_threshold_low = 450,//180
        .eint_num = 7,
    },
#endif

#ifdef CFG_STK3321_NEW_SUPPORT
    {
        .name = "stk3x1x",
        .i2c_num    = 0,
        .i2c_addr = {0x48, 0},
        .polling_mode_ps = 0,
        .polling_mode_als = 1,
        .als_level  = {0, 328, 861, 1377, 3125, 7721, 7767, 12621, 23062, 28430, 33274, 47116, 57694, 57694, 65535},
        .als_value  = {0, 200, 320, 502, 1004, 2005, 3058, 5005, 8008, 10010, 12000, 16000, 20000, 20000, 20000, 20000},
        .ps_threshold_high = 2600,
        .ps_threshold_low = 2500,
        .eint_num = 11,
    },
#endif
};
