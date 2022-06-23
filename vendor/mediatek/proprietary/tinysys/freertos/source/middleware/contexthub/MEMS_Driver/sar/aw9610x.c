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
#include <aw9610x.h>
#include <aw9610x_reg.h>
#include <timer.h>

#define SAR_NAME			"aw9610x_sar"
#define I2C_SPEED			400000
#define AW9610X_DRIVER_VERSION		"v0.1.0.3"

#define AW_READ_CHIPID_RETRIES			(10)
#define AW_I2C_RETRIES				(5)

static struct AW9610XTask {
	/* txBuf for i2c operation, fill register and fill value */
	uint8_t txBuf[8];
	uint8_t rxBuf[8];
	uint8_t sarIntStatus;

	uint8_t i2c_addr;
//	uint8_t chipId[4];
	struct sar_hw *hw;

	uint32_t timerHandle;
	/* rxBuf for i2c operation, receive rawdata */
	struct transferDataInfo dataInfo;
	struct SarData_t data;

	/* data for factory */
	struct TripleAxisDataPoint factoryData;
} mTask;

static int i2c_write(uint16_t reg_addr, uint32_t reg_data,
	I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state)
{
	mTask.txBuf[0] = (uint8_t)(reg_addr >> 8);
	mTask.txBuf[1] = (uint8_t)(reg_addr >> 0);
	mTask.txBuf[2] = (uint8_t)(reg_data >> 24);
	mTask.txBuf[3] = (uint8_t)(reg_data >> 16);
	mTask.txBuf[4] = (uint8_t)(reg_data >> 8);
	mTask.txBuf[5] = (uint8_t)(reg_data >> 0);

    i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 6,
            NULL, 0, i2cCallBack, next_state);

	return 0;
}

static int i2c_read(uint16_t reg_addr, uint32_t *reg_data,
	I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state)
{
	mTask.txBuf[0] = (uint8_t)(reg_addr >> 8);
	mTask.txBuf[1] = (uint8_t)(reg_addr >> 0);

	i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
			mTask.rxBuf, 4, i2cCallBack, next_state);

	*reg_data = ((uint32_t)mTask.rxBuf[3]) | ((uint32_t)mTask.rxBuf[2] << 8) |
			((uint32_t)mTask.rxBuf[1] << 16) | ((uint32_t)mTask.rxBuf[0] << 24);

	return 0;
}

static int32_t aw9610x_i2c_write(uint16_t reg_addr16, uint32_t reg_data32,
	I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state)
{
	int32_t ret = -1;
	uint8_t cnt = 0;

	while (cnt < AW_I2C_RETRIES) {
		ret = i2c_write(reg_addr16, reg_data32,
						i2cCallBack, spiCallBack, next_state);
		if (ret < 0)
			osLog(LOG_ERROR,
					"aw9610x write cnt = %d,error = %d", cnt, ret);
		else
			break;

		cnt++;
	}

	return ret;
}

static int32_t aw9610x_i2c_read(uint16_t reg_addr16, uint32_t *reg_data32,
	I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state)
{
	int32_t ret = -1;
	uint8_t cnt = 0;

	while (cnt < AW_I2C_RETRIES) {
		ret = i2c_read(reg_addr16, reg_data32,
				i2cCallBack, spiCallBack, next_state);
		if (ret < 0) {
			osLog(LOG_ERROR, "aw9610x i2c_read cnt=%d error=%d", cnt, ret);
		} else {
			break;
		}
		cnt++;
	}

	return ret;
}

/*
static int32_t
aw9610x_i2c_write_bits(uint16_t reg_addr16,
				        uint32_t mask, uint32_t reg_data32)
{
	uint32_t reg_val;

	aw9610x_i2c_read(reg_addr16, &reg_val);
	reg_val &= mask;
	reg_val |= reg_data32;
	aw9610x_i2c_write(reg_addr16, reg_val);

	return 0;
}
*/

static int aw9610xIrqClear(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
				void *inBuf, uint8_t inSize, uint8_t elemInSize,
				void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	uint32_t reg_val = 0;

	osLog(LOG_INFO, "%s start\n",__func__);

	aw9610x_i2c_read(REG_HOSTIRQSRC, &reg_val, i2cCallBack, spiCallBack, next_state);

	osLog(LOG_INFO, "%s REG_HOSTIRQSRC=0x%x\n",__func__, reg_val);

	return 0;
}

static int aw9610xCheckHwState(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
					void *inBuf, uint8_t inSize, uint8_t elemInSize,
					void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	uint32_t irq_reg_val = 0;

	if (rxTransferDataInfo(&mTask.dataInfo, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize)) {
		sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, ERROR_EVT);
		osLog(LOG_ERROR, "sar rx error\n");
		return -1;
	}

	aw9610x_i2c_read(REG_HOSTIRQSRC, &irq_reg_val, i2cCallBack, NULL, next_state);
	osLog(LOG_INFO, "%s REG_HOSTIRQSRC 0x%08x\n",__func__, irq_reg_val);
	if (((irq_reg_val >> 1) & 0X01) == 1) {
		mTask.sarIntStatus = 1;
	} else if (((irq_reg_val >> 2) & 0X01) == 1) {
		mTask.sarIntStatus = 0;
	}

	return 0;
}

//+Maple 437,xuyanan.wt,modify,20211030,factory ch0 and ch0diff report
//#define DEBUG_FOR_CUSTOM_M10
#define DEBUG_FOR_CUSTOM_M10_FACTORY  1
static int aw9310SampleRawData(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
				void *inBuf, uint8_t inSize, uint8_t elemInSize,
				void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	//uint32_t i = 0;
	uint32_t reg_val_temp = 0;
	int32_t reg_val = 0;
	int32_t raw_data[AW_CHANNEL_MAX + 1] = { 0 };
	uint32_t coff_data = 0;
	uint32_t coff_data_int = 0;
	uint32_t coff_data_dec = 0;
#ifdef DEBUG_FOR_CUSTOM_M10
	for (i = 0; i < 3; i++) { //get ch0 and ch1
		if (i == 2) {
			aw9610x_i2c_read(REG_DIFF_CH0, &reg_val_temp, i2cCallBack, NULL, next_state); // get ch0 diff
            osLog(LOG_INFO, "%s  ch0_diff[0] = 0x%08x\n", __func__,reg_val_temp);
		} else {
			aw9610x_i2c_read(REG_RAW_CH0 + i * 4, &reg_val_temp, NULL, NULL, NULL);
            osLog(LOG_INFO, "%s  raw_data1[%d] = 0x%08x\n", __func__, i, reg_val_temp);
		}
		osLog(LOG_INFO, "%s  raw_data1[%d] = 0x%08x\n", __func__, i, reg_val_temp);
		reg_val = (int32_t)reg_val_temp;
		reg_val /= AW_DATA_PROCESS_FACTOR;
		raw_data[i] = reg_val;

		osLog(LOG_INFO, "%s  raw_data[%d] = %d\n", __func__, i, raw_data[i]);
	}

#elif DEBUG_FOR_CUSTOM_M10_FACTORY
	//get ch0 cap
	aw9610x_i2c_read(REG_AFECFG1_CH0, &reg_val_temp, NULL, NULL, NULL);
	/* Convert to capacitance value */
		coff_data = (reg_val_temp >> 24) * 900 +
						((reg_val_temp >> 16) & 0xff) * 13;
	coff_data_int = coff_data / 1000;
	coff_data_dec = coff_data % 1000;
	raw_data[0] = coff_data_int;

	osLog(LOG_INFO, "%s  ch0 cap = 0x%08x, PARASITIC_DATA_CH = %d.%d\n", 
						__func__, reg_val_temp, coff_data_int, coff_data_dec);

	//get ch1 cap
	aw9610x_i2c_read(REG_AFECFG0_CH1, &reg_val_temp, NULL, NULL, NULL);
	/* Convert to capacitance value */
	coff_data = (reg_val_temp >> 24) * 900 +
						((reg_val_temp >> 16) & 0xff) * 13;
	coff_data_int = coff_data / 1000;
	coff_data_dec = coff_data % 1000;
	raw_data[1] = coff_data_int;
	osLog(LOG_INFO, "%s  ch1 cap = 0x%08x, PARASITIC_DATA_CH = %d.%d\n", 
						__func__, reg_val_temp, coff_data_int, coff_data_dec);

	//get ch0 diff
	aw9610x_i2c_read(REG_DIFF_CH0, &reg_val_temp, i2cCallBack, NULL, next_state);
	osLog(LOG_INFO, "%s  ch0 diff = 0x%08x\n", __func__, reg_val_temp);
	reg_val = (int32_t)reg_val_temp;
	reg_val /= AW_DATA_PROCESS_FACTOR;
	raw_data[2] = reg_val;

#else
	for (i = 0; i < AW_CHANNEL_MAX + 1; i++) {
		if (i == AW_CHANNEL_MAX) {
			aw9610x_i2c_read(REG_RAW_CH0 + i * 4, &reg_val_temp, i2cCallBack, NULL, next_state);
		} else {
			aw9610x_i2c_read(REG_RAW_CH0 + i * 4, &reg_val_temp, NULL, NULL, NULL);
		}
		osLog(LOG_INFO, "%s  raw_data1[%d] = 0x%08x\n", __func__, i, reg_val_temp);
		reg_val = (int32_t)reg_val_temp;
		reg_val /= AW_DATA_PROCESS_FACTOR;
		raw_data[i] = reg_val;

		osLog(LOG_INFO, "%s  raw_data[%d] = %d\n", __func__, i, raw_data[i]);
	}
#endif
	mTask.factoryData.ix = raw_data[0];
	mTask.factoryData.iy = raw_data[1];
	mTask.factoryData.iz = raw_data[2];


	return 0;
}


static int aw9610xConvert(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
				void *inBuf, uint8_t inSize, uint8_t elemInSize,
				void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
#ifndef AW_DEBUG_PARA
	uint8_t i = 0;
	uint8_t curr_ch_state = 0;
	uint32_t curr_status = 0;
	aw9610x_i2c_read(REG_STAT0, &curr_status, NULL, NULL, NULL);
	osLog(LOG_INFO, "%s REG_STAT0 0x%08x\n",__func__, curr_status);

	for (i = 0; i < 3 ; i++) {
		curr_ch_state = (((uint8_t)(curr_status >> (24 + i)) & 0x1)) |
				(((uint8_t)(curr_status >> (16 + i)) & 0x1) << 1) |
				(((uint8_t)(curr_status >> (8 + i)) & 0x1) << 2) |
				(((uint8_t)(curr_status >> (i)) & 0x1) << 3);

		mTask.data.sarData[i] = curr_ch_state;
		osLog(LOG_INFO, "%s curr_ch_state[%d]:%d\n",__func__, i, curr_ch_state);
	}

	if (mTask.sarIntStatus){
		mTask.data.sarState = SAR_STATE_NEAR;
	} else {
		mTask.data.sarState = SAR_STATE_FAR;
	}
#else
	uint8_t i = 0;
	uint32_t reg_val_tmp[3] = {0};
	int32_t reg_val[3] = {0};

	for (i = 0; i < AW_CHANNEL_MAX + 1; i++) {
		memset(reg_val_tmp, 0, sizeof(reg_val_tmp));

		aw9610x_i2c_read(REG_VALID_CH0 + (i * 4), &reg_val_tmp[0], NULL, NULL, NULL);
		reg_val[0] = (int32_t)reg_val_tmp[0];
		reg_val[0] /= AW_DATA_PROCESS_FACTOR;

		aw9610x_i2c_read(REG_BASELINE_CH0 + (i * 4), &reg_val_tmp[1], NULL, NULL, NULL);
		reg_val[1] = (int32_t)reg_val_tmp[1];
		reg_val[1] /= AW_DATA_PROCESS_FACTOR;

		aw9610x_i2c_read(REG_DIFF_CH0 + (i * 4), &reg_val_tmp[2], NULL, NULL, NULL);
		reg_val[2] = (int32_t)reg_val_tmp[2];
		reg_val[2] /= AW_DATA_PROCESS_FACTOR;

		if (i < 2) {
			mTask.data.sarData[i] = reg_val[2];
		}

		osLog(LOG_INFO, "%s REG_VALID_CH%d: 0x%08x, REG_BASELINE_CH%d: 0x%08x, REG_DIFF_CH%d: 0x%08x\n",
			__func__, i, reg_val_tmp[0], i, reg_val_tmp[1], i, reg_val_tmp[2]);

		osLog(LOG_INFO, "%s REG_VALID_CH%d: %d, REG_BASELINE_CH%d: %d, REG_DIFF_CH%d: %d\n",
			__func__, i, reg_val[0], i, reg_val[1], i, reg_val[2]);
	}
#endif
	
        mTask.data.sarData[0] = mTask.sarIntStatus;
        mTask.data.sarData[1] = mTask.factoryData.ix;
        mTask.data.sarData[2] = mTask.factoryData.iz;
	mTask.data.sensType = SENS_TYPE_SAR;
//-Maple 437,xuyanan.wt,modify,20211030,factory ch0 and ch0diff report	
	txTransferDataInfo(&mTask.dataInfo, 1, &mTask.data);

	sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);

	return 0;
}

static int aw9610xEnable(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
				void *inBuf, uint8_t inSize, uint8_t elemInSize,
				void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	uint32_t reg_val = 0;

//+Maple 437,xuyanan.wt,modify,20211002,seft-cali	    	
 /* enable cali */
	aw9610x_i2c_read(REG_SCANCTRL0, &reg_val, NULL, NULL, NULL);
 	aw9610x_i2c_write(REG_SCANCTRL0, (reg_val | 0x3f00), NULL, NULL, NULL);
	osLog(LOG_INFO, "%s enablecali\n",__func__);

 /* enable active */
	osLog(LOG_INFO, "%s enter\n",__func__);

	aw9610x_i2c_write(REG_CMD, AW9610X_ACTIVE_MODE, i2cCallBack, spiCallBack, next_state);
//-Maple 437,xuyanan.wt,modify,20211002,seft-cali
	return 0;
}

static int aw9610xDisable(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
				void *inBuf, uint8_t inSize, uint8_t elemInSize,
				void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	uint32_t reg_val = 0;

	osLog(LOG_INFO, "%s enter\n",__func__);

	aw9610x_i2c_read(REG_HOSTIRQSRC, &reg_val, NULL, NULL, NULL);
	aw9610x_i2c_write(REG_CMD, AW9610X_SLEEP_MODE, i2cCallBack, spiCallBack, next_state);

	return 0;
}

static int aw9610xRate(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
				void *inBuf, uint8_t inSize, uint8_t elemInSize,
				void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	int ret = 0;
	struct SarCntlPacket cntlPacket = {0};

	ret = rxControlInfo(&cntlPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);
	if (ret < 0) {
		sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, ERROR_EVT);
		osLog(LOG_ERROR, "sx9310Rate, rx inSize and elemSize error\n");
		return -1;
	}

	sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);

	return 0;
}

static int aw9610xCali(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
			void *inBuf, uint8_t inSize, uint8_t elemInSize,
			void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	osLog(LOG_INFO, "%s enter\n",__func__);
	aw9610x_i2c_write(REG_SCANCTRL0, aw9610x_reg_default[0].val, i2cCallBack, spiCallBack, next_state);

	return 0;
}

static int aw9610xReset(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
				void *inBuf, uint8_t inSize, uint8_t elemInSize,
				void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	osLog(LOG_INFO, "%s enter\n",__func__);
	aw9610x_i2c_write(REG_HOSTCTRL2, 0, i2cCallBack, spiCallBack, next_state);
	osLog(LOG_INFO, "%s end\n",__func__);

	return 0;
}

static int aw9610xDelay(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
				void *inBuf, uint8_t inSize, uint8_t elemInSize,
				void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	unsigned long long timerDelay = 200000000ull / 100;

	osLog(LOG_INFO, "%s enter\n",__func__);
	if (mTask.timerHandle) {
		timTimerCancel(mTask.timerHandle);
	}

	mTask.timerHandle = timTimerSet(timerDelay * 10, 0, 50, delayCallback, next_state, true);

	if (!mTask.timerHandle) {
		configASSERT(0);
	}

	return 0;
}

/***************************************************************************
* para loaded
****************************************************************************/
static uint32_t reg_load_cnt = 0;
static uint32_t aw9610x_reg_default_len = sizeof(aw9610x_reg_default) / sizeof(aw9610x_reg_default[0]);

static int aw9610x_para_loaded(I2cCallbackF i2cCallBack,void *next_state)
{
	uint8_t index = 0;
	uint32_t once_load_len = 0;
	int32_t cnt = 0;
	int32_t ret = 0;

	index =  reg_load_cnt * 13;
	if (reg_load_cnt == 10) {
		once_load_len = aw9610x_reg_default_len;
	} else {
		once_load_len = index + 13;
	}
	osLog(LOG_INFO, "%s,reg_load_cnt = %d, once_load_len = %d\r\n", __func__, reg_load_cnt, once_load_len);
	for (cnt = index; cnt < once_load_len; cnt++) {
		if (cnt == once_load_len - 1) {
			osLog(LOG_INFO, "%s, cnt = %d\r\n", __func__, cnt);
			aw9610x_i2c_write(aw9610x_reg_default[cnt].reg,
					aw9610x_reg_default[cnt].val,
					i2cCallBack, NULL, next_state);

		} else {
			//osLog(LOG_INFO, "%s, cnt = %d\r\n", __func__, cnt);
			aw9610x_i2c_write(aw9610x_reg_default[cnt].reg,
					aw9610x_reg_default[cnt].val,
					NULL, NULL, NULL);
		}
		if(ret < 0){
			osLog(LOG_ERROR, "%s write error! = %d\r\n", __func__, ret);
			return -1;
		}
	}
	osLog(LOG_INFO,"%s para writen completely cnt = %d\n", __func__, reg_load_cnt);

	return 0;
}

static int aw9610xRegisterInit_0(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
					void *inBuf, uint8_t inSize, uint8_t elemInSize,
					void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	aw9610x_para_loaded(i2cCallBack,next_state);
	reg_load_cnt += 1;

	return 0;
}

static int aw9610xRegisterInit_1(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
					void *inBuf, uint8_t inSize, uint8_t elemInSize,
					void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	aw9610x_para_loaded(i2cCallBack,next_state);
	reg_load_cnt += 1;

	return 0;
}

static int aw9610xRegisterInit_2(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
					void *inBuf, uint8_t inSize, uint8_t elemInSize,
					void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	aw9610x_para_loaded(i2cCallBack,next_state);
	reg_load_cnt += 1;

	return 0;
}

static int aw9610xRegisterInit_3(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
					void *inBuf, uint8_t inSize, uint8_t elemInSize,
					void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	aw9610x_para_loaded(i2cCallBack,next_state);
	reg_load_cnt += 1;

	return 0;
}

static int aw9610xRegisterInit_4(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
					void *inBuf, uint8_t inSize, uint8_t elemInSize,
					void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	aw9610x_para_loaded(i2cCallBack,next_state);
	reg_load_cnt += 1;

	return 0;
}

static int aw9610xRegisterInit_5(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
					void *inBuf, uint8_t inSize, uint8_t elemInSize,
					void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	aw9610x_para_loaded(i2cCallBack,next_state);
	reg_load_cnt += 1;

	return 0;
}

static int aw9610xRegisterInit_6(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
					void *inBuf, uint8_t inSize, uint8_t elemInSize,
					void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	aw9610x_para_loaded(i2cCallBack,next_state);
	reg_load_cnt += 1;

	return 0;
}

static int aw9610xRegisterInit_7(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
					void *inBuf, uint8_t inSize, uint8_t elemInSize,
					void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	aw9610x_para_loaded(i2cCallBack,next_state);
	reg_load_cnt += 1;

	return 0;
}

static int aw9610xRegisterInit_8(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
					void *inBuf, uint8_t inSize, uint8_t elemInSize,
					void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	aw9610x_para_loaded(i2cCallBack,next_state);
	reg_load_cnt += 1;

	return 0;
}

static int aw9610xRegisterInit_9(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
					void *inBuf, uint8_t inSize, uint8_t elemInSize,
					void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	aw9610x_para_loaded(i2cCallBack,next_state);
	reg_load_cnt += 1;

	return 0;
}

static int aw9610xRegisterInit_10(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
					void *inBuf, uint8_t inSize, uint8_t elemInSize,
					void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	aw9610x_para_loaded(i2cCallBack,next_state);
	osLog(LOG_INFO,"%s: download para ok!\n", __func__);

	return 0;
}

static void aw9610x_scan_start()
{
	aw9610x_i2c_write(REG_CMD, AW9610X_ACTIVE_MODE, NULL, NULL, NULL);
}

static void sarGetSensorInfo(struct sensorInfo_t *data)
{
	strncpy(data->name, SAR_NAME, sizeof(data->name));
}

static void sarGetData(void *sample) {
	struct TripleAxisDataPoint *tripleSample = (struct TripleAxisDataPoint *)sample;
	tripleSample->ix = mTask.factoryData.ix;
	tripleSample->iy = mTask.factoryData.iy;
	tripleSample->iz = mTask.factoryData.iz;
}

static int aw9610xScanStart(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
					void *inBuf, uint8_t inSize, uint8_t elemInSize,
					void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	osLog(LOG_INFO, "%s enter\n",__func__);
	aw9610x_scan_start();

	struct sensorCoreInfo mInfo;
	memset(&mInfo, 0x00, sizeof(struct sensorCoreInfo));
	/* Register sensor Core */
	mInfo.sensType = SENS_TYPE_SAR;
	mInfo.getData = sarGetData;
	mInfo.getSensorInfo = sarGetSensorInfo;
	sensorCoreRegister(&mInfo);

	sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);

	return 0;
}

static struct sensorFsm aw9610xFsm[] = {
	sensorFsmCmd(STATE_CHECK_INT, STATE_SAMPLE_RAW, aw9610xCheckHwState),
	sensorFsmCmd(STATE_SAMPLE_RAW, STATE_CONVERT, aw9310SampleRawData),
	sensorFsmCmd(STATE_CONVERT, STATE_SAMPLE_DONE, aw9610xConvert),

	sensorFsmCmd(STATE_ENABLE, STATE_ENABLE_DONE, aw9610xEnable),

	sensorFsmCmd(STATE_DISABLE, STATE_DISABLE_DONE, aw9610xDisable),

	sensorFsmCmd(STATE_RATECHG, STATE_RATECHG_DONE, aw9610xRate),

	sensorFsmCmd(STATE_CALI, STATE_CALI_DONE, aw9610xCali),

	sensorFsmCmd(STATE_RESET, STATE_DEALY, aw9610xReset),
	sensorFsmCmd(STATE_DEALY, STATE_IRQ_CLEAR, aw9610xDelay),
	sensorFsmCmd(STATE_IRQ_CLEAR, STATE_INIT_0, aw9610xIrqClear),
	sensorFsmCmd(STATE_INIT_0, STATE_INIT_1, aw9610xRegisterInit_0),
	sensorFsmCmd(STATE_INIT_1, STATE_INIT_2, aw9610xRegisterInit_1),
	sensorFsmCmd(STATE_INIT_2, STATE_INIT_3, aw9610xRegisterInit_2),
	sensorFsmCmd(STATE_INIT_3, STATE_INIT_4, aw9610xRegisterInit_3),
	sensorFsmCmd(STATE_INIT_4, STATE_INIT_5, aw9610xRegisterInit_4),
	sensorFsmCmd(STATE_INIT_5, STATE_INIT_6, aw9610xRegisterInit_5),
	sensorFsmCmd(STATE_INIT_6, STATE_INIT_7, aw9610xRegisterInit_6),
	sensorFsmCmd(STATE_INIT_7, STATE_INIT_8, aw9610xRegisterInit_7),
	sensorFsmCmd(STATE_INIT_8, STATE_INIT_9, aw9610xRegisterInit_8),
	sensorFsmCmd(STATE_INIT_9, STATE_INIT_10, aw9610xRegisterInit_9),
	sensorFsmCmd(STATE_INIT_10, SCAN_START, aw9610xRegisterInit_10),

	sensorFsmCmd(SCAN_START, STATE_INIT_DONE, aw9610xScanStart),
};

static int aw9610xInit(void)
{
	int ret = 0;
	uint32_t chip_id = 0;

	mTask.hw = get_cust_sar("aw9610x");
	if (NULL == mTask.hw) {
		osLog(LOG_ERROR, "aw9610x get_cust_sar fail\n");
		ret = -1;
		goto err_out;
	}

	mTask.i2c_addr = mTask.hw->i2c_addr[0];
	osLog(LOG_ERROR, "aw9610x version :%s sar i2c_num: %d, i2c_addr: 0x%x\n", AW9610X_DRIVER_VERSION, mTask.hw->i2c_num, mTask.i2c_addr);

	i2cMasterRequest(mTask.hw->i2c_num, I2C_SPEED);
	mTask.txBuf[0] = (REG_CHIP_ID >> 8) & 0xff;
	mTask.txBuf[1] = (REG_CHIP_ID >> 0) & 0xff;

	for (uint8_t i = 0; i < AW_READ_CHIPID_RETRIES; i++) {
		ret = i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
					mTask.rxBuf, 4, NULL, NULL);
		chip_id = ((uint32_t)mTask.rxBuf[3]) | ((uint32_t)mTask.rxBuf[2] << 8) |
			((uint32_t)mTask.rxBuf[1] << 16) | ((uint32_t)mTask.rxBuf[0] << 24);

		if (ret >= 0 && (chip_id >> 16) == AW9610X_CHIP_ID) {
			osLog(LOG_INFO, "aw9610x auto detect success 0x%08x\n", chip_id);
			break;
		} else {
			ret = -1;
		}
	}
	if (ret < 0) {
		ret = -1;
		osLog(LOG_ERROR, "aw9610x id fail: 0x%08x\n", chip_id);
		i2cMasterRelease(mTask.hw->i2c_num);
		goto err_out;
	}
	sarSensorRegister();
	registerSarDriverFsm(aw9610xFsm, ARRAY_SIZE(aw9610xFsm));
err_out:
	return ret;
}

#ifndef CFG_OVERLAY_INIT_SUPPORT
MODULE_DECLARE(aw9610x, SENS_TYPE_SAR, aw9610xInit);
#else
#include "mtk_overlay_init.h"
OVERLAY_DECLARE(aw9610x, OVERLAY_ID_SAR, aw9610xInit);
#endif
