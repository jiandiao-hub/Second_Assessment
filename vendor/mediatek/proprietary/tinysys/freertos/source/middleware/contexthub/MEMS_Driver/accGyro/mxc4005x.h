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
#ifndef __M_X_C_4005_X_H
#define __M_X_C_4005_X_H

#include <algos/time_sync.h>
#include <atomic.h>
#include <cpu/inc/cpuMath.h>
#include <gpio.h>
#include <heap.h>
#include <hostIntf.h>
/* #include <isr.h> */
#include <nanohub_math.h>
#include <nanohubPacket.h>
/* #include <plat/inc/exti.h> */
/* #include <plat/inc/gpio.h> */
/* #include <plat/inc/syscfg.h> */
#include <plat/inc/rtc.h>
#include <sensors.h>
#include <seos.h>
#include <slab.h>
#include <spi.h>
#include <plat/inc/spichre.h>
#include <spichre-plat.h>
#include <timer.h>
/* #include <variant/inc/sensType.h> */
#include <variant/inc/variant.h>
#include <util.h>
#include <accGyro.h>
#include <cust_accGyro.h>
#include "hwsen.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <contexthub_core.h>
#include "eint.h"
#include <performance.h>
#include <API_sensor_calibration.h>

#define I2C_SPEED                       400000
#define s16 int16_t

#define EVT_SENSOR_ANY_MOTION       sensorGetMyEventType(SENS_TYPE_ANY_MOTION)
////////////////
#define MXC400X_REG_X			0x03
#define MXC400X_REG_Y			0x05
#define MXC400X_REG_Z			0x07
#define MXC400X_REG_TEMP		0x09
#define MXC400X_REG_INT_MASK0          0x0a
#define MXC400X_REG_INT_MASK1          0x0b
#define MXC400X_REG_INT_CLR_0           0x0

#define MXC400X_REG_CTRL		0x0D
#define MXC400X_AWAKE			0x40	/* power on */
#define MXC400X_SLEEP			0x01	/* power donw */
#define MXC400X_RANGE_8G		0x40

#define MXC4005X_REG_CHIP_ID          0x0E
#define MXC400X_ID_1			0x02
#define MXC400X_ID_2			0x03
//////////////////////

/***********************************************
 *** BUS TYPE REG(0X0D) BIT[7:6]
 ***********************************************/
#define MXC4005X_BUS_I2C              0x15

 //#define sensorFsmEnqueueFakeI2cEvt(...)    

 enum MXC4005XState {
    STATE_SAMPLE = CHIP_SAMPLING,
    STATE_FIFO = CHIP_FIFO,
    STATE_CONVERT = CHIP_CONVERT,
    STATE_SAMPLE_DONE = CHIP_SAMPLING_DONE,
    STATE_ACC_ENABLE = CHIP_ACC_ENABLE,
    STATE_ACC_ENABLE_DONE = CHIP_ACC_ENABLE_DONE,
    STATE_ACC_DISABLE = CHIP_ACC_DISABLE,
    STATE_ACC_DISABLE_DONE = CHIP_ACC_DISABLE_DONE,
    STATE_ACC_RATECHG = CHIP_ACC_RATECHG,
    STATE_ACC_RATECHG_DONE = CHIP_ACC_RATECHG_DONE,
    STATE_ACC_CALI = CHIP_ACC_CALI,
    STATE_ACC_CALI_DONE = CHIP_ACC_CALI_DONE,
    STATE_ACC_CFG = CHIP_ACC_CFG,
    STATE_ACC_CFG_DONE = CHIP_ACC_CFG_DONE,
    STATE_ANYMO_ENABLE = CHIP_ANYMO_ENABLE,
    STATE_ANYMO_ENABLE_DONE = CHIP_ANYMO_ENABLE_DONE,
    STATE_ANYMO_DISABLE = CHIP_ANYMO_DISABLE,
    STATE_ANYMO_DISABLE_DONE = CHIP_ANYMO_DISABLE_DONE,
    STATE_HW_INT_STATUS_CHECK = CHIP_HW_INT_STATUS_CHECK,
    STATE_HW_INT_HANDLING = CHIP_HW_INT_HANDLING,
    STATE_HW_INT_HANDLING_DONE = CHIP_HW_INT_HANDLING_DONE,
    STATE_INIT_DONE = CHIP_INIT_DONE,
    STATE_IDLE = CHIP_IDLE,
    STATE_SW_RESET = CHIP_RESET,
	
	
};

enum SensorIndex {
    ACC = 0,
    ANYMO,
    NUM_OF_SENSOR,
};

struct mxc4005xSensor {
    bool powered;
    bool configed;
    bool startCali;
    float staticCali[AXES_NUM];
    int32_t accuracy;
    uint32_t rate;
    uint64_t latency;
    uint32_t hwRate;  // rate set in hw
    uint8_t samplesToDiscard;
    float sensitivity;
};

struct MXC4005XTask {
    uint8_t i2c_addr;
   /* txBuf for i2c operation, fill register and fill value */
    uint8_t txBuf[8];
    /* rxBuf for i2c operation, receive rawdata */
    uint8_t rxBuf[8];
   uint8_t autoDetect[2];
    struct mxc4005xSensor sensors[NUM_OF_SENSOR];

    uint16_t watermark;
    uint32_t fifoDataToRead;
    uint64_t hwSampleTime;
    uint64_t swSampleTime;
    uint64_t lastSampleTime;
	uint64_t startTime;
    uint8_t *regBuffer;
    uint8_t *statusBuffer;
    uint8_t *wakeupBuffer;
    uint8_t *tempStatusBuffer;
    uint8_t *tempBuffer;
    uint8_t *dummyBuffer;
	uint8_t *dataBuffer;
	float staticCali[AXES_NUM];
    float temperature;
    bool tempReady;

    SpiCbkF spiCallBack;
    I2cCallbackF i2cCallBack;
    struct transferDataInfo dataInfo;
    struct accGyroDataPacket accGyroPacket;
    struct accGyro_hw *hw;
    struct sensorDriverConvert cvt;

    uint16_t mWbufCnt;
    uint8_t mRegCnt;
    uint8_t mRetryLeft;

    /* data for factory */
    struct TripleAxisDataPoint accFactoryData;
    int32_t accSwCali[AXES_NUM];
    int32_t debug_trace;

    int latch_time_id;
};

#endif
