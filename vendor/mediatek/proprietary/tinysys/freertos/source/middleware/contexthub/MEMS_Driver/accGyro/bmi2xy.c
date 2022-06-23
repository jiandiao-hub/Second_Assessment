/*!
 * @section LICENSE
 *
 * (C) Copyright 2011~2020 Bosch Sensortec GmbH All Rights Reserved
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
 *
 *------------------------------------------------------------------------------
 *  Disclaimer
 *
 * Common: Bosch Sensortec products are developed for the consumer goods
 * industry. They may only be used within the parameters of the respective valid
 * product data sheet.  Bosch Sensortec products are provided with the express
 * understanding that there is no warranty of fitness for a particular purpose.
 * They are not fit for use in life-sustaining, safety or security sensitive
 * systems or any system or device that may lead to bodily harm or property
 * damage if the system or device malfunctions. In addition, Bosch Sensortec
 * products are not fit for use in products which interact with motor vehicle
 * systems.  The resale and/or use of products are at the purchaser's own risk
 * and his own responsibility. The examination of fitness for the intended use
 * is the sole responsibility of the Purchaser.
 *
 * The purchaser shall indemnify Bosch Sensortec from all third party claims,
 * including any claims for incidental, or consequential damages, arising from
 * any product use not covered by the parameters of the respective valid product
 * data sheet or not approved by Bosch Sensortec and reimburse Bosch Sensortec
 * for all costs in connection with such claims.
 *
 * The purchaser must monitor the market for the purchased products,
 * particularly with regard to product safety and inform Bosch Sensortec without
 * delay of all security relevant incidents.
 *
 * Engineering Samples are marked with an asterisk (*) or (e). Samples may vary
 * from the valid technical specifications of the product series. They are
 * therefore not intended or fit for resale to third parties or for use in end
 * products. Their sole purpose is internal client testing. The testing of an
 * engineering sample may in no way replace the testing of a product series.
 * Bosch Sensortec assumes no liability for the use of engineering samples. By
 * accepting the engineering samples, the Purchaser agrees to indemnify Bosch
 * Sensortec from all claims arising from the use of engineering samples.
 *
 * Special: This software module (hereinafter called "Software") and any
 * information on application-sheets (hereinafter called "Information") is
 * provided free of charge for the sole purpose to support your application
 * work. The Software and Information is subject to the following terms and
 * conditions:
 *
 * The Software is specifically designed for the exclusive use for Bosch
 * Sensortec products by personnel who have special experience and training. Do
 * not use this Software if you do not have the proper experience or training.
 *
 * This Software package is provided `` as is `` and without any expressed or
 * implied warranties, including without limitation, the implied warranties of
 * merchantability and fitness for a particular purpose.
 *
 * Bosch Sensortec and their representatives and agents deny any liability for
 * the functional impairment of this Software in terms of fitness, performance
 * and safety. Bosch Sensortec and their representatives and agents shall not be
 * liable for any direct or indirect damages or injury, except as otherwise
 * stipulated in mandatory applicable law.
 *
 * The Information provided is believed to be accurate and reliable. Bosch
 * Sensortec assumes no responsibility for the consequences of use of such
 * Information nor for any infringement of patents or other rights of third
 * parties which may result from its use.
 *
 */
//#define SUPPORT_MT6885   1
#include <algos/time_sync.h>
#include <atomic.h>
#include <cpu/inc/cpuMath.h>
#include <gpio.h>
#include <heap.h>
#include <hostIntf.h>
#include <nanohub_math.h>
#include <nanohubPacket.h>
#include <plat/inc/rtc.h>
#include <sensors.h>
#include <seos.h>
#include <slab.h>
#include <spi.h>
#if defined(SUPPORT_MT6885)
#else
#include <plat/inc/spichre.h>
#include <spichre-plat.h>
#endif
#include <timer.h>
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
#if !defined(SUPPORT_MT6885)
#include <performance.h>
#else
#include "mt_eint.h"
#include "xgpt.h"

//#include <performance.h>
#endif
#ifndef CFG_MTK_CALIBRATION_V1_0
#include <API_sensor_calibration.h>
#else
#include "calibration/factory_calibration/factory_cal.h"
#endif


#define BMI2XY_SELFTEST   1
//#define BMI260_OIS    1

#include "bmi2_defs.h"
#include "bmi2xy_ver.h"
//#define BMI220
#define BMI220
#if defined(BMI261)
#include "bmi261.h"
#define BMI2XY_ID                 0x21
#endif
#if defined(BMI260)
#include "bmi260.h"
#define BMI2XY_ID                 0x27
#endif

#if defined(BMI220)
#include "bmi220.h"
#define BMI2XY_ID                 0x26
#endif
#define ULONG_LONG_MAX 0XFFFFFFFFFFFFFFFFULL


#define INFO_PRINT(fmt, ...) do { \
        osLog(LOG_INFO, "%s " fmt, "[BMI2XY]", ##__VA_ARGS__); \
    } while (0);

#define ERROR_PRINT(fmt, ...) do { \
        osLog(LOG_ERROR, "%s " fmt, "[BMI2XY] ERROR:", ##__VA_ARGS__); \
    } while (0);

#define DEBUG_PRINT(fmt, ...) do { \
        if (DBG_ENABLE) {  \
            INFO_PRINT(fmt,  ##__VA_ARGS__); \
        } \
    } while (0);

#define DEBUG_PRINT_IF(cond, fmt, ...) do { \
        if ((cond) && DBG_ENABLE) {  \
            INFO_PRINT(fmt,  ##__VA_ARGS__); \
        } \
    } while (0);

#define DBG_ENABLE                0
#define DBG_CHUNKED               1
#define DBG_INT                   0
#define DBG_SHALLOW_PARSE         0
#define DBG_STATE                 0
#define DBG_WM_CALC               0
#define TIMESTAMP_DBG             0
#ifdef CFG_HW_STEP_COUNTER_SUPPORT
#define STEPCNT_DBG               0
#endif


#define INT_ANY_MOTION  0x40
#define INT_FIFO_WM     0x02
#define ACC_NAME     "bmi2xy_acc"
#define GYRO_NAME     "bmi2xy_gyro"

#define BMI2XY_APP_ID APP_ID_MAKE(APP_ID_VENDOR_GOOGLE, 2)

#define BMI2XY_SPI_WRITE          0x00
#define BMI2XY_SPI_READ           0x80

#define BMI2XY_SPI_BUS_ID         1
#define BMI2XY_SPI_SPEED_HZ       8000000
#define BMI2XY_SPI_MODE           3
#define BMI2XY_WRITE_LEN       1020


#define BMI2XY_FRAME_HEADER_INVALID  0x80   // mark the end of valid data
#define BMI2XY_FRAME_HEADER_SKIP     0x81   // not defined by hw, used for skip a byte in buffer

#define WATERMARK_MIN                1
#define WATERMARK_MAX                200    // must <= 255 (0xff)

#define WATERMARK_MAX_SENSOR_RATE    400    // Accel and gyro are 400 Hz max
#define WATERMARK_TIME_UNIT_NS       (1000000000ULL / (WATERMARK_MAX_SENSOR_RATE))

#define gSPI    BMI2XY_SPI_BUS_ID
#define SPI_WRITE_0(addr, data) spiQueueWrite(addr, data, 2)
#define SPI_WRITE_1(addr, data, delay) spiQueueWrite(addr, data, delay)
#define GET_SPI_WRITE_MACRO(_1, _2, _3, NAME, ...) NAME
#define SPI_WRITE(...) GET_SPI_WRITE_MACRO(__VA_ARGS__, SPI_WRITE_1, SPI_WRITE_0)(__VA_ARGS__)

#define SPI_READ_0(addr, size, buf) spiQueueRead(addr, size, buf, 0)
#define SPI_READ_1(addr, size, buf, delay) spiQueueRead(addr, size, buf, delay)
#define GET_SPI_READ_MACRO(_1, _2, _3, _4, NAME, ...) NAME
#define SPI_READ(...) GET_SPI_READ_MACRO(__VA_ARGS__, SPI_READ_1, SPI_READ_0)(__VA_ARGS__)
#define EVT_SENSOR_ANY_MOTION       sensorGetMyEventType(SENS_TYPE_ANY_MOTION)
#ifdef CFG_HW_STEP_COUNTER_SUPPORT
#define EVT_SENSOR_STEP_COUNTER sensorGetMyEventType(SENS_TYPE_STEP_COUNT)
#endif
#define MAX_NUM_COMMS_EVENT_SAMPLES 15

#define BMI2XY_FS_125_LSB               2624
#define BMI2XY_FS_250_LSB               1312
#define BMI2XY_FS_500_LSB               656
#define BMI2XY_FS_1000_LSB              328
#define BMI2XY_FS_2000_LSB              164

#define kScale_acc    0.00239501953f  // ACC_range * 9.81f / 32768.0f;
#define kScale_gyr    (DEGREE_TO_RADIRAN_SCALAR / 16.4f) //0.00106472439f  // GYR_range * M_PI / (180.0f * 32768.0f);
#define kScale_temp   0.001953125f    // temperature in deg C
#define kTempInvalid  -1000.0f

#define kTimeSyncPeriodNs        100000000ull  // sync sensor and RTC time every 100ms
#define kSensorTimerIntervalUs   39ull        // bmi160 clock increaments every 39000ns

#define kMinRTCTimeIncrementNs   1250000ull  // forced min rtc time increment, 1.25ms for 400Hz
#define kMinSensorTimeIncrement  64         // forced min sensortime increment,
                                            // 64 = 2.5 msec for 400Hz

#define ACC_MIN_RATE    5
#define GYR_MIN_RATE    6
#define ACC_MAX_RATE    12
#define GYR_MAX_RATE    13
#define ACC_MAX_OSR     3
#define GYR_MAX_OSR     4
#define OSR_THRESHOLD   8

#define MOTION_ODR         7

#define RETRY_CNT_CALIBRATION 1
#define RETRY_CNT_ID 5

#define SPI_PACKET_SIZE 30
#define FIFO_READ_SIZE  (896) //1024 - 64 - 32 - 32(for margin), for SPI transfor length must smaller than 1024 bytes
#define CHUNKED_READ_SIZE (64)
#define BUF_MARGIN 32   // some extra buffer for additional reg RW when a FIFO read happens
#define SPI_BUF_SIZE (FIFO_READ_SIZE + CHUNKED_READ_SIZE + BUF_MARGIN + 32) //must shorter than or equal 1024 bytes, current = 1024

enum sensorindex {
    ACC = 0,
    GYR,
    ANYMO,
#ifdef CFG_HW_STEP_COUNTER_SUPPORT
    STEPCNT,
#endif
    NUM_OF_SENSOR,
};

struct bmi2xysensor {
    uint32_t handle;
    uint32_t rate;
    uint32_t hwRate;  //rate set in hw
    uint64_t latency;
    uint32_t offset[3];
    bool powered;  // activate status
    bool configed;  // configure status
    bool startCali;
    uint8_t flush;
    float staticCali[AXES_NUM];
    int32_t accuracy;
    uint8_t samplesToDiscard;
    enum sensorindex idx;
    float sensitivity;
};

struct BMI2XYTask {
    uint32_t tid;
    struct bmi2xysensor sensors[NUM_OF_SENSOR];

    // time keeping.
    uint64_t last_sensortime;
    uint64_t frame_sensortime;
    uint64_t prev_frame_time[3];
    uint64_t time_delta[3];
    uint64_t next_delta[3];
    uint64_t tempTime;

    // spi and interrupt
    spi_cs_t cs;
    struct SpiMode mode;
    struct SpiPacket packets[SPI_PACKET_SIZE];
    struct SpiDevice *spiDev;
    time_sync_t gSensorTime2RTC;

    float tempCelsius;
    uint8_t *config_stream_status;

#ifdef CFG_HW_STEP_COUNTER_SUPPORT
    uint8_t interrupt_enable_1;
    bool step_cnt_changed;
    uint32_t total_step_cnt;
    uint32_t last_step_cnt;
#endif
    uint8_t acc_downsample;
    uint8_t gyr_downsample;
    bool fifo_enabled[3];
    uint16_t configFileSize;
    // spi buffers
    int xferCnt;
    uint8_t *dataBuffer;
    uint8_t *statusBuffer;
    uint8_t *sampleStatusBuffer;
    uint16_t fifoLength;
    uint8_t *stepBuffer;
    uint8_t temperatureBuffer[4];
    uint8_t txrxBuffer[SPI_BUF_SIZE];
    bool frame_sensortime_valid;

    // FIFO setting
    uint16_t  watermark;
    uint16_t temperature;
    uint8_t *reg24_25;
    uint8_t *reg40_4A;
    uint8_t *reg53_59;
    uint8_t *reg7c_7d;
    // spi rw
    struct SlabAllocator *mDataSlab;
    uint16_t mWbufCnt;
    uint8_t mRegCnt;
    uint8_t mRetryLeft;
    bool spiInUse;

    SpiCbkF spiCallBack;
    void *next_state;

    struct transferDataInfo dataInfo;
    struct accGyroDataPacket accGyroPacket;
    uint64_t hwSampleTime;
    uint64_t swSampleTime;
    uint64_t lastSampleTime;
    struct accGyro_hw *hw;
    struct sensorDriverConvert cvt;
    /* data for factory */
    struct TripleAxisDataPoint accFactoryData;
    struct TripleAxisDataPoint gyroFactoryData;

    int32_t accSwCali[AXES_NUM];
    int32_t gyroSwCali[AXES_NUM];
    int32_t debug_trace;
    int latch_time_id;
#ifdef CFG_MTK_CALIBRATION_V1_0
    struct AccFactoryCal accFactoryCal;
    struct GyroFactoryCal gyroFactoryCal;
#endif
#ifdef BMI2XY_SELFTEST
    uint8_t *chip_id;
    uint8_t *accSelf;
    uint8_t *confData;
    uint8_t *bufData;
    uint8_t reg_val[17];
   uint8_t *accSelfTestStatus;
    int selfValue;
    int16_t acc_data_positive[3];
    int16_t acc_data_negtive[3];
    uint8_t *gyroSelf;
    uint8_t *gyroStatus;
    uint8_t *gyroUserGainStatus;
    int gyroSelfValue;
#endif
#if defined(BMI260_OIS)
    uint8_t ois_config;
#endif

};
#if !defined(SUPPORT_MT6885)
static struct BMI2XYTask mTask;
#else
SRAM_REGION_BSS static struct BMI2XYTask mTask;
#endif
typedef struct BMI2XYTask _Task;
#define TASK  _Task* const _task

// To get rid of static variables all task functions should have a task structure pointer input.
// This is an intermediate step.
#define TDECL()  TASK = &mTask; (void)_task

// Access task variables without explicitly specify the task structure pointer.
#define T(v)  (_task->v)

// Atomic get state
#define GET_STATE() (atomicReadByte(&(_task->state)))

// Atomic set state, this set the state to arbitrary value, use with caution
#define SET_STATE(s) do {\
        DEBUG_PRINT_IF(DBG_STATE, "set state %s\n", getStateName(s));\
        atomicWriteByte(&(_task->state), (s));\
    }while(0)
#define BMI2XY_TO_ANDROID_COORDINATE(x, y, z)   \
    do {                                        \
        float xi = x, yi = y, zi = z;         \
        x = -yi; y = xi; z = zi;                \
    } while (0)

#define max(x, y)   (x > y ? x : y)
// Short-hand
#define trySwitchState(s) trySwitchState_(_task, (s))


static void accGetCalibration(int32_t *cali, int32_t size);
static void gyroGetCalibration(int32_t *cali, int32_t size);

enum BMI2XYState {
    STATE_RESET_BMI2XY = CHIP_RESET + 1,
    STATE_INIT_BMI2XY,
    STATE_SENSOR_LOAD_CONFIG_FILE0,
    STATE_SENSOR_LOAD_CONFIG_FILE1,
    STATE_SENSOR_LOAD_CONFIG_FILE2,
    STATE_SENSOR_LOAD_CONFIG_FILE3,
    STATE_SENSOR_LOAD_CONFIG_FILE4,
    STATE_SENSOR_LOAD_CONFIG_FILE5,
    STATE_SENSOR_LOAD_CONFIG_FILE6,
    STATE_SENSOR_LOAD_CONFIG_FILE7,
    STATE_SENSOR_LOAD_CONFIG_FILE8,
    STATE_SENSOR_LOAD_CONFIG_FILE_CHECK,
    STATE_SENSOR_REGISTRATION,
    STATE_FIFO,
    STATE_CONVERT,
    STATE_UNMASK_EINT,
    STATE_ACC_CHECK_REGISTER,
    STATE_GYRO_CHECK_REGISTER,
#if defined(BMI260_OIS)
    STATE_ACC_CFG_CHECK_REGISTER,
#endif
#ifdef  BMI2XY_SELFTEST
    /* for acc */
    STATE_ACC_SELF_SET_ACC_CONF,
    STATE_ACC_SELF_SET_ACC_CHECK,
    STATE_ACC_SELF_SET_PWOER_MODE,
    STATE_ACC_SELF_GET_ACC_POSITIVE,
    STATE_ACC_SELF_GET_ACC_NEGATIVE,
    STATE_ACC_SELF_GET_ACC_SELFVALUE,
    STATE_ACC_SELF_TEST_RECOVER,
    STATE_ACC_SELF_TEST_INIT_REG,
    STATE_ACC_SELF_TEST_INIT_CHANGE_SENSORS,
    STATE_ACC_SELF_TEST_POWER_ON,
    /* for gyro */
    STATE_GYRO_SELF_TEST,
    STATE_GYRO_SELF_ENABLE_SELF,
    STATE_GYRO_SELF_VALIDATE_VALUE,
    STATE_GYRO_SELF_GET_GYRO_SELFVALUE,
#endif
};

static uint16_t calcWatermark(uint32_t numOfData)
{
    uint16_t watermark;
    // Data size
    watermark = numOfData * 6;

    // Header Size
    watermark += numOfData * max(mTask.sensors[ACC].rate, mTask.sensors[GYR].rate) / (mTask.sensors[ACC].rate + mTask.sensors[GYR].rate);
    //if ((numOfData == 1) &&(mTask.sensors[ACC].rate != 0) && (mTask.sensors[GYR].rate != 0) && (mTask.sensors[ACC].configed != 0) && (mTask.sensors[GYR].configed != 0))
   // {
    //    watermark = 28;
   // }
    if ((numOfData == 1) && (mTask.sensors[GYR].rate >= 203900) && (mTask.sensors[GYR].configed != 0))
    {
        watermark = 14;
    }
    osLog(LOG_ERROR,"acc = %d(%ld), gyro = %d(%ld), numOfData = %ld, wm = %d\n",
        mTask.sensors[ACC].configed, mTask.sensors[ACC].rate,
        mTask.sensors[GYR].configed, mTask.sensors[GYR].rate,
        numOfData, watermark);
    /* here make sure the frame count not more than 100 */
    if (watermark > 700) {
        watermark = 700;
    }
    return watermark;
}

static void spiQueueWrite(uint8_t addr, uint8_t data, uint32_t delay) {
    TDECL();
    /*if (T(spiInUse)) {
        ERROR_PRINT("SPI in use, cannot queue write\n");
        return;
    }*/
    T(packets[T(mRegCnt)]).size = 2;
    T(packets[T(mRegCnt)]).txBuf = &T(txrxBuffer[T(mWbufCnt)]);
    T(packets[T(mRegCnt)]).rxBuf = &T(txrxBuffer[T(mWbufCnt)]);
    T(packets[T(mRegCnt)]).delay = delay * 1000;
    T(txrxBuffer[T(mWbufCnt++)]) = BMI2XY_SPI_WRITE | addr;
    T(txrxBuffer[T(mWbufCnt++)]) = data;
    T(mWbufCnt) = (T(mWbufCnt) + 3) & 0xFFFC;
    T(mRegCnt)++;
}

static void spiQueueWriteBlock(uint8_t *data, uint32_t len, uint32_t delay)
{
    mTask.packets[mTask.mRegCnt].size = len;
    mTask.packets[mTask.mRegCnt].txBuf = data;
    mTask.packets[mTask.mRegCnt].rxBuf = bmi220_config_file_back;
    mTask.packets[mTask.mRegCnt].delay = delay * 1000;
    mTask.mRegCnt++;
}

/*
 * need to be sure size of buf is larger than read size
 */
static void spiQueueRead(uint8_t addr, size_t size, uint8_t **buf, uint32_t delay)
{
    TDECL();
    *buf = &T(txrxBuffer[T(mWbufCnt)]);
    T(packets[T(mRegCnt)]).size = size + 1;  // first byte will not contain valid data
    T(packets[T(mRegCnt)]).txBuf = &T(txrxBuffer[T(mWbufCnt)]);
    T(packets[T(mRegCnt)]).rxBuf = *buf;
    T(packets[T(mRegCnt)]).delay = delay * 1000;
    T(txrxBuffer[T(mWbufCnt)++]) = BMI2XY_SPI_READ | addr;
    T(mWbufCnt) = (T(mWbufCnt) + size + 3) & 0xFFFC;
    T(mRegCnt)++;
}

static void spiBatchTxRx(struct SpiMode *mode,
        SpiCbkF callback, void *cookie, const char * src) {
    TDECL();
    if (T(mWbufCnt) > SPI_BUF_SIZE) {
        ERROR_PRINT("NO enough SPI buffer space, dropping transaction.\n");
        return;
    }
    if (T(mRegCnt) > SPI_PACKET_SIZE) {
        ERROR_PRINT("spiBatchTxRx too many packets!\n");
        return;
    }

    // T(spiInUse) = true;
    spiMasterRxTx(T(spiDev), T(cs), T(packets), T(mRegCnt), mode, callback, cookie);
    T(mRegCnt) = 0;
    T(mWbufCnt) = 0;
}
#if !defined(SUPPORT_MT6885)
static void bmi2xyIsr1(int arg)
#else
SRAM_REGION_FUNCTION static void bmi2xyIsr1(int arg)
#endif
{
    if (mTask.latch_time_id < 0) {
        mTask.hwSampleTime = rtcGetTime();
    } else {
        mTask.hwSampleTime = get_latch_time_timestamp(mTask.latch_time_id);
    }
    if ((mTask.debug_trace & 0x02) == 0x02) {
        osLog(LOG_ERROR, "bmi2xyIsr1, %lld\n", mTask.hwSampleTime);
    }
#if !defined(SUPPORT_MT6885)
    mark_timestamp(SENS_TYPE_ACCEL, SENS_TYPE_ACCEL, INT_ISR, mTask.hwSampleTime);
    mark_timestamp(SENS_TYPE_GYRO, SENS_TYPE_GYRO, INT_ISR, mTask.hwSampleTime);
#endif
    accGyroHwIntCheckStatus();
}

static inline bool anyFifoEnabled(void) {
    return (mTask.fifo_enabled[ACC] || mTask.fifo_enabled[GYR]);
}

static void configFifo(void)
{
    TDECL();
    uint8_t data[2] = {0};
    uint8_t val = 0x10;

    if (mTask.watermark == 0) {
        mTask.watermark = 1;
    }
    /* config the fifo config register for acc gyro fifo enable bit*/
    SPI_WRITE(BMI2_FIFO_CONFIG_1_ADDR, val, 5000);
    /* cleanup the fifo and invalidate time*/
    SPI_WRITE(BMI2_CMD_REG_ADDR, 0xb0, 5000);
    /* if ACC is configed, enable ACC bit in fifo_config reg.*/
    if (mTask.sensors[ACC].configed) {/* && mTask.sensors[ACC].latency != SENSOR_LATENCY_NODATA) {*/
        val |= 0x40;
        mTask.fifo_enabled[ACC] = true;
    } else {
        /*disable ACC bit in fifo_config reg.*/
        val = val & 0xBF;
        mTask.fifo_enabled[ACC] = false;
    }

    /* if GYR is configed, enable GYR bit in fifo_config reg.*/
    if (mTask.sensors[GYR].configed) {/* && mTask.sensors[GYR].latency != SENSOR_LATENCY_NODATA) {*/
        val |= 0x80;
        mTask.fifo_enabled[GYR] = true;
    } else {
       /* disable GYR bit in fifo_config reg.*/
        val = val & 0x7F;
        mTask.fifo_enabled[GYR] = false;
    }

    /* config the watermark */
    data[0] = BMI2_GET_LSB(mTask.watermark);
    data[1] = BMI2_GET_MSB(mTask.watermark);
    SPI_WRITE(BMI2_FIFO_WTM_0_ADDR, data[0], 1000);
    SPI_WRITE(BMI2_FIFO_WTM_1_ADDR, data[1], 1000);

    /* config the fifo config register for acc gyro fifo enable bit*/
    SPI_WRITE(BMI2_FIFO_CONFIG_1_ADDR, val, 1000);
}

static int accPowerOn(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint8_t val = 0x0c;
    if (mTask.sensors[GYR].powered == 1) {
            val = val | 0x02;/* bit 3 temp, bit 2 acc bit 1 gyr bit 0 aux */
    }
    /* enable the acc power bit set acc normal power mode*/
    SPI_WRITE(BMI2_PWR_CTRL_ADDR, val, 5000);
    osLog(LOG_ERROR, "bmi2xy accPowerOn\n");
    mTask.sensors[ACC].powered = 1;
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;
}

static int accPowerOff(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    // set ACC power mode to SUSPEND
    int ret = 0;
    uint8_t val = 0x00;
    struct accGyroCntlPacket cntlPacket;

    ret = rxControlInfo(&cntlPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);
    if (ret < 0) {
        sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "gyrPowerOff, rx inSize and elemSize error\n");
        return -1;
    }

    mTask.sensors[ACC].configed = false;
    mTask.sensors[ACC].rate = 0;
    registerAccGyroFifoInfo((mTask.sensors[ACC].rate == 0) ? 0 : 1024000000000 / mTask.sensors[ACC].rate,
        (mTask.sensors[GYR].rate == 0) ? 0 : 1024000000000 / mTask.sensors[GYR].rate);
    mTask.watermark = calcWatermark(cntlPacket.waterMark);
    configFifo();
    if (mTask.sensors[ANYMO].powered) {
        val = val | 0x0c;
    }
#ifdef CFG_HW_STEP_COUNTER_SUPPORT
    if (mTask.sensors[STEPCNT].powered) {
        val = val | 0x0c;
    }
#endif
    if (mTask.sensors[GYR].powered) {
        val = val | 0x0A;
    }
    SPI_WRITE(BMI2_PWR_CTRL_ADDR, val, 5000); /* set acc to suspend mode */
    osLog(LOG_ERROR, "bmi2xy accPowerOff, enter acc suspend mode\n");
    mTask.sensors[ACC].powered = 0;
    if ((mTask.debug_trace & 0x01) == 0x01) {
        SPI_READ(BMI2_ACC_CONF_ADDR, 11, &mTask.reg40_4A);
        SPI_READ(BMI2_INT1_IO_CTRL_ADDR, 8, &mTask.reg53_59);
        SPI_READ(BMI2_PWR_CONF_ADDR, 3, &mTask.reg7c_7d);
        SPI_READ(BMI2_FIFO_LENGTH_0_ADDR, 3, &mTask.reg24_25);
    }
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);

    return 0;
}

static int accGyroUnmaskEint(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    if ((mTask.debug_trace & 0x01) == 0x01) {

        osLog(LOG_ERROR, "acc off power reg addr:0x40[%x]0x41[%x]0x42[%x]0x43[%x]0x44[%x]0x45[%x]0x46[%x]\n",
        mTask.reg40_4A[2], mTask.reg40_4A[3], mTask.reg40_4A[4], mTask.reg40_4A[5], mTask.reg40_4A[6], mTask.reg40_4A[7], mTask.reg40_4A[8]);
        osLog(LOG_ERROR, "acc off power reg addr:0x47[%x]0x48[%x]0x49[%x]0x4A[%x]\n",
        mTask.reg40_4A[9], mTask.reg40_4A[10], mTask.reg40_4A[11], mTask.reg40_4A[12]);
        osLog(LOG_ERROR, "acc off power reg addr:0x56[%x]0x57[%x]0x58[%x]0x7c[%x]0x7d[%x] 0x24[%x] 0x25[%x]\n",
        mTask.reg53_59[5], mTask.reg53_59[6], mTask.reg53_59[7], mTask.reg7c_7d[2], mTask.reg7c_7d[3], mTask.reg24_25[2], mTask.reg24_25[3]);
    }

    /* unmask eint to prevent eint been masked due to in time acc/gyro disable but some motion still need */
    mt_eint_unmask(mTask.hw->eint_num);
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int gyrPowerOn(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint8_t val = 0;
#ifdef CFG_HW_STEP_COUNTER_SUPPORT
    if (!mTask.sensors[ANYMO].powered
        && !mTask.sensors[STEPCNT].powered
        && !mTask.sensors[ACC].powered)
#else
    if (!mTask.sensors[ANYMO].powered
        && !mTask.sensors[ACC].powered)
#endif
    {
            val = 0x0A;/* bit 3 temp, bit 2 acc bit 1 gyr bit 0 aux */
    }
    else
    {
            val = 0x0E;/* bit 3 temp, bit 2 acc bit 1 gyr bit 0 aux */
     }
    osLog(LOG_ERROR, "bmi2xy gyrPoweron\n");
    /* set gyro to normal power mode */
    SPI_WRITE(BMI2_PWR_CTRL_ADDR, val, 5000);
    if (anyFifoEnabled() && (1 != mTask.sensors[GYR].powered)) {
#if TIMESTAMP_DBG
        DEBUG_PRINT("minimize_sensortime_history()\n");
#endif
    }
    mTask.sensors[GYR].powered = 1;
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);

    return 0;
}

static int gyrPowerOff(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    // set GYR power mode to SUSPEND
    int ret = 0;
    uint8_t val = 0;
    struct accGyroCntlPacket cntlPacket;

    ret = rxControlInfo(&cntlPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);
    if (ret < 0) {
        sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "gyrPowerOff, rx inSize and elemSize error\n");
        return -1;
    }

    mTask.sensors[GYR].configed = false;
    mTask.sensors[GYR].rate = 0;
    registerAccGyroFifoInfo((mTask.sensors[ACC].rate == 0) ? 0 : 1024000000000 / mTask.sensors[ACC].rate,
        (mTask.sensors[GYR].rate == 0) ? 0 : 1024000000000 / mTask.sensors[GYR].rate);
    mTask.watermark = calcWatermark(cntlPacket.waterMark);
    configFifo();
   #ifdef CFG_HW_STEP_COUNTER_SUPPORT
    if (!mTask.sensors[ANYMO].powered
        && !mTask.sensors[STEPCNT].powered
        && !mTask.sensors[ACC].powered)
    #else
    if (!mTask.sensors[ANYMO].powered
        && !mTask.sensors[ACC].powered)
    #endif
    {
        val = 0x00;/* bit 3 temp, bit 2 acc bit 1 gyr bit 0 aux */
    }
    else
    {
        val = 0x0C;/* bit 3 temp, bit 2 acc bit 1 gyr bit 0 aux */
    }
    /* set gyro to suspend mode */
    SPI_WRITE(BMI2_PWR_CTRL_ADDR, val, 5000);
    osLog(LOG_ERROR, "bmi2xy gyrPowerOff\n");
    if (anyFifoEnabled() && 0 != mTask.sensors[GYR].powered) {
#if TIMESTAMP_DBG
        DEBUG_PRINT("minimize_sensortime_history()\n");
#endif
    }

    mTask.sensors[GYR].powered = 0;
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);

    return 0;
}

static int hwIntHandling(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{

    union EmbeddedDataPoint trigger_axies;
    uint8_t int_status_0 = mTask.statusBuffer[2];
    uint8_t int_status_1 = mTask.statusBuffer[3];
    if ((mTask.debug_trace & 0x02) == 0x02) {
        osLog(LOG_ERROR, "int 1 %d, int 2 %d\n", int_status_0, int_status_1);
    }
    if ((int_status_0 & INT_ANY_MOTION) && mTask.sensors[ANYMO].powered) {
        trigger_axies.idata = 7;
        osLog(LOG_ERROR,"Detected any motion\n");
        osLog(LOG_ERROR, "int 1 %d, int 2 %d\n", int_status_0, int_status_1);
        osEnqueueEvt(EVT_SENSOR_ANY_MOTION, trigger_axies.vptr, NULL);
    }
    if (int_status_1 & INT_FIFO_WM) {
        /* INFO_PRINT("Detected fifo watermark\n"); */
        accGyroInterruptOccur();
    } else {
        /* If not enter accGyroInterruptOccur by INT_FIFO_WM, need unmask eint here */
        mt_eint_unmask(mTask.hw->eint_num);
    }
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int hwIntStatusCheck(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    /* get the interrupt status */
    SPI_READ(BMI2_INT_STATUS_0_ADDR, 3, &mTask.statusBuffer);
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;
}

static int anyMotionPowerOn(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    if (!mTask.sensors[ACC].powered)
    {
        if (mTask.sensors[GYR].powered)
        {
            SPI_WRITE(BMI2_PWR_CTRL_ADDR, 0x0E, 5000);
        }
        else
        {
            SPI_WRITE(BMI2_PWR_CTRL_ADDR, 0x04, 5000);
        }
        osLog(LOG_ERROR, "anyMotionPowerOn, enter acc low power mode\n");
    }
    osLog(LOG_ERROR, "anyMotionPowerOn\n");
    mTask.sensors[ANYMO].powered = true;
    /* map the anymotion to the int 1 */
    SPI_WRITE(BMI2_INT1_MAP_FEAT_ADDR, 0x40, 1000);
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;
}

static int anyMotionPowerOff(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                             void *inBuf, uint8_t inSize, uint8_t elemInSize,
                             void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.sensors[ANYMO].configed = false;
    mTask.sensors[ANYMO].powered = false;
    /* for accel disable */
#ifdef CFG_HW_STEP_COUNTER_SUPPORT
    if (!mTask.sensors[ACC].powered
        && !mTask.sensors[STEPCNT].powered) {
#else
    if (!mTask.sensors[ACC].powered) {
#endif
        if (mTask.sensors[GYR].powered)
        {
            SPI_WRITE(BMI2_PWR_CTRL_ADDR, 0x0A, 5000);
        }
         else
        {
            SPI_WRITE(BMI2_PWR_CTRL_ADDR, 0x00, 5000);
        }
        osLog(LOG_ERROR, "anyMotionPowerOff, enter acc suspend mode\n");
    }
    osLog(LOG_ERROR, "anyMotionPowerOff\n");
    /* disable map the anymotion to the int 1 */
    SPI_WRITE(BMI2_INT1_MAP_FEAT_ADDR, 0x00, 1000);
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;
}

#ifdef CFG_HW_STEP_COUNTER_SUPPORT
uint8_t step_enable_config[5]__attribute__((aligned(4))) = {0x30, 0x00, 0x10, 0x02, 0x07};
uint8_t step_disable_config[5]__attribute__((aligned(4))) = {0x30, 0x00, 0x00, 0x02, 0x07};
static int stepCountPowerOn(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
     osLog(LOG_INFO, "Enter stepCountPowerOn\n");
     uint8_t page = BMI2_PAGE_1;
#if defined(BMI220)
    page = BMI2_PAGE_1;
#endif

    if (!mTask.sensors[ACC].powered && !mTask.sensors[ANYMO].powered) {
        /* set accel to normal mode if accel is not enabled */
        SPI_WRITE(BMI2_PWR_CTRL_ADDR, 0x04, 5000);
        osLog(LOG_ERROR, "stepCountPowerOn, enter acc low power mode\n");
    }
    /* Set the page */
    SPI_WRITE(BMI2_FEAT_PAGE_ADDR, page, 1000);
    /* enable the stepcounter config */
#if defined(BMI220)
    spiQueueWriteBlock(step_enable_config, 5, 1000);/* enable stepcounter*/
#endif
    /* enable stepcount interrupt */
    mTask.sensors[STEPCNT].powered = true;
    /* get the config value from featurein ox5e register */
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;

}

static int stepCountPowerOff(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                             void *inBuf, uint8_t inSize, uint8_t elemInSize,
                             void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    osLog(LOG_INFO, "Enter stepCountPowerOff\n");
    uint8_t val = 0x00;
    uint8_t page = BMI2_PAGE_1;
#if defined(BMI220)
    page = BMI2_PAGE_1;
#endif
    mTask.sensors[STEPCNT].configed = false;
    mTask.sensors[STEPCNT].powered = false;

    /* Set the page */
    SPI_WRITE(BMI2_FEAT_PAGE_ADDR, page, 1000);
    /* enable the stepcounter config */
#if defined(BMI220)
    spiQueueWriteBlock(step_disable_config, 5, 1000);/* disable stepcounter*/
#endif

    /* for accel disable */
    if (mTask.sensors[ACC].powered | mTask.sensors[ANYMO].powered) {
        val = val | 0x0c;
        osLog(LOG_ERROR, "should set acc power bit 1\n");
    }
   /* for gyro disable */
    if (mTask.sensors[GYR].powered) {
        val = val | 0x02;
        osLog(LOG_ERROR, "should set gyro power bit 1\n");
    }
    SPI_WRITE(BMI2_PWR_CTRL_ADDR, val, 5000);
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;

}

#endif


#ifdef CFG_HW_STEP_COUNTER_SUPPORT
static int StepCntSendData(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    union EmbeddedDataPoint step_cnt;
    uint32_t cur_step_cnt;
    cur_step_cnt = (int)(mTask.stepBuffer[2] | (mTask.stepBuffer[3] << 8) | mTask.stepBuffer[4] << 16 | (mTask.stepBuffer[5] << 24));
    osLog(LOG_INFO, "StepCntSendData buf2= 0x%x,buf3=0x%x buf4= 0x%x,buf5=0x%x\n",
        mTask.stepBuffer[2], mTask.stepBuffer[3], mTask.stepBuffer[4], mTask.stepBuffer[5]);

    if (cur_step_cnt != mTask.last_step_cnt) {
        /* Check for possible overflow */
        if (cur_step_cnt < mTask.last_step_cnt) {
            mTask.total_step_cnt += cur_step_cnt + (0xFFFF - mTask.last_step_cnt);
        } else {
            mTask.total_step_cnt += (cur_step_cnt - mTask.last_step_cnt);
        }
        mTask.last_step_cnt = cur_step_cnt;
        step_cnt.idata = mTask.total_step_cnt;
        osLog(LOG_INFO, "step count =%ld\n", step_cnt.idata);
        osEnqueueEvt(EVT_SENSOR_STEP_COUNTER, step_cnt.vptr, NULL);
    }

   sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
   return 0;
}

static int stepCntGetData(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint8_t page = BMI2_PAGE_0;
    osLog(LOG_INFO, "stepCntGetData\n");
    /* Set the page */
    SPI_WRITE(BMI2_FEAT_PAGE_ADDR, page, 1000);
    /* get the step counter value */
    SPI_READ(BMI2_FEATURES_REG_ADDR, 5, &mTask.stepBuffer);
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;
}
#endif

static void updateTimeDelta(uint8_t idx, uint8_t odr) {
}

// compute the register value from sensor rate.
static uint8_t computeOdr(uint32_t *rate) {
    uint8_t odr = 0x00;

    if (*rate > SENSOR_HZ(1600))
        *rate = SENSOR_HZ(3200);
    else if (*rate > SENSOR_HZ(800))
        *rate = SENSOR_HZ(1600);
    else if (*rate > SENSOR_HZ(400))
        *rate = SENSOR_HZ(800);
    else if (*rate > SENSOR_HZ(200))
        *rate = SENSOR_HZ(400);
    else if (*rate > SENSOR_HZ(100))
        *rate = SENSOR_HZ(200);
    else if (*rate > SENSOR_HZ(50))
        *rate = SENSOR_HZ(100);
    else if (*rate > SENSOR_HZ(25))
        *rate = SENSOR_HZ(50);
    else if (*rate > SENSOR_HZ(25.0f/2.0f))
        *rate = SENSOR_HZ(25);
    else if (*rate > SENSOR_HZ(25.0f/4.0f))
        *rate = SENSOR_HZ(25.0f/2.0f);
    else if (*rate > SENSOR_HZ(25.0f/8.0f))
        *rate = SENSOR_HZ(25.0f/4.0f);
    else if (*rate > SENSOR_HZ(25.0f/16.0f))
        *rate = SENSOR_HZ(25.0f/8.0f);
    else if (*rate > SENSOR_HZ(25.0f/32.0f))
        *rate = SENSOR_HZ(25.0f/16.0f);
    else
        *rate = SENSOR_HZ(25.0f/32.0f);

    switch (*rate) {
    // fall through intended to get the correct register value
    case SENSOR_HZ(3200): odr++;
    case SENSOR_HZ(1600): odr++;
    case SENSOR_HZ(800): odr++;
    case SENSOR_HZ(400): odr++;
    case SENSOR_HZ(200): odr++;
    case SENSOR_HZ(100): odr++;
    case SENSOR_HZ(50): odr++;
    case SENSOR_HZ(25): odr++;
    case SENSOR_HZ(25.0f/2.0f): odr++;
    case SENSOR_HZ(25.0f/4.0f): odr++;
    case SENSOR_HZ(25.0f/8.0f): odr++;
    case SENSOR_HZ(25.0f/16.0f): odr++;
    case SENSOR_HZ(25.0f/32.0f): odr++;
    default:
        return odr;
    }
}
static int accSetRate(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    TDECL();
    int odr = 0;
    int ret = 0;
    struct accGyroCntlPacket cntlPacket;

    ret = rxControlInfo(&cntlPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);
    if (ret < 0) {
        sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "accSetRate, rx inSize and elemSize error\n");
        return -1;
    }

    mTask.sensors[ACC].rate = cntlPacket.rate;
    mTask.sensors[ACC].latency = SENSOR_LATENCY_NODATA;  // latency;
    // odr = computeOdr(cntlPacket.rate);
    odr = computeOdr(&mTask.sensors[ACC].rate);
    if (!odr) {
        ERROR_PRINT("invalid acc rate\n");
        return false;
    }
    osLog(LOG_ERROR, "bmi2 acc Rate acc rate:%ld, latency:%lld, watermark:%d\n",
        mTask.sensors[ACC].rate, mTask.sensors[ACC].latency, mTask.watermark);
    registerAccGyroFifoInfo((mTask.sensors[ACC].rate == 0) ? 0 : 1024000000000 / mTask.sensors[ACC].rate,
        (mTask.sensors[GYR].rate == 0) ? 0 : 1024000000000 / mTask.sensors[GYR].rate);

    updateTimeDelta(ACC, odr);

    // minimum supported rate for ACCEL is 12.5Hz.
    // Anything lower than that shall be acheived by downsampling.
      // if (odr < ACC_MIN_RATE) {
      //      osr = ACC_MIN_RATE - odr;
        //    odr = ACC_MIN_RATE;
        //}

    // for high odrs, oversample to reduce hw latency and downsample
        // to get desired odr
        //if (odr > OSR_THRESHOLD) {
        //    osr = (ACC_MAX_OSR + odr) > ACC_MAX_RATE ? (ACC_MAX_RATE - odr) : ACC_MAX_OSR;
         //   odr += osr;
        //}
    //}

    mTask.sensors[ACC].configed = true;
    mTask.acc_downsample = 0;

    // configure ANY_MOTION and NO_MOTION based on odr
    //configMotion(odr);

    // set ACC bandwidth parameter to 2 (bits[4:6])
    // set the rate (bits[0:3])
    SPI_WRITE(BMI2_ACC_CONF_ADDR, 0xA0 | odr);

    mTask.watermark = calcWatermark(cntlPacket.waterMark);
    osLog(LOG_ERROR, "bmi2 AccRate acc hwRate:%ld regValue %d wm = %d\n", mTask.sensors[ACC].rate, (0xA0 | odr), mTask.watermark);

    // flush the data and configure the fifo
    configFifo();
    mt_eint_unmask(mTask.hw->eint_num);
    if ((mTask.debug_trace & 0x01) == 0x01) {
        SPI_READ(BMI2_ACC_CONF_ADDR, 11, &mTask.reg40_4A);
        SPI_READ(BMI2_INT1_IO_CTRL_ADDR, 8, &mTask.reg53_59);
        SPI_READ(BMI2_PWR_CONF_ADDR, 3, &mTask.reg7c_7d);
        SPI_READ(BMI2_FIFO_LENGTH_0_ADDR, 3, &mTask.reg24_25);
    }
    spiBatchTxRx(&mTask.mode, spiCallBack/*sensorSpiCallback*/, next_state/*&mTask.sensors[ACC]*/, __FUNCTION__);
    return true;
}

static int bmi2xyAccRegisterCheck(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    if ((mTask.debug_trace & 0x01) == 0x01) {

        osLog(LOG_ERROR, "accrate reg addr:0x40[%x]0x41[%x]0x42[%x]0x43[%x]0x44[%x]0x45[%x]0x46[%x]\n",
        mTask.reg40_4A[2], mTask.reg40_4A[3], mTask.reg40_4A[4], mTask.reg40_4A[5], mTask.reg40_4A[6], mTask.reg40_4A[7], mTask.reg40_4A[8]);
        osLog(LOG_ERROR, "accrate reg addr:0x47[%x]0x48[%x]0x49[%x]0x4A[%x]\n",
        mTask.reg40_4A[9], mTask.reg40_4A[10], mTask.reg40_4A[11], mTask.reg40_4A[12]);
        osLog(LOG_ERROR, "accrate reg addr:0x56[%x]0x57[%x]0x58[%x]0x7c[%x]0x7d[%x] 0x24[%x] 0x25[%x]\n",
        mTask.reg53_59[5], mTask.reg53_59[6], mTask.reg53_59[7], mTask.reg7c_7d[2], mTask.reg7c_7d[3], mTask.reg24_25[2], mTask.reg24_25[3]);

    }
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int gyrSetRate(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    TDECL();
    int odr, osr = 0;
    int ret = 0;
    struct accGyroCntlPacket cntlPacket;

    ret = rxControlInfo(&cntlPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);
    if (ret < 0) {
        sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "gyrSetRate, rx inSize and elemSize error\n");
        return -1;
    }

    mTask.sensors[GYR].rate = cntlPacket.rate;
    mTask.sensors[GYR].latency = SENSOR_LATENCY_NODATA;  // latency;
    // odr = computeOdr(cntlPacket.rate);
    odr = computeOdr(&mTask.sensors[GYR].rate);
    if (!odr) {
        ERROR_PRINT("invalid gyr rate\n");
        return false;
    }
    osLog(LOG_ERROR, "bmi2 gyro Rate acc rate:%ld, latency:%lld, watermark:%d\n",
        mTask.sensors[GYR].rate, mTask.sensors[GYR].latency, mTask.watermark);

    registerAccGyroFifoInfo((mTask.sensors[ACC].rate == 0) ? 0 : 1024000000000 / mTask.sensors[ACC].rate,
        (mTask.sensors[GYR].rate == 0) ? 0 : 1024000000000 / mTask.sensors[GYR].rate);

    updateTimeDelta(GYR, odr);

    // minimum supported rate for GYRO is 25.0Hz.
    // Anything lower than that shall be acheived by downsampling.
    if (odr < GYR_MIN_RATE) {
        osr = GYR_MIN_RATE - odr;
        odr = GYR_MIN_RATE;
    } else {
        osr = 0;
    }

    mTask.sensors[GYR].configed = true;
    mTask.gyr_downsample = osr;

    // set GYR bandwidth parameter to 2 (bits[4:6])
    // set the rate (bits[0:3])
    SPI_WRITE(BMI2_GYR_CONF_ADDR, 0xE0 | odr);

    // configure down sampling ratio, 0x88 is to specify we are using
    // filtered samples
    SPI_WRITE(BMI2_FIFO_DOWNS_ADDR, (mTask.acc_downsample << 4) | mTask.gyr_downsample | 0x88);

    mTask.watermark = calcWatermark(cntlPacket.waterMark);
    osLog(LOG_ERROR, "bmi2 gyroRate :%ld regValue %d wm = %d\n", mTask.sensors[GYR].rate, (0xE0 | odr), mTask.watermark);

    // flush the data and configure the fifo
    configFifo();
    mt_eint_unmask(mTask.hw->eint_num);
    if ((mTask.debug_trace & 0x01) == 0x01) {
        SPI_READ(BMI2_ACC_CONF_ADDR, 11, &mTask.reg40_4A);
        SPI_READ(BMI2_INT1_IO_CTRL_ADDR, 8, &mTask.reg53_59);
        SPI_READ(BMI2_PWR_CONF_ADDR, 3, &mTask.reg7c_7d);
        SPI_READ(BMI2_FIFO_LENGTH_0_ADDR, 3, &mTask.reg24_25);
    }
    spiBatchTxRx(&mTask.mode, spiCallBack/*sensorSpiCallback*/, next_state/*&mTask.sensors[GYR]*/, __FUNCTION__);
    return true;
}

static int bmi2xyGyroRegisterCheck(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    if ((mTask.debug_trace & 0x01) == 0x01) {

        osLog(LOG_ERROR, "gyrorate reg addr:0x40[%x]0x41[%x]0x42[%x]0x43[%x]0x44[%x]0x45[%x]0x46[%x]\n",
        mTask.reg40_4A[2], mTask.reg40_4A[3], mTask.reg40_4A[4], mTask.reg40_4A[5], mTask.reg40_4A[6], mTask.reg40_4A[7], mTask.reg40_4A[8]);
        osLog(LOG_ERROR, "gyrorate reg addr:0x47[%x]0x48[%x]0x49[%x]0x4A[%x]\n",
        mTask.reg40_4A[9], mTask.reg40_4A[10], mTask.reg40_4A[11], mTask.reg40_4A[12]);
        osLog(LOG_ERROR, "gyrorate reg addr:0x56[%x]0x57[%x]0x58[%x]0x7c[%x]0x7d[%x] 0x24[%x] 0x25[%x]\n",
        mTask.reg53_59[5], mTask.reg53_59[6], mTask.reg53_59[7], mTask.reg7c_7d[2], mTask.reg7c_7d[3], mTask.reg24_25[2], mTask.reg24_25[3]);
    }
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int bmi2xyAccCali(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
#ifndef CFG_MTK_CALIBRATION_V1_0
    float bias[AXES_NUM] = {0};
#endif
    mTask.sensors[ACC].startCali = true;
    osLog(LOG_INFO, "bmi2xyAccCali\n");
#ifndef CFG_MTK_CALIBRATION_V1_0
    Acc_init_calibration(bias);
#else
    accFactoryCalibrateInit(&mTask.accFactoryCal);
#endif
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}


static int bmi2xyGyroCali(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
#ifndef CFG_MTK_CALIBRATION_V1_0
    float slope[AXES_NUM] = {0};
    float intercept[AXES_NUM] = {0};
#endif
    mTask.sensors[GYR].startCali = true;
    osLog(LOG_ERROR, "bmi2xyGyroCali\n");
#ifndef CFG_MTK_CALIBRATION_V1_0
    Gyro_init_calibration(slope, intercept);
#else
    gyroFactoryCalibrateInit(&mTask.gyroFactoryCal);
#endif
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;

}

static int bmi2xyAccCfgCali(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;
    struct accGyroCaliCfgPacket caliCfgPacket;
#if defined(BMI260_OIS)
    uint8_t page = BMI2_PAGE_2;
    uint8_t val = 0x00;
#endif
    ret = rxCaliCfgInfo(&caliCfgPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);

    if (ret < 0) {
        sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "accHwCaliCheck, rx inSize and elemSize error\n");
        return -1;
    }
    osLog(LOG_INFO, "bmi2xyAccCfgCali: cfgData[0]:%ld, cfgData[1]:%ld, cfgData[2]:%ld\n",
        caliCfgPacket.caliCfgData[0], caliCfgPacket.caliCfgData[1], caliCfgPacket.caliCfgData[2]);

    mTask.sensors[ACC].staticCali[0] = (float)caliCfgPacket.caliCfgData[0] / 1000;
    mTask.sensors[ACC].staticCali[1] = (float)caliCfgPacket.caliCfgData[1] / 1000;
    mTask.sensors[ACC].staticCali[2] = (float)caliCfgPacket.caliCfgData[2] / 1000;
    #if defined(SUPPORT_MT6885)
    accGyroSendCalibrationResult(SENS_TYPE_ACCEL, (int32_t *)&caliCfgPacket.caliCfgData[0], (uint8_t)STILL_DETECT);
    #endif
#if defined(BMI260_OIS)
    mTask.ois_config = 1;
#endif
    #if defined(BMI260_OIS)
    /*enable ois*/
    if (mTask.ois_config == 1) {
        osLog(LOG_ERROR, "bmi2xy config ois = %d\n", mTask.ois_config);
        /* Set the page */
        SPI_WRITE(BMI2_FEAT_PAGE_ADDR, page, 1000);
        spiQueueWriteBlock(bmi260_primary_ois_ctrl_enable, 3, 1000);/* enable primary ois ctl*/
        /*set the ois interface to 1*/
        val = 0x10;
        SPI_WRITE(BMI2_IF_CONF_ADDR, val, 1000);
    } else {
        osLog(LOG_ERROR, "bmi2xy config ois = %d\n", mTask.ois_config);
        /* set the page */
        SPI_WRITE(BMI2_FEAT_PAGE_ADDR, page, 1000);
        /* close the ois function */
        spiQueueWriteBlock(bmi260_primary_ois_ctrl_disable, 3, 1000);/* disable primary ois ctl*/
        /* close the ois interface */
        SPI_WRITE(BMI2_IF_CONF_ADDR, val, 1000);
    }
    /* get the ois from the page */
    SPI_READ(BMI2_FEATURES_REG_ADDR, 17, &mTask.config_stream_status);
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    #else
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    #endif
    return 0;
}
#if defined(BMI260_OIS)
static int bmi2xyAccCfgCaliCheck(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    osLog(LOG_ERROR, "OIS reg addr:30, data0x%x data0x%x data0x%x data0x%x data0x%x data0x%x data0x%x data0x%x\n",
        mTask.config_stream_status[2],mTask.config_stream_status[3],mTask.config_stream_status[4],mTask.config_stream_status[5],
        mTask.config_stream_status[6],mTask.config_stream_status[7],mTask.config_stream_status[8],mTask.config_stream_status[9]);
     //osLog(LOG_ERROR, "OIS reg addr:30, data0 %x\n", mTask.config_stream_status[2]);
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}
#endif

static int bmi2xyGyroCfgCali(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;
    struct accGyroCaliCfgPacket caliCfgPacket;

    ret = rxCaliCfgInfo(&caliCfgPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);

    if (ret < 0) {
        sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "gyroHwCaliCheck, rx inSize and elemSize error\n");
        return -1;
    }

    osLog(LOG_INFO, "bmi2xyGyroCfgCali: cfgData[0]:%ld, cfgData[1]:%ld, cfgData[2]:%ld\n",
            caliCfgPacket.caliCfgData[0], caliCfgPacket.caliCfgData[1], caliCfgPacket.caliCfgData[2]);

    mTask.sensors[GYR].staticCali[0] = (float)caliCfgPacket.caliCfgData[0] / 1000;
    mTask.sensors[GYR].staticCali[1] = (float)caliCfgPacket.caliCfgData[1] / 1000;
    mTask.sensors[GYR].staticCali[2] = (float)caliCfgPacket.caliCfgData[2] / 1000;
#if defined(SUPPORT_MT6885)
    accGyroSendCalibrationResult(SENS_TYPE_GYRO, (int32_t *)&caliCfgPacket.caliCfgData[0], (uint8_t)STILL_DETECT);
#endif
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static void spiIsrCallBack(void *cookie, int err)
{
    if (err != 0) {
        osLog(LOG_ERROR, "bmi2xy: spiIsrCallBack err\n");
        sensorFsmEnqueueFakeSpiEvt(mTask.spiCallBack, cookie, ERROR_EVT);
    } else {
        mTask.swSampleTime = rtcGetTime();
        sensorFsmEnqueueFakeSpiEvt(mTask.spiCallBack, cookie, SUCCESS_EVT);
    }
}

static int bmi2xySample(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    TDECL();
    int ret = 0;
#if !defined(SUPPORT_MT6885)
    mark_timestamp(SENS_TYPE_ACCEL, SENS_TYPE_ACCEL, SAMPLE_BEGIN, rtcGetTime());
    mark_timestamp(SENS_TYPE_GYRO, SENS_TYPE_GYRO, SAMPLE_BEGIN, rtcGetTime());
#endif
    ret = rxTransferDataInfo(&mTask.dataInfo, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);
    if (ret < 0) {
        sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "bmi160Sample, rx dataInfo error\n");
        return -1;
    }
    /* get the temperature and fifo length */
    SPI_READ((BMI2_INTERNAL_STATUS_ADDR + 1), 5, &mTask.sampleStatusBuffer);
    mTask.spiCallBack = spiCallBack;
    spiBatchTxRx(&mTask.mode, spiIsrCallBack, next_state, __FUNCTION__);
    return 0;
}

static int bmi2xyReadFifo(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.temperature = mTask.sampleStatusBuffer[2] | (mTask.sampleStatusBuffer[3] << 8);
    mTask.fifoLength = mTask.sampleStatusBuffer[4] | (mTask.sampleStatusBuffer[5] << 8);
    if ((mTask.debug_trace & 0x02) == 0x02) {
        osLog(LOG_ERROR, "temperature %d, fifo length %d\n", mTask.temperature, mTask.fifoLength);
    }
    /* get the fifo length */
    if (mTask.fifoLength > FIFO_READ_SIZE) {
        /* here
        MTK do not support the burst read length more than 1024,
        bmi260 fifo is 2048 bytes
        once detect the fifo length more than 1024, this will clear the fifo data , make sure the interrupt pin can become low
        also here set the read length 1024 bytes to avoid the clear the fifo not finished
        */
        osLog(LOG_ERROR, "long time do not handle the fifo data fifo length %d fifo watermark %d\n",mTask.fifoLength, mTask.watermark);
        mTask.fifoLength = FIFO_READ_SIZE;
        /* read the 896 byte data */
        SPI_READ(BMI2_FIFO_DATA_ADDR, mTask.fifoLength, &mTask.dataBuffer);
        /* cleanup the fifo*/
        SPI_WRITE(BMI2_CMD_REG_ADDR, 0xb0, 1000);
    } else {
        mTask.fifoLength = mTask.fifoLength + 32;
        SPI_READ(BMI2_FIFO_DATA_ADDR, mTask.fifoLength, &mTask.dataBuffer);
    }
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;
}

static void parseRawData(struct accGyroData *data, uint8_t *buf, float kScale, uint8_t sensorType)
{

    int16_t raw_data[AXES_NUM];
    int16_t remap_data[AXES_NUM];
    int32_t SwCali[AXES_NUM];
    int32_t caliResult[AXES_NUM] = {0};
    float temp_data[AXES_NUM] = {0};
    float calibrated_data_output[AXES_NUM] = {0};
    int16_t status = 0;
    memset(SwCali, 0, sizeof(SwCali));

    if (sensorType == SENS_TYPE_ACCEL) {
        accGetCalibration(SwCali, 0);
    }
    else if (sensorType == SENS_TYPE_GYRO) {
        gyroGetCalibration(SwCali, 0);
    }
    raw_data[AXIS_X] = (buf[0] | buf[1] << 8);
    raw_data[AXIS_Y] = (buf[2] | buf[3] << 8);
    raw_data[AXIS_Z] = (buf[4] | buf[5] << 8);
    /* osLog(LOG_ERROR,"ACCEL:raw_data_x=%d, raw_data_y=%d, raw_data_z=%d\n",
                    raw_data[AXIS_X], raw_data[AXIS_Y], raw_data[AXIS_Z]); */
    raw_data[AXIS_X] = raw_data[AXIS_X] + SwCali[AXIS_X];
    raw_data[AXIS_Y] = raw_data[AXIS_Y] + SwCali[AXIS_Y];
    raw_data[AXIS_Z] = raw_data[AXIS_Z] + SwCali[AXIS_Z];

    remap_data[mTask.cvt.map[AXIS_X]] = mTask.cvt.sign[AXIS_X] * raw_data[AXIS_X];
    remap_data[mTask.cvt.map[AXIS_Y]] = mTask.cvt.sign[AXIS_Y] * raw_data[AXIS_Y];
    remap_data[mTask.cvt.map[AXIS_Z]] = mTask.cvt.sign[AXIS_Z] * raw_data[AXIS_Z];

      /* osLog(LOG_ERROR,"ACCEL:remap_data=%d, remap_data=%d, remap_data=%d\n",
                    remap_data[AXIS_X], remap_data[AXIS_Y], remap_data[AXIS_Z]); */
    if (sensorType == SENS_TYPE_ACCEL) {
        temp_data[AXIS_X] = (float)remap_data[AXIS_X] * kScale;
        temp_data[AXIS_Y] = (float)remap_data[AXIS_Y] * kScale;
        temp_data[AXIS_Z] = (float)remap_data[AXIS_Z] * kScale;
        if (UNLIKELY(mTask.sensors[ACC].startCali)) {
#ifndef CFG_MTK_CALIBRATION_V1_0
            status = Acc_run_factory_calibration_timeout(0,
                temp_data, calibrated_data_output, (int *)&mTask.sensors[ACC].accuracy, rtcGetTime());
            if (status != 0) {
                mTask.sensors[ACC].startCali = false;
                if (status > 0) {
                    osLog(LOG_INFO, "ACC cali detect shake %lld\n", rtcGetTime());
                    caliResult[AXIS_X] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_X] * 1000);
                    caliResult[AXIS_Y] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_Y] * 1000);
                    caliResult[AXIS_Z] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_Z] * 1000);
                    accGyroSendCalibrationResult(SENS_TYPE_ACCEL, (int32_t *)&caliResult[0], (uint8_t)status);
                } else
                    osLog(LOG_INFO, "ACC cali time out %lld\n", rtcGetTime());
            } else if (mTask.sensors[ACC].accuracy == 3) {
                mTask.sensors[ACC].startCali = false;
                mTask.sensors[ACC].staticCali[AXIS_X] = calibrated_data_output[AXIS_X] - temp_data[AXIS_X];
                mTask.sensors[ACC].staticCali[AXIS_Y] = calibrated_data_output[AXIS_Y] - temp_data[AXIS_Y];
                mTask.sensors[ACC].staticCali[AXIS_Z] = calibrated_data_output[AXIS_Z] - temp_data[AXIS_Z];
                caliResult[AXIS_X] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_X] * 1000);
                caliResult[AXIS_Y] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_Y] * 1000);
                caliResult[AXIS_Z] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_Z] * 1000);
                accGyroSendCalibrationResult(SENS_TYPE_ACCEL, (int32_t *)&caliResult[0], (uint8_t)status);
                osLog(LOG_INFO,
                      "ACCEL cali done %lld:caliResult[0]:%ld, caliResult[1]:%ld, caliResult[2]:%ld, offset[0]:%f, offset[1]:%f, offset[2]:%f\n",
                      rtcGetTime(), caliResult[AXIS_X], caliResult[AXIS_Y], caliResult[AXIS_Z],
                      (double)mTask.sensors[ACC].staticCali[AXIS_X],
                      (double)mTask.sensors[ACC].staticCali[AXIS_Y],
                      (double)mTask.sensors[ACC].staticCali[AXIS_Z]);
            }
#else
            status = accFactoryCalibrateRun(&mTask.accFactoryCal,
                temp_data[AXIS_X], temp_data[AXIS_Y], temp_data[AXIS_Z], rtcGetTime());
            if (status == UNSTABLE_DETECT) {
                mTask.sensors[ACC].startCali = false;
                caliResult[AXIS_X] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_X] * 1000);
                caliResult[AXIS_Y] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_Y] * 1000);
                caliResult[AXIS_Z] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_Z] * 1000);
                accGyroSendCalibrationResult(SENS_TYPE_ACCEL, (int32_t *)&caliResult[0], (uint8_t)status);
        } else if (status == STILL_DETECT) {
                mTask.sensors[ACC].startCali = false;
                accFactoryCalibrateRemoveBias(&mTask.accFactoryCal,
                    temp_data[AXIS_X], temp_data[AXIS_Y], temp_data[AXIS_Z],
                    &calibrated_data_output[AXIS_X], &calibrated_data_output[AXIS_Y],
                    &calibrated_data_output[AXIS_Z]);
                accFactoryCalibrateGetBias(&mTask.accFactoryCal,
                    &mTask.sensors[ACC].staticCali[AXIS_X],
                    &mTask.sensors[ACC].staticCali[AXIS_Y],
                    &mTask.sensors[ACC].staticCali[AXIS_Z],
                    (int *)&mTask.sensors[ACC].accuracy);
                caliResult[AXIS_X] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_X] * 1000);
                caliResult[AXIS_Y] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_Y] * 1000);
                caliResult[AXIS_Z] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_Z] * 1000);
                accGyroSendCalibrationResult(SENS_TYPE_ACCEL, (int32_t *)&caliResult[0], (uint8_t)status);
                osLog(LOG_INFO, "accel cali bias: [%f, %f, %f]\n",
                    (double)mTask.sensors[ACC].staticCali[AXIS_X],
                    (double)mTask.sensors[ACC].staticCali[AXIS_Y],
                    (double)mTask.sensors[ACC].staticCali[AXIS_Z]);
            }
#endif
        }

        data->sensType = sensorType;
#ifndef CFG_MTK_CALIBRATION_V1_0
        data->x = temp_data[AXIS_X] + mTask.sensors[ACC].staticCali[AXIS_X];
        data->y = temp_data[AXIS_Y] + mTask.sensors[ACC].staticCali[AXIS_Y];
        data->z = temp_data[AXIS_Z] + mTask.sensors[ACC].staticCali[AXIS_Z];
#else
        data->x = temp_data[AXIS_X] - mTask.sensors[ACC].staticCali[AXIS_X];
        data->y = temp_data[AXIS_Y] - mTask.sensors[ACC].staticCali[AXIS_Y];
        data->z = temp_data[AXIS_Z] - mTask.sensors[ACC].staticCali[AXIS_Z];
#endif

        mTask.accFactoryData.ix = (int32_t)(data->x * ACCELEROMETER_INCREASE_NUM_AP);
        mTask.accFactoryData.iy = (int32_t)(data->y * ACCELEROMETER_INCREASE_NUM_AP);
        mTask.accFactoryData.iz = (int32_t)(data->z * ACCELEROMETER_INCREASE_NUM_AP);
    } else if (sensorType == SENS_TYPE_GYRO) {
        temp_data[AXIS_X] = (float)remap_data[AXIS_X] * kScale;
        temp_data[AXIS_Y] = (float)remap_data[AXIS_Y] * kScale;
        temp_data[AXIS_Z] = (float)remap_data[AXIS_Z] * kScale;
        if (UNLIKELY(mTask.sensors[GYR].startCali)) {
#ifndef CFG_MTK_CALIBRATION_V1_0
            status = Gyro_run_factory_calibration_timeout(0,
                temp_data, calibrated_data_output, (int *)&mTask.sensors[GYR].accuracy, 0, rtcGetTime());
            if (status != 0) {
                mTask.sensors[GYR].startCali = false;
                if (status > 0) {
                    osLog(LOG_INFO, "GYRO cali detect shake %lld\n", rtcGetTime());
                    caliResult[AXIS_X] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_X] * 1000);
                    caliResult[AXIS_Y] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_Y] * 1000);
                    caliResult[AXIS_Z] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_Z] * 1000);
                    accGyroSendCalibrationResult(SENS_TYPE_GYRO, (int32_t *)&caliResult[0], (uint8_t)status);
                } else
                    osLog(LOG_INFO, "GYRO cali time osssut %lld\n", rtcGetTime());
            } else if (mTask.sensors[GYR].accuracy == 3) {
                mTask.sensors[GYR].startCali = false;
                mTask.sensors[GYR].staticCali[AXIS_X] = calibrated_data_output[AXIS_X] - temp_data[AXIS_X];
                mTask.sensors[GYR].staticCali[AXIS_Y] = calibrated_data_output[AXIS_Y] - temp_data[AXIS_Y];
                mTask.sensors[GYR].staticCali[AXIS_Z] = calibrated_data_output[AXIS_Z] - temp_data[AXIS_Z];
                caliResult[AXIS_X] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_X] * 1000);
                caliResult[AXIS_Y] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_Y] * 1000);
                caliResult[AXIS_Z] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_Z] * 1000);
                accGyroSendCalibrationResult(SENS_TYPE_GYRO, (int32_t *)&caliResult[0], (uint8_t)status);
                osLog(LOG_INFO,
                      "GYRO cali done %lld: caliResult[0]:%ld, caliResult[1]:%ld, caliResult[2]:%ld, offset[0]:%f, offset[1]:%f, offset[2]:%f\n",
                      rtcGetTime(), caliResult[AXIS_X], caliResult[AXIS_Y], caliResult[AXIS_Z],
                      (double)mTask.sensors[GYR].staticCali[AXIS_X],
                      (double)mTask.sensors[GYR].staticCali[AXIS_Y],
                      (double)mTask.sensors[GYR].staticCali[AXIS_Z]);
            }
#else
            status = gyroFactoryCalibrateRun(&mTask.gyroFactoryCal,
                temp_data[AXIS_X], temp_data[AXIS_Y], temp_data[AXIS_Z], rtcGetTime());
            if (status == UNSTABLE_DETECT) {
                osLog(LOG_INFO, "GYRO UNSTABLE_DETECT %lld\n", rtcGetTime());
                mTask.sensors[GYR].startCali = false;
                caliResult[AXIS_X] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_X] * 1000);
                caliResult[AXIS_Y] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_Y] * 1000);
                caliResult[AXIS_Z] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_Z] * 1000);
                accGyroSendCalibrationResult(SENS_TYPE_GYRO, (int32_t *)&caliResult[0], (uint8_t)status);
            } else if (status == STILL_DETECT) {
                osLog(LOG_INFO, "GYRO STILL_DETECT %lld\n", rtcGetTime());
                mTask.sensors[GYR].startCali = false;
                gyroFactoryCalibrateRemoveBias(&mTask.gyroFactoryCal,
                    temp_data[AXIS_X], temp_data[AXIS_Y], temp_data[AXIS_Z],
                    &calibrated_data_output[AXIS_X], &calibrated_data_output[AXIS_Y],
                    &calibrated_data_output[AXIS_Z]);
                gyroFactoryCalibrateGetBias(&mTask.gyroFactoryCal,
                    &mTask.sensors[GYR].staticCali[AXIS_X],
                    &mTask.sensors[GYR].staticCali[AXIS_Y],
                    &mTask.sensors[GYR].staticCali[AXIS_Z],
                    (int *)&mTask.sensors[GYR].accuracy);
                caliResult[AXIS_X] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_X] * 1000);
                caliResult[AXIS_Y] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_Y] * 1000);
                caliResult[AXIS_Z] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_Z] * 1000);
                accGyroSendCalibrationResult(SENS_TYPE_GYRO, (int32_t *)&caliResult[0], (uint8_t)status);
                osLog(LOG_INFO, "gyro cali bias: [%f, %f, %f]\n",
                    (double)mTask.sensors[GYR].staticCali[AXIS_X],
                    (double)mTask.sensors[GYR].staticCali[AXIS_Y],
                    (double)mTask.sensors[GYR].staticCali[AXIS_Z]);
            }
#endif
        }

        data->sensType = sensorType;

#ifndef CFG_MTK_CALIBRATION_V1_0
        data->x = temp_data[AXIS_X] + mTask.sensors[GYR].staticCali[AXIS_X];
        data->y = temp_data[AXIS_Y] + mTask.sensors[GYR].staticCali[AXIS_Y];
        data->z = temp_data[AXIS_Z] + mTask.sensors[GYR].staticCali[AXIS_Z];
#else
        data->x = temp_data[AXIS_X] - mTask.sensors[GYR].staticCali[AXIS_X];
        data->y = temp_data[AXIS_Y] - mTask.sensors[GYR].staticCali[AXIS_Y];
        data->z = temp_data[AXIS_Z] - mTask.sensors[GYR].staticCali[AXIS_Z];
#endif
/*
        mTask.gyroFactoryData.ix =
            (int32_t)((float)remap_data[AXIS_X] * BMI2XY_FS_250_LSB / BMI2XY_FS_2000_LSB);
        mTask.gyroFactoryData.iy =
            (int32_t)((float)remap_data[AXIS_Y] * BMI2XY_FS_250_LSB / BMI2XY_FS_2000_LSB);
        mTask.gyroFactoryData.iz =
            (int32_t)((float)remap_data[AXIS_Z] * BMI2XY_FS_250_LSB / BMI2XY_FS_2000_LSB);
*/
    mTask.gyroFactoryData.ix =
        (int32_t)((float)data->x * GYROSCOPE_INCREASE_NUM_AP / DEGREE_TO_RADIRAN_SCALAR);
    mTask.gyroFactoryData.iy =
        (int32_t)((float)data->y * GYROSCOPE_INCREASE_NUM_AP / DEGREE_TO_RADIRAN_SCALAR);
    mTask.gyroFactoryData.iz =
        (int32_t)((float)data->z * GYROSCOPE_INCREASE_NUM_AP / DEGREE_TO_RADIRAN_SCALAR);
    }
    if (mTask.debug_trace) {
        switch (sensorType) {
        case SENS_TYPE_ACCEL:
            if ((mTask.debug_trace & 0x02) == 0x02) {
                osLog(LOG_ERROR, "ACCEL:raw_data_x=%f, raw_data_y=%f, raw_data_z=%f\n",
                    (double)data->x, (double)data->y, (double)data->z);
            }
            break;
        case SENS_TYPE_GYRO:
            if ((mTask.debug_trace & 0x02) == 0x02) {

            osLog(LOG_ERROR, "GYRO:raw_data_x=%f, raw_data_y=%f, raw_data_z=%f\n",
                (double)data->x, (double)data->y, (double)data->z);
            }
            break;
        default:
            break;
        }
    }
}

static int bmi2xyConvert(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    size_t i = 2, j;
    size_t size = mTask.fifoLength;
    int fh_mode, fh_param;
    uint8_t *buf = mTask.dataBuffer;

    uint64_t min_delta = ULONG_LONG_MAX;
    uint32_t sensor_time24;
    /* uint64_t full_sensor_time; */
    uint64_t frame_sensor_time = mTask.frame_sensortime;
    bool observed[2] = {false, false};
    uint64_t tmp_frame_time, tmp_time[3];
    bool frame_sensor_time_valid = true;  // mTask.frame_sensortime_valid;
#if TIMESTAMP_DBG
    int frame_num = -1;
#endif
    struct accGyroData *data = mTask.accGyroPacket.outBuf;
    uint8_t accEventSize = 0;
    uint8_t gyroEventSize = 0;
    uint64_t swSampleTime = 0, realSampleTime = 0;
#if !defined(SUPPORT_MT6885)
    int16_t temperature16 = 0;
#else
    int32_t temperature16 = 0;
    uint32_t accDelay = 0, gyroDelay = 0;
#endif
    float temperature = 0;

    if (!mTask.frame_sensortime_valid) {
        // This is the first FIFO delivery after any sensor is enabled in
        // bmi2xy. Sensor time reference is not establised until end of this
        // FIFO frame. Assume time start from zero and do a dry run to estimate
        // the time and then go through this FIFO again.
        frame_sensor_time = 0ull;
    }

    while (size > 0) {
        if (buf[i] == BMI2XY_FRAME_HEADER_INVALID) {
            // reaching invalid header means no more data
            break;
        } else if (buf[i] == BMI2XY_FRAME_HEADER_SKIP) {
            // manually injected skip header
            DEBUG_PRINT_IF(DBG_CHUNKED, "skip nop header");
            i++;
            size--;
            continue;
        }

        fh_mode = buf[i] >> 6;
        fh_param = (buf[i] >> 2) & 0xf;

        i++;
        size--;
#if TIMESTAMP_DBG
        ++frame_num;
#endif
        if (fh_mode == 1) {
            /* control frame */
            if (fh_param == 0) {
                /* skip frame, we skip it */
                if (size >= 1) {
                    i++;
                    size--;
                } else {
                    size = 0;
                }
            } else if (fh_param == 1) {
                /* sensortime frame */
                if (size >= 3) {
                    // The active sensor with the highest odr/lowest delta is the one that
                    // determines the sensor time increments.
                    for (j = ACC; j <= GYR; j++) {
                        if (mTask.sensors[j].configed &&
                                mTask.sensors[j].latency != SENSOR_LATENCY_NODATA) {
                            min_delta = min_delta < mTask.time_delta[j] ? min_delta :
                                    mTask.time_delta[j];
                        }
                    }
                    sensor_time24 = buf[i + 2] << 16 | buf[i + 1] << 8 | buf[i];

                    // clear lower bits that measure time from taking the sample to reading the
                    // FIFO, something we're not interested in.
                    sensor_time24 &= ~(min_delta - 1);
                    i += 3;
                    size -= 3;
                } else {
                    size = 0;
                }
            } else if (fh_param == 2) {
                /* fifo_input config frame */
#if TIMESTAMP_DBG
                DEBUG_PRINT("frame %d config change 0x%02x\n", frame_num, buf[i]);
#endif
                if (size >= 4) {
                    i = i + 4;
                    size -= 4;
                } else {
                    size = 0;
                }
            } else {
                size = 0;/* drop this batch */
                osLog(LOG_ERROR, "Invalid fh_param in conttrol frame\n");
            }
        } else if (fh_mode == 2) {
            // Calcutate candidate frame time (tmp_frame_time):
            // 1) When sensor is first enabled, reference from other sensors if possible.
            // Otherwise, add the smallest increment to the previous data frame time.
            // 2) The newly enabled sensor could only underestimate its
            // frame time without reference from other sensors.
            // 3) The underestimated frame time of a newly enabled sensor will be corrected
            // as soon as it shows up in the same frame with another sensor.
            // 4) (prev_frame_time == ULONG_LONG_MAX) means the sensor wasn't enabled.
            // 5) (prev_frame_time == ULONG_LONG_MAX -1) means the sensor didn't appear in the last
            // data frame of the previous fifo read.  So it won't be used as a frame time reference.

            tmp_frame_time = 0;
            for (j = ACC; j <= GYR; j++) {
                observed[j] = false;/* reset at each data frame */
                tmp_time[j] = 0;
                if ((mTask.prev_frame_time[j] < ULONG_LONG_MAX - 1) && (fh_param & (1 << j))) {
                    tmp_time[j] = mTask.prev_frame_time[j] + mTask.time_delta[j];
                    tmp_frame_time = (tmp_time[j] > tmp_frame_time) ? tmp_time[j] : tmp_frame_time;
                }
            }
            tmp_frame_time = (frame_sensor_time + kMinSensorTimeIncrement > tmp_frame_time)
                ? (frame_sensor_time + kMinSensorTimeIncrement) : tmp_frame_time;

            /* regular frame, dispatch data to each sensor's own fifo */
            if (fh_param & 4) {/* have mag data */
                osLog(LOG_ERROR, "Not support, should not enter here!\n");
            }
            if (fh_param & 2) {/* have gyro data */
                if (size >= 6) {
                    if (frame_sensor_time_valid) {
                        if ((accEventSize + gyroEventSize) < MAX_RECV_PACKET) {
                            parseRawData(&data[accEventSize + gyroEventSize], &buf[i], kScale_gyr, SENS_TYPE_GYRO);
                            gyroEventSize++;
                        } else
                            ERROR_PRINT("outBuf full, accEventSize = %d, gyroEventSize = %d\n",
                                accEventSize, gyroEventSize);
                    }
                    mTask.prev_frame_time[GYR] = tmp_frame_time;
                    i += 6;
                    size -= 6;
                    observed[GYR] = true;
                } else {
                    size = 0;
                }
            }
            if (fh_param & 1) {/* have accel data */
                if (size >= 6) {
                    if (frame_sensor_time_valid) {
                        if ((accEventSize + gyroEventSize) < MAX_RECV_PACKET) {
                            parseRawData(&data[accEventSize + gyroEventSize], &buf[i], kScale_acc, SENS_TYPE_ACCEL);
                            accEventSize++;
                        } else
                            ERROR_PRINT("outBuf full, accEventSize = %d, gyroEventSize = %d\n",
                                accEventSize, gyroEventSize);
                    }
                    mTask.prev_frame_time[ACC] = tmp_frame_time;
                    i += 6;
                    size -= 6;
                    observed[ACC] = true;
                } else {
                    size = 0;
                }
            }

            if (observed[ACC] || observed[GYR]) {
                frame_sensor_time = tmp_frame_time;
            }
        } else {
            size = 0;  // drop this batch
            osLog(LOG_ERROR, "Invalid fh_mode\n");
        }
    } /* while (size > 0) { */
#ifndef CFG_MTK_CALIBRATION_V1_0
    /*if factory true , can't send to runtime cali in parseRawData in accGyro*/
    if (mTask.sensors[ACC].startCali || mTask.sensors[GYR].startCali) {
        accEventSize = 0;
        gyroEventSize = 0;
        registerAccGyroFifoInfo((mTask.sensors[ACC].rate == 0) ? 0 : 1024000000000 / mTask.sensors[ACC].rate,
                                (mTask.sensors[GYR].rate == 0) ? 0 : 1024000000000 / mTask.sensors[GYR].rate);
    }
#endif
#if !defined(SUPPORT_MT6885)
    swSampleTime = mTask.swSampleTime;
    realSampleTime = calcFakeInterruptTime(swSampleTime, mTask.hwSampleTime, mTask.lastSampleTime,
        mTask.sensors[ACC].rate, mTask.sensors[ACC].configed, accEventSize,
        mTask.sensors[GYR].rate, mTask.sensors[GYR].configed, gyroEventSize);
    DEBUG_PRINT("bmi160Convert, swSampleTime=%lld, hwSampleTime=%lld, realSampleTime=%lld, lastSampleTime=%lld, now=%lld\n",
        swSampleTime, mTask.hwSampleTime, realSampleTime, mTask.lastSampleTime, rtcGetTime());
    mTask.hwSampleTime = realSampleTime;
    mTask.lastSampleTime = realSampleTime;
#else
    getRealSampleDelay(&accDelay, &gyroDelay);
    //swSampleTime = mTask.swSampleTime;
    if ((mTask.sensors[ACC].configed && !accDelay) || (mTask.sensors[GYR].configed && !gyroDelay))
        osLog(LOG_ERROR, "bmi2xyConvert, delay unreliable%u %u %u %u\n",
            mTask.sensors[ACC].configed, mTask.sensors[GYR].configed, accDelay, gyroDelay);

    realSampleTime = calcFakeInterruptTime3(&mTask.swSampleTime, &mTask.hwSampleTime, &mTask.lastSampleTime,
        accDelay, mTask.sensors[ACC].configed, accEventSize,
        gyroDelay, mTask.sensors[GYR].configed, gyroEventSize);
    DEBUG_PRINT("bmi160Convert, swSampleTime=%lld, hwSampleTime=%lld, realSampleTime=%lld, lastSampleTime=%lld, now=%lld\n",
        swSampleTime, mTask.hwSampleTime, realSampleTime, mTask.lastSampleTime, rtcGetTime());
    //mTask.hwSampleTime = realSampleTime;
    //mTask.lastSampleTime = realSampleTime;
#endif
    temperature16 = mTask.temperature;
    if (temperature16 == 0x8000) {
        temperature = kTempInvalid;
    } else {
        temperature = 23.0f + temperature16 * kScale_temp;
    }
    txTransferDataInfo(&mTask.dataInfo, accEventSize, gyroEventSize, realSampleTime, data, temperature);
    mt_eint_unmask(mTask.hw->eint_num);
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}


/* For Anymotion End */
static void accGetCalibration(int32_t *cali, int32_t size)
{
    cali[AXIS_X] = mTask.accSwCali[AXIS_X];
    cali[AXIS_Y] = mTask.accSwCali[AXIS_Y];
    cali[AXIS_Z] = mTask.accSwCali[AXIS_Z];
    /* osLog(LOG_INFO, "accGetCalibration cali x:%d, y:%d, z:%d\n", cali[AXIS_X], cali[AXIS_Y], cali[AXIS_Z]); */
}
#if !defined(SUPPORT_MT6885)
static void accSetCalibration(int32_t *cali, int32_t size)
{
    mTask.accSwCali[AXIS_X] = cali[AXIS_X];
    mTask.accSwCali[AXIS_Y] = cali[AXIS_Y];
    mTask.accSwCali[AXIS_Z] = cali[AXIS_Z];
    /* osLog(LOG_INFO, "accSetCalibration cali x:%d, y:%d, z:%d\n", mTask.accSwCali[AXIS_X],
        mTask.accSwCali[AXIS_Y], mTask.accSwCali[AXIS_Z]); */
}
#endif
#if !defined(SUPPORT_MT6885)
static void accGetData(void *sample)
{
    struct TripleAxisDataPoint *tripleSample = (struct TripleAxisDataPoint *)sample;
    tripleSample->ix = mTask.accFactoryData.ix;
    tripleSample->iy = mTask.accFactoryData.iy;
    tripleSample->iz = mTask.accFactoryData.iz;
}
#else
static void accGetData(uint32_t sequence, SCP_SENSOR_HUB_REQ *req,
                       int (*ack)(uint32_t, SCP_SENSOR_HUB_REQ *, void *, int))
{
    struct data_unit_t data;

    memset(&data, 0, sizeof(data));
    data.value[0] = mTask.accFactoryData.ix;
    data.value[1] = mTask.accFactoryData.iy;
    data.value[2] = mTask.accFactoryData.iz;
    ack(sequence, req, &data, 0);
}
#endif
static void gyroGetCalibration(int32_t *cali, int32_t size)
{
    cali[AXIS_X] = mTask.gyroSwCali[AXIS_X];
    cali[AXIS_Y] = mTask.gyroSwCali[AXIS_Y];
    cali[AXIS_Z] = mTask.gyroSwCali[AXIS_Z];
    /* osLog(LOG_INFO, "gyroGetCalibration cali x:%d, y:%d, z:%d\n", cali[AXIS_X], cali[AXIS_Y], cali[AXIS_Z]); */
}
#if !defined(SUPPORT_MT6885)
static void gyroSetCalibration(int32_t *cali, int32_t size)
{
    mTask.gyroSwCali[AXIS_X] = cali[AXIS_X];
    mTask.gyroSwCali[AXIS_Y] = cali[AXIS_Y];
    mTask.gyroSwCali[AXIS_Z] = cali[AXIS_Z];
    /* osLog(LOG_INFO, "gyroSetCalibration cali x:%d, y:%d, z:%d\n", mTask.gyroSwCali[AXIS_X],
        mTask.gyroSwCali[AXIS_Y], mTask.gyroSwCali[AXIS_Z]); */
}
#endif
#if !defined(SUPPORT_MT6885)
static void gyroGetData(void *sample)
{
    struct TripleAxisDataPoint *tripleSample = (struct TripleAxisDataPoint *)sample;
    tripleSample->ix = mTask.gyroFactoryData.ix;
    tripleSample->iy = mTask.gyroFactoryData.iy;
    tripleSample->iz = mTask.gyroFactoryData.iz;
}
#else
static void gyroGetData(uint32_t sequence, SCP_SENSOR_HUB_REQ *req,
                        int (*ack)(uint32_t, SCP_SENSOR_HUB_REQ *, void *, int))
{
    struct data_unit_t data;

    memset(&data, 0, sizeof(data));
    data.value[0] = mTask.gyroFactoryData.ix;
    data.value[1] = mTask.gyroFactoryData.iy;
    data.value[2] = mTask.gyroFactoryData.iz;
    ack(sequence, req, &data, 0);
}
#endif
#if !defined(SUPPORT_MT6885)
static void accGetSensorInfo(struct sensorInfo_t *data)
{
    strncpy(data->name, ACC_NAME, sizeof(data->name));
}

static void gyroGetSensorInfo(struct sensorInfo_t *data)
{
    strncpy(data->name, GYRO_NAME, sizeof(data->name));
}

#else
static void accGetSensorInfo(uint32_t sequence, SCP_SENSOR_HUB_REQ *req,
                             int (*ack)(uint32_t, SCP_SENSOR_HUB_REQ *, void *, int))
{
    struct sensorInfo_t info;

    memset(&info, 0, sizeof(info));
    strncpy(info.name, ACC_NAME, sizeof(info.name));
    ack(sequence, req, &info, 0);
}

static void gyroGetSensorInfo(uint32_t sequence, SCP_SENSOR_HUB_REQ *req,
                              int (*ack)(uint32_t, SCP_SENSOR_HUB_REQ *, void *, int))
{
    struct sensorInfo_t info;

    memset(&info, 0, sizeof(info));
    strncpy(info.name, GYRO_NAME, sizeof(info.name));
    ack(sequence, req, &info, 0);
}

#endif
/*
this should use in the AP fot mt6771/mt6797
mTask.debug_trace == 0x01------can dump some register
mTask.debug_trace == 0x02------print the log
mTask.debug_trace == 0x03------include dump register and print the log

example
adb cmd:
k71v1_64_bsp_pre:/sys/bus/platform/drivers/gsensor # echo 0x1 > trace

for mt6885

30|k6885v1_64_pre:/sys/bus/platform/drivers/mtk_nanohub #echo 0,1 > mtk_nanohub_trace
this will call this function
*/
#if !defined(SUPPORT_MT6885)
static void bmi2xySetDebugTrace(int32_t trace)
#else
static void bmi2xySetDebugTrace(uint32_t sequence, SCP_SENSOR_HUB_REQ *req,
                                int (*ack)(uint32_t, SCP_SENSOR_HUB_REQ *, void *, int))
#endif
{
#if !defined(SUPPORT_MT6885)
    int ret = 0;
     uint8_t txData[2] = {0}, rxData[82] = {0};
     osLog(LOG_ERROR, "DRIVER VERSION:0x%x\n",
         BMI2XY_MTK_HUB_DRIVER_VER);
     osLog(LOG_ERROR, "%s ==> trace:%ld\n", __func__, mTask.debug_trace);
#else
    SCP_SENSOR_HUB_SET_CUST_REQ *set_cust_req = (SCP_SENSOR_HUB_SET_CUST_REQ *)req;
    CUST_SET_REQ_P cust_req = &set_cust_req->cust_set_req;
    int ret = 0;
    uint8_t txData[2] = {0}, rxData[82] = {0};
    osLog(LOG_ERROR, "DRIVER VERSION:0x%x\n",BMI2XY_MTK_HUB_DRIVER_VER);
    mTask.debug_trace = cust_req->setTrace.trace;
    osLog(LOG_ERROR, "%s ==> trace:%d\n", __func__, mTask.debug_trace);
    ack(sequence, req, NULL, 0);
#endif
     if ((mTask.debug_trace & 0x01) == 0x01) {

         txData[0] = BMI2_FIFO_LENGTH_0_ADDR | 0x80;
         ret = spiMasterRxTxSync(mTask.spiDev, rxData, txData, 4);
         osLog(LOG_ERROR, "dump reg addr:0x24, data0x%x\n", rxData[2]);
         osLog(LOG_ERROR, "dump reg addr:0x25, data0x%x\n", rxData[3]);
         txData[0] = BMI2_ACC_CONF_ADDR | 0x80;
         ret = spiMasterRxTxSync(mTask.spiDev, rxData, txData, 6);
         osLog(LOG_ERROR, "dump reg addr:0x40, data0x%x\n", rxData[2]);
         osLog(LOG_ERROR, "dump reg addr:0x41, data0x%x\n", rxData[3]);
         osLog(LOG_ERROR, "dump reg addr:0x40, data0x%x\n", rxData[4]);
         osLog(LOG_ERROR, "dump reg addr:0x41, data0x%x\n", rxData[5]);
         txData[0] = BMI2_FIFO_DOWNS_ADDR | 0x80;
         ret = spiMasterRxTxSync(mTask.spiDev, rxData, txData, 7);
         osLog(LOG_ERROR, "dump reg addr:0x45, data0x%x\n", rxData[2]);
         osLog(LOG_ERROR, "dump reg addr:0x46, data0x%x\n", rxData[3]);
         osLog(LOG_ERROR, "dump reg addr:0x47, data0x%x\n", rxData[4]);
         osLog(LOG_ERROR, "dump reg addr:0x48, data0x%x\n", rxData[5]);
         osLog(LOG_ERROR, "dump reg addr:0x49, data0x%x\n", rxData[6]);
         txData[0] = BMI2_INT1_IO_CTRL_ADDR | 0x80;
         ret = spiMasterRxTxSync(mTask.spiDev, rxData, txData, 9);
         osLog(LOG_ERROR, "dump reg addr:0x53, data0x%x\n", rxData[2]);
         osLog(LOG_ERROR, "dump reg addr:0x54, data0x%x\n", rxData[3]);
         osLog(LOG_ERROR, "dump reg addr:0x55, data0x%x\n", rxData[4]);
         osLog(LOG_ERROR, "dump reg addr:0x56, data0x%x\n", rxData[5]);
         osLog(LOG_ERROR, "dump reg addr:0x57, data0x%x\n", rxData[6]);
         osLog(LOG_ERROR, "dump reg addr:0x58, data0x%x\n", rxData[7]);
         osLog(LOG_ERROR, "dump reg addr:0x59, data0x%x\n", rxData[8]);
         txData[0] = BMI2_PWR_CONF_ADDR | 0x80;
         ret = spiMasterRxTxSync(mTask.spiDev, rxData, txData, 4);
         osLog(LOG_ERROR, "dump reg addr:0x7c, data0x%x\n", rxData[2]);
         osLog(LOG_ERROR, "dump reg addr:0x7d, data0x%x\n", rxData[3]);
    }
    if (ret < 0) {
             osLog(LOG_ERROR, "SPI read failed ret: %d\n", ret);
    }
}

static int bmi2xyRegisterCore(void)
{
    struct sensorCoreInfo mInfo;

    memset(&mInfo, 0, sizeof(struct sensorCoreInfo));
    /* Register sensor Core */
    mInfo.sensType = SENS_TYPE_ACCEL;
#if !defined(SUPPORT_MT6885)
    mInfo.gain = GRAVITY_EARTH_1000;
    mInfo.sensitivity = 4096;/* mTask.accReso->sensitivity;*/
    mInfo.cvt = mTask.cvt;
    mInfo.getCalibration = accGetCalibration;
    mInfo.setCalibration = accSetCalibration;
#endif
    mInfo.getData = accGetData;
    mInfo.setDebugTrace = bmi2xySetDebugTrace;
    mInfo.getSensorInfo = accGetSensorInfo;
    sensorCoreRegister(&mInfo);

    memset(&mInfo, 0x00, sizeof(struct sensorCoreInfo));
    mInfo.sensType = SENS_TYPE_GYRO;
#if !defined(SUPPORT_MT6885)
    mInfo.gain = BMI2XY_FS_250_LSB;/* GYROSCOPE_INCREASE_NUM_AP;*/
    mInfo.sensitivity = BMI2XY_FS_2000_LSB;/* mTask.gyroReso->sensitivity;*/
    mInfo.cvt = mTask.cvt;
    mInfo.getCalibration = gyroGetCalibration;
    mInfo.setCalibration = gyroSetCalibration;
#endif
    mInfo.getData = gyroGetData;
    mInfo.getSensorInfo = gyroGetSensorInfo;
    sensorCoreRegister(&mInfo);
    return 0;
}

static int sensorInit(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
        /* perform soft reset and wait for 100ms */
        SPI_WRITE(BMI2_CMD_REG_ADDR, 0xb6, 100000);
        /* dummy reads after soft reset for switch SPI bus, wait 100us */
        SPI_READ(BMI2_CHIP_ID_ADDR, 2, &mTask.dataBuffer, 2000);
        /* right now the bus is SPI bus, get chip id again */
        SPI_READ(BMI2_CHIP_ID_ADDR, 2, &mTask.dataBuffer, 2000);
        /* disable the advance power mode */
        SPI_WRITE(BMI2_PWR_CONF_ADDR, 0x00, 1000);
        spiBatchTxRx(&mTask.mode, spiCallBack, next_state, SUCCESS_EVT);
        return 0;
}
static int sensorInitReg(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    /* disable accel, gyro and mag data in FIFO, enable header, enable fifo time */
    SPI_WRITE(BMI2_FIFO_CONFIG_1_ADDR, 0x10, 450);/*enable the fifo header */
    SPI_WRITE(BMI2_FIFO_CONFIG_0_ADDR, 0x02, 450);/*enable the fifo time */

    /* set the watermark to 13 byte */
    SPI_WRITE(BMI2_FIFO_WTM_0_ADDR, 0xD, 450);
    SPI_WRITE(BMI2_FIFO_WTM_1_ADDR, 0, 450);

    /* FIFO watermark and fifo_full interrupt enabled */
    SPI_WRITE(BMI2_INT_MAP_DATA_ADDR, 0x03, 450);

    /* INT1 enabled, high-level (push-pull) triggered */
    SPI_WRITE(BMI2_INT1_IO_CTRL_ADDR, 0x0a, 1000);
    SPI_WRITE(BMI2_INT2_IO_CTRL_ADDR, 0x0a, 1000);

    /* NO Map all interrupts to INT1 and INT2 */
    SPI_WRITE(BMI2_INT1_MAP_FEAT_ADDR, 0x00, 450);
    SPI_WRITE(BMI2_INT2_MAP_FEAT_ADDR, 0x00, 450);

    /* initial range for accel (+-8g) and gyro (+-2000 degree) */
    SPI_WRITE(BMI2_ACC_CONF_ADDR + 1, 0x02, 450);
    SPI_WRITE(BMI2_GYR_CONF_ADDR + 1, 0x00, 450);
    //#if defined(BMI260_OIS)
    /* clear the spare3 adn asda_pupsel bit */
    SPI_WRITE(BMI2_AUX_IF_TRIM_ADDR, 0x00, 450);
    //#endif
    SPI_WRITE(BMI2_GYR_CONF_ADDR, 0xE9, 450); /* set the gyro conf bit gyr_noise_perf to 1 at begining */

    /* disable the advance power config */
    SPI_WRITE(BMI2_PWR_CONF_ADDR, 0x00, 1000);

    /* disable the config loading */
    SPI_WRITE(BMI2_INIT_CTRL_ADDR, 0x00, 1000);
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int sensorInitOnChange(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
     /* Enable config loading and FIFO mode */
     SPI_WRITE(BMI2_INIT_CTRL_ADDR, 0x01, 150000);
    /* read config file status check if load successful */
     SPI_READ(BMI2_INTERNAL_STATUS_ADDR, 2, &mTask.config_stream_status);
     spiBatchTxRx(&mTask.mode, spiCallBack, next_state, SUCCESS_EVT);
     return 0;
}

static void bmi2xyConfigLoadConfigFile(uint16_t index)
{
    uint8_t asic_msb = (uint8_t)((index / 2) >> 4);
    uint8_t asic_lsb = ((index / 2) & 0x0F);
    /* Write to feature config register */
    SPI_WRITE(BMI2_INIT_ADDR_0, asic_lsb, 500);
    /* Write to feature config register */
    SPI_WRITE(BMI2_INIT_ADDR_1, asic_msb, 500);
    #if defined(BMI220)
    if (index == 8160) {
        internel_write[0] = 0x5e;
        memcpy(&internel_write[1], &bmi220_config_file[index], 32);
        osLog(LOG_ERROR, "internel_write last %p %x %x %x %x %x\n", internel_write,
        internel_write[0], internel_write[1], internel_write[2], internel_write[3],
        internel_write[32]);
        /* Write to feature config registers */
        spiQueueWriteBlock(internel_write, (32 + 1), 1000);
    }
    #endif
    #if defined(BMI260)
    if (index == 6120) {
        internel_write[0] = 0x5e;
        memcpy(&internel_write[1], &bmi260_config_file[index], (mTask.configFileSize - 6120));
        osLog(LOG_ERROR, "internel_write last %p %x %x %x %x %x\n", internel_write,
        internel_write[0], internel_write[1], internel_write[2], internel_write[(mTask.configFileSize - 6120) - 1],
        internel_write[(mTask.configFileSize - 6120)]);
        /* Write to feature config registers */
        spiQueueWriteBlock(internel_write, ((mTask.configFileSize - 6120) + 1), 1000);
    }
    #endif
    else {
        internel_write[0] = 0x5e;
#if defined(BMI220)
        memcpy(&internel_write[1], &bmi220_config_file[index], 1020);
#endif
#if defined(BMI260)
        memcpy(&internel_write[1], &bmi260_config_file[index], 1020);
#endif
        osLog(LOG_ERROR, "internel_write %p %x %x %x %x %x %x\n", internel_write,
        internel_write[0], internel_write[1], internel_write[2], internel_write[3],
        internel_write[1019], internel_write[1020]);
        /* Write to feature config registers */
        spiQueueWriteBlock(internel_write, (1020 + 1), 1000);
    }
}


/* for config file, every time write 1020 bytes  */
static int bmi2xyLoad_configfile0(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
#if defined(BMI220)
    osLog(LOG_ERROR, "begin load config file %d\n", sizeof(bmi220_config_file));
    mTask.configFileSize = sizeof(bmi220_config_file);
#endif
#if defined(BMI260)
    osLog(LOG_ERROR, "begin load config file %d\n", sizeof(bmi260_config_file));
    mTask.configFileSize = sizeof(bmi260_config_file);
#endif
    uint16_t index = 0;
    bmi2xyConfigLoadConfigFile(index);
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;
}

static int bmi2xyLoad_configfile1(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint16_t index = 1020;
    bmi2xyConfigLoadConfigFile(index);
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;
}

static int bmi2xyLoad_configfile2(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint16_t index = 2040;
    bmi2xyConfigLoadConfigFile(index);
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;

}

static int bmi2xyLoad_configfile3(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint16_t index = 3060;
    bmi2xyConfigLoadConfigFile(index);
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;
}

static int bmi2xyLoad_configfile4(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint16_t index = 4080;
    bmi2xyConfigLoadConfigFile(index);
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;
}

static int bmi2xyLoad_configfile5(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint16_t index = 5100;
    bmi2xyConfigLoadConfigFile(index);
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;

}

static int bmi2xyLoad_configfile6(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint16_t index = 6120;
    bmi2xyConfigLoadConfigFile(index);
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;
}

static int bmi2xyLoad_configfile7(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
#if defined(BMI220)
    uint16_t index = 7140;
    bmi2xyConfigLoadConfigFile(index);
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
#else
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
#endif
    return 0;
}
static int bmi2xyLoad_configfile8(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
#if defined(BMI220)
    uint16_t index = 8160;
    bmi2xyConfigLoadConfigFile(index);
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
#else
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
#endif
   return 0;
}

static int sensorInitRegistration(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint8_t config_stream_status = 0;
    uint8_t page = BMI2_PAGE_1;
    config_stream_status = mTask.config_stream_status[2];
    if (config_stream_status != BMI2_CONFIG_LOAD_SUCCESS) {
      osLog(LOG_ERROR, "bmi2xyLoadConfigfile failed\n");
    }
    osLog(LOG_ERROR, "config_stream_status = %x\n", mTask.config_stream_status[2]);
    /* enable the anymotion in feature */

    osLog(LOG_ERROR,"enable anyMotionPower in feature\n");
#if defined(BMI220)
    page = BMI2_PAGE_2;
#endif
#if defined(BMI260)
    page = BMI2_PAGE_1;
#endif
    /* Set the page */
    SPI_WRITE(BMI2_FEAT_PAGE_ADDR, page, 1000);
    /* enable the anymotion config */
#if defined(BMI220)
    spiQueueWriteBlock(bmi220_any_enable_config, 5, 1000);/* enable anymotion*/
#endif
#if defined(BMI260)
    spiQueueWriteBlock(bmi260_any_enable_config, 5, 1000);/* enable anymotion*/
#endif
     /* get the anymotion from the page */
     SPI_READ(BMI2_FEATURES_REG_ADDR, 17, &mTask.config_stream_status);
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;
}

static int sensorInitEintRegistration(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    TDECL();
    osLog(LOG_ERROR, "dump reg addr:30, data0x%x data0x%x data0x%x data0x%x data0x%x data0x%x data0x%x data0x%x\n",
        mTask.config_stream_status[2],mTask.config_stream_status[3],mTask.config_stream_status[4],mTask.config_stream_status[5],
        mTask.config_stream_status[6],mTask.config_stream_status[7],mTask.config_stream_status[8],mTask.config_stream_status[9]);
    mt_eint_dis_hw_debounce(mTask.hw->eint_num);
    mt_eint_registration(mTask.hw->eint_num, LEVEL_SENSITIVE, HIGH_LEVEL_TRIGGER, bmi2xyIsr1, EINT_INT_UNMASK,
        EINT_INT_AUTO_UNMASK_OFF);
    osLog(LOG_ERROR,"bmi2x INIT_DONE DRIVER VERSION:0x%x\n",
         BMI2XY_MTK_HUB_DRIVER_VER);
    bmi2xyRegisterCore();
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}

#ifdef BMI2XY_SELFTEST
#define ABS(x) (x >= 0 ? (x) : (-x))

static int Bmi2xyAccSelfTestBackup(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    osLog(LOG_INFO, "Bmi2xyAccSelfTestBackup\n");

    SPI_READ(BMI2_ACC_CONF_ADDR, 11, &mTask.reg40_4A);
    SPI_READ(BMI2_INT1_IO_CTRL_ADDR, 8, &mTask.reg53_59);
    SPI_READ(BMI2_PWR_CONF_ADDR, 3, &mTask.reg7c_7d);

    /* under sampling here is a function, but need judge type */
    SPI_READ(BMI2_ACC_CONF_ADDR, 2, &mTask.confData, 1000);
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;
}

static int Bmi2xyAccSetConf(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    osLog(LOG_ERROR, "Bmi2xyAccSetConf\n");
    osLog(LOG_ERROR, "reg addr:0x40[%x]0x41[%x]0x42[%x]0x43[%x]0x44[%x]0x45[%x]0x46[%x]\n",
    mTask.reg40_4A[2], mTask.reg40_4A[3], mTask.reg40_4A[4], mTask.reg40_4A[5], mTask.reg40_4A[6], mTask.reg40_4A[7], mTask.reg40_4A[8]);
    osLog(LOG_ERROR, "reg addr:0x47[%x]0x48[%x]0x49[%x]0x4A[%x]\n",
    mTask.reg40_4A[9], mTask.reg40_4A[10], mTask.reg40_4A[11], mTask.reg40_4A[12]);
    osLog(LOG_ERROR, "reg addr:0x56[%x]0x57[%x]0x58[%x]0x7c[%x]0x7d[%x]\n",
    mTask.reg53_59[5], mTask.reg53_59[6], mTask.reg53_59[7], mTask.reg7c_7d[2], mTask.reg7c_7d[3]);
    mTask.reg_val[2] = mTask.reg40_4A[7];//reg45s ---fifo downs
    mTask.reg_val[3] = mTask.reg40_4A[8];//reg46 ---fifo wml0
    mTask.reg_val[4] = mTask.reg40_4A[9];//reg47 ---fifo wml1
    mTask.reg_val[5] = mTask.reg40_4A[10];//reg48 ---fifo cnf0
    mTask.reg_val[6] = mTask.reg40_4A[11];//reg49 ---fifo cnf1
    mTask.reg_val[7] = mTask.reg40_4A[12];//reg4a ---station
    mTask.reg_val[8] = mTask.reg53_59[5];//reg56 ---feature map 1
    mTask.reg_val[9] = mTask.reg53_59[6];//reg57 ---feature map 2
    mTask.reg_val[10] = mTask.reg53_59[7];//reg58 ---data map
    mTask.reg_val[11] = mTask.reg7c_7d[2];//reg7c ---power cfg
    mTask.reg_val[12] = mTask.reg7c_7d[3];//reg7d ---power
    mTask.reg_val[13] = mTask.reg40_4A[2];//reg40 ---acc cfg
    mTask.reg_val[14] = mTask.reg40_4A[4];//reg42 ---gyro cfg
     osLog(LOG_ERROR,"reg %x %x %x %x %x %x %x %x %x %x %x %x %x\n",
        mTask.reg_val[2], mTask.reg_val[3], mTask.reg_val[4], mTask.reg_val[5], mTask.reg_val[6],
        mTask.reg_val[7], mTask.reg_val[8], mTask.reg_val[9], mTask.reg_val[10], mTask.reg_val[11], mTask.reg_val[12], mTask.reg_val[13], mTask.reg_val[14]);
     /*disable the adv power mode */
    SPI_WRITE(BMI2_PWR_CONF_ADDR, 0x00, 3000);
    /* set acc config */
    SPI_WRITE(BMI2_ACC_CONF_ADDR, 0xAC, 3000);
    /* set acc config */
    SPI_WRITE(BMI2_ACC_CONF_ADDR, 0xAC, 3000);
    /* set acc range */
    SPI_WRITE((BMI2_ACC_CONF_ADDR + 1), 0x03, 5000);
    /* set acc power */
    SPI_WRITE(BMI2_PWR_CTRL_ADDR, 0x04, 5000);
    /* read the error reg */
    SPI_READ((BMI2_CHIP_ID_ADDR + 1), 2, &mTask.confData, 100);
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;
}

static int Bmi2xyAccSelfCheckSet(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint8_t data;
    osLog(LOG_ERROR, "Bmi2xy0AccSelfCheckSet\n");
    data = mTask.confData[2];
    osLog(LOG_ERROR, "Bmi160AccSelfCheckSet error reg: %u\n", data);
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int Bmi2xyAccSelfEnable(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    osLog(LOG_ERROR, "Bmi2xyAccSelfEnable\n");


    /* get positive data*/
    SPI_WRITE(BMI2_ACC_SELF_TEST_ADDR, 0x0D, 200000);/* delay 100ms */

    SPI_READ(BMI2_ACC_X_LSB_ADDR, 7, &mTask.bufData, 1000);
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;

}

/* Perform accel self test with positive excitation */
static int Bmi2xyAccGetPositive(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint8_t *buf;
    osLog(LOG_ERROR, "Bmi2xyAccGetPositive\n");

    buf = &mTask.bufData[2];

    mTask.acc_data_positive[0] = buf[0] | buf[1] << 8;
    mTask.acc_data_positive[1] = buf[2] | buf[3] << 8;
    mTask.acc_data_positive[2] = buf[4] | buf[5] << 8;

    osLog(LOG_ERROR, "Bmi2xy positive data: %d %d %d\n",
     mTask.acc_data_positive[0], mTask.acc_data_positive[1], mTask.acc_data_positive[2]);

    /* get negative data*/
    SPI_WRITE(BMI2_ACC_SELF_TEST_ADDR, 0x09, 200000);/* delay 100ms */
    SPI_READ(BMI2_ACC_X_LSB_ADDR, 7, &mTask.bufData, 1000);

    /* read data */
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;
}

static int Bmi2xyAccGetNegative(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint8_t *buf;
    osLog(LOG_ERROR, "Bmi2xyAccGetNegative\n");

    buf = &mTask.bufData[2];
    mTask.acc_data_negtive[0] = buf[0] | buf[1] << 8;
    mTask.acc_data_negtive[1] = buf[2] | buf[3] << 8;
    mTask.acc_data_negtive[2] = buf[4] | buf[5] << 8;

    osLog(LOG_ERROR, "Bmi2xy negative data: %d %d %d\n",
    mTask.acc_data_negtive[0], mTask.acc_data_negtive[1], mTask.acc_data_negtive[2]);

    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}


static int Bmi2xyAccGetAccSelfvalue(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int32_t rslt = 0;
    /*
        x-axis signal   y-axis signal   z-axis signal
    Minimum difference signal   >16 g   <-15 g  >10 g
    */
    if (ABS((int32_t)(mTask.acc_data_negtive[0] - mTask.acc_data_positive[0])) > 32768)
     rslt++;
    if (ABS((int32_t)(mTask.acc_data_negtive[1] - mTask.acc_data_positive[1])) > 30720)
     rslt++;
    if (ABS((int32_t)(mTask.acc_data_negtive[2] - mTask.acc_data_positive[2])) > 20480)
     rslt++;
    
    if (rslt > 2)
     mTask.selfValue = 1;
    else
     mTask.selfValue = 0;
    
    osLog(LOG_ERROR, "Bmi2xyAccGetAccSelfvalue: %d\n", mTask.selfValue);
    /* 0 fail, 1 success, and you could define by yourself */
    accGyroSendTestResult(SENS_TYPE_ACCEL, mTask.selfValue);
    /* spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__); */
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int Bmi2xyAccSelfTestRecover(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    osLog(LOG_ERROR, "RESET_BMI2\n");
    /* do software reset */
    SPI_WRITE(BMI2_CMD_REG_ADDR, 0xb6, 100000);
    /* dummy reads after soft reset for switch SPI bus, wait 100us */
    SPI_READ(BMI2_CHIP_ID_ADDR, 2, &mTask.chip_id, 2000);
    /* right now the bus is SPI bus, get chip id again */
    SPI_READ(BMI2_CHIP_ID_ADDR, 2, &mTask.chip_id, 2000);
    /* right now the bus is SPI bus, get chip id again */
    SPI_READ(BMI2_CHIP_ID_ADDR, 2, &mTask.chip_id, 2000);
    /* disable the advance power mode */
    SPI_WRITE(BMI2_PWR_CONF_ADDR, 0x00, 1000);

    /* disable the config loading */
    SPI_WRITE(BMI2_INIT_CTRL_ADDR, 0x00, 1000);
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;
}

static int Bmi2xyAccSelfTestInitReg(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    osLog(LOG_ERROR, "INIT_BMI2xy chip_id = %x\n", mTask.chip_id[2]);
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int Bmi2xyAccSelfTestInitPowerOn(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint8_t config_stream_status = 0;
    uint8_t page = BMI2_PAGE_1;
    config_stream_status = mTask.config_stream_status[2];
    if (config_stream_status != BMI2_CONFIG_LOAD_SUCCESS) {
      osLog(LOG_ERROR, "bmi2xyLoadConfigfile failed\n");
    }
    osLog(LOG_ERROR, "config_stream_status = %x\n", mTask.config_stream_status[2]);
    /* enable the anymotion in feature */

    osLog(LOG_ERROR,"enable anyMotionPower in feature\n");
#if defined(BMI220)
    page = BMI2_PAGE_2;
#endif
#if defined(BMI260)
    page = BMI2_PAGE_1;
#endif
    /* Set the page */
    SPI_WRITE(BMI2_FEAT_PAGE_ADDR, page, 1000);
    /* enable the anymotion config */
#if defined(BMI220)
    spiQueueWriteBlock(bmi220_any_enable_config, 5, 1000);/* enable anymotion*/
#endif
#if defined(BMI260)
    spiQueueWriteBlock(bmi260_any_enable_config, 5, 1000);/* enable anymotion*/
#endif
/*restore the register*/
    /* disable accel, gyro and mag data in FIFO, enable header, enable fifo time */
    SPI_WRITE(BMI2_FIFO_CONFIG_1_ADDR, 0x10, 450);/*enable the fifo header */
    SPI_WRITE(BMI2_FIFO_CONFIG_0_ADDR, 0x02, 450);/*enable the fifo time */

    /* set the watermark to 13 byte */
    //SPI_WRITE(BMI2_FIFO_WTM_0_ADDR, 0xD, 450);
    //SPI_WRITE(BMI2_FIFO_WTM_1_ADDR, 0, 450);

    /* FIFO watermark and fifo_full interrupt enabled */
    SPI_WRITE(BMI2_INT_MAP_DATA_ADDR, 0x03, 450);

    /* INT1 enabled, high-level (push-pull) triggered */
    SPI_WRITE(BMI2_INT1_IO_CTRL_ADDR, 0x0a, 1000);
    SPI_WRITE(BMI2_INT2_IO_CTRL_ADDR, 0x0a, 1000);

    /* Map all interrupts to INT1 and INT2 */
    //SPI_WRITE(BMI2_INT1_MAP_FEAT_ADDR, mTask.anymap[2], 1000);
    //SPI_WRITE(BMI2_INT2_MAP_FEAT_ADDR, 0xFF, 450);

    /* initial range for accel (+-8g) and gyro (+-2000 degree) */
    SPI_WRITE(BMI2_ACC_CONF_ADDR + 1, 0x02, 450);
    SPI_WRITE(BMI2_GYR_CONF_ADDR + 1, 0x00, 450);
   //#if defined(BMI260_OIS)
    /* clear the spare3 adn asda_pupsel bit */
    SPI_WRITE(BMI2_AUX_IF_TRIM_ADDR, 0x00);
   //#endif
    /*
    mTask.reg_val[2] = mTask.reg40_4A[7]//reg45 ---fifo downs
    mTask.reg_val[3] = mTask.reg40_4A[8]//reg46 ---fifo wml0
    mTask.reg_val[4] = mTask.reg40_4A[9]//reg47 ---fifo wml1
    mTask.reg_val[5] = mTask.reg40_4A[10]//reg48 ---fifo cnf0
    mTask.reg_val[6] = mTask.reg40_4A[11]//reg49 ---fifo cnf1
    //mTask.reg_val[7] = mTask.reg40_4A[12]//reg4a ---station
    mTask.reg_val[8] = mTask.reg53_59[5]//reg56 ---feature map 1
    mTask.reg_val[9] = mTask.reg53_59[6]//reg57 ---feature map 2
    mTask.reg_val[10] = mTask.reg53_59[7]//reg58 ---data map
    mTask.reg_val[11] = mTask.reg7c_7d[2]//reg7c ---power cfg
    mTask.reg_val[12] = mTask.reg7c_7d[3]//reg7d ---power
    mTask.reg_val[13] = mTask.reg40_4A[2];//reg40 ---acc cfg
    mTask.reg_val[14] = mTask.reg40_4A[4];//reg42 ---gyro cfg
    */
    SPI_WRITE(BMI2_ACC_CONF_ADDR, mTask.reg_val[13], 1000); /*acc cfg*/
    SPI_WRITE(BMI2_GYR_CONF_ADDR, mTask.reg_val[14], 1000); /*gyro cfg*/
    SPI_WRITE(BMI2_FIFO_DOWNS_ADDR, mTask.reg_val[2], 1000); /*set fifo down */
    SPI_WRITE(BMI2_FIFO_WTM_0_ADDR, mTask.reg_val[3], 1000); /*set fifo wml0 */
    SPI_WRITE(BMI2_FIFO_WTM_1_ADDR, mTask.reg_val[4], 1000); /*set fifo wml1 */
    SPI_WRITE(BMI2_FIFO_CONFIG_0_ADDR, mTask.reg_val[5], 1000); /*set fifo config0 */
    SPI_WRITE(BMI2_FIFO_CONFIG_1_ADDR, mTask.reg_val[6], 1000); /*set fifo config1 */
    SPI_WRITE(BMI2_INT1_MAP_FEAT_ADDR, mTask.reg_val[8], 1000); /*set feature map1*/
    SPI_WRITE(BMI2_INT2_MAP_FEAT_ADDR, mTask.reg_val[9], 1000); /*set feature map2*/
    SPI_WRITE(BMI2_PWR_CONF_ADDR, mTask.reg_val[11], 5000); /*set power cfg*/
    SPI_WRITE(BMI2_PWR_CTRL_ADDR, mTask.reg_val[12], 5000); /*set power */

/* get the anymotion from the page */
//SPI_READ(BMI2_FEATURES_REG_ADDR, 17, &mTask.config_stream_status);
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;
}
static int Bmi2xyGyroSetConf(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    osLog(LOG_ERROR, "Bmi2xyGyroSetConf\n");
    uint8_t val = 0x06;
    SPI_READ(BMI2_ACC_CONF_ADDR, 11, &mTask.reg40_4A);
    SPI_READ(BMI2_INT1_IO_CTRL_ADDR, 8, &mTask.reg53_59);
    SPI_READ(BMI2_PWR_CONF_ADDR, 3, &mTask.reg7c_7d);


    /* set acc power */
    SPI_WRITE(BMI2_PWR_CTRL_ADDR,val, 50000);
#if BMI26X_CONFIG_ENABLE_GYRO_SELF_TEST_ONESHOT
    /* send the g_trigger command */
    SPI_WRITE(BMI2_CMD_REG_ADDR,0x02, 500000);
    /* read the gyr_st_axes_done register bit 0,*/
    SPI_READ(BMI2_GYR_SELF_TEST_AXES_ADDR, 2, &mTask.gyroStatus,100);

#else
    /* send the gyro self test command */
    SPI_WRITE((BMI2_GYR_SELF_TEST_AXES_ADDR + 1),0x01, 500000);
    /* read the gyr_st_axes_done register bit 0,*/
    SPI_READ((BMI2_GYR_SELF_TEST_AXES_ADDR + 1), 2, &mTask.gyroStatus,100);
#endif
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;
}

static int Bmi2xyGyroEnableSelftest(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint8_t page = 0;
    osLog(LOG_ERROR, "Bmi2xyGyroEnableSelftest\n");
    osLog(LOG_ERROR, "reg addr:0x40[%x]0x41[%x]0x42[%x]0x43[%x]0x44[%x]0x45[%x]0x46[%x]\n",
    mTask.reg40_4A[2], mTask.reg40_4A[3], mTask.reg40_4A[4], mTask.reg40_4A[5], mTask.reg40_4A[6], mTask.reg40_4A[7], mTask.reg40_4A[8]);
    osLog(LOG_ERROR, "reg addr:0x47[%x]0x48[%x]0x49[%x]0x4A[%x]\n",
    mTask.reg40_4A[9], mTask.reg40_4A[10], mTask.reg40_4A[11], mTask.reg40_4A[12]);
    osLog(LOG_ERROR, "reg addr:0x56[%x]0x57[%x]0x58[%x]0x7c[%x]0x7d[%x]\n",
    mTask.reg53_59[5], mTask.reg53_59[6], mTask.reg53_59[7], mTask.reg7c_7d[2], mTask.reg7c_7d[3]);
    mTask.reg_val[2] = mTask.reg40_4A[7];//reg45 ---fifo downs
    mTask.reg_val[3] = mTask.reg40_4A[8];//reg46 ---fifo wml0
    mTask.reg_val[4] = mTask.reg40_4A[9];//reg47 ---fifo wml1
    mTask.reg_val[5] = mTask.reg40_4A[10];//reg48 ---fifo cnf0
    mTask.reg_val[6] = mTask.reg40_4A[11];//reg49 ---fifo cnf1
    mTask.reg_val[7] = mTask.reg40_4A[12];//reg4a ---station
    mTask.reg_val[8] = mTask.reg53_59[5];//reg56 ---feature map 1
    mTask.reg_val[9] = mTask.reg53_59[6];//reg57 ---feature map 2
    mTask.reg_val[10] = mTask.reg53_59[7];//reg58 ---data map
    mTask.reg_val[11] = mTask.reg7c_7d[2];//reg7c ---power cfg
    mTask.reg_val[12] = mTask.reg7c_7d[3];//reg7d ---power
    mTask.reg_val[13] = mTask.reg40_4A[2];//reg40 ---acc cfg
    mTask.reg_val[14] = mTask.reg40_4A[4];//reg42 ---gyro cfg

    osLog(LOG_ERROR,"reg %x %x %x %x %x %x %x %x %x %x %x %x %x\n",
        mTask.reg_val[2], mTask.reg_val[3], mTask.reg_val[4], mTask.reg_val[5], mTask.reg_val[6],
        mTask.reg_val[7], mTask.reg_val[8], mTask.reg_val[9], mTask.reg_val[10], mTask.reg_val[11], mTask.reg_val[12], mTask.reg_val[13], mTask.reg_val[14]);
    /*judge the gry_st_axes_donw bit status*/
    if (mTask.gyroStatus[2] & 0x01) {
        osLog(LOG_ERROR, "Bmi2xyGyroEnableSelftest finished 0x%x\n", mTask.gyroStatus[2]);
    } else {
        osLog(LOG_ERROR, "Bmi2xyGyroEnableSelftest not finished 0x%x\n", mTask.gyroStatus[2]);
   }
    /* GYR_GAIN_STATUS.g_trig_status report a successful self-test or execution errors */
    /* Set the page */
    SPI_WRITE(BMI2_FEAT_PAGE_ADDR, page, 1000);
    /* read the gyro usergain status */
    SPI_READ(BMI2_FEATURES_REG_ADDR, 17, &mTask.gyroUserGainStatus, 1000);
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;
}

static int Bmi2xyGyroValidateValue(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
#if BMI26X_CONFIG_ENABLE_GYRO_SELF_TEST_ONESHOT
    uint8_t sat_x = 0;
    uint8_t sat_y = 0;
    uint8_t sat_z = 0;
    uint8_t g_tigger_status = 0x00;

    g_tigger_status = mTask.gyroUserGainStatus[2] & 0x38;/* bit 3...5 */
    if (g_tigger_status ==0) {
        osLog(LOG_ERROR, "no_err");
    } else if (g_tigger_status == 1) {
        osLog(LOG_ERROR, "precon_err");
    } else if (g_tigger_status == 2) {
        osLog(LOG_ERROR, "dl_err");
    } else if (g_tigger_status == 3) {
        osLog(LOG_ERROR, "abort_err");
    }
    sat_x = mTask.gyroUserGainStatus[2] & 0x01;/*bit 0*/
    sat_y = mTask.gyroUserGainStatus[2] & 0x02;/*bit 1*/
    sat_z = mTask.gyroUserGainStatus[2] & 0x04;/*bit 2*/
    if ((sat_x == 1) && (sat_y == 1) && (sat_z == 1)) {
        mTask.gyroSelfValue = 1;
    } else {
        mTask.gyroSelfValue = 0;
    }
#else
 if (mTask.gyroStatus[2] & 0x02) {
    mTask.gyroSelfValue = 1;
     osLog(LOG_ERROR, "Bmi2xyGyroEnableSelftest finished 0x%x\n", mTask.gyroStatus[2]);
} else {
    mTask.gyroSelfValue = 0;
    osLog(LOG_ERROR, "Bmi2xyGyroEnableSelftest not finished 0x%x\n", mTask.gyroStatus[2]);
}

#endif
    osLog(LOG_ERROR, "bmi2xy gyroSelfValue: %d\n", mTask.gyroSelfValue);
    accGyroSendTestResult(SENS_TYPE_GYRO, mTask.gyroSelfValue);
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);

    return 0;
}

static int Bmi2xyGyroGetSelfvalue(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    osLog(LOG_ERROR, "%s", __FUNCTION__);
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}
#endif

static void initSensorStruct(struct bmi2xysensor *sensor, enum sensorindex idx)
{
    sensor->powered = false;
    sensor->configed = false;
    sensor->rate = 0;
    sensor->hwRate = 0;
    sensor->offset[0] = 0;
    sensor->offset[1] = 0;
    sensor->offset[2] = 0;
    sensor->latency = 0;
    sensor->flush = 0;
    sensor->startCali = false;
    sensor->staticCali[AXIS_X] = 0;
    sensor->staticCali[AXIS_Y] = 0;
    sensor->staticCali[AXIS_Z] = 0;
    sensor->accuracy = 0;
    sensor->samplesToDiscard = 0;
}

static struct sensorFsm bmi2xyFsm[] = {
    /* Int Status Check */
    sensorFsmCmd(CHIP_HW_INT_STATUS_CHECK, CHIP_HW_INT_HANDLING, hwIntStatusCheck),
    sensorFsmCmd(CHIP_HW_INT_HANDLING, CHIP_HW_INT_HANDLING_DONE, hwIntHandling),

    sensorFsmCmd(CHIP_SAMPLING, STATE_CONVERT, bmi2xySample),
    sensorFsmCmd(STATE_CONVERT, STATE_FIFO, bmi2xyReadFifo),
    sensorFsmCmd(STATE_FIFO, CHIP_SAMPLING_DONE, bmi2xyConvert),
    /* acc enable state */
    sensorFsmCmd(CHIP_ACC_ENABLE, CHIP_ACC_ENABLE_DONE, accPowerOn),

    /* acc disable state */
    sensorFsmCmd(CHIP_ACC_DISABLE, STATE_UNMASK_EINT, accPowerOff),
    /* acc disable unmask eint state */
    sensorFsmCmd(STATE_UNMASK_EINT, CHIP_ACC_DISABLE_DONE, accGyroUnmaskEint),
    /* acc rate state */
    sensorFsmCmd(CHIP_ACC_RATECHG, STATE_ACC_CHECK_REGISTER, accSetRate),
    sensorFsmCmd(STATE_ACC_CHECK_REGISTER, CHIP_ACC_RATECHG_DONE, bmi2xyAccRegisterCheck),
    /* acc cfg state */
   sensorFsmCmd(CHIP_ACC_CALI, CHIP_ACC_CALI_DONE, bmi2xyAccCali),
   #if !defined(BMI260_OIS)
    sensorFsmCmd(CHIP_ACC_CFG, CHIP_ACC_CFG_DONE, bmi2xyAccCfgCali),
    #else
    sensorFsmCmd(CHIP_ACC_CFG, STATE_ACC_CFG_CHECK_REGISTER, bmi2xyAccCfgCali),
    sensorFsmCmd(STATE_ACC_CFG_CHECK_REGISTER, CHIP_ACC_CFG_DONE, bmi2xyAccCfgCaliCheck),
    #endif
    /* gyro enable state */
    sensorFsmCmd(CHIP_GYRO_ENABLE, CHIP_GYRO_ENABLE_DONE, gyrPowerOn),
    /* gyro disable state */
    sensorFsmCmd(CHIP_GYRO_DISABLE, STATE_UNMASK_EINT, gyrPowerOff),
    /* gyro disable unmask eint state */
    sensorFsmCmd(STATE_UNMASK_EINT, CHIP_GYRO_DISABLE_DONE, accGyroUnmaskEint),
    /* gyro rate state */
    sensorFsmCmd(CHIP_GYRO_RATECHG, STATE_GYRO_CHECK_REGISTER, gyrSetRate),
    sensorFsmCmd(STATE_GYRO_CHECK_REGISTER, CHIP_GYRO_RATECHG_DONE, bmi2xyGyroRegisterCheck),
    /* gyro cfg state */
    sensorFsmCmd(CHIP_GYRO_CALI, CHIP_GYRO_CALI_DONE, bmi2xyGyroCali),
     sensorFsmCmd(CHIP_GYRO_CFG, CHIP_GYRO_CFG_DONE, bmi2xyGyroCfgCali),
    /* init state */
    sensorFsmCmd(CHIP_RESET, STATE_RESET_BMI2XY, sensorInit),
    sensorFsmCmd(STATE_RESET_BMI2XY, STATE_INIT_BMI2XY, sensorInitReg),
    sensorFsmCmd(STATE_INIT_BMI2XY, STATE_SENSOR_LOAD_CONFIG_FILE0, bmi2xyLoad_configfile0),
    sensorFsmCmd(STATE_SENSOR_LOAD_CONFIG_FILE0, STATE_SENSOR_LOAD_CONFIG_FILE1, bmi2xyLoad_configfile1),
    sensorFsmCmd(STATE_SENSOR_LOAD_CONFIG_FILE1, STATE_SENSOR_LOAD_CONFIG_FILE2, bmi2xyLoad_configfile2),
    sensorFsmCmd(STATE_SENSOR_LOAD_CONFIG_FILE2, STATE_SENSOR_LOAD_CONFIG_FILE3, bmi2xyLoad_configfile3),
    sensorFsmCmd(STATE_SENSOR_LOAD_CONFIG_FILE3, STATE_SENSOR_LOAD_CONFIG_FILE4, bmi2xyLoad_configfile4),
    sensorFsmCmd(STATE_SENSOR_LOAD_CONFIG_FILE4, STATE_SENSOR_LOAD_CONFIG_FILE5, bmi2xyLoad_configfile5),
    sensorFsmCmd(STATE_SENSOR_LOAD_CONFIG_FILE5, STATE_SENSOR_LOAD_CONFIG_FILE6, bmi2xyLoad_configfile6),
    sensorFsmCmd(STATE_SENSOR_LOAD_CONFIG_FILE6, STATE_SENSOR_LOAD_CONFIG_FILE7, bmi2xyLoad_configfile7),
    sensorFsmCmd(STATE_SENSOR_LOAD_CONFIG_FILE7, STATE_SENSOR_LOAD_CONFIG_FILE8, bmi2xyLoad_configfile8),
    sensorFsmCmd(STATE_SENSOR_LOAD_CONFIG_FILE8, STATE_SENSOR_LOAD_CONFIG_FILE_CHECK, sensorInitOnChange),
   sensorFsmCmd( STATE_SENSOR_LOAD_CONFIG_FILE_CHECK, STATE_SENSOR_REGISTRATION, sensorInitRegistration),
    sensorFsmCmd(STATE_SENSOR_REGISTRATION, CHIP_INIT_DONE, sensorInitEintRegistration),
    /* For Anymotion */
    sensorFsmCmd(CHIP_ANYMO_ENABLE, CHIP_ANYMO_ENABLE_DONE, anyMotionPowerOn),
    sensorFsmCmd(CHIP_ANYMO_DISABLE, CHIP_ANYMO_DISABLE_DONE, anyMotionPowerOff),

#ifdef CFG_HW_STEP_COUNTER_SUPPORT
    /* for stepCounter */
    sensorFsmCmd(CHIP_STEP_COUNT_ENABLE, CHIP_STEP_COUNT_DONE, stepCountPowerOn),
    sensorFsmCmd(CHIP_STEP_COUNT_DISABLE, CHIP_STEP_COUNT_DISABLE_DONE, stepCountPowerOff),
    sensorFsmCmd(CHIP_STEP_COUNT_SAMPLE, CHIP_STEP_COUNT_SEND, stepCntGetData),
    sensorFsmCmd(CHIP_STEP_COUNT_SEND, CHIP_STEP_COUNT_SAMPLE_DONE, StepCntSendData),
#endif
#ifdef BMI2XY_SELFTEST
/* acc self test */
    sensorFsmCmd(CHIP_ACC_SELFTEST, STATE_ACC_SELF_SET_ACC_CONF, Bmi2xyAccSelfTestBackup),
    sensorFsmCmd(STATE_ACC_SELF_SET_ACC_CONF, STATE_ACC_SELF_SET_ACC_CHECK, Bmi2xyAccSetConf),
    sensorFsmCmd(STATE_ACC_SELF_SET_ACC_CHECK, STATE_ACC_SELF_SET_PWOER_MODE, Bmi2xyAccSelfCheckSet),
    sensorFsmCmd(STATE_ACC_SELF_SET_PWOER_MODE, STATE_ACC_SELF_GET_ACC_POSITIVE, Bmi2xyAccSelfEnable),
    sensorFsmCmd(STATE_ACC_SELF_GET_ACC_POSITIVE, STATE_ACC_SELF_GET_ACC_NEGATIVE, Bmi2xyAccGetPositive),
    sensorFsmCmd(STATE_ACC_SELF_GET_ACC_NEGATIVE, STATE_ACC_SELF_GET_ACC_SELFVALUE, Bmi2xyAccGetNegative),
    sensorFsmCmd(STATE_ACC_SELF_GET_ACC_SELFVALUE, STATE_ACC_SELF_TEST_RECOVER, Bmi2xyAccGetAccSelfvalue),
    sensorFsmCmd(STATE_ACC_SELF_TEST_RECOVER, STATE_ACC_SELF_TEST_INIT_REG, Bmi2xyAccSelfTestRecover),
    sensorFsmCmd(STATE_ACC_SELF_TEST_INIT_REG, STATE_ACC_SELF_TEST_POWER_ON, Bmi2xyAccSelfTestInitReg),
    sensorFsmCmd(STATE_ACC_SELF_TEST_POWER_ON, STATE_SENSOR_LOAD_CONFIG_FILE0, bmi2xyLoad_configfile0),
    sensorFsmCmd(STATE_SENSOR_LOAD_CONFIG_FILE0, STATE_SENSOR_LOAD_CONFIG_FILE1, bmi2xyLoad_configfile1),
    sensorFsmCmd(STATE_SENSOR_LOAD_CONFIG_FILE1, STATE_SENSOR_LOAD_CONFIG_FILE2, bmi2xyLoad_configfile2),
    sensorFsmCmd(STATE_SENSOR_LOAD_CONFIG_FILE2, STATE_SENSOR_LOAD_CONFIG_FILE3, bmi2xyLoad_configfile3),
    sensorFsmCmd(STATE_SENSOR_LOAD_CONFIG_FILE3, STATE_SENSOR_LOAD_CONFIG_FILE4, bmi2xyLoad_configfile4),
    sensorFsmCmd(STATE_SENSOR_LOAD_CONFIG_FILE4, STATE_SENSOR_LOAD_CONFIG_FILE5, bmi2xyLoad_configfile5),
    sensorFsmCmd(STATE_SENSOR_LOAD_CONFIG_FILE5, STATE_SENSOR_LOAD_CONFIG_FILE6, bmi2xyLoad_configfile6),
    sensorFsmCmd(STATE_SENSOR_LOAD_CONFIG_FILE6, STATE_SENSOR_LOAD_CONFIG_FILE7, bmi2xyLoad_configfile7),
    sensorFsmCmd(STATE_SENSOR_LOAD_CONFIG_FILE7, STATE_SENSOR_LOAD_CONFIG_FILE8, bmi2xyLoad_configfile8),
    sensorFsmCmd(STATE_SENSOR_LOAD_CONFIG_FILE8, STATE_SENSOR_LOAD_CONFIG_FILE_CHECK, sensorInitOnChange),
    sensorFsmCmd( STATE_SENSOR_LOAD_CONFIG_FILE_CHECK, CHIP_ACC_SELFTEST_DONE, Bmi2xyAccSelfTestInitPowerOn),
    /* gyro self test */
    sensorFsmCmd(CHIP_GYRO_SELFTEST, STATE_GYRO_SELF_ENABLE_SELF, Bmi2xyGyroSetConf),
    sensorFsmCmd(STATE_GYRO_SELF_ENABLE_SELF, STATE_GYRO_SELF_VALIDATE_VALUE, Bmi2xyGyroEnableSelftest),
    sensorFsmCmd(STATE_GYRO_SELF_VALIDATE_VALUE, STATE_GYRO_SELF_GET_GYRO_SELFVALUE, Bmi2xyGyroValidateValue),
    sensorFsmCmd(STATE_GYRO_SELF_GET_GYRO_SELFVALUE, STATE_ACC_SELF_TEST_INIT_REG, Bmi2xyGyroGetSelfvalue),
    sensorFsmCmd(STATE_ACC_SELF_TEST_INIT_REG, STATE_ACC_SELF_TEST_POWER_ON, Bmi2xyAccSelfTestRecover),
    sensorFsmCmd(STATE_ACC_SELF_TEST_POWER_ON, STATE_SENSOR_LOAD_CONFIG_FILE0, bmi2xyLoad_configfile0),
    sensorFsmCmd(STATE_SENSOR_LOAD_CONFIG_FILE0, STATE_SENSOR_LOAD_CONFIG_FILE1, bmi2xyLoad_configfile1),
    sensorFsmCmd(STATE_SENSOR_LOAD_CONFIG_FILE1, STATE_SENSOR_LOAD_CONFIG_FILE2, bmi2xyLoad_configfile2),
    sensorFsmCmd(STATE_SENSOR_LOAD_CONFIG_FILE2, STATE_SENSOR_LOAD_CONFIG_FILE3, bmi2xyLoad_configfile3),
    sensorFsmCmd(STATE_SENSOR_LOAD_CONFIG_FILE3, STATE_SENSOR_LOAD_CONFIG_FILE4, bmi2xyLoad_configfile4),
    sensorFsmCmd(STATE_SENSOR_LOAD_CONFIG_FILE4, STATE_SENSOR_LOAD_CONFIG_FILE5, bmi2xyLoad_configfile5),
    sensorFsmCmd(STATE_SENSOR_LOAD_CONFIG_FILE5, STATE_SENSOR_LOAD_CONFIG_FILE6, bmi2xyLoad_configfile6),
    sensorFsmCmd(STATE_SENSOR_LOAD_CONFIG_FILE6, STATE_SENSOR_LOAD_CONFIG_FILE7, bmi2xyLoad_configfile7),
    sensorFsmCmd(STATE_SENSOR_LOAD_CONFIG_FILE7, STATE_SENSOR_LOAD_CONFIG_FILE8, bmi2xyLoad_configfile8),
    sensorFsmCmd(STATE_SENSOR_LOAD_CONFIG_FILE8, STATE_SENSOR_LOAD_CONFIG_FILE_CHECK, sensorInitOnChange),
    sensorFsmCmd( STATE_SENSOR_LOAD_CONFIG_FILE_CHECK, CHIP_GYRO_SELFTEST_DONE, Bmi2xyAccSelfTestInitPowerOn),
#endif

};

static int bmi2xyInit(void)
{
    int ret = 0;
    uint8_t txData[2] = {0}, rxData[3] = {0};
    TDECL();

    enum sensorindex i;
    osLog(LOG_ERROR, "bmi2xyInit\n");

    insertMagicNum(&mTask.accGyroPacket);
    mTask.hw = get_cust_accGyro("bmi2xy");
    if (NULL == mTask.hw) {
        osLog(LOG_ERROR, "get_cust_acc_hw fail\n");
        ret = -1;
        goto err_out;
    }
    osLog(LOG_ERROR, "acc spi_num: %d\n", mTask.hw->i2c_num);

    ret = sensorDriverGetConvert(mTask.hw->direction, &mTask.cvt);
    if (0 != (ret)) {
        osLog(LOG_ERROR, "invalid direction: %d\n", mTask.hw->direction);
    }
    osLog(LOG_ERROR, "acc map[0]:%d, map[1]:%d, map[2]:%d, sign[0]:%d, sign[1]:%d, sign[2]:%d\n\r",
        mTask.cvt.map[AXIS_X], mTask.cvt.map[AXIS_Y], mTask.cvt.map[AXIS_Z],
        mTask.cvt.sign[AXIS_X], mTask.cvt.sign[AXIS_Y], mTask.cvt.sign[AXIS_Z]);

    mTask.sensors[ACC].sensitivity = (float)65536 / (8 * 2);  /*  +/-8g */
    mTask.sensors[GYR].sensitivity = (float)65536 / (2000 * 2);//LSB/dps sensitivity
    mTask.frame_sensortime_valid = false;
    mTask.tempCelsius = kTempInvalid;
    /* 8MHZ */
    mTask.mode.speed = 8000000;
    mTask.mode.bitsPerWord = 8;
    mTask.mode.cpol = SPI_CPOL_IDLE_LO;
    mTask.mode.cpha = SPI_CPHA_LEADING_EDGE;
    mTask.mode.nssChange = true;
    mTask.mode.format = SPI_FORMAT_MSB_FIRST;

    mTask.watermark = 0;

    spiMasterRequest(mTask.hw->i2c_num, &mTask.spiDev);

    mTask.latch_time_id = alloc_latch_time();
    enable_latch_time(mTask.latch_time_id, mTask.hw->eint_num);

    for (i = ACC; i < NUM_OF_SENSOR; i++) {
        initSensorStruct(&mTask.sensors[i], i);
    }

    mTask.mWbufCnt = 0;
    mTask.mRegCnt = 0;
    // T(spiInUse) = false;


    // initialize the last bmi160 time to be ULONG_MAX, so that we know it's
    // not valid yet.
    mTask.last_sensortime = 0;
    mTask.frame_sensortime = ULONG_LONG_MAX;
    /* init the debug_trace, default 0 */
    mTask.debug_trace = 3;//0x00;

    // it's ok to leave interrupt open all the time.

    // dummy reads after boot, wait 100us
    txData[0] = BMI2_CHIP_ID_ADDR | 0x80;
    ret = spiMasterRxTxSync(mTask.spiDev, rxData, txData, 3);
    udelay(100);
    // read the device ID for bmi160
    txData[0] = BMI2_CHIP_ID_ADDR | 0x80;

    for (uint8_t retry = 0; retry < 3;) {
        ret = spiMasterRxTxSync(mTask.spiDev, rxData, txData, 3);
        INFO_PRINT("bmi2xy auto detect success: %02x\n", rxData[2]);
        if (ret >= 0 && (rxData[2] == BMI2XY_ID))
            break;
        ++retry;
        if (retry >= 3) {
            ret = -1;
            #if !defined(SUPPORT_MT6885)
            sendSensorErrToAp(ERR_SENSOR_ACC_GYR, ERR_CASE_ACC_GYR_INIT, ACC_NAME);
            sendSensorErrToAp(ERR_SENSOR_ACC_GYR, ERR_CASE_ACC_GYR_INIT, GYRO_NAME);
            #endif
            spiMasterRelease(mTask.spiDev);
            disable_latch_time(mTask.latch_time_id);
            free_latch_time(mTask.latch_time_id);
            goto err_out;
        }
    }

    INFO_PRINT("bmi260 auto detect success: %02x\n", rxData[1]);
    accSensorRegister();
    gyroSensorRegister();
    anyMotionSensorRegister();
#ifdef CFG_HW_STEP_COUNTER_SUPPORT
    stepCntSensorRegister();
    #endif
    registerAccGyroInterruptMode(ACC_GYRO_FIFO_INTERRUPTIBLE);
    registerAccGyroDriverFsm(bmi2xyFsm, ARRAY_SIZE(bmi2xyFsm));

#ifdef CFG_MTK_CALIBRATION_V1_0
    const struct AccFactoryCalParameters acc_factory_cal_parameters = {
        .param = {
            .var_threshold = 8e-3f,
            .bias_mean_limit = 0.5f,
            .debounce_time_duration_nanos = SEC_TO_NANOS(0.5f),
            .window_time_duration_nanos = SEC_TO_NANOS(1),
            .standard_x = 0.0f,
            .standard_y = 0.0f,
            .standard_z = 9.807f,
            .factory_calibration_enable = true,
        },
    };
    accFactoryCalibrateParam(&mTask.accFactoryCal, &acc_factory_cal_parameters);

    const struct GyroFactoryCalParameters gyro_factory_cal_parameters = {
        .param = {
            .var_threshold = 5e-5f,
            .bias_mean_limit = 0.1f,
            .debounce_time_duration_nanos = SEC_TO_NANOS(0.5f),
            .window_time_duration_nanos = SEC_TO_NANOS(1),
            .standard_x = 0.0f,
            .standard_y = 0.0f,
            .standard_z = 0.0f,
            .factory_calibration_enable = true,
        },
    };
    gyroFactoryCalibrateParam(&mTask.gyroFactoryCal, &gyro_factory_cal_parameters);

    const struct GyroCalParameters gyro_cal_parameters = {
        .min_still_duration_nanos        = SEC_TO_NANOS(5),
        .max_still_duration_nanos        = SEC_TO_NANOS(5.9f),
        .calibration_time_nanos          = 0,
        .window_time_duration_nanos      = SEC_TO_NANOS(1.5f),
        .bias_x                          = 0,
        .bias_y                          = 0,
        .bias_z                          = 0,
        .stillness_threshold             = 0.95f,
        .stillness_mean_delta_limit      = MDEG_TO_RAD * 40.0f,
        .gyro_var_threshold              = 7.5e-5f,
        .gyro_confidence_delta           = 1.5e-5f,
        .accel_var_threshold             = 4.5e-3f,
        .accel_confidence_delta          = 9e-4f,
        .mag_var_threshold               = 1.4f,
        .mag_confidence_delta            = 0.25f,
        .temperature_delta_limit_celsius = 1.5f,
        .gyro_calibration_enable         = true
    };
    gyroCalInit(GetGyroCalBase(), &gyro_cal_parameters);

    const struct OverTempCalParameters gyro_otc_parameters = {
        .min_temp_update_period_nanos = MSEC_TO_NANOS(500),
        .age_limit_nanos = DAYS_TO_NANOS(2),
        .delta_temp_per_bin = 0.75f,
        .jump_tolerance = 40.0f * MDEG_TO_RAD,
        .outlier_limit = 50.0f * MDEG_TO_RAD,
        .temp_sensitivity_limit = 80.0f * MDEG_TO_RAD,
        .sensor_intercept_limit = 3.0e3f * MDEG_TO_RAD,
        .significant_offset_change = 0.1f * MDEG_TO_RAD,
        .min_num_model_pts = 5,
        .over_temp_enable = true
    };
    overTempCalInit(GetOverTempCalBase(), &gyro_otc_parameters);
#endif
err_out:
    return ret;
}


#ifndef CFG_OVERLAY_INIT_SUPPORT
#if !defined(SUPPORT_MT6885)
MODULE_DECLARE(bmi2xy, SENS_TYPE_ACCEL, bmi2xyInit);
#else
#include "module_init.h"
MODULE_DECLARE(bmi2xy, "TAG_ACCGYRO", bmi2xyInit);
#endif
#else
#include "mtk_overlay_init.h"
OVERLAY_DECLARE(bmi2xy, OVERLAY_ID_ACCGYRO, bmi2xyInit);
#endif
/** @}*/
