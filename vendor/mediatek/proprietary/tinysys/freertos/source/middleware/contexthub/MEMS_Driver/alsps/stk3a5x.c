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
#include "stk3a5x.h"
#include "alsps.h"
#include "cache_internal.h"

enum stk3a5xState {
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
    STATE_PS_TUNE_FAE,
    STATE_PS_TUNE_FAE_DONE,
    STATE_INIT_DONE = CHIP_INIT_DONE,
    STATE_IDEL = CHIP_IDLE,
    STATE_RESET = CHIP_RESET,
    /* Alsps sample */
    STATE_GET_ALS_DATA,
    STATE_GET_ALS_DATA01_PRE,
    STATE_GET_ALS_DATA01,
    STATE_ALS_SET_DEBOUNCE,
    STATE_GET_PS_FLG,
    STATE_GET_PS_RAW_DATA,
    STATE_PS_SET_DEBOUNCE,
    /* Power on & off */
    STATE_GET_ALSPS_STATE,
    STATE_ALS_POWER_ON,
    STATE_ALS_POWER_OFF,
    STATE_PS_POWER_ON,
    STATE_PS_POWER_OFF,
    STATE_PS_UNMASK_EINT,
    STATE_CLR_INT,
    STATE_EN_EINT,
    /* Init state */
    STATE_SET_SW_RST,
	STATE_SET_SW_RST_WAIT01,
    STATE_SET_SW_RST_WAIT02,
    STATE_SET_SW_RST_WAIT03,
    STATE_SET_SW_RST_WAIT04,
    STATE_SET_SW_RST_WAIT05,
    STATE_SET_ALSPS_CTRL,
    STATE_SET_WAIT,
    STATE_SET_PS_THDH,
    STATE_SET_PS_THDL,
    STATE_SET_ALSCTRL2,
	STATE_SET_INTELLI_WAIT,
	STATE_SET_BGIR,
	STATE_SET_PD,
	STATE_SET_AGAIN,
	STATE_SET_SPEC1,
    STATE_SET_SPEC2,
    STATE_SETUP_EINT,
    STATE_CORE,
    STATE_PS_POWER_ON_GET_PS_THD,   
	STATE_PS_POWER_ON_SET_PS_THD,
	STATE_PS_POWER_ON_SET_INT,
	STATE_PS_POWER_OFF_CLR_INT,
    STATE_PS_POWER_OFF_RESET_THD,
	STATE_GET_PS_FLG_RAW,
	STATE_PS_TUNE_FAE_STEP1,
	STATE_PS_TUNE_FAE_STEP2,
	STATE_PS_TUNE_FAE_STEP3,
	STATE_PS_TUNE_FAE_STEP4,
	STATE_PS_TUNE_FAE_STEP5,
	STATE_PS_TUNE_FAE_STEP6,
	STATE_PS_TUNE_FAE_STEP7,
	STATE_PS_TUNE_FAE_STEP8,
	STATE_PS_TUNE_FAE_STEP9,	
	STATE_PS_TUNE_FAE_STEP10,
	STATE_PS_TUNE_FAE_STEP11,
	STATE_PS_TUNE_FAE_STEP12,
	STATE_PS_TUNE_FAE_STEP13,
    STATE_GET_PS_RAW_DATA_ONCE
};

#define I2C_SPEED                       400000
#define MAX_I2C_PER_PACKET          8
#define ICM20645_DATA_LEN           6
#define MAX_RXBUF                   512
#define MAX_TXBUF (MAX_RXBUF / MAX_I2C_PER_PACKET)

//PS Auto Cali Config
#define STK_H_HAND  1000//800
#define STK_H_HT    280
#define STK_H_LT    260

#define STK_HT_N_CT  110//150
#define STK_LT_N_CT  100 //135
#define STK_PS_DIFF  60

#define STK_ALS_CONVERSION  600

SRAM_REGION_BSS static struct stk3a5xTask {
    /* txBuf for i2c operation, fill register and fill value */
    bool alsPowerOn;
    bool psPowerOn;
    unsigned int    als_debounce_time;
    unsigned int    als_debounce_on;    /*indicates if the debounce is on*/
    portTickType    als_debounce_end;
    unsigned int    ps_debounce_time;
    unsigned int    ps_debounce_on;     /*indicates if the debounce is on*/
    portTickType    ps_debounce_end;
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
    int32_t psCali;
    int32_t alsCali;
    uint32_t    als_raw_data;
    uint16_t    r_data;
    uint32_t    prox_raw_data;
    uint32_t    ps_threshold_high;
    uint32_t    ps_threshold_low;
    uint32_t    als_threshold_high;
    uint32_t    als_threshold_low;
    uint8_t     ledctrl_val;
    uint8_t     state_val;
    uint8_t     als_ctrl_val;
    uint8_t     ps_ctrl_val;
    uint16_t    ir_code;
    uint16_t    last_ir_code;

	uint32_t	psoff_data;
	uint32_t	psi;
	uint32_t	psa;
	uint8_t		flag_reg;
	uint32_t	psi_set;
	enum ProxState prox_state;
	bool		ps_thd_update;
	
	uint32_t 	debug_count;
	bool		start_tune;
	bool 		first_int_after_en;
    bool        ps_int_flag;
    uint32_t 	als_debug_count;
	uint16_t    pocket_threshold;
} mTask;

static struct stk3a5xTask *stk3a5xDebugPoint;
static int getLuxFromAlsData(void);

#if 0
static int check_timeout(portTickType *end_tick) {
    return xTaskGetTickCount() > *end_tick ? 1 : 0;
}
#endif

static void cal_end_time(unsigned int ms, portTickType *end_tick) {
    *end_tick = xTaskGetTickCount() +
                (portTickType)(ms / (1000 / portTICK_RATE_MS));
}

static int stk3a5x_read_als_data(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    if (rxTransferDataInfo(&mTask.dataInfo, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize)) {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "als rx error\n");
        return -1;
    }
	osLog(LOG_ERROR, "stk3a5x_read_als_data...\n");  
     mTask.als_debug_count++;
    if(mTask.als_debug_count > 10000){
        mTask.als_debug_count = 0;
    }

    mTask.txBuf[0] = STK_DATA1_ALS_REG;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 4, i2cCallBack,
                         next_state);
}

static int stk3a5x_get_als_value(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    uint16_t als_data, r_data;

    als_data = ((mTask.rxBuf[0] << 8) | mTask.rxBuf[1]);
	r_data = ((mTask.rxBuf[2] << 8) | mTask.rxBuf[3]);
    if((mTask.als_debug_count%5) == 1){
	    osLog(LOG_ERROR, "stk3a5x_get_als_value: als=%d, r=%d", als_data, r_data);
    } 

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int stk3a5x_get_als01_pre(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.txBuf[0] = 0x17;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                            mTask.rxBuf, 6, i2cCallBack,
                                next_state);

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;

}

static int stk3a5x_get_als01(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    uint16_t g_data, b_data, c_data;
	uint16_t cg_ratio = 0;
    int als_data = 0;

    g_data = ((mTask.rxBuf[0] << 8) | mTask.rxBuf[1]);
    b_data = ((mTask.rxBuf[2] << 8) | mTask.rxBuf[3]);
    c_data = ((mTask.rxBuf[4] << 8) | mTask.rxBuf[5]);

    mTask.als_raw_data = g_data;

    if((mTask.als_debug_count%5) == 1){
        osLog(LOG_ERROR, "stk3a5x_get_als01: g=%d, b=%d, c=%d\n", g_data, b_data, c_data);
    }

    if(g_data < 100){
        cg_ratio = 65;
    }else{
        cg_ratio = (c_data * 100)/ g_data;
    }

    if(cg_ratio >= 721){
        //A
        mTask.r_data = 2270;
    }else if(cg_ratio >= 100){
        //D65
        mTask.r_data = 2615;
    }else{
        //CWF
        mTask.r_data = 3845;
    }

	als_data = getLuxFromAlsData(); //aal mapping value
	if (als_data < 0){
		mTask.data[0].sensType = SENS_TYPE_INVALID; /*invalid value when debounce is on*/
	}else{
		if (als_data <=3) {
			mTask.data[0].als_data = 0;
		} else {
			mTask.data[0].als_data = als_data + (mTask.als_debug_count%2);
		}
		mTask.data[0].sensType = SENS_TYPE_ALS;
	}

    if((mTask.als_debug_count%5) == 1){
	    osLog(LOG_ERROR, "stk3a5x_get_als_value: lux=%d, r_data=%d\n", mTask.data[0].als_data, mTask.r_data);
    }

    txTransferDataInfo(&mTask.dataInfo, 1, &mTask.data[0]);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int getLuxFromAlsData(void) {
	return (int)((mTask.als_raw_data * mTask.r_data) / 1000);
}


static void alsGetData(void *sample) {
    struct SingleAxisDataPoint *singleSample = (struct SingleAxisDataPoint *)sample;
    singleSample->idata = getLuxFromAlsData();
}

static int stk3a5x_read_ps(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                          void *inBuf, uint8_t inSize, uint8_t elemInSize,
                          void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    if(mTask.psPowerOn == false){
        osLog(LOG_ERROR, "stk3a5x_read_ps power off"); 
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
        return 0;        
    }
    if (rxTransferDataInfo(&mTask.dataInfo, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize)) {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "stk3a5x ps, rx dataInfo error\n");
        return -1;
    }
	osLog(LOG_ERROR, "stk3a5x_read_ps...\n"); 
    mTask.debug_count++;
    if(mTask.debug_count > 50000){
        mTask.debug_count = 0;
    }

    mTask.txBuf[0] = STK_DATA1_PS_REG;

    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 2, i2cCallBack,
                         next_state);	
}


static int stk3a5x_read_ps_flag(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                          void *inBuf, uint8_t inSize, uint8_t elemInSize,
                          void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    if(mTask.psPowerOn == false){
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
        return 0;        
    }
	mTask.prox_raw_data = (mTask.rxBuf[0] << 8) | mTask.rxBuf[1];
	osLog(LOG_INFO, "stk read: ps=%d\n", mTask.prox_raw_data);
	
    mTask.txBuf[0] = STK_FLAG_REG;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 1, i2cCallBack,
                         next_state);
}

static int stk3a5x_get_ps_status(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    //uint32_t    ps_threshold_high;
    //uint32_t    ps_threshold_low;

    int ps_flag = mTask.rxBuf[0] & 0x01;
	
    if(mTask.psPowerOn == false){
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
        return 0;        
    }

    mTask.ps_int_flag = (mTask.rxBuf[0] & STK_FLG_PSINT_MASK) ? true:false;

    if((mTask.ps_int_flag == true) || (mTask.prox_state == PROX_STATE_INIT))
    {
	    if (ps_flag == 1) { /* 1 is far */
	        mTask.data[0].prox_state = PROX_STATE_FAR; /* far */
			mTask.prox_state = PROX_STATE_FAR;
	        mTask.data[0].prox_data = 1; /* far state distance is 1cm */
	    } else if (ps_flag == 0) {
	        mTask.data[0].prox_state = PROX_STATE_NEAR; /* near */
			mTask.prox_state = PROX_STATE_NEAR;
	        mTask.data[0].prox_data = 0; /* near state distance is 0cm */
	    } else{
            osLog(LOG_ERROR, "stk get_ps_status error status: %d\n", mTask.rxBuf[1]);
            sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
	        return -1;
        }
	}
    mTask.txBuf[0] = STK_DATA1_PS_REG;

	osLog(LOG_INFO, "stk get flag=0x%x, dis=%d, int flag=%d\n", mTask.rxBuf[0], mTask.data[0].prox_state, mTask.ps_int_flag);
	
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 2, i2cCallBack,
                         next_state);
}

static int stk3a5x_get_ps_raw_data(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                  void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                  void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    if(mTask.psPowerOn == false){
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
        return 0;        
    }
    mTask.prox_raw_data = (mTask.rxBuf[0] << 8) | mTask.rxBuf[1];
	osLog(LOG_ERROR, "stk get ps=%d, distance=%d, cali=%d\n", mTask.prox_raw_data, mTask.data[0].prox_state, mTask.psCali);
    if (mTask.prox_raw_data < mTask.psCali)
        mTask.prox_raw_data = 0;
    mTask.prox_raw_data -= mTask.psCali;

    mTask.txBuf[0] = STK_FLAG_REG;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                       mTask.rxBuf, 1, i2cCallBack,
                       next_state);
}

static int stk3a5x_clr_int(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                  void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                  void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    if(mTask.psPowerOn == false){
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
        return 0;        
    }
    if(mTask.ps_int_flag == true){                                
    osLog(LOG_ERROR, "stk3a5x_clr_int...\n");                              
    mTask.txBuf[0] = STK_FLAG_REG;
    mTask.txBuf[1] = mTask.rxBuf[0] & ~(STK_FLG_PSINT_MASK | STK_FLG_ALSINT_MASK);
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                       i2cCallBack, next_state);
    }
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int stk3a5x_enable_eint(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                  void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                  void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    if(mTask.psPowerOn == false){
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
        return 0;        
    }
    if(mTask.ps_int_flag == true){                                   
		 osLog(LOG_ERROR, "stk3a5x_enable_eint...\n");    
         mt_eint_unmask(mTask.hw->eint_num);
         mTask.data[0].sensType = SENS_TYPE_PROX;
         mTask.ps_int_flag = false;
         if(mTask.first_int_after_en == true)
            mTask.first_int_after_en = false;
         txTransferDataInfo(&mTask.dataInfo, 1, &mTask.data[0]);
    }
    else if(mTask.first_int_after_en == true){
        osLog(LOG_ERROR, "stk_first_report_event:%d\n", mTask.data[0].prox_data);
        mTask.data[0].sensType = SENS_TYPE_PROX;
        mTask.first_int_after_en = false;
        txTransferDataInfo(&mTask.dataInfo, 1, &mTask.data[0]);
    }
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}
static bool stk3a5x_is_skip(void) {
    if((mTask.psPowerOn == false)
        || (mTask.ps_int_flag == true)){
        return true;        
    } 
	
    return false;
}

static void psGetData(void *sample) {
    char txBuf[1];
    static char rxBuf[2];
    struct TripleAxisDataPoint *tripleSample = (struct TripleAxisDataPoint *)sample;
    tripleSample->ix = mTask.prox_raw_data;
    tripleSample->iy = (mTask.data[0].prox_state == PROX_STATE_NEAR ? 0 : 1);

    void get_ps_data(void *cookie, size_t tx, size_t rx, int err) {
        char *rxBuf = cookie;
        if (err == 0) {
            mTask.prox_raw_data = (rxBuf[0] << 8) | rxBuf[1];
            if (mTask.prox_raw_data < mTask.psCali)
                mTask.prox_raw_data = 0;
            mTask.prox_raw_data -= mTask.psCali;
        } else
            osLog(LOG_INFO, "stk3a5x: read ps data i2c error (%d)\n", err);
    }
    txBuf[0] = STK_DATA1_PS_REG;
    i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, txBuf, 1,
                  rxBuf, 2, get_ps_data, rxBuf);
}

static int stk3a5x_ps_set_threshold(uint32_t threshold_high, uint32_t threshold_low) {
    char txBuf[6];

    /* when user don't do ps cali, AP will set 0 to here,
     * so filter it and still use alsps cust setting */
    osLog(LOG_INFO, "stk set threshold:  high:%d    low:%d\n", threshold_high, threshold_low);
    if (threshold_high == 0 && threshold_low == 0)
        return -1;
    txBuf[0] = STK_THDH1_PS_REG;
    txBuf[1] = (uint8_t)((threshold_high & 0xFF00) >> 8);
    txBuf[2] = (uint8_t)(threshold_high & 0xFF);
    txBuf[3] = (uint8_t)((threshold_low & 0xFF00) >> 8);
    txBuf[4] = (uint8_t)(threshold_low & 0xFF);
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, &txBuf[0], 5, NULL, 0);
}

static void psGetCalibration(int32_t *cali, int32_t size) {
	osLog(LOG_INFO, "stk get calibration:  %d\n", cali[0]);
    cali[0] = mTask.psCali;
}

static void psSetCalibration(int32_t *cali, int32_t size) {
	osLog(LOG_INFO, "stk set calibration:  %d\n", cali[0]);
    mTask.psCali = cali[0];
    stk3a5x_ps_set_threshold(mTask.ps_threshold_high + mTask.psCali,
                            mTask.ps_threshold_low + mTask.psCali);
}

static void psGetThreshold(uint32_t *threshold_high, uint32_t *threshold_low) {
    *threshold_high = mTask.ps_threshold_high;
    *threshold_low = mTask.ps_threshold_low;
    osLog(LOG_INFO, "stk get ==>threshold_high:%ld threshold_low:%ld \n",  *threshold_high, *threshold_low);
}

static void psSetThreshold(uint32_t threshold_high, uint32_t threshold_low) {
    osLog(LOG_INFO, "stk set==>threshold_high:%ld threshold_low:%ld \n", threshold_high, threshold_low);
    mTask.ps_threshold_high = threshold_high;
    mTask.ps_threshold_low = threshold_low;
    stk3a5x_ps_set_threshold(mTask.ps_threshold_high, mTask.ps_threshold_low);
}

static void psGetSensorInfo(struct sensorInfo_t *data)
{
    strncpy(data->name, "stk3a5x_p", sizeof(data->name));
}

static void alsGetSensorInfo(struct sensorInfo_t *data)
{
    strncpy(data->name, "stk3a5x_l", sizeof(data->name));
}

static int stk3a5x_mask_eint(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    mt_eint_mask(mTask.hw->eint_num);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int stk3a5x_unmask_eint(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    mt_eint_unmask(mTask.hw->eint_num);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int stk3a5x_set_als_debounce_on(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                      void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                      void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    osLog(LOG_INFO, "stk3a5x_set_als_debounce_on\n");
    mTask.als_debounce_on = 1;
    cal_end_time(mTask.als_debounce_time, &mTask.als_debounce_end);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int stk3a5x_set_ps_debounce_on(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                     void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                     void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    osLog(LOG_INFO, "stk3a5x_set_ps_debounce_on\n");
    mTask.ps_debounce_on = 1;
    cal_end_time(mTask.ps_debounce_time, &mTask.ps_debounce_end);
    mTask.data[0].prox_state = PROX_STATE_INIT;
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int stk3a5x_set_als_power_on(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                   void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                   void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    osLog(LOG_INFO, "stk als power on state=0x%x\n", mTask.rxBuf[0]);         
	mTask.alsPowerOn = true;
    mTask.als_debug_count = 0;
    mTask.txBuf[0] = STK_STATE_REG;
	if(mTask.psPowerOn == true){
		mTask.txBuf[1] = STK_STATE_EN_ALS_MASK | STK_STATE_EN_PS_MASK | STK_STATE_EN_WAIT_MASK;
	}else{
		mTask.txBuf[1] = STK_STATE_EN_ALS_MASK;
	}
	
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                       i2cCallBack, next_state);
}

static int stk3a5x_set_als_power_off(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                    void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                    void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
	osLog(LOG_INFO, "stk als power off state=0x%x\n", mTask.rxBuf[0]);     
    mTask.als_debounce_on = 0;
    mTask.txBuf[0] = STK_STATE_REG;
	if(mTask.psPowerOn == true){
		mTask.txBuf[1] = STK_STATE_EN_PS_MASK | STK_STATE_EN_WAIT_MASK;
	}else{
		mTask.txBuf[1] = 0x00;
	}
	mTask.alsPowerOn = false;
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                       i2cCallBack, next_state);
}

static int stk3a5x_als_ratechg(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                              void *inBuf, uint8_t inSize, uint8_t elemInSize,
                              void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    osLog(LOG_INFO, "stk als retechg\n");
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int stk3a5x_als_cali(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                              void *inBuf, uint8_t inSize, uint8_t elemInSize,
                              void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    //int32_t alsCali[2];
    //float   alsCali_mi;

    //alsCali_mi = 0.345;
    //alsCali[0] = alsCali_mi * ALS_INCREASE_NUM_AP;
    //alsCali[1] = 0;

    //sendAlsPsCalibrationResult(SENS_TYPE_ALS, alsCali);
    osLog(LOG_INFO, "set als cali");  
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int stk3a5x_als_cfg(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                              void *inBuf, uint8_t inSize, uint8_t elemInSize,
                              void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    int ret = 0;
    struct alspsCaliCfgPacket caliCfgPacket;
    //double  alsCali_mi;

    ret = rxCaliCfgInfo(&caliCfgPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);

    if (ret < 0) {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
        osLog(LOG_ERROR, "stk3a5x_als_cfg, rx inSize and elemSize error\n");
        return -1;
    }
    //alsCali_mi = (double)((float)caliCfgPacket.caliCfgData[0] / ALS_INCREASE_NUM_AP);
    //APS_ALOGD("stk3a5x_als_cfg: [%f]\n", alsCali_mi);
    mTask.alsCali = caliCfgPacket.caliCfgData[0];
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int stk3a5x_set_ps_power_on(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                  void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                  void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    osLog(LOG_INFO, "stk ps power on state=0x%x\n", mTask.rxBuf[0]);                                 
	mTask.psPowerOn = true;
	mTask.ps_thd_update = false;
	mTask.psi_set = 0;
	mTask.psi = 0xffff;
	mTask.psa = 0;
	mTask.ps_thd_update = 0;/* Standby */
	mTask.debug_count = 0;
	mTask.start_tune = false;	
	mTask.prox_state = PROX_STATE_INIT;
	mTask.data[0].prox_state = PROX_STATE_INIT;
    mTask.txBuf[0] = STK_STATE_REG;
    mTask.ps_int_flag = false;
    mTask.pocket_threshold = 0;
    osLog(LOG_INFO, "stk_power_on:cur HT=%d, LT=%d\n", mTask.ps_threshold_high, mTask.ps_threshold_low);

	if(mTask.alsPowerOn == true){
		mTask.txBuf[1] = STK_STATE_EN_PS_MASK | STK_STATE_EN_WAIT_MASK | STK_STATE_EN_ALS_MASK;
	}else{
		mTask.txBuf[1] = STK_STATE_EN_PS_MASK | STK_STATE_EN_WAIT_MASK;
	}
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                       i2cCallBack, next_state);
}

static int stk3a5x_set_ps_power_on_set_int(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                   void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                   void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    osLog(LOG_INFO, "stk ps power on set int"); 
	
    mTask.txBuf[0] = STK_INTCTRL1_REG;
    mTask.txBuf[1] = 0x01;
	
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                       i2cCallBack, next_state);
}

static int stk3a5x_power_on_get_ps_thd(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                          void *inBuf, uint8_t inSize, uint8_t elemInSize,
                          void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    mTask.txBuf[0] = STK_THDH1_PS_REG;

    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 2, i2cCallBack,
                         next_state); 
}

static int stk3a5x_power_on_set_ps_thd(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                  void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                  void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    
    //int ret;
    char txBuf[6];
    uint32_t    ps_threshold_high;
    uint32_t    ps_threshold_low;
    uint16_t    ct_last, high_thd_last;
                         
    high_thd_last = (mTask.rxBuf[0] << 8) | mTask.rxBuf[1];
    if(high_thd_last < STK_HT_N_CT){
        if(mTask.hw->ps_threshold_high > STK_HT_N_CT){
            ct_last = mTask.hw->ps_threshold_high - STK_HT_N_CT;
        }else{
            ct_last = mTask.hw->ps_threshold_high;
        }
    }
    else{
        ct_last = high_thd_last - STK_HT_N_CT;
    }
    osLog(LOG_INFO, "stk ps power on last ht=%d, ct=%d, hw_thd_h=%d\n", high_thd_last, ct_last, mTask.hw->ps_threshold_high);
    ps_threshold_high = ct_last + STK_H_HT;
    ps_threshold_low = ct_last +  STK_H_LT;
    mTask.pocket_threshold = ct_last + STK_HT_N_CT + STK_H_HT;
    mTask.first_int_after_en = true;

	osLog(LOG_INFO, "stk ps power on set HT=%d, LT=%d, POCKET=%d\n", ps_threshold_high, ps_threshold_low, mTask.pocket_threshold); 

    txBuf[0] = STK_THDH1_PS_REG;
    txBuf[1] = (uint8_t)((ps_threshold_high & 0xFF00) >> 8);
    txBuf[2] = (uint8_t)(ps_threshold_high & 0xFF);
    txBuf[3] = (uint8_t)((ps_threshold_low & 0xFF00) >> 8);
    txBuf[4] = (uint8_t)(ps_threshold_low & 0xFF);
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, &txBuf[0], 5,
                i2cCallBack, next_state);
}
								  

#define STK_VAL_THD	1000

static int stk3a5x_compare_sun_level()
{
	int32_t lii = STK_VAL_THD;

	if((mTask.debug_count%24)==9){
		osLog(LOG_INFO, "stk psoff_data=%d, lii=%d\n", mTask.psoff_data, lii);
	}
	
	if(mTask.psoff_data > lii)	{
		return 0xFFFF;
	}
	return 0;
}


static int stk3a5x_get_flag_pre(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) 
{
    //APS_ALOGF(); 
    if(stk3a5x_is_skip()){
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
		return 0;        
    }

    mTask.txBuf[0] = STK_FLAG_REG;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 1, i2cCallBack,
                         next_state);
}

static int stk3a5x_get_flag(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
	int ps_flag;
   //f(mTask.ps_int_flag == true){  
    if(stk3a5x_is_skip()){
		sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
		return 0;
    }  	

	if(mTask.prox_state == PROX_STATE_INIT){
		ps_flag = mTask.rxBuf[0] & 0x01;
	    if (ps_flag == 1) { /* 1 is far */
	        mTask.data[0].prox_state = PROX_STATE_FAR; /* far */
	        mTask.prox_state = PROX_STATE_FAR; /* far */
	    } else if (ps_flag == 0) {
	        mTask.data[0].prox_state = PROX_STATE_NEAR; /* near */
	        mTask.prox_state = PROX_STATE_NEAR; /* near */
	    } 
	}
	if((mTask.debug_count%12) == 1){
		osLog(LOG_INFO, "stk get flag=0x%x", mTask.rxBuf[0]);
	}
	
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}


static int stk3a5x_get_ps_rawdata_pre(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) 
{
    //APS_ALOGF(); 
    if(stk3a5x_is_skip()){
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
		return 0;        
    }

    mTask.txBuf[0] = STK_DATA1_PS_REG;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 2, i2cCallBack,
                         next_state);
}

static int stk3a5x_get_ps_rawdata(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    if(stk3a5x_is_skip()){
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
		return 0;        
    }

	mTask.prox_raw_data = (mTask.rxBuf[0] << 8) | mTask.rxBuf[1];
	mTask.start_tune	= true;
	//APS_ALOGD("%d", mTask.data[0].prox_data);

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int stk3a5x_get_psoff_pre(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) 
{
    //APS_ALOGF(); 
    if(stk3a5x_is_skip()){
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
		return 0;        
    }

    mTask.txBuf[0] = 0x24;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 4, i2cCallBack,
                         next_state);
}

static int stk3a5x_get_psoff(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    if(stk3a5x_is_skip()){
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
		return 0;        
    } 

	mTask.psoff_data = 0;
	
	mTask.psoff_data = ((mTask.rxBuf[0] << 8) | mTask.rxBuf[1]);
	mTask.psoff_data += ((mTask.rxBuf[2] << 8) | mTask.rxBuf[3]);	

	//APS_ALOGD("1: 0x%x, 0x%x, 0x%x\n", mTask.rxBuf[0], mTask.rxBuf[1], mTask.rxBuf[2]);
	//APS_ALOGD("2: 0x%x, %d\n", mTask.rxBuf[3], mTask.psoff_data);

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}


static int stk3a5x_get_threshold_pre(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) 
{
    //APS_ALOGF(); 
    if(stk3a5x_is_skip()){
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
		return 0;        
    }

    if((mTask.debug_count%24) == 3){
		mTask.txBuf[0] = STK_THDH1_PS_REG;
		return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
							 mTask.rxBuf, 4, i2cCallBack,
							 next_state);
    }else{
		sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
		return 0; 

    }
}

static int stk3a5x_get_threshold(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {

	uint16_t thdh, thdl;
    if(stk3a5x_is_skip()){
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
		return 0;        
    }

	if((mTask.debug_count%24) == 3){
		thdh = ((mTask.rxBuf[0] << 8) | mTask.rxBuf[1]);
		thdl = ((mTask.rxBuf[2] << 8) | mTask.rxBuf[3]);	

		//APS_ALOGD("1: 0x%x, 0x%x, 0x%x\n", mTask.rxBuf[0], mTask.rxBuf[1], mTask.rxBuf[2]);
		//APS_ALOGD("2: 0x%x, %d, %d\n", mTask.rxBuf[3], thdh, thdl);
		osLog(LOG_INFO, "stk get3: ht:%d, lt:%d\n", thdh, thdl);
	}
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}



static int stk3a5x_get_allreg_pre(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) 
{
    //APS_ALOGF(); 
    if(stk3a5x_is_skip()){
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
		return 0;        
    }

    if((mTask.debug_count%12) == 6){
		mTask.txBuf[0] = STK_STATE_REG;
		
	    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
	                         mTask.rxBuf, 6, i2cCallBack,
	                         next_state);
    }else{
    	sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    	return 0;    
    }

}

static int stk3a5x_get_allreg(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    if(stk3a5x_is_skip()){
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
		return 0;        
    }

	if((mTask.debug_count%12) == 6){
		osLog(LOG_INFO, "stk all1: 0x%x, 0x%x, 0x%x\n", mTask.rxBuf[0], mTask.rxBuf[1], mTask.rxBuf[2]);	
		osLog(LOG_INFO, "stk all2: 0x%x, 0x%x, 0x%x\n", mTask.rxBuf[3], mTask.rxBuf[4], mTask.rxBuf[5]);	
	}
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int stk3a5x_ps_tune_start(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) 
{

    //APS_ALOGF();

	//APS_ALOGE("power=%d, first int=%d, int rest=%d", mTask.psPowerOn, mTask.first_int_after_en, mTask.first_int_reset_thd);		
    if(stk3a5x_is_skip()){
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
		return 0;        
    }

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;    
}    


static int stk3a5x_ps_tune_fae(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) 
{
    //APS_ALOGF();
    if(stk3a5x_is_skip()){
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
		return 0;        
    } 

	if(stk3a5x_compare_sun_level() != 0){
		osLog(LOG_INFO, "stk fae:At sun, psoff_data=%d\n", mTask.psoff_data);
		sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
		return 0;
	}

	if((mTask.debug_count%12)==9){
		osLog(LOG_INFO, "stk fae:ps=%d, dis=%d\n", mTask.prox_raw_data, mTask.prox_state);
	}
	if (mTask.psi_set != 0)
	{
		if (mTask.prox_state == PROX_STATE_NEAR)
		{
			if ((mTask.prox_raw_data - mTask.psi) > STK_H_HAND)
			{
				if(mTask.ps_threshold_high != (mTask.psi + STK_H_HT)){
					mTask.ps_threshold_high = mTask.psi + STK_H_HT;
					mTask.ps_threshold_low = mTask.psi + STK_H_LT;
					//mTask.psi_set = mTask.psi;
					
					stk3a5x_ps_set_threshold(mTask.ps_threshold_high, mTask.ps_threshold_low);
					
					mTask.ps_thd_update = true;

					osLog(LOG_INFO, "stk fae: near update ht=%d, lt=%d\n",
						mTask.ps_threshold_high, mTask.ps_threshold_low);
				}
			}
		}
		else
		{
			if (mTask.ps_thd_update)//reset thd
			{
				mTask.ps_thd_update = false;
				if((mTask.prox_raw_data + STK_HT_N_CT) < mTask.ps_threshold_high){
					mTask.psi = mTask.prox_raw_data;
					mTask.ps_threshold_high = mTask.psi + STK_HT_N_CT;
					mTask.ps_threshold_low = mTask.psi + STK_LT_N_CT;
										
				stk3a5x_ps_set_threshold(mTask.ps_threshold_high, mTask.ps_threshold_low);
				
					osLog(LOG_INFO, "stk fae:far update1 ht=%d, lt=%d",
						mTask.ps_threshold_high, mTask.ps_threshold_low);
				}else{
					osLog(LOG_INFO, "far update = 1 reset"); 
				}

			}
			else//Tracking
			{
				if ((mTask.prox_raw_data > 0) && (mTask.prox_raw_data < (mTask.ps_threshold_high - STK_HT_N_CT - 5)))
				{
					mTask.psi = mTask.prox_raw_data;
					mTask.ps_threshold_high = mTask.psi + STK_HT_N_CT;
					mTask.ps_threshold_low = mTask.psi + STK_LT_N_CT;
					
					stk3a5x_ps_set_threshold(mTask.ps_threshold_high, mTask.ps_threshold_low);

					osLog(LOG_INFO, "stk fae:far update2 ht=%d, lt=%d\n",
						mTask.ps_threshold_high, mTask.ps_threshold_low);

				}
			}
		}
	}
	else
	{
		if (mTask.prox_raw_data != 0)
		{
			if (mTask.prox_raw_data > mTask.psa)
			{
				mTask.psa = mTask.prox_raw_data;
				osLog(LOG_INFO, "stk fae:update psa, psa=%d,psi=%d\n", mTask.psa, mTask.psi);
			}
			if (mTask.prox_raw_data < mTask.psi)
			{
				mTask.psi = mTask.prox_raw_data; 
				osLog(LOG_INFO, "stk fae:update psi, psa=%d,psi=%d\n", mTask.psa, mTask.psi);
			}
		}

		if (mTask.psa > mTask.psi){
			if (mTask.psa - mTask.psi > STK_PS_DIFF)
			{
                if((mTask.psi + STK_HT_N_CT) > mTask.pocket_threshold){
                    osLog(LOG_INFO, "stk fae:in pocket ps=%d, thd=%d\n", mTask.prox_raw_data, mTask.pocket_threshold);
                }else{
                    mTask.ps_threshold_high = mTask.psi + STK_HT_N_CT;
                    mTask.ps_threshold_low = mTask.psi + STK_LT_N_CT;
                    mTask.psi_set = mTask.psi;

                    stk3a5x_ps_set_threshold(mTask.ps_threshold_high, mTask.ps_threshold_low);

                    osLog(LOG_INFO, "stk fae:set HT=%d, LT=%d\n", mTask.ps_threshold_high, mTask.ps_threshold_low);
                }
            }
		}
	}
	
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;    
}

static int stk3a5x_read_ps_once(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    if (rxTransferDataInfo(&mTask.dataInfo, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize)) {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "ps rx error\n");
        return -1;
    }
    mTask.txBuf[0] = STK_DATA1_PS_REG;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 2, i2cCallBack,
                         next_state);
}

static int stk3a5x_get_ps_raw_data_once(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                   void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                   void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.prox_raw_data = mTask.rxBuf[1] << 8 | mTask.rxBuf[0];
    if (mTask.prox_raw_data < mTask.psCali)
        mTask.prox_raw_data = 0;
    mTask.prox_raw_data -= mTask.psCali;
/* for the first state reporting*/
    uint32_t ps_flag = 0;
    if (mTask.prox_raw_data >= mTask.ps_threshold_high) {
        ps_flag = 1;
    } else if (mTask.prox_raw_data <= mTask.ps_threshold_low) {
        ps_flag = 0;
    }
    if (ps_flag == 0) { /* 0 is far */
        mTask.data[0].prox_state = PROX_STATE_FAR; /* far */
        mTask.data[0].prox_data = 1; /* far state distance is 1cm */
    } else {
        mTask.data[0].prox_state = PROX_STATE_NEAR; /* near */
        mTask.data[0].prox_data = 0; /* near state distance is 0cm */
    }
	osLog(LOG_INFO, "stk %s:prox_raw_data=%d, prox_state=%d", __func__,mTask.prox_raw_data,mTask.data[0].prox_state);
    mTask.data[0].sensType = SENS_TYPE_PROX;
    txTransferDataInfo(&mTask.dataInfo, 1, &mTask.data[0]);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}


#if 0
uint32_t timerHandle;  // fate psensor interrupt( using timer )
void psTimerCallback(uint32_t timerId, void *cookie) {
    //alsPsInterruptOccur();
	stk3a5x_prx_tune_zero_func_fae();
}
#endif
static int stk3a5x_set_ps_power_off(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                   void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                   void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    osLog(LOG_INFO, "stk set power off state=0x%x\n", mTask.rxBuf[0]);                                
    mTask.ps_debounce_on = 0;
    mTask.data[0].prox_state = PROX_STATE_FAR;
    mTask.prox_state = PROX_STATE_FAR; 
    mTask.txBuf[0] = STK_STATE_REG;
	
	if(mTask.alsPowerOn == true){
		mTask.txBuf[1] = STK_STATE_EN_ALS_MASK;
	}else{
		mTask.txBuf[1] = 0x00;
	}
#if 0	
    //mTask.txBuf[1] = mTask.rxBuf[0] & ~STK_STATE_EN_PS_MASK;
    if (timerHandle)
        timTimerCancel(timerHandle);
#endif	
	mTask.psPowerOn = false;
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                       i2cCallBack, next_state);
}

static int stk3a5x_set_ps_power_off_clr_int(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                   void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                   void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    osLog(LOG_INFO, "stk set power off clr int\n"); 
	
    mTask.txBuf[0] = STK_INTCTRL1_REG;
    mTask.txBuf[1] = 0x00;
	
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                       i2cCallBack, next_state);
}

static int stk3a5x_power_off_reset_thd(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                  void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                  void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    
     //int ret;
    char txBuf[6];

    osLog(LOG_INFO, "stk ps power off psi_set=%d, ht=%d, lt=%d\n", mTask.psi_set, mTask.ps_threshold_high, mTask.ps_threshold_low);

    if(mTask.psi_set == 0){
        txBuf[0] = STK_THDH1_PS_REG;
        txBuf[1] = (uint8_t)((mTask.ps_threshold_high & 0xFF00) >> 8);
        txBuf[2] = (uint8_t)(mTask.ps_threshold_high & 0xFF);
        txBuf[3] = (uint8_t)((mTask.ps_threshold_low & 0xFF00) >> 8);
        txBuf[4] = (uint8_t)(mTask.ps_threshold_low & 0xFF);
        return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, &txBuf[0], 5,
                    i2cCallBack, next_state);        
    }

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int stk3a5x_ps_ratechg(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                             void *inBuf, uint8_t inSize, uint8_t elemInSize,
                             void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    osLog(LOG_INFO, "stk ps ratechg\n");                        
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int stk3a5x_ps_cali(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                              void *inBuf, uint8_t inSize, uint8_t elemInSize,
                              void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
//    int32_t threshold[2];
    //threshold[0] = 2;  // high simulation data
    //threshold[1] = 3;  // low simulation data
    //sendAlsPsCalibrationResult(SENS_TYPE_PROX, threshold);
    osLog(LOG_INFO, "stk ps cali\n");
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int stk3a5x_ps_cfg(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                              void *inBuf, uint8_t inSize, uint8_t elemInSize,
                              void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    int ret = 0;
    struct alspsCaliCfgPacket caliCfgPacket;
	osLog(LOG_INFO, "stk ps cfg\n");  
	
    ret = rxCaliCfgInfo(&caliCfgPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);

    if (ret < 0) {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
        osLog(LOG_ERROR, "stk3a5x_ps_cfg, rx inSize and elemSize error\n");
        return -1;
    }
    osLog(LOG_INFO, "stk [high, low]: [%d, %d]\n",
        caliCfgPacket.caliCfgData[0], caliCfgPacket.caliCfgData[1]);
	if(caliCfgPacket.caliCfgData[0] != 0){
        osLog(LOG_INFO, "Use default thd for the first boot\n");
    	//stk3a5x_ps_set_threshold(caliCfgPacket.caliCfgData[0], caliCfgPacket.caliCfgData[1]);
	}
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int stk3a5x_get_alsps_state(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                               void *inBuf, uint8_t inSize, uint8_t elemInSize,
                               void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    osLog(LOG_INFO, "stk get alsps state\n");                             
    mTask.txBuf[0] = STK_STATE_REG;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 1, i2cCallBack,
                         next_state);
}

static int stk3a5x_set_wait_time(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    osLog(LOG_INFO, "stk set wait time\n");
    mTask.txBuf[0] = STK_WAIT_REG;
    mTask.txBuf[1] = STK_WAIT_RST;
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                       i2cCallBack, next_state);
}

static int stk3a5x_set_sw_reset(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                             void *inBuf, uint8_t inSize, uint8_t elemInSize,
                             void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    osLog(LOG_INFO, "stk set sw reset\n");                          
    mTask.txBuf[0] = STK_SW_RESET_REG;
    mTask.txBuf[1] = 0;
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                       i2cCallBack, next_state);
}

static int stk3a5x_sw_reset_wait(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {

	osLog(LOG_INFO, "stk sw reset wait\n");		
    mdelay(10);

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;	
}

static int stk3a5x_set_alsps_ctrl(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {

	osLog(LOG_INFO, "stk set alsps ctrl");							
    mTask.txBuf[0] = STK_STATE_REG;
    mTask.txBuf[1] = mTask.state_val;
    mTask.txBuf[2] = mTask.ps_ctrl_val;
    mTask.txBuf[3] = mTask.als_ctrl_val;
    mTask.txBuf[4] = mTask.ledctrl_val;
    mTask.txBuf[5] = STK_INT_CTRL;

    if (0 == mTask.hw->polling_mode_als)
        mTask.txBuf[5] |= 1 << STK_INT_ALS_SHIFT;

    mTask.txBuf[6] = STK_WAIT_50MS;

	mdelay(10);

    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 7,
                       i2cCallBack, next_state);
}

static int stk3a5x_set_alsctrl2(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                             void *inBuf, uint8_t inSize, uint8_t elemInSize,
                             void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    osLog(LOG_ERROR, "stk3a5x_set_alsctrl2...");                          
    mTask.txBuf[0] = STK_ALSCTRL2_REG;
    mTask.txBuf[1] = 0x20; //GAIN16
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                       i2cCallBack, next_state);
}

static int stk3a5x_set_intelli_wait(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                             void *inBuf, uint8_t inSize, uint8_t elemInSize,
                             void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    osLog(LOG_ERROR, "stk3a5x_set_intelli_wait...");                          
    mTask.txBuf[0] = STK_INTELLI_WAIT_PS_REG;
    mTask.txBuf[1] = 0x3F; //25ms
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                       i2cCallBack, next_state);
}

static int stk3a5x_set_bgir(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                             void *inBuf, uint8_t inSize, uint8_t elemInSize,
                             void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    osLog(LOG_ERROR, "stk3a5x_set_bgir...");                          
    mTask.txBuf[0] = STK_BGIR_REG;
    mTask.txBuf[1] = 0x10;
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                       i2cCallBack, next_state);
}

static int stk3a5x_set_pd(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                             void *inBuf, uint8_t inSize, uint8_t elemInSize,
                             void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    osLog(LOG_ERROR, "stk3a5x_set_pd...");                          
    mTask.txBuf[0] = STK_PD_REG;
    mTask.txBuf[1] = 0x0F|0x70; //ps0+ps1+ps2+ps3
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                       i2cCallBack, next_state);
}

static int stk3a5x_set_again(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                             void *inBuf, uint8_t inSize, uint8_t elemInSize,
                             void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    osLog(LOG_ERROR, "stk3a5x_set_again...");                          
    mTask.txBuf[0] = STK_AGAIN_REG;
    mTask.txBuf[1] = 0x01; //Als AGain*2, Ps AGain*1
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                       i2cCallBack, next_state);
}

static int stk3a5x_set_spec1(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                             void *inBuf, uint8_t inSize, uint8_t elemInSize,
                             void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    osLog(LOG_ERROR, "stk3a5x_set_spec1...");                          
    mTask.txBuf[0] = STK_SPEC1_REG;
    mTask.txBuf[1] = STK_SPEC1_VAL;
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                       i2cCallBack, next_state);
}

static int stk3a5x_set_spec2(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                             void *inBuf, uint8_t inSize, uint8_t elemInSize,
                             void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    osLog(LOG_ERROR, "stk3a5x_set_spec2...");                          
    mTask.txBuf[0] = STK_SPEC2_REG;
    mTask.txBuf[1] = STK_SPEC2_VAL;
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                       i2cCallBack, next_state);
}

static int stk3a5x_set_ps_thdl(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
	osLog(LOG_INFO, "stk set ps:thdl=%d, cali=%d, polling=%d\n", mTask.ps_threshold_low, mTask.psCali, mTask.hw->polling_mode_ps);  
    if (mTask.hw->polling_mode_ps == 0) {
        mTask.txBuf[0] = STK_THDL1_PS_REG;
        mTask.txBuf[1] = (uint8_t)(((mTask.ps_threshold_low + mTask.psCali) & 0xFF00) >> 8);
        mTask.txBuf[2] = (uint8_t)((mTask.ps_threshold_low + mTask.psCali) & 0x00FF);

        return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 3,
                           i2cCallBack, next_state);
    } else {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
        return 0;
    }
}

static int stk3a5x_set_ps_thdh(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    osLog(LOG_INFO, "stk set ps:thdh=%d, cali=%d, polling=%d\n", mTask.ps_threshold_high, mTask.psCali, mTask.hw->polling_mode_ps);                              
    if (mTask.hw->polling_mode_ps == 0) {
        mTask.txBuf[0] = STK_THDH1_PS_REG;
        mTask.txBuf[1] = (uint8_t)(((mTask.ps_threshold_high + mTask.psCali) & 0xFF00) >> 8);
        mTask.txBuf[2] = (uint8_t)((mTask.ps_threshold_high + mTask.psCali) & 0x00FF);

        return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 3,
                           i2cCallBack, next_state);
    } else {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
        return 0;
    }
}

SRAM_REGION_FUNCTION void stk3a5x_eint_handler(int arg) {
    osLog(LOG_INFO, "stk eint handler\n");
    alsPsInterruptOccur();
}

static int stk3a5x_setup_eint(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                             void *inBuf, uint8_t inSize, uint8_t elemInSize,
                             void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    osLog(LOG_INFO, "stk setup eint\n");                           
    mt_eint_dis_hw_debounce(mTask.hw->eint_num);
    mt_eint_registration(mTask.hw->eint_num, LEVEL_SENSITIVE, LOW_LEVEL_TRIGGER, stk3a5x_eint_handler, EINT_INT_UNMASK,
                         EINT_INT_AUTO_UNMASK_OFF);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int stk3a5x_register_core(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    struct sensorCoreInfo mInfo;
	int ret = 0;

    osLog(LOG_INFO, "stk register core\n");
    memset(&mInfo, 0x00, sizeof(struct sensorCoreInfo));
    /* Register sensor Core */
    mInfo.sensType = SENS_TYPE_ALS;
    mInfo.getData = alsGetData;
    mInfo.getSensorInfo = alsGetSensorInfo;
    sensorCoreRegister(&mInfo);

    memset(&mInfo, 0x00, sizeof(struct sensorCoreInfo));
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

static struct sensorFsm stk3a5xFsm[] = {
    /* sample als */
    sensorFsmCmd(STATE_SAMPLE_ALS, STATE_GET_ALS_DATA, stk3a5x_read_als_data),
    sensorFsmCmd(STATE_GET_ALS_DATA, STATE_GET_ALS_DATA01_PRE, stk3a5x_get_als_value),
    sensorFsmCmd(STATE_GET_ALS_DATA01_PRE, STATE_GET_ALS_DATA01, stk3a5x_get_als01_pre),
    sensorFsmCmd(STATE_GET_ALS_DATA01, STATE_SAMPLE_ALS_DONE, stk3a5x_get_als01),

    /* sample ps */
	sensorFsmCmd(STATE_SAMPLE_PS, STATE_GET_PS_FLG_RAW, stk3a5x_read_ps),
    sensorFsmCmd(STATE_GET_PS_FLG_RAW, STATE_GET_PS_FLG, stk3a5x_read_ps_flag),
    sensorFsmCmd(STATE_GET_PS_FLG, STATE_GET_PS_RAW_DATA, stk3a5x_get_ps_status),
    sensorFsmCmd(STATE_GET_PS_RAW_DATA, STATE_PS_TUNE_FAE, stk3a5x_get_ps_raw_data),

    /*ps tune state*/
    sensorFsmCmd(STATE_PS_TUNE_FAE, STATE_PS_TUNE_FAE_STEP1, stk3a5x_ps_tune_start),  
    sensorFsmCmd(STATE_PS_TUNE_FAE_STEP1, STATE_PS_TUNE_FAE_STEP2, stk3a5x_get_psoff_pre),
    sensorFsmCmd(STATE_PS_TUNE_FAE_STEP2, STATE_PS_TUNE_FAE_STEP3, stk3a5x_get_psoff),
    sensorFsmCmd(STATE_PS_TUNE_FAE_STEP3, STATE_PS_TUNE_FAE_STEP4, stk3a5x_get_ps_rawdata_pre),
    sensorFsmCmd(STATE_PS_TUNE_FAE_STEP4, STATE_PS_TUNE_FAE_STEP5, stk3a5x_get_ps_rawdata),
    sensorFsmCmd(STATE_PS_TUNE_FAE_STEP5, STATE_PS_TUNE_FAE_STEP6, stk3a5x_get_threshold_pre),
    sensorFsmCmd(STATE_PS_TUNE_FAE_STEP6, STATE_PS_TUNE_FAE_STEP7, stk3a5x_get_threshold),  
    sensorFsmCmd(STATE_PS_TUNE_FAE_STEP7, STATE_PS_TUNE_FAE_STEP8, stk3a5x_get_flag_pre),
    sensorFsmCmd(STATE_PS_TUNE_FAE_STEP8, STATE_PS_TUNE_FAE_STEP9, stk3a5x_get_flag),  
    sensorFsmCmd(STATE_PS_TUNE_FAE_STEP9, STATE_PS_TUNE_FAE_STEP10, stk3a5x_get_allreg_pre),
    sensorFsmCmd(STATE_PS_TUNE_FAE_STEP10, STATE_PS_TUNE_FAE_STEP11, stk3a5x_get_allreg),  
    sensorFsmCmd(STATE_PS_TUNE_FAE_STEP11, STATE_PS_TUNE_FAE_STEP12, stk3a5x_get_threshold_pre),
    sensorFsmCmd(STATE_PS_TUNE_FAE_STEP12, STATE_PS_TUNE_FAE_STEP13, stk3a5x_get_threshold),   
    sensorFsmCmd(STATE_PS_TUNE_FAE_STEP13, STATE_CLR_INT, stk3a5x_ps_tune_fae),

    sensorFsmCmd(STATE_CLR_INT, STATE_EN_EINT, stk3a5x_clr_int),
    sensorFsmCmd(STATE_EN_EINT, STATE_SAMPLE_PS_DONE, stk3a5x_enable_eint),
	/*ps sample data once, for interrupt mode*/
    sensorFsmCmd(STATE_SAMPLE_PS_ONCE, STATE_GET_PS_RAW_DATA_ONCE, stk3a5x_read_ps_once),
    sensorFsmCmd(STATE_GET_PS_RAW_DATA_ONCE, STATE_SAMPLE_PS_DONE, stk3a5x_get_ps_raw_data_once),

    /* als enable state */
    sensorFsmCmd(STATE_ALS_ENABLE, STATE_ALS_POWER_ON, stk3a5x_get_alsps_state),
    sensorFsmCmd(STATE_ALS_POWER_ON, STATE_ALS_SET_DEBOUNCE, stk3a5x_set_als_power_on),
    sensorFsmCmd(STATE_ALS_SET_DEBOUNCE, STATE_ALS_ENABLE_DONE, stk3a5x_set_als_debounce_on),

    /* als disable state */
    sensorFsmCmd(STATE_ALS_DISABLE, STATE_ALS_POWER_OFF, stk3a5x_get_alsps_state),
    sensorFsmCmd(STATE_ALS_POWER_OFF, STATE_ALS_DISABLE_DONE, stk3a5x_set_als_power_off),
    /* als cali state */
    sensorFsmCmd(STATE_ALS_CALI, CHIP_ALS_CALI_DONE, stk3a5x_als_cali),
    /* als cfg state */
    sensorFsmCmd(STATE_ALS_CFG, CHIP_ALS_CFG_DONE, stk3a5x_als_cfg),

    /* ps enable state */
    sensorFsmCmd(STATE_PS_ENABLE, STATE_PS_POWER_ON, stk3a5x_get_alsps_state),
	//sensorFsmCmd(STATE_PS_POWER_ON, STATE_PS_POWER_ON_PREPARE, stk3a5x_set_ps_power_on_prepare),
	//sensorFsmCmd(STATE_PS_POWER_ON_PREPARE, STATE_GET_STATE_REG, stk3a5x_get_alsps_state),
    sensorFsmCmd(STATE_PS_POWER_ON, STATE_PS_POWER_ON_SET_INT, stk3a5x_set_ps_power_on),
    sensorFsmCmd(STATE_PS_POWER_ON_SET_INT, STATE_PS_POWER_ON_GET_PS_THD, stk3a5x_set_ps_power_on_set_int),
    sensorFsmCmd(STATE_PS_POWER_ON_GET_PS_THD, STATE_PS_POWER_ON_SET_PS_THD, stk3a5x_power_on_get_ps_thd),
    sensorFsmCmd(STATE_PS_POWER_ON_SET_PS_THD, STATE_PS_SET_DEBOUNCE, stk3a5x_power_on_set_ps_thd),
    sensorFsmCmd(STATE_PS_SET_DEBOUNCE, STATE_PS_UNMASK_EINT, stk3a5x_set_ps_debounce_on),
    sensorFsmCmd(STATE_PS_UNMASK_EINT, STATE_PS_ENABLE_DONE, stk3a5x_unmask_eint),
    /* ps disable state */
    sensorFsmCmd(STATE_PS_DISABLE, STATE_GET_ALSPS_STATE, stk3a5x_mask_eint),
    sensorFsmCmd(STATE_GET_ALSPS_STATE, STATE_PS_POWER_OFF, stk3a5x_get_alsps_state),
    sensorFsmCmd(STATE_PS_POWER_OFF, STATE_PS_POWER_OFF_CLR_INT, stk3a5x_set_ps_power_off),
    sensorFsmCmd(STATE_PS_POWER_OFF_CLR_INT, STATE_PS_POWER_OFF_RESET_THD, stk3a5x_set_ps_power_off_clr_int),
    sensorFsmCmd(STATE_PS_POWER_OFF_RESET_THD, STATE_PS_DISABLE_DONE, stk3a5x_power_off_reset_thd),
    /* als rate change */
    sensorFsmCmd(STATE_ALS_RATECHG, STATE_ALS_RATECHG_DONE, stk3a5x_als_ratechg),
    /* ps rate change */
    sensorFsmCmd(STATE_PS_RATECHG, STATE_PS_RATECHG_DONE, stk3a5x_ps_ratechg),

    /* ps cali state */
    sensorFsmCmd(STATE_PS_CALI, CHIP_PS_CALI_DONE, stk3a5x_ps_cali),
    /* ps cfg state */
    sensorFsmCmd(STATE_PS_CFG, CHIP_PS_CFG_DONE, stk3a5x_ps_cfg),

    /* init state */
    sensorFsmCmd(STATE_RESET, STATE_SET_SW_RST, stk3a5x_set_wait_time),
    sensorFsmCmd(STATE_SET_SW_RST, STATE_SET_SW_RST_WAIT01, stk3a5x_set_sw_reset),
	sensorFsmCmd(STATE_SET_SW_RST_WAIT01, STATE_SET_SW_RST_WAIT02, stk3a5x_sw_reset_wait),
    sensorFsmCmd(STATE_SET_SW_RST_WAIT02, STATE_SET_SW_RST_WAIT03, stk3a5x_sw_reset_wait),
    sensorFsmCmd(STATE_SET_SW_RST_WAIT03, STATE_SET_SW_RST_WAIT04, stk3a5x_sw_reset_wait),
    sensorFsmCmd(STATE_SET_SW_RST_WAIT04, STATE_SET_SW_RST_WAIT05, stk3a5x_sw_reset_wait),
    sensorFsmCmd(STATE_SET_SW_RST_WAIT05, STATE_SET_ALSPS_CTRL, stk3a5x_sw_reset_wait),
    sensorFsmCmd(STATE_SET_ALSPS_CTRL, STATE_SET_ALSCTRL2, stk3a5x_set_alsps_ctrl),
    sensorFsmCmd(STATE_SET_ALSCTRL2, STATE_SET_INTELLI_WAIT, stk3a5x_set_alsctrl2),
    sensorFsmCmd(STATE_SET_INTELLI_WAIT, STATE_SET_BGIR, stk3a5x_set_intelli_wait),
    sensorFsmCmd(STATE_SET_BGIR, STATE_SET_PD, stk3a5x_set_bgir),
    sensorFsmCmd(STATE_SET_PD, STATE_SET_AGAIN, stk3a5x_set_pd),
    sensorFsmCmd(STATE_SET_AGAIN, STATE_SET_SPEC1, stk3a5x_set_again),
    sensorFsmCmd(STATE_SET_SPEC1, STATE_SET_SPEC2, stk3a5x_set_spec1),
    sensorFsmCmd(STATE_SET_SPEC2, STATE_SET_PS_THDH, stk3a5x_set_spec2),
    sensorFsmCmd(STATE_SET_PS_THDH, STATE_SET_PS_THDL, stk3a5x_set_ps_thdh),
    sensorFsmCmd(STATE_SET_PS_THDL, STATE_SETUP_EINT, stk3a5x_set_ps_thdl),
    sensorFsmCmd(STATE_SETUP_EINT, STATE_CORE, stk3a5x_setup_eint),
    sensorFsmCmd(STATE_CORE, STATE_INIT_DONE, stk3a5x_register_core),
};

static int stk3a5xInit(void) {
    int ret = 0;
    osLog(LOG_INFO, "stk init\n");
    stk3a5xDebugPoint = &mTask;

    mTask.hw = get_cust_alsps("stk3a5x");
    if (NULL == mTask.hw) {
        osLog(LOG_ERROR, "get_cust_acc_hw fail\n");
        return 0;
    }
    mTask.i2c_addr = mTask.hw->i2c_addr[0]; // Proximity Sensor STK3337-X
    mTask.als_debounce_time = 200;
    mTask.als_debounce_on = 0;
    mTask.ps_debounce_time = 200;
    mTask.ps_debounce_on = 0;
    mTask.psCali = 0;
    mTask.als_threshold_high = mTask.hw->als_threshold_high;
    mTask.als_threshold_low = mTask.hw->als_threshold_low;
    mTask.ps_threshold_high = mTask.hw->ps_threshold_high;
    mTask.ps_threshold_low = mTask.hw->ps_threshold_low;
    mTask.als_ctrl_val = STK_ALSCTRL_VAL;
    mTask.ps_ctrl_val = STK_PSCTRL_VAL;
    mTask.ledctrl_val = STK_LED_100MA;
    mTask.state_val = 0; /* Standby */
	mTask.psi_set = 0;
	mTask.psi = 0xffff;
	mTask.psa = 0;
	mTask.ps_thd_update = 0;/* Standby */
	mTask.debug_count = 0;
	mTask.start_tune = false;
    i2cMasterRequest(mTask.hw->i2c_num, I2C_SPEED);
    osLog(LOG_INFO, "stk3a5x i2c_num=%d, i2d_addr=%02x\n", mTask.hw->i2c_num, mTask.i2c_addr);
    mTask.txBuf[0] = STK_PDT_ID_REG;

    for (uint8_t i = 0; i < 3; i++) {
        ret = i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
            &mTask.deviceId, 1, NULL, NULL);
        if (ret >= 0)
            break;
		
		mdelay(10);
		osLog(LOG_ERROR, "stk3a5x i2cMasterTxRxSync fail i=%d\n", i);
    }
	
    if (ret < 0) {
        osLog(LOG_ERROR, "stk3a5x i2cMasterTxRxSync fail!!!\n");
        ret = -1;
        i2cMasterRelease(mTask.hw->i2c_num);
        goto err_out;
    }

    osLog(LOG_INFO, "stk3a5x deviceId: 0x%x\n", mTask.deviceId);
		
    alsSensorRegister();
    psSensorRegister();
    registerAlsPsDriverFsm(stk3a5xFsm, ARRAY_SIZE(stk3a5xFsm));

err_out:
    return ret;
}

#ifndef CFG_OVERLAY_INIT_SUPPORT
MODULE_DECLARE(stk3a5x, SENS_TYPE_ALS, stk3a5xInit);
#else
#include "mtk_overlay_init.h"
OVERLAY_DECLARE(stk3a5x, OVERLAY_ID_ALSPS, stk3a5xInit);
#endif
