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

#include <barometer.h>
#include <contexthub_core.h>
#include <cust_baro.h>
#include <plat/inc/rtc.h>

#define BARO_NAME                       "bmp380"
#define BOSCH_BMP380_ID                 0x50

#define BOSCH_BMP380_REG_RESET          0x7e
#define BOSCH_BMP380_REG_DIG_T1         0x31
#define BOSCH_BMP380_REG_ID             0x00
#define BOSCH_BMP380_REG_CTRL_ODR       0x1d  //Control the Output Data Rate
#define BOSCH_BMP380_REG_CTRL_OSR       0x1c  //Control the OverSampling
#define BOSCH_BMP380_REG_CTRL_PWR       0x1b
#define BOSCH_BMP380_REG_CONFIG         0x1f  //iir filter coefficents
#define BOSCH_BMP380_REG_PRESS_LSB      0x04
#define BOSCH_BMP380_REG_FIFO_WTM_1     0x16
#define BOSCH_BMP380_REG_FIFO_WTM_0     0x15
#define BOSCH_BMP380_REG_FIFO_CONFIG_1  0x17
#define BOSCH_BMP380_REG_FIFO_CONFIG_2  0x18

#define I2C_SPEED 400000
#define UNIT_MS_TO_NS 1000000
#define UNIT_HPA_TO_PA 100
#define UNIT_PA_TO_HPA 0.01
#define MIN_DELAY_TIME (1 * UNIT_MS_TO_NS)
#define MAXIMUM_TOLERENCE_ERROR 5000 //vendor recommend 1000 here.
#define REQUIRED_DATA_NUMBER 100
//#define BOSCH_BMP380_DEBUG

enum BMP380State {
    STATE_SAMPLE = CHIP_SAMPLING,
    STATE_CONVERT = CHIP_CONVERT,
    STATE_SAMPLE_DONE = CHIP_SAMPLING_DONE,
    STATE_ENABLE = CHIP_ENABLE,
    STATE_ENABLE_DONE = CHIP_ENABLE_DONE,
    STATE_DISABLE = CHIP_DISABLE,
    STATE_DISABLE_DONE = CHIP_DISABLE_DONE,
    STATE_RATECHG = CHIP_RATECHG,
    STATE_RATECHG_DONE = CHIP_RATECHG_DONE,
    STATE_CALIBRATE = CHIP_CALIBRATE,
    STATE_CALIBRATE_DONE = CHIP_CALIBRATE_DONE,
    STATE_CFGCALI = CHIP_CFGCALI,
    STATE_CFGCALI_DONE = CHIP_CFGCALI_DONE,
    STATE_INIT_DONE = CHIP_INIT_DONE,
    STATE_IDLE = CHIP_IDLE,
    STATE_RESET = CHIP_RESET,
    STATE_VERIFY_ID,
    STATE_AWAITING_COMP_PARAMS,
    STATE_MEAS,
    STATE_CORE,
};

struct BMP380CompParams {
    uint16_t parT1;
    uint16_t parT2;
    int8_t   parT3;

    int16_t  parP1;
    int16_t  parP2;
    int8_t   parP3;
    int8_t   parP4;
    uint16_t parP5;
    uint16_t parP6;
    int8_t   parP7;
    int8_t   parP8;
    int16_t  parP9;
    int8_t   parP10;
    int8_t   parP11;
    int64_t  tLin;
} __attribute__((packed));

#ifdef BMP3_DOUBLE_PRECISION_COMPENSATION
struct bmp3_quantized_calib_data {
    double parT1;
    double parT2;
    double parT3;
    double parP1;
    double parP2;
    double parP3;
    double parP4;
    double parP5;
    double parP6;
    double parP7;
    double parP8;
    double parP9;
    double parP10;
    double parP11;
    double tLin;
} __attribute__((packed));
#endif

SRAM_REGION_BSS static struct BMP380Task {
    /* i2c operation read some data which is only use in prative driver */
    struct BMP380CompParams comp;
#ifdef BMP3_DOUBLE_PRECISION_COMPENSATION
    struct bmp3_quantized_calib_data quantizedComp;
#endif
    /* txBuf for i2c operation, fill register and fill value */
    uint8_t txBuf[8];
    /* rxBuf for i2c operation, receive rawdata */
    struct transferDataInfo dataInfo;
    struct baroDataPacket baroPacket;
    uint8_t rxBuf[8];
    uint8_t verifyId;
    struct baro_hw *hw;
    uint8_t i2cAddr;
    /* data for factory */
    struct SingleAxisDataPoint factoryData;
    bool startCali;
    int32_t referenceValuePa;
    int32_t staticCaliPa;
    bool debounceOn;
    uint64_t debounceTimeNs;
    uint64_t endDebounceTimeNs;
} mTask;
static struct BMP380Task *bmp380DebugPoint;

#ifdef BMP3_DOUBLE_PRECISION_COMPENSATION
static double bmp3Pow(uint8_t base, uint8_t power)
{
    double pow_output = 1;

    while (power != 0) {
        pow_output = base * pow_output;
        power--;
    }

    return pow_output;
}

static void quantizeCalibData()
{
    double tempVar;

    tempVar = 0.00390625;
    mTask.quantizedComp.parT1 = ((double)mTask.comp.parT1 / tempVar);

    tempVar = 1073741824.0;
    mTask.quantizedComp.parT2 = ((double)mTask.comp.parT2 / tempVar);

    tempVar = 281474976710656.0;
    mTask.quantizedComp.parT3 = ((double)mTask.comp.parT3 / tempVar);

    tempVar = 1048576.0;
    mTask.quantizedComp.parP1 = ((double)(mTask.comp.parP1 - (16384)) / tempVar);

    tempVar = 536870912.0;
    mTask.quantizedComp.parP2 = ((double)(mTask.comp.parP2 - (16384)) / tempVar);

    tempVar = 4294967296.0;
    mTask.quantizedComp.parP3 = ((double)mTask.comp.parP3 / tempVar);

    tempVar = 137438953472.0;
    mTask.quantizedComp.parP4 = ((double)mTask.comp.parP4 / tempVar);

    tempVar = 0.125;
    mTask.quantizedComp.parP5 = ((double)mTask.comp.parP5 / tempVar);

    tempVar = 64.0f;
    mTask.quantizedComp.parP6 = ((double)mTask.comp.parP6 / tempVar);

    tempVar = 256.0;
    mTask.quantizedComp.parP7 = ((double)mTask.comp.parP7 / tempVar);

    tempVar = 32768.0;
    mTask.quantizedComp.parP8 = ((double)mTask.comp.parP8 / tempVar);

    tempVar = 281474976710656.0;
    mTask.quantizedComp.parP9 = ((double)mTask.comp.parP9 / tempVar);

    tempVar = 281474976710656.0;
    mTask.quantizedComp.parP10 = ((double)mTask.comp.parP10 / tempVar);

    tempVar = 36893488147419103232.0;
    mTask.quantizedComp.parP11 = ((double)mTask.comp.parP11 / tempVar);

}

/**
 * compensateTemp - double precision temp compensaztion API
 * @uncompTemp: temp value read from register directly
 *
 * Returns compensated temp value in centigrade.
*/
static double compensateTemp(uint32_t uncompTemp)
{
    double partialData1;
    double partialData2;

    partialData1 = (double)(uncompTemp - mTask.quantizedComp.parT1);
    partialData2 = (double)(partialData1 * mTask.quantizedComp.parT2);
    mTask.quantizedComp.tLin = partialData2 + (partialData1 * partialData1)
        * mTask.quantizedComp.parT3;

    //return tempeature in the unit of centigrade.
    return mTask.quantizedComp.tLin;
}

/**
 * compensateBaro - double precision press compensaztion API
 * @uncompTemp: press value read from register directly
 *
 * Returns compensated press value in Pa.
*/
static double compensateBaro(uint32_t uncompPress)
{
    double compPress;

    double partialData1;
    double partialData2;
    double partialData3;
    double partialData4;
    double partialOut1;
    double partialOut2;

    partialData1 = mTask.quantizedComp.parP6 * mTask.quantizedComp.tLin;
    partialData2 = mTask.quantizedComp.parP7 * bmp3Pow(mTask.quantizedComp.tLin, 2);
    partialData3 = mTask.quantizedComp.parP8 * bmp3Pow(mTask.quantizedComp.tLin, 3);
    partialOut1 = mTask.quantizedComp.parP5 + partialData1 + partialData2 + partialData3;

    partialData1 = mTask.quantizedComp.parP2 * mTask.quantizedComp.tLin;
    partialData2 = mTask.quantizedComp.parP3 * bmp3Pow(mTask.quantizedComp.tLin, 2);
    partialData3 = mTask.quantizedComp.parP4 * bmp3Pow(mTask.quantizedComp.tLin, 3);
    partialOut2 = uncompPress *
        (mTask.quantizedComp.parP1 + partialData1 + partialData2 + partialData3);

    partialData1 = bmp3Pow((double)uncompPress, 2);
    partialData2 = mTask.quantizedComp.parP9 + mTask.quantizedComp.parP10 * mTask.quantizedComp.tLin;
    partialData3 = partialData1 * partialData2;
    partialData4 = partialData3 + bmp3Pow((double)uncompPress, 3) * mTask.quantizedComp.parP11;
    compPress = partialOut1 + partialOut2 + partialData4;

    //return the press in the unit of Pa.
    return compPress;
}

#else

/**
 * compensateTemp - int precision temp compensaztion API
 * @uncompTemp: temp value read from register directly
 *
 * Returns compensated temp value in 0.01 centigrade.
*/
static int64_t compensateTemp(uint32_t uncompTemp)
{
    uint64_t partialData1;
    uint64_t partialData2;
    uint64_t partialData3;
    int64_t partialData4;
    int64_t partialData5;
    int64_t partialData6;
    int64_t comp_temp;

    partialData1 = uncompTemp - (256 * mTask.comp.parT1);
    partialData2 = mTask.comp.parT2 * partialData1;
    partialData3 = partialData1 * partialData1;
    partialData4 = (int64_t)partialData3 * mTask.comp.parT3;
    partialData5 = ((int64_t)(partialData2 * 262144) + partialData4);
    partialData6 = partialData5 / 4294967296;
    mTask.comp.tLin = partialData6;
    comp_temp = (int64_t)((partialData6 * 25)  / 16384);

    //return the tempeature in the unit of 0.01 centigrade.
    return comp_temp;

}

/**
 * compensateBaro - int precision press compensaztion API
 * @uncompTemp: press value read from register directly
 *
 * Returns compensated press value in 0.01 Pa.
*/
static int64_t compensateBaro(uint32_t uncompPress)
{

    int64_t partialData1;
    int64_t partialData2;
    int64_t partialData3;
    int64_t partialData4;
    int64_t partialData5;
    int64_t partialData6;
    int64_t offset;
    int64_t sensitivity;
    uint64_t compPress;

    partialData1 = mTask.comp.tLin * mTask.comp.tLin;
    partialData2 = partialData1 / 64;
    partialData3 = (partialData2 * mTask.comp.tLin) / 256;
    partialData4 = (mTask.comp.parP8 * partialData3) / 32;
    partialData5 = (mTask.comp.parP7 * partialData1) * 16;
    partialData6 = (mTask.comp.parP6 * mTask.comp.tLin) * 4194304;
    offset = (mTask.comp.parP5 * 140737488355328) + partialData4 + partialData5 + partialData6;

    partialData2 = (mTask.comp.parP4 * partialData3) / 32;
    partialData4 = (mTask.comp.parP3 * partialData1) * 4;
    partialData5 = (mTask.comp.parP2 - 16384) * mTask.comp.tLin * 2097152;
    sensitivity = ((mTask.comp.parP1 - 16384) * 70368744177664) + partialData2 + partialData4
        + partialData5;

    partialData1 = (sensitivity / 16777216) * uncompPress;
    partialData2 = mTask.comp.parP10 * mTask.comp.tLin;
    partialData3 = partialData2 + (65536 * mTask.comp.parP9);
    partialData4 = (partialData3 * uncompPress) / 8192;
    partialData5 = (partialData4 / 10 * uncompPress) / 512 * 10;
    partialData6 = (int64_t)((uint64_t)uncompPress * (uint64_t)uncompPress);
    partialData2 = (mTask.comp.parP11 * partialData6) / 65536;
    partialData3 = (partialData2 * uncompPress) / 128;
    partialData4 = (offset / 4) + partialData1 + partialData5 + partialData3;
    compPress = (((uint64_t)partialData4 * 25) / (uint64_t)1099511627776);

    //return the press in the unit of the 0.01 Pa.
    return compPress;
}
#endif

static void getTempAndBaro(const uint8_t *tmp, float *pressurehPa,
    float *temp_centigrade)
{
    uint32_t pressAdc = ((uint32_t)tmp[2] << 16) | ((uint32_t)tmp[1] << 8) | ((uint32_t)tmp[0]);
    uint32_t tempAdc = ((uint32_t)tmp[5] << 16) | ((uint32_t)tmp[4] << 8) | ((uint32_t)tmp[3]);

#ifdef BMP3_DOUBLE_PRECISION_COMPENSATION
    double temp = compensateTemp(tempAdc);
    double press = compensateBaro(pressAdc);
    *temp_centigrade = temp;//convert to centigrade
    *pressurehPa = press / 100; //convert to hPa
#else
    int64_t temp = compensateTemp(tempAdc);
    int64_t press = compensateBaro(pressAdc);
    *temp_centigrade = (float)temp * 0.01f;//convert to centigrade
    *pressurehPa = (float)press * 0.0001f; //convert to hPa
#endif

#ifdef BOSCH_BMP380_DEBUG
    osLog(LOG_INFO, "bmp380: regdata: press:[%u,%u,%u]\n", tmp[0],  tmp[1], tmp[2]);
    osLog(LOG_INFO, "bmp380: regdata: temp:[%u,%u,%u]\n", tmp[3],  tmp[4], tmp[5]);
    osLog(LOG_INFO, "bmp380: temp_centigrade:%f centigrade\n", (double)*temp_centigrade);
    osLog(LOG_INFO, "bmp380: pressurehPa:%f hPa\n", (double)*pressurehPa);
#endif
}

static int bmp380CaliDebounce(uint64_t sampleTimeNs)
{
    bool debounce_ready = false;

    if (mTask.debounceOn) {
        mTask.endDebounceTimeNs = sampleTimeNs + mTask.debounceTimeNs;
        mTask.debounceOn = false;
    } else
        debounce_ready = sampleTimeNs > mTask.endDebounceTimeNs;
    return debounce_ready;
}

static void bmp380StaticCali(float pressurehPa, uint64_t sampleTimeNs)
{
    static float sumValuehPa = 0.0f, averageValuePa = 0.0f;
    static uint8_t inputCount = 0;
    int32_t tmpCali = 0;
    int32_t baroCali[2] = {0};

    if (!bmp380CaliDebounce(sampleTimeNs))
        return;

    inputCount++;
    if (inputCount <= REQUIRED_DATA_NUMBER) {
        sumValuehPa += pressurehPa;
        return;
    } else {
        mTask.startCali = false;
        inputCount = 0;
        averageValuePa = (sumValuehPa * UNIT_HPA_TO_PA / REQUIRED_DATA_NUMBER);
        tmpCali = (float)mTask.referenceValuePa - averageValuePa;
        sumValuehPa = 0.0f;

        if (abs(tmpCali) > MAXIMUM_TOLERENCE_ERROR)
            osLog(LOG_ERROR, "bmp380 cali fail:[%d,%d]\n",
                (int32_t)averageValuePa, (int32_t)tmpCali);
        else {
            baroCali[0] = mTask.referenceValuePa;
            baroCali[1] = mTask.staticCaliPa = (int32_t)tmpCali;
            baroSendCalibrationResult(SENS_TYPE_BARO, baroCali);
            osLog(LOG_INFO, "bmp380 cali success:[%d,%d]\n",
                baroCali[0], baroCali[1]);
        }
    }
}

static void baroGetCalibration(int32_t *cali, int32_t size)
{

}

static void baroSetCalibration(int32_t *cali, int32_t size)
{

}

static void baroGetData(void *sample)
{
    struct SingleAxisDataPoint *singleSample = (struct SingleAxisDataPoint *)sample;
    singleSample->idata = mTask.factoryData.idata;
}

static void bmp380GetSensorInfo(struct sensorInfo_t *data)
{
    strncpy(data->name, BARO_NAME, sizeof(data->name));
}

static int bmp380Reset(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.txBuf[0] = BOSCH_BMP380_REG_RESET;
    mTask.txBuf[1] = 0xB6;
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2cAddr, mTask.txBuf, 2,
                       i2cCallBack, next_state);
}
static int bmp380VerifyId(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    vTaskDelay(2);
    mTask.txBuf[0] = BOSCH_BMP380_REG_ID;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2cAddr, mTask.txBuf, 1,
                         (uint8_t *)&mTask.verifyId, 1, i2cCallBack,
                         next_state);
}
static int bmp380CompParams(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;

    if (mTask.verifyId != BOSCH_BMP380_ID) {
        osLog(LOG_INFO, "BMP380: not detected\n");
        return -1;
    }

    /* Get compensation parameters */
    mTask.txBuf[0] = BOSCH_BMP380_REG_DIG_T1;
    ret = i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2cAddr, (uint8_t*)&mTask.txBuf[0], 1,
                        (uint8_t*)&mTask.comp, 8, i2cCallBack,
                        (void *)STATE_IDLE);
    mTask.txBuf[1] = BOSCH_BMP380_REG_DIG_T1 + 8;
    ret = i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2cAddr, (uint8_t*)&mTask.txBuf[1], 1,
                        (uint8_t*)((uint8_t*)&mTask.comp + 8), 8, i2cCallBack,
                        (void *)STATE_IDLE);
    mTask.txBuf[2] = BOSCH_BMP380_REG_DIG_T1 + 16;
    ret = i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2cAddr, (uint8_t*)&mTask.txBuf[2], 1,
                        (uint8_t*)((uint8_t*)&mTask.comp + 16), 5, i2cCallBack,
                        next_state);
    return ret;
}

static int bmp380Meas(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
#ifdef BMP3_DOUBLE_PRECISION_COMPENSATION
      quantizeCalibData();
#endif

    mTask.txBuf[0] = BOSCH_BMP380_REG_CTRL_OSR;
    mTask.txBuf[1] = (4 << 3) | (1 << 0);
    mTask.txBuf[2] = BOSCH_BMP380_REG_CTRL_ODR;
    mTask.txBuf[3] = 4;
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2cAddr,
        mTask.txBuf, 4, i2cCallBack, next_state);
}

static int bmp380RegisterCore(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    struct sensorCoreInfo mInfo;
    memset(&mInfo, 0x00, sizeof(struct sensorCoreInfo));
    /* Register sensor Core */
    mInfo.sensType = SENS_TYPE_BARO;
    mInfo.getCalibration = baroGetCalibration;
    mInfo.setCalibration = baroSetCalibration;
    mInfo.getData = baroGetData;
    mInfo.getSensorInfo = bmp380GetSensorInfo;
    sensorCoreRegister(&mInfo);

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int bmp380EnableBaro(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.txBuf[0] = BOSCH_BMP380_REG_CTRL_PWR;
    //power mode:normol, tmp: on, press: on
    mTask.txBuf[1] = (3 << 4) | (1 << 1) | 1 << 0;
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2cAddr, mTask.txBuf, 2, i2cCallBack,
                       next_state);
}

static int bmp380DisableBaro(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.txBuf[0] = BOSCH_BMP380_REG_CTRL_PWR;
    mTask.txBuf[1] = (2 << 5) | (5 << 2) | 0;
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2cAddr, mTask.txBuf, 2, i2cCallBack,
                       next_state);
}

static int bmp380Rate(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;
    struct baroCntlPacket cntlPacket;
    ret = rxControlInfo(&cntlPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);
    if (ret < 0) {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "bmp380Rate, rx inSize and elemSize error\n");
        return -1;
    }
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int bmp380Calibrate(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                              void *inBuf, uint8_t inSize, uint8_t elemInSize,
                              void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.startCali = true;
    mTask.debounceOn = true;
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int bmp380CfgCali(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                              void *inBuf, uint8_t inSize, uint8_t elemInSize,
                              void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;
    int32_t baroCali[2] = {0};
    struct baroCaliCfgPacket caliCfgPacket;

    ret = rxCaliCfgInfo(&caliCfgPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);

    if (ret < 0) {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "%s, rx inSize and elemSize error\n", __func__);
        return -1;
    }

    if (caliCfgPacket.caliCfgData[0] != 0 && caliCfgPacket.caliCfgData[1] != 0) {
        baroCali[0] = mTask.referenceValuePa = caliCfgPacket.caliCfgData[0];
        baroCali[1] = mTask.staticCaliPa = caliCfgPacket.caliCfgData[1];
        baroSendCalibrationResult(SENS_TYPE_BARO, baroCali);
    } else if (caliCfgPacket.caliCfgData[0] != 0)
          mTask.referenceValuePa = caliCfgPacket.caliCfgData[0];

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int bmp380Sample(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;
    ret = rxTransferDataInfo(&mTask.dataInfo, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);
    if (ret < 0) {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "bmp380Sample, rx dataInfo error\n");
        return -1;
    }
    mTask.txBuf[0] = BOSCH_BMP380_REG_PRESS_LSB;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2cAddr, mTask.txBuf, 1,
                         mTask.rxBuf, 6, i2cCallBack,
                         next_state);

}
static int bmp380Convert(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    struct baroData *data = mTask.baroPacket.outBuf;
    float pressurehPa, temp_centigrade;
    uint64_t sampleTime = rtcGetTime();

    getTempAndBaro(mTask.rxBuf, &pressurehPa, &temp_centigrade);
    mTask.factoryData.idata = (uint32_t)(pressurehPa * BAROMETER_INCREASE_NUM_AP);
    if (UNLIKELY(mTask.startCali))
        bmp380StaticCali(pressurehPa, sampleTime);

    data[0].sensType = SENS_TYPE_BARO;
    data[0].fdata = pressurehPa + (float)mTask.staticCaliPa * UNIT_PA_TO_HPA;
    txTransferDataInfo(&mTask.dataInfo, 1, data);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static struct sensorFsm bmp380Fsm[] = {
    sensorFsmCmd(STATE_SAMPLE, STATE_CONVERT, bmp380Sample),
    sensorFsmCmd(STATE_CONVERT, STATE_SAMPLE_DONE, bmp380Convert),

    sensorFsmCmd(STATE_ENABLE, STATE_ENABLE_DONE, bmp380EnableBaro),

    sensorFsmCmd(STATE_DISABLE, STATE_DISABLE_DONE, bmp380DisableBaro),

    sensorFsmCmd(STATE_RATECHG, STATE_RATECHG_DONE, bmp380Rate),

    sensorFsmCmd(STATE_CALIBRATE, CHIP_CALIBRATE_DONE, bmp380Calibrate),

    sensorFsmCmd(STATE_CFGCALI, CHIP_CFGCALI_DONE, bmp380CfgCali),

    sensorFsmCmd(STATE_RESET, STATE_VERIFY_ID, bmp380Reset),
    sensorFsmCmd(STATE_VERIFY_ID, STATE_AWAITING_COMP_PARAMS, bmp380VerifyId),
    sensorFsmCmd(STATE_AWAITING_COMP_PARAMS, STATE_MEAS, bmp380CompParams),
    sensorFsmCmd(STATE_MEAS, STATE_CORE, bmp380Meas),
    sensorFsmCmd(STATE_CORE, STATE_INIT_DONE, bmp380RegisterCore),
};

static int bmp380Init(void)
{
    int ret = 0;
    bmp380DebugPoint = &mTask;

    mTask.hw = get_cust_baro("bmp380");
    if (NULL == mTask.hw) {
        osLog(LOG_ERROR, "get_cust_baro_hw fail\n\r");
        ret = -1;
        goto err_out;
    }

    mTask.i2cAddr = mTask.hw->i2c_addr[0];
    osLog(LOG_ERROR, "i2c_num: %d, i2cAddr: 0x%x\n\r", mTask.hw->i2c_num, mTask.i2cAddr);
    i2cMasterRequest(mTask.hw->i2c_num, I2C_SPEED);
    mTask.txBuf[0] = BOSCH_BMP380_REG_ID;

    for (uint8_t i = 0; i < 3; i++) {
        ret = i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2cAddr, mTask.txBuf, 1,
            (uint8_t *)&mTask.verifyId, 1, NULL, NULL);
        if (ret >= 0)
            break;
    }
    if (ret < 0) {
        ret = -1;
        sendSensorErrToAp(ERR_SENSOR_BARO, ERR_CASE_BARO_INIT, BARO_NAME);
        i2cMasterRelease(mTask.hw->i2c_num);
        goto err_out;
    }

    if (mTask.verifyId == BOSCH_BMP380_ID) {
        osLog(LOG_INFO, "bmp380: auto detect success %x\n", mTask.verifyId);
        goto success_out;
    } else {
        ret = -1;
        sendSensorErrToAp(ERR_SENSOR_BARO, ERR_CASE_BARO_INIT, BARO_NAME);
        i2cMasterRelease(mTask.hw->i2c_num);
        goto err_out;
    }

success_out:
    mTask.referenceValuePa = 101325;
    mTask.debounceTimeNs = 200 * UNIT_MS_TO_NS;
    baroSensorRegister();
    baroRegisterInterruptMode(BARO_UNINTERRUPTIBLE);
    registerBaroDriverFsm(bmp380Fsm, ARRAY_SIZE(bmp380Fsm));

err_out:
    return ret;
}
#ifndef CFG_OVERLAY_INIT_SUPPORT
MODULE_DECLARE(bmp380, SENS_TYPE_BARO, bmp380Init);
#else
#include "mtk_overlay_init.h"
OVERLAY_DECLARE(bmp380, OVERLAY_ID_BARO, bmp380Init);
#endif
