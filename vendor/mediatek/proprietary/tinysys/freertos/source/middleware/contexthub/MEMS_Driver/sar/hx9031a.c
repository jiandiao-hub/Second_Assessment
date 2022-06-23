/*************************************************************
*                                                            *
*  Driver for NanJingTianYiHeXin HX9031A & HX9023E Cap Sense *
*                                                            *
*************************************************************/


#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <stdbool.h>
#include <stdint.h>
#include <seos.h>
#include <util.h>
#include <plat/inc/rtc.h>
#include <sensors.h>

#include <sar.h>
#include <contexthub_core.h>
#include <cust_sar.h>
#include "eint.h"
#include <hx9031a.h>
#include <timer.h>
#include <heap.h>
#include "cache_internal.h"


enum HX9031AState {
    STATE_CHECK_INT = CHIP_SAR_SAMPLING,
    STATE_CONVERT = CHIP_SAR_CONVERT,
    STATE_SAMPLE_DONE = CHIP_SAR_SAMPLING_DONE,
    STATE_ENABLE = CHIP_SAR_ENABLE,
    STATE_ENABLE_DONE = CHIP_SAR_ENABLE_DONE,
    STATE_DISABLE = CHIP_SAR_DISABLE,
    STATE_DISABLE_DONE = CHIP_SAR_DISABLE_DONE,
    STATE_RATECHG = CHIP_SAR_RATECHG,
    STATE_RATECHG_DONE = CHIP_SAR_RATECHG_DONE,
    STATE_CALI = CHIP_SAR_CALI,
    STATE_CALI_DONE = CHIP_SAR_CALI_DONE,
    STATE_INIT_DONE = CHIP_INIT_DONE,
    STATE_IDLE = CHIP_IDLE,
    STATE_RESET = CHIP_RESET,         //15

    STATE_CORE,//16
    STATE_DEALY_1,//17
    STATE_DEALY_2,//18
    STATE_REG_INIT1,//19
    STATE_REG_INIT2,
    STATE_REG_INIT3,
    STATE_REG_INIT4,
    STATE_REG_INIT5,
    STATE_REG_INIT6,
    STATE_REG_INIT7,
    STATE_CH_CFG,
    STATE_DATA_SWITCH,
    STATE_SET_THRES_NEAR,
    STATE_SET_THRE_FAR,    
    STATE_STATE_MONITOR,
    STATE_GET_THRES_NEAR,
    STATE_GET_THRES_FAR,
    STATE_GET_RAW_BL,
    STATE_GET_DIFF_LP,
    STATE_GET_OFFSET,
    STATE_SETUP_EINT,
    STATE_ENABLE_EINT,
    STATE_DISABLE_EINT,
    STATE_SAMPLE,
};

static struct HX9031ATask {
    /* txBuf for i2c operation, fill register and fill value */
    uint8_t txBuf[12];
    uint8_t rxBuf[12];
    uint8_t sarIntStatus;

    uint8_t i2c_addr;
    uint8_t chipId;
    struct sar_hw *hw;

    uint32_t timerHandle;
    /* rxBuf for i2c operation, receive rawdata */
    struct transferDataInfo dataInfo;
    struct SarData_t data;

    /* data for factory */
    struct TripleAxisDataPoint factoryData;

    /**/
    int irq;      //irq number
    int irq_gpio; //gpio numver
    char irq_disabled;
    uint32_t sarIntStatusCmp;//prox status after compared between data and threshold
} mTask;

struct addr_value_pair_t {
    uint8_t addr;
    uint8_t value;
};

struct hx9031a_near_far_threshold {
    uint16_t thr_near;
    uint16_t thr_far;
};

struct hx9031a_channel_info {
    char *name;
    bool enabled;
    bool used;   //this should be set 0 if ch3 is disable on the hx9023
    int state;

};


static const struct hx9031a_channel_info hx9031a_channels[] = {
    {
        .name = "hx9031a_ch0",
        .enabled = false,
        .used = false,
    },
    {
        .name = "hx9031a_ch1",
        .enabled = false,
        .used = false,
    },
    {
        .name = "hx9031a_ch2",
        .enabled = false,
        .used = false,
    },
    {
        .name = "hx9031a_ch3",
        .enabled = false,
        .used = false,
    },
    {
        .name = "hx9031a_ch4",
        .enabled = false,
        .used = false,
    },
};

static struct addr_value_pair_t hx9031a_reg_init_list1[] = {
    {RA_CH_NUM_CFG_0x24,               0x00}, //close channel.
    {RA_GLOBAL_CTRL1_0x01,             0x82}, //open chop
    {RA_GLOBAL_CTRL0_0x00,             0x40}, //open dither
    {RA_PRF_CFG_0x02,                  0x0f}, //odr 0x0B: 33ms 0x03: 6m, 0x0f: 50ms
    {RA_RANGE_7_0_0x0D,                0x00}, //range ch1 ch0 1.25p
    {RA_RANGE_9_8_0x0E,                0x00}, //range ch3 1.25p ch2 1.25pf
    {RA_AVG0_NOSR0_CFG_0x10,           0x11}, //ch0 osr 512 avg 1 warm 16.
    {RA_NOSR12_CFG_0x11,               0x54} //ch2 osr 512; ch1 osr 512
};

static struct addr_value_pair_t hx9031a_reg_init_list2[] = {
    {RA_NOSR43_CFG_0x12,               0x15}, //ch3 osr 512
    {RA_AVG12_CFG_0x13,                0x00}, //ch1 and ch2 avg 1
    {RA_AVG34_CFG_0x14,                0x00}, //ch3 avg 1
    {RA_SAMPLE_NUM_7_0_0x1f,           0x40}, //sample frequency 0x96: 31K. 0x4B 62K, 0x3a 83K
    {RA_INTEGRATION_NUM_7_0_0x21,      0x40}, //sample frequency 0x96: 31K. 0x4B 62K, 0x3a 83K
    {RA_MOD_RST_CFG_0x28,              0xf5}, //change offset cali delay
    {RA_LP_ALP_1_0_CFG_0x2a,           0x22}, //ch1 LP alpha 4. ch0 LP alpha 4
    {RA_LP_ALP_3_2_CFG_0x2b,           0x22} //ch3 LP alpha 4. ch2 LP alpha 4
};

static struct addr_value_pair_t hx9031a_reg_init_list3[] = {
    {RA_UP_ALP_1_0_CFG_0x2c,           0x88}, //ch1 ch0 BL UP alpha 128
    {RA_UP_ALP_3_2_CFG_0x2d,           0x88}, //ch3 ch2 BL UP alpha 128
    {RA_DN_UP_ALP_0_4_CFG_0x2e,        0x10}, //ch0 BL DN alpha 4
    {RA_DN_ALP_2_1_CFG_0x2f,           0x11}, //ch2 ch1 BL DN alpha 4
    {RA_DN_ALP_4_3_CFG_0x30,           0x01}, //ch3 BL DN alpha 4
    {RA_RAW_BL_RD_CFG_0x38,            0xf0}, //7:4 bl lp
    {RA_INTERRUPT_CFG_0x39,            0xFF}, //enable threshold interrupt, one bit for one channel
    {RA_CALI_DIFF_CFG_0x3b,            0x07} //cali diff 16384 bit7:4 int_conversion_ready
};

static struct addr_value_pair_t hx9031a_reg_init_list4[] = {
    {RA_ANALOG_MOD_AMP_COMP_0x50,      0xb5}, //increase amp current
    {RA_PROX_INT_HIGH_CFG_0x6c,        0x01}, //prox int high count.  prox interrupt debound
    {RA_PROX_INT_LOW_CFG_0x6d,         0x01}, //prox int low count. far interrupt debound
    {RA_CAP_INI_CFG_0x6e,              0x00}, //close cap ini calibration

    {RA_PROX_HIGH_DIFF_CFG_CH0_0_0x80, 0x80}, //high int 0x3FF*32
    {RA_PROX_HIGH_DIFF_CFG_CH0_1_0x81, 0x00},
    {RA_PROX_HIGH_DIFF_CFG_CH1_0_0x82, 0x80}, //high int 0x3FF*32
    {RA_PROX_HIGH_DIFF_CFG_CH1_1_0x83, 0x00}
};

static struct addr_value_pair_t hx9031a_reg_init_list5[] = {
    {RA_PROX_HIGH_DIFF_CFG_CH2_1_0x85, 0x00},
    {RA_PROX_HIGH_DIFF_CFG_CH3_0_0x86, 0x80}, //high int 0x3FF*32
    {RA_PROX_HIGH_DIFF_CFG_CH3_1_0x87, 0x00},

    {RA_PROX_LOW_DIFF_CFG_CH0_0_0x88,  0x60}, //low int 0x2FF*32
    {RA_PROX_LOW_DIFF_CFG_CH0_1_0x89,  0x00},
    {RA_PROX_LOW_DIFF_CFG_CH1_0_0x8a,  0x60}, //low int 0x2FF*32
    {RA_PROX_LOW_DIFF_CFG_CH1_1_0x8b,  0x00},
    {RA_PROX_LOW_DIFF_CFG_CH2_0_0x8c,  0x60} //low int 0x2FF*32
};
static struct addr_value_pair_t hx9031a_reg_init_list6[] = {
    {RA_PROX_LOW_DIFF_CFG_CH2_1_0x8d,  0x00},
    {RA_PROX_LOW_DIFF_CFG_CH3_0_0x8e,  0x60}, //low int 0x2FF*32
    {RA_PROX_LOW_DIFF_CFG_CH3_1_0x8f,  0x00},

    {RA_CH10_SCAN_FACTOR_0xc0,         0x00}, // 0x32: ch1 odr * 8 ch0 odr * 4.
    {RA_CH32_SCAN_FACTOR_0xc1,         0x00}, // 0x70: ch3 odr * 128 (samle 1 per 128 period).
    {RA_OFFSET_CALI_CTRL_0xc2,         0x00},
    {RA_DSP_CONFIG_CTRL0_0xc3,         0xf0}, //bsline,offset, only open bit3 bit0; ch2 close force zero.
    {RA_DSP_CONFIG_CTRL3_0xca,         0x00} //lp up threshold 0.
};
static struct addr_value_pair_t hx9031a_reg_init_list7[] = {

    {RA_CH0_CFG_7_0_0x03,              0x00}, //channel config
    {RA_CH0_CFG_9_8_0x04,              0x00},
    {RA_CH1_CFG_7_0_0x05,              0x00},
    {RA_CH1_CFG_9_8_0x06,              0x00},
    {RA_CH2_CFG_7_0_0x07,              0x00},
    {RA_CH2_CFG_9_8_0x08,              0x00},
    {RA_CH3_CFG_7_0_0x09,              0x00},
    {RA_CH3_CFG_9_8_0x0A,              0x00},

    //{RA_CH_NUM_CFG_0x24,              0x03}, //open channel.
};



static uint8_t hx9031a_monitor_addr = RA_PRF_CFG_0x02;
static uint8_t hx9031a_monitor_val = 0;
static uint8_t ch_enable_status = 0x00;
//static uint8_t hx9031a_polling_enable = 0;
//static int hx9031a_polling_period_ms = 0;

static int16_t data_raw[HX9031A_CH_NUM] = {0};
static int16_t data_diff[HX9031A_CH_NUM] = {0};
static int16_t data_lp[HX9031A_CH_NUM] = {0};
static int16_t data_bl[HX9031A_CH_NUM] = {0};
static uint16_t data_offset_dac[HX9031A_CH_NUM] = {0};

static struct hx9031a_near_far_threshold hx9031a_ch_thres[HX9031A_CH_NUM] = {
    {.thr_near = 576, .thr_far = 448}, //ch0
    {.thr_near = 576, .thr_far = 448},
    {.thr_near = 320, .thr_far = 256},
    {.thr_near = 4096, .thr_far = 3200},
};
static uint8_t hx9031a_ch_near_state[HX9031A_CH_NUM] = {0};  //just far and near
static volatile uint8_t hx9031a_irq_from_suspend_flag = 0;
static volatile uint8_t hx9031a_irq_en_flag = 1;
static volatile uint8_t hx9031a_hw_monitor_en_flag = 1;
static volatile uint8_t hx9031a_output_switch = HX9031A_OUTPUT_TYPE;

#ifdef HX9031A_EXTERN_ALG
//external alg
#define HX9031A_RAW_DIFF_RATIO   8
#define BL_UP_ALPHA             64
#define BL_DN_ALPHA             4

#define BL_UP_THRES_CH0             50
#define BL_UP_THRES_CH1             128
#define BL_UP_THRES_CH2             128
#define BL_UP_THRES_CH3             128

#define ACC_THRES_CH0             25
#define ACC_THRES_CH1             50
#define ACC_THRES_CH2             50
#define ACC_THRES_CH3             50

#define HX9031A_PROX_THRES_NEAR_CH0 100
#define HX9031A_PROX_THRES_NEAR_CH1 100
#define HX9031A_PROX_THRES_NEAR_CH2 200
#define HX9031A_PROX_THRES_NEAR_CH3 100

#define HX9031A_PROX_THRES_FAR_CH0  80
#define HX9031A_PROX_THRES_FAR_CH1  80
#define HX9031A_PROX_THRES_FAR_CH2  180
#define HX9031A_PROX_THRES_FAR_CH3  80

#define HX9031A_BL_LOW_THRES     -16384
#define HX9031A_BL_MAX_VALUE     32700
#define HX9031A_DEGLITCH_THRES   2000

#define HX9031A_PROX_CONTINUOUS_CNT 4


static uint8_t ch_enable_first_time = 0x00;

static int32_t data_prox[HX9031A_CH_NUM] = {0};
static int32_t lp_deglitch[HX9031A_CH_NUM] = {0};
static uint8_t prox_status_channel[HX9031A_CH_NUM] = {0};

int32_t hx9031a_alg_raw_pre[HX9031A_CH_NUM] = {0};
int32_t hx9031a_alg_raw_pre2[HX9031A_CH_NUM] = {0};
int32_t hx9031a_alg_bl_data_multiple256[HX9031A_CH_NUM] = {0};

int32_t hx9031a_alg_raw_deglitch_pre[HX9031A_CH_NUM] = {0};
int32_t hx9031a_alg_raw_deglitch_acc[HX9031A_CH_NUM] = {0};

int32_t hx9031a_alg_raw_deglitch_ref_first_val = 0;
int64_t hx9031a_alg_raw_deglitch_ref_last_val_multipl_100 = 0;
int32_t hx9031a_far_continuous_counter[HX9031A_CH_NUM] = {0};


volatile uint8_t hx9031a_alg_bl_error_flag = 0; //8bit: xxxx 3210  example: 0x08 : channel 3 error

int hx9031a_alg_raw_diff_ratio = HX9031A_RAW_DIFF_RATIO;
int hx9031a_alg_bl_up_alpha = BL_UP_ALPHA;
int hx9031a_alg_bl_dn_alpha = BL_DN_ALPHA;

int hx9031a_bl_up_thres[4] = {BL_UP_THRES_CH0, BL_UP_THRES_CH1, \
    BL_UP_THRES_CH2, BL_UP_THRES_CH3};

int hx9031a_acc_thres[4] = {ACC_THRES_CH0, ACC_THRES_CH1, \
    ACC_THRES_CH2, ACC_THRES_CH3};

int hx9031a_near_thres[4] = {HX9031A_PROX_THRES_NEAR_CH0, HX9031A_PROX_THRES_NEAR_CH1, \
    HX9031A_PROX_THRES_NEAR_CH2, HX9031A_PROX_THRES_NEAR_CH3};

int hx9031a_far_thres[4] = {HX9031A_PROX_THRES_FAR_CH0, HX9031A_PROX_THRES_FAR_CH1, \
    HX9031A_PROX_THRES_FAR_CH2, HX9031A_PROX_THRES_FAR_CH3};

void hx9031a_reset_first_time_flag(void)
{
    ch_enable_first_time = 0;
}

uint8_t hx9031a_get_first_time_flag(void)
{
    return ch_enable_first_time;
}

void hx9031a_set_first_time(uint8_t val)
{
    ch_enable_first_time = val;
}


void hx9031a_set_first_time_defalut_value(int16_t lp_data,uint8_t i)
{
    hx9031a_alg_raw_pre[i] = lp_data;
    hx9031a_alg_raw_pre2[i] = lp_data;
    lp_deglitch[i] = lp_data;
    hx9031a_alg_bl_data_multiple256[i] = (lp_data + 500)*256;   

    hx9031a_alg_raw_deglitch_pre[i] = lp_data;
    hx9031a_alg_raw_deglitch_acc[i] = lp_data;

    if(i == 1)
    {
        hx9031a_alg_raw_deglitch_ref_first_val = lp_data;
        hx9031a_alg_raw_deglitch_ref_last_val_multipl_100 = lp_data * 100;
    }
}

bool hx9031a_glith_judgement(int16_t lp_data,uint8_t i)
{
    if( ((lp_data- hx9031a_alg_raw_pre[i] > HX9031A_DEGLITCH_THRES) && (hx9031a_alg_raw_pre2[i] - hx9031a_alg_raw_pre[i] > HX9031A_DEGLITCH_THRES)) ||\
        ((hx9031a_alg_raw_pre[i] - lp_data > HX9031A_DEGLITCH_THRES) && (hx9031a_alg_raw_pre[i] - hx9031a_alg_raw_pre2[i] > HX9031A_DEGLITCH_THRES)))
    {
        return true;
    }
    else
    {
        return false;
    }
}

int32_t hx9031a_get_raw_deglitch_value(int16_t lp_data,uint8_t i)
{
    int32_t data_deglich;

    if(hx9031a_glith_judgement(lp_data, i))
    {
        data_deglich = hx9031a_alg_raw_pre2[i];
        hx9031a_alg_raw_pre[i] = hx9031a_alg_raw_pre2[i];
    }
    else
    {
        data_deglich = hx9031a_alg_raw_pre[i];
    } 

    hx9031a_alg_raw_pre2[i] = hx9031a_alg_raw_pre[i];
    hx9031a_alg_raw_pre[i] = lp_data;
    return data_deglich;
}
/*
int32_t hx9031a_raw_channel_diff(int32_t  data_deglich_pos,int16_t data_deglich_neg)
{
    int32_t  deglich_pos; 

    hx9031a_alg_raw_deglitch_ref_last_val_multipl_100 = (int64_t)(data_deglich_neg * 100 * 1/16 + \
        hx9031a_alg_raw_deglitch_ref_last_val_multipl_100 * 15/16);
    deglich_pos =data_deglich_pos- \
        (int32_t)((hx9031a_alg_raw_deglitch_ref_last_val_multipl_100 / 100 - hx9031a_alg_raw_deglitch_ref_first_val )  * hx9031a_alg_raw_diff_ratio);      
    return deglich_pos;
}
*/
void hx9031a_prox_calculation(int32_t *deglitch, int32_t *prox,uint8_t *prox_status_ch,int16_t lp_data,uint8_t ch)
{
    if((ch == 0) || (ch == 1) || (ch == 2) )
    {
        *prox = *deglitch - hx9031a_alg_bl_data_multiple256[ch]/256;
        *prox = *prox > 32767? 32767 : *prox;
    }
    else if(ch == 3)
    {
        *prox = *deglitch - lp_data;//bl_data = raw_data = lp_data
        *prox = *prox > 32767? 32767 : *prox;
    }

    if(*prox > hx9031a_near_thres[ch])
    {
        *prox_status_ch = 1;
        hx9031a_far_continuous_counter[ch] = 0;
    }
    else if(*prox < hx9031a_far_thres[ch] && (*prox_status_ch == 1))
    {
        hx9031a_far_continuous_counter[ch]++;
        if(hx9031a_far_continuous_counter[ch] >= HX9031A_PROX_CONTINUOUS_CNT)
        {
            *prox_status_ch = 0;
        }
    }
    else
    {
        hx9031a_far_continuous_counter[ch] = 0;
    }

    if(*prox_status_ch == 0)
    {
        if((ch == 0) || (ch == 1) || (ch == 2))
        {
            if(*prox > hx9031a_bl_up_thres[ch])
            {
                hx9031a_alg_bl_data_multiple256[ch] = hx9031a_alg_bl_data_multiple256[ch] * (hx9031a_alg_bl_up_alpha-1)/hx9031a_alg_bl_up_alpha + *deglitch * 256 /hx9031a_alg_bl_up_alpha;
            }
            else
            {
                hx9031a_alg_bl_data_multiple256[ch] = hx9031a_alg_bl_data_multiple256[ch] * (hx9031a_alg_bl_dn_alpha-1)/hx9031a_alg_bl_dn_alpha + *deglitch * 256 /hx9031a_alg_bl_dn_alpha;
            }
            
            hx9031a_alg_raw_deglitch_acc[ch] = hx9031a_alg_bl_data_multiple256[ch]/256;
        }
    }
    else
    {
        if((ch == 0) || (ch == 1) || (ch == 2) )
        {
             if((*deglitch < hx9031a_alg_raw_deglitch_pre[ch] + hx9031a_acc_thres[ch]) &&\
                (hx9031a_alg_raw_deglitch_pre[ch] < *deglitch + hx9031a_acc_thres[ch]))
            {
                hx9031a_alg_raw_deglitch_acc[ch] = hx9031a_alg_raw_deglitch_acc[ch] + \
                    *deglitch - hx9031a_alg_raw_deglitch_pre[ch];
            }   
            hx9031a_alg_bl_data_multiple256[ch] = hx9031a_alg_bl_data_multiple256[ch] * 3 / 4 + hx9031a_alg_raw_deglitch_acc[ch] * 256 /4;
        }
    }

    if( hx9031a_alg_bl_data_multiple256[ch] > HX9031A_BL_MAX_VALUE * 256)
    {
        hx9031a_alg_bl_data_multiple256[ch] = HX9031A_BL_MAX_VALUE * 256;
    }
    
    if( (hx9031a_alg_bl_data_multiple256[ch] < HX9031A_BL_LOW_THRES * 256) || (*deglitch < HX9031A_BL_LOW_THRES))
    {
        if((ch == 1) || (ch == 2))
        {
            hx9031a_alg_bl_error_flag |= (1 << ch);
        }
    } 
    hx9031a_alg_raw_deglitch_pre[ch] = *deglitch;
}
//external alg end
#endif
static void hx9031a_data_lock(uint8_t lock_flag)
{
    int ret = -1;

    if(HX9031A_DATA_LOCK == lock_flag) {
        /*
        mTask.txBuf[0] = RA_DSP_CONFIG_CTRL1_0xc8;
        ret = i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                                mTask.txBuf, 1, NULL, NULL);
        if(0 != ret) {
            PRINT_ERR("hx9031a_read failed\n");
        }*/

        mTask.txBuf[0] = RA_DSP_CONFIG_CTRL1_0xc8;
        mTask.txBuf[1] = 0x10;
        ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                       NULL, NULL);
        if(0 != ret) {
            PRINT_ERR("hx9031a_write failed\n");
        }
    } else if(HX9031A_DATA_UNLOCK == lock_flag) {
                /*
        mTask.txBuf[0] = RA_DSP_CONFIG_CTRL1_0xc8;
        ret = i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                                mTask.txBuf, 1, NULL, NULL);
        if(0 != ret) {
            PRINT_ERR("hx9031a_read failed\n");
        }*/

        mTask.txBuf[0] = RA_DSP_CONFIG_CTRL1_0xc8;
        mTask.txBuf[1] = 0x00;
        ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                       NULL, NULL);
        if(0 != ret) {
            PRINT_ERR("hx9031a_write failed\n");
        }
    } else {
        PRINT_ERR("ERROR!!! hx9031a_data_lock wrong para. now do data unlock!\n");
        /*
        mTask.txBuf[0] = RA_DSP_CONFIG_CTRL1_0xc8;
        ret = i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                                mTask.txBuf, 1, NULL, NULL);
        if(0 != ret) {
            PRINT_ERR("hx9031a_read failed\n");
        }*/

        mTask.txBuf[0] = RA_DSP_CONFIG_CTRL1_0xc8;
        mTask.txBuf[1] = 0x00;
        ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                       NULL, NULL);
        if(0 != ret) {
            PRINT_ERR("hx9031a_write failed\n");
        }
    }
}

static uint8_t hx9031a_get_board_info(void)
{
    ENTER;

    return HX9031A_ON_BOARD;
}

static void hx9031a_ch_cfg(uint8_t chip_select)
{
    int ret = -1;
    int ii = 0;
    uint16_t ch_cfg = 0;
    uint8_t cfg[HX9031A_CH_NUM * 2] = {0};

    uint8_t CS0 = 0;
    uint8_t CS1 = 0;
    //uint8_t CS2 = 0;
    uint8_t CS3 = 0;
    uint8_t CS4 = 0;
    uint8_t NA = 0;
    uint8_t CH0_POS = 0;
    uint8_t CH0_NEG = 0;
    uint8_t CH1_POS = 0;
    uint8_t CH1_NEG = 0;
    uint8_t CH2_POS = 0;
    uint8_t CH2_NEG = 0;
    uint8_t CH3_POS = 0;
    uint8_t CH3_NEG = 0;

    ENTER;

    //CS NO. are have different order in the hx9031a and hx9023
    if(HX9023E_ON_BOARD == chip_select) {
        CS0 = 0;
        CS1 = 1;
        //CS2 = 2;
        //CS3 = 3;
        //CS4 = 4;
        NA = 10;
        PRINT_INF("HX9023E_ON_BOARD\n");
    } else if(HX9031A_ON_BOARD == chip_select) {
        CS0 = 2;
        CS1 = 1;
     //   CS2 = 3;
        CS3 = 0;
        CS4 = 4;
        NA = 10;
        PRINT_INF("HX9031A_ON_BOARD\n");
    } else {
        PRINT_ERR("unknow chip on board\n");
    }

    //remap channels start for customer design
    //each channel positive and negative all can connect to the each channel
    if(HX9023E_ON_BOARD == chip_select) {
        CH0_POS = CS1;	//CS1 ANT
        CH0_NEG = NA;	//CS0 REF
        CH1_POS = NA;
        CH1_NEG = NA;
        CH2_POS = NA;
        CH2_NEG = NA;
        CH3_POS = NA;
        CH3_NEG = NA;
    } else if(HX9031A_ON_BOARD == chip_select) {
        CH0_POS = CS0;
        CH0_NEG = CS1;  //yorke CS1 NA
        CH1_POS = CS4;
        CH1_NEG = CS3;
        CH2_POS = NA;
        CH2_NEG = NA;
        CH3_POS = NA;
        CH3_NEG = NA;
    } else {
        PRINT_ERR("unknow chip on board\n");
    }
    //remap channels end for customer design

    ch_cfg = (uint16_t)((0x03 << (CH0_POS * 2)) + (0x02 << (CH0_NEG * 2)));
    cfg[ii++] = (uint8_t)(ch_cfg);
    cfg[ii++] = (uint8_t)(ch_cfg >> 8);

    ch_cfg = (uint16_t)((0x03 << (CH1_POS * 2)) + (0x02 << (CH1_NEG * 2)));
    cfg[ii++] = (uint8_t)(ch_cfg);
    cfg[ii++] = (uint8_t)(ch_cfg >> 8);

    ch_cfg = (uint16_t)((0x03 << (CH2_POS * 2)) + (0x02 << (CH2_NEG * 2)));
    cfg[ii++] = (uint8_t)(ch_cfg);
    cfg[ii++] = (uint8_t)(ch_cfg >> 8);

    ch_cfg = (uint16_t)((0x03 << (CH3_POS * 2)) + (0x02 << (CH3_NEG * 2)));
    cfg[ii++] = (uint8_t)(ch_cfg);
    cfg[ii++] = (uint8_t)(ch_cfg >> 8);


    mTask.txBuf[0] = RA_CH0_CFG_7_0_0x03;
    mTask.txBuf[1] = cfg[0];
    mTask.txBuf[2] = cfg[1];
    mTask.txBuf[3] = cfg[2];
    mTask.txBuf[4] = cfg[3];
    ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 5,
                   NULL, NULL);
    if(0 != ret) {
        PRINT_ERR("hx9031a_write failed\n");
    }
    mTask.txBuf[0] = RA_CH2_CFG_7_0_0x07;
    mTask.txBuf[1] = cfg[4];
    mTask.txBuf[2] = cfg[5];
    mTask.txBuf[3] = cfg[6];
    mTask.txBuf[4] = cfg[7];
    ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 5,
                   NULL, NULL);
    if(0 != ret) {
        PRINT_ERR("hx9031a_write failed\n");
    }
}

static void hx9031a_register_init1(void)
{
    int ii = 0;
    int ret = -1;
    //PRINT_DBG("reg_list len =%d\n", ARRAY_SIZE(hx9031a_reg_init_list1));
    while(ii < ARRAY_SIZE(hx9031a_reg_init_list1)) {
        PRINT_DBG("WRITE:Reg0x%02X=0x%02X\n", hx9031a_reg_init_list1[ii].addr, hx9031a_reg_init_list1[ii].value);
        mTask.txBuf[0] = hx9031a_reg_init_list1[ii].addr;
        mTask.txBuf[1] = hx9031a_reg_init_list1[ii].value;
        ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                   NULL, NULL);
        if(0 != ret) {
            PRINT_ERR("hx9031a_write failed\n");
        }

        if(hx9031a_monitor_addr == hx9031a_reg_init_list1[ii].addr)
            hx9031a_monitor_val = hx9031a_reg_init_list1[ii].value;

        ii++;
    }
}
static void hx9031a_register_init2(void)
{
    int ii = 0;
    int ret = -1;
    //PRINT_DBG("reg_list len =%d\n", ARRAY_SIZE(hx9031a_reg_init_list2));
    while(ii < ARRAY_SIZE(hx9031a_reg_init_list2)) {
        PRINT_DBG("WRITE:Reg0x%02X=0x%02X\n", hx9031a_reg_init_list2[ii].addr, hx9031a_reg_init_list2[ii].value);
        mTask.txBuf[0] = hx9031a_reg_init_list2[ii].addr;
        mTask.txBuf[1] = hx9031a_reg_init_list2[ii].value;
        ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                   NULL, NULL);
        if(0 != ret) {
            PRINT_ERR("hx9031a_write failed\n");
        }

        if(hx9031a_monitor_addr == hx9031a_reg_init_list2[ii].addr)
            hx9031a_monitor_val = hx9031a_reg_init_list2[ii].value;

        ii++;
    }
}
static void hx9031a_register_init3(void)
{
    int ii = 0;
    int ret = -1;
    //PRINT_DBG("reg_list len =%d\n", ARRAY_SIZEARRAY_SIZE(hx9031a_reg_init_list3));
    while(ii < ARRAY_SIZE(hx9031a_reg_init_list3)) {
        PRINT_DBG("WRITE:Reg0x%02X=0x%02X\n", hx9031a_reg_init_list3[ii].addr, hx9031a_reg_init_list3[ii].value);
        mTask.txBuf[0] = hx9031a_reg_init_list3[ii].addr;
        mTask.txBuf[1] = hx9031a_reg_init_list3[ii].value;
        ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                   NULL, NULL);
        if(0 != ret) {
            PRINT_ERR("hx9031a_write failed\n");
        }

        if(hx9031a_monitor_addr == hx9031a_reg_init_list3[ii].addr)
            hx9031a_monitor_val = hx9031a_reg_init_list3[ii].value;

        ii++;
    }
}
static void hx9031a_register_init4(void)
{
    int ii = 0;
    int ret = -1;
   // PRINT_DBG("reg_list len =%d\n", ARRAY_SIZE(hx9031a_reg_init_list4));
    while(ii < ARRAY_SIZE(hx9031a_reg_init_list4)) {
        PRINT_DBG("WRITE:Reg0x%02X=0x%02X\n", hx9031a_reg_init_list4[ii].addr, hx9031a_reg_init_list4[ii].value);
        mTask.txBuf[0] = hx9031a_reg_init_list4[ii].addr;
        mTask.txBuf[1] = hx9031a_reg_init_list4[ii].value;
        ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                   NULL, NULL);
        if(0 != ret) {
            PRINT_ERR("hx9031a_write failed\n");
        }

        if(hx9031a_monitor_addr == hx9031a_reg_init_list4[ii].addr)
            hx9031a_monitor_val = hx9031a_reg_init_list4[ii].value;

        ii++;
    }
}
static void hx9031a_register_init5(void)
{
    int ii = 0;
    int ret = -1;
    //PRINT_DBG("reg_list len =%d\n", ARRAY_SIZE(hx9031a_reg_init_list5));
    while(ii < ARRAY_SIZE(hx9031a_reg_init_list5)) {
        PRINT_DBG("WRITE:Reg0x%02X=0x%02X\n", hx9031a_reg_init_list5[ii].addr, hx9031a_reg_init_list5[ii].value);
        mTask.txBuf[0] = hx9031a_reg_init_list5[ii].addr;
        mTask.txBuf[1] = hx9031a_reg_init_list5[ii].value;
        ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                   NULL, NULL);
        if(0 != ret) {
            PRINT_ERR("hx9031a_write failed\n");
        }

        if(hx9031a_monitor_addr == hx9031a_reg_init_list5[ii].addr)
            hx9031a_monitor_val = hx9031a_reg_init_list5[ii].value;

        ii++;
    }
}
static void hx9031a_register_init6(void)
{
    int ii = 0;
    int ret = -1;
    //PRINT_DBG("reg_list len =%d\n", ARRAY_SIZE(hx9031a_reg_init_list6));
    while(ii < ARRAY_SIZE(hx9031a_reg_init_list6)) {
        PRINT_DBG("WRITE:Reg0x%02X=0x%02X\n", hx9031a_reg_init_list6[ii].addr, hx9031a_reg_init_list6[ii].value);
        mTask.txBuf[0] = hx9031a_reg_init_list6[ii].addr;
        mTask.txBuf[1] = hx9031a_reg_init_list6[ii].value;
        ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                   NULL, NULL);
        if(0 != ret) {
            PRINT_ERR("hx9031a_write failed\n");
        }

        if(hx9031a_monitor_addr == hx9031a_reg_init_list6[ii].addr)
            hx9031a_monitor_val = hx9031a_reg_init_list6[ii].value;

        ii++;
    }
}
static void hx9031a_register_init7(void)
{
    int ii = 0;
    int ret = -1;
    //PRINT_DBG("reg_list len =%d\n", ARRAY_SIZE(hx9031a_reg_init_list7));
    while(ii < ARRAY_SIZE(hx9031a_reg_init_list7)) {
        PRINT_DBG("WRITE:Reg0x%02X=0x%02X\n", hx9031a_reg_init_list7[ii].addr, hx9031a_reg_init_list7[ii].value);
        mTask.txBuf[0] = hx9031a_reg_init_list7[ii].addr;
        mTask.txBuf[1] = hx9031a_reg_init_list7[ii].value;
        ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                   NULL, NULL);
        if(0 != ret) {
            PRINT_ERR("hx9031a_write failed\n");
        }

        if(hx9031a_monitor_addr == hx9031a_reg_init_list7[ii].addr)
            hx9031a_monitor_val = hx9031a_reg_init_list7[ii].value;

        ii++;
    }
}

/*
static void hx9031a_manual_offset_calibration(uint8_t ch_id)
{
    int ret = 0;
    uint8_t buf[1] = {0};

    ENTER;

    mTask.i2c_addr = RA_OFFSET_CALI_CTRL_0xc2;        
    ret = i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                            &mTask.chipId, 1, NULL, NULL);
    if(0 != ret) {
        PRINT_ERR("hx9031a_read failed\n");
    }

    buf[0] |= (1 << (ch_id + 4));
    mTask.txBuf[0] = RA_OFFSET_CALI_CTRL_0xc2;
    mTask.txBuf[1] = buf[0];
   
    ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
               NULL, NULL);
    if(0 != ret) {
        PRINT_ERR("hx9031a_write failed\n");
    }
}
*/


static uint16_t hx9031a_set_thres_near(uint8_t ch, uint16_t val)
{
    int ret = -1;
    
    val /= 32;
    hx9031a_ch_thres[ch].thr_near = (val & 0x03FF) * 32;

    mTask.txBuf[0] = RA_PROX_HIGH_DIFF_CFG_CH0_0_0x80 + (ch * CH_DATA_2BYTES);
    mTask.txBuf[1] = val & 0xFF;
    mTask.txBuf[2] = (val >> 8) & 0x03;
    ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 3,
               NULL, NULL);//10位！！！
    if(0 != ret) {
        PRINT_ERR("hx9031a_write failed\n");
    }

    PRINT_INF("hx9031a_ch_thres[%d].thr_near=%d\n", ch, hx9031a_ch_thres[ch].thr_near);
    return hx9031a_ch_thres[ch].thr_near;
}

static uint16_t hx9031a_set_thres_far(uint8_t ch, uint16_t val)
{
    int ret = -1;

    val /= 32;
    hx9031a_ch_thres[ch].thr_far = (val & 0x03FF) * 32;

    mTask.txBuf[0] = RA_PROX_LOW_DIFF_CFG_CH0_0_0x88 + (ch * CH_DATA_2BYTES);
    ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 3,
               NULL, NULL);//10位！！！
    if(0 != ret) {
        PRINT_ERR("hx9031a_write failed\n");
    }

    PRINT_INF("hx9031a_ch_thres[%d].thr_far=%d\n", ch, hx9031a_ch_thres[ch].thr_far);
    return hx9031a_ch_thres[ch].thr_far;
}

//raw data and bl data use 0xE8 register，lp data and diff data use 0xF4 register，this function for switch data。
static void hx9031a_data_switch(uint8_t output_switch)
{
    int ret = -1;
    uint8_t buf[1] = {0};

    //7:4 raw(1:bl 0:raw) 3:0 diff(1:diff 0:lp)
    if(HX9031A_OUTPUT_RAW_DIFF == output_switch) { //raw & diff
        hx9031a_output_switch = HX9031A_OUTPUT_RAW_DIFF;
        buf[0] = 0x0F;
        PRINT_INF("output data: raw & diff\n");
    } else { //lp & bl
        hx9031a_output_switch = HX9031A_OUTPUT_LP_BL;
        buf[0] = 0xF0;
        PRINT_INF("output data: lp & bl\n");
    }

    mTask.txBuf[0] = RA_RAW_BL_RD_CFG_0x38;
    mTask.txBuf[1] = buf[0];
    ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
               NULL, NULL);//10位！！！
    if(0 != ret) {
        PRINT_ERR("hx9031a_write failed\n");
    }

}


static int hx9031a_ch_en(uint8_t ch_id, uint8_t en)
{
    int ret = 0;

    if(ch_enable_status > 0) {
        if(1 == en) {
            ch_enable_status |= (1 << ch_id);
            PRINT_INF("ch_%d enabled\n", ch_id);
        } else {
            ch_enable_status &= ~(1 << ch_id);
            if(0 == ch_enable_status) {
                mTask.txBuf[0] = RA_CH_NUM_CFG_0x24;
                mTask.txBuf[1] = 0x00;
                ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                           NULL, NULL);//10位！！！
                if(0 != ret) {
                    PRINT_ERR("hx9031a_write failed\n");
                }
                //cancel_delayed_work_sync(&(mTask.hw_monitor_work));
                PRINT_INF("ch_%d disabled, all channels disabled\n", ch_id);
            } else {
                PRINT_INF("ch_%d disabled\n", ch_id);
            }
        }
    } else {
        if(1 == en) {
            mTask.txBuf[0] = RA_CH_NUM_CFG_0x24;
            mTask.txBuf[1] = HX9031A_OPEN_CHANNEL;
            ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                       NULL, NULL);//10bits
            if(0 != ret) {
                PRINT_ERR("hx9031a_write failed\n");
            }

            ch_enable_status |= (1 << ch_id);
            //schedule_delayed_work(&mTask.hw_monitor_work, msecs_to_jiffies(10000));
            PRINT_INF("ch_%d enabled\n", ch_id);
        } else {
            PRINT_INF("all channels disabled already\n");
        }
    }

    return ret;
}




static int hx9031a_set_enable(unsigned int enable)
{
    int ret = -1;
    int ii = 0;

#ifdef HX9031A_EXTERN_ALG
    if(1 == enable)
    {
        hx9031a_set_first_time(HX9031A_OPEN_CHANNEL);
    }
    else
    {
        hx9031a_reset_first_time_flag();
    }
#endif

    for (ii = 0; ii < ARRAY_SIZE(hx9031a_channels); ii++) {
        if (1 == enable) {
            PRINT_INF("enable ch_%d(name:%s)\n", ii, hx9031a_channels[ii].name);
            ret = hx9031a_ch_en(ii, 1);
            if(0 != ret) {
                PRINT_ERR("hx9031a_ch_en failed\n");
                return -1;
            }
            //hx9031a_channels[ii].state = IDLE;
            //hx9031a_channels[ii].enabled = true;
/*
            mTask.sarIntStatus = 0;
            mTask.data.sarData[0] = mTask.sarIntStatus;
            mTask.data.sensType = SENS_TYPE_SAR;
            PRINT_DBG("sar report data:%d\n", mTask.data.sarState);
            txTransferDataInfo(&mTask.dataInfo, 1, &mTask.data);
*/
        } else if (0 == enable) {
            PRINT_INF("disable ch_%d(name:%s)\n", ii, hx9031a_channels[ii].name);
            ret = hx9031a_ch_en(ii, 0);
            if(0 != ret) {
                PRINT_ERR("hx9031a_ch_en failed\n");
                return -1;
            }
            //hx9031a_channels[ii].state = IDLE;
            //hx9031a_channels[ii].enabled = false;
/*
            mTask.sarIntStatus = IDLE;
            mTask.data.sarData[0] = -1;
            mTask.data.sensType = SENS_TYPE_SAR;
            PRINT_DBG("sar report data:%d\n", mTask.data.sarState);
            txTransferDataInfo(&mTask.dataInfo, 1, &mTask.data);
*/
        } else {
            PRINT_ERR("unknown enable symbol\n");
        }
   }


    return 0;
}

/*
static int hx9031a_reg_reinitialize(void)
{
    int ret = -1;
    int ii = 0;
    uint8_t chip_select = 0;

    ENTER;
    hx9031a_register_init1();
    hx9031a_register_init2();
    hx9031a_register_init3();
    hx9031a_register_init4();
    hx9031a_register_init5();
    hx9031a_register_init6();
    hx9031a_register_init7();
    chip_select = hx9031a_get_board_info();
    hx9031a_ch_cfg(chip_select);
    hx9031a_data_switch(HX9031A_OUTPUT_RAW_DIFF);

    for(ii = 0; ii < HX9031A_CH_NUM; ii++) {
        hx9031a_set_thres_near(ii, hx9031a_ch_thres[ii].thr_near);
        hx9031a_set_thres_far(ii, hx9031a_ch_thres[ii].thr_far);
    }

    if(ch_enable_status > 0) {
        mTask.txBuf[0] = RA_CH_NUM_CFG_0x24;
        mTask.txBuf[1] = CHANNEL_USED;
        ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                   NULL, NULL);//10位！！！
        if(0 != ret) {
            PRINT_ERR("hx9031a_write failed\n");
            return -1;
        }
    }
    return 0;
}
*/
/////////

static int hx9031aCheckHwState(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                              void *inBuf, uint8_t inSize, uint8_t elemInSize,
                              void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    ENTER;
    int ret = -1;
    uint8_t ii = 0;

    if (rxTransferDataInfo(&mTask.dataInfo, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize)) {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, ERROR_EVT);
        PRINT_ERR("rxTransferDataInfo error\n");
        return -1;
    }

    mTask.sarIntStatus = 0;
    mTask.sarIntStatusCmp = 0;
    for(ii = 0; ii < HX9031A_CH_NUM; ii++) {
        hx9031a_ch_near_state[ii] = 0;
    }


    mTask.txBuf[0] = hx9031a_monitor_addr;
    ret = i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                        mTask.rxBuf, 1, i2cCallBack, next_state);
    if(0 != ret) {
        PRINT_ERR("hx9031a_read failed\n");
    }
    
    return ret;
}


static int hx9031aStateMonitor(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                              void *inBuf, uint8_t inSize, uint8_t elemInSize,
                              void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    ENTER;
    int ret = -1;
    
    if(0 == hx9031a_hw_monitor_en_flag) {
        PRINT_INF("hw monitor is disabled\n");
        goto next_state_read;
    }

    if(hx9031a_monitor_val != mTask.rxBuf[0]) {
      PRINT_ERR("chip restarted abnormally! do reg reinitialize!\n");
      //hx9031a_reg_reinitialize();
    } else {
      PRINT_INF("OK\n");
    }
    //setFsmReset();
    
    next_state_read:
    mTask.txBuf[0] = RA_PROX_STATUS_0x6b;
    ret = i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                      mTask.rxBuf, 1, i2cCallBack, next_state);
    if(0 != ret) {
        PRINT_ERR("hx9031a_read failed\n");
    }

    return ret;
}


static int hx9031aGetThreNear(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                              void *inBuf, uint8_t inSize, uint8_t elemInSize,
                              void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    ENTER;
    int ret = -1;
    
    mTask.sarIntStatus = mTask.rxBuf[0];
    //PRINT_DBG("hx9031a sar int status: 0x%02X\n", mTask.sarIntStatus);

    
    mTask.txBuf[0] = RA_PROX_HIGH_DIFF_CFG_CH0_0_0x80;
    ret = i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                        mTask.rxBuf, HX9031A_CH_NUM*CH_DATA_2BYTES, i2cCallBack, next_state);
    if(0 != ret) {
        PRINT_ERR("hx9031a_read failed\n");
    }
    
    return ret;
}
static int hx9031aGetThresFar(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                              void *inBuf, uint8_t inSize, uint8_t elemInSize,
                              void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    ENTER;
    int ret = -1;
    uint8_t ii = 0;
    
    // get near threshold, buf data were got from last state.
    for(ii = 0; ii < HX9031A_CH_NUM; ii++) {
        hx9031a_ch_thres[ii].thr_near = (mTask.rxBuf[ii*CH_DATA_2BYTES] + ((mTask.rxBuf[ii*CH_DATA_2BYTES+1] & 0x03) << 8)) * 32;
        //PRINT_DBG("hx9031a sar thr_near:ch%d = 0x%02X\n",ii, hx9031a_ch_thres[ii].thr_near);
        if(data_diff[ii] >= hx9031a_ch_thres[ii].thr_near) {
            hx9031a_ch_near_state[ii] = 1;//near
            mTask.sarIntStatusCmp |= (1 << ii);
        }
    }

    PRINT_INF("mTask.sarIntStatus=0x%08X, mTask.sarIntStatusCmp=0x%08X, hx9031a_ch_near_state:%d-%d-%d-%d\n",
          mTask.sarIntStatus,
          mTask.sarIntStatusCmp,
          hx9031a_ch_near_state[3],
          hx9031a_ch_near_state[2],
          hx9031a_ch_near_state[1],
          hx9031a_ch_near_state[0]);
    
    //
    mTask.txBuf[0] = RA_PROX_LOW_DIFF_CFG_CH0_0_0x88;
    ret = i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                        mTask.rxBuf, HX9031A_CH_NUM*CH_DATA_2BYTES, i2cCallBack, next_state);
    if(0 != ret) {
        PRINT_ERR("hx9031a_read failed\n");
    }
    
    return ret;
}

static int hx9031aGetRawBl(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                        void *inBuf, uint8_t inSize, uint8_t elemInSize,
                        void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    ENTER;
    int ret = -1;
    uint8_t ii = 0;
    uint8_t bytes_all_cahnnel = 0;
    
    // get far threshold, buf data were got from last state.
    for(ii = 0; ii < HX9031A_CH_NUM; ii++) {
        hx9031a_ch_thres[ii].thr_far = (mTask.rxBuf[ii*CH_DATA_2BYTES] + ((mTask.rxBuf[ii*CH_DATA_2BYTES+1] & 0x03) << 8)) * 32;
        //PRINT_DBG("hx9031a sar thr_far:ch%d = 0x%02X\n",ii, hx9031a_ch_thres[ii].thr_far);
        if(data_diff[ii] >= hx9031a_ch_thres[ii].thr_near) {
            hx9031a_ch_near_state[ii] = 0;//far
        }
    }

    //read raw and bl data
    hx9031a_data_lock(HX9031A_DATA_LOCK);
    mTask.txBuf[0] = RA_RAW_BL_CH0_0_0xe8;

    bytes_all_cahnnel = HX9031A_CH_NUM*CH_DATA_3BYTES;
    if(bytes_all_cahnnel > 8)
    {
        bytes_all_cahnnel = 8;
    }
    ret = i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                        mTask.rxBuf, bytes_all_cahnnel, i2cCallBack, next_state);

    return ret;

}
static int hx9031aGetDiffLp(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                        void *inBuf, uint8_t inSize, uint8_t elemInSize,
                        void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    ENTER;
    int ret = -1;
    uint8_t ii = 0;
    uint8_t bytes_per_channel = 0;
    uint8_t bytes_all_cahnnel = 0;
    uint16_t data[HX9031A_CH_NUM] = {0};

    bytes_per_channel = CH_DATA_3BYTES;
    bytes_all_cahnnel = HX9031A_CH_NUM*CH_DATA_3BYTES;
    if(bytes_all_cahnnel>8)
    {
        mTask.txBuf[0] = RA_RAW_BL_CH0_0_0xe8 + 8;
        ret = i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                                        &mTask.rxBuf[8], bytes_all_cahnnel-8, NULL, NULL);        
    }

    for(ii = 0; ii < HX9031A_CH_NUM; ii++) {
        data[ii] = ((mTask.rxBuf[ii * bytes_per_channel + 2] << 8) | (mTask.rxBuf[ii * bytes_per_channel + 1]));//24bits data, discard low 8bits; only use high 16bits
        if(HX9031A_OUTPUT_RAW_DIFF == hx9031a_output_switch) {
            data_raw[ii] = (data[ii] > 0x7FFF) ? (data[ii] - (0xFFFF + 1)) : data[ii];
            data_bl[ii] = 0;
        } else {
            data_raw[ii] = 0;
            data_bl[ii] = (data[ii] > 0x7FFF) ? (data[ii] - (0xFFFF + 1)) : data[ii]; 
        }
    }

    //read diff and lp data
    mTask.txBuf[0] = RA_LP_DIFF_CH0_0_0xf4;    
    bytes_all_cahnnel = HX9031A_CH_NUM*CH_DATA_3BYTES;
    if(bytes_all_cahnnel > 8)
    {
        bytes_all_cahnnel = 8;
    }
    ret = i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                        mTask.rxBuf, bytes_all_cahnnel, i2cCallBack, next_state);
    return ret;
}
          
static int hx9031aGetOffset(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                        void *inBuf, uint8_t inSize, uint8_t elemInSize,
                        void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    ENTER;
    int ret = -1;
    uint8_t ii = 0;
    uint8_t bytes_per_channel = 0;
    uint8_t bytes_all_cahnnel = 0;
    uint16_t data[HX9031A_CH_NUM] = {0};

    bytes_per_channel = CH_DATA_3BYTES;
    bytes_all_cahnnel = HX9031A_CH_NUM*CH_DATA_3BYTES;
    if(bytes_all_cahnnel>8)
    {
        mTask.txBuf[0] = RA_LP_DIFF_CH0_0_0xf4 + 8;
        ret = i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                                        &mTask.rxBuf[8], bytes_all_cahnnel-8, NULL, NULL);        
    }
    
    for(ii = 0; ii < HX9031A_CH_NUM; ii++) {
        data[ii] = ((mTask.rxBuf[ii * bytes_per_channel + 2] << 8) | (mTask.rxBuf[ii * bytes_per_channel + 1]));//24bits data, discard low 8bits; only use high 16bits
        if(HX9031A_OUTPUT_RAW_DIFF == hx9031a_output_switch) {
            data_diff[ii] = (data[ii] > 0x7FFF) ? (data[ii] - (0xFFFF + 1)) : data[ii]; 
            data_lp[ii] = 0;
        } else {
            data_diff[ii] = 0;
            data_lp[ii] = (data[ii] > 0x7FFF) ? (data[ii] - (0xFFFF + 1)) : data[ii]; 
        }
    }
    
    //read offset
    mTask.txBuf[0] = RA_OFFSET_DAC0_7_0_0x15;
    ret = i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                        mTask.rxBuf, HX9031A_CH_NUM*CH_DATA_2BYTES, i2cCallBack, next_state);
    return ret;
}


static int hx9031aSample(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                        void *inBuf, uint8_t inSize, uint8_t elemInSize,
                        void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    ENTER;

    uint8_t ii = 0;
    uint8_t bytes_per_channel = 0;

    uint16_t data[HX9031A_CH_NUM] = {0};
    bytes_per_channel = CH_DATA_2BYTES;

    for(ii = 0; ii < HX9031A_CH_NUM; ii++) {
        data[ii] = ((mTask.rxBuf[ii * bytes_per_channel + 1] << 8) | (mTask.rxBuf[ii * bytes_per_channel]));
        data[ii] = data[ii] & 0xFFF;    //12bits
        data_offset_dac[ii] = data[ii];
    }

    hx9031a_data_lock(HX9031A_DATA_UNLOCK);

    PRINT_DBG("DIFF  : %8d, %8d, %8d, %8d\n", data_diff[0], data_diff[1], data_diff[2], data_diff[3]);
    PRINT_DBG("RAW   : %8d, %8d, %8d, %8d\n", data_raw[0], data_raw[1], data_raw[2], data_raw[3]);
    PRINT_DBG("OFFSET: %8d, %8d, %8d, %8d\n", data_offset_dac[0], data_offset_dac[1], data_offset_dac[2], data_offset_dac[3]);
    PRINT_DBG("BL    : %8d, %8d, %8d, %8d\n", data_bl[0], data_bl[1], data_bl[2], data_bl[3]);
    PRINT_DBG("LP    : %8d, %8d, %8d, %8d\n", data_lp[0], data_lp[1], data_lp[2], data_lp[3]);

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int hx9031aConvert(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    ENTER;

#ifdef HX9031A_EXTERN_ALG
    uint8_t ii =0;

    //external alg
    if(0 != hx9031a_get_first_time_flag())
    {
        for(ii = 0; ii < HX9031A_CH_NUM; ii++)
        {
            if((ch_enable_first_time & (1 << ii)) != 0)
            {
                hx9031a_set_first_time_defalut_value(data_raw[ii],ii); 
            }
        }                
        hx9031a_reset_first_time_flag();
    } 

    
    for(ii = 0; ii < HX9031A_CH_NUM; ii++) 
    {
        lp_deglitch[ii]=hx9031a_get_raw_deglitch_value(data_lp[ii], ii);          
        //PRINT_DBG("hx9031a ch[%d].deglitch = %d ", i, channel_data[i].raw_data_deglitch);

        if(ii == 2)
        {
         //   lp_deglitch[ii] = hx9031a_raw_channel_diff(lp_deglitch[ii],lp_deglitch[ii - 1]); //channel diff ch2+, ch1-;
        }          
        //PRINT_DBG("hx9031a ch[%d].deglitch = %d ", i, channel_data[i].raw_data_deglitch);

        hx9031a_prox_calculation(&lp_deglitch[ii],&data_prox[ii],&prox_status_channel[ii],data_lp[ii],ii);

        if(data_prox[ii] < 0)
        {
            data_prox[ii] = 0;
        }
    }

    if(hx9031a_alg_bl_error_flag != 0)
    {
        //hx9031a_mannual_reoffset(instance, hx90xx_alg_bl_error_flag);
        hx9031a_alg_bl_error_flag = 0;
    }

    PRINT_DBG("cvt_deglitch: %8d, %8d, %8d, %8d\n", lp_deglitch[0], lp_deglitch[1], lp_deglitch[2], lp_deglitch[3]);
    PRINT_DBG("cvt_prox   : %8d, %8d, %8d, %8d\n", data_prox[0], data_prox[1], data_prox[2], data_prox[3]);
    PRINT_DBG("cvt_status : %8d, %8d, %8d, %8d\n", prox_status_channel[0], prox_status_channel[1], prox_status_channel[2], prox_status_channel[3]);
#endif


    /*
    mTask.data.sarData[0] = mTask.factoryData.ix;	//
    mTask.data.sarData[1] = mTask.factoryData.iy;	//
    mTask.data.sarData[2] = mTask.factoryData.iz;	//

    mTask.data.sarData[0] = SAR_STATE_NEAR; //or  SAR_STATE_FAR
    mTask.data.sarData[1] = SAR_STATE_NEAR; //or  SAR_STATE_FAR
    mTask.data.sarData[2] = SAR_STATE_NEAR; //or  SAR_STATE_FAR
    */
    //hx9031a_channels[].state;

    mt_eint_unmask(mTask.hw->eint_num); 

#ifdef HX9031A_EXTERN_ALG
    mTask.data.sarState = prox_status_channel[0];
    mTask.data.sarData[0] =data_prox[0];
#else
    if (mTask.sarIntStatus)
        mTask.data.sarState = SAR_STATE_NEAR;
    else
        mTask.data.sarState = SAR_STATE_FAR;
#endif
    mTask.data.sarData[0] = mTask.sarIntStatus;
    mTask.data.sarData[1] = data_offset_dac[0];
    mTask.data.sarData[2] = data_diff[0];
    mTask.data.sensType = SENS_TYPE_SAR;
    PRINT_DBG("sar report data:%d\n", mTask.sarIntStatus);
//-Maple 437,xuyanan.wt,modify,20211030,factoyr ch0 and ch0diff report	
    txTransferDataInfo(&mTask.dataInfo, 1, &mTask.data);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}
//+Maple 437,xuyanan.wt,modify,20211102,seft-cali	
static int hx9031aEnable(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                        void *inBuf, uint8_t inSize, uint8_t elemInSize,
                        void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    ENTER;
    int ret = -1;

    ret = hx9031a_set_enable(1);
    mTask.txBuf[0] = RA_OFFSET_CALI_CTRL_0xc2;
    mTask.txBuf[1] = 0xf0;
   
    ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
               NULL, NULL);
 
    PRINT_DBG("enablecali: %d\n",mTask.txBuf[0]);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return ret;
}
//-Maple 437,xuyanan.wt,modify,20211102,seft-cali	

static int hx9031aDisable(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    ENTER;
    int ret = -1;

    ret = hx9031a_set_enable(0);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return ret;

}

static int hx9031aRate(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                      void *inBuf, uint8_t inSize, uint8_t elemInSize,
                      void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    ENTER;
    int ret = 0;
    struct SarCntlPacket cntlPacket = {0};

    ret = rxControlInfo(&cntlPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);
    if (ret < 0) {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, ERROR_EVT);
        PRINT_ERR("hx9031aRate, rx inSize and elemSize error\n");
        return -1;
    }

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int hx9031aCali(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                      void *inBuf, uint8_t inSize, uint8_t elemInSize,
                      void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    ENTER;
    int ret = 0;
    //void sarSendCalibrationResult(uint8_t sensorType,
    //int32_t bias[3], int8_t status); //status:0 sucess;-1 fail

    mTask.txBuf[0] = RA_OFFSET_CALI_CTRL_0xc2;
    mTask.txBuf[1] = 0xf0;
   
    ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
               NULL, NULL);

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return ret;
}

static int hx9031aReset(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                       void *inBuf, uint8_t inSize, uint8_t elemInSize,
                       void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    ENTER;
    int ret = -1;

    mTask.txBuf[0] = RA_CH_NUM_CFG_0x24;
    mTask.txBuf[1] = 0x00;

    ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2, i2cCallBack, next_state);
    if(0 != ret) {
        PRINT_ERR("i2c tx error\n");
    }
    
    return ret;
}

static int hx9031aRegInit1(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
     int ret = 0;

    ENTER;

    hx9031a_register_init1();
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return ret;

}
 static int hx9031aRegInit2(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                          void *inBuf, uint8_t inSize, uint8_t elemInSize,
                          void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
 {
     int ret = 0;

     ENTER;
 
     hx9031a_register_init2();
     sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
     return ret;
 
 }
static int hx9031aRegInit3(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                       void *inBuf, uint8_t inSize, uint8_t elemInSize,
                       void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
   int ret = 0;

   ENTER;
 
  hx9031a_register_init3();
  sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
  return ret;

}
static int hx9031aRegInit4(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                      void *inBuf, uint8_t inSize, uint8_t elemInSize,
                      void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;

    ENTER;

    hx9031a_register_init4();
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return ret;

}
static int hx9031aRegInit5(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                     void *inBuf, uint8_t inSize, uint8_t elemInSize,
                     void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;

    ENTER;

    hx9031a_register_init5();
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return ret;

}
static int hx9031aRegInit6(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                    void *inBuf, uint8_t inSize, uint8_t elemInSize,
                    void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
   int ret = 0;

   ENTER;

   hx9031a_register_init6();
   sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
   return ret;

}

static int hx9031aRegInit7(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ii = 0;
    int ret = 0;

    ENTER;

    hx9031a_register_init7();

    for (ii = 0; ii < ARRAY_SIZE(hx9031a_channels); ii++) {
        if (HX9031A_OPEN_CHANNEL >> ii & 0x1) { 
            //hx9031a_channels[ii].used = true;
            //hx9031a_channels[ii].state = IDLE;
        }
    }
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return ret;

}

static int hx9031aChCfg(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint8_t chip_select = 0;
    int ret = 0;

    ENTER;

    chip_select = hx9031a_get_board_info();
    hx9031a_ch_cfg(chip_select);

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return ret;
}
static int hx9031aDataSwitch(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;     
    ENTER;

    hx9031a_data_switch(HX9031A_OUTPUT_TYPE);

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return ret;
}
static int hx9031aSetThresNear(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ii = 0;
    int ret = 0;
     
    ENTER;

    for(ii = 0; ii < HX9031A_CH_NUM; ii++) {
        hx9031a_set_thres_near(ii, hx9031a_ch_thres[ii].thr_near);
    }

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return ret;
}
static int hx9031aSetThresFar(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ii = 0;
    int ret = 0;

    ENTER;

    for(ii = 0; ii < HX9031A_CH_NUM; ii++) {
        hx9031a_set_thres_far(ii, hx9031a_ch_thres[ii].thr_far);
    }

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return ret;
}


static int hx9031aDelay(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                       void *inBuf, uint8_t inSize, uint8_t elemInSize,
                       void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint64_t timer_delay;
    timer_delay = 1000000000ull / 100; //delay 10ms
    PRINT_DBG("timer_delay=%ulns\n", timer_delay);

    if (mTask.timerHandle)
        timTimerCancel(mTask.timerHandle);

    //last parameter: true= one time interrupt for delay
    mTask.timerHandle = timTimerSet(timer_delay, 0, 50, delayCallback, next_state, true);
    if (mTask.timerHandle == 0)
        configASSERT(0);
    return 0;
}

static void sarGetSensorInfo(struct sensorInfo_t *data)
{
    ENTER;
    strncpy(data->name, HX9031A_DRIVER_NAME, sizeof(data->name));
}

static void sarGetData(void *sample)
{
    ENTER;
    struct TripleAxisDataPoint *tripleSample = (struct TripleAxisDataPoint *)sample;
    tripleSample->ix = mTask.factoryData.ix;
    tripleSample->iy = mTask.factoryData.iy;
    tripleSample->iz = mTask.factoryData.iz;
}

static int hx9031aMaskEint(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    ENTER;
    if(SAR_WORK_MODE == SAR_INTERRUPT_MODE){
        PRINT_DBG("sar mTask.hw->eint_num: %d\n",mTask.hw->eint_num);
        mt_eint_mask(mTask.hw->eint_num);
    }

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

SRAM_REGION_FUNCTION void hx9031aEintHandler(int arg)
{
    ENTER;
    if(SAR_WORK_MODE == SAR_INTERRUPT_MODE){
        PRINT_DBG("sar mTask.hw->eint_num: %d\n",mTask.hw->eint_num);
        sarInterruptOccur();
    }
}

/*  Need add unmask operation to ratechg, if sensor has setting rate */
static int hx9031aUnmaskEint(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    ENTER;
    if(SAR_WORK_MODE == SAR_INTERRUPT_MODE){
        PRINT_DBG("sar mTask.hw->eint_num: %d\n",mTask.hw->eint_num);
        mt_eint_dis_hw_debounce(mTask.hw->eint_num); /* MT6797 use 11 EDGE_SENSITIVE, HIGH_LEVEL_TRIGGER LOW_LEVEL_TRIGGER*/
        mt_eint_registration(mTask.hw->eint_num, LEVEL_SENSITIVE, LOW_LEVEL_TRIGGER, hx9031aEintHandler, EINT_INT_UNMASK,
                             EINT_INT_AUTO_UNMASK_OFF); /* MT6797 use 11 */
    }

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}


static int hx9031aSetEint(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                              void *inBuf, uint8_t inSize, uint8_t elemInSize,
                              void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    ENTER;
    if(SAR_WORK_MODE == SAR_INTERRUPT_MODE){
        mt_eint_dis_hw_debounce(mTask.hw->eint_num); /* MT6797 use 11 HIGH_LEVEL_TRIGGER LOW_LEVEL_TRIGGER*/
        mt_eint_registration(mTask.hw->eint_num, LEVEL_SENSITIVE, LOW_LEVEL_TRIGGER, hx9031aEintHandler, EINT_INT_UNMASK,
                             EINT_INT_AUTO_UNMASK_OFF); /* MT6797 use 11 */
    }

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}


static int hx9031aRegisterCore(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                              void *inBuf, uint8_t inSize, uint8_t elemInSize,
                              void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    ENTER;

    int ret = -1;
    struct sensorCoreInfo mInfo;
    memset(&mInfo, 0x00, sizeof(struct sensorCoreInfo));
    /* Register sensor Core */
    mInfo.sensType = SENS_TYPE_SAR;
    mInfo.getData = sarGetData;
    mInfo.getSensorInfo = sarGetSensorInfo;
    ret = sensorCoreRegister(&mInfo);
    if(0 != ret) {
        PRINT_ERR("hx9031aRegisterCore Failed!\n");
        return ret;
    }

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static struct sensorFsm hx9031aFsm[] = {
    sensorFsmCmd(STATE_CHECK_INT, STATE_STATE_MONITOR, hx9031aCheckHwState),
    sensorFsmCmd(STATE_STATE_MONITOR, STATE_GET_THRES_NEAR, hx9031aStateMonitor),
    sensorFsmCmd(STATE_GET_THRES_NEAR, STATE_GET_THRES_FAR, hx9031aGetThreNear),
    sensorFsmCmd(STATE_GET_THRES_FAR, STATE_GET_RAW_BL, hx9031aGetThresFar),
    sensorFsmCmd(STATE_GET_RAW_BL, STATE_GET_DIFF_LP, hx9031aGetRawBl),
    sensorFsmCmd(STATE_GET_DIFF_LP, STATE_GET_OFFSET, hx9031aGetDiffLp),
    sensorFsmCmd(STATE_GET_OFFSET, STATE_SAMPLE, hx9031aGetOffset),
    sensorFsmCmd(STATE_SAMPLE, STATE_CONVERT, hx9031aSample),
    sensorFsmCmd(STATE_CONVERT, STATE_SAMPLE_DONE, hx9031aConvert),


    sensorFsmCmd(STATE_ENABLE, STATE_ENABLE_EINT, hx9031aEnable),
    sensorFsmCmd(STATE_ENABLE_EINT, STATE_ENABLE_DONE, hx9031aUnmaskEint),

    sensorFsmCmd(STATE_DISABLE, STATE_DISABLE_EINT, hx9031aDisable),
    sensorFsmCmd(STATE_DISABLE_EINT, STATE_DISABLE_DONE, hx9031aMaskEint),

    sensorFsmCmd(STATE_RATECHG, STATE_RATECHG_DONE, hx9031aRate),

    sensorFsmCmd(STATE_CALI, STATE_CALI_DONE, hx9031aCali),

    sensorFsmCmd(STATE_RESET, STATE_DEALY_1, hx9031aReset),
    sensorFsmCmd(STATE_DEALY_1, STATE_REG_INIT1, hx9031aDelay),
    sensorFsmCmd(STATE_REG_INIT1, STATE_REG_INIT2, hx9031aRegInit1),
    sensorFsmCmd(STATE_REG_INIT2, STATE_REG_INIT3, hx9031aRegInit2),
    sensorFsmCmd(STATE_REG_INIT3, STATE_REG_INIT4, hx9031aRegInit3),
    sensorFsmCmd(STATE_REG_INIT4, STATE_REG_INIT5, hx9031aRegInit4),
    sensorFsmCmd(STATE_REG_INIT5, STATE_REG_INIT6, hx9031aRegInit5),
    sensorFsmCmd(STATE_REG_INIT6, STATE_REG_INIT7, hx9031aRegInit6),
    sensorFsmCmd(STATE_REG_INIT7, STATE_CH_CFG, hx9031aRegInit7),
    sensorFsmCmd(STATE_CH_CFG, STATE_DATA_SWITCH, hx9031aChCfg),
    sensorFsmCmd(STATE_DATA_SWITCH, STATE_SET_THRES_NEAR, hx9031aDataSwitch),
    sensorFsmCmd(STATE_SET_THRES_NEAR, STATE_SET_THRE_FAR, hx9031aSetThresNear),
    sensorFsmCmd(STATE_SET_THRE_FAR, STATE_SETUP_EINT, hx9031aSetThresFar),
    sensorFsmCmd(STATE_SETUP_EINT, STATE_DEALY_2, hx9031aSetEint),
    sensorFsmCmd(STATE_DEALY_2, STATE_CORE, hx9031aDelay),
    sensorFsmCmd(STATE_CORE, STATE_INIT_DONE, hx9031aRegisterCore),
};

static int hx9031aInit(void)
{
    ENTER;
    int ret = -1;
    
    PRINT_DBG("driver_ver =%s\n", HX9031A_DRIVER_VER);

    mTask.hw = get_cust_sar("hx9031a");
    if (NULL == mTask.hw) {
        PRINT_ERR("hx9031a get_cust_sar fail\n");
        ret = -1;
        goto err_out;
    }
    mTask.i2c_addr = mTask.hw->i2c_addr[0];
    PRINT_INF("sar i2c_num: %d, i2c_addr: 0x%02X\n", mTask.hw->i2c_num, mTask.i2c_addr);

    i2cMasterRequest(mTask.hw->i2c_num, I2C_SPEED);
    mTask.txBuf[0] = RA_ID_REG_0x60;


    for (uint8_t ii = 0; ii < 3; ii++) {
        ret = i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                                &mTask.chipId, 1, NULL, NULL);

        PRINT_INF("ret =%d , hx9031a mTask.chipId %x\n", ret, mTask.chipId);

        if (ret >= 0 && HX90XX_CHIP_ID_9031A == mTask.chipId) {
            PRINT_INF("hx9031a auto detect success ID=0x%02X\n", mTask.chipId);
            goto success_out;
        } else
            ret = -1;
    }

    if (ret < 0) {
        ret = -1;
        PRINT_ERR("get hx9031a id failed: ID=0x%02X\n", mTask.chipId);
        i2cMasterRelease(mTask.hw->i2c_num);
        goto err_out;
    }

success_out:
    sarSensorRegister();
    registerSarDriverFsm(hx9031aFsm, ARRAY_SIZE(hx9031aFsm));
err_out:
    return ret;
}

#ifndef CFG_OVERLAY_INIT_SUPPORT
MODULE_DECLARE(hx9031a, SENS_TYPE_SAR, hx9031aInit);
#else
#include "mtk_overlay_init.h"
OVERLAY_DECLARE(hx9031a, OVERLAY_ID_SAR, hx9031aInit);
#endif
