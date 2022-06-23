/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly
 * prohibited.
 */
/* MediaTek Inc. (C) 2021. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY
 * ACKNOWLEDGES THAT IT IS RECEIVER\'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY
 * THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK
 * SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO
 * RECEIVER\'S SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN
 * FORUM. RECEIVER\'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK\'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER
 * WILL BE, AT MEDIATEK\'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE
 * AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY
 * RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation
 * ("MediaTek Software") have been modified by MediaTek Inc. All revisions are
 * subject to any receiver\'s applicable license agreements with MediaTek Inc.
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
#include "eint.h"
#include "vc36658.h"
#include "alsps.h"
#include "cache_internal.h"

#define ALS_NAME     "vc36658_l"
#define PS_NAME      "vc36658_p"
#define I2C_SPEED                       400000
#define DEVICE_ID   0x58

#define UNIT_MS_TO_NS 1000000

enum cm36558State {
    STATE_SAMPLE_ALS = CHIP_SAMPLING_ALS,
    STATE_SAMPLE_ALS_DONE = CHIP_SAMPLING_ALS_DONE,
    STATE_SAMPLE_PS = CHIP_SAMPLING_PS,
    STATE_SAMPLE_PS_ONCE = CHIP_SAMPLING_PS_ONCE,
    STATE_SAMPLE_PS_DONE = CHIP_SAMPLING_PS_DONE,
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
    STATE_RGBW_ENABLE = CHIP_RGBW_ENABLE,
    STATE_RGBW_ENABLE_DONE = CHIP_RGBW_ENABLE_DONE,
    STATE_RGBW_DISABLE = CHIP_RGBW_DISABLE,
    STATE_RGBW_DISABLE_DONE = CHIP_RGBW_DISABLE_DONE,
    STATE_RGBW_RATECHG = CHIP_RGBW_RATECHG,
    STATE_RGBW_RATECHG_DONE = CHIP_RGBW_RATECHG_DONE,
    STATE_INIT_DONE = CHIP_INIT_DONE,
    STATE_IDEL = CHIP_IDLE,
    STATE_RESET = CHIP_RESET,

    STATE_GET_ALS_CS_G_DATA,
    STATE_GET_ALS_CS_B_DATA,
    STATE_GET_ALS_CS_IR_DATA,
    STATE_GET_ALS_LUX,
    STATE_ALS_SET_DEBOUNCE,
    STATE_GET_PS_RAW_DATA,
    STATE_GET_PS_RAW_DATA_ONCE,
    STATE_PS_SET_DEBOUNCE,
    STATE_ALS_CONF_R,
    STATE_ALS_THDH_W,
    STATE_ALS_THDL_W,
    STATE_ALS_POWER_ON,
    STATE_ALS_POWER_OFF,
    STATE_ALS_CS_CONF1_CS_SD,
    STATE_PS_CONF_R,
    STATE_PS_CONF1_2_W,
    STATE_PS_CONF3_MS_W,
    STATE_PS_CANC_W,
    STATE_PS_THDH_W,
    STATE_PS_THDL_W,
    STATE_PS_POWER_OFF,
    STATE_PS_GET_CONF,
    STATE_PS_UNMASK_EINT,
    STATE_SETUP_EINT,
    STATE_CORE
};

struct scale_factor {
    unsigned char  whole;
    unsigned char  fraction;
};

#define MAX_RXBUF 8
#define MAX_TXBUF 8
SRAM_REGION_BSS static struct vc36558Task {
    /* txBuf for i2c operation, fill register and fill value */
    bool alsPowerOn;
    bool psPowerOn;
    unsigned int    als_debounce_time;
    unsigned int    als_debounce_on;    /*indicates if the debounce is on*/
    uint64_t        als_debounce_end;
    unsigned int    ps_debounce_time;
    unsigned int    ps_debounce_on;     /*indicates if the debounce is on*/
    uint64_t        ps_debounce_end;
    unsigned int    ps_suspend;
    uint8_t txBuf[MAX_TXBUF];
    /* rxBuf for i2c operation, receive rawdata */
    uint8_t rxBuf[MAX_RXBUF];
    uint8_t deviceId;
    /* data for factory */
    struct alsps_hw *hw;
    uint8_t i2c_addr;
    struct transferDataInfo dataInfo;
    struct AlsPsData data[2];
    uint32_t als_raw_data;
    uint32_t prox_raw_data;
    int32_t psCali;
    int32_t alsCali;
    uint32_t    ps_threshold_high;
    uint32_t    ps_threshold_low;
    uint32_t    als_threshold_high;
    uint32_t    als_threshold_low;

    uint16_t cs_raw_r_data;
    uint16_t cs_raw_g_data;
    uint16_t cs_raw_b_data;
    uint16_t cs_raw_ir_data;

    bool ps_enable;
    bool als_enable;
    bool rgbw_enabled;
} mTask;

static struct vc36558Task *vc36558DebugPoint;

static void cal_end_time(unsigned int ms, uint64_t *end_tick)
{
    *end_tick = rtcGetTime() +
            ((uint64_t)ms * UNIT_MS_TO_NS);
}

static int vc36658_read_als(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    if (rxTransferDataInfo(&mTask.dataInfo, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize)) {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "als rx error\n");
        return -1;
    }
    mTask.txBuf[0] = VC36658_REG_CS_R_DATA;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                  mTask.rxBuf, 2, i2cCallBack, next_state);
}

static int vc36658_read_als_cs_g_data(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.cs_raw_r_data = (mTask.rxBuf[1] << 8 | mTask.rxBuf[0]);
    mTask.txBuf[0] = VC36658_REG_CS_G_DATA;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                  mTask.rxBuf, 2, i2cCallBack, next_state);
}

static int vc36658_read_als_cs_b_data(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.cs_raw_g_data = (mTask.rxBuf[1] << 8 | mTask.rxBuf[0]);
    mTask.txBuf[0] = VC36658_REG_CS_B_DATA;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                  mTask.rxBuf, 2, i2cCallBack, next_state);
}

static int vc36658_read_als_cs_ir_data(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.cs_raw_b_data = (mTask.rxBuf[1] << 8 | mTask.rxBuf[0]);
    mTask.txBuf[0] = VC36658_REG_CS_IR_DATA;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 2, i2cCallBack, next_state);
}

int getLuxFromAlsData(void)
{
    float lux = 0.0, icf = 0.0, luxPerBit = 0.0;

    if (mTask.cs_raw_g_data > 0)
        icf = (float)mTask.cs_raw_ir_data / mTask.cs_raw_g_data;
    else
        icf = (float)mTask.cs_raw_ir_data;

    if (icf < ICF_LUX_DIVIDE_POINT) {
        luxPerBit = LPB_C;
        lux =  (float)mTask.cs_raw_g_data * luxPerBit * LUX_GAIN * Lin_C
                * CORRECTION_FACTOR;
    } else {
        luxPerBit = LPB_A2 * icf * icf + LPB_A1 * icf + LPB_A0;
        lux =  (float)mTask.cs_raw_g_data * luxPerBit * LUX_GAIN * Lin_A
                * CORRECTION_FACTOR;
    }
    return (int)lux;
}

static int vc36658_get_als_value(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                 void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                 void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int als_data = 0;

    mTask.cs_raw_ir_data = ((mTask.rxBuf[1] << 8) | mTask.rxBuf[0]);
    als_data = getLuxFromAlsData();

    if (als_data < 0)
        mTask.data[0].sensType = SENS_TYPE_INVALID; /*invalid lux value when debounce is on*/
    else {
        mTask.data[0].als_data = als_data;
        mTask.data[0].sensType = SENS_TYPE_ALS;
    }

    txTransferDataInfo(&mTask.dataInfo, 1, &mTask.data[0]);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static void alsGetData(void *sample)
{
    struct SingleAxisDataPoint *singleSample = (struct SingleAxisDataPoint *)sample;
    singleSample->idata = getLuxFromAlsData();
}

static int vc36658_read_ps(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    if (rxTransferDataInfo(&mTask.dataInfo, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize)) {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "ps rx error\n");
        return -1;
    }

    mTask.txBuf[0] = VC36658_REG_INT_FLAG;
    i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 2, NULL,
                         NULL);

    mTask.txBuf[0] = VC36658_REG_PS_DATA;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 2, i2cCallBack,
                         next_state);
}

static int vc36658_get_ps_raw_data(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                   void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                   void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint32_t ps_flag = 0;

    mTask.prox_raw_data = mTask.rxBuf[1] << 8 | mTask.rxBuf[0];
    if (mTask.prox_raw_data < mTask.psCali)
        mTask.prox_raw_data = 0;
    mTask.prox_raw_data -= mTask.psCali;

    if (mTask.prox_raw_data >= mTask.ps_threshold_high) {
        ps_flag = 0;
    } else if (mTask.prox_raw_data <= mTask.ps_threshold_low) {
        ps_flag = 1;
    }
    if (ps_flag == 1) { /* 0 is far */
        mTask.data[0].prox_state = PROX_STATE_FAR; /* far */
        mTask.data[0].prox_data = 1; /* far state distance is 1cm */
    } else {
        mTask.data[0].prox_state = PROX_STATE_NEAR; /* near */
        mTask.data[0].prox_data = 0; /* near state distance is 0cm */
    }

    mt_eint_unmask(mTask.hw->eint_num);
    mTask.data[0].sensType = SENS_TYPE_PROX;
    txTransferDataInfo(&mTask.dataInfo, 1, &mTask.data[0]);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int vc36658_read_ps_once(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    if (rxTransferDataInfo(&mTask.dataInfo, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize)) {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "ps rx error\n");
        return -1;
    }
    mTask.txBuf[0] = VC36658_REG_PS_DATA;
    i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 2, i2cCallBack,
                         next_state);
    return 0;
}

static int vc36658_get_ps_raw_data_once(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                   void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                   void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint32_t ps_flag = 0;

    mTask.prox_raw_data = mTask.rxBuf[1] << 8 | mTask.rxBuf[0];
    if (mTask.prox_raw_data < mTask.psCali)
        mTask.prox_raw_data = 0;
    mTask.prox_raw_data -= mTask.psCali;

    /* for the first state reporting*/
    if (mTask.prox_raw_data >= mTask.ps_threshold_high) {
        ps_flag = 0;
    } else if (mTask.prox_raw_data <= mTask.ps_threshold_low) {
        ps_flag = 1;
    }
    if (ps_flag == 1) { /* 0 is far */
        mTask.data[0].prox_state = PROX_STATE_FAR; /* far */
        mTask.data[0].prox_data = 1; /* far state distance is 1cm */
    } else {
        mTask.data[0].prox_state = PROX_STATE_NEAR; /* near */
        mTask.data[0].prox_data = 0; /* near state distance is 0cm */
    }

    mTask.data[0].sensType = SENS_TYPE_PROX;
    txTransferDataInfo(&mTask.dataInfo, 1, &mTask.data[0]);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static void psGetData(void *sample)
{
    char txBuf[1];
    static char rxBuf[2];
    struct TripleAxisDataPoint *tripleSample = (struct TripleAxisDataPoint *)sample;
    tripleSample->ix = mTask.prox_raw_data;
    tripleSample->iy = (mTask.data[0].prox_state == PROX_STATE_NEAR ? 0 : 1);

    void get_ps_data(void *cookie, size_t tx, size_t rx, int err) {
        char *rxBuf = cookie;
        if (err == 0) {
            mTask.prox_raw_data = rxBuf[1] << 8 | rxBuf[0];
            if (mTask.prox_raw_data < mTask.psCali)
                mTask.prox_raw_data = 0;
            mTask.prox_raw_data -= mTask.psCali;
        } else
            osLog(LOG_INFO, "psGetData error (%d)\n", err);
    }
    txBuf[0] = VC36658_REG_PS_DATA;
    i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, txBuf, 1,
                  rxBuf, 2, get_ps_data, rxBuf);
}

void cm36558_ps_set_threshold(uint32_t threshold_high, uint32_t threshold_low)
{
    int ret;
    char txBuf[6];

    if (threshold_high <= 0 || threshold_high >= 0xffff
            || threshold_low <= 0 || threshold_low >= 0xffff
            || threshold_high < threshold_low) {
        osLog(LOG_INFO, "%s: Invalid thresholds[%u, %u]\n",
            __func__, threshold_high, threshold_low);
        return;
    }
    txBuf[0] = VC36658_REG_PS_THDH;
    txBuf[1] = (unsigned char)(threshold_high & 0xFF);
    txBuf[2] = (unsigned char)(threshold_high >> 8);
    ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, txBuf, 3,
                      NULL, 0);
    if (ret) {
        osLog(LOG_INFO, "ps thr_h error (%d)\n", ret);
        return;
    }
    txBuf[3] = VC36658_REG_PS_THDL;
    txBuf[4] = (unsigned char)(threshold_low & 0xFF);
    txBuf[5] = (unsigned char)(threshold_low >> 8);
    ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, &txBuf[3], 3,
                NULL, 0);
    if (ret) {
        osLog(LOG_INFO, "ps thr_l error (%d)\n", ret);
        return;
    }
}

static void psGetCalibration(int32_t *cali, int32_t size)
{
    cali[0] = mTask.psCali;
}

static void psSetCalibration(int32_t *cali, int32_t size)
{
    mTask.psCali = cali[0];
    cm36558_ps_set_threshold(mTask.ps_threshold_high + mTask.psCali,
                             mTask.ps_threshold_low + mTask.psCali);
}

static void psGetThreshold(uint32_t *threshold_high, uint32_t *threshold_low)
{
    *threshold_high = mTask.ps_threshold_high;
    *threshold_low = mTask.ps_threshold_low;
}

static void psSetThreshold(uint32_t threshold_high, uint32_t threshold_low)
{
    osLog(LOG_ERROR, "%s threshold_high:%lu threshold_low:%lu\n", __func__, threshold_high, threshold_low);
    mTask.ps_threshold_high = threshold_high;
    mTask.ps_threshold_low = threshold_low;
    cm36558_ps_set_threshold(mTask.ps_threshold_high, mTask.ps_threshold_low);
}

static void psGetSensorInfo(struct sensorInfo_t *data)
{
    strncpy(data->name, PS_NAME, sizeof(data->name));
}

static void alsGetSensorInfo(struct sensorInfo_t *data)
{
    strncpy(data->name, ALS_NAME, sizeof(data->name));
}

static int vc36658_set_als_debounce_on(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                       void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                       void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.als_debounce_on = 1;
    mTask.als_enable = true;
    cal_end_time(mTask.als_debounce_time, &mTask.als_debounce_end);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int vc36658_set_ps_debounce_on(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                      void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                      void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.ps_debounce_on = 1;
    mTask.ps_enable = true;
    cal_end_time(mTask.ps_debounce_time, &mTask.ps_debounce_end);
    mTask.data[0].prox_state = PROX_STATE_INIT;
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int vc36658_set_als_power_on(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                    void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                    void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.txBuf[0] = VC36658_REG_CS_CONF;
    mTask.txBuf[1] = mTask.rxBuf[0] & 0xFE;
    mTask.txBuf[2] = mTask.rxBuf[1];
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 3,
                       i2cCallBack, next_state);
}

static int vc36658_set_als_power_off(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                     void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                     void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.als_debounce_on = 0;
    mTask.als_enable = false;

    if (mTask.ps_enable == false) {
        mTask.txBuf[0] = VC36658_REG_CS_CONF;
        mTask.txBuf[1] = mTask.rxBuf[0] & (~(1 << 1));
        mTask.txBuf[2] = mTask.rxBuf[1];
        return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 3,
                           i2cCallBack, next_state);
    } else {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
        return 0;
    }
}

static int vc36658_als_ratechg(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                               void *inBuf, uint8_t inSize, uint8_t elemInSize,
                               void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int vc36658_als_cali(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                              void *inBuf, uint8_t inSize, uint8_t elemInSize,
                              void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int32_t alsCali[2];
    float   alsCali_mi;

    alsCali_mi = 0.345;
    alsCali[0] = alsCali_mi * ALS_INCREASE_NUM_AP;
    alsCali[1] = 0;

    alsPsSendCalibrationResult(SENS_TYPE_ALS, alsCali);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int vc36658_als_cfg(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                              void *inBuf, uint8_t inSize, uint8_t elemInSize,
                              void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;
    struct alspsCaliCfgPacket caliCfgPacket;

    ret = rxCaliCfgInfo(&caliCfgPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);

    if (ret < 0) {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "cm36558_als_cfg, rx inSize and elemSize error\n");
        return -1;
    }
    mTask.alsCali = caliCfgPacket.caliCfgData[0];
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int vc36658_mask_eint(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    mt_eint_mask(mTask.hw->eint_num);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

/*  Need add unmask operation to ratechg, if sensor has setting rate */
static int vc36658_unmask_eint(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    mt_eint_unmask(mTask.hw->eint_num);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int vc36658_set_ps_power_on(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                   void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                   void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    if (mTask.als_enable == false) {
        mTask.txBuf[0] = VC36658_REG_CS_CONF;
        i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                             mTask.rxBuf, 2, NULL, NULL);

        mTask.txBuf[0] = VC36658_REG_CS_CONF;
        mTask.txBuf[1] = mTask.rxBuf[0] | (1 << 1);
        mTask.txBuf[2] = mTask.rxBuf[1];
        i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 3,
                          NULL, 0, NULL, NULL);

        mTask.txBuf[0] = VC36658_REG_CS_CONF;
        mTask.txBuf[1] |= (1 << 7);
        i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                          NULL, 0, NULL, NULL);
    }

    mTask.txBuf[0] = VC36658_REG_PS_CONF1_2;
    i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 2, NULL, NULL);

    mTask.txBuf[0] = VC36658_REG_PS_CONF1_2;
    mTask.txBuf[1] = mTask.rxBuf[0] & 0xFE;
    mTask.txBuf[2] = mTask.rxBuf[1];
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 3,
                       i2cCallBack, next_state);
}

static int vc36658_set_ps_power_off(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                    void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                    void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.ps_debounce_on = 0;
    mTask.ps_enable = false;
    mTask.data[0].prox_state = PROX_STATE_FAR; /* default value */
    mTask.txBuf[0] = VC36658_REG_PS_CONF1_2;
    mTask.txBuf[1] = mTask.rxBuf[0] | 0x01;
    mTask.txBuf[2] = mTask.rxBuf[1];
    i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 3,
                       NULL, NULL);

    if (mTask.als_enable == false) {
        mTask.txBuf[0] = VC36658_REG_CS_CONF;
        i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 2, NULL, NULL);

        mTask.txBuf[0] = VC36658_REG_CS_CONF;
        mTask.txBuf[1] = mTask.rxBuf[0] & (~(1 << 1));
        mTask.txBuf[2] = mTask.rxBuf[1];
        return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 3,
                       i2cCallBack, next_state);
    } else {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
        return 0;
    }
}

static int vc36658_ps_ratechg(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                              void *inBuf, uint8_t inSize, uint8_t elemInSize,
                              void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int vc36658_ps_cali(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                              void *inBuf, uint8_t inSize, uint8_t elemInSize,
                              void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int32_t threshold[2];
    threshold[0] = 2; // high simulation data
    threshold[1] = 3; // low simulation data
    alsPsSendCalibrationResult(SENS_TYPE_PROX, threshold);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int vc36658_ps_cfg(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                              void *inBuf, uint8_t inSize, uint8_t elemInSize,
                              void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;
    struct alspsCaliCfgPacket caliCfgPacket;

    ret = rxCaliCfgInfo(&caliCfgPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);

    if (ret < 0) {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "cm36558_ps_cfg, rx inSize and elemSize error\n");
        return -1;
    }
    cm36558_ps_set_threshold(caliCfgPacket.caliCfgData[0], caliCfgPacket.caliCfgData[1]);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int vc36658_get_ps_conf(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                               void *inBuf, uint8_t inSize, uint8_t elemInSize,
                               void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.txBuf[0] = VC36658_REG_PS_CONF1_2;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 2, i2cCallBack, next_state);
}

static int vc36658_als_enable(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    if (mTask.ps_enable == false) {
        mTask.txBuf[0] = VC36658_REG_CS_CONF;
        i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                          mTask.rxBuf, 2, NULL, NULL);
        mTask.txBuf[0] = VC36658_REG_CS_CONF;
        mTask.txBuf[1] = mTask.rxBuf[0] | (1 << 1);
        mTask.txBuf[2] = mTask.rxBuf[1];
        i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 3,
                          NULL, 0, NULL, NULL);
        mTask.txBuf[0] = VC36658_REG_CS_CONF;
        mTask.txBuf[1] |= (1 << 7);
        i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                          NULL, 0, NULL, NULL);
    }
    mTask.txBuf[0] = VC36658_REG_CS_CONF;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 2, i2cCallBack, next_state);
}

static int vc36658_als_disen(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.txBuf[0] = VC36658_REG_CS_CONF;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 2, i2cCallBack, next_state);
}

static int vc36658_als_cs_conf1_cs_sd(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.txBuf[0] = VC36658_REG_CS_CONF;
    mTask.txBuf[1] = mTask.rxBuf[0] | 0x1;
    mTask.txBuf[2] = mTask.rxBuf[1];
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 3,
                       i2cCallBack, next_state);
}

static int vc36658_set_als_conf(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.als_enable = false;
    mTask.txBuf[0] = VC36658_REG_CS_CONF;
    mTask.txBuf[1] = 0b00000001;
    if (1 == mTask.hw->polling_mode_als)
        mTask.txBuf[2] = 0b00000010;
    else
        mTask.txBuf[2] = 0b00000011;
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 3,
                       i2cCallBack, next_state);
}

static int vc36658_set_als_thdh(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.txBuf[0] = VC36658_REG_CS_THDH;
    mTask.txBuf[1] = (unsigned char)(mTask.als_threshold_high & 0xFF);
    mTask.txBuf[2] = (unsigned char)(mTask.als_threshold_high >> 8);
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 3,
                       i2cCallBack, next_state);
}

static int vc36658_set_als_thdl(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.txBuf[0] = VC36658_REG_CS_THDL;
    mTask.txBuf[1] = (unsigned char)(mTask.als_threshold_low & 0xFF);
    mTask.txBuf[2] = (unsigned char)(mTask.als_threshold_low >> 8);
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 3,
                       i2cCallBack, next_state);
}

static int vc36658_set_ps_conf1_2(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                  void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                  void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.ps_enable = false;
    mTask.txBuf[0] = VC36658_REG_PS_CONF1_2;
    mTask.txBuf[1] = 0b00000011;
    if (!mTask.hw->polling_mode_ps)
        mTask.txBuf[1] |= 0x08;
    mTask.txBuf[2] = 0b00001001;
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 3,
                       i2cCallBack, next_state);
}

static int vc36658_set_ps_conf3_ms(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                   void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                   void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.txBuf[0] = VC36658_REG_PS_CONF3_MS;
    mTask.txBuf[1] = 0b00001000;
    mTask.txBuf[2] = 0b00000010;
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 3,
                       i2cCallBack, next_state);
}

static int vc36658_set_ps_canc(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                               void *inBuf, uint8_t inSize, uint8_t elemInSize,
                               void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.txBuf[0] = VC36658_REG_PS_CANC;
    mTask.txBuf[1] = 0x00;
    mTask.txBuf[2] = 0x00;
    i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 3,
                NULL, NULL);
    mTask.txBuf[0] = VC36658_REG_PS_AC;
    mTask.txBuf[1] = 0x00;
    mTask.txBuf[2] = 0x00;
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 3,
                       i2cCallBack, next_state);
}

static int vc36658_set_ps_thdh(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                               void *inBuf, uint8_t inSize, uint8_t elemInSize,
                               void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.txBuf[0] = VC36658_REG_PS_THDH;
    mTask.txBuf[1] = (unsigned char)((mTask.ps_threshold_high + mTask.psCali) & 0xFF);
    mTask.txBuf[2] = (unsigned char)((mTask.ps_threshold_high + mTask.psCali) >> 8);
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 3,
                       i2cCallBack, next_state);
}

static int vc36658_set_ps_thdl(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                               void *inBuf, uint8_t inSize, uint8_t elemInSize,
                               void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.txBuf[0] = VC36658_REG_PS_THDL;
    mTask.txBuf[1] = (unsigned char)((mTask.ps_threshold_low + mTask.psCali) & 0xFF);
    mTask.txBuf[2] = (unsigned char)((mTask.ps_threshold_low + mTask.psCali) >> 8);
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 3,
                       i2cCallBack, next_state);
}

static int vc36658_rgbw_enable(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                               void *inBuf, uint8_t inSize, uint8_t elemInSize,
                               void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.rgbw_enabled = true;
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int vc36658_rgbw_disable(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                               void *inBuf, uint8_t inSize, uint8_t elemInSize,
                               void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.rgbw_enabled = false;
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int vc36658_rgbw_ratechg(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                               void *inBuf, uint8_t inSize, uint8_t elemInSize,
                               void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

SRAM_REGION_FUNCTION void vc36658_eint_handler(int arg)
{
    alsPsInterruptOccur();
}

static int vc36658_setup_eint(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                              void *inBuf, uint8_t inSize, uint8_t elemInSize,
                              void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    if (!mTask.hw->polling_mode_ps) {
        mt_eint_dis_hw_debounce(mTask.hw->eint_num);
        mt_eint_registration(mTask.hw->eint_num, LEVEL_SENSITIVE, LOW_LEVEL_TRIGGER, vc36658_eint_handler, EINT_INT_UNMASK,
                         EINT_INT_AUTO_UNMASK_OFF);
    }
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int vc36658_register_core(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                 void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                 void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    struct sensorCoreInfo mInfo;
    int ret = 0;

    memset(&mInfo, 0x00, sizeof(struct sensorCoreInfo));
    /* Register sensor Core */
    mInfo.sensType = SENS_TYPE_ALS;
    mInfo.getData = alsGetData;
    mInfo.getSensorInfo = alsGetSensorInfo;
    sensorCoreRegister(&mInfo);

    mInfo.sensType = SENS_TYPE_PROX,
    mInfo.gain = 1;
    mInfo.sensitivity = 1;
    ret = sensorDriverGetConvert(0, &mInfo.cvt);
    if (ret != 0)
        osLog(LOG_ERROR, "alsps: invalid direction!\n");
    mInfo.getCalibration = psGetCalibration;
    mInfo.setCalibration = psSetCalibration;
    mInfo.getThreshold = psGetThreshold;
    mInfo.setThreshold = psSetThreshold;
    mInfo.getData = psGetData;
    mInfo.getSensorInfo = psGetSensorInfo;
    sensorCoreRegister(&mInfo);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static struct sensorFsm vc36658Fsm[] = {
    sensorFsmCmd(STATE_SAMPLE_ALS, STATE_GET_ALS_CS_G_DATA, vc36658_read_als),
    sensorFsmCmd(STATE_GET_ALS_CS_G_DATA, STATE_GET_ALS_CS_B_DATA, vc36658_read_als_cs_g_data),
    sensorFsmCmd(STATE_GET_ALS_CS_B_DATA, STATE_GET_ALS_CS_IR_DATA, vc36658_read_als_cs_b_data),
    sensorFsmCmd(STATE_GET_ALS_CS_IR_DATA, STATE_GET_ALS_LUX, vc36658_read_als_cs_ir_data),
    sensorFsmCmd(STATE_GET_ALS_LUX, STATE_SAMPLE_ALS_DONE, vc36658_get_als_value),

    sensorFsmCmd(STATE_SAMPLE_PS, STATE_GET_PS_RAW_DATA, vc36658_read_ps),
    sensorFsmCmd(STATE_GET_PS_RAW_DATA, STATE_SAMPLE_PS_DONE, vc36658_get_ps_raw_data),

    /*ps sample data once, for interrupt mode*/
    sensorFsmCmd(STATE_SAMPLE_PS_ONCE, STATE_GET_PS_RAW_DATA_ONCE, vc36658_read_ps_once),
    sensorFsmCmd(STATE_GET_PS_RAW_DATA_ONCE, STATE_SAMPLE_PS_DONE, vc36658_get_ps_raw_data_once),
    /* als enable state */
    sensorFsmCmd(STATE_ALS_ENABLE, STATE_ALS_POWER_ON, vc36658_als_enable),
    sensorFsmCmd(STATE_ALS_POWER_ON, STATE_ALS_SET_DEBOUNCE, vc36658_set_als_power_on),
    sensorFsmCmd(STATE_ALS_SET_DEBOUNCE, STATE_ALS_ENABLE_DONE, vc36658_set_als_debounce_on),
    /* als disable state */
    sensorFsmCmd(STATE_ALS_DISABLE, STATE_ALS_CS_CONF1_CS_SD, vc36658_als_disen),
    sensorFsmCmd(STATE_ALS_CS_CONF1_CS_SD, STATE_ALS_POWER_OFF, vc36658_als_cs_conf1_cs_sd),
    sensorFsmCmd(STATE_ALS_POWER_OFF, STATE_ALS_DISABLE_DONE, vc36658_set_als_power_off),
    /* als rate state */
    sensorFsmCmd(STATE_ALS_RATECHG, CHIP_ALS_RATECHG_DONE, vc36658_als_ratechg),
    /* als cali state */
    sensorFsmCmd(STATE_ALS_CALI, CHIP_ALS_CALI_DONE, vc36658_als_cali),
    /* als cfg state */
    sensorFsmCmd(STATE_ALS_CFG, CHIP_ALS_CFG_DONE, vc36658_als_cfg),
    /* ps enable state */
    sensorFsmCmd(STATE_PS_ENABLE, STATE_PS_SET_DEBOUNCE, vc36658_set_ps_power_on),
    sensorFsmCmd(STATE_PS_SET_DEBOUNCE, STATE_PS_UNMASK_EINT, vc36658_set_ps_debounce_on),
    sensorFsmCmd(STATE_PS_UNMASK_EINT, STATE_PS_ENABLE_DONE, vc36658_unmask_eint),
    /* ps disable state */
    sensorFsmCmd(STATE_PS_DISABLE, STATE_PS_GET_CONF, vc36658_mask_eint),
    sensorFsmCmd(STATE_PS_GET_CONF, STATE_PS_POWER_OFF, vc36658_get_ps_conf),
    sensorFsmCmd(STATE_PS_POWER_OFF, STATE_PS_DISABLE_DONE, vc36658_set_ps_power_off),
    /* ps rate state */
    sensorFsmCmd(STATE_PS_RATECHG, CHIP_PS_RATECHG_DONE, vc36658_ps_ratechg),
    /* ps cali state */
    sensorFsmCmd(STATE_PS_CALI, CHIP_PS_CALI_DONE, vc36658_ps_cali),
    /* ps cfg state */
    sensorFsmCmd(STATE_PS_CFG, CHIP_PS_CFG_DONE, vc36658_ps_cfg),
    /* rgbw enable state */
    sensorFsmCmd(STATE_RGBW_ENABLE, STATE_RGBW_ENABLE_DONE, vc36658_rgbw_enable),
    /* rgbw disable state */
    sensorFsmCmd(STATE_RGBW_DISABLE, STATE_RGBW_DISABLE_DONE, vc36658_rgbw_disable),
    /* rgbw rate state */
    sensorFsmCmd(STATE_RGBW_RATECHG, CHIP_RGBW_RATECHG_DONE, vc36658_rgbw_ratechg),
    /* init state */
    sensorFsmCmd(STATE_RESET, STATE_ALS_THDH_W, vc36658_set_als_conf),
    sensorFsmCmd(STATE_ALS_THDH_W, STATE_ALS_THDL_W, vc36658_set_als_thdh),
    sensorFsmCmd(STATE_ALS_THDL_W, STATE_PS_CONF1_2_W, vc36658_set_als_thdl),
    sensorFsmCmd(STATE_PS_CONF1_2_W, STATE_PS_CONF3_MS_W, vc36658_set_ps_conf1_2),
    sensorFsmCmd(STATE_PS_CONF3_MS_W, STATE_PS_CANC_W, vc36658_set_ps_conf3_ms),
    sensorFsmCmd(STATE_PS_CANC_W, STATE_PS_THDH_W, vc36658_set_ps_canc),
    sensorFsmCmd(STATE_PS_THDH_W, STATE_PS_THDL_W, vc36658_set_ps_thdh),
    sensorFsmCmd(STATE_PS_THDL_W, STATE_SETUP_EINT, vc36658_set_ps_thdl),
    sensorFsmCmd(STATE_SETUP_EINT, STATE_CORE, vc36658_setup_eint),
    sensorFsmCmd(STATE_CORE, STATE_INIT_DONE, vc36658_register_core),
};

static int init_vc36658(void)
{
    int ret = 0;
    vc36558DebugPoint = &mTask;

    mTask.hw = get_cust_alsps("vc36658");
    if (NULL == mTask.hw) {
        osLog(LOG_ERROR, "get_cust_acc_hw fail\n");
        return 0;
    }
    mTask.i2c_addr = mTask.hw->i2c_addr[0];
    mTask.als_debounce_time = 200;
    mTask.als_debounce_on = 0;
    mTask.ps_debounce_time = 200;
    mTask.ps_debounce_on = 0;
    mTask.psCali = 0;
    mTask.alsCali = 0;
    mTask.als_threshold_high = mTask.hw->als_threshold_high;
    mTask.als_threshold_low = mTask.hw->als_threshold_low;
    mTask.ps_threshold_high = mTask.hw->ps_threshold_high;
    mTask.ps_threshold_low = mTask.hw->ps_threshold_low;
    osLog(LOG_ERROR, "alsps i2c_num: %d, i2c_addr: 0x%x\n", mTask.hw->i2c_num, mTask.i2c_addr);

    i2cMasterRequest(mTask.hw->i2c_num, I2C_SPEED);
    if (mTask.hw->polling_mode_ps)
        registerPsInterruptMode(1);

    mTask.txBuf[0] = VC36658_REG_ID;

    for (uint8_t i = 0; i < 3; i++) {
        ret = i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
            &mTask.deviceId, 1, NULL, NULL);
        if (ret >= 0)
            break;
    }

    if (ret < 0) {
        ret = -1;
        sendSensorErrToAp(ERR_SENSOR_ALS_PS, ERR_CASE_ALS_PS_INIT, ALS_NAME);
        sendSensorErrToAp(ERR_SENSOR_ALS_PS, ERR_CASE_ALS_PS_INIT, PS_NAME);
        i2cMasterRelease(mTask.hw->i2c_num);
        goto err_out;
    }

    if (mTask.deviceId == DEVICE_ID) {
        osLog(LOG_INFO, "vc36558 auto detect success %x\n", mTask.deviceId);
        goto success_out;
    } else {
        ret = -1;
        sendSensorErrToAp(ERR_SENSOR_ALS_PS, ERR_CASE_ALS_PS_INIT, ALS_NAME);
        sendSensorErrToAp(ERR_SENSOR_ALS_PS, ERR_CASE_ALS_PS_INIT, PS_NAME);
        i2cMasterRelease(mTask.hw->i2c_num);
        goto err_out;
    }

success_out:
    alsSensorRegister();
    psSensorRegister();
    registerAlsPsDriverFsm(vc36658Fsm, ARRAY_SIZE(vc36658Fsm));
err_out:
    return ret;
}

#ifndef CFG_OVERLAY_INIT_SUPPORT
MODULE_DECLARE(vc36658, SENS_TYPE_ALS, init_vc36658);
#else
#include "mtk_overlay_init.h"
OVERLAY_DECLARE(vc36658, OVERLAY_ID_ALSPS, init_vc36658);
#endif
