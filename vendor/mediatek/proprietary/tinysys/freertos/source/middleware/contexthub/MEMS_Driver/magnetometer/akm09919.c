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
#include <stdbool.h>
#include <stdint.h>
#include <seos.h>
#include <util.h>
#include <sensors.h>
#include <plat/inc/rtc.h>

#include <magnetometer.h>
#include <contexthub_core.h>
#include <cust_mag.h>
#include "cache_internal.h"
#include <mt_gpt.h>
#include <vendor_fusion.h>

#ifndef CFG_MAG_CALIBRATION_IN_AP
#include <akm_wrapper.h>
#endif

#ifdef CFG_VENDOR_FUSION_SUPPORT
#include <akm_wrapper.h>
#endif

#define MAG_NAME                    "akm09919"
#define I2C_SPEED                   400000
#define I2C_OK                      0x00
#define AKM09919_DATA_LEN           8
#define AKM09919_COMPANY_ID         0x48
#define AKM09919_DEVICE_ID          0x0e

/* Device specific constant values */
#define AKM09919_REG_WIA1           0x00
#define AKM09919_REG_WIA2           0x01
#define AKM09919_REG_INFO1          0x02
#define AKM09919_REG_INFO2          0x03
#define AKM09919_REG_ST1            0x10
#define AKM09919_REG_HXH            0x11
#define AKM09919_REG_HXL            0x12
#define AKM09919_REG_HYH            0x13
#define AKM09919_REG_HYL            0x14
#define AKM09919_REG_HZH            0x15
#define AKM09919_REG_HZL            0x16
#define AKM09919_REG_TMPS           0x17
#define AKM09919_REG_ST2            0x18
#define AKM09919_REG_CNTL1          0x30
#define AKM09919_REG_CNTL2          0x31
#define AKM09919_REG_CNTL3          0x32


/*! \name AKM09919 operation mode
 \anchor AKM09919_Mode
 Defines an operation mode of the AKM09919.*/
#define AKM09919_MODE_SNG_MEASURE   0x01
#define AKM09919_MODE_10HZ_MEASURE  0x02
#define AKM09919_MODE_20HZ_MEASURE  0x04
#define AKM09919_MODE_50HZ_MEASURE  0x06
#define AKM09919_MODE_100HZ_MEASURE 0x08

#define AKM09919_MODE_SELF_TEST     0x10
#define AKM09919_MODE_FUSE_ACCESS   0x1F
#define AKM09919_MODE_POWERDOWN     0x00

#define AKM09919_RESET_DATA         0x01

#define DATA_READY_BIT              0X01

#define TLIMIT_TN_REVISION_09919    ""
#define TLIMIT_NO_RST_WIA1_09919    "1-3"
#define TLIMIT_TN_RST_WIA1_09919    "RST_WIA1"
#define TLIMIT_LO_RST_WIA1_09919    AKM09919_COMPANY_ID
#define TLIMIT_HI_RST_WIA1_09919    AKM09919_COMPANY_ID
#define TLIMIT_NO_RST_WIA2_09919    "1-4"
#define TLIMIT_TN_RST_WIA2_09919    "RST_WIA2"
#define TLIMIT_LO_RST_WIA2_09919    AKM09919_DEVICE_ID
#define TLIMIT_HI_RST_WIA2_09919    AKM09919_DEVICE_ID

#define TLIMIT_NO_SNG_ST1_09919     "2-3"
#define TLIMIT_TN_SNG_ST1_09919     "SNG_ST1"
#define TLIMIT_LO_SNG_ST1_09919     1
#define TLIMIT_HI_SNG_ST1_09919     1

#define TLIMIT_NO_SNG_HX_09919      "2-4"
#define TLIMIT_TN_SNG_HX_09919      "SNG_HX"
#define TLIMIT_LO_SNG_HX_09919      -32751
#define TLIMIT_HI_SNG_HX_09919      32751

#define TLIMIT_NO_SNG_HY_09919      "2-6"
#define TLIMIT_TN_SNG_HY_09919      "SNG_HY"
#define TLIMIT_LO_SNG_HY_09919      -32751
#define TLIMIT_HI_SNG_HY_09919      32751

#define TLIMIT_NO_SNG_HZ_09919      "2-8"
#define TLIMIT_TN_SNG_HZ_09919      "SNG_HZ"
#define TLIMIT_LO_SNG_HZ_09919      -32751
#define TLIMIT_HI_SNG_HZ_09919      32751

#define TLIMIT_NO_SNG_ST2_09919     "2-10"
#define TLIMIT_TN_SNG_ST2_09919     "SNG_ST2"
#define TLIMIT_LO_SNG_ST2_09919     0
#define TLIMIT_HI_SNG_ST2_09919     0
#define TLIMIT_ST2_MASK_09919       0x08

#define TLIMIT_NO_SLF_ST1_09919     "2-13"
#define TLIMIT_TN_SLF_ST1_09919     "SLF_ST1"
#define TLIMIT_LO_SLF_ST1_09919     1
#define TLIMIT_HI_SLF_ST1_09919     1

#define TLIMIT_NO_SLF_RVHX_09919    "2-14"
#define TLIMIT_TN_SLF_RVHX_09919    "SLF_REVSHX"
#define TLIMIT_LO_SLF_RVHX_09919    -200
#define TLIMIT_HI_SLF_RVHX_09919    200

#define TLIMIT_NO_SLF_RVHY_09919    "2-16"
#define TLIMIT_TN_SLF_RVHY_09919    "SLF_REVSHY"
#define TLIMIT_LO_SLF_RVHY_09919    -200
#define TLIMIT_HI_SLF_RVHY_09919    200

#define TLIMIT_NO_SLF_RVHZ_09919    "2-18"
#define TLIMIT_TN_SLF_RVHZ_09919    "SLF_REVSHZ"
#define TLIMIT_LO_SLF_RVHZ_09919    -1000
#define TLIMIT_HI_SLF_RVHZ_09919    -150

#define TLIMIT_NO_SLF_ST2_09919     "2-20"
#define TLIMIT_TN_SLF_ST2_09919     "SLF_ST2"
#define TLIMIT_LO_SLF_ST2_09919     0
#define TLIMIT_HI_SLF_ST2_09919     0

#define UNIT_MS_TO_NS 1000000
#define MIN_DELAY_TIME (1 * UNIT_MS_TO_NS)


enum AKM09918State {
    STATE_SAMPLE = CHIP_SAMPLING,
    STATE_DATA_STATE,
    STATE_CONVERT = CHIP_CONVERT,
    STATE_SAMPLE_DONE = CHIP_SAMPLING_DONE,
    STATE_ENABLE = CHIP_ENABLE,
    STATE_ENABLE_DONE = CHIP_ENABLE_DONE,
    STATE_DISABLE = CHIP_DISABLE,
    STATE_DISABLE_DONE = CHIP_DISABLE_DONE,
    STATE_RATECHG = CHIP_RATECHG,
    STATE_RATECHG_DONE = CHIP_RATECHG_DONE,
#ifdef AKM_SELFTEST
    STATE_SELFTEST = CHIP_SELFTEST,
    STATE_SELFTEST_DONE = CHIP_SELFTEST_DONE,
#endif
    STATE_INIT_DONE = CHIP_INIT_DONE,
    STATE_IDLE = CHIP_IDLE,
    STATE_CORE = CHIP_RESET,
    STATE_AKM_INIT,
};

SRAM_REGION_BSS static struct AKM09919Task {
    /* txBuf for i2c operation, fill register and fill value */
    uint8_t txBuf[8];
    /* rxBuf for i2c operation, receive rawdata */
    uint8_t rxBuf[8];
    uint8_t fuse[3];
    uint8_t autoDetect[2];
    uint64_t hwSampleTime;
    struct magTimeStampBuffer magTimeBuf; /* please define in sram region */
    struct transferDataInfo dataInfo;
    struct magDataPacket magPacket;
    /* data for factory */
    struct TripleAxisDataPoint factoryData;
    struct mag_hw *hw;
    struct sensorDriverConvert cvt;
    uint8_t i2c_addr;
    struct mag_dev_info_t mag_dev_info;
} mTask;

static struct AKM09919Task *akm09919DebugPoint;

SRAM_REGION_FUNCTION void akm09919TimerCbkF(uint64_t time)
{
    magTimeBufferWrite(&mTask.magTimeBuf, time);
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

static void magGetSensorInfo(struct sensorInfo_t *data)
{
    strncpy(data->name, MAG_NAME, sizeof(data->name));
    memcpy(&data->mag_dev_info, &mTask.mag_dev_info, sizeof(struct mag_dev_info_t));
}

static int akm09919RegisterCore(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    struct sensorCoreInfo mInfo;
    memset(&mInfo, 0x00, sizeof(struct sensorCoreInfo));
    /* Register sensor Core */
    mInfo.sensType = SENS_TYPE_MAG;
    mInfo.getCalibration = magGetCalibration;
    mInfo.setCalibration = magSetCalibration;
    mInfo.getData = magGetData;
    mInfo.getSensorInfo = magGetSensorInfo;
    sensorCoreRegister(&mInfo);

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int akm09919RegInit(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.txBuf[0] = AKM09919_REG_CNTL3;
    mTask.txBuf[1] = AKM09919_RESET_DATA;
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2, i2cCallBack,
                       next_state);
}

static int akm09919Enable(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                          void *inBuf, uint8_t inSize, uint8_t elemInSize,
                          void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    //osLog(LOG_INFO, "AKM09919: state enable mag\n");
#ifndef CFG_MAG_CALIBRATION_IN_AP
    AKM_EnableCalibrate();
#endif

    mTask.txBuf[0] = AKM09919_REG_CNTL2;
    mTask.txBuf[1] = AKM09919_MODE_100HZ_MEASURE;
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2, i2cCallBack,
                       next_state);
}
static int akm09919Disable(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.txBuf[0] = AKM09919_REG_CNTL2;
    mTask.txBuf[1] = AKM09919_MODE_POWERDOWN;
    return i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2, i2cCallBack,
                       next_state);
}
static int akm09919Rate(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                        void *inBuf, uint8_t inSize, uint8_t elemInSize,
                        void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;
    uint32_t  sample_rate, water_mark;
    struct magCntlPacket cntlPacket;

    ret = rxControlInfo(&cntlPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);
    if (ret < 0) {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "akm09919Rate, rx inSize and elemSize error\n");
        return -1;
    }
    sample_rate = cntlPacket.rate;
    water_mark = cntlPacket.waterMark;
    magTimeBufferReset(&mTask.magTimeBuf);

    osLog(LOG_INFO, "akm09919Rate: %lu, water_mark:%lu\n", sample_rate, water_mark);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int akm09919Sample(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                          void *inBuf, uint8_t inSize, uint8_t elemInSize,
                          void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;

    ret = rxTransferDataInfo(&mTask.dataInfo, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);
    if (ret < 0) {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "akm09918Sample, rx dataInfo error\n");
        return -1;
    }

    mTask.txBuf[0] = AKM09919_REG_ST1;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 1, i2cCallBack, next_state);
}

static int akm09919DataState(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                          void *inBuf, uint8_t inSize, uint8_t elemInSize,
                          void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    if (!(mTask.rxBuf[0] & 0x1))
        osLog(LOG_ERROR, "akm09919 error [%u]: data not ready!\n", mTask.rxBuf[0]);

    mTask.txBuf[0] = AKM09919_REG_HXH;
    return i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         mTask.rxBuf, 8, i2cCallBack, next_state);
}

static int akm09919Convert(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    struct magData *data = mTask.magPacket.outBuf;
    int16_t remap_data[AXES_NUM];
    int16_t idata[AXES_NUM];
    uint64_t timestamp = 0;
    uint8_t data_size = 0;

    idata[AXIS_X] = (int16_t)((mTask.rxBuf[AXIS_X * 2] << 8) | (mTask.rxBuf[AXIS_X * 2 + 1]));
    idata[AXIS_Y] = (int16_t)((mTask.rxBuf[AXIS_Y * 2] << 8) | (mTask.rxBuf[AXIS_Y * 2 + 1]));
    idata[AXIS_Z] = (int16_t)((mTask.rxBuf[AXIS_Z * 2] << 8) | (mTask.rxBuf[AXIS_Z * 2 + 1]));

    remap_data[mTask.cvt.map[AXIS_X]] = mTask.cvt.sign[AXIS_X] * idata[AXIS_X];
    remap_data[mTask.cvt.map[AXIS_Y]] = mTask.cvt.sign[AXIS_Y] * idata[AXIS_Y];
    remap_data[mTask.cvt.map[AXIS_Z]] = mTask.cvt.sign[AXIS_Z] * idata[AXIS_Z];

    mTask.factoryData.ix = (int32_t)(remap_data[AXIS_X] * MAGNETOMETER_INCREASE_NUM_AP);
    mTask.factoryData.iy = (int32_t)(remap_data[AXIS_Y] * MAGNETOMETER_INCREASE_NUM_AP);
    mTask.factoryData.iz = (int32_t)(remap_data[AXIS_Z] * MAGNETOMETER_INCREASE_NUM_AP);
    //osLog(LOG_ERROR, "akm09918Convert msensor output data: %f, %f, %f!\n", (double)data[0].x, (double)data[0].y, (double)data[0].z);

    data_size = magTimeBufferSize(&mTask.magTimeBuf);
    for (uint8_t i = 0; i < data_size; i++) {
        data[i].x = remap_data[AXIS_X];
        data[i].y = remap_data[AXIS_Y];
        data[i].z = remap_data[AXIS_Z];
        magTimeBufferRead(&mTask.magTimeBuf, &mTask.hwSampleTime);
        timestamp = addThenRetreiveAverageMagTimeStamp(mTask.hwSampleTime);
    }

    txTransferDataInfo(&mTask.dataInfo, data_size, timestamp, data);
    /*osLog(LOG_ERROR, "akm09918Convert raw data: %f, %f, %f, timestamp: %llu size: %u!\n", (double)remap_data[AXIS_X],
    (double)remap_data[AXIS_Y], (double)remap_data[AXIS_Z], timestamp, data_size); */

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

#ifdef AKM_SELFTEST
static int TEST_DATA(const char testno[], const char testname[], const int testdata,
                     const int lolimit, const int hilimit, int *pf_total)
{
    int pf;         /* Pass;1, Fail;-1 */

    if ((testno == NULL) && (strncmp(testname, "START", 5) == 0)) {
        osLog(LOG_INFO, " Test Name Fail Test Data [Low High]\n");
        pf = 1;
    } else if ((testno == NULL) && (strncmp(testname, "END", 3) == 0)) {
        if (*pf_total == 1)
            osLog(LOG_ERROR, "self-test was passed.\n\n");
        else
            osLog(LOG_ERROR, "self-test was failed.\n\n");

        pf = 1;
    } else {
        if ((lolimit <= testdata) && (testdata <= hilimit))
            pf = 1;
        else
            pf = -1;

        /* display result */
        osLog(LOG_INFO, " %7s  %-10s     %c %9d [%9d    %9d]\n",
              testno, testname, ((pf == 1) ? ('.') : ('F')),
              testdata, lolimit, hilimit);
    }

    /* Pass/Fail check */
    if (*pf_total != 0) {
        if ((*pf_total == 1) && (pf == 1))
            *pf_total = 1;  /* Pass */
        else
            *pf_total = -2; /* Fail */
    }
    return pf;
}
static int akm09919Seltest(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int hdata[3];
    int selftest_result = 1;
    /* *********************************************** */
    /* Step1 */
    /* *********************************************** */
    /*  Test DeviceID, Read values from WIA*/
    mTask.txBuf[0] = AKM09919_REG_WIA1;
    i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                      mTask.rxBuf, 2, NULL, NULL);

    /* TEST */
    TEST_DATA(TLIMIT_NO_RST_WIA1_09919, TLIMIT_TN_RST_WIA1_09919,
              (int)mTask.rxBuf[0],
              TLIMIT_LO_RST_WIA1_09919, TLIMIT_HI_RST_WIA1_09919,
              &selftest_result);
    TEST_DATA(TLIMIT_NO_RST_WIA2_09919, TLIMIT_TN_RST_WIA2_09919,
              (int)mTask.rxBuf[1],
              TLIMIT_LO_RST_WIA2_09919, TLIMIT_HI_RST_WIA2_09919,
              &selftest_result);
    /* *********************************************** */
    /* Step2 */
    /* *********************************************** */
    /* Set to Self-test mode (Set CNTL register) */
    mTask.txBuf[0] = AKM09919_REG_CNTL2;
    mTask.txBuf[1] = AKM09919_MODE_SELF_TEST;
    i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2,
                      NULL, 0, NULL, NULL);
    /* Wait for DRDY pin changes to HIGH. */
    mdelay(10);

    /* Get measurement data from AK09918 */
    /*  (HXL + HXH) + (HYL + HYH) + (HZL + HZH) + TEMP + ST2 */
    /* =  (1 + 1) + (1 + 1) + (1 + 1) + 1 + 1 = 8yte */
    mTask.txBuf[0] = AKM09919_REG_HXH;
    i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                      mTask.rxBuf, 8, NULL, NULL);

    hdata[0] = (int16_t)(mTask.rxBuf[0] << 8 | mTask.rxBuf[1]));
    hdata[1] = (int16_t)(mTask.rxBuf[2] << 8 | mTask.rxBuf[3]));
    hdata[2] = (int16_t)(mTask.rxBuf[4] << 8 | mTask.rxBuf[5]));

    /* TEST */
    TEST_DATA(TLIMIT_NO_SLF_RVHX_09919,
              TLIMIT_TN_SLF_RVHX_09919, hdata[0], TLIMIT_LO_SLF_RVHX_09919,
              TLIMIT_HI_SLF_RVHX_09919, &selftest_result);

    TEST_DATA(TLIMIT_NO_SLF_RVHY_09919,
              TLIMIT_TN_SLF_RVHY_09919, hdata[1], TLIMIT_LO_SLF_RVHY_09919,
              TLIMIT_HI_SLF_RVHY_09919, &selftest_result);

    TEST_DATA(TLIMIT_NO_SLF_RVHZ_09919, TLIMIT_TN_SLF_RVHZ_09919,
              hdata[2], TLIMIT_LO_SLF_RVHZ_09919,
              TLIMIT_HI_SLF_RVHZ_09919, &selftest_result);

    TEST_DATA(TLIMIT_NO_SLF_ST2_09919, TLIMIT_TN_SLF_ST2_09919,
              (int)mTask.rxBuf[7] & TLIMIT_ST2_MASK_09919,
              TLIMIT_LO_SLF_ST2_09919, TLIMIT_HI_SLF_ST2_09919, &selftest_result);
    osLog(LOG_ERROR, "akm09919Seltest finish result:%d\n", selftest_result);
    magSendTestResult(selftest_result);

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}
#endif

static struct sensorFsm akm09919Fsm[] = {
    sensorFsmCmd(STATE_SAMPLE, STATE_DATA_STATE, akm09919Sample),
    sensorFsmCmd(STATE_DATA_STATE, STATE_CONVERT, akm09919DataState),
    sensorFsmCmd(STATE_CONVERT, STATE_SAMPLE_DONE, akm09919Convert),

    sensorFsmCmd(STATE_ENABLE, STATE_ENABLE_DONE, akm09919Enable),
    sensorFsmCmd(STATE_DISABLE, STATE_DISABLE_DONE, akm09919Disable),

    sensorFsmCmd(STATE_RATECHG, STATE_RATECHG_DONE, akm09919Rate),

    sensorFsmCmd(STATE_CORE, STATE_AKM_INIT, akm09919RegInit),
    sensorFsmCmd(STATE_AKM_INIT, STATE_INIT_DONE, akm09919RegisterCore),
#ifdef AKM_SELFTEST
    sensorFsmCmd(STATE_SELFTEST, STATE_SELFTEST_DONE, akm09919Seltest),
#endif

};

#ifndef CFG_MAG_CALIBRATION_IN_AP
static int akm09919CaliInitLib(int hwGyroSupported)
{
    AKM_SetGyroFlagForCalibrate(hwGyroSupported);
    return 0;
}

static int akm09919CaliApiGetOffset(float offset[AXES_NUM])
{
    return 0;
}
static int akm09919CaliApiSetOffset(float offset[AXES_NUM])
{
    AKM_ReloadContext(offset);
    AKM_Open();
    return 0;
}
static int akm09919CaliApiSetGyroData(struct magCaliDataInPut *inputData)
{
    return AKM_SetGyroData((double)inputData->x, (double)inputData->y,
                           (double)inputData->z, inputData->timeStamp);
}
static int akm09919DoCaliAPI(struct magCaliDataInPut *inputData,
                             struct magCaliDataOutPut *outputData)
{
    int err = 0;
    int16_t tmpStatus = 0;
    double data_offset[AXES_NUM];
    double data_cali[AXES_NUM];

    err = AKM_SetMagData((int32_t)inputData->x, (int32_t)inputData->y,
        (int32_t)inputData->z, inputData->timeStamp);
    if (err < 0)
        return err;
    err = AKM_GetMagData(data_cali, data_offset, &tmpStatus);
    if (err < 0)
        return err;
    outputData->x = data_cali[AXIS_X];
    outputData->y = data_cali[AXIS_Y];
    outputData->z = data_cali[AXIS_Z];
    outputData->x_bias = data_offset[AXIS_X];
    outputData->y_bias = data_offset[AXIS_Y];
    outputData->z_bias = data_offset[AXIS_Z];
    outputData->status = tmpStatus;

    return 0;
}
static int akm09919CaliApiGetCaliParam(int32_t caliParameter[6])
{
    AKM_SaveCaliInfoToFile(caliParameter);
    return 0;
}
static int akm09919CaliApiSetCaliParam(int32_t caliParameter[6])
{
    AKM_ReloadCaliInfoFromFile(caliParameter);
    //osLog(LOG_INFO, "SetCaliParam :%d, %d, %d, %d, %d, %d\n",
    //caliParameter[0], caliParameter[1], caliParameter[2],
    //caliParameter[3], caliParameter[4], caliParameter[5]);
    AKM_Open();
    return 0;
}

static struct magCalibrationLibAPI akm09919CaliAPI = {
    .initLib = akm09919CaliInitLib,
    .caliApiGetOffset = akm09919CaliApiGetOffset,
    .caliApiSetOffset = akm09919CaliApiSetOffset,
    .caliApiGetCaliParam = akm09919CaliApiGetCaliParam,
    .caliApiSetCaliParam = akm09919CaliApiSetCaliParam,
    .caliApiSetGyroData = akm09919CaliApiSetGyroData,
    .doCaliApi = akm09919DoCaliAPI
};
#endif //#ifndef cali_in_ap

#ifdef CFG_VENDOR_FUSION_SUPPORT
static int akm09919FusionInitLib(int hwGyroSupport)
{
    AKM_SetGyroFlagForFusion(hwGyroSupport);
    return 0;
}
static int akm09919FusionEnableLib(struct InterfaceEnParam *pEnParam)
{
    if (pEnParam->en) {
        AKM_EnableFusion();
        AKM_FusionSetLatency(pEnParam->latency);
    } else
        AKM_DisableFusion();

    return 0;
}
static int akm09919FusionSetGyro(struct InterfaceDataIn *inData)
{
    return AKM_FusionSetGyroData(inData->vec[0],
                                 inData->vec[1], inData->vec[2], inData->timeStamp);
}
static int akm09919FusionSetAcc(struct InterfaceDataIn *inData)
{
    return AKM_FusionSetAccData(inData->vec[0],
                                inData->vec[1], inData->vec[2], inData->timeStamp);
}
static int akm09919FusionSetMag(struct InterfaceDataIn *inData)
{
    return AKM_FusionSetMagData(inData->vec[0],
                                inData->vec[1], inData->vec[2], inData->timeStamp);
}
static int akm09919FusionGetGravity(struct InterfaceDataOut *outData)
{
    int ret = 0;
    double vec[6] = {0};
    int16_t accurancy = 0;

    ret = AKM_GetGravity(vec, &accurancy);
    outData->vec[0] = vec[0];
    outData->vec[1] = vec[1];
    outData->vec[2] = vec[2];
    outData->status = accurancy;
    return ret;
}
static int akm09919FusionGetRotationVector(struct InterfaceDataOut *outData)
{
    int ret = 0;
    double vec[6] = {0};
    int16_t accurancy = 0;

    ret = AKM_GetRotationVector(vec, &accurancy);
    outData->vec[0] = vec[0];
    outData->vec[1] = vec[1];
    outData->vec[2] = vec[2];
    outData->vec[3] = vec[3];
    outData->status = accurancy;
    return ret;
}
static int akm09919FusionGetOrientation(struct InterfaceDataOut *outData)
{
    int ret = 0;
    double vec[6] = {0};
    int16_t accurancy = 0;

    ret = AKM_GetOri(vec, &accurancy);
    outData->vec[0] = vec[0];
    outData->vec[1] = vec[1];
    outData->vec[2] = vec[2];
    outData->status = accurancy;
    return ret;
}
static int akm09919FusionGetLinearaccel(struct InterfaceDataOut *outData)
{
    int ret = 0;
    double vec[6] = {0};
    int16_t accurancy = 0;

    ret = AKM_GetLinearAccelerometer(vec, &accurancy);
    outData->vec[0] = vec[0];
    outData->vec[1] = vec[1];
    outData->vec[2] = vec[2];
    outData->status = accurancy;
    return ret;
}
static int akm09919FusionGetGameRotationVector(struct InterfaceDataOut *outData)
{
    int ret = 0;
    double vec[6] = {0};
    int16_t accurancy = 0;

    ret = AKM_GetRotationVector(vec, &accurancy);
    outData->vec[0] = vec[0];
    outData->vec[1] = vec[1];
    outData->vec[2] = vec[2];
    outData->vec[3] = vec[3];
    outData->status = accurancy;
    return ret;
}
static int akm09919FusionGetGeoMagnetic(struct InterfaceDataOut *outData)
{
    int ret = 0;
    double vec[6] = {0};
    int16_t accurancy = 0;

    ret = AKM_GetRotationVector(vec, &accurancy);
    outData->vec[0] = vec[0];
    outData->vec[1] = vec[1];
    outData->vec[2] = vec[2];
    outData->vec[3] = vec[3];
    outData->status = accurancy;
    return ret;
}
static int akm09919FusionGetVirtualGyro(struct InterfaceDataOut *outData)
{
    int ret = 0;
    double vec[6] = {0};
    int16_t accurancy = 0;

    ret = AKM_GetGyroscope(vec, &accurancy);
    outData->vec[0] = vec[0];
    outData->vec[1] = vec[1];
    outData->vec[2] = vec[2];
    outData->status = accurancy;
    return ret;
}
static struct VendorFusionInterfact_t akm09919Interface = {
    .name = "akm09919",
    .initLib = akm09919FusionInitLib,
    .enableLib = akm09919FusionEnableLib,
    .setGyroData = akm09919FusionSetGyro,
    .setAccData = akm09919FusionSetAcc,
    .setMagData = akm09919FusionSetMag,
    .getGravity = akm09919FusionGetGravity,
    .getRotationVector = akm09919FusionGetRotationVector,
    .getOrientation = akm09919FusionGetOrientation,
    .getLinearaccel = akm09919FusionGetLinearaccel,
    .getGameRotationVector = akm09919FusionGetGameRotationVector,
    .getGeoMagnetic = akm09919FusionGetGeoMagnetic,
    .getVirtualGyro = akm09919FusionGetVirtualGyro
};
#endif //#ifdef vendor_fusion

static int akm09919Init(void)
{
    int ret = 0;

    akm09919DebugPoint = &mTask;
    insertMagicNum(&mTask.magPacket);
    mTask.hw = get_cust_mag(MAG_NAME);
    if (NULL == mTask.hw) {
        osLog(LOG_ERROR, "akm09919 get_cust_mag fail\n");
        ret = -1;
        goto err_out;
    }
    mTask.i2c_addr = mTask.hw->i2c_addr[0];
    osLog(LOG_ERROR, "mag i2c_num: %d, i2c_addr: 0x%x\n", mTask.hw->i2c_num, mTask.i2c_addr);

    if (0 != (ret = sensorDriverGetConvert(mTask.hw->direction, &mTask.cvt))) {
        osLog(LOG_ERROR, "invalid direction: %d\n", mTask.hw->direction);
    }
    osLog(LOG_ERROR, "mag map[0]:%d, map[1]:%d, map[2]:%d, sign[0]:%d, sign[1]:%d, sign[2]:%d\n\r",
          mTask.cvt.map[AXIS_X], mTask.cvt.map[AXIS_Y], mTask.cvt.map[AXIS_Z],
          mTask.cvt.sign[AXIS_X], mTask.cvt.sign[AXIS_Y], mTask.cvt.sign[AXIS_Z]);
    i2cMasterRequest(mTask.hw->i2c_num, I2C_SPEED);
    mTask.txBuf[0] = AKM09919_REG_WIA1;
    for (uint8_t i = 0; i < 3; i++) {
        ret = i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                                (uint8_t *)mTask.autoDetect, 2, NULL, NULL);
        if (ret >= 0)
            break;
    }
    if (ret < 0) {
        ret = -1;
        i2cMasterRelease(mTask.hw->i2c_num);
        sendSensorErrToAp(ERR_SENSOR_MAG, ERR_CASE_MAG_INIT, MAG_NAME);
        goto err_out;
    }
    if ((mTask.autoDetect[0] == AKM09919_COMPANY_ID) && (mTask.autoDetect[1] == AKM09919_DEVICE_ID)) {
        osLog(LOG_INFO, "akm09919: auto detect success\n");
        goto success_out;
    } else {
        ret = -1;
        i2cMasterRelease(mTask.hw->i2c_num);
        sendSensorErrToAp(ERR_SENSOR_MAG, ERR_CASE_MAG_INIT, MAG_NAME);
        goto err_out;
    }
success_out:
    mTask.mag_dev_info.layout = 0x00;
    mTask.mag_dev_info.deviceid = 0x10;
    strncpy(mTask.mag_dev_info.libname, "akm", sizeof(mTask.mag_dev_info.libname));

    magSensorRegister();
    magRegisterInterruptMode(MAG_UNFIFO);
    registerMagDriverFsm(akm09919Fsm, ARRAY_SIZE(akm09919Fsm));
    registerMagTimerCbk(akm09919TimerCbkF);
#ifndef CFG_MAG_CALIBRATION_IN_AP
    registerMagCaliAPI(&akm09919CaliAPI);
    AKM_SetDevice(mTask.autoDetect);
    AKM_Open();
#endif

#ifdef CFG_VENDOR_FUSION_SUPPORT
    registerVendorInterface(&akm09919Interface);
#endif

err_out:
    return ret;
}

#ifndef CFG_OVERLAY_INIT_SUPPORT
MODULE_DECLARE(akm09919, SENS_TYPE_MAG, akm09919Init);
#else
#include "mtk_overlay_init.h"
OVERLAY_DECLARE(akm09919, OVERLAY_ID_MAG, akm09919Init);
#endif
