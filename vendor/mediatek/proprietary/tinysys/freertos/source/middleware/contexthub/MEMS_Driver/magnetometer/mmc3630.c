/*
 * Copyright (C) 2016 The Android Open Source Project
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
#include <stdbool.h>
#include <stdint.h>
#include <seos.h>
#include <util.h>
#include <sensors.h>
#include <plat/inc/rtc.h>
#include "cache_internal.h"

#include <magnetometer.h>
#include <contexthub_core.h>
#include <cust_mag.h>
#include <vendor_fusion.h>
#include "mmc3630.h"

#define SET_INTV 150
#define I2C_SPEED 400000

#define DRIVER_VERSION "MEMSIC_V1.000"


enum MMC3630State
{

    STATE_SAMPLE = CHIP_SAMPLING,
    STATE_CONVERT = CHIP_CONVERT,
    STATE_SAMPLE_DONE = CHIP_SAMPLING_DONE,
    STATE_ENABLE = CHIP_ENABLE,
    STATE_ENABLE_DONE = CHIP_ENABLE_DONE,
    STATE_DISABLE = CHIP_DISABLE,
    STATE_DISABLE_DONE = CHIP_DISABLE_DONE,
    STATE_RATECHG = CHIP_RATECHG,
    STATE_RATECHG_DONE = CHIP_RATECHG_DONE,
    STATE_INIT_DONE = CHIP_INIT_DONE,
    STATE_IDLE = CHIP_IDLE,

    STATE_FUSE_EN = CHIP_RESET,
    STATE_FUSE_RD,
    STATE_CORE,

    STATE_CHIP_BITS,
    STATE_CHIP_TM,
    STATE_CHIP_READ_OTP,
    STATE_CHIP_SAMPLE,
    STATE_CHIP_SAMPLE_OVER,
};

SRAM_REGION_BSS static struct MMC3630Task
{
    /* txBuf for i2c operation, fill register and fill value */
    uint8_t txBuf[8];
    /* rxBuf for i2c operation, receive rawdata */
    uint8_t rxBuf[8];
    uint8_t fuse[3];

    uint8_t autoDetect[2];
    uint64_t hwSampleTime;
    struct transferDataInfo dataInfo;
    struct magDataPacket magPacket;
    /* data for factory */
    struct TripleAxisDataPoint factoryData;
    struct mag_hw *hw;
    struct sensorDriverConvert cvt;
    uint8_t i2c_addr;
    struct mag_dev_info_t mag_dev_info;
} mTask;
static struct MMC3630Task *mmc3630DebugPoint;

static unsigned short otpMatrix[3] = {1000, 1000, 1350};

SRAM_REGION_FUNCTION void mmc3630TimerCbkF(uint64_t time)
{
    mTask.hwSampleTime = time;
}
static void magGetCalibration(int32_t *cali, int32_t size)
{
}
static void magSetCalibration(int32_t *cali, int32_t size)
{
}
static void magGetData(void *sample)
{
    struct TripleAxisDataPoint *tripleSample = (struct TripleAxisDataPoint *)sample;

    tripleSample->ix = mTask.factoryData.ix;
    tripleSample->iy = mTask.factoryData.iy;
    tripleSample->iz = mTask.factoryData.iz;
}


static int mmc3630ChipBits(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = -1;
    osLog(LOG_INFO, "MMC3630: %s \n", __func__);

    mTask.txBuf[0] = MMC3630_REG_BITS;
    mTask.txBuf[1] = MMC3630_BITS_SLOW_16;
    ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                      i2cCallBack, next_state);
    return ret;
}
static int mmc3630ChipTm(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = -1;
    osLog(LOG_INFO, "MMC3630: %s \n", __func__);

    mTask.txBuf[0] = MMC3630_REG_CTRL;
    mTask.txBuf[1] = MMC3630_CTRL_TM;
    ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                      i2cCallBack, next_state);
    vTaskDelay(MMC3630_DELAY_TM);
    return ret;
}

static int mmc3630ReadOtp(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                          void *inBuf, uint8_t inSize, uint8_t elemInSize,
                          void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = -1;
    osLog(LOG_INFO, "MMC3630: %s \n", __func__);

    mTask.txBuf[0] = MMC36XX_MAG_REG_ADDR_OTP_GATE0;
    mTask.txBuf[1] = MMC36XX_MAG_OTP_OPEN0;
    ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                      NULL, NULL);

    mTask.txBuf[0] = MMC36XX_MAG_REG_ADDR_OTP_GATE1;
    mTask.txBuf[1] = MMC36XX_MAG_OTP_OPEN1;
    ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                      NULL, NULL);

    mTask.txBuf[0] = MMC36XX_MAG_REG_ADDR_OTP_GATE2;
    mTask.txBuf[1] = MMC36XX_MAG_OTP_OPEN2;
    ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                      NULL, NULL);

    mTask.txBuf[0] = MMC36XX_MAG_REG_ADDR_CTRL2;
    mTask.txBuf[1] = MMC36XX_MAG_OTP_OPEN2;
    ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                      NULL, NULL);
    mTask.txBuf[0] = MMC3630_REG_OTP;
    ret = i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                        mTask.rxBuf, 2, i2cCallBack,
                        next_state);
    return ret;
}
static void mmc3630_convert_otp(uint8_t *data)
{
    signed short stemp = 0;
    unsigned short utemp = 0;

    otpMatrix[0] = 1000;
    stemp = (signed short)(data[0] & 0x3f);
    if (stemp >= 32)
        stemp = 32 - stemp;
    otpMatrix[1] = (unsigned short)(stemp * 6 + 1000); //*0.006*1000

    stemp = (signed short)(((data[1] & 0x0f) << 2) | (data[0] & 0xc0 >> 6));
    if (stemp >= 32)
        stemp = 32 - stemp;
    utemp = (unsigned short)(stemp * 6 + 1000); //magnify 1000 times
    otpMatrix[2] = utemp + (utemp * 3) / 10 + (utemp * 30 % 100 + utemp * 5) / 100;

    osLog(LOG_INFO, " mmc3630 read otp %d %d %d ", otpMatrix[0], otpMatrix[1], otpMatrix[2]);
}
#ifdef VENDOR_EDIT
#define abs(x) (x >= 0 ? (x) : (-x))
static void mmc3630_selftest(int32_t *testResult)
{
    int temp_result = -1;
    unsigned char buf[2] = {
        0,
    };
    int32_t prev_raw[3] = {0};
    int32_t curr_raw[3] = {0};
    unsigned char buf[2] = {
        0,
    };
    unsigned char rbuf = 0;

    buf[0] = 0x0F;
    buf[1] = 0xE1;
    i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, buf, 2, NULL, NULL);

    buf[0] = 0x20;
    i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, buf, 1, &rbuf, 1, NULL, NULL);

    buf[1] = rbuf & 0xE7;
    buf[0] = 0x20;
    i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, buf, 2, NULL, NULL);
    buf[0] = MMC3630_REG_CTRL;
    buf[1] = MMC3630_CTRL_RESET;
    i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, buf, 2, NULL, NULL);
    vTaskDelay(1);
    osLog(LOG_ERROR, "%s  mmc3530 set\n", __func__);

    buf[0] = MMC36530_REG_CTRL;
    buf[1] = MMC3630_CTRL_TM;
    i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, buf, 2, NULL, NULL);
    vTaskDelay(MMC3630_DELAY_TM);

    buf[0] = MMC3630_REG_DATA;
    i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, buf, 1, mTask.rxBuf, 6, NULL, NULL);

    prev_raw[0] = (int)((uint16_t)((mTask.rxBuf[AXIS_X * 2 + 1] << 8) + (uint16_t)(mTask.rxBuf[AXIS_X * 2])));
    prev_raw[1] = (int)((uint16_t)((mTask.rxBuf[AXIS_Y * 2 + 1] << 8) + (uint16_t)(mTask.rxBuf[AXIS_Y * 2])));
    prev_raw[2] = (int)((uint16_t)((mTask.rxBuf[AXIS_Z * 2 + 1] << 8) + (uint16_t)(mTask.rxBuf[AXIS_Z * 2])));

    osLog(LOG_ERROR, "%s prev_raw: %d %d %d \n", __func__, prev_raw[0], prev_raw[1], prev_raw[2]);
    buf[0] = 0x0F;
    buf[1] = 0xE1;
    i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, buf, 2, NULL, NULL);

    buf[0] = 0x20;
    i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, buf, 1, &rbuf, 1, NULL, NULL);

    buf[1] = rbuf & 0xE7;
    buf[0] = 0x20;
    i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, buf, 2, NULL, NULL);
    buf[0] = MMC3630_REG_CTRL;
    buf[1] = MMC3630_CTRL_SET;
    i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, buf, 2, NULL, NULL);
    vTaskDelay(1);
    osLog(LOG_ERROR, "%s  mmc3630 reset\n", __func__);

    buf[0] = MMC3630_REG_CTRL;
    buf[1] = MMC3630_CTRL_TM;
    i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, buf, 2, NULL, NULL);
    vTaskDelay(MMC3630_DELAY_TM);

    buf[0] = MMC3630_REG_DATA;
    i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, buf, 1, mTask.rxBuf, 6, NULL, NULL);

    curr_raw[0] = (int)((uint16_t)((mTask.rxBuf[AXIS_X * 2 + 1] << 8) + (uint16_t)(mTask.rxBuf[AXIS_X * 2])));
    curr_raw[1] = (int)((uint16_t)((mTask.rxBuf[AXIS_Y * 2 + 1] << 8) + (uint16_t)(mTask.rxBuf[AXIS_Y * 2])));
    curr_raw[2] = (int)((uint16_t)((mTask.rxBuf[AXIS_Z * 2 + 1] << 8) + (uint16_t)(mTask.rxBuf[AXIS_Z * 2])));
    osLog(LOG_ERROR, "%s curr_raw: %d %d %d \n", __func__, curr_raw[0], curr_raw[1], curr_raw[2]);

    osLog(LOG_ERROR, "%s diff: %d %d %d \n", __func__, abs(curr_raw[0] - prev_raw[0]), abs(curr_raw[1] - prev_raw[1]), abs(curr_raw[2] - prev_raw[2]));

    if ((abs(curr_raw[0] - prev_raw[0]) > 100) || (abs(curr_raw[1] - prev_raw[1]) > 100) || (abs(curr_raw[2] - prev_raw[2]) > 100))
    {
        temp_result = 1;
        osLog(LOG_INFO, "%s : MEMSIC3630_selftest pass \n", __func__);
    }
    else
    {
        temp_result = -1;
        osLog(LOG_INFO, "%s : MEMSIC3630_selftest fail \n", __func__);
    }

    *testResult = (int32_t)temp_result;
}
#endif

static void magGetSensorInfo(struct sensorInfo_t *data)
{
    memcpy(&data->mag_dev_info, &mTask.mag_dev_info, sizeof(struct mag_dev_info_t));
}

static int mmc3630RegisterCore(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                               void *inBuf, uint8_t inSize, uint8_t elemInSize,
                               void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    struct sensorCoreInfo mInfo;

    mmc3630_convert_otp(mTask.rxBuf);

    /* Register sensor Core */
    memset(&mInfo, 0x00, sizeof(struct sensorCoreInfo));
    mInfo.sensType = SENS_TYPE_MAG;
    mInfo.getCalibration = magGetCalibration;
    mInfo.setCalibration = magSetCalibration;
    mInfo.getData = magGetData;
    mInfo.getSensorInfo = magGetSensorInfo;
    sensorCoreRegister(&mInfo);

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int mmc3630Enable(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    osLog(LOG_INFO, "%s :  MMC3630 enable mag\n", __func__);

    mTask.txBuf[0] = MMC3630_REG_CTRL;
    mTask.txBuf[1] = MMC3630_CTRL_TM;

    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2, i2cCallBack,
                       next_state);
}
static int mmc3630Disable(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                          void *inBuf, uint8_t inSize, uint8_t elemInSize,
                          void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    osLog(LOG_INFO, "%s : MMC3630 disable mag\n", __func__);

    mTask.txBuf[0] = MMC3630_REG_CTRL;
    mTask.txBuf[1] = MMC3630_CTRL_TM;

    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2, i2cCallBack,
                       next_state);
}
static int mmc3630Rate(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                       void *inBuf, uint8_t inSize, uint8_t elemInSize,
                       void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;
    struct magCntlPacket cntlPacket;

    ret = rxControlInfo(&cntlPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);
    if (ret < 0)
    {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "mmc3630Rate, rx inSize and elemSize error\n");
        return -1;
    }

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int mmc3630GetStatus(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;

    ret = rxTransferDataInfo(&mTask.dataInfo, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);
    if (ret < 0)
    {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "%s mmc3630Sample, rx dataInfo error\n", __func__);
        return -1;
    }
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}
static int mmc3630Sample(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{

    //osLog(LOG_ERROR, "%s Enter\n", __func__);
    static int read_idx = 0;
    unsigned char buf[2] = {0, 0};
    unsigned char rbuf = 0;

    mTask.txBuf[0] = MMC3630_REG_DATA;
    i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1, mTask.rxBuf, 6, NULL, NULL);
    read_idx++;

    if (!(read_idx % SET_INTV))
    {
        buf[0] = 0x0F;
        buf[1] = 0xE1;
        i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, buf, 2, NULL, NULL);

        buf[0] = 0x20;
        i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, buf, 1, &rbuf, 1, NULL, NULL);

        buf[1] = rbuf & 0xE7;
        buf[0] = 0x20;
        i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, buf, 2, NULL, NULL);

        mTask.txBuf[0] = MMC3630_REG_CTRL;
        mTask.txBuf[1] = MMC3630_CTRL_SET;

        i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2, i2cCallBack, next_state);

        read_idx = 0;
    }
    else
    {
        mTask.txBuf[0] = MMC3630_REG_CTRL;
        mTask.txBuf[1] = MMC3630_CTRL_TM;
        i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2, i2cCallBack, next_state);
    }

    return 0;
}

static int mmc3630Convert(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                          void *inBuf, uint8_t inSize, uint8_t elemInSize,
                          void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    struct magData *data = mTask.magPacket.outBuf;
    float remap_data[AXES_NUM];
    int32_t idata[AXES_NUM];
    uint64_t timestamp = 0;
    int ret = -1;
    //osLog(LOG_ERROR, "mmc3630Convert direction: %d\n", mTask.hw->direction);

    idata[AXIS_X] = (int)((uint16_t)((mTask.rxBuf[AXIS_X * 2 + 1] << 8) + (uint16_t)(mTask.rxBuf[AXIS_X * 2])));
    idata[AXIS_Y] = (int)((uint16_t)((mTask.rxBuf[AXIS_Y * 2 + 1] << 8) + (uint16_t)(mTask.rxBuf[AXIS_Y * 2])));
    idata[AXIS_Z] = (int)((uint16_t)((mTask.rxBuf[AXIS_Z * 2 + 1] << 8) + (uint16_t)(mTask.rxBuf[AXIS_Z * 2])));
    data[0].x = idata[AXIS_X];
    data[0].y = idata[AXIS_Y] - idata[AXIS_Z] + 32768;
    data[0].z = idata[AXIS_Y] + idata[AXIS_Z] - 32768;
    data[0].x -= 32768;
    data[0].y -= 32768;
    data[0].z -= 32768;

    data[0].x = data[0].x * otpMatrix[0] / 1000;
    data[0].y = data[0].y * otpMatrix[1] / 1000;
    data[0].z = data[0].z * otpMatrix[2] / 1000;
    
	remap_data[mTask.cvt.map[AXIS_X]] = mTask.cvt.sign[AXIS_X] * data[0].x;
    remap_data[mTask.cvt.map[AXIS_Y]] = mTask.cvt.sign[AXIS_Y] * data[0].y;
    remap_data[mTask.cvt.map[AXIS_Z]] = mTask.cvt.sign[AXIS_Z] * data[0].z;
    //osLog(LOG_ERROR, "%s::tt remaped: %f %f %f;;;raw: %f %f %f\n", __func__, (double)remap_data[0], (double)remap_data[1], (double)remap_data[2], (double)data[0].x, (double)data[0].y, (double)data[0].z);
    data[0].x = remap_data[AXIS_X] * 100.0f / 1024.0f;
    data[0].y = remap_data[AXIS_Y] * 100.0f / 1024.0f;
    data[0].z = remap_data[AXIS_Z] * 100.0f / 1024.0f;

    mTask.factoryData.ix = (int32_t)(data[0].x * MAGNETOMETER_INCREASE_NUM_AP * MAGNETOMETER_INCREASE_NUM_AP);
    mTask.factoryData.iy = (int32_t)(data[0].y * MAGNETOMETER_INCREASE_NUM_AP * MAGNETOMETER_INCREASE_NUM_AP);
    mTask.factoryData.iz = (int32_t)(data[0].z * MAGNETOMETER_INCREASE_NUM_AP * MAGNETOMETER_INCREASE_NUM_AP);

    timestamp = addThenRetreiveAverageMagTimeStamp(mTask.hwSampleTime);
    txTransferDataInfo(&mTask.dataInfo, 1, timestamp, data);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    //osLog(LOG_ERROR, "mmc3630Convert: hwSampleTime: %lld, timestamp = %d!\n", mTask.hwSampleTime, timestamp);
    
	return ret;
}

static int mmc3630SampleDone(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                             void *inBuf, uint8_t inSize, uint8_t elemInSize,
                             void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}
static struct sensorFsm mmc3630Fsm[] = {

    sensorFsmCmd(STATE_SAMPLE, STATE_CHIP_SAMPLE, mmc3630GetStatus),
    sensorFsmCmd(STATE_CHIP_SAMPLE, STATE_CONVERT, mmc3630Sample),
    sensorFsmCmd(STATE_CONVERT, STATE_CHIP_SAMPLE_OVER, mmc3630Convert),
    sensorFsmCmd(STATE_CHIP_SAMPLE_OVER, STATE_SAMPLE_DONE, mmc3630SampleDone),

    sensorFsmCmd(STATE_ENABLE, STATE_ENABLE_DONE, mmc3630Enable),
    sensorFsmCmd(STATE_DISABLE, STATE_DISABLE_DONE, mmc3630Disable),

    sensorFsmCmd(STATE_RATECHG, STATE_RATECHG_DONE, mmc3630Rate),

    sensorFsmCmd(STATE_FUSE_EN, STATE_CHIP_TM, mmc3630ChipBits),
    sensorFsmCmd(STATE_CHIP_TM, STATE_CHIP_READ_OTP, mmc3630ChipTm),
    sensorFsmCmd(STATE_CHIP_READ_OTP, STATE_CORE, mmc3630ReadOtp),
    sensorFsmCmd(STATE_CORE, STATE_INIT_DONE, mmc3630RegisterCore),
};

static int mmc3630Init(void)
{
    int ret = 0;

    mmc3630DebugPoint = &mTask;
    insertMagicNum(&mTask.magPacket);
    osLog(LOG_ERROR, "driver version %s\n", DRIVER_VERSION);
    mTask.hw = get_cust_mag("mmc3630");
    if (NULL == mTask.hw)
    {
        osLog(LOG_ERROR, "mmc3630 get_cust_mag fail\n");
        ret = -1;
        goto err_out;
    }
    mTask.i2c_addr = mTask.hw->i2c_addr[0];
    osLog(LOG_ERROR, "mag i2c_num: %d, i2c_addr: 0x%x\n", mTask.hw->i2c_num, mTask.i2c_addr);

    if (0 != (ret = sensorDriverGetConvert(mTask.hw->direction, &mTask.cvt)))
    {
        osLog(LOG_ERROR, "invalid direction: %d\n", mTask.hw->direction);
    }
    osLog(LOG_ERROR, "mag map[0]:%d, map[1]:%d, map[2]:%d, sign[0]:%d, sign[1]:%d, sign[2]:%d\n\r",
          mTask.cvt.map[AXIS_X], mTask.cvt.map[AXIS_Y], mTask.cvt.map[AXIS_Z],
          mTask.cvt.sign[AXIS_X], mTask.cvt.sign[AXIS_Y], mTask.cvt.sign[AXIS_Z]);

    i2cMasterRequest(mTask.hw->i2c_num, I2C_SPEED);

	//read 0x2fh
    mTask.txBuf[0] = MMC3630_REG_PID;
    ret = i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                            (uint8_t *)mTask.autoDetect, 1, NULL, NULL);
    if (ret < 0)
    {
        osLog(LOG_ERROR, "mmc3630 read 0x2f register i2cMasterTxRxSync fail!!!\n");
        ret = -1;
        i2cMasterRelease(mTask.hw->i2c_num);
        goto err_out;
    }
   //read 0x39h

    mTask.txBuf[0] = 0x39;
    ret = i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1, (uint8_t *)&mTask.autoDetect[1], 1, NULL, NULL);
    if (ret < 0)
    {
        osLog(LOG_ERROR, "mmc3630 read 0x39h register i2cMasterTxRxSync fail!!!\n");
        ret = -1;
        i2cMasterRelease(mTask.hw->i2c_num);
        goto err_out;
    }
   
   // if (mTask.autoDetect[0] == MMC3630_DEVICE_ID)	
    if((mTask.autoDetect[1] !=0x10)&&(mTask.autoDetect[0]==MMC3630_DEVICE_ID))
    { 
        osLog(LOG_ERROR, "%s read id:0x%x 0x%x suceess!!!\n", __func__, mTask.autoDetect[0],mTask.autoDetect[1]);
        osLog(LOG_INFO, "mmc3630: auto detect success\n");

        mTask.mag_dev_info.layout = 0x00;
        mTask.mag_dev_info.deviceid = 0x00;
        strncpy(mTask.mag_dev_info.libname, "memsic", sizeof(mTask.mag_dev_info.libname));
        magSensorRegister();
        magRegisterInterruptMode(MAG_UNFIFO);
        registerMagDriverFsm(mmc3630Fsm, ARRAY_SIZE(mmc3630Fsm));

        registerMagTimerCbk(mmc3630TimerCbkF);
    }
    else
    {
        i2cMasterRelease(mTask.hw->i2c_num);
        osLog(LOG_ERROR, "mmc3630 read id fail!!!\n");
        ret = -1;
        goto err_out;
    }
    osLog(LOG_INFO, "mmc3630: init end\n");
err_out:
    osLog(LOG_INFO, "mmc3630: init error, %d\n", ret);
    return ret;
}
#ifndef CFG_OVERLAY_INIT_SUPPORT
MODULE_DECLARE(mmc3630, SENS_TYPE_MAG, mmc3630Init);
#else
#include "mtk_overlay_init.h"
OVERLAY_DECLARE(mmc3630, OVERLAY_WORK_01, mmc3630Init);
#endif

