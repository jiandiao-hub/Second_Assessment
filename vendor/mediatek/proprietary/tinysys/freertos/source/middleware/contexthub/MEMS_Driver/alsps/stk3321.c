/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <seos.h>
#include <util.h>
#include <sensors.h>
#include <plat/inc/rtc.h>
#include <contexthub_core.h>
#include <mt_gpt.h>
#include <timer.h>
#include "eint.h"
#include "alsps_separate.h"
#include "stk3321.h"

#define I2C_SPEED               400000
#define MAX_I2C_PER_PACKET      8
#define ICM20645_DATA_LEN       6
#define MAX_RXBUF               512
#define MAX_TXBUF               (MAX_RXBUF / MAX_I2C_PER_PACKET)
#define STK3321_PID_LIST_NUM    3

enum stk3321_State
{
#ifdef STK3321_ALS_ENABLE
    STATE_SAMPLE_ALS = CHIP_SAMPLING_ALS,
    STATE_SAMPLE_ALS_DONE = CHIP_SAMPLING_ALS_DONE,
    STATE_ALS_ENABLE = CHIP_ALS_ENABLE,
    STATE_ALS_ENABLE_DONE = CHIP_ALS_ENABLE_DONE,
    STATE_ALS_DISABLE = CHIP_ALS_DISABLE,
    STATE_ALS_DISABLE_DONE = CHIP_ALS_DISABLE_DONE,
    STATE_ALS_RATECHG = CHIP_ALS_RATECHG,
    STATE_ALS_RATECHG_DONE = CHIP_ALS_RATECHG_DONE,
    STATE_ALS_CALI = CHIP_ALS_CALI,
    STATE_ALS_CALI_DONE = CHIP_ALS_CALI_DONE,
    STATE_ALS_CFG = CHIP_ALS_CFG,
    STATE_ALS_CFG_DONE = CHIP_ALS_CFG_DONE,
#endif
#ifdef STK3321_PS_ENABLE
    STATE_SAMPLE_PS = CHIP_SAMPLING_PS,
#ifdef STK3321_PS_OC_REPORT
    STATE_SAMPLE_PS_ONCE = CHIP_SAMPLING_PS_ONCE,
#endif
    STATE_SAMPLE_PS_DONE = CHIP_SAMPLING_PS_DONE,
    STATE_PS_ENABLE = CHIP_PS_ENABLE,
    STATE_PS_ENABLE_DONE = CHIP_PS_ENABLE_DONE,
    STATE_PS_DISABLE = CHIP_PS_DISABLE,
    STATE_PS_DISABLE_DONE = CHIP_PS_DISABLE_DONE,
    STATE_PS_RATECHG = CHIP_PS_RATECHG,
    STATE_PS_RATECHG_DONE = CHIP_PS_RATECHG_DONE,
    STATE_PS_CALI = CHIP_PS_CALI,
    STATE_PS_CALI_DONE = CHIP_PS_CALI_DONE,
    STATE_PS_CFG = CHIP_PS_CFG,
    STATE_PS_CFG_DONE = CHIP_PS_CFG_DONE,
#ifdef STK_TUNE0
    STATE_PS_CT_TUNE = CHIP_PS_CT_TUNE,
    STATE_PS_CT_TUNE_DONE = CHIP_PS_CT_TUNE_DONE,
#endif
#endif
#if (defined(STK3321_ALS_ENABLE) && !defined(STK3321_PS_ENABLE))
    STATE_INIT_DONE = CHIP_PS_RESET,
#else
    STATE_INIT_DONE = CHIP_INIT_DONE,
#endif
    STATE_IDEL = CHIP_IDLE,
#if (defined(STK3321_PS_ENABLE) && !defined(STK3321_ALS_ENABLE))
    STATE_RESET = CHIP_PS_RESET,
#else
    STATE_RESET = CHIP_RESET,
#endif

    /* Alsps sample */
#ifdef STK3321_ALS_ENABLE
    STATE_GET_ALS_DR,
    STATE_GET_ALS_DATA,
    STATE_ALS_SET_DEBOUNCE,
    STATE_ALS_POWER_RESET_IT,
    STATE_ALS_POWER_ON,
    STATE_ALS_SET_DISABLE,
    STATE_GET_ALS_FLAG,
    STATE_CLR_INTR,
    STATE_ALS_POWER_OFF,

    //IRS
	STATE_IRS_SET_IT,
	STATE_IRS_ENABLE,
	STATE_IRS_GET_IR_FLAG,
	STATE_IRS_CHECK_IRS_READY,
	STATE_IRS_CLR_IRS_INT,
	STATE_IRS_GET_IR_START,
	STATE_IRS_DRUP_DATA_1,
	STATE_IRS_DRUP_DATA_2,
	STATE_IRS_DRUP_DATA_3,
	STATE_IRS_GET_IR_END,
#endif
#ifdef STK3321_PS_ENABLE
    STATE_GET_PS_RAW_DATA,
    STATE_GET_PS_RAW_DATA_ONCE,
    STATE_PS_SET_DEBOUNCE,
    /* Power on & off */
    STATE_GET_ALSPS_STATE,
    STATE_PS_POWER_ON,
    STATE_PS_POWER_OFF,
    STATE_PS_UNMASK_EINT,
    STATE_CLR_INT,
    STATE_EN_EINT,
#ifdef STK_TUNE0
    /* CT tracking */
    STATE_PS_CT_INVALID_FLAG,
    STATE_PS_CT_BGIR,
    STATE_PS_CT_RECALI,
    STATE_PS_MAX_MIN,
#endif
#endif
    /* Init state */
    STATE_SET_ALSPS_CONF,
    STATE_SET_DEFAULT_PS_THD,
    STATE_SET_DEFAULT_ALS_THD,
    STATE_SETUP_EINT,
    STATE_SET_BGIR,
    STATE_SET_ALSCTRL2,
    STATE_CORE
};

static struct stk3321Task
{
    // ALS parameters
#ifdef STK3321_ALS_ENABLE
    unsigned int    als_debounce_time;
    unsigned int    als_debounce_on;    /*indicates if the debounce is on*/
    portTickType    als_debounce_end;
    uint32_t        als_raw_data;
    int32_t alsCali;
    uint8_t als_pw_state;
    bool als_pw_ctl;
#ifdef ALS_MID_FIR
    stk3321_data_filter als_data_filter;
#endif
#endif
    // PS parameters
#ifdef STK3321_PS_ENABLE
    unsigned int    ps_debounce_time;
    unsigned int    ps_debounce_on;     /*indicates if the debounce is on*/
    portTickType    ps_debounce_end;
    unsigned int    ps_suspend;
    int32_t psCali;
    uint8_t last_prox_nearby;
    uint8_t ps_invalid_flag;
    uint16_t prox_raw_data;
#ifdef STK_TUNE0
    uint16_t psi_set;
    uint16_t psi;
    uint16_t psa;
    uint32_t ps_tracking_data[3];
    int8_t ps_tracking_data_count;
    bool ps_smudge_update;
    bool ps_is_turn_on;
    bool ps_valid;
    bool ps_thred_need_to_update;
#endif
#endif
    // Common parameters
    uint8_t txBuf[MAX_TXBUF];
    /* rxBuf for i2c operation, receive rawdata */
    uint8_t rxBuf[MAX_RXBUF];
    uint8_t deviceId;
    /* data for factory */
    struct alsps_hw *hw;
    uint8_t i2c_addr;
    struct transferDataInfo dataInfo;
    struct AlsPsData data;
    uint32_t    ps_threshold_high;
    uint32_t    ps_threshold_low;
    uint32_t    als_threshold_high;
    uint32_t    als_threshold_low;
    uint8_t     ledctrl_val;
    uint8_t     state_val;
    uint8_t     als_ctrl_val;
    uint8_t     ps_ctrl_val;
    uint8_t     clr_flag;
    bool        psPowerOn;
    bool        alsPowerOn;
    uint8_t		irs_running;
#ifdef STK_IRS
    bool		als_drup_data;
    uint8_t		irs_slp_time;
	uint32_t	als_data_index;
	uint16_t	als_correct_factor;
	uint16_t	ir_code;
	uint16_t    last_ir_code;
#endif
} mTask;

#ifdef STK3321_PS_ENABLE
    static char ps_cali_flag = 0;
#endif
static struct stk3321Task *stk3321DebugPoint;

/* ****************************************************
 * System control function
 * ****************************************************/
static void cal_end_time(unsigned int ms, portTickType *end_tick)
{
    *end_tick = xTaskGetTickCount() +
                (portTickType)(ms / (1000 / portTICK_RATE_MS));
}

static int stk3321_get_alsps_state(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                   void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                   void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.txBuf[0] = STK_STATE_REG;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 1, i2cCallBack,
                         next_state);
}

#ifdef STK_TUNE0
static void stk3321_ps_smudge_judgement()
{
    uint32_t raw_data = 0;

    if (mTask.prox_raw_data > mTask.psi)
    {
        raw_data = mTask.prox_raw_data - mTask.psi;
        //osLog(LOG_ERROR, "%s:: raw_data = %d\n", __func__, raw_data);
    }

    if ((raw_data > STK3321_SMUDGE_DIFF) &&
        (mTask.ps_smudge_update == false) &&
        (mTask.psi != 0xFFFF))
    {
        mTask.ps_threshold_high = mTask.psi + STK3321_SMUDGE_NT;
        mTask.ps_threshold_low  = mTask.psi + STK3321_SMUDGE_FT;
        mTask.ps_thred_need_to_update = true;
        mTask.ps_smudge_update = true;
        //osLog(LOG_ERROR, "%s:: near = 0x%X, far = 0x%X\n",
        //      __func__,
        //      mTask.ps_threshold_high,
        //      mTask.ps_threshold_low);
    }
}

static int stk3321_ps_tracking_max_min(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                       void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                       void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint16_t diff = 0;

    if ((!mTask.ps_is_turn_on) || (!mTask.ps_valid))
    {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
        return -1;
    }
    else if (mTask.psi_set != 0xFFFF)
    {
        goto max_min_update;
    }

    if (mTask.prox_raw_data > mTask.psa)
    {
        mTask.psa = mTask.prox_raw_data;
    }

    if (mTask.prox_raw_data < mTask.psi)
    {
        mTask.psi = mTask.prox_raw_data;
    }

    diff = mTask.psa - mTask.psi;
    //osLog(LOG_ERROR, "%s(%d): diff:%d\n",__func__, __LINE__, diff);

    if (diff > STK3321_MAX_MIN_DIFF)
    {
        osLog(LOG_ERROR, "%s: max:%d min:%d diff:%d\n",
              __func__, mTask.psa, mTask.psi, diff);
        mTask.psi_set = mTask.psi;
        mTask.ps_threshold_high = mTask.psi + STK3321_HT_N_CT;
        mTask.ps_threshold_low  = mTask.psi + STK3321_LT_N_CT;
        mTask.ps_thred_need_to_update = true;
    }

max_min_update:

    if (mTask.ps_thred_need_to_update)
    {
        mTask.ps_thred_need_to_update = false;
        mTask.txBuf[0] = STK_THDH1_PS_REG;
        mTask.txBuf[1] = (uint8_t)((mTask.ps_threshold_high & 0xFF00) >> 8);
        mTask.txBuf[2] = (uint8_t)(mTask.ps_threshold_high & 0x00FF);
        mTask.txBuf[3] = (uint8_t)((mTask.ps_threshold_low & 0xFF00) >> 8);
        mTask.txBuf[4] = (uint8_t)(mTask.ps_threshold_low & 0x00FF);
        osLog(LOG_ERROR, "%s: update thd h:%d l:%d\n",
              __func__, mTask.ps_threshold_high, mTask.ps_threshold_low);
        return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 5,
                           i2cCallBack, next_state);
    }
    else
    {
        //osLog(LOG_ERROR, "%s\n", __func__);
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
        return 0;
    }
}

static int stk3321_ps_tracking_recali(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                      void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                      void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint16_t ct_value;

    if ((!mTask.ps_is_turn_on) || (!mTask.ps_valid))
    {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
        return -1;
    }
    //osLog(LOG_INFO, "%s:: flag = 0x%x\n", __func__, mTask.rxBuf[0]);
    if ((mTask.rxBuf[0] >> 6) & 0x1)
    {
        mTask.prox_raw_data = ((mTask.rxBuf[1] << 8) | mTask.rxBuf[2]);
        osLog(LOG_INFO, "%s(%d):: prox_raw_data = %d\n", __func__, __LINE__, mTask.prox_raw_data);
    }

    if (mTask.psi_set == 0xFFFF)
    {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
        return 0;
    }

    if (mTask.last_prox_nearby == 1)
    {
        mTask.ps_tracking_data[1] += mTask.prox_raw_data;

        if (mTask.prox_raw_data > mTask.ps_tracking_data[0])
        {
            mTask.ps_tracking_data[0] = mTask.prox_raw_data;
        }

        if (mTask.prox_raw_data < mTask.ps_tracking_data[2])
        {
            mTask.ps_tracking_data[2] = mTask.prox_raw_data;
        }

        mTask.ps_tracking_data_count++;
        //osLog(LOG_INFO, "%s: count = %d, word_data = %d\n",
        //      __func__,
        //      mTask.ps_tracking_data_count,
        //      mTask.prox_raw_data);

        if (mTask.ps_tracking_data_count == STK3321_TRACKING_QUANTI)
        {
            mTask.ps_tracking_data[1] /= mTask.ps_tracking_data_count;
            ct_value = mTask.ps_threshold_high - STK3321_HT_N_CT;
            //osLog(LOG_INFO, "%s: ct_value = %d, word_data = %d\n",
            //      __func__,
            //      ct_value,
            //      mTask.prox_raw_data);

            if ((mTask.ps_tracking_data[1] < ct_value) &&
                ((ct_value - mTask.ps_tracking_data[1]) >= 5) &&
                ((mTask.ps_tracking_data[0] - mTask.ps_tracking_data[2]) <= STK3321_QUANTI_RANGE))
            {
                //    osLog(LOG_INFO, "%s: ps variation = %d\n",
                //          __func__,
                //          (mTask.ps_tracking_data[0] - mTask.ps_tracking_data[2]));
                mTask.ps_threshold_high = mTask.ps_tracking_data[1] + STK3321_HT_N_CT;
                mTask.ps_threshold_low  = mTask.ps_tracking_data[1] + STK3321_LT_N_CT;
                mTask.psi = mTask.ps_tracking_data[1];
                mTask.ps_thred_need_to_update = true;
                mTask.ps_smudge_update = false;
            }

            mTask.ps_tracking_data[0] = 0;
            mTask.ps_tracking_data[1] = 0;
            mTask.ps_tracking_data[2] = 9999;
            mTask.ps_tracking_data_count = 0;
        }
    }
    else
    {
        mTask.ps_tracking_data[0] = 0;
        mTask.ps_tracking_data[1] = 0;
        mTask.ps_tracking_data[2] = 9999;
        mTask.ps_tracking_data_count = 0;
        stk3321_ps_smudge_judgement();
    }
    osLog(LOG_ERROR, "%s\n", __func__);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int stk3321_ps_val_valid(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint32_t word_data, limit;
    uint8_t  mode;
    mTask.ps_valid = false;

    if (!mTask.ps_is_turn_on)
    {
        osLog(LOG_ERROR, "%s(%d)\n", __func__, __LINE__);
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
        return 0;
    }

    word_data = ((mTask.rxBuf[0] << 8) | mTask.rxBuf[1]);
    word_data += ((mTask.rxBuf[2] << 8) | mTask.rxBuf[3]);
    mode = mTask.ps_ctrl_val & 0x3F;

    switch (mode)
    {
        case 0x30:
            limit = 100;
            break;

        case 0x31:
            limit = 200;
            break;

        case 0x32:
            limit = 400;
            break;

        case 0x33:
            limit = 800;
            break;

        default:
            osLog(LOG_ERROR, "%s: Unsupported PS_IT(0x%x)\n", __func__, mode);
            sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
            return 0;
    }

    if (word_data > limit)
    {
        osLog(LOG_ERROR, "%s(%d): Out of limit\n", __func__, __LINE__);
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
        return 0;
    }
    //osLog(LOG_ERROR, "%s\n", __func__);
    mTask.ps_valid = true;
    mTask.txBuf[0] = STK_FLAG_REG;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 3, i2cCallBack,
                         next_state);
}

static int stk3321_ps_tracking_task(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                    void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                    void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    if (!mTask.psPowerOn)
    {
        osLog(LOG_ERROR, "%s: PS is not ready\n", __func__);
        mTask.ps_is_turn_on = false;
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
        return 0;
    }
    else
    {
        //osLog(LOG_ERROR, "%s\n", __func__);
        mTask.ps_is_turn_on = true;
        mTask.txBuf[0] = 0x20;
        return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                             mTask.rxBuf, 4, i2cCallBack,
                             next_state);
    }

    return 0;
}
#endif

#ifdef STK3321_ALS_ENABLE
/* ****************************************************
 * ALS function
 * ****************************************************/
#ifdef ALS_MID_FIR
static void stk3321_als_bubble_sort(uint16_t* sort_array, uint8_t size_n)
{
    int i, j, tmp;

    for (i = 1; i < size_n; i++)
    {
        tmp = sort_array[i];
        j = i - 1;

        while (j >= 0 && sort_array[j] > tmp)
        {
            sort_array[j + 1] = sort_array[j];
            j = j - 1;
        }

        sort_array[j + 1] = tmp;
    }
}

static void stk3321_als_recal_filter(uint16_t* channel_data)
{
    if (mTask.als_data_filter.number < STK_ALS_MID_FIR_LEN)
    {
        mTask.als_data_filter.raw[mTask.als_data_filter.number] = *channel_data;
        mTask.als_data_filter.number++;
        mTask.als_data_filter.index++;
    }
    else
    {
        uint8_t index;
        uint16_t mid_als;
        uint16_t cpraw[STK_ALS_MID_FIR_LEN] = {0};
        index = mTask.als_data_filter.index % mTask.als_data_filter.number;
        mTask.als_data_filter.raw[index] = *channel_data;
        mTask.als_data_filter.index++;
        memcpy(cpraw, mTask.als_data_filter.raw, sizeof(cpraw));
        stk3321_als_bubble_sort(cpraw, sizeof(cpraw) / sizeof(cpraw[0]));
        mid_als = cpraw[STK_ALS_MID_FIR_LEN / 2];
        *channel_data = mid_als;
    }
}
#endif

static int stk3321_set_als_power_on(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                    void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                    void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.alsPowerOn = true;
    mTask.txBuf[0] = STK_STATE_REG;
    mTask.txBuf[1] = mTask.rxBuf[0] | STK_STATE_EN_ALS_MASK;
    mTask.txBuf[1] &= (~STK_STATE_EN_WAIT_MASK);

    osLog(LOG_INFO, "%s: mTask.txBuf[1] = 0x%x\n", __func__, mTask.txBuf[1]);
#ifdef STK_IRS
    mTask.irs_running = false;
    mTask.als_data_index = 0;
    mTask.ir_code = 0;
#endif
    mTask.state_val = mTask.txBuf[1];
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                       i2cCallBack, next_state);
}

static int stk3321_set_als_power_off(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                     void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                     void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.alsPowerOn = false;
    mTask.als_debounce_on = 0;
    mTask.als_pw_ctl = false;
    mTask.clr_flag = 0;
    mTask.txBuf[0] = STK_STATE_REG;
    mTask.txBuf[1] = mTask.als_pw_state;
    mTask.state_val = mTask.txBuf[1];
    osLog(LOG_INFO, "%s: mTask.txBuf[1] = 0x%x\n", __func__, mTask.txBuf[1]);
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                       i2cCallBack, next_state);
}

static int stk3321_set_als_disable(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                   void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                   void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.als_pw_state = 0;
    mTask.als_pw_ctl = true;
    mTask.als_pw_state = mTask.rxBuf[0] & (~(STK_STATE_EN_WAIT_MASK | STK_STATE_EN_ALS_MASK | STK_STATE_EN_IRS_MASK));

    if (mTask.psPowerOn)
        mTask.als_pw_state |= STK_STATE_EN_WAIT_MASK;

    mTask.clr_flag = (STK_FLG_ALSINT_MASK | STK_FLG_IR_RDY_MASK);
    mTask.txBuf[0] = STK_DATA1_ALS_REG;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 2, i2cCallBack,
                         next_state);
}


static int stk3321_set_als_debounce_on(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                       void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                       void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.als_debounce_on = 1;
    cal_end_time(mTask.als_debounce_time, &mTask.als_debounce_end);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static void alsGetData(void *sample)
{
    struct SingleAxisDataPoint *singleSample = (struct SingleAxisDataPoint *)sample;
    singleSample->idata = mTask.data.als_data;
}

static int stk3321_read_als_data(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                 void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                 void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
#if 0
    if (rxTransferDataInfo(&mTask.dataInfo, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize))
    {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "als rx error\n");
        return -1;
    }
#endif
	//osLog(LOG_INFO, "%s(%d): flag = 0x%x\n", __func__, __LINE__, mTask.rxBuf[0]);
    mTask.txBuf[0] = STK_DATA1_ALS_REG;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 2, i2cCallBack,
                         next_state);
}

#ifdef STK_IRS
static int stk3321_clr_intr(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.txBuf[0] = STK_FLAG_REG;
    mTask.txBuf[1] = mTask.rxBuf[0] & (STK_FLG_ALSINT_MASK | STK_FLG_PSINT_MASK
                                       | STK_FLG_OUI_MASK | STK_FLG_IR_RDY_MASK);
    mTask.txBuf[1] &= (~mTask.clr_flag);
    osLog(LOG_ERROR, "%s(%d): mTask.txBuf[1] = 0x%x\n", __func__, __LINE__, mTask.txBuf[1]);
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                       i2cCallBack, next_state);
}

static int stk3321_set_irs_it(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
	void *inBuf, uint8_t inSize, uint8_t elemInSize,
	void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	//Als raw data
	mTask.als_raw_data = ((mTask.rxBuf[0] << 8) | mTask.rxBuf[1]);
	osLog(LOG_INFO, "%s: als_raw_data = %lu\n", __func__, mTask.als_raw_data);

#ifdef STK_IRS
    uint8_t irs_alsctrl;

    if (mTask.als_data_index < 60000)
        mTask.als_data_index++;
    else
        mTask.als_data_index = 0;

    //osLog(LOG_INFO, "%s: als_data_index = %d\n", __func__, mTask.als_data_index);

    if (mTask.als_data_index % 10 != 1)
    {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
        return 0;
    }

    mTask.irs_running = true;

    //set IT 
    irs_alsctrl = (mTask.als_ctrl_val & 0x0F) - 2;
    switch (irs_alsctrl)
    {
        case 3:
            mTask.irs_slp_time = 1;
            break;

        case 4:
            mTask.irs_slp_time = 3;
            break;

        case 5:
            mTask.irs_slp_time = 6;
            break;

        case 6:
            mTask.irs_slp_time = 12;
            break;

        case 7:
            mTask.irs_slp_time = 24;
            break;

        case 8:
            mTask.irs_slp_time = 48;
            break;

        case 9:
            mTask.irs_slp_time = 96;
            break;

        default:
            mTask.irs_slp_time = 12;
            osLog(LOG_INFO, "%s: unknown ALS IT=0x%x\n", __func__, irs_alsctrl);
    }

    irs_alsctrl |= (mTask.als_ctrl_val & 0xF0);
    mTask.txBuf[0] = STK_ALSCTRL_REG;
    mTask.txBuf[1] = irs_alsctrl;
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
        i2cCallBack, next_state);
#else
    mTask.irs_running = false;
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
#endif // STK_IRS
	return 0;
}

static int stk3321_set_irs_enable(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
	void *inBuf, uint8_t inSize, uint8_t elemInSize,
	void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	if (mTask.irs_running != true)
	{
		sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
		return 0;
	}

#ifdef STK_IRS
    uint8_t	i2c_config_reg = 0;
	//Enable IRS_STATE
	i2c_config_reg = mTask.state_val | STK_STATE_EN_IRS_MASK;
	mTask.state_val = i2c_config_reg;
	mTask.txBuf[0] = STK_STATE_REG;
	mTask.txBuf[1] = i2c_config_reg;
	return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
		i2cCallBack, next_state);

#else
	sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
	return 0;
#endif
}

static int stk3321_get_ir_flag(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
	void *inBuf, uint8_t inSize, uint8_t elemInSize,
	void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	if (mTask.irs_running != true)
	{
		sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
		return 0;
	}
    osLog(LOG_INFO, "%s\n", __func__);
#ifdef STK_IRS
	//wait IRS IT
	vTaskDelay(mTask.irs_slp_time + 1);

	mTask.txBuf[0] = STK_FLAG_REG;
	return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
		mTask.rxBuf, 1, i2cCallBack,
		next_state);
#endif // STK_IRS

	sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
	return 0;
}

static int stk3321_check_irs_ready(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
	void *inBuf, uint8_t inSize, uint8_t elemInSize,
	void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	if (mTask.irs_running != true)
	{
		sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
		return 0;
	}
    //osLog(LOG_INFO, "%s\n", __func__);
#ifdef STK_IRS
	if (mTask.rxBuf[0] & STK_FLG_IR_RDY_MASK)
	{
		//IRS is complete
		osLog(LOG_INFO, "%s: IRS is complete\n", __func__);
	}
	else
	{		
		osLog(LOG_INFO, "%s: IRS is not end!\n", __func__);
		mTask.irs_running = false;

		//Reset IT
		mTask.txBuf[0] = STK_ALSCTRL_REG;
		mTask.txBuf[1] = mTask.als_ctrl_val;
		return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
			i2cCallBack, next_state);
	}

	mTask.txBuf[0] = STK_FLAG_REG;
	mTask.txBuf[1] = mTask.rxBuf[0] & ~STK_FLG_IR_RDY_MASK;
	return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
		i2cCallBack, next_state);
#endif // STK_IRS

	sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
	return 0;
}

static int stk3321_get_ir_start(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
	void *inBuf, uint8_t inSize, uint8_t elemInSize,
	void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	if (mTask.irs_running != true)
	{
		sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
		return 0;
	}
	//osLog(LOG_INFO, "%s\n", __func__);
#ifdef STK_IRS
	mTask.txBuf[0] = STK_DATA1_IR;
	return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
		mTask.rxBuf, 2, i2cCallBack,
		next_state);
#endif // STK_IRS

	sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
	return 0;
}

static int stk3321_get_ir_end(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
	void *inBuf, uint8_t inSize, uint8_t elemInSize,
	void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	if (mTask.irs_running != true)
	{
		sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
		return 0;
	}

#ifdef STK_IRS
	mTask.als_drup_data = true;
	mTask.irs_running = false;

	mTask.ir_code = mTask.rxBuf[0] << 8 | mTask.rxBuf[1];
	mTask.last_ir_code = mTask.rxBuf[0] << 8 | mTask.rxBuf[1];

	//Reset IT
	mTask.txBuf[0] = STK_ALSCTRL_REG;
	mTask.txBuf[1] = mTask.als_ctrl_val;
	osLog(LOG_INFO, "%s: ir=%d, als_ctrl_val = 0x%x\n", __func__, mTask.ir_code, mTask.als_ctrl_val);
	return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
		i2cCallBack, next_state);
#endif // STK_IRS

	sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
	return 0;
}

static int stk3321_drup_data_1(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
	void *inBuf, uint8_t inSize, uint8_t elemInSize,
	void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) 
{
#ifdef STK_IRS
	if (mTask.als_drup_data)
	{
		mTask.txBuf[0] = STK_STATE_REG;
		return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
			mTask.rxBuf, 1, i2cCallBack,
			next_state);
	}
#endif // STK_IRS
	sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
	return 0;
}

static int stk3321_drup_data_2(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
	void *inBuf, uint8_t inSize, uint8_t elemInSize,
	void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) 
{

#ifdef STK_IRS
	if (mTask.als_drup_data)
	{
	    mTask.txBuf[0] = STK_STATE_REG;
		mTask.txBuf[1] = mTask.rxBuf[0];
		return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
			i2cCallBack, next_state);		
	}
#endif // STK_IRS
	sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
	return 0;
}

static int stk3321_drup_data_3(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
	void *inBuf, uint8_t inSize, uint8_t elemInSize,
	void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) 
{
#ifdef STK_IRS
	if (mTask.als_drup_data)
	{
		mTask.als_drup_data = false;

		mTask.txBuf[0] = STK_DATA1_ALS_REG;
		return	i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
			mTask.rxBuf, 2, i2cCallBack,
			next_state);		
	}
#endif // STK_IRS
	sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
	return 0;
}

static int stk3321_als_data_correct_factor(uint16_t raw_data)
{
	uint32_t raw_data_u32 = 0;

	raw_data_u32 = raw_data;

#ifdef STK_IRS
	if (mTask.ir_code != 0)
	{
		mTask.als_correct_factor = 4000; //CWF(cool white)
		if (raw_data < STK_IRC_MAX_ALS_CODE && raw_data > STK_IRC_MIN_ALS_CODE &&
			mTask.ir_code > STK_IRC_MIN_IR_CODE)
		{
                        if (mTask.ir_code > als_comperator_HZ * raw_data)
				mTask.als_correct_factor = 2300;
			else if (mTask.ir_code > als_comperator_A * raw_data)
				mTask.als_correct_factor = 2700;
			else if (mTask.ir_code > als_comperator_D * raw_data)
				mTask.als_correct_factor = 2900;
			else//CWF
			{				
				mTask.als_correct_factor = 4000;
			}
		}

		osLog(LOG_ERROR, "%s: als=%d, ir=%d, als_correct_factor=%d \n",
			__func__, raw_data, mTask.ir_code, mTask.als_correct_factor);

		mTask.ir_code = 0;
	}

	raw_data_u32 = raw_data_u32 * mTask.als_correct_factor / 1000;
#endif // STK_IRS

	raw_data = (uint16_t)raw_data_u32;
	return raw_data;
}
#endif

static int stk3321_read_als_status(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                  void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                  void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    if (rxTransferDataInfo(&mTask.dataInfo, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize))
    {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "stk3321 als, rx dataInfo error\n");
        return -1;
    }

    mTask.txBuf[0] = STK_FLAG_REG;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 1, i2cCallBack,
                         next_state);
}

static int stk3321_get_als_value(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                 void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                 void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
#ifdef STK_IRS
    uint16_t als_raw_data = (uint16_t)stk3321_als_data_correct_factor(mTask.als_raw_data);
#else
    uint16_t als_raw_data = ((mTask.rxBuf[0] << 8) | mTask.rxBuf[1]);
#endif
    osLog(LOG_INFO, "%s: ALS raw data = %d\n", __func__, als_raw_data);
#ifdef ALS_MID_FIR
    stk3321_als_recal_filter(&als_raw_data);
    osLog(LOG_INFO, "%s: ALS raw data(FIR) = %d\n", __func__, als_raw_data);
#endif

    mTask.data.als_data = (uint32_t)als_raw_data;
    mTask.data.als_data = mTask.data.als_data * mTask.alsCali / 1000;
	osLog(LOG_INFO, "%s: LUX = %d\n", __func__, mTask.data.als_data);//gg
	mTask.data.sensType = SENS_TYPE_ALS;//gg
    txTransferDataInfo(&mTask.dataInfo, 1, &mTask.data);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int stk3321_reset_als_it(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
	void *inBuf, uint8_t inSize, uint8_t elemInSize,
	void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
	//Reset IT
	mTask.txBuf[0] = STK_ALSCTRL_REG;
	mTask.txBuf[1] = mTask.als_ctrl_val;
	osLog(LOG_INFO, "%s: mTask.txBuf[1] = 0x%x\n", __func__, mTask.txBuf[1]);
	return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
		i2cCallBack, next_state);
}

static int stk3321_als_cali(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int32_t alsCali[2];
    float   alsCali_mi;
    float als_raw_data = (float)stk3321_als_data_correct_factor(mTask.als_raw_data);
    alsCali_mi = 500.0 / als_raw_data;
    alsCali[0] = alsCali_mi * ALS_INCREASE_NUM_AP;
    alsCali[1] = 0;
//Maple 437,xuyanan.wt,modify,20211027,als cali
    mTask.alsCali = alsCali[0];
    alsPsSendCalibrationResult(SENS_TYPE_ALS, alsCali);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int stk3321_als_cfg(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;
    struct alspsCaliCfgPacket caliCfgPacket;
    double  alsCali_mi;
    ret = rxCaliCfgInfo(&caliCfgPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);

    if (ret < 0)
    {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "%s: rx inSize and elemSize error\n", __func__);
        return -1;
    }

    alsCali_mi = (double)((float)caliCfgPacket.caliCfgData[0] / ALS_INCREASE_NUM_AP);
    osLog(LOG_INFO, "%s: [%f]\n", __func__, alsCali_mi);

    if (caliCfgPacket.caliCfgData[0] != 0)
        mTask.alsCali = caliCfgPacket.caliCfgData[0];

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int stk3321_read_alsps_flag(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                   void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                   void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    if (mTask.als_pw_ctl)
    {
        mTask.clr_flag = (STK_FLG_ALSINT_MASK | STK_FLG_IR_RDY_MASK);
        osLog(LOG_INFO, "%s: mTask.clr_flag = 0x%x\n", __func__, mTask.clr_flag);
    }
    else
    {
        mTask.clr_flag = STK_FLG_IR_RDY_MASK;
    }

    mTask.txBuf[0] = STK_FLAG_REG;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 1, i2cCallBack,
                         next_state);
}


static int stk3321_als_ratechg(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                               void *inBuf, uint8_t inSize, uint8_t elemInSize,
                               void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}
//-Maple 437,xuyanan.wt,modify,20211030,modify hardinfo name
static void alsGetSensorInfo(struct sensorInfo_t *data)
{
    strncpy(data->name, "stk3329_l", sizeof(data->name));
}
#endif
//-Maple 437,xuyanan.wt,modify,20211030,modify hardinfo name
#ifdef STK3321_PS_ENABLE
/* ****************************************************
 * PS function
 * ****************************************************/
static int stk3321_mask_eint(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                             void *inBuf, uint8_t inSize, uint8_t elemInSize,

                             void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mt_eint_mask(mTask.hw->eint_num);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int stk3321_unmask_eint(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                               void *inBuf, uint8_t inSize, uint8_t elemInSize,
                               void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mt_eint_unmask(mTask.hw->eint_num);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int stk3321_set_ps_power_on(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                   void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                   void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.psPowerOn = true;
    mTask.txBuf[0] = STK_STATE_REG;
    mTask.txBuf[1] = mTask.rxBuf[0] | STK_STATE_EN_PS_MASK;

    if (!mTask.alsPowerOn)
    {
        mTask.txBuf[1] |= STK_STATE_EN_WAIT_MASK;
    }
    mTask.state_val = mTask.txBuf[1];
    osLog(LOG_INFO, "%s(%d): mTask.txBuf[1] = 0x%x\n", __func__, __LINE__, mTask.txBuf[1]);
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                       i2cCallBack, next_state);
}

static int stk3321_set_ps_power_off(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                    void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                    void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.psPowerOn = false;
    mTask.ps_debounce_on = 0;
    mTask.data.prox_state = PROX_STATE_FAR;
    mTask.txBuf[0] = STK_STATE_REG;
    mTask.txBuf[1] = mTask.rxBuf[0] & (~STK_STATE_EN_PS_MASK);
    mTask.txBuf[1] &= (~STK_STATE_EN_WAIT_MASK);
    mTask.state_val = mTask.txBuf[1];
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                       i2cCallBack, next_state);
}

static int stk3321_set_ps_debounce_on(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                      void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                      void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.ps_debounce_on = 1;
    cal_end_time(mTask.ps_debounce_time, &mTask.ps_debounce_end);
    mTask.data.prox_state = PROX_STATE_INIT;
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static void psGetData(void *sample)
{
    char txBuf[1];
    static char rxBuf[2];
    struct TripleAxisDataPoint *tripleSample = (struct TripleAxisDataPoint *)sample;
    tripleSample->ix = mTask.data.prox_data;
    tripleSample->iy = (mTask.data.prox_state == PROX_STATE_NEAR ? 0 : 1);
    void get_ps_data(void *cookie, size_t tx, size_t rx, int err)
    {
        char *rxBuf = cookie;

        if (err == 0)
        {
            mTask.data.prox_data = (rxBuf[0] << 8) | rxBuf[1];
            osLog(LOG_ERROR, "%s: ps = %d, cali = %d\n",
                  __func__,
                  mTask.data.prox_data,
                  mTask.psCali);

            if (mTask.data.prox_data < mTask.psCali)
                mTask.data.prox_data = 0;

            mTask.data.prox_data -= mTask.psCali;
        }
        else
            osLog(LOG_INFO, "stk3321: read ps data i2c error (%d)\n", err);
    }
    txBuf[0] = STK_DATA1_PS_REG;
    i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, txBuf, 1,
                  rxBuf, 2, get_ps_data, rxBuf);
}

static int stk3321_read_ps_status(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                  void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                  void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    if (rxTransferDataInfo(&mTask.dataInfo, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize))
    {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "stk3321 ps, rx dataInfo error\n");
        return -1;
    }

    mTask.txBuf[0] = STK_FLAG_REG;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 3, i2cCallBack,
                         next_state);
}

static int stk3321_get_ps_raw_data(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                   void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                   void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ps_flag = mTask.rxBuf[0] & 0x01;
    int is_ready = (mTask.rxBuf[0] >> 6) & 0x1;
    osLog(LOG_ERROR, "%s: flag_reg: 0x%x\n", __func__, mTask.rxBuf[0]);

    if (ps_flag == 1)
    {
        mTask.last_prox_nearby = PROX_STATE_FAR;
        mTask.data.prox_state = PROX_STATE_FAR; /* far */
    }
    else if (ps_flag == 0)
    {
        mTask.last_prox_nearby = PROX_STATE_NEAR;
        mTask.data.prox_state = PROX_STATE_NEAR; /* near */
    }
    else
    {
        osLog(LOG_ERROR, "%s: error status: %d\n", __func__, mTask.rxBuf[0]);
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
        return -1;
    }

    if (is_ready) //Because the PS tracking timer will get the ps data first
    {
        mTask.prox_raw_data = (mTask.rxBuf[1] << 8) | mTask.rxBuf[2];
    }

    mTask.data.prox_data = mTask.prox_raw_data;

    //Here is MTK plan, should be confirmed
    if (mTask.data.prox_data < mTask.psCali)
    {
        mTask.data.prox_data = 0;
    }

    //mTask.data.prox_data -= mTask.psCali;
    osLog(LOG_INFO, "%s: NF: %d, ps_data: %ld, ps_threshold_high: %d, ps_threshold_low: %d\n",
          __func__,
          mTask.data.prox_state,
          mTask.data.prox_data,
          mTask.ps_threshold_high,
          mTask.ps_threshold_low);
    mTask.txBuf[0] = STK_FLAG_REG;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 1, i2cCallBack,
                         next_state);
}

#ifdef STK3321_PS_OC_REPORT
static int stk3321_get_ps_raw_data_once(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                        void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                        void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ps_flag = mTask.rxBuf[0] & 0x01;
    int is_ready = (mTask.rxBuf[0] >> 6) & 0x1;
    osLog(LOG_ERROR, "%s: status: 0x%x\n", __func__, mTask.rxBuf[0]);
    if (ps_flag == 1)
    {
        mTask.last_prox_nearby = PROX_STATE_FAR;
        mTask.data.prox_state = PROX_STATE_FAR; /* far */
    }
    else if (ps_flag == 0)
    {
        mTask.last_prox_nearby = PROX_STATE_NEAR;
        mTask.data.prox_state = PROX_STATE_NEAR; /* near */
    }

    if (is_ready) //Because the PS tracking timer will get the ps data first
    {
        mTask.prox_raw_data = (mTask.rxBuf[1] << 8) | mTask.rxBuf[2];
    }

    mTask.data.prox_data = mTask.prox_raw_data;

    //Here is MTK plan, should be confirmed
    if (mTask.data.prox_data < mTask.psCali)
    {
        mTask.data.prox_data = 0;
    }

    //mTask.data.prox_data -= mTask.psCali;
    osLog(LOG_INFO, "%s: NF: %d, ps_data: %ld, ps_threshold_high: %d, ps_threshold_low: %d\n",
          __func__,
          mTask.data.prox_state,
          mTask.data.prox_data,
          mTask.ps_threshold_high,
          mTask.ps_threshold_low);

    txTransferDataInfo(&mTask.dataInfo, 1, &mTask.data);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);    
    return 0;
}
#endif

static int stk3321_ps_ratechg(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                              void *inBuf, uint8_t inSize, uint8_t elemInSize,
                              void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int stk3321_ps_cali(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int32_t threshold[2];
    threshold[0] = 2;  // high simulation data
    threshold[1] = 3;  // low simulation data
    alsPsSendCalibrationResult(SENS_TYPE_PROX, threshold);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static void stk3321_ps_set_thd(uint32_t threshold_high, uint32_t threshold_low)
{
    int ret;
    mTask.txBuf[0] = STK_THDH1_PS_REG;
    mTask.txBuf[1] = (uint8_t)((threshold_high & 0xFF00) >> 8);
    mTask.txBuf[2] = (uint8_t)(threshold_high & 0x00FF);
    mTask.txBuf[3] = (uint8_t)((threshold_low & 0xFF00) >> 8);
    mTask.txBuf[4] = (uint8_t)(threshold_low & 0x00FF);
    osLog(LOG_INFO, "%s: h:%d l:%d\n", __func__, threshold_high, threshold_low);
    ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 5,
                      NULL, 0);

    if (ret)
    {
        osLog(LOG_INFO, "%s: set ps threshold i2c error (%d)\n", __func__, ret);
        return;
    }
}

static int stk3321_ps_cfg(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                          void *inBuf, uint8_t inSize, uint8_t elemInSize,
                          void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;
    struct alspsCaliCfgPacket caliCfgPacket;
    ret = rxCaliCfgInfo(&caliCfgPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);

    if (ret < 0)
    {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "%s: rx inSize and elemSize error\n", __func__);
        return -1;
    }

    osLog(LOG_INFO, "%s: [high, low]: [%d, %d]\n",
          __func__,
          caliCfgPacket.caliCfgData[0],
          caliCfgPacket.caliCfgData[1]);

    if (caliCfgPacket.caliCfgData[0] != 0 && caliCfgPacket.caliCfgData[1] != 0 && caliCfgPacket.caliCfgData[0] > caliCfgPacket.caliCfgData[1])
    {
        stk3321_ps_set_thd(caliCfgPacket.caliCfgData[0], caliCfgPacket.caliCfgData[1]);
        ps_cali_flag = 1;
    }
    else
    {
        caliCfgPacket.caliCfgData[0] = mTask.ps_threshold_high;
        caliCfgPacket.caliCfgData[1] = mTask.ps_threshold_low;
        stk3321_ps_set_thd(caliCfgPacket.caliCfgData[0], caliCfgPacket.caliCfgData[1]);
    }

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int stk3321_clr_int(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.txBuf[0] = STK_FLAG_REG;
    mTask.txBuf[1] = mTask.rxBuf[0] & ~STK_FLG_PSINT_MASK;
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                       i2cCallBack, next_state);
}

static int stk3321_enable_eint(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                               void *inBuf, uint8_t inSize, uint8_t elemInSize,
                               void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mt_eint_unmask(mTask.hw->eint_num);
    txTransferDataInfo(&mTask.dataInfo, 1, &mTask.data);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static void psGetCalibration(int32_t *cali, int32_t size)
{
    cali[0] = mTask.psCali;
}

static void psSetCalibration(int32_t *cali, int32_t size)
{
    mTask.psCali = cali[0];
    stk3321_ps_set_thd(mTask.ps_threshold_high + mTask.psCali,
                       mTask.ps_threshold_low + mTask.psCali);
}

static void psGetThreshold(uint32_t *threshold_high, uint32_t *threshold_low)
{
    *threshold_high = mTask.ps_threshold_high;
    *threshold_low = mTask.ps_threshold_low;
    osLog(LOG_ERROR, "%s ==>threshold_high:%ld threshold_low:%ld \n", __func__, *threshold_high, *threshold_low);
}

static void psSetThreshold(uint32_t threshold_high, uint32_t threshold_low)
{
    osLog(LOG_ERROR, "%s ==>threshold_high:%ld threshold_low:%ld \n", __func__, threshold_high, threshold_low);
    mTask.ps_threshold_high = threshold_high;
    mTask.ps_threshold_low = threshold_low;
    stk3321_ps_set_thd(mTask.ps_threshold_high, mTask.ps_threshold_low);
}

static void psGetSensorInfo(struct sensorInfo_t *data)
{
    strncpy(data->name, "stk3321_p", sizeof(data->name));
}
#endif

static int stk3321_set_sw_reset(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    osLog(LOG_ERROR, "%s: start \n", __func__);
    mTask.txBuf[0] = STK_SW_RESET_REG;
    mTask.txBuf[1] = 0;
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                       i2cCallBack, next_state);
}

static int stk3321_set_alsps_conf(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                  void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                  void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.txBuf[0] = STK_STATE_REG;
    mTask.txBuf[1] = mTask.state_val;
    mTask.txBuf[2] = mTask.ps_ctrl_val;
    mTask.txBuf[3] = mTask.als_ctrl_val;
    mTask.txBuf[4] = mTask.ledctrl_val;
    mTask.txBuf[5] = 0;
    osLog(LOG_INFO, "%s: mTask.als_ctrl_val = 0x%x\n", __func__, mTask.als_ctrl_val);

    if (0 == mTask.hw->polling_mode_als)
    {
        mTask.txBuf[5] |= 1 << STK_INT_ALS_SHIFT;
    }

    if (0 == mTask.hw->polling_mode_ps)
    {
        mTask.txBuf[5] |= 1 << STK_INT_PS_SHIFT;
    }

    mTask.txBuf[6] = STK_WAIT_50;
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 7,
                       i2cCallBack, next_state);
}

static int stk3321_set_default_ps_thd(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                      void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                      void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.txBuf[0] = STK_THDH1_PS_REG;
    mTask.txBuf[1] = (uint8_t)((mTask.ps_threshold_high & 0xFF00) >> 8);
    mTask.txBuf[2] = (uint8_t)(mTask.ps_threshold_high & 0x00FF);
    mTask.txBuf[3] = (uint8_t)((mTask.ps_threshold_low & 0xFF00) >> 8);
    mTask.txBuf[4] = (uint8_t)(mTask.ps_threshold_low & 0x00FF);
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 5,
                       i2cCallBack, next_state);
}

static int stk3321_set_default_als_thd(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                       void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                       void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    if (mTask.hw->polling_mode_als == 0) //INT mode, force trigger the first ps data
    {
        mTask.txBuf[0] = STK_THDH1_ALS_REG;
        mTask.txBuf[1] = 0x00;
        mTask.txBuf[2] = 0x00;
        mTask.txBuf[3] = 0xFF;
        mTask.txBuf[4] = 0xFF;
        return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 5,
                           i2cCallBack, next_state);
    }
    else
    {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    }

    return 0;
}

static void stk3321_eint_handler(int arg)
{
    alsPsInterruptOccur();
}

static int stk3321_setup_eint(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                              void *inBuf, uint8_t inSize, uint8_t elemInSize,
                              void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mt_eint_dis_hw_debounce(mTask.hw->eint_num);
    mt_eint_registration(mTask.hw->eint_num, LEVEL_SENSITIVE, LOW_LEVEL_TRIGGER, stk3321_eint_handler, EINT_INT_UNMASK,
                         EINT_INT_AUTO_UNMASK_OFF);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int stk3321_register_core(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                 void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                 void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    struct sensorCoreInfo mInfo;
#ifdef STK3321_PS_ENABLE
    int ret = 0;
#endif
#ifdef STK3321_ALS_ENABLE
    memset(&mInfo, 0x00, sizeof(struct sensorCoreInfo));
    /* Register sensor Core */
    mInfo.sensType = SENS_TYPE_ALS;
    mInfo.getData = alsGetData;
    mInfo.getSensorInfo = alsGetSensorInfo;
    sensorCoreRegister(&mInfo);
#endif
#ifdef STK3321_PS_ENABLE
    memset(&mInfo, 0x00, sizeof(struct sensorCoreInfo));
    mInfo.sensType = SENS_TYPE_PROX,
          mInfo.gain = 1;
    mInfo.sensitivity = 1;
    sensorDriverGetConvert(0, &mInfo.cvt);

    if (ret)
    {
        osLog(LOG_ERROR, "%s: invalid direction!\n", __func__);
    }

    mInfo.getCalibration = psGetCalibration;
    mInfo.setCalibration = psSetCalibration;
    mInfo.getThreshold = psGetThreshold;
    mInfo.setThreshold = psSetThreshold;
    mInfo.getData = psGetData;
    mInfo.getSensorInfo = psGetSensorInfo;
    sensorCoreRegister(&mInfo);
#endif
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static struct sensorFsm stk3321Fsm[] =
{
#ifdef STK3321_ALS_ENABLE
    /* sample als */
#ifdef STK_IRS
    //sensorFsmCmd(STATE_SAMPLE_ALS, STATE_GET_ALS_DATA, stk3321_read_als_data),
    //sensorFsmCmd(STATE_GET_ALS_DATA, STATE_IRS_SET_IT, stk3321_set_irs_it),
    sensorFsmCmd(STATE_SAMPLE_ALS, STATE_GET_ALS_DR, stk3321_read_als_status),
    sensorFsmCmd(STATE_GET_ALS_DR, STATE_GET_ALS_DATA, stk3321_read_als_data),
	sensorFsmCmd(STATE_GET_ALS_DATA, STATE_IRS_SET_IT, stk3321_set_irs_it),
	sensorFsmCmd(STATE_IRS_SET_IT, STATE_IRS_ENABLE, stk3321_set_irs_enable),
	sensorFsmCmd(STATE_IRS_ENABLE, STATE_IRS_GET_IR_FLAG, stk3321_get_ir_flag),
	sensorFsmCmd(STATE_IRS_GET_IR_FLAG, STATE_IRS_CHECK_IRS_READY, stk3321_check_irs_ready),
	sensorFsmCmd(STATE_IRS_CHECK_IRS_READY, STATE_IRS_GET_IR_START, stk3321_get_ir_start),
	sensorFsmCmd(STATE_IRS_GET_IR_START, STATE_IRS_GET_IR_END, stk3321_get_ir_end),
	sensorFsmCmd(STATE_IRS_GET_IR_END, 	STATE_IRS_DRUP_DATA_1, stk3321_drup_data_1),
	sensorFsmCmd(STATE_IRS_DRUP_DATA_1,	STATE_IRS_DRUP_DATA_2, stk3321_drup_data_2),
	sensorFsmCmd(STATE_IRS_DRUP_DATA_2,	STATE_IRS_DRUP_DATA_3, stk3321_drup_data_3),
	sensorFsmCmd(STATE_IRS_DRUP_DATA_3, STATE_SAMPLE_ALS_DONE, stk3321_get_als_value),
#else
    sensorFsmCmd(STATE_SAMPLE_ALS, STATE_GET_ALS_DATA, stk3321_read_als_data),
    sensorFsmCmd(STATE_GET_ALS_DATA, STATE_SAMPLE_ALS_DONE, stk3321_get_als_value),
#endif
    /* als enable state */
    //sensorFsmCmd(STATE_ALS_ENABLE, STATE_ALS_POWER_ON, stk3321_get_alsps_state),
    sensorFsmCmd(STATE_ALS_ENABLE, STATE_ALS_POWER_RESET_IT, stk3321_reset_als_it),
	sensorFsmCmd(STATE_ALS_POWER_RESET_IT, STATE_ALS_POWER_ON, stk3321_get_alsps_state),
    sensorFsmCmd(STATE_ALS_POWER_ON, STATE_ALS_SET_DEBOUNCE, stk3321_set_als_power_on),
    sensorFsmCmd(STATE_ALS_SET_DEBOUNCE, STATE_ALS_ENABLE_DONE, stk3321_set_als_debounce_on),
    /* als disable state */
    sensorFsmCmd(STATE_ALS_DISABLE, STATE_ALS_SET_DISABLE, stk3321_get_alsps_state),
    sensorFsmCmd(STATE_ALS_SET_DISABLE, STATE_GET_ALS_FLAG, stk3321_set_als_disable),
    sensorFsmCmd(STATE_GET_ALS_FLAG, STATE_CLR_INTR, stk3321_read_alsps_flag),
    sensorFsmCmd(STATE_CLR_INTR, STATE_ALS_POWER_OFF, stk3321_clr_intr),
    sensorFsmCmd(STATE_ALS_POWER_OFF, STATE_ALS_DISABLE_DONE, stk3321_set_als_power_off),
    /* als cali state */
    sensorFsmCmd(STATE_ALS_CALI, CHIP_ALS_CALI_DONE, stk3321_als_cali),
    /* als cfg state */
    sensorFsmCmd(STATE_ALS_CFG, CHIP_ALS_CFG_DONE, stk3321_als_cfg),
    /* als rate change */
    sensorFsmCmd(STATE_ALS_RATECHG,         STATE_ALS_RATECHG_DONE,     stk3321_als_ratechg),
#endif
#ifdef STK3321_PS_ENABLE
    /* sample ps */
    sensorFsmCmd(STATE_SAMPLE_PS,           STATE_GET_PS_RAW_DATA,      stk3321_read_ps_status),
    sensorFsmCmd(STATE_GET_PS_RAW_DATA,     STATE_CLR_INT,              stk3321_get_ps_raw_data),
    sensorFsmCmd(STATE_CLR_INT,             STATE_EN_EINT,              stk3321_clr_int),
    sensorFsmCmd(STATE_EN_EINT,             STATE_SAMPLE_PS_DONE,       stk3321_enable_eint),
#ifdef STK3321_PS_OC_REPORT
    /*ps sample data once, for interrupt mode*/    
    sensorFsmCmd(STATE_SAMPLE_PS_ONCE, STATE_GET_PS_RAW_DATA_ONCE,      stk3321_read_ps_status),    
    sensorFsmCmd(STATE_GET_PS_RAW_DATA_ONCE, STATE_SAMPLE_PS_DONE,      stk3321_get_ps_raw_data_once),
#endif

    /* ps enable state */
    sensorFsmCmd(STATE_PS_ENABLE, STATE_PS_POWER_ON, stk3321_get_alsps_state),
    sensorFsmCmd(STATE_PS_POWER_ON, STATE_PS_SET_DEBOUNCE, stk3321_set_ps_power_on),
    sensorFsmCmd(STATE_PS_SET_DEBOUNCE, STATE_PS_UNMASK_EINT, stk3321_set_ps_debounce_on),
    sensorFsmCmd(STATE_PS_UNMASK_EINT, STATE_PS_ENABLE_DONE, stk3321_unmask_eint),
    /* ps disable state */
    sensorFsmCmd(STATE_PS_DISABLE, STATE_GET_ALSPS_STATE, stk3321_mask_eint),
    sensorFsmCmd(STATE_GET_ALSPS_STATE, STATE_PS_POWER_OFF, stk3321_get_alsps_state),
    sensorFsmCmd(STATE_PS_POWER_OFF, STATE_PS_DISABLE_DONE, stk3321_set_ps_power_off),
    /* ps rate change */
    sensorFsmCmd(STATE_PS_RATECHG, STATE_PS_RATECHG_DONE, stk3321_ps_ratechg),
    /* ps cali state */
    sensorFsmCmd(STATE_PS_CALI, CHIP_PS_CALI_DONE, stk3321_ps_cali),
    /* ps cfg state */
    sensorFsmCmd(STATE_PS_CFG, CHIP_PS_CFG_DONE, stk3321_ps_cfg),
#ifdef STK_TUNE0
    /* CT tracking */
    sensorFsmCmd(STATE_PS_CT_TUNE,          STATE_PS_CT_INVALID_FLAG,   stk3321_ps_tracking_task),
    sensorFsmCmd(STATE_PS_CT_INVALID_FLAG,  STATE_PS_CT_RECALI,         stk3321_ps_val_valid),
    sensorFsmCmd(STATE_PS_CT_RECALI,        STATE_PS_MAX_MIN,           stk3321_ps_tracking_recali),
    sensorFsmCmd(STATE_PS_MAX_MIN,          STATE_PS_CT_TUNE_DONE,      stk3321_ps_tracking_max_min),
#endif
#endif
    /* init state */
    sensorFsmCmd(STATE_RESET,               STATE_SET_ALSPS_CONF,       stk3321_set_sw_reset),
    sensorFsmCmd(STATE_SET_ALSPS_CONF,      STATE_SET_DEFAULT_PS_THD,   stk3321_set_alsps_conf),
    sensorFsmCmd(STATE_SET_DEFAULT_PS_THD,  STATE_SET_DEFAULT_ALS_THD,  stk3321_set_default_ps_thd),
    sensorFsmCmd(STATE_SET_DEFAULT_ALS_THD, STATE_SETUP_EINT,           stk3321_set_default_als_thd),
    sensorFsmCmd(STATE_SETUP_EINT,          STATE_CORE,         stk3321_setup_eint),
    sensorFsmCmd(STATE_CORE, STATE_INIT_DONE, stk3321_register_core),
};

static int stk3321Init(void)
{
    int ret = 0, pid_count = 0;
    osLog(LOG_ERROR, "%s: task starting, version: %s\n", __func__, STK3321_VERSION);
    stk3321DebugPoint = &mTask;
    mTask.hw = get_cust_alsps("stk3321");

    if (NULL == mTask.hw)
    {
        osLog(LOG_ERROR, "get_cust_acc_hw fail\n");
        return -1;
    }

    mTask.i2c_addr = mTask.hw->i2c_addr[0];
    osLog(LOG_ERROR, "%s: i2c_addr = 0x%X\n", __func__, mTask.i2c_addr);
#ifdef STK3321_ALS_ENABLE
    mTask.als_debounce_time = 200;
    mTask.als_debounce_on = 0;
    mTask.alsCali = 1000;
    mTask.als_pw_ctl = false;
#ifdef ALS_FIR
    mTask.als_fir.count = 0;
    mTask.als_fir.idx = 0;
    mTask.fir_len = STK_FIR_LEN;
#endif /* STK_FIR */
#endif
#ifdef STK3321_PS_ENABLE
    mTask.ps_debounce_time = 10;
    mTask.ps_debounce_on = 0;
    mTask.psCali = 0;
#ifdef STK_TUNE0
    mTask.psi_set = 0xFFFF;
    mTask.psi = 0xFFFF; //to avoid Out of range when psensor auto-cali
    mTask.psa = 0x0;
    mTask.ps_tracking_data[0] = 0;
    mTask.ps_tracking_data[1] = 0;
    mTask.ps_tracking_data[2] = 9999;
    mTask.ps_tracking_data_count = 0;
    mTask.ps_smudge_update = false;
    mTask.ps_is_turn_on = false;
    mTask.ps_thred_need_to_update = false;
#endif
#endif
    mTask.als_threshold_high = mTask.hw->als_threshold_high;
    mTask.als_threshold_low = mTask.hw->als_threshold_low;
    mTask.ps_threshold_high = mTask.hw->ps_threshold_high;
    mTask.ps_threshold_low = mTask.hw->ps_threshold_low;
    mTask.als_ctrl_val = (STK_ALS_PRS1 << STK_ALS_PRST_SHIFT) |
                         (STK_ALS_GAIN64 << STK_ALS_GAIN_SHIFT) |
                         (STK_ALS_IT50 << STK_ALS_IT_SHIFT);
    mTask.ps_ctrl_val = (STK_PS_PRS1 << STK_PS_PRST_SHIFT) |
                        (STK_PS_GAIN64 << STK_PS_GAIN_SHIFT) |
                        (STK_PS_IT400 << STK_PS_IT_SHIFT);
#ifdef STK_IRS
    mTask.ir_code = 0;
    mTask.als_data_index = 0;
    mTask.als_correct_factor = 1000;
    mTask.irs_running = false;
#endif // STK_IRS

    mTask.state_val = 0; /* Standby */
    i2cMasterRequest(mTask.hw->i2c_num, I2C_SPEED);
    mTask.txBuf[0] = STK_PDT_ID_REG;
    ret = i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                            &mTask.deviceId, 1, NULL, NULL);

    if (ret < 0)
    {
        osLog(LOG_ERROR, "%s: i2cMasterTxRxSync fail!!!\n", __func__);
        ret = -1;
        i2cMasterRelease(mTask.hw->i2c_num);
        goto err_out;
    }

    osLog(LOG_ERROR, "%s: auto detect success:0x%x\n", __func__, mTask.deviceId);

    for (pid_count = 0; pid_count < STK3321_PID_LIST_NUM; pid_count++)
    {
        if (((mTask.deviceId & 0xF0) == 0x10) || ((mTask.deviceId & 0xF0) == 0x20) || ((mTask.deviceId & 0xF0) == 0x30))
        {
            goto pid_pass;
        }
    }

    osLog(LOG_ERROR, "%s: Init Fail\n", __func__);
    goto err_out;
pid_pass:

    if (mTask.deviceId == STK3310SA_PID || mTask.deviceId == STK3311SA_PID)
        mTask.ledctrl_val = STK_LED_12_5mA | STK_LED_DT_64_64;
    else
        mTask.ledctrl_val = STK_LED_100mA | STK_LED_DT_64_64;

#ifdef STK3321_ALS_ENABLE
    alsSensorRegister();
#endif
#ifdef STK3321_PS_ENABLE
    psSensorRegister();
#endif
#if (defined(STK3321_ALS_ENABLE) && !defined(STK3321_PS_ENABLE))
    registerAlsSeparateFsm(stk3321Fsm, ARRAY_SIZE(stk3321Fsm));
#elif (!defined(STK3321_ALS_ENABLE) && defined(STK3321_PS_ENABLE))
    registerPsSeparateFsm(stk3321Fsm, ARRAY_SIZE(stk3321Fsm));
    registerPsInterruptMode(PS_INTERRUPT_MODE);
#elif (defined(STK3321_ALS_ENABLE) && defined(STK3321_PS_ENABLE))
    registerAlsPsDriverFsm(stk3321Fsm, ARRAY_SIZE(stk3321Fsm));
    registerPsInterruptMode(PS_INTERRUPT_MODE);
#endif
    osLog(LOG_ERROR, "%s: Init Done\n", __func__);
err_out:
    return ret;
}

#ifndef CFG_OVERLAY_INIT_SUPPORT
    MODULE_DECLARE(stk3321, SENS_TYPE_ALS, stk3321Init);
#else
    #include "mtk_overlay_init.h"
    #if (defined(STK3321_PS_ENABLE) && !defined(STK3321_ALS_ENABLE))
        OVERLAY_DECLARE(stk3321, OVERLAY_ID_ALSPS_SECONDARY, stk3321Init);
    #else
        OVERLAY_DECLARE(stk3321, OVERLAY_ID_ALSPS, stk3321Init);
    #endif
#endif
