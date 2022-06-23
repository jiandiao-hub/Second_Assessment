/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      https://urldefense.proofpoint.com/v2/url?u=http-3A__www.apache.org_licenses_LICENSE-2D2.0&d=DwIGAg&c=WoJWtq5JV8YrKnzRxvD8NxmTP_1wxfE0prPmo0NeZwg&r=n5zyvxv_7Kg5zsEDPjJUj2q93eJSr47kyQFMQmhecpo&m=Viz22NbURQK7ob9_nfsJ1owW6CYQAJIal6dttIn96Jw&s=tNaUmQYpCOoNUolLiVjiitAAukSJDJthPQl-lc8o6cY&e=
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*
version list:

This driver is for icm206xx series, need a little bit change for 20690 and 20608
/version/   release record / owner name / date  /

v1.0.1     pass cts test basically, need further test under vendor ,  update by jonny  /2018/10/18 ,          
v1.0.2     modify fifo config logic and update driver because of MTK sensorhub update ,  update by jonny  /2018/11/1 , 
v1.0.3     add icm20627 and icm20690 support and match the latest MTK sensor firmware version of P80   update by jonny   /2018/11/27,
v1.0.4     add icm20602 support and change the gyro FS, setting back to 2000DPS for icm20627 &icm20690 update by jonny   /2018/12/05,
v1.0.5     add selftest function and FSM, add log switch Macro        update by jonny   /2018/12/21,
v1.0.6     change intteruput trigger mode to level trigger,add addtion Time Stamp operation & data overflow logic when handle fifo data    jonny  /2019/2/15
v1.0.7     fix some cts error, change spi mode from mode 3 to mode 0 in driver   update by jonny  /2019/3/21 
v1.0.8     fix acc no data issue.Improve anymotionPoweron/off logic realted to acc. update by aaron /2019/05/28
*/

#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <stdbool.h>
#include <stdint.h>
#include <seos.h>
#include <util.h>
#include <atomic.h>
#include <cpu/inc/cpuMath.h>
#include <gpio.h>
#include <heap.h>
#include <slab.h>
#include <timer.h>
#include <variant/inc/variant.h>
#include <limits.h>
#include <performance.h>
#include <sensors.h>
#include <plat/inc/rtc.h>
#include <plat/inc/spichre.h>
#include <spichre-plat.h>
#include <hostIntf.h>
#include <nanohub_math.h>
#include <nanohubPacket.h>
#include <plat/inc/rtc.h>
#include <spi.h>
#include <contexthub_core.h>
#include <accGyro.h>
#include <contexthub_core.h>
#include <cust_accGyro.h>
#include <mt_gpt.h>
#ifndef CFG_MTK_CALIBRATION_V1_0
#include <API_sensor_calibration.h>
#else
#include "calibration/factory_calibration/factory_cal.h"
#endif
#include <algos/time_sync.h>
#include "hwsen.h"
#include "eint.h"

#define MTK_VERSION_DIFF 1

#if MTK_VERSION_DIFF 
#define ACC_NAME     "icm206xx_acc"
#define GYRO_NAME     "icm206xx_gyro"
#endif 

#define SUPPORT_SELFTEST    1
#define SUPPORT_ANYMO       1
#define ALL_LOGINFO_OFF     0
#define DEBUG_PHASE         0
#define DEBUG_STATUS        0
#define DEBUG_FIFO_DATA     0
#define DEBUG_ACC_DATA      0
#define DEBUG_GYRO_DATA     0
#define DEBUG_TEMP_DATA     0

#define DEBUG_WM_INTERUPT   0

#if ALL_LOGINFO_OFF
#define INVLOG(...) 
#else
#define INVLOG osLog
#endif

#define SAMPLE_TO_DISCARD 0 //3
#define ODR2SMPLRT_DIV(odr)         (1000/(odr) - 1)

#define SELFTEST_SAMPLES_NUMBER 10
#define DEF_SELFTEST_PRECISION      1000
#define DEF_ACC_ST_SHIFT_DELTA_MIN  50 //percentage
#define DEF_ACC_ST_SHIFT_DELTA_MAX  150 //percentage
#define DEF_GYRO_ST_SHIFT_DELTA     50 //percentage
//Gyro Offset Max Value (dps)
#define DEF_GYRO_OFFSET_MAX         20
//Gyro Self Test Absolute Limits ST_AL (dps)
#define DEF_GYRO_ST_AL              60
//Accel Self Test Absolute Limits ST_AL (mg)
#define DEF_ACCEL_ST_AL_MIN         225 //TBD
#define DEF_ACCEL_ST_AL_MAX         675 //TBD

#ifndef abs
#define abs(a)      ((a) > 0 ? (a) : -(a)) /*!< Absolute value */
#endif

/* ICM2060X register  */
#define AXIS_X              0
#define AXIS_Y              1
#define AXIS_Z              2
#define AXES_NUM            3

#define ICM20608G_WHOAMI            0xAF
#define ICM20608D_WHOAMI            0xAE
#define ICM20609_WHOAMI             0xA6
#define ICM20600_WHOAMI             0x11
#define ICM20602_WHOAMI             0x12
#define ICM20607_WHOAMI             0x05
#define ICM20690_WHOAMI             0x20
#define ICM20627_WHOAMI             0x22
#define MPU6887_WHOAMI              0x2e

//#define ICM2060X_REG_LP_CONFIG          0x1e
#define ICM2060X_PWR_MGMT_1             0x6b
#define ICM2060X_PWR_MGMT_2             0x6c
#define ICM2060X_REG_FIFO_R_W           0x74
#define ICM2060X_REG_FIFO_COUNTH        0x72
#define ICM2060X_REG_FIFO_COUNTL        0x73
#define ICM2060X_REG_FIFO_EN_2          0x23
#define ICM2060X_REG_FIFO_RST           0x6a
#define ICM2060X_REG_USER_CTRL          0x6a
#define ICM2060X_REG_INT_ENABLE         0x38
#define ICM2060X_REG_FIFO_WM_TH1        0x60
#define ICM2060X_REG_FIFO_WM_TH2        0x61
#define ICM2060X_REG_FIFO_WM_STATUS     0x39
#define ICM2060X_REG_INT_STATUS         0x3a
#define ICM2060X_REG_I2C_IF             0x70
#define ICM2060X_REG_INT_PIN_CFG        0x37

#define ICM2060X_REG_ACCEL_INTEL_CTRL   0x69
#define ICM2060X_REG_WOM_THR_X          0x20
#define ICM2060X_REG_WOM_THR_Y          0x21
#define ICM2060X_REG_WOM_THR_Z          0x22
#define ICM2060X_REG_WOM_THR            0x1f

#define ICM2060X_REG_SELF_TEST_X_GYRO       mTask.gyro_st_code_reg[0]
#define ICM2060X_REG_SELF_TEST_Y_GYRO       mTask.gyro_st_code_reg[1]
#define ICM2060X_REG_SELF_TEST_Z_GYRO       mTask.gyro_st_code_reg[2]
#define ICM2060X_REG_SELF_TEST_X_ACC            0xD
#define ICM2060X_REG_SELF_TEST_Y_ACC            0xE
#define ICM2060X_REG_SELF_TEST_Z_ACC            0xF
#define ICM2060X_REG_ACC_DATA_XOUTH             0x3B
#define ICM2060X_REG_GYRO_DATA_XOUTH            0x43 // 67 /* big endian */

#define BIT_WOM_EN                      0xE0

//REG_PWR_MGMT_1
#define BIT_DEVICE_RESET            0x80
#define BIT_SLEEP                   0x40
#define BIT_CYCLE                   0x20
#define BIT_CLKSEL                  0x07

//#define BIT_GYRO_LP_EN                  0x80
//#define BIT_ACC_LP_EN                   0x20
//#define BIT_G_AVGCFG                    0x00
//#define BIT_ACC_I2C_MST                 0x40

//#define BIT_LP_EN                       0x20
#define BIT_CLK_PLL                     0x01
//#define BIT_TEMP_DIS                    (1<<3)

#define BIT_PWR_ACCEL_STBY              0x38
#define BIT_PWR_GYRO_STBY               0x07

//#define ICM2060X_TIMEBASE_PLL           0x28

/* gyro */
#define ICM2060X_REG_SAMRT_DIV          0x19
#define ICM2060X_REG_GYRO_CONFIG        0x1a
#define ICM2060X_REG_GYRO_CONFIG_1      0x1b


#define SHIFT_GYRO_FS_SEL_4_2                  2
#define SHIFT_GYRO_FS_SEL_4_3                  3
#define GYRO_FS_SEL_250                     (0x00 )
#define GYRO_FS_SEL_500                     (0x01 )
#define GYRO_FS_SEL_1000                    (0x02 )
#define GYRO_FS_SEL_2000                    (0x03 )

#define ICM2060X_FS_250_LSB             131.0f
#define ICM2060X_FS_500_LSB             65.5f
#define ICM2060X_FS_1000_LSB            32.8f
#define ICM2060X_FS_2000_LSB            16.4f

#define GYRO_AVGCFG_1X                     0
#define GYRO_AVGCFG_2X                     1
#define GYRO_AVGCFG_4X                     2
#define GYRO_AVGCFG_8X                     3
#define GYRO_AVGCFG_16X                    4
#define GYRO_AVGCFG_32X                    5
#define GYRO_AVGCFG_64X                    6
#define GYRO_AVGCFG_128X                   7
#define GYRO_DLPFCFG                    (0x04)      //dlpf_cfg for gyro @4
#define GYRO_FCHOICE                    (0x00)
/* gyro */

/* acc */
//#define ICM2060X_REG_SAMRT_DIV1         0x10
//#define ICM2060X_REG_SAMRT_DIV2         0x11
#define ICM2060X_REG_ACC_CONFIG           0x1c
#define ICM2060X_REG_ACC_CONFIG_2         0x1d


#define ICM2060X_RANGE_2G               (0x00 << 3)
#define ICM2060X_RANGE_4G               (0x01 << 3)
#define ICM2060X_RANGE_8G               (0x02 << 3)
#define ICM2060X_RANGE_16G              (0x03 << 3)

#define ACCEL_AVGCFG_1_4X               (0x00 << 4)
#define ACCEL_AVGCFG_8X                 (0x01 << 4)
#define ACCEL_AVGCFG_16X                (0x02 << 4)
#define ACCEL_AVGCFG_32X                (0x03 << 4)
#define ACCEL_FCHOICE                      0 << 3
#define ACCEL_DLPFCFG                      4

/* acc */
#define ACCEL_AVG_SAMPLE                (0x3)

#define MAX_FIFO_SIZE_1024BYTES         0xc0

#define ICM2060X_REG_DATAX0             0x2d
#define ICM2060X_REG_DATAY0             0x2f
#define ICM2060X_REG_DATAZ0             0x31


#define ICM2060X_SLEEP                  0x40    //enable low power sleep mode
#define ICM2060X_DEV_RESET              0x80
 
#define FIFO_MODE                       0x40    //fifo mode for fifo 
#define BIT_FIFO_RECORD_MODE            0x80    //record mode should be used for 20690 and 20627
#define FIFO_EN                         0x40
#define FIFO_DIS                        0x00
#define FIFO_RST                        0x04
#define FIFO_BYTE_MODE                  14      //A 6 , T 2,G 6
#define BIT_FIFO_OFLOW_EN               0x10

#define BIT_ACC_SELFTEST                0xE0
#define BIT_GYRO_SELFTEST               0xE0

#define BIT_WOM_EN                      0xE0 //0x40?
//REG_FIFO_WM_INT_STATUS
#define BIT_FIFO_WM_INT                 0x40
//REG_INT_STATUS
#define BIT_DRI_INT                     0x01
#define BIT_WOM_X_INT                   0x80
#define BIT_WOM_Y_INT                   0x40
#define BIT_WOM_Z_INT                   0x20
#define BIT_FIFO_OFLOW_INT              0x10

//REG_ACC_INTEL_CTRL
#define BIT_ACC_INTEL_EN            0x80
#define BIT_ACC_INTEL_MODE          0x40
#define BIT_ACCEL_FCHOICE_OIS_B     0x30
#define BIT_WOM_INT_MODE            0x01

#define DEF_WOM_THRESHOLD           (100 >> 2) //default:100mg, max=1020mg, LSB = 4mg.

/* ICM2060X Register Map  (Please refer to ICM2060X Specifications) */
#define ICM2060X_REG_DEVID              0x75
#define remapFifoDelay(delay_ns, pll)     ((delay_ns) * (1270ULL)/(1270ULL + pll))

#define ICM2060X_MAX_ODR_500HZ (1)

//#define ICM2060X_FIFO_SIZE                       2080
#define G                                        9.80665
#define PI                                       3.141592
#define KSCALE_ACC_8G_RANGE                      0.002394202f  //sensitivity m/s^2   // ACC_range * 9.81f / 65536.0f;
#define KSCALE_GYRO_2000_RANGE                   0.001065264f  // sesitivity rps // GYR_range * M_PI / (180.0f * 65536.0f);
#define KSCALE_GYRO_500_RANGE                    0.000266462f   

#define ICM2060X_ODR_2HZ_REG_VALUE                    (0xe)
#define ICM2060X_ODR_3HZ_REG_VALUE                    (0xd)
#define ICM2060X_ODR_6HZ_REG_VALUE                    (0xc)
#define ICM2060X_ODR_13HZ_REG_VALUE                   (0xb)
#define ICM2060X_ODR_25HZ_REG_VALUE                   (0xa)
#define ICM2060X_ODR_50HZ_REG_VALUE                   (0x9)
#define ICM2060X_ODR_100HZ_REG_VALUE                  (0x8)
#define ICM2060X_ODR_200HZ_REG_VALUE                  (0x7)
#define ICM2060X_ODR_500HZ_REG_VALUE                  (0xf)
#define ICM2060X_ODR_1000HZ_REG_VALUE                 (0x6)
#define ICM2060X_ODR_2000HZ_REG_VALUE                 (0x5)
#define ICM2060X_ODR_4000HZ_REG_VALUE                 (0x4)
#define ICM2060X_ODR_8000HZ_REG_VALUE                 (0x3)

//Temperature
#define TEMP_SENSITIVITY            326.8f //326.8LSB/Celsius degree
#define ROOM_TEMP_OFFSET            25.0f //Celsius degree

#define max(x, y)   (x > y ? x : y)

#define AXIS_X              0
#define AXIS_Y              1
#define AXIS_Z              2
#define AXES_NUM            3

/* Support odr range 25HZ - 200HZ */
static const uint32_t ICM2060XHWRates[] = {
    SENSOR_HZ(5.0f),
    SENSOR_HZ(10.0f),
    SENSOR_HZ(15.0f),
    SENSOR_HZ(20.0f),
    SENSOR_HZ(25.0f),
    SENSOR_HZ(40.0f),
    SENSOR_HZ(50.0f),
    SENSOR_HZ(100.0f),
    SENSOR_HZ(200.0f),
    SENSOR_HZ(250.0f),
#if ICM2060X_MAX_ODR_500HZ
    SENSOR_HZ(500.0f),
#endif
    0,
};

/* chip type */
enum chip_type {
    UNVALID_TYPE = 0,
    MPU6881,
	MPU6887,
    ICM20600,
    ICM20602,
    ICM20608,
    ICM20609,
    ICM20689,
    ICM20690,
    ICM20622,
    ICM20627,
    ICM20607,
    ICM20621
};


#define SPI_PACKET_SIZE 30
#define ICM2060X_MAX_FIFO_SIZE (1050) //1024,Max FIFO count size for FIFO overflow case and add some additional SPI operation count for reserve
#define SPI_BUF_SIZE    (ICM2060X_MAX_FIFO_SIZE + 4)
#define EVT_SENSOR_ANY_MOTION       sensorGetMyEventType(SENS_TYPE_ANY_MOTION)

#define SPI_WRITE_0(addr, data) spiQueueWrite(addr, data, 2)
#define SPI_WRITE_1(addr, data, delay) spiQueueWrite(addr, data, delay)
#define GET_SPI_WRITE_MACRO(_1, _2, _3, NAME, ...) NAME
#define SPI_WRITE(...) GET_SPI_WRITE_MACRO(__VA_ARGS__, SPI_WRITE_1, SPI_WRITE_0)(__VA_ARGS__)

#define SPI_READ_0(addr, size, buf) spiQueueRead(addr, size, buf, 0)
#define SPI_READ_1(addr, size, buf, delay) spiQueueRead(addr, size, buf, delay)
#define GET_SPI_READ_MACRO(_1, _2, _3, _4, NAME, ...) NAME
#define SPI_READ(...) GET_SPI_READ_MACRO(__VA_ARGS__, SPI_READ_1, SPI_READ_0)(__VA_ARGS__)

#if MTK_VERSION_DIFF
#define sendDataCalibrationResult(sensortype,caliresult,status) accGyroSendCalibrationResult(sensortype,caliresult,status)
#endif

#if MTK_VERSION_DIFF
#define OVERLAY_WORK_INV OVERLAY_ID_ACCGYRO     // if there shows OVERLAY_ID_ACCGYRO does not define in mtk_overlay_init.h.  Then please remove this line 
#else
#define OVERLAY_WORK_INV OVERLAY_WORK_00
#endif

enum ICM2060XState {
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
#if SUPPORT_SELFTEST
    STATE_ACC_SELF_TEST = CHIP_ACC_SELFTEST,
    STATE_ACC_SELF_TEST_DONE = CHIP_ACC_SELFTEST_DONE,
#endif
    STATE_GYRO_ENABLE = CHIP_GYRO_ENABLE,
    STATE_GYRO_ENABLE_DONE = CHIP_GYRO_ENABLE_DONE,
    STATE_GYRO_DISABLE = CHIP_GYRO_DISABLE,
    STATE_GYRO_DISABLE_DONE = CHIP_GYRO_DISABLE_DONE,
    STATE_GYRO_RATECHG = CHIP_GYRO_RATECHG,
    STATE_GYRO_RATECHG_DONE = CHIP_GYRO_RATECHG_DONE,
    STATE_GYRO_CALI = CHIP_GYRO_CALI,
    STATE_GYRO_CALI_DONE = CHIP_GYRO_CALI_DONE,
    STATE_GYRO_CFG = CHIP_GYRO_CFG,
    STATE_GYRO_CFG_DONE = CHIP_GYRO_CFG_DONE,
#if SUPPORT_SELFTEST
    STATE_GYRO_SELF_TEST = CHIP_GYRO_SELFTEST,
    STATE_GYRO_SELF_TEST_DONE = CHIP_GYRO_SELFTEST_DONE,
#endif    
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
    STATE_SW_RESET_W,
    STATE_RESET_CHECK,
    STATE_INT_STATUS,
    STATE_CLKSEL_REG_READ,
    STATE_CLKSEL_REG_WRITE,
    STATE_POWER_R,
    STATE_ENPOWER_W,
    STATE_POWER2_R,
    STATE_INIT_REG,
    STATE_SENSOR_REGISTRATION,
    STATE_EINT_REGISTRATION,
    STATE_FIFO_COUNT_CHECK,
#if SUPPORT_SELFTEST
    STATE_SELFTEST_INIT,
    STATE_ACC_SELFTEST_PHASE1,
    STATE_ACC_SELFTEST_PHASE2,
    STATE_ACC_SELFTEST_RESULT,
    STATE_GYRO_SELFTEST_PHASE1,
    STATE_GYRO_SELFTEST_PHASE2,
    STATE_GYRO_SELFTEST_RESULT,
    STATE_SELFTEST_RECOVER,
#endif
#if DEBUG_PHASE
    STATE_REGDUMP_READ,
    STATE_REGDUMP_RESULT,
#endif
};

enum SensorIndex {
    ACC = 0,
    GYR,
#if MTK_VERSION_DIFF
    ANYMO,
#endif
    NUM_OF_SENSOR,
};

#if !MTK_VERSION_DIFF
enum SensorHandleIndex {
    ANYMO = 0,
    NUM_OF_HANDLE,
};
#endif 

struct scale_factor {
    unsigned char  whole;
    unsigned char  fraction;
};

struct acc_data_resolution {
    struct scale_factor scalefactor;
    int                 sensitivity;
};
struct gyro_data_resolution {
    float   sensitivity;
};

struct ICM2060XSensor {
    float staticCali[AXES_NUM];
    float sensitivity;
    uint64_t latency;
    int32_t accuracy;
    uint32_t rate;
    uint32_t hwRate;
    uint32_t preRealRate;
    uint32_t handle;
    uint32_t samplesToDiscard;  //Terry
    
    bool powered;
    bool configed;
    bool startCali;
    bool needDiscardSample;     //Terry
    
#if !MTK_VERSION_DIFF 
    enum SensorHandleIndex idx_handle;
#endif
};

#if SUPPORT_SELFTEST
/* sensor x,y,z */
struct sensor_st_code {
    uint8_t x;
    uint8_t y;
    uint8_t z;
};

/* ST_OTP = (2620 / 2^FS) * 1.01^(ST_Code -1) (LSB) */
//lookup table: convert selft test code in register to otp value
static const uint16_t mpu_st_tb[256] = {
    2620, 2646, 2672, 2699, 2726, 2753, 2781, 2808,
    2837, 2865, 2894, 2923, 2952, 2981, 3011, 3041,
    3072, 3102, 3133, 3165, 3196, 3228, 3261, 3293,
    3326, 3359, 3393, 3427, 3461, 3496, 3531, 3566,
    3602, 3638, 3674, 3711, 3748, 3786, 3823, 3862,
    3900, 3939, 3979, 4019, 4059, 4099, 4140, 4182,
    4224, 4266, 4308, 4352, 4395, 4439, 4483, 4528,
    4574, 4619, 4665, 4712, 4759, 4807, 4855, 4903,
    4953, 5002, 5052, 5103, 5154, 5205, 5257, 5310,
    5363, 5417, 5471, 5525, 5581, 5636, 5693, 5750,
    5807, 5865, 5924, 5983, 6043, 6104, 6165, 6226,
    6289, 6351, 6415, 6479, 6544, 6609, 6675, 6742,
    6810, 6878, 6946, 7016, 7086, 7157, 7229, 7301,
    7374, 7448, 7522, 7597, 7673, 7750, 7828, 7906,
    7985, 8065, 8145, 8227, 8309, 8392, 8476, 8561,
    8647, 8733, 8820, 8909, 8998, 9088, 9178, 9270,
    9363, 9457, 9551, 9647, 9743, 9841, 9939, 10038,
    10139, 10240, 10343, 10446, 10550, 10656, 10763, 10870,
    10979, 11089, 11200, 11312, 11425, 11539, 11654, 11771,
    11889, 12008, 12128, 12249, 12371, 12495, 12620, 12746,
    12874, 13002, 13132, 13264, 13396, 13530, 13666, 13802,
    13940, 14080, 14221, 14363, 14506, 14652, 14798, 14946,
    15096, 15247, 15399, 15553, 15709, 15866, 16024, 16184,
    16346, 16510, 16675, 16842, 17010, 17180, 17352, 17526,
    17701, 17878, 18057, 18237, 18420, 18604, 18790, 18978,
    19167, 19359, 19553, 19748, 19946, 20145, 20347, 20550,
    20756, 20963, 21173, 21385, 21598, 21814, 22033, 22253,
    22475, 22700, 22927, 23156, 23388, 23622, 23858, 24097,
    24338, 24581, 24827, 25075, 25326, 25579, 25835, 26093,
    26354, 26618, 26884, 27153, 27424, 27699, 27976, 28255,
    28538, 28823, 29112, 29403, 29697, 29994, 30294, 30597,
    30903, 31212, 31524, 31839, 32157, 32479, 32804
};
#endif

static struct ICM2060XTask {
    struct ICM2060XSensor sensors[NUM_OF_SENSOR];
#if !MTK_VERSION_DIFF
    struct ICM2060XSensor sensors_handle[NUM_OF_SENSOR];
#endif

    uint64_t hwSampleTime;
    uint64_t last_hwSampleTime;
    uint64_t swSampleTime;
    uint64_t unmask_eint_time;

    uint64_t lastSampleTime;
    uint8_t *regBuffer;
    uint8_t *statusBuffer;
    uint8_t *int_statusBuffer;
    uint8_t *fifo_counter;
    uint8_t *fifo_overflow_counter;
#if SUPPORT_SELFTEST
    uint8_t *accel_st_code_buffer;
    uint8_t *gyro_st_code_buffer;
    uint8_t *rawdata_buffer;
#endif

#if DEBUG_PHASE
    uint8_t *debugintBuffer;
    uint8_t *regdumpBuffer;
    //uint8_t *debugintBuffer;
    //uint8_t *debugPWRBuffer;
    //uint8_t *debugFifoTH;
    //uint8_t *debugFifoEN;
#endif

    SpiCbkF spiCallBack;
    struct transferDataInfo dataInfo;
    struct accGyroDataPacket accGyroPacket;
    /* data for factory */
    struct TripleAxisDataPoint accFactoryData;
    struct TripleAxisDataPoint gyroFactoryData;

    int32_t accHwCali[AXES_NUM];
    int32_t gyroHwCali[AXES_NUM];
    struct acc_data_resolution *accReso;
    struct gyro_data_resolution *gyroReso;
    struct accGyro_hw *hw;
    struct sensorDriverConvert cvt;

    spi_cs_t cs;
    struct SpiMode mode;
    struct SpiPacket packets[SPI_PACKET_SIZE];
    struct SpiDevice *spiDev;
    uint8_t txrxBuffer[SPI_BUF_SIZE];
    uint16_t mWbufCnt;
    uint8_t mRegCnt;
    uint8_t mRetryLeft;
    int32_t debug_trace;
    uint32_t fifoDataToRead;

    int latch_time_id;
    uint16_t watermark;
    /* For save reg status */
    uint8_t int_src0;
    uint8_t smd_cfg_reg69; 
    //uint8_t acc_cfg0;
    //uint8_t gyro_cfg0;
    //uint8_t fifo_cfg1;
    uint8_t pwr_mgmr_config;
    uint8_t pwr_mgmr_config2;
    uint8_t acc_report_size;
    uint8_t gyro_report_size;
    uint64_t realtime_report_ts;
    uint8_t fifo_reRead_count;
    uint8_t int_pin_cfg;
	
#if SUPPORT_SELFTEST  
    struct sensor_st_code st_code[2];
    int     avg[3];
    int     acc_avg[3];
    int     acc_avg_st[3];
    int     gyro_avg[3];
    int     gyro_avg_st[3];
    
    uint8_t gyro_config;
    uint8_t accel_config;
    uint8_t gyro_st_code_reg[3];
    uint8_t selftest_rawdata_reg;

#endif   
    
    enum chip_type product;
    float chip_temper;
    
#ifdef CFG_MTK_CALIBRATION_V1_0
    struct AccFactoryCal accFactoryCal;
    struct GyroFactoryCal gyroFactoryCal;
#endif
    
} mTask;
static struct ICM2060XTask *ICM2060XDebugPoint;

static uint32_t first_data = 0;
static bool isrwm_after_set_rate = false;

static void calc_long_diff(uint32_t accRate, bool accConfiged, uint8_t accEventSize,
    uint32_t gyroRate, bool gyroConfiged, uint8_t gyroEventSize,
    uint32_t *minDelay, uint64_t *long_diff, uint8_t *long_EventSize)
{
    uint32_t maxRate = 0;

    if (accConfiged && gyroConfiged) {
        maxRate = (accRate > gyroRate) ? accRate : gyroRate;
        *minDelay = 1024000000000ULL / maxRate;
        *long_diff = ((accRate > gyroRate) ? accEventSize : gyroEventSize) * *minDelay;
        *long_EventSize = ((accRate > gyroRate) ? accEventSize : gyroEventSize);
    } else if (accConfiged && !gyroConfiged) {
        maxRate = accRate;
        *minDelay = 1024000000000ULL / maxRate;
        *long_diff = accEventSize * *minDelay;
        *long_EventSize = accEventSize;
    } else if (!accConfiged && gyroConfiged) {
        maxRate = gyroRate;
        *minDelay = 1024000000000ULL / maxRate;
        *long_diff = gyroEventSize * *minDelay;
        *long_EventSize = gyroEventSize;
    } else if (!accConfiged && !gyroConfiged) {
        *minDelay = 0;
        *long_diff = 0;
        *long_EventSize = 0;
    }
    return;
}
 
static void spi_report_fiforeamining_data_CallBack(void *cookie, int err);
static void spi_handle_fiforeaminingdata_CallBack(void *cookie, int err);

static void spiQueueWrite(uint8_t addr, uint8_t data, uint32_t delay) {
    mTask.packets[mTask.mRegCnt].size = 2;
    mTask.packets[mTask.mRegCnt].txBuf = &mTask.txrxBuffer[mTask.mWbufCnt];
    mTask.packets[mTask.mRegCnt].rxBuf = &mTask.txrxBuffer[mTask.mWbufCnt];
    mTask.packets[mTask.mRegCnt].delay = delay * 1000;
    mTask.txrxBuffer[mTask.mWbufCnt++] = addr;
    mTask.txrxBuffer[mTask.mWbufCnt++] = data;
    mTask.mWbufCnt = (mTask.mWbufCnt + 3) & 0xFFFC;
    mTask.mRegCnt++;
}

static void spiQueueRead(uint8_t addr, size_t size, uint8_t **buf, uint32_t delay) {
    *buf = &mTask.txrxBuffer[mTask.mWbufCnt];
    mTask.packets[mTask.mRegCnt].size = size + 1;  // first byte will not contain valid data
    mTask.packets[mTask.mRegCnt].txBuf = &mTask.txrxBuffer[mTask.mWbufCnt];
    mTask.packets[mTask.mRegCnt].rxBuf = *buf;
    mTask.packets[mTask.mRegCnt].delay = delay * 1000;
    mTask.txrxBuffer[mTask.mWbufCnt++] = 0x80 | addr;
    mTask.mWbufCnt = (mTask.mWbufCnt + size + 3) & 0xFFFC;
    mTask.mRegCnt++;
}

static int spiBatchTxRx(struct SpiMode *mode,
        SpiCbkF callback, void *cookie, const char *src) {
    int err = 0;

    if (mTask.mWbufCnt > SPI_BUF_SIZE) {
        INVLOG(LOG_INFO, "NO enough SPI buffer space, dropping transaction.\n");
        return -1;
    }
    if (mTask.mRegCnt > SPI_PACKET_SIZE) {
        INVLOG(LOG_INFO, "spiBatchTxRx too many packets!\n");
        return -1;
    }

    err = spiMasterRxTx(mTask.spiDev, mTask.cs, mTask.packets, mTask.mRegCnt, mode, callback, cookie);
    mTask.mRegCnt = 0;
    mTask.mWbufCnt = 0;
    return err;
}

/* remove cali information reset in init sturcture , to see further impace later  2018.12.26*/
static void initSensorStruct(struct ICM2060XSensor *sensor, enum SensorIndex idx) {
    sensor->powered = false;
    sensor->configed = false;
    sensor->startCali = false;
    sensor->staticCali[AXIS_X] = 0;
    sensor->staticCali[AXIS_Y] = 0;
    sensor->staticCali[AXIS_Z] = 0;
    sensor->accuracy = 0;
    sensor->rate = 0;
    sensor->latency = SENSOR_LATENCY_NODATA;
    sensor->hwRate = 0;
    sensor->preRealRate = 0;
}

static int ICM2060XCalcuOdr(uint32_t *rate, uint32_t *report_rate) {
    int i;

    for (i = 0; i < (ARRAY_SIZE(ICM2060XHWRates) - 1); i++) {
        if (*rate <= ICM2060XHWRates[i]) {
            *report_rate = ICM2060XHWRates[i];
            break;
        }
    }

    if (*rate > ICM2060XHWRates[(ARRAY_SIZE(ICM2060XHWRates) - 2)]) {
        i = (ARRAY_SIZE(ICM2060XHWRates) - 2);
        *report_rate = ICM2060XHWRates[i];
    }

    return  (*report_rate)/1024;
}

void ICM2060XTimerCbkF(void) {
    mTask.hwSampleTime = rtcGetTime();
}

static int ICM2060XResetWrite(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    uint8_t val = 0;

    INVLOG(LOG_INFO, "ICM2060XResetWrite 0x%x\n", mTask.regBuffer[1]);
    val = mTask.regBuffer[1] | ICM2060X_DEV_RESET;
    /* Wait 200ms to check reset status, then check again */
    SPI_WRITE(ICM2060X_PWR_MGMT_1, val, 200000);
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

static int ICM2060XResetCheck(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
                         
    if (!(BIT_DEVICE_RESET & mTask.regBuffer[1]))
        INVLOG(LOG_INFO, "ICM2060XResetCheck Done\n");
    else
        osLog(LOG_ERROR, "ICM2060XResetCheck Fail 0x%x\n", mTask.regBuffer[1]);
        
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int ICM2060XResetRead(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    SPI_READ(ICM2060X_PWR_MGMT_1, 1, &mTask.regBuffer);
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}


static int ICM2060XClkSelWrite(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    uint8_t val = 0;

    INVLOG(LOG_INFO, "ICM2060XClkSelWrite 0x%x\n", mTask.regBuffer[1]);
    val = mTask.regBuffer[1] | BIT_CLK_PLL;
    SPI_WRITE(ICM2060X_PWR_MGMT_1, val);
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

static int ICM2060X_PWRMGMT_Read(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    SPI_READ(ICM2060X_PWR_MGMT_1, 1, &mTask.regBuffer);
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

static int ICM2060X_PWRMGMT2_Read(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    SPI_READ(ICM2060X_PWR_MGMT_2, 1, &mTask.statusBuffer);
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

static int ICM2060XPowerEnableWrite(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
 
    INVLOG(LOG_INFO, "ICM2060XPowerEnableWrite 0x%x\n", mTask.regBuffer[1]);
    mTask.pwr_mgmr_config = mTask.regBuffer[1] & ~ICM2060X_SLEEP;
    SPI_WRITE(ICM2060X_PWR_MGMT_1, mTask.pwr_mgmr_config,5000);
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}



static void ICM2060XFIFO_Restart(void) {
    uint8_t i2c_dis = 0x00;

    if(mTask.product == ICM20627 || mTask.product == ICM20690)
        i2c_dis = 0x10; //in SPI MODE always disable i2c 
        
    INVLOG(LOG_INFO, "ICM2060XFIFO_Restart\n");
    SPI_WRITE(ICM2060X_REG_USER_CTRL, i2c_dis | FIFO_DIS);
    SPI_WRITE(ICM2060X_REG_FIFO_EN_2, 0x00);
    SPI_WRITE(ICM2060X_REG_USER_CTRL, i2c_dis | FIFO_RST);
    SPI_WRITE(ICM2060X_REG_FIFO_EN_2, 0xF8);
    SPI_WRITE(ICM2060X_REG_USER_CTRL, i2c_dis | FIFO_EN);
}

static void ICM2060XFIFO_Restart_Read(void) {
    uint8_t i2c_dis = 0x00;

    if(mTask.product == ICM20627 || mTask.product == ICM20690)
        i2c_dis = 0x10; //in SPI MODE always disable i2c

    SPI_WRITE(ICM2060X_REG_FIFO_EN_2, 0x00);
    //SPI_WRITE(ICM2060X_REG_USER_CTRL, i2c_dis | FIFO_DIS);
    
    if(mTask.product == ICM20690 || mTask.product == ICM20627)
        mTask.fifoDataToRead = ((mTask.fifo_overflow_counter[1] << 8) | mTask.fifo_overflow_counter[2]) * FIFO_BYTE_MODE;  
    else
        mTask.fifoDataToRead = (mTask.fifo_overflow_counter[1] << 8) | mTask.fifo_overflow_counter[2];  

    INVLOG(LOG_INFO, "ICM2060XFIFO_Restart read %d\n",mTask.fifoDataToRead);    
    SPI_READ(ICM2060X_REG_FIFO_R_W, mTask.fifoDataToRead, &mTask.regBuffer);
    SPI_WRITE(ICM2060X_REG_USER_CTRL, i2c_dis | FIFO_RST);
    SPI_WRITE(ICM2060X_REG_FIFO_EN_2, 0xF8);
    SPI_WRITE(ICM2060X_REG_USER_CTRL, i2c_dis | FIFO_EN);
}

#if DEBUG_WM_INTERUPT
static void ICM2060XEnable_WM() {
    uint8_t  buffer[2];
    uint16_t watermarkReg;

    watermarkReg = mTask.watermark;

    if(mTask.product != ICM20690 && mTask.product != ICM20627){
        if (watermarkReg < FIFO_BYTE_MODE)
            watermarkReg = FIFO_BYTE_MODE;
    }else{
        if (watermarkReg == 0)
        watermarkReg = 1;
    }
    
    INVLOG(LOG_INFO, "watermarkReg in Debug = %d\n", mTask.watermark);
    buffer[0] = watermarkReg & 0x00ff;
    buffer[1] = (watermarkReg & 0xff00) >> 8;

    SPI_WRITE(ICM2060X_REG_FIFO_WM_TH2 ,buffer[0]);
    if(mTask.product != ICM20690 && mTask.product != ICM20627)   // for record mode, there is no need to set High part of WTH
        SPI_WRITE(ICM2060X_REG_FIFO_WM_TH1, buffer[1]);
 
}
#endif

static void ICM2060XConfigFifo(bool odr_change) {
    uint8_t  buffer[2];
    uint16_t watermarkReg;
    uint8_t i2c_dis = 0x00;

    if(mTask.product == ICM20627 || mTask.product == ICM20690)
        i2c_dis = 0x10; //in SPI MODE always disable i2c

    watermarkReg = mTask.watermark;

   
    if(mTask.product != ICM20690 && mTask.product != ICM20627){
        if (watermarkReg < FIFO_BYTE_MODE)
            watermarkReg = FIFO_BYTE_MODE;
    }else{
        if (watermarkReg == 0)
        watermarkReg = 1;
    }
	
	#if DEBUG_STATUS
	INVLOG(LOG_INFO, "ICM2060XConfigFifo DUMP: PW0 %x PW1 %x ACC %d %d GY %d %d WOM %d\n",
		mTask.pwr_mgmr_config, mTask.pwr_mgmr_config2,
		mTask.sensors[ACC].powered,mTask.sensors[ACC].configed,
		mTask.sensors[GYR].powered,mTask.sensors[GYR].configed,
		#if MTK_VERSION_DIFF
		mTask.sensors[ANYMO].powered);
		#else
		mTask.sensors_handle[ANYMO].powered);
		#endif
	#endif

    INVLOG(LOG_INFO, "watermarkReg=%d\n", mTask.watermark);
    buffer[0] = watermarkReg & 0x00ff;
    buffer[1] = (watermarkReg & 0xff00) >> 8;

    if (odr_change) {
        /* reset & restart fifo first if odr change */
        ICM2060XFIFO_Restart();
        /* set threshold */
        SPI_WRITE(ICM2060X_REG_FIFO_WM_TH2 ,buffer[0]);
        
        if(mTask.product != ICM20690 && mTask.product != ICM20627)   // for record mode, there is no need to set High part of WTH 
            SPI_WRITE(ICM2060X_REG_FIFO_WM_TH1, buffer[1]);
 
        INVLOG(LOG_INFO, "ICM2060XConfigFifo: Reset, TH_L:0x%x, TH_H:0x%x\n",
            buffer[0], buffer[1]);
        //INVLOG(LOG_INFO, "fifo config rtc %llu\n",rtcGetTime());
    } else {
        SPI_WRITE(ICM2060X_REG_FIFO_WM_TH2 ,buffer[0]);
        if(mTask.product != ICM20690 && mTask.product != ICM20627)   // for record mode, there is no need to set High part of WTH
            SPI_WRITE(ICM2060X_REG_FIFO_WM_TH1, buffer[1]);
        if (mTask.sensors[ACC].configed || mTask.sensors[GYR].configed) {
            SPI_WRITE(ICM2060X_REG_USER_CTRL,i2c_dis | FIFO_EN);
            SPI_WRITE(ICM2060X_REG_FIFO_EN_2, 0xF8);
        } else {
            SPI_WRITE(ICM2060X_REG_USER_CTRL, i2c_dis | FIFO_DIS);        // if acc and gyro all off, fifo disable
            SPI_WRITE(ICM2060X_REG_FIFO_EN_2, 0x00);
        }
        INVLOG(LOG_INFO, "ICM2060XConfigFifo, TH_L:0x%x, TH_H:0x%x\n",
            buffer[0], buffer[1]);
    }
}

static uint16_t ICM2060XCalcuWm(void) {
    uint8_t handle;
    uint64_t min_latency = SENSOR_LATENCY_NODATA;
    uint8_t min_watermark = 1;
    /* FIFO length is 1024 and a record is 16Bytes, so max_watermark set 60 */
    uint8_t max_watermark = 42;
    uint16_t watermark = 0;
    uint32_t temp_cnt, total_cnt = 0;
    uint32_t temp_delay = 0;

    //max_watermark = 54 < (MAX_RECV_PACKET/2) ? 54 : (MAX_RECV_PACKET/2); /*60*/
    
    for (handle = ACC; handle < NUM_OF_SENSOR; handle++) {
        if (mTask.sensors[handle].hwRate && mTask.sensors[handle].latency != SENSOR_LATENCY_NODATA) {
            min_latency =
                mTask.sensors[handle].latency < min_latency ? mTask.sensors[handle].latency : min_latency;
        }
    }
    /*if acc and gyr off or acc and gyr latency = SENSOR_LATENCY_NODATA, watermark = 0 or 1 or 2*/
    if (min_latency == SENSOR_LATENCY_NODATA) {
        for (handle = ACC; handle < NUM_OF_SENSOR; handle++) {
            if (mTask.sensors[handle].hwRate) {
                watermark++;
            }
        }
    } else {
        for (handle = ACC; handle < NUM_OF_SENSOR; handle++) {
            if (mTask.sensors[handle].hwRate) {
                temp_delay = 1024000000000ULL / mTask.sensors[handle].hwRate;
                temp_cnt = min_latency / temp_delay;
                if(0 == temp_cnt)
                    temp_cnt = 1;
                min_watermark = mTask.sensors[handle].hwRate / SENSOR_HZ(400.0f);
                #if ICM2060X_MAX_ODR_500HZ
                min_watermark =  (mTask.sensors[handle].hwRate == SENSOR_HZ(500.0f)) ? 2 : min_watermark;
                #endif
                total_cnt = temp_cnt > min_watermark ? temp_cnt : min_watermark;
                INVLOG(LOG_INFO, "ICM2060XCalcuWm, delay=%d, latency:%lld, min_wm=%d, total_cnt=%d\n",
                    temp_delay, min_latency, min_watermark, total_cnt);
            }
        }

        watermark = total_cnt;
        watermark = watermark < min_watermark ? min_watermark : watermark;
        watermark = watermark > max_watermark ? max_watermark : watermark;
    }
    
    if(mTask.product == ICM20690 || mTask.product == ICM20627)
        return watermark;   // for record mode , watermark is just the number of package
    else
        return watermark * FIFO_BYTE_MODE;/* for byte mode, watermark should be multipled by FIFO package size */
}


#if SUPPORT_SELFTEST
static int ICM2060X_Selftest_Recover(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    int odr = 0;
    uint8_t regValue = 0x00;
    uint32_t sampleRate = 0;
    uint32_t maxRate = 0;

    INVLOG(LOG_INFO, "ICM2060X_Selftest_Recover\n");

    mTask.int_src0 |= BIT_FIFO_OFLOW_EN;
    INVLOG(LOG_INFO, "recover write full ICM2060XEintRegistration interrupt status 0x%x\n", mTask.int_src0);
    SPI_WRITE(ICM2060X_REG_INT_ENABLE, mTask.int_src0 | BIT_FIFO_OFLOW_EN );
  

    if(false == mTask.sensors[ACC].powered && 
       false == mTask.sensors[GYR].powered &&
       false == mTask.sensors[ANYMO].powered){

       INVLOG(LOG_INFO, "no sensor opened before return \n");
       return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    
       }
     
    //1st reopen sensor 
    if(true == mTask.sensors[ACC].powered || true == mTask.sensors[GYR].powered ){
        INVLOG(LOG_INFO, "Recover power state from selftest\n");
        mTask.pwr_mgmr_config &= ~ICM2060X_SLEEP;
        SPI_WRITE(ICM2060X_PWR_MGMT_1, mTask.pwr_mgmr_config,5000);  
    }

    if(true == mTask.sensors[ACC].powered){
        mTask.pwr_mgmr_config2 &= ~BIT_PWR_ACCEL_STBY;
        SPI_WRITE(ICM2060X_PWR_MGMT_2, mTask.pwr_mgmr_config2,40000); //set 40ms for accel start-up time, note:at least 200us here to sync fly changes
    }

    if(true == mTask.sensors[GYR].powered){
        mTask.pwr_mgmr_config2 &= ~BIT_PWR_GYRO_STBY;
        SPI_WRITE(ICM2060X_PWR_MGMT_2, mTask.pwr_mgmr_config2,80000); //set 80ms for gyro start-up time, note:at least 200us here to sync fly changes
    }

    if(true == mTask.sensors[ACC].configed || true == mTask.sensors[GYR].configed){
        INVLOG(LOG_INFO, "recover sensor A/G application\n");
    //2st recover odr 
        maxRate = max( mTask.sensors[ACC].hwRate, mTask.sensors[GYR].hwRate);
        mTask.sensors[ACC].hwRate = maxRate;
        mTask.sensors[GYR].hwRate = maxRate;     

        odr = ICM2060XCalcuOdr(&maxRate, &sampleRate);
        if(15 == odr)
            regValue = 66;
        else
            regValue = ODR2SMPLRT_DIV(odr); 
            
        SPI_WRITE(ICM2060X_REG_SAMRT_DIV, regValue);
    // 3st config fifo again
        INVLOG(LOG_INFO, "set odr %d,mTask.sensors[ACC].hwRate %d,rlatency %d\n",odr,mTask.sensors[ACC].hwRate,mTask.sensors[ACC].latency);
        registerAccGyroFifoInfo((mTask.sensors[ACC].hwRate == 0) ? 0 : 1024000000000 / mTask.sensors[ACC].hwRate,
            (mTask.sensors[GYR].hwRate == 0) ? 0 : 1024000000000 / mTask.sensors[GYR].hwRate);

        mTask.watermark = ICM2060XCalcuWm();
        ICM2060XConfigFifo(true);
    
        first_data = 1;
        isrwm_after_set_rate = false;
    }

    // 4st reconfig anymo
    if(true == mTask.sensors[ANYMO].powered){
        INVLOG(LOG_INFO, "recover sensor AnyMotion\n");
        
        if(false == mTask.sensors[ACC].powered && false == mTask.sensors[GYR].powered){
	        INVLOG(LOG_INFO, "ICM2060X_Selftest_Recover: Selftest go out sleep\n");
	        mTask.pwr_mgmr_config &= ~ICM2060X_SLEEP;
	        SPI_WRITE(ICM2060X_PWR_MGMT_1, mTask.pwr_mgmr_config,5000); 
	        // if acc & gyro are all off, set sample rate to 10hz , otherwise do no change sample dive
	        SPI_WRITE(ICM2060X_REG_SAMRT_DIV, 99);
        }
 
        if(false == mTask.sensors[ACC].powered){
           // enable acc 
            mTask.sensors[ACC].powered = true;
            mTask.pwr_mgmr_config2 &= ~BIT_PWR_ACCEL_STBY;
            SPI_WRITE(ICM2060X_PWR_MGMT_2, mTask.pwr_mgmr_config2,20000); //set 20ms for accel start-up time, note:at least 200us here to sync fly changes  
        }

        /*config wom mode to enable */
        mTask.smd_cfg_reg69 |= (BIT_ACC_INTEL_EN | BIT_ACC_INTEL_MODE | BIT_WOM_INT_MODE);
        SPI_WRITE(ICM2060X_REG_ACCEL_INTEL_CTRL,mTask.smd_cfg_reg69);

        /*config WOM threshold */
        SPI_WRITE(ICM2060X_REG_WOM_THR_X,DEF_WOM_THRESHOLD);
        SPI_WRITE(ICM2060X_REG_WOM_THR_Y,DEF_WOM_THRESHOLD);
        SPI_WRITE(ICM2060X_REG_WOM_THR_Z,DEF_WOM_THRESHOLD);
        SPI_WRITE(ICM2060X_REG_WOM_THR,DEF_WOM_THRESHOLD);

        /* enable interrupt of wom  */
        mTask.int_src0 |= BIT_WOM_EN;
        SPI_WRITE(ICM2060X_REG_INT_ENABLE, mTask.int_src0);
    }

   return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

static int ICM2060XSelftest_acc_result(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
 
    uint8_t st_code[3] = {0};
    uint16_t st_otp[3] = {0};
    int32_t st_shift_cust[3] = {0};
    bool otp_value_zero = false;
    uint8_t test_result = 1;
    uint8_t i;

    st_code[0] = mTask.st_code[ACC].x;
    st_code[1] = mTask.st_code[ACC].y;
    st_code[2] = mTask.st_code[ACC].z;

    mTask.acc_avg_st[0] = mTask.avg[0];
    mTask.acc_avg_st[1] = mTask.avg[1];
    mTask.acc_avg_st[2] = mTask.avg[2];

    INVLOG(LOG_INFO,  "acc_avg_st %d %d %d\n",mTask.acc_avg_st[0],mTask.acc_avg_st[1],mTask.acc_avg_st[2] );
    
    /* lookup table to get selftest OTP value according to selftest code */
    for (i = 0; i < 3; i++) {
        if (st_code[i] != 0) {
            st_otp[i] = mpu_st_tb[st_code[i] - 1];
        } else {
            INVLOG(LOG_INFO,  "self test code is zero\n");
            st_otp[i] = 0;
            otp_value_zero = true;
        }
    }

    for (i = 0; i < 3; i++) {
        st_shift_cust[i] = mTask.acc_avg_st[i] - mTask.acc_avg[i];
        if (!otp_value_zero) {
            /* Self Test Pass/Fail Criteria A */
            if (st_shift_cust[i] < (st_otp[i] * DEF_SELFTEST_PRECISION * DEF_ACC_ST_SHIFT_DELTA_MIN / 100)) {
                INVLOG(LOG_INFO,  "ACC Fail@Criteria A(<MIN): st_shift_cust[%d] = %d, st_otp[%d] = %d",
                                                i, st_shift_cust[i], i, st_otp[i]);
                test_result = 0;
            }
            if (st_shift_cust[i] > (st_otp[i] * DEF_SELFTEST_PRECISION * DEF_ACC_ST_SHIFT_DELTA_MAX / 100)) {
                INVLOG(LOG_INFO,  "ACC Fail@Criteria A(>MAX): st_shift_cust[%d] = %d, st_otp[%d] = %d",
                                                i, st_shift_cust[i], i, st_otp[i]);
                test_result = 0;
            }
        } else {
            /* Self Test Pass/Fail Criteria B */
            if (abs(st_shift_cust[i]) < (DEF_ACCEL_ST_AL_MIN * 16384 / 1000 * DEF_SELFTEST_PRECISION)) {
                INVLOG(LOG_INFO,  "ACC Fail@Criteria B(<MIN): st_shift_cust[%d] = %d", i, st_shift_cust[i]);
                test_result = 0;
            }
            if (abs(st_shift_cust[i]) > (DEF_ACCEL_ST_AL_MAX * 16384 / 1000 * DEF_SELFTEST_PRECISION)) {
                INVLOG(LOG_INFO,  "ACC Fail@Criteria B(>MAX): st_shift_cust[%d] = %d", i, st_shift_cust[i]);
                test_result = 0;
            }
        }
    }

    INVLOG(LOG_INFO, "AccGetAccSelfvalue: %d", test_result);
    accGyroSendTestResult(SENS_TYPE_ACCEL, test_result); // 0 fail, 1 success, and you could define by yourself
 
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
    
}

static int ICM2060XSelftest_gyro_result(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
 
    uint8_t st_code[3] = {0};
    uint16_t st_otp[3] = {0};
    int32_t st_shift_cust[3] = {0};
    bool otp_value_zero = false;
    uint8_t test_result = 1;
    uint8_t i;

    st_code[0] = mTask.st_code[GYR].x;
    st_code[1] = mTask.st_code[GYR].y;
    st_code[2] = mTask.st_code[GYR].z;

    mTask.gyro_avg_st[0] = mTask.avg[0];
    mTask.gyro_avg_st[1] = mTask.avg[1];
    mTask.gyro_avg_st[2] = mTask.avg[2];

    INVLOG(LOG_INFO,  "gyro_avg_st %d %d %d\n",mTask.gyro_avg_st[0],mTask.gyro_avg_st[1],mTask.gyro_avg_st[2] );
    
    /* lookup table to get selftest OTP value according to selftest code */
    for (i = 0; i < 3; i++) {
        if (st_code[i] != 0) {
            st_otp[i] = mpu_st_tb[st_code[i] - 1];
        } else {
            INVLOG(LOG_INFO, "self test code is zero");
            st_otp[i] = 0;
            otp_value_zero = true;
        }
    }

    for (i = 0; i < 3; i++) {
        st_shift_cust[i] = mTask.gyro_avg_st[i] - mTask.gyro_avg[i];
        if (!otp_value_zero) {
            /* Self Test Pass/Fail Criteria A */
            if (st_shift_cust[i] < (st_otp[i] * DEF_SELFTEST_PRECISION * DEF_GYRO_ST_SHIFT_DELTA / 100)) {
                INVLOG(LOG_INFO, "GYRO Fail@Criteria A: st_shift_cust[%d] = %d, st_otp[%d] = %d",
                                                i, st_shift_cust[i], i, st_otp[i]);
                test_result = 0;
            }
        } else {
            /* Self Test Pass/Fail Criteria B */
            if (abs(st_shift_cust[i]) < DEF_GYRO_ST_AL * 32768 / 250 * DEF_SELFTEST_PRECISION) {
                INVLOG(LOG_INFO, "GYRO Fail@Criteria B: st_shift_cust[%d] = %d", i, st_shift_cust[i]);
                test_result = 0;
            }
        }
    }

    if (test_result) {
        /* Self Test Pass/Fail Criteria C */
        for (i = 0; i < 3; i++) {
            if (abs( mTask.gyro_avg[i]) > DEF_GYRO_OFFSET_MAX * 32768 / 250 * DEF_SELFTEST_PRECISION) {
                INVLOG(LOG_INFO, "GYRO Fail@Criteria C: gyro[%d] = %d", i, mTask.gyro_avg[i]);
                test_result = 0;
            }
        }
    }
    
    osLog(LOG_ERROR, "Gyro GetGyroSelfvalue: %d", test_result);
    accGyroSendTestResult(SENS_TYPE_GYRO, test_result); // 0 fail, 1 success, and you could define by yourself
 
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
    
}

static void spi_readraw_data_CallBack(void *cookie, int err) {

    static uint8_t i;
    static int sum[3] ={0};
    uint8_t j;
    int16_t raw_data[AXES_NUM] = {0};

   if (err != 0) {
        osLog(LOG_ERROR, "icm206xx: spiIsrCallBack err\n");
        sensorFsmEnqueueFakeSpiEvt(mTask.spiCallBack, cookie, ERROR_EVT);
        return;
    }    
    
    /* Use big endian mode */
    raw_data[AXIS_X] = (mTask.rawdata_buffer[2] | mTask.rawdata_buffer[1] << 8);
    raw_data[AXIS_Y] = (mTask.rawdata_buffer[4] | mTask.rawdata_buffer[3] << 8);
    raw_data[AXIS_Z] = (mTask.rawdata_buffer[6] | mTask.rawdata_buffer[5] << 8);

    INVLOG(LOG_INFO, "rawdata %d %d %d\n", raw_data[AXIS_X],raw_data[AXIS_Y],raw_data[AXIS_Z]);

    for (j = 0; j < 3; j++) {
            sum[j] += raw_data[j];
        }
    
    if(i < SELFTEST_SAMPLES_NUMBER -1){
        i++;
        //read raw data reg until all get required sample number
        SPI_READ(mTask.selftest_rawdata_reg, 6, &mTask.rawdata_buffer,1000);  //delay 1ms for wait new sample 
        spiBatchTxRx(&mTask.mode, spi_readraw_data_CallBack, cookie, __FUNCTION__);
    }
    else {
        /* average samples and enlarge value */
        for (j = 0; j < 3; j++) {
            sum[j] = sum[j] / SELFTEST_SAMPLES_NUMBER;//average samples.
            mTask.avg[j] = sum[j] * DEF_SELFTEST_PRECISION;//enlarge value to keep precision.
            sum[j]= 0;
        }
        // all sample gotten , switch to next state 
        i = 0;
        sensorFsmEnqueueFakeSpiEvt(mTask.spiCallBack, cookie, SUCCESS_EVT);
    }
}
/* *********************acc self test ************************************/
static int icm2060XSelftest_startread_accdata(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    INVLOG(LOG_INFO, "start acc Selftest\n");
    mTask.spiCallBack = spiCallBack;
    
    mTask.st_code[ACC].x = mTask.accel_st_code_buffer[1];
    mTask.st_code[ACC].y = mTask.accel_st_code_buffer[2];
    mTask.st_code[ACC].z = mTask.accel_st_code_buffer[3];

    mTask.selftest_rawdata_reg = ICM2060X_REG_ACC_DATA_XOUTH;
    SPI_READ(mTask.selftest_rawdata_reg, 6, &mTask.rawdata_buffer,1000);
    
    INVLOG(LOG_INFO, "chip_read_st_code(acc[%d,%d,%d]).\n", mTask.st_code[ACC].x,mTask.st_code[ACC].y,mTask.st_code[ACC].z);

    return spiBatchTxRx(&mTask.mode, spi_readraw_data_CallBack, next_state, __FUNCTION__);
}

static int icm2060XSelftest_startread_STaccdata(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    INVLOG(LOG_INFO, "start acc Selftest in ST mode\n");
    mTask.spiCallBack = spiCallBack;

    mTask.acc_avg[0] = mTask.avg[0];
    mTask.acc_avg[1] = mTask.avg[1];
    mTask.acc_avg[2] = mTask.avg[2];

    INVLOG(LOG_INFO, "(acc avg[%d,%d,%d]).\n", mTask.acc_avg[0],mTask.acc_avg[1],mTask.acc_avg[2]);
    //enable selftest mode 
    mTask.accel_config |= BIT_ACC_SELFTEST;
    SPI_WRITE(ICM2060X_REG_ACC_CONFIG, mTask.accel_config,20000);  //delay 20ms 

    mTask.selftest_rawdata_reg = ICM2060X_REG_ACC_DATA_XOUTH;
    SPI_READ(mTask.selftest_rawdata_reg, 6, &mTask.rawdata_buffer,1000);
    
    return spiBatchTxRx(&mTask.mode, spi_readraw_data_CallBack, next_state, __FUNCTION__);
}

/* *********************gyro self test ************************************/

static int icm2060XSelftest_startread_gyrodata(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    INVLOG(LOG_INFO, "start gyro Selftest\n");
    mTask.spiCallBack = spiCallBack;
    
    mTask.st_code[GYR].x = mTask.gyro_st_code_buffer[1];
    mTask.st_code[GYR].y = mTask.gyro_st_code_buffer[2];
    mTask.st_code[GYR].z = mTask.gyro_st_code_buffer[3];

    mTask.selftest_rawdata_reg = ICM2060X_REG_GYRO_DATA_XOUTH;
    SPI_READ(mTask.selftest_rawdata_reg, 6, &mTask.rawdata_buffer,1000);
    
    INVLOG(LOG_INFO, "chip_read_st_code(gyro[%d,%d,%d]).\n", mTask.st_code[ACC].x,mTask.st_code[ACC].y,mTask.st_code[ACC].z);

    return spiBatchTxRx(&mTask.mode, spi_readraw_data_CallBack, next_state, __FUNCTION__);
}

static int icm2060XSelftest_startread_STgyrodata(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    INVLOG(LOG_INFO, "start gyro Selftest in ST mode\n");
    mTask.spiCallBack = spiCallBack;

    mTask.gyro_avg[0] = mTask.avg[0];
    mTask.gyro_avg[1] = mTask.avg[1];
    mTask.gyro_avg[2] = mTask.avg[2];

    INVLOG(LOG_INFO, "(gyro avg[%d,%d,%d]).\n", mTask.gyro_avg[0],mTask.gyro_avg[1],mTask.gyro_avg[2]);
    //enable selftest mode of gyro 
    mTask.gyro_config |= BIT_GYRO_SELFTEST;
    SPI_WRITE(ICM2060X_REG_GYRO_CONFIG_1, mTask.gyro_config,20000);  //delay 20ms 

    mTask.selftest_rawdata_reg = ICM2060X_REG_GYRO_DATA_XOUTH;
    SPI_READ(mTask.selftest_rawdata_reg, 6, &mTask.rawdata_buffer,1000);
    
    return spiBatchTxRx(&mTask.mode, spi_readraw_data_CallBack, next_state, __FUNCTION__);
}

static int ICM2060XSelftest_init(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    uint8_t i;
    INVLOG(LOG_INFO, "ICM2060X SELFTEST InitConfig\n");
    
    if(mTask.product == ICM20600 || 
       mTask.product == ICM20607 ||
       mTask.product == ICM20602 ||
	   mTask.product == MPU6887)
		
        SPI_WRITE(ICM2060X_REG_I2C_IF,0x40);                             // disable i2c module in 0x70
    else
        SPI_WRITE(ICM2060X_REG_USER_CTRL,0x10);                          // disable i2c module in 0x6a

    /* exit sleep mode */
    mTask.pwr_mgmr_config &= ~ICM2060X_SLEEP;
    SPI_WRITE(ICM2060X_PWR_MGMT_1, mTask.pwr_mgmr_config,5000); 

       /* gyro  fs @25-dps   gyro DLPCFG @2 in self test mode */
    if(mTask.product == ICM20690 || mTask.product == ICM20627){
        SPI_WRITE(ICM2060X_REG_GYRO_CONFIG_1,(GYRO_FS_SEL_250 << SHIFT_GYRO_FS_SEL_4_2)| GYRO_FCHOICE);
        mTask.gyro_config = (GYRO_FS_SEL_250 << SHIFT_GYRO_FS_SEL_4_2)| GYRO_FCHOICE ;

        SPI_WRITE(ICM2060X_REG_GYRO_CONFIG,FIFO_MODE | 0x02 | BIT_FIFO_RECORD_MODE);     //  write bit 7 for 1        
        }
    else{
        SPI_WRITE(ICM2060X_REG_GYRO_CONFIG_1,(GYRO_FS_SEL_250 << SHIFT_GYRO_FS_SEL_4_3) | GYRO_FCHOICE);
        mTask.gyro_config = (GYRO_FS_SEL_250 << SHIFT_GYRO_FS_SEL_4_3) | GYRO_FCHOICE;

        SPI_WRITE(ICM2060X_REG_GYRO_CONFIG,FIFO_MODE | 0x02);     // write bit 7 for 0 
        }
      
    //mTask.acc_cfg0 =  ICM2060X_RANGE_2G;       // set 2G in selftest mode 
    SPI_WRITE(ICM2060X_REG_ACC_CONFIG, ICM2060X_RANGE_2G );
    mTask.accel_config = ICM2060X_RANGE_2G;
    
    // fifo size @ 1024bytes,acc DLPCFG@2
    SPI_WRITE(ICM2060X_REG_ACC_CONFIG_2, MAX_FIFO_SIZE_1024BYTES | ACCEL_AVGCFG_1_4X | 0x02 | ACCEL_FCHOICE);       

    // set odr to 1000hz
    SPI_WRITE(ICM2060X_REG_SAMRT_DIV, 0x00);

    if(mTask.product == ICM20600 || mTask.product == ICM20607 || mTask.product == ICM20602 || mTask.product == MPU6887){
        mTask.gyro_st_code_reg[0] = 0x50;
        mTask.gyro_st_code_reg[1] = 0x51;
        mTask.gyro_st_code_reg[2] = 0x52;
    }else{
        mTask.gyro_st_code_reg[0] = 0x00;
        mTask.gyro_st_code_reg[1] = 0x01;
        mTask.gyro_st_code_reg[2] = 0x02;
    }

    //reset selftest value flag
    for (i = 0; i < 3; i++) {
        mTask.avg[i] = 0;
        mTask.acc_avg[i] = 0;
        mTask.acc_avg_st[i] = 0 ;
        mTask.gyro_avg[i] = 0;
        mTask.gyro_avg_st[i] = 0;
    }
    /* read acc st code */
    SPI_READ(ICM2060X_REG_SELF_TEST_X_ACC, 3, &mTask.accel_st_code_buffer);

    /*read gyro st code  */
    SPI_READ(ICM2060X_REG_SELF_TEST_X_GYRO,3,&mTask.gyro_st_code_buffer);
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

#endif

static int ICM2060XInitConfig(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    INVLOG(LOG_INFO, "ICM2060XInitConfig\n");
    if(mTask.product == ICM20600 || 
       mTask.product == ICM20607 ||
       mTask.product == ICM20602 ||
	   mTask.product == MPU6887)
        SPI_WRITE(ICM2060X_REG_I2C_IF,0x40);                             // disable i2c module in 0x70
    else
        SPI_WRITE(ICM2060X_REG_USER_CTRL,0x10);                          // disable i2c module in 0x6a
        
    /* gyro  fs */
    mTask.pwr_mgmr_config2 = mTask.statusBuffer[1];
    //mTask.gyro_cfg0 =  FIFO_MODE | GYRO_DLPFCFG;                      //SET fifo mode
    if(mTask.product == ICM20690 || mTask.product == ICM20627)
        SPI_WRITE(ICM2060X_REG_GYRO_CONFIG_1,(GYRO_FS_SEL_2000 << SHIFT_GYRO_FS_SEL_4_2)| GYRO_FCHOICE);
    else
        SPI_WRITE(ICM2060X_REG_GYRO_CONFIG_1,(GYRO_FS_SEL_2000 << SHIFT_GYRO_FS_SEL_4_3) | GYRO_FCHOICE);
        
    if(mTask.product == ICM20690 || mTask.product == ICM20627)
        SPI_WRITE(ICM2060X_REG_GYRO_CONFIG,FIFO_MODE | GYRO_DLPFCFG | BIT_FIFO_RECORD_MODE);     //  write bit 7 for 1  
    else
        SPI_WRITE(ICM2060X_REG_GYRO_CONFIG,FIFO_MODE | GYRO_DLPFCFG);     // write bit 7 for 0 
    
    //mTask.acc_cfg0 =  ICM2060X_RANGE_8G;     // BIT_ACC_FS_4G -> 8G
    SPI_WRITE(ICM2060X_REG_ACC_CONFIG, ICM2060X_RANGE_8G | 0x02);
    // fifo size @ 1024bytes,acc DLPCFG@4
    SPI_WRITE(ICM2060X_REG_ACC_CONFIG_2, MAX_FIFO_SIZE_1024BYTES | ACCEL_AVGCFG_1_4X | ACCEL_DLPFCFG | ACCEL_FCHOICE);

    /* always set A T G into fifo, format the fifo package @14bytes  */
    SPI_WRITE(ICM2060X_REG_FIFO_EN_2, 0xF8);

    /* acc & gyro  stand by , sleep mode in idle */
    SPI_WRITE(ICM2060X_PWR_MGMT_2, mTask.pwr_mgmr_config2 | BIT_PWR_GYRO_STBY | BIT_PWR_ACCEL_STBY);
    SPI_WRITE(ICM2060X_PWR_MGMT_1, mTask.pwr_mgmr_config | ICM2060X_SLEEP);

    /*update globle status pwr_mgmr_config2, pwr_mgmr_config2 */

    mTask.pwr_mgmr_config2 |= BIT_PWR_GYRO_STBY | BIT_PWR_ACCEL_STBY;
    mTask.pwr_mgmr_config  |= ICM2060X_SLEEP;

    /* get interrupt reg status */
    SPI_READ(ICM2060X_REG_INT_ENABLE, 1, &mTask.regBuffer);

    /*get ACCEL_INTEL_CTRL  */
    SPI_READ(ICM2060X_REG_ACCEL_INTEL_CTRL,1,&mTask.statusBuffer);
    SPI_READ(ICM2060X_REG_INT_PIN_CFG,1,&mTask.int_statusBuffer);
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

static void ICM2060XFIFO_ResetOff(void) {
    uint8_t i2c_dis = 0x00;

    if(mTask.product == ICM20627 || mTask.product == ICM20690)
        i2c_dis = 0x10; //in SPI MODE always disable i2c 
        
    INVLOG(LOG_INFO, "ICM2060XFIFO_ResetOff\n");
    SPI_WRITE(ICM2060X_REG_FIFO_EN_2, 0x00);
    SPI_WRITE(ICM2060X_REG_USER_CTRL, i2c_dis | FIFO_DIS);
    SPI_WRITE(ICM2060X_REG_USER_CTRL, i2c_dis | FIFO_RST); 
}

static int ICM2060XgEnable(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    int ret = 0;

    INVLOG(LOG_INFO, "ICM2060XgEnable power mode=%d, time:%lld\n", mTask.sensors[ACC].powered, rtcGetTime());
    
    //Go back from sleep mode to normal mode if necessary 
    if(false == mTask.sensors[ACC].powered && false == mTask.sensors[GYR].powered){
        INVLOG(LOG_INFO, "ICM2060XgEnable: acc go out sleep\n");
        mTask.pwr_mgmr_config &= ~ICM2060X_SLEEP;
        SPI_WRITE(ICM2060X_PWR_MGMT_1, mTask.pwr_mgmr_config,5000);  
    }
    mTask.sensors[ACC].powered = true;
    mTask.pwr_mgmr_config2 &= ~BIT_PWR_ACCEL_STBY;
    SPI_WRITE(ICM2060X_PWR_MGMT_2, mTask.pwr_mgmr_config2,40000); //set 40ms for accel start-up time, note:at least 200us here to sync fly changes

    INVLOG(LOG_INFO, "ICM2060XgEnable ICM2060X_PWR_MGMT_2 %x\n",mTask.pwr_mgmr_config2);
    //mTask.sensors[ACC].configed = true;
    ret = spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return ret;
}

static int ICM2060XgDisable(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    int ret = 0;
    int odr = 0;
    uint8_t regValue = 0x00;
    uint32_t sampleRate = 0;
    struct accGyroCntlPacket cntlPacket;
    bool accelOdrChanged = false;
    
    INVLOG(LOG_INFO, "ICM2060XAccPowerOff\n");

    ret = rxControlInfo(&cntlPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);
    if (ret < 0) {
        sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "ICM2060XAccPowerOff, rx inSize and elemSize error\n");
        return -1;
    }

    mTask.sensors[ACC].rate = 0;
    mTask.sensors[ACC].preRealRate = 0;
    mTask.sensors[ACC].hwRate = 0;
    mTask.sensors[ACC].latency = SENSOR_LATENCY_NODATA;
    mTask.sensors[ACC].needDiscardSample = false;  
    mTask.sensors[ACC].samplesToDiscard = 0; 

#if MTK_VERSION_DIFF
    if ((mTask.sensors[GYR].powered == false) && (mTask.sensors[ANYMO].powered == false)) 
#else
    if ((mTask.sensors[GYR].powered == false) && (mTask.sensors_handle[ANYMO].powered == false)) 
#endif
    {
        mt_eint_mask(mTask.hw->eint_num);
         /* acc & gyro  stand by , sleep mode in idle */
        SPI_WRITE(ICM2060X_PWR_MGMT_2, mTask.pwr_mgmr_config2 | BIT_PWR_GYRO_STBY | BIT_PWR_ACCEL_STBY);
        SPI_WRITE(ICM2060X_PWR_MGMT_1, mTask.pwr_mgmr_config | ICM2060X_SLEEP);

        /*update globle status pwr_mgmr_config2, pwr_mgmr_config2 */

        mTask.pwr_mgmr_config2 |= BIT_PWR_GYRO_STBY | BIT_PWR_ACCEL_STBY;
        mTask.pwr_mgmr_config  |= ICM2060X_SLEEP;

    } else {  //  update gyro old ord
        if (mTask.sensors[GYR].hwRate != mTask.sensors[GYR].preRealRate 
            && mTask.sensors[GYR].powered == true ) {
            mTask.sensors[GYR].hwRate = mTask.sensors[GYR].preRealRate;
            //terry odr=CalcuOdr ?
            odr = ICM2060XCalcuOdr(&mTask.sensors[GYR].hwRate, &sampleRate);

            if(15 == odr)
                regValue = 66;
            else
                regValue = ODR2SMPLRT_DIV(odr);         
           
            SPI_WRITE(ICM2060X_REG_SAMRT_DIV, regValue);
            accelOdrChanged = true;
            INVLOG(LOG_INFO, "DIV %d,gyro change rate to preRealRate:%d\n",regValue, mTask.sensors[GYR].hwRate);
            
        }
        /* turn off acc if ANYMO OFF*/
    #if MTK_VERSION_DIFF
        if(mTask.sensors[ANYMO].powered == false){
    #else
        if(mTask.sensors_handle[ANYMO].powered == false){
    #endif
            SPI_WRITE(ICM2060X_PWR_MGMT_2, mTask.pwr_mgmr_config2 | BIT_PWR_ACCEL_STBY);
            
            /*update globle status , pwr_mgmr_config2 */
            mTask.pwr_mgmr_config2 |=  BIT_PWR_ACCEL_STBY;     
        }
            
    }
    INVLOG(LOG_INFO, "ICM2060XgDisable ICM2060X_PWR_MGMT_2 %x\n",mTask.pwr_mgmr_config2);
    registerAccGyroFifoInfo((mTask.sensors[ACC].hwRate == 0) ? 0 : 1024000000000 / mTask.sensors[ACC].hwRate,
        (mTask.sensors[GYR].hwRate == 0) ? 0 : 1024000000000 / mTask.sensors[GYR].hwRate);

    mTask.watermark = ICM2060XCalcuWm();
    mTask.sensors[ACC].powered = false;
    mTask.sensors[ACC].configed = false;  

    ICM2060XConfigFifo(accelOdrChanged);

    if(accelOdrChanged){
        first_data = 1;
        isrwm_after_set_rate = false;
    }
#if 0
    if (mTask.sensors[GYR].powered == true) {
        mTask.fifo_cfg1 |= 0x2F;    //Terry
        SPI_WRITE(ICM2060X_REG_FIFO_CONFIG_1, mTask.fifo_cfg1);
    }
#endif

    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

#if DEBUG_PHASE
static int ICM2060Xregdump(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
                         
     SPI_READ(ICM2060X_REG_SAMRT_DIV, 0x23 - ICM2060X_REG_SAMRT_DIV +1, &mTask.debugintBuffer);
     SPI_READ(0x37, 4, &mTask.statusBuffer);
     SPI_READ(0x68, 0x72 - 0x68, &mTask.regdumpBuffer);

    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

static int ICM2060Xregresult(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {

     uint8_t *reg_data = &mTask.debugintBuffer[1];
     uint8_t regaddr = ICM2060X_REG_SAMRT_DIV ;
     uint8_t i =0;
     while( regaddr <= 0x23)
        {
           INVLOG(LOG_INFO, "regaddr 0x%x regvalue 0x%x\n",regaddr,reg_data[i]);
           regaddr++;
           i++;
        }

     i =0;
     reg_data = &mTask.statusBuffer[1];
     regaddr  = 0x37;  

     while( regaddr <= 0x3A)
        {
           INVLOG(LOG_INFO, "regaddr 0x%x regvalue 0x%x\n",regaddr,reg_data[i]);
           regaddr++;
           i++;
        }
     
     i =0;
     reg_data = &mTask.regdumpBuffer[1];
     regaddr  = 0x68;
     while( regaddr < 0x72)
        {
           INVLOG(LOG_INFO, "regaddr 0x%x regvalue 0x%x\n",regaddr,reg_data[i]);
           regaddr++;
           i++;
        }
  
     
     sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
     return 0;
}
#endif
static int ICM2060XgRate(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    int ret = 0;
    int odr = 0;
    uint8_t regValue = 0x00;
    uint32_t sampleRate = 0;
    uint32_t maxRate = 0;
    bool accelOdrChanged = false;
    struct accGyroCntlPacket cntlPacket;

    ret = rxControlInfo(&cntlPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);
    if (ret < 0) {
        sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "ICM2060XgRate, rx inSize and elemSize error\n");
        return -1;
    }

    mTask.sensors[ACC].rate = cntlPacket.rate;
    mTask.sensors[ACC].latency = cntlPacket.latency;

    if (0 == mTask.sensors[ACC].preRealRate)  //Terry
    {
      mTask.sensors[ACC].needDiscardSample = true;
      mTask.sensors[ACC].samplesToDiscard = SAMPLE_TO_DISCARD;
    }

    odr = ICM2060XCalcuOdr(&mTask.sensors[ACC].rate, &sampleRate);
    mTask.sensors[ACC].preRealRate = sampleRate;

	#if DEBUG_STATUS
	INVLOG(LOG_INFO, "ICM2060XgRate DUMP: PW0 %x PW1 %x ACC %d %d GY %d %d WOM %d\n",
		mTask.pwr_mgmr_config, mTask.pwr_mgmr_config2,
		mTask.sensors[ACC].powered,mTask.sensors[ACC].configed,
		mTask.sensors[GYR].powered,mTask.sensors[GYR].configed,
		#if MTK_VERSION_DIFF
		mTask.sensors[ANYMO].powered);
		#else
		mTask.sensors_handle[ANYMO].powered);
		#endif
	#endif
	
    /*if gyr configed ,compare maxRate with acc and gyr rate*/
    if (mTask.sensors[GYR].configed) {
        maxRate = max(sampleRate, mTask.sensors[GYR].preRealRate);   // choose with preRealRate
        if ((maxRate != mTask.sensors[ACC].hwRate) || (maxRate != mTask.sensors[GYR].hwRate)) {
            mTask.sensors[ACC].hwRate = maxRate;
            mTask.sensors[GYR].hwRate = maxRate;
            odr = ICM2060XCalcuOdr(&maxRate, &sampleRate);
            if(15 == odr)
                regValue = 66;
            else
                regValue = ODR2SMPLRT_DIV(odr);         
            INVLOG(LOG_INFO, "reg value %x\n",regValue);
            SPI_WRITE(ICM2060X_REG_SAMRT_DIV, regValue);
          
            accelOdrChanged = true;
         } else {
            accelOdrChanged = false;
         }
    } else {
        if ((sampleRate != mTask.sensors[ACC].hwRate)) {
            mTask.sensors[ACC].hwRate = sampleRate;
            if(15 == odr)
                regValue = 66;
            else
                regValue = ODR2SMPLRT_DIV(odr);         
            INVLOG(LOG_INFO, "reg value %x\n",regValue);
            SPI_WRITE(ICM2060X_REG_SAMRT_DIV, regValue);
            accelOdrChanged = true;
            
        } else {
            accelOdrChanged = false;
        }
    }
    INVLOG(LOG_INFO, "set odr %d [ACC].hwRate %d,rlatency %lld\n",odr,mTask.sensors[ACC].hwRate,mTask.sensors[ACC].latency);
    registerAccGyroFifoInfo((mTask.sensors[ACC].hwRate == 0) ? 0 : 1024000000000 / mTask.sensors[ACC].hwRate,
        (mTask.sensors[GYR].hwRate == 0) ? 0 : 1024000000000 / mTask.sensors[GYR].hwRate);

    mTask.watermark = ICM2060XCalcuWm();

    mTask.sensors[ACC].configed = true;
    ICM2060XConfigFifo(accelOdrChanged);

    if(accelOdrChanged){
        first_data = 1;
        isrwm_after_set_rate = false;
    }
    
    mt_eint_unmask(mTask.hw->eint_num);
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

static int ICM2060XgyEnable(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    int ret = 0;

    INVLOG(LOG_INFO, "ICM2060XgyEnable power mode=%d\n", mTask.sensors[GYR].powered);

    
    //Go back from sleep mode to normal mode if necessary 
    if(false == mTask.sensors[ACC].powered && false == mTask.sensors[GYR].powered){
        INVLOG(LOG_INFO, "ICM2060XgyEnable: gyro go out sleep\n");
        mTask.pwr_mgmr_config &= ~ICM2060X_SLEEP;
        SPI_WRITE(ICM2060X_PWR_MGMT_1, mTask.pwr_mgmr_config,5000);  
    }
    mTask.sensors[GYR].powered = true;
    mTask.pwr_mgmr_config2 &= ~BIT_PWR_GYRO_STBY;
    SPI_WRITE(ICM2060X_PWR_MGMT_2, mTask.pwr_mgmr_config2,80000); //set 80ms for gyro start-up time, note:at least 200us here to sync fly changes
    //mTask.sensors[GYR].configed = true;
    ret = spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return ret;
}

static int ICM2060XgyDisable(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    int ret = 0;
    int odr = 0;
    uint8_t regValue = 0x00;
    uint32_t sampleRate = 0;
    struct accGyroCntlPacket cntlPacket;
    bool gyroOdrChanged = false;
    INVLOG(LOG_INFO, "ICM2060XgyDisable\n");

    ret = rxControlInfo(&cntlPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);
    if (ret < 0) {
        sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "ICM2060XAccPowerOff, rx inSize and elemSize error\n");
        return -1;
    }

    mTask.sensors[GYR].rate = 0;
    mTask.sensors[GYR].preRealRate = 0;
    mTask.sensors[GYR].hwRate = 0;
    mTask.sensors[GYR].latency = SENSOR_LATENCY_NODATA;
    mTask.sensors[GYR].needDiscardSample = false;  //Terry
    mTask.sensors[GYR].samplesToDiscard = 0;       //Terry

    /* enter off mode */
#if MTK_VERSION_DIFF    
    if ((mTask.sensors[ACC].powered == false) && (mTask.sensors[ANYMO].powered == false)) {
#else
    if ((mTask.sensors[ACC].powered == false) && (mTask.sensors_handle[ANYMO].powered == false)) {
#endif
        mt_eint_mask(mTask.hw->eint_num);
        /* acc & gyro  stand by , sleep mode in idle */
        SPI_WRITE(ICM2060X_PWR_MGMT_2, mTask.pwr_mgmr_config2 | BIT_PWR_GYRO_STBY | BIT_PWR_ACCEL_STBY);
        SPI_WRITE(ICM2060X_PWR_MGMT_1, mTask.pwr_mgmr_config | ICM2060X_SLEEP);

        /*update globle status pwr_mgmr_config2, pwr_mgmr_config2 */

        mTask.pwr_mgmr_config2 |= BIT_PWR_GYRO_STBY | BIT_PWR_ACCEL_STBY;
        mTask.pwr_mgmr_config  |= ICM2060X_SLEEP;
    } else if (mTask.sensors[ACC].powered == true) {  //  update ACC old ord
        if (mTask.sensors[ACC].hwRate != mTask.sensors[ACC].preRealRate) {
            mTask.sensors[ACC].hwRate = mTask.sensors[ACC].preRealRate;
            //terry odr=CalcuOdr ?
            odr = ICM2060XCalcuOdr(&mTask.sensors[ACC].hwRate, &sampleRate);
            if(15 == odr)
                regValue = 66;
            else
                regValue = ODR2SMPLRT_DIV(odr);         
           
            SPI_WRITE(ICM2060X_REG_SAMRT_DIV, regValue);
            gyroOdrChanged = true;
            
            INVLOG(LOG_INFO, "acc change rate to preRealRate:%d\n", mTask.sensors[ACC].hwRate);
        }
        // GYRO off
        SPI_WRITE(ICM2060X_PWR_MGMT_2, mTask.pwr_mgmr_config2 | BIT_PWR_GYRO_STBY );
        /*update globle status pwr_mgmr_config2, pwr_mgmr_config2 */
        mTask.pwr_mgmr_config2 |= BIT_PWR_GYRO_STBY ;
      
    }
#if MTK_VERSION_DIFF    
    else if (mTask.sensors[ANYMO].powered == true) {
#else
    else if (mTask.sensors_handle[ANYMO].powered == true) {
#endif
        // GYRO off
        SPI_WRITE(ICM2060X_PWR_MGMT_2, mTask.pwr_mgmr_config2 | BIT_PWR_GYRO_STBY );
        /*update globle status pwr_mgmr_config2, pwr_mgmr_config2 */
        mTask.pwr_mgmr_config2 |= BIT_PWR_GYRO_STBY ;
    }

    registerAccGyroFifoInfo((mTask.sensors[ACC].hwRate == 0) ? 0 : 1024000000000 / mTask.sensors[ACC].hwRate,
        (mTask.sensors[GYR].hwRate == 0) ? 0 : 1024000000000 / mTask.sensors[GYR].hwRate);

    mTask.watermark = ICM2060XCalcuWm();
    mTask.sensors[GYR].powered = false;
    mTask.sensors[GYR].configed = false;

    ICM2060XConfigFifo(gyroOdrChanged);

    if(gyroOdrChanged){
       first_data = 1;
       isrwm_after_set_rate = false;
    }
#if 0
    if (mTask.sensors[ACC].powered == true) {
        /* enable fifo INT */
        mTask.fifo_cfg1 |= 0x2F;    //Terry
        SPI_WRITE(ICM2060X_REG_FIFO_CONFIG_1, mTask.fifo_cfg1);
    }
#endif

    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

static int ICM2060XgyRate(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    int ret = 0;
    int odr = 0;
    uint8_t regValue = 0x00;
    uint32_t sampleRate = 0;
    uint32_t maxRate = 0;
    bool gyroOdrChanged = false;
    struct accGyroCntlPacket cntlPacket;

    ret = rxControlInfo(&cntlPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);
    if (ret < 0) {
        sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "ICM2060XgRate, rx inSize and elemSize error\n");
        return -1;
    }

    mTask.sensors[GYR].rate = cntlPacket.rate;
    mTask.sensors[GYR].latency = cntlPacket.latency;

    if (0 == mTask.sensors[GYR].preRealRate) {   
      mTask.sensors[GYR].needDiscardSample = true;
      mTask.sensors[GYR].samplesToDiscard = SAMPLE_TO_DISCARD;
    }

    /*  get hw sample rate */
    odr = ICM2060XCalcuOdr(&mTask.sensors[GYR].rate, &sampleRate);

    mTask.sensors[GYR].preRealRate = sampleRate;

	
	#if DEBUG_STATUS
	INVLOG(LOG_INFO, "ICM2060XgyRate DUMP: PW0 %x PW1 %x ACC %d %d GY %d %d WOM %d\n",
		mTask.pwr_mgmr_config, mTask.pwr_mgmr_config2,
		mTask.sensors[ACC].powered,mTask.sensors[ACC].configed,
		mTask.sensors[GYR].powered,mTask.sensors[GYR].configed,
		#if MTK_VERSION_DIFF
		mTask.sensors[ANYMO].powered);
		#else
		mTask.sensors_handle[ANYMO].powered);
		#endif
	#endif
	
    /*if gyr configed ,compare maxRate with acc and gyr rate*/
    if (mTask.sensors[ACC].configed) {
        maxRate = max(sampleRate, mTask.sensors[ACC].preRealRate);
        if ((maxRate != mTask.sensors[ACC].hwRate) || (maxRate != mTask.sensors[GYR].hwRate)) {
            mTask.sensors[ACC].hwRate = maxRate;
            mTask.sensors[GYR].hwRate = maxRate;
            /* update new odr */
             odr = ICM2060XCalcuOdr(&maxRate, &sampleRate);
            if(15 == odr)
                regValue = 66;
            else
                regValue = ODR2SMPLRT_DIV(odr);         
           
            SPI_WRITE(ICM2060X_REG_SAMRT_DIV, regValue);
            INVLOG(LOG_INFO, "ICM2060XgyRate: gyro config %d\n",regValue);
            gyroOdrChanged = true;
        } else {
            gyroOdrChanged = false;
        }
    } else {
        if ((sampleRate != mTask.sensors[GYR].hwRate)) {
            mTask.sensors[GYR].hwRate = sampleRate;
            if(15 == odr)
                regValue = 66;
            else
                regValue = ODR2SMPLRT_DIV(odr);
            SPI_WRITE(ICM2060X_REG_SAMRT_DIV, regValue);
            INVLOG(LOG_INFO, "ICM2060XgyRate-S: gyro config %d\n",regValue);
            gyroOdrChanged = true;
        } else {
            gyroOdrChanged = false;
        }
    }

    registerAccGyroFifoInfo((mTask.sensors[ACC].hwRate == 0) ? 0 : 1024000000000 / mTask.sensors[ACC].hwRate,
    (mTask.sensors[GYR].hwRate == 0) ? 0 : 1024000000000 / mTask.sensors[GYR].hwRate);
    mTask.sensors[GYR].configed = true;

    /* watermark update */
    mTask.watermark = ICM2060XCalcuWm();

    mTask.sensors[GYR].configed = true;
    ICM2060XConfigFifo(gyroOdrChanged);

     if(gyroOdrChanged){
       first_data = 1;
       isrwm_after_set_rate = false;
    }

    mt_eint_unmask(mTask.hw->eint_num);
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

#if 1
static int ICM2060XAccCali(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
#ifndef CFG_MTK_CALIBRATION_V1_0   
    float bias[AXES_NUM] = {0};
#endif
    mTask.sensors[ACC].startCali = true;

    INVLOG(LOG_INFO, "ICM2060XAccCali %d\n",mTask.sensors[ACC].startCali);
#ifndef CFG_MTK_CALIBRATION_V1_0    
    Acc_init_calibration(bias);
#else
    accFactoryCalibrateInit(&mTask.accFactoryCal);
#endif

    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int ICM2060XGyroCali(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
#ifndef CFG_MTK_CALIBRATION_V1_0 
    float slope[AXES_NUM] = {0};
    float intercept[AXES_NUM] = {0};
#endif    
    mTask.sensors[GYR].startCali = true;

    INVLOG(LOG_INFO, "ICM2060XGyroCali %d\n",mTask.sensors[GYR].startCali);
#ifndef CFG_MTK_CALIBRATION_V1_0    
    Gyro_init_calibration(slope, intercept);
#else
    gyroFactoryCalibrateInit(&mTask.gyroFactoryCal);
#endif
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}
#endif

static int ICM2060XAccCfgCali(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    int ret = 0;
    struct accGyroCaliCfgPacket caliCfgPacket;

    ret = rxCaliCfgInfo(&caliCfgPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);

    if (ret < 0) {
        sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "accHwCaliCheck, rx inSize and elemSize error\n");
        return -1;
    }
    INVLOG(LOG_INFO, "ICM2060XAccCfgCali: cfgData[0]:%d, cfgData[1]:%d, cfgData[2]:%d\n",
        caliCfgPacket.caliCfgData[0], caliCfgPacket.caliCfgData[1], caliCfgPacket.caliCfgData[2]);

    mTask.sensors[ACC].staticCali[0] = (float)caliCfgPacket.caliCfgData[0] / 1000;
    mTask.sensors[ACC].staticCali[1] = (float)caliCfgPacket.caliCfgData[1] / 1000;
    mTask.sensors[ACC].staticCali[2] = (float)caliCfgPacket.caliCfgData[2] / 1000;

    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int ICM2060XGyroCfgCali(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    int ret = 0;
    struct accGyroCaliCfgPacket caliCfgPacket;

    ret = rxCaliCfgInfo(&caliCfgPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);

    if (ret < 0) {
        sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "gyroHwCaliCheck, rx inSize and elemSize error\n");
        return -1;
    }

    INVLOG(LOG_INFO, "ICM2060XGyroCfgCali: cfgData[0]:%d, cfgData[1]:%d, cfgData[2]:%d\n",
            caliCfgPacket.caliCfgData[0], caliCfgPacket.caliCfgData[1], caliCfgPacket.caliCfgData[2]);

    mTask.sensors[GYR].staticCali[0] = (float)caliCfgPacket.caliCfgData[0] / 1000;
    mTask.sensors[GYR].staticCali[1] = (float)caliCfgPacket.caliCfgData[1] / 1000;
    mTask.sensors[GYR].staticCali[2] = (float)caliCfgPacket.caliCfgData[2] / 1000;

    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static void accGetCalibration(int32_t *cali, int32_t size) {
    cali[AXIS_X]  = mTask.accHwCali[AXIS_X];
    cali[AXIS_Y]  = mTask.accHwCali[AXIS_Y];
    cali[AXIS_Z]  = mTask.accHwCali[AXIS_Z];
    //  INVLOG(LOG_INFO, "accGetCalibration cali x:%d, y:%d, z:%d\n", cali[AXIS_X], cali[AXIS_Y], cali[AXIS_Z]);
}
static void accSetCalibration(int32_t *cali, int32_t size) {
    mTask.accHwCali[AXIS_X] = cali[AXIS_X];
    mTask.accHwCali[AXIS_Y] = cali[AXIS_Y];
    mTask.accHwCali[AXIS_Z] = cali[AXIS_Z];
    INVLOG(LOG_INFO, "accSetCalibration cali x:%d, y:%d, z:%d\n", mTask.accHwCali[AXIS_X],
        mTask.accHwCali[AXIS_Y], mTask.accHwCali[AXIS_Z]);
}
static void accGetData(void *sample) {
    struct TripleAxisDataPoint *tripleSample = (struct TripleAxisDataPoint *)sample;
    tripleSample->ix = mTask.accFactoryData.ix;
    tripleSample->iy = mTask.accFactoryData.iy;
    tripleSample->iz = mTask.accFactoryData.iz;

    INVLOG(LOG_INFO, "accGetData x:%d, y:%d, z:%d\n", tripleSample->ix, tripleSample->iy, tripleSample->iz);
}
static void gyroGetCalibration(int32_t *cali, int32_t size) {
    cali[AXIS_X] = mTask.gyroHwCali[AXIS_X];
    cali[AXIS_Y] = mTask.gyroHwCali[AXIS_Y];
    cali[AXIS_Z] = mTask.gyroHwCali[AXIS_Z];
    //  INVLOG(LOG_INFO, "gyroGetCalibration cali x:%d, y:%d, z:%d\n", cali[AXIS_X], cali[AXIS_Y], cali[AXIS_Z]);
}
static void gyroSetCalibration(int32_t *cali, int32_t size) {
    mTask.gyroHwCali[AXIS_X] = cali[AXIS_X];
    mTask.gyroHwCali[AXIS_Y] = cali[AXIS_Y];
    mTask.gyroHwCali[AXIS_Z] = cali[AXIS_Z];
    INVLOG(LOG_INFO, "gyroSetCalibration cali x:%d, y:%d, z:%d\n", mTask.gyroHwCali[AXIS_X],
        mTask.gyroHwCali[AXIS_Y], mTask.gyroHwCali[AXIS_Z]);
}
static void gyroGetData(void *sample) {
    struct TripleAxisDataPoint *tripleSample = (struct TripleAxisDataPoint *)sample;
    tripleSample->ix = mTask.gyroFactoryData.ix;
    tripleSample->iy = mTask.gyroFactoryData.iy;
    tripleSample->iz = mTask.gyroFactoryData.iz;

    INVLOG(LOG_INFO, "gyroGetData x:%d, y:%d, z:%d\n", tripleSample->ix, tripleSample->iy, tripleSample->iz);
}

static void spiIsrCallBack(void *cookie, int err) {
    if (err != 0) {
        osLog(LOG_ERROR, "icm206xx: spiIsrCallBack err\n");
        sensorFsmEnqueueFakeSpiEvt(mTask.spiCallBack, cookie, ERROR_EVT);
    } else {
        mTask.swSampleTime = rtcGetTime();
        sensorFsmEnqueueFakeSpiEvt(mTask.spiCallBack, cookie, SUCCESS_EVT);
    }
}

static int ICM2060XSample(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    int ret = 0;

    ret = rxTransferDataInfo(&mTask.dataInfo, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);
    if (ret < 0) {
        sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, ERROR_EVT);
        osLog(LOG_ERROR, "ICM2060XSample, rx dataInfo error\n");
        return -1;
    }

    SPI_READ(ICM2060X_REG_FIFO_COUNTH, 2, &mTask.fifo_counter);
    mTask.spiCallBack = spiCallBack;

    return spiBatchTxRx(&mTask.mode, spiIsrCallBack, next_state, __FUNCTION__);
}

static int ICM2060XReadFifo(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    /* when fifo use record mode: one record equal to 14Byte, so fifoDataToRead need multiple by 14 */
    if(mTask.product == ICM20690 || mTask.product == ICM20627)
        mTask.fifoDataToRead = ((mTask.fifo_counter[1] << 8) | mTask.fifo_counter[2]) * FIFO_BYTE_MODE;  
    else
        mTask.fifoDataToRead = (mTask.fifo_counter[1] << 8) | mTask.fifo_counter[2];    
    /* covert record num to bytes */
    //mTask.fifoDataToRead *= 14;//N/A for byte mode
    #if DEBUG_FIFO_DATA
    INVLOG(LOG_INFO, "DataR  %d,H %x, L %x\n", mTask.fifoDataToRead,mTask.fifo_counter[1],mTask.fifo_counter[2]);
    #endif
    if (mTask.fifoDataToRead <= 0 || mTask.fifoDataToRead > ICM2060X_MAX_FIFO_SIZE) {
        sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
        osLog(LOG_ERROR, "ICM2060XReadFifo error mTask.fifoDataToRead = %d\n", mTask.fifoDataToRead);
        return 0;
    }
//    mTask.fifoDataToRead += 1;   //Terry, LiKang, read one more byte to eliminate double interrupt

    SPI_READ(ICM2060X_REG_FIFO_R_W, mTask.fifoDataToRead, &mTask.regBuffer);
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

static void parseRawData(struct accGyroData *data, uint8_t *buf, uint8_t sensorType) {
    int16_t raw_data[AXES_NUM] = {0};
    int16_t remap_data[AXES_NUM] = {0};
    int32_t SwCali[AXES_NUM] = {0};

    int32_t caliResult[AXES_NUM] = {0};
    float temp_data[AXES_NUM] = {0};
    float calibrated_data_output[AXES_NUM] = {0};
    
    int16_t status = 0;

    if (sensorType == SENS_TYPE_ACCEL)
        accGetCalibration(SwCali, 0);
    else if (sensorType == SENS_TYPE_GYRO)
        gyroGetCalibration(SwCali, 0);

    if(sensorType == SENS_TYPE_TEMP) {
        /* Use big endian mode */
         raw_data[AXIS_X] = (buf[1] | buf[0] << 8);
        /* convert to physical unit: Celsius degree */
         mTask.chip_temper = ((float)raw_data[AXIS_X] / TEMP_SENSITIVITY) + ROOM_TEMP_OFFSET;

        return;
    }
    /* Use big endian mode */
    raw_data[AXIS_X] = (buf[1] | buf[0] << 8);
    raw_data[AXIS_Y] = (buf[3] | buf[2] << 8);
    raw_data[AXIS_Z] = (buf[5] | buf[4] << 8);

//    if (sensorType == SENS_TYPE_GYRO) {
//        osLog(LOG_ERROR, "GYRO: %d %d %d\n", raw_data[AXIS_X], raw_data[AXIS_Y], raw_data[AXIS_Z]);
//    }

    raw_data[AXIS_X] = raw_data[AXIS_X] + SwCali[AXIS_X];
    raw_data[AXIS_Y] = raw_data[AXIS_Y] + SwCali[AXIS_Y];
    raw_data[AXIS_Z] = raw_data[AXIS_Z] + SwCali[AXIS_Z];

    remap_data[mTask.cvt.map[AXIS_X]] = mTask.cvt.sign[AXIS_X] * raw_data[AXIS_X];
    remap_data[mTask.cvt.map[AXIS_Y]] = mTask.cvt.sign[AXIS_Y] * raw_data[AXIS_Y];
    remap_data[mTask.cvt.map[AXIS_Z]] = mTask.cvt.sign[AXIS_Z] * raw_data[AXIS_Z];

    if (sensorType == SENS_TYPE_ACCEL) {
        temp_data[AXIS_X] = (float)remap_data[AXIS_X] * KSCALE_ACC_8G_RANGE;
        temp_data[AXIS_Y] = (float)remap_data[AXIS_Y] * KSCALE_ACC_8G_RANGE;
        temp_data[AXIS_Z] = (float)remap_data[AXIS_Z] * KSCALE_ACC_8G_RANGE;

        #if DEBUG_ACC_DATA
        osLog(LOG_ERROR, "A: %f %f %f,cali %d\n", (double)temp_data[AXIS_X], (double)temp_data[AXIS_Y], (double)temp_data[AXIS_Z],mTask.sensors[ACC].startCali);
        #endif
        if (UNLIKELY(mTask.sensors[ACC].startCali)) {
#ifndef CFG_MTK_CALIBRATION_V1_0
            status = Acc_run_factory_calibration_timeout(0,
                temp_data, calibrated_data_output, (int *)&mTask.sensors[ACC].accuracy, rtcGetTime());
            INVLOG(LOG_INFO, "ACC accuracy %d\n",(int *)&mTask.sensors[ACC].accuracy);
            if (status != 0) {
                mTask.sensors[ACC].startCali = false;
                if (status > 0) {
                    INVLOG(LOG_INFO, "ACC cali detect shake\n");
                    caliResult[AXIS_X] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_X] * 1000);
                    caliResult[AXIS_Y] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_Y] * 1000);
                    caliResult[AXIS_Z] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_Z] * 1000);
                    sendDataCalibrationResult(SENS_TYPE_ACCEL, (int32_t *)&caliResult[0], (uint8_t)status);
                } else
                    INVLOG(LOG_INFO, "ACC cali time out\n");
            } else if (mTask.sensors[ACC].accuracy == 3) {
                mTask.sensors[ACC].startCali = false;
                mTask.sensors[ACC].staticCali[AXIS_X] = calibrated_data_output[AXIS_X] - temp_data[AXIS_X];
                mTask.sensors[ACC].staticCali[AXIS_Y] = calibrated_data_output[AXIS_Y] - temp_data[AXIS_Y];
                mTask.sensors[ACC].staticCali[AXIS_Z] = calibrated_data_output[AXIS_Z] - temp_data[AXIS_Z];
                caliResult[AXIS_X] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_X] * 1000);
                caliResult[AXIS_Y] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_Y] * 1000);
                caliResult[AXIS_Z] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_Z] * 1000);
                sendDataCalibrationResult(SENS_TYPE_ACCEL, (int32_t *)&caliResult[0], (uint8_t)status);
                INVLOG(LOG_INFO, "ACCEL cali done:caliResult[0]:%d, caliResult[1]:%d, caliResult[2]:%d,offset[0]:%f, offset[1]:%f, offset[2]:%f\n",
                               caliResult[AXIS_X], caliResult[AXIS_Y], caliResult[AXIS_Z],
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
                INVLOG(LOG_INFO, "accel cali bias: [%f, %f, %f]\n",
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

        //Need "m/s^2 * 1000" for factory calibration program
        //Need LSB in cali program = mTask.accFactoryData.ix * mInfo.sensitivity / mInfo.gain;
        mTask.accFactoryData.ix = (int32_t)(data->x * ACCELEROMETER_INCREASE_NUM_AP/*1000*/);
        mTask.accFactoryData.iy = (int32_t)(data->y * ACCELEROMETER_INCREASE_NUM_AP);
        mTask.accFactoryData.iz = (int32_t)(data->z * ACCELEROMETER_INCREASE_NUM_AP);

    } else if (sensorType == SENS_TYPE_GYRO) {
       
        temp_data[AXIS_X] = (float)remap_data[AXIS_X] * KSCALE_GYRO_2000_RANGE;
        temp_data[AXIS_Y] = (float)remap_data[AXIS_Y] * KSCALE_GYRO_2000_RANGE;
        temp_data[AXIS_Z] = (float)remap_data[AXIS_Z] * KSCALE_GYRO_2000_RANGE;
        
        #if DEBUG_GYRO_DATA
        osLog(LOG_ERROR, "GY: %f %f %f cali %d\n", (double)temp_data[AXIS_X], (double)temp_data[AXIS_Y], (double)temp_data[AXIS_Z],mTask.sensors[GYR].startCali);
        #endif
        if (UNLIKELY(mTask.sensors[GYR].startCali)) {
#ifndef CFG_MTK_CALIBRATION_V1_0
            status = Gyro_run_factory_calibration_timeout(0,
                temp_data, calibrated_data_output, (int *)&mTask.sensors[GYR].accuracy, 0, rtcGetTime());
            INVLOG(LOG_INFO, "ACC gyro %d,status %d\n",(int *)&mTask.sensors[GYR].accuracy,status);
            if (status != 0) {
                mTask.sensors[GYR].startCali = false;
                if (status > 0) {
                    INVLOG(LOG_INFO, "GYRO cali detect shake\n");
                    caliResult[AXIS_X] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_X] * 1000);
                    caliResult[AXIS_Y] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_Y] * 1000);
                    caliResult[AXIS_Z] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_Z] * 1000);
                    sendDataCalibrationResult(SENS_TYPE_GYRO, (int32_t *)&caliResult[0], (uint8_t)status);
                } else
                    INVLOG(LOG_INFO, "GYRO cali time out\n");
            } else if (mTask.sensors[GYR].accuracy == 3) {
                mTask.sensors[GYR].startCali = false;
                mTask.sensors[GYR].staticCali[AXIS_X] = calibrated_data_output[AXIS_X] - temp_data[AXIS_X];
                mTask.sensors[GYR].staticCali[AXIS_Y] = calibrated_data_output[AXIS_Y] - temp_data[AXIS_Y];
                mTask.sensors[GYR].staticCali[AXIS_Z] = calibrated_data_output[AXIS_Z] - temp_data[AXIS_Z];
                caliResult[AXIS_X] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_X] * 1000);
                caliResult[AXIS_Y] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_Y] * 1000);
                caliResult[AXIS_Z] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_Z] * 1000);
                sendDataCalibrationResult(SENS_TYPE_GYRO, (int32_t *)&caliResult[0], (uint8_t)status);
                INVLOG(LOG_INFO, "GYRO cali done: caliResult[0]:%d, caliResult[1]:%d, caliResult[2]:%d, offset[0]:%f, offset[1]:%f, offset[2]:%f\n",
                                       caliResult[AXIS_X], caliResult[AXIS_Y], caliResult[AXIS_Z],
                                  (double)mTask.sensors[GYR].staticCali[AXIS_X],
                                  (double)mTask.sensors[GYR].staticCali[AXIS_Y],
                                  (double)mTask.sensors[GYR].staticCali[AXIS_Z]); 
            }
#else
            status = gyroFactoryCalibrateRun(&mTask.gyroFactoryCal,
                temp_data[AXIS_X], temp_data[AXIS_Y], temp_data[AXIS_Z], rtcGetTime());
            if (status == UNSTABLE_DETECT) {
                mTask.sensors[GYR].startCali = false;
                caliResult[AXIS_X] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_X] * 1000);
                caliResult[AXIS_Y] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_Y] * 1000);
                caliResult[AXIS_Z] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_Z] * 1000);
                accGyroSendCalibrationResult(SENS_TYPE_GYRO, (int32_t *)&caliResult[0], (uint8_t)status);
            } else if (status == STILL_DETECT) {
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
                INVLOG(LOG_INFO, "gyro cali bias: [%f, %f, %f]\n",
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
        //Need "dps*GYROSCOPE_INCREASE_NUM_AP" for factory calibration program
        //Need LSB in cali program = mTask.accFactoryData.ix * mInfo.sensitivity / mInfo.gain;
        mTask.gyroFactoryData.ix =
            (int32_t)((float)data->x * GYROSCOPE_INCREASE_NUM_AP/*131*1000*/ / DEGREE_TO_RADIRAN_SCALAR/*pi/180*/);
        mTask.gyroFactoryData.iy =
            (int32_t)((float)data->y * GYROSCOPE_INCREASE_NUM_AP / DEGREE_TO_RADIRAN_SCALAR);
        mTask.gyroFactoryData.iz =
            (int32_t)((float)data->z * GYROSCOPE_INCREASE_NUM_AP / DEGREE_TO_RADIRAN_SCALAR);
    }

    if (mTask.debug_trace) {
        switch (sensorType) {
            case SENS_TYPE_ACCEL:
                osLog(LOG_ERROR, "ACCEL:raw_data_x=%f, raw_data_y=%f, raw_data_z=%f\n",
                    (double)data->x, (double)data->y, (double)data->z);
                break;
            case SENS_TYPE_GYRO:
                osLog(LOG_ERROR, "GYRO:raw_data_x=%f, raw_data_y=%f, raw_data_z=%f\n",
                    (double)data->x, (double)data->y, (double)data->z);
                break;
            default:
                break;
        }
    }
}

static void spi_report_fiforeamining_data_CallBack(void *cookie, int err) {

    struct accGyroData *data = mTask.accGyroPacket.outBuf;
	uint16_t wm_count;
    
        /* when fifo use record mode: one record equal to 14Byte, so fifoDataToRead need multiple by 14 */
    if(mTask.product == ICM20690 || mTask.product == ICM20627)
        mTask.fifoDataToRead = ((mTask.fifo_counter[1] << 8) | mTask.fifo_counter[2]) * FIFO_BYTE_MODE;  
    else
        mTask.fifoDataToRead = (mTask.fifo_counter[1] << 8) | mTask.fifo_counter[2];    
 
    INVLOG(LOG_INFO, "Reread fifo in spi callback data %d,H %x, L %x\n", mTask.fifoDataToRead,mTask.fifo_counter[1],mTask.fifo_counter[2]);
    
    if (mTask.fifoDataToRead < 0 || mTask.fifoDataToRead > ICM2060X_MAX_FIFO_SIZE) {
        sensorFsmEnqueueFakeSpiEvt(mTask.spiCallBack, cookie, SUCCESS_EVT);
        osLog(LOG_ERROR, "ICM2060XReadFifo error mTask.fifoDataToRead = %d\n", mTask.fifoDataToRead);
    }

    if (cookie == NULL)
          INVLOG(LOG_INFO, "cookie nuLL \n");

    if (mTask.spiCallBack == NULL)
          INVLOG(LOG_INFO, "mTask.spiCallBack null\n");

	if(mTask.product == ICM20690 || mTask.product == ICM20627)
		wm_count = mTask.watermark * FIFO_BYTE_MODE; //Record mode
	else
		wm_count = mTask.watermark;
	
    if ( mTask.fifoDataToRead >= wm_count && mTask.fifo_reRead_count < 5) {
        INVLOG(LOG_INFO, "need read fifo data again in spicall back, fifo toRead %d, WM %d,reread times %d \n", mTask.fifoDataToRead,wm_count,mTask.fifo_reRead_count);
        SPI_READ(ICM2060X_REG_FIFO_R_W, mTask.fifoDataToRead, &mTask.regBuffer);
        mTask.fifo_reRead_count ++;
        spiBatchTxRx(&mTask.mode, spi_handle_fiforeaminingdata_CallBack, cookie, __FUNCTION__);

        }
    else if(mTask.fifoDataToRead < wm_count){
        INVLOG(LOG_INFO, "report fifo data  in spicall back, fifo toRead %d, WM %d\n", mTask.fifoDataToRead,wm_count);
        /* if fifo data read counter is smaller than water mark,then exit this handle_irq operation, report all data */
        txTransferDataInfo(&mTask.dataInfo, mTask.acc_report_size, mTask.gyro_report_size, mTask.realtime_report_ts, data, mTask.chip_temper);
        INVLOG(LOG_INFO, "hwT totol %llu SwT %llu rT %llu,accS %d,gyroS %d ", mTask.hwSampleTime,mTask.swSampleTime,mTask.realtime_report_ts,mTask.acc_report_size,mTask.gyro_report_size);
        //clear all flag
        mTask.fifo_reRead_count = 0;
        mTask.acc_report_size = 0;
        mTask.gyro_report_size = 0;
        mTask.realtime_report_ts = 0;
	    //update hwsample time & last sample time 
        mTask.hwSampleTime = mTask.realtime_report_ts;
        mTask.last_hwSampleTime = mTask.realtime_report_ts;
        mTask.lastSampleTime = mTask.realtime_report_ts;
        
        //accGyro CHIP_HW_INT_STATUS_CHECK pending by nowState:0
        mt_eint_unmask(mTask.hw->eint_num);

        #if DEBUG_WM_INTERUPT
        // turn on WM interrupt;
        ICM2060XEnable_WM();
        #endif
        mTask.unmask_eint_time = rtcGetTime();
        sensorFsmEnqueueFakeSpiEvt(mTask.spiCallBack, cookie, SUCCESS_EVT);

    }else{
        ICM2060XFIFO_Restart();
        /* if fifo data read counter is smaller than water mark,then exit this handle_irq operation, report all data */
        txTransferDataInfo(&mTask.dataInfo, mTask.acc_report_size, mTask.gyro_report_size, mTask.realtime_report_ts, data, mTask.chip_temper);
        INVLOG(LOG_INFO, "flush fifo , hwT totol %llu SwT %llu rT %llu,accS %d,gyroS %d ", mTask.hwSampleTime,mTask.swSampleTime,mTask.realtime_report_ts,mTask.acc_report_size,mTask.gyro_report_size);
        //clear all flag
        mTask.fifo_reRead_count = 0;
        mTask.acc_report_size = 0;
        mTask.gyro_report_size = 0;
        mTask.realtime_report_ts = 0;
          //update hwsample time & last sample time 
        mTask.hwSampleTime = mTask.realtime_report_ts;
        mTask.last_hwSampleTime = mTask.realtime_report_ts;
        mTask.lastSampleTime = mTask.realtime_report_ts;
	
        //accGyro CHIP_HW_INT_STATUS_CHECK pending by nowState:0
        mt_eint_unmask(mTask.hw->eint_num);
        mTask.unmask_eint_time = rtcGetTime();

        #if DEBUG_WM_INTERUPT
         // turn on WM interrupt;
        ICM2060XEnable_WM();
        #endif
        
        spiBatchTxRx(&mTask.mode, mTask.spiCallBack, cookie, __FUNCTION__);
        osLog(LOG_ERROR, "handle too slow fifo data read condition,only flush \n");
    }
}


static void spi_handle_fiforeaminingdata_CallBack(void *cookie, int err) {

    struct accGyroData *data = mTask.accGyroPacket.outBuf;
    uint32_t i = 0;
    uint8_t *fifo_data = &mTask.regBuffer[1];
    uint32_t hw_delay;
    uint32_t hw_Rate;

    INVLOG(LOG_INFO, "spi_handle_fiforeaminingdata_CallBack\n");
    if(mTask.sensors[ACC].configed){
        hw_Rate = mTask.sensors[ACC].hwRate;
    }
    else if(mTask.sensors[GYR].configed){
        hw_Rate = mTask.sensors[GYR].hwRate;
    }
    else{
        osLog(LOG_ERROR,"error condition,acc & gyro all off\n");
        hw_Rate = 1;
    }  
    hw_delay = 1024000000000ULL /hw_Rate ;

    for (i=0; i < mTask.fifoDataToRead; i+=FIFO_BYTE_MODE) {
        if ((mTask.acc_report_size + mTask.gyro_report_size) < MAX_RECV_PACKET) {
            /* 1st update realSampleTime   */
            mTask.realtime_report_ts += (uint64_t)hw_delay;
            INVLOG(LOG_INFO, "rT_ts %llu,hw_d %d \n", mTask.realtime_report_ts,hw_delay);
            /* 2st add flush data into report data package */
            if (mTask.sensors[ACC].configed && mTask.sensors[ACC].powered) {
                parseRawData(&data[mTask.acc_report_size + mTask.gyro_report_size], &fifo_data[i], SENS_TYPE_ACCEL);
                mTask.acc_report_size++;
            }
       
            if (mTask.sensors[GYR].configed && mTask.sensors[GYR].powered) {
                parseRawData(&data[mTask.acc_report_size + mTask.gyro_report_size], &fifo_data[i+8], SENS_TYPE_GYRO);
                mTask.gyro_report_size++;
                }
        } else
            osLog(LOG_ERROR, "outBuf full, accEventSize = %d, gyroEventSize = %d\n", mTask.acc_report_size,  mTask.gyro_report_size);
    }

    // check fifo counter again 
    SPI_READ(ICM2060X_REG_FIFO_COUNTH, 2, &mTask.fifo_counter);
    spiBatchTxRx(&mTask.mode, spi_report_fiforeamining_data_CallBack, cookie, __FUNCTION__);
  
}

static int ICM2060X_RereadFifo(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    struct accGyroData *data = mTask.accGyroPacket.outBuf;    
    mTask.spiCallBack = spiCallBack;

        /* when fifo use record mode: one record equal to 14Byte, so fifoDataToRead need multiple by 14 */
    if(mTask.product == ICM20690 || mTask.product == ICM20627)
        mTask.fifoDataToRead = ((mTask.fifo_counter[1] << 8) | mTask.fifo_counter[2]) * FIFO_BYTE_MODE;  
    else
        mTask.fifoDataToRead = (mTask.fifo_counter[1] << 8) | mTask.fifo_counter[2];    
 
    #if DEBUG_FIFO_DATA
    INVLOG(LOG_INFO, "Reread fifo data %d,H %x, L %x\n", mTask.fifoDataToRead,mTask.fifo_counter[1],mTask.fifo_counter[2]);
    #endif
    
    if (mTask.fifoDataToRead < 0 || mTask.fifoDataToRead > ICM2060X_MAX_FIFO_SIZE) {
        sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
        osLog(LOG_ERROR, "ICM2060XRe-readFifo error mTask.fifoDataToRead = %d\n", mTask.fifoDataToRead);
        return 0;
    }

    if ( mTask.fifoDataToRead >= (mTask.watermark * FIFO_BYTE_MODE) && mTask.watermark != 0) {
        INVLOG(LOG_INFO, "need read fifo data again, fifo toRead %d, WM %d \n", mTask.fifoDataToRead,mTask.watermark);
        SPI_READ(ICM2060X_REG_FIFO_R_W, mTask.fifoDataToRead, &mTask.regBuffer);
        return spiBatchTxRx(&mTask.mode, spi_handle_fiforeaminingdata_CallBack, next_state, __FUNCTION__);
    }else {

    /* if fifo data read counter is smaller than water mark,then exit this handle_irq operation, report all data */
        txTransferDataInfo(&mTask.dataInfo, mTask.acc_report_size, mTask.gyro_report_size, mTask.realtime_report_ts, data, mTask.chip_temper);
        #if DEBUG_WM_INTERUPT
        // turn on WM interrupt;
        ICM2060XEnable_WM();
        #endif 
        
        //accGyro CHIP_HW_INT_STATUS_CHECK pending by nowState:0
        mt_eint_unmask(mTask.hw->eint_num);
        mTask.unmask_eint_time = rtcGetTime();
        sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
        return 0;
    }
}

static int ICM2060XConvert(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    struct accGyroData *data = mTask.accGyroPacket.outBuf;
    uint32_t i = 0;
    uint8_t accEventSize = 0;
    uint8_t gyroEventSize = 0;
    uint8_t accEventSize_Discard = 0;
    uint8_t gyroEventSize_Discard = 0;
    uint64_t swSampleTime = 0, realSampleTime = 0;
    uint8_t *fifo_data = &mTask.regBuffer[1];
    //uint32_t max_recv_packet = 60;   //Terry no need, sample 
    uint32_t minDelay = 0;
    uint64_t long_diff = 0;
    uint8_t long_EventSize = 0;
    uint64_t temp_diff = 0;


    for (i=0; i < mTask.fifoDataToRead; i+=FIFO_BYTE_MODE) {
        if ((accEventSize + gyroEventSize) < MAX_RECV_PACKET) {
            if (mTask.sensors[ACC].configed && mTask.sensors[ACC].powered) {
                if (mTask.sensors[ACC].samplesToDiscard) {
                    mTask.sensors[ACC].samplesToDiscard--;
                accEventSize_Discard++;
                } else {
                    parseRawData(&data[accEventSize+gyroEventSize], &fifo_data[i], SENS_TYPE_ACCEL);
                    accEventSize++;
                }
            }
            if (mTask.sensors[GYR].configed || mTask.sensors[ACC].configed) {
            // prase temperature data
                parseRawData(&data[accEventSize+gyroEventSize], &fifo_data[i+6], SENS_TYPE_TEMP);
            }
            if (mTask.sensors[GYR].configed && mTask.sensors[GYR].powered) {
                if (mTask.sensors[GYR].samplesToDiscard) {
                    mTask.sensors[GYR].samplesToDiscard--;
            gyroEventSize_Discard++;
                } else {
                    parseRawData(&data[accEventSize+gyroEventSize], &fifo_data[i+8], SENS_TYPE_GYRO);
                    gyroEventSize++;
                }
            }
        } else
            osLog(LOG_ERROR, "outBuf full, accEventSize = %d, gyroEventSize = %d\n", accEventSize, gyroEventSize);
    }

#ifndef CFG_MTK_CALIBRATION_V1_0    
    /*if factory true , can't send to runtime cali in parseRawData in accGyro*/
    if (mTask.sensors[ACC].startCali || mTask.sensors[GYR].startCali) {
        accEventSize = 0;
        gyroEventSize = 0;
        registerAccGyroFifoInfo((mTask.sensors[ACC].hwRate == 0) ? 0 : 1024000000000 / mTask.sensors[ACC].hwRate,
                                (mTask.sensors[GYR].hwRate == 0) ? 0 : 1024000000000 / mTask.sensors[GYR].hwRate);
    }
#endif
    //INVLOG(LOG_INFO, "ICM2060XConvert, fifoDataToRead:%d, accEventSize:%d, gyroEventSize:%d\n",
        //mTask.fifoDataToRead, accEventSize, gyroEventSize);

    swSampleTime = mTask.swSampleTime;

/* Pre Process lastSampleTime and hwSampleTime */
    calc_long_diff(
        mTask.sensors[ACC].hwRate, mTask.sensors[ACC].configed, accEventSize + accEventSize_Discard,
        mTask.sensors[GYR].hwRate, mTask.sensors[GYR].configed, gyroEventSize + gyroEventSize_Discard,
        &minDelay, &long_diff, &long_EventSize);

    // if first_data, lastSampleTime cannot use in below Fake time caculate, so assin value to handle it
    if (first_data == 1) {
        mTask.lastSampleTime = swSampleTime - long_diff;
        #if DEBUG_FIFO_DATA
        INVLOG(LOG_INFO, "lastST %llu,longdiff %llu\n",mTask.lastSampleTime,long_diff);
        #endif
        first_data = 0;
    }

    /* N/A for interrupt pulse mode, only work in LEVEL trigger */
#ifndef ICM_EDGE_SENSITIVE 
    // check if hw latch time can trust
    // we think when mTask.hwSampleTime == mTask.last_hwSampleTime, it is flush operation because of hwSampleTime is not updated

    if(mTask.hwSampleTime != mTask.last_hwSampleTime ){
        if (mTask.hwSampleTime > mTask.unmask_eint_time)
            temp_diff = mTask.hwSampleTime - mTask.unmask_eint_time;
        else
            temp_diff = mTask.unmask_eint_time - mTask.hwSampleTime;
        if (temp_diff < 200000)   // 200us
            mTask.hwSampleTime = mTask.lastSampleTime +long_diff;
    }
#endif
 /* End Pre Process lastSampleTime and hwSampleTime */
    
    if(mTask.hwSampleTime != mTask.last_hwSampleTime ){
        realSampleTime = calcFakeInterruptTime(swSampleTime, mTask.hwSampleTime, mTask.lastSampleTime,
        mTask.sensors[ACC].hwRate, mTask.sensors[ACC].configed, accEventSize + accEventSize_Discard,
        mTask.sensors[GYR].hwRate, mTask.sensors[GYR].configed, gyroEventSize + gyroEventSize_Discard);
    }else{
        // if hwSampleTime equal to last_hwSampleTime time, it is flush condition . it does not need to calculate the Fake Interrupt time(because hwsample is not updated because of no current Interrupt comming)
        if(isrwm_after_set_rate){ 
            realSampleTime = mTask.lastSampleTime + long_diff;
            #if DEBUG_FIFO_DATA
            INVLOG(LOG_INFO, "hwT %llu,lastST %llu,longdiff %llu\n",mTask.hwSampleTime,mTask.lastSampleTime,long_diff);
            #endif
        }else{
            //if there is no interrupt happen after set rate, we use SW time directly.
            realSampleTime = swSampleTime;
            #if DEBUG_FIFO_DATA
            INVLOG(LOG_INFO, "use swT %llu no isr before\n",swSampleTime);
            #endif

        }
    }
    //INVLOG(LOG_INFO, "ICM2060XConvert, swSampleTime=%llu, hwSampleTime=%llu, realSampleTime=%llu, lastSampleTime=%llu, now=%llu, unmask_eint_time=%llu\n",
    //swSampleTime, mTask.hwSampleTime, realSampleTime, mTask.lastSampleTime, rtcGetTime(), mTask.unmask_eint_time);
#if DEBUG_FIFO_DATA
    INVLOG(LOG_INFO, "hwT %llu SwT %llu rT %llu,accS %d,gyroS %d,Data RByt %d\n", mTask.hwSampleTime,swSampleTime,realSampleTime,accEventSize,gyroEventSize,mTask.fifoDataToRead);
#endif
#if DEBUG_TEMP_DATA
    INVLOG(LOG_INFO, "Temp1: %f \n",(double)mTask.chip_temper);
#endif
    mTask.hwSampleTime = realSampleTime;
    mTask.last_hwSampleTime = realSampleTime;
    mTask.lastSampleTime = realSampleTime;

    mTask.acc_report_size = accEventSize;
    mTask.gyro_report_size = gyroEventSize;
    mTask.realtime_report_ts = realSampleTime;

    //after handle the big buffer fifo , check fifo counter again 
    SPI_READ(ICM2060X_REG_FIFO_COUNTH, 2, &mTask.fifo_counter);
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
  
}

static void ICM2060XIsr1(int arg) {
    if (mTask.latch_time_id < 0) {
        mTask.hwSampleTime = rtcGetTime();
        //osLog(LOG_ERROR, "ICM2060XIsr1, mTask.latch_time_id < 0, mTask.hwSampleTime=%llu\n", mTask.hwSampleTime);
    } else {
        mTask.hwSampleTime = get_latch_time_timestamp(mTask.latch_time_id);
 #if DEBUG_PHASE
    //INVLOG(LOG_ERROR, "Isr1 hwST %llu, rtc %llu ,lastST %llu\n", mTask.hwSampleTime, rtcGetTime(),mTask.lastSampleTime);
 #endif
    }
    accGyroHwIntCheckStatus();
}

static int ICM2060XIntStatusCheck(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    SPI_READ(ICM2060X_REG_FIFO_WM_STATUS, 1, &mTask.statusBuffer);
    SPI_READ(ICM2060X_REG_INT_STATUS, 1, &mTask.int_statusBuffer);
    SPI_READ(ICM2060X_REG_FIFO_COUNTH, 2, &mTask.fifo_overflow_counter);
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

static int ICM2060XIntHandling(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {

    uint8_t wm_status = mTask.statusBuffer[1];
    uint8_t int_status = mTask.int_statusBuffer[1];

#if DEBUG_FIFO_DATA
    INVLOG(LOG_INFO, "ICM Int WM 0x%x int_s 0x%x\n", wm_status, int_status);
#endif

#if SUPPORT_ANYMO
    union EmbeddedDataPoint trigger_axies;
#if MTK_VERSION_DIFF
    if (mTask.sensors[ANYMO].powered) {
#else
    if (mTask.sensors_handle[ANYMO].powered) {
#endif
       if (int_status & (BIT_WOM_X_INT | BIT_WOM_Y_INT | BIT_WOM_Z_INT)) {
           trigger_axies.idata = (int_status & (BIT_WOM_X_INT | BIT_WOM_Y_INT | BIT_WOM_Z_INT));
           INVLOG(LOG_INFO, "Detected any motion\n");
           osEnqueueEvt(EVT_SENSOR_ANY_MOTION, trigger_axies.vptr, NULL);
       }
    }
#endif
    if (int_status & BIT_FIFO_OFLOW_INT) {
        // reset fifo
        INVLOG(LOG_INFO, "fifo over flow\n");
        ICM2060XFIFO_Restart_Read();
        mt_eint_unmask(mTask.hw->eint_num);
        return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    }else if (wm_status & BIT_FIFO_WM_INT) {

        #if DEBUG_WM_INTERUPT
        // turn off WM interrupt when going to fifo WM handle
        SPI_WRITE(ICM2060X_REG_FIFO_WM_TH2 ,0x00);
        if(mTask.product != ICM20690 && mTask.product != ICM20627)   
            SPI_WRITE(ICM2060X_REG_FIFO_WM_TH1, 0x00);
        #endif   
#if DEBUG_FIFO_DATA
       INVLOG(LOG_INFO, "Detected fifo watermark \n");
#endif 
       /*when no app require data for acc & gyro, but happen WM INT, there seems something conflict logic when happen */
       if (!mTask.sensors[ACC].configed && !mTask.sensors[GYR].configed) {
           INVLOG(LOG_INFO, "Unexpected FIFO WM INTR fired\n");
           // reset fifo
           ICM2060XFIFO_ResetOff();
           #if DEBUG_WM_INTERUPT
           // turn on WM interrupt;
           ICM2060XEnable_WM();
           #endif
           mt_eint_unmask(mTask.hw->eint_num);
           return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
       } else {
           isrwm_after_set_rate = true;
           accGyroInterruptOccur();
       }
#if DEBUG_WM_INTERUPT
       return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
#endif
       
    }  else{
           //other unkonw interrupt
           INVLOG(LOG_INFO, "force unmask interrupt\n");
           mt_eint_unmask(mTask.hw->eint_num);
    }
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);

    return 0;
}

static int ICM2060XEintRegistration(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
#ifdef ICM_EDGE_SENSITIVE  
    mt_eint_dis_hw_debounce(mTask.hw->eint_num);
    mt_eint_registration(mTask.hw->eint_num, EDGE_SENSITIVE, HIGH_LEVEL_TRIGGER, ICM2060XIsr1, EINT_INT_UNMASK,
        EINT_INT_AUTO_UNMASK_OFF);
#else
    mt_eint_dis_hw_debounce(mTask.hw->eint_num);
    mt_eint_registration(mTask.hw->eint_num, LEVEL_SENSITIVE, HIGH_LEVEL_TRIGGER, ICM2060XIsr1, EINT_INT_UNMASK,
        EINT_INT_AUTO_UNMASK_OFF);
#endif
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}

#if SUPPORT_ANYMO
#if !MTK_VERSION_DIFF
static uint32_t SensorHasRegisterReturnHandle(uint32_t sensorType) {
    uint32_t Handle = 0;

    sensorFind(sensorType, 0, &Handle);
    return Handle;
}

static bool SensorHasRegisterCheck(uint32_t sensorType) {
    uint32_t Handle;

    if (sensorFind(sensorType, 0, &Handle) == NULL)
        return false;
    else
        return true;
}
#endif
#endif
static void ICM2060XSetDebugTrace(int32_t trace) {
    mTask.debug_trace = trace;
    osLog(LOG_ERROR, "%s ==> trace:%d\n", __func__, mTask.debug_trace);
}

#if SUPPORT_ANYMO
static int anyMotionPowerOn(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
	INVLOG(LOG_INFO, "Enter anyMotionPowerOn\n");
	#if DEBUG_STATUS
	INVLOG(LOG_INFO, "anyMotionPowerOn DUMP: PW0 %x PW1 %x ACC %d %d GY %d %d WOM %d\n",
		mTask.pwr_mgmr_config, mTask.pwr_mgmr_config2,
		mTask.sensors[ACC].powered,mTask.sensors[ACC].configed,
		mTask.sensors[GYR].powered,mTask.sensors[GYR].configed,
		#if MTK_VERSION_DIFF
		mTask.sensors[ANYMO].powered);
		#else
		mTask.sensors_handle[ANYMO].powered);
		#endif
	#endif
	
    if(false == mTask.sensors[ACC].powered && false == mTask.sensors[GYR].powered){
        INVLOG(LOG_INFO, "anyMotionPowerOn: Anymotion go out sleep\n");
        mTask.pwr_mgmr_config &= ~ICM2060X_SLEEP;
        SPI_WRITE(ICM2060X_PWR_MGMT_1, mTask.pwr_mgmr_config,5000); 
        // if acc & gyro are all off, set sample rate to 10hz , otherwise do no change sample dive
        SPI_WRITE(ICM2060X_REG_SAMRT_DIV, 99);
    }
 
    if(false == mTask.sensors[ACC].powered){
        // enable acc 
        mTask.pwr_mgmr_config2 &= ~BIT_PWR_ACCEL_STBY;
        SPI_WRITE(ICM2060X_PWR_MGMT_2, mTask.pwr_mgmr_config2,20000); //set 20ms for accel start-up time, note:at least 200us here to sync fly changes  
    }

    /*config wom mode to enable */
    mTask.smd_cfg_reg69 |= (BIT_ACC_INTEL_EN | BIT_ACC_INTEL_MODE | BIT_WOM_INT_MODE);
    SPI_WRITE(ICM2060X_REG_ACCEL_INTEL_CTRL,mTask.smd_cfg_reg69);

    /*config WOM threshold */
    SPI_WRITE(ICM2060X_REG_WOM_THR_X,DEF_WOM_THRESHOLD);
    SPI_WRITE(ICM2060X_REG_WOM_THR_Y,DEF_WOM_THRESHOLD);
    SPI_WRITE(ICM2060X_REG_WOM_THR_Z,DEF_WOM_THRESHOLD);
    SPI_WRITE(ICM2060X_REG_WOM_THR,DEF_WOM_THRESHOLD);

    /* enable interrupt of wom  */
    mTask.int_src0 |= BIT_WOM_EN;
    SPI_WRITE(ICM2060X_REG_INT_ENABLE, mTask.int_src0);

#if MTK_VERSION_DIFF
    mTask.sensors[ANYMO].powered = true;
#else
    mTask.sensors_handle[ANYMO].powered = true;
#endif

    mt_eint_unmask(mTask.hw->eint_num);

    return  spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

static int anyMotionPowerOff(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                             void *inBuf, uint8_t inSize, uint8_t elemInSize,
                             void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
	INVLOG(LOG_INFO, "Enter anyMotionPowerOff\n");
	#if DEBUG_STATUS
	INVLOG(LOG_INFO, "anyMotionPowerOff DUMP: PW0 %x PW1 %x ACC %d %d GY %d %d WOM %d\n",
		mTask.pwr_mgmr_config, mTask.pwr_mgmr_config2,
		mTask.sensors[ACC].powered,mTask.sensors[ACC].configed,
		mTask.sensors[GYR].powered,mTask.sensors[GYR].configed,
		#if MTK_VERSION_DIFF
		mTask.sensors[ANYMO].powered);
		#else
		mTask.sensors_handle[ANYMO].powered);
		#endif
	#endif
	
#if MTK_VERSION_DIFF  
    mTask.sensors[ANYMO].powered = false;
    mTask.sensors[ANYMO].configed = false; 
#else
    mTask.sensors_handle[ANYMO].powered = false;
    mTask.sensors_handle[ANYMO].configed = false; 
#endif
    
    /*config wom mode to disable */
    mTask.smd_cfg_reg69 &= ~(BIT_ACC_INTEL_EN | BIT_ACC_INTEL_MODE | BIT_WOM_INT_MODE);
    SPI_WRITE(ICM2060X_REG_ACCEL_INTEL_CTRL,mTask.smd_cfg_reg69);

    /* disable interrupt of wom  */
    mTask.int_src0 &= ~ BIT_WOM_EN;
    SPI_WRITE(ICM2060X_REG_INT_ENABLE, mTask.int_src0);

     if((false == mTask.sensors[ACC].configed) && (false == mTask.sensors[ACC].powered)){
        // disable acc 
        mTask.pwr_mgmr_config2 |= BIT_PWR_ACCEL_STBY;
        SPI_WRITE(ICM2060X_PWR_MGMT_2, mTask.pwr_mgmr_config2); //set 20ms for accel start-up time, note:at least 200us here to sync fly changes  
        INVLOG(LOG_INFO, "change ICM2060X_PWR_MGMT_2 in anyMotionPowerOff %x\n",mTask.pwr_mgmr_config2);
    }

    if ((mTask.sensors[ACC].powered == false) &&
 #if MTK_VERSION_DIFF   
        (mTask.sensors[ANYMO].powered == false)&&
 #else
        (mTask.sensors_handle[ANYMO].powered == false)&&
 #endif
        (mTask.sensors[GYR].powered == false)
        ){
		 INVLOG(LOG_INFO, "mask intr and switch to sleep in anyMotionPowerOff \n");
         mt_eint_mask(mTask.hw->eint_num);
         /*  sleep mode in idle */
       
         SPI_WRITE(ICM2060X_PWR_MGMT_1, mTask.pwr_mgmr_config | ICM2060X_SLEEP);
         mTask.pwr_mgmr_config  |= ICM2060X_SLEEP;
    }
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

#if !MTK_VERSION_DIFF
static bool anyMotionPower(bool on, void *cookie) {
    INVLOG(LOG_INFO, "anyMotionPower: Power on, on=%d\n", on);
    accGyroanyMotionPower(on);
    sensorSignalInternalEvt(mTask.sensors_handle[ANYMO].handle, SENSOR_INTERNAL_EVT_POWER_STATE_CHG, on, 0);

    return true;
}

static bool anyMotionFirmwareUpload(void *cookie) {
    sensorSignalInternalEvt(mTask.sensors_handle[ANYMO].handle, SENSOR_INTERNAL_EVT_FW_STATE_CHG, 1, 0);
    return true;
}

static bool anyMotionSetRate(uint32_t rate, uint64_t latency, void *cookie) {
    mTask.sensors_handle[ANYMO].rate = rate;
    mTask.sensors_handle[ANYMO].latency = latency;
    mTask.sensors_handle[ANYMO].configed = true;

    sensorSignalInternalEvt(mTask.sensors_handle[ANYMO].handle, SENSOR_INTERNAL_EVT_RATE_CHG, rate, latency);
    return true;
}

static bool anyMotionFlush(void *cookie) {
    return osEnqueueEvt(EVT_SENSOR_ANY_MOTION, SENSOR_DATA_EVENT_FLUSH, NULL);
}

#define DEC_OPS(power, firmware, rate, flush) \
    .sensorPower = power, \
    .sensorFirmwareUpload = firmware, \
    .sensorSetRate = rate, \
    .sensorFlush = flush

#define DEC_INFO(name, type, axis, inter, samples) \
    .sensorName = name, \
    .sensorType = type, \
    .numAxis = axis, \
    .interrupt = inter, \
    .minSamples = samples

static const struct SensorInfo mSensorInfo_handle[NUM_OF_HANDLE] = {
    { DEC_INFO("ICM2060X Any Motion", SENS_TYPE_ANY_MOTION, NUM_AXIS_EMBEDDED, NANOHUB_INT_NONWAKEUP, 20) },
};

static const struct SensorOps mSensorOps[NUM_OF_HANDLE] = {
    { DEC_OPS(anyMotionPower, anyMotionFirmwareUpload, anyMotionSetRate, anyMotionFlush) },
};

static void anymoSensorRegistration(void) {
    enum SensorHandleIndex j;
    uint32_t Handle;
    /*check if sw anymotion has registered and then unregister*/
    if (SensorHasRegisterCheck(SENS_TYPE_ANY_MOTION) == true) {
        Handle = SensorHasRegisterReturnHandle(SENS_TYPE_ANY_MOTION);
        INVLOG(LOG_INFO, "ICM2060X find Any has registered, Handle=%d\n", Handle);
        sensorUnregister(Handle);
    }

    // For hw Anymotion register
    for (j = ANYMO; j < NUM_OF_HANDLE; j++) {
        mTask.sensors_handle[j].idx_handle = j;
        mTask.sensors_handle[j].handle = sensorRegister(&mSensorInfo_handle[j], &mSensorOps[j], (void *)j, false);
        INVLOG(LOG_INFO, "mTask.sensors_handle[j].handle=%d\n", mTask.sensors_handle[j].handle);
    }
}
    #endif
#endif 

#if MTK_VERSION_DIFF
static void accGetSensorInfo(struct sensorInfo_t *data)
{
    strncpy(data->name, ACC_NAME, sizeof(data->name));
}

static void gyroGetSensorInfo(struct sensorInfo_t *data)
{
    strncpy(data->name, GYRO_NAME, sizeof(data->name));
}
#endif

static void sensorCoreRegistration(void) {
    struct sensorCoreInfo mInfo;
    INVLOG(LOG_INFO, "ICM2060XRegisterCore\n");

    memset(&mInfo, 0x00, sizeof(struct sensorCoreInfo));
    /* Register sensor Core */
    mInfo.sensType = SENS_TYPE_ACCEL;
    mInfo.gain = GRAVITY_EARTH_1000;
    mInfo.sensitivity = mTask.sensors[ACC].sensitivity;
    mInfo.cvt = mTask.cvt;
    mInfo.getCalibration = accGetCalibration;
    mInfo.setCalibration = accSetCalibration;
    mInfo.getData = accGetData;
    mInfo.setDebugTrace = ICM2060XSetDebugTrace;

#if MTK_VERSION_DIFF
    mInfo.getSensorInfo = accGetSensorInfo;
#endif
    sensorCoreRegister(&mInfo);

    mInfo.sensType = SENS_TYPE_GYRO;
    mInfo.gain = GYROSCOPE_INCREASE_NUM_AP;
    mInfo.sensitivity = mTask.sensors[GYR].sensitivity;
    mInfo.cvt = mTask.cvt;
    mInfo.getCalibration = gyroGetCalibration;
    mInfo.setCalibration = gyroSetCalibration;
    mInfo.getData = gyroGetData;
#if MTK_VERSION_DIFF
     mInfo.getSensorInfo = gyroGetSensorInfo;
#endif
    sensorCoreRegister(&mInfo);
}

static int ICM2060XSensorRegistration(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    
    //record ICM2060X_REG_ACCEL_INTEL_CTRL
    mTask.smd_cfg_reg69 = mTask.statusBuffer[1];

#ifdef ICM_EDGE_SENSITIVE
    mTask.int_pin_cfg   =  mTask.int_statusBuffer[1];
#else
    mTask.int_pin_cfg   =  (mTask.int_statusBuffer[1] & (~0xe0 )) | ( 0xe0 & 0x20)  ;
    INVLOG(LOG_INFO, " write int pin cfg 0x%x\n", mTask.int_pin_cfg);
    //enable fifo over flow int
    SPI_WRITE(ICM2060X_REG_INT_PIN_CFG, mTask.int_pin_cfg);
#endif
   
    INVLOG(LOG_INFO, "ICM2060XEintRegistration interrupt status 0x%x\n", mTask.regBuffer[1]);
    //enable fifo over flow int
    SPI_WRITE(ICM2060X_REG_INT_ENABLE, mTask.regBuffer[1] | BIT_FIFO_OFLOW_EN );
    mTask.int_src0 = mTask.regBuffer[1] | BIT_FIFO_OFLOW_EN;

#if SUPPORT_ANYMO
    #if !MTK_VERSION_DIFF
    anymoSensorRegistration();    
    uint8_t i;
    // For motion
    for (i = 0; i < NUM_OF_HANDLE; i++) {
        sensorRegisterInitComplete(mTask.sensors_handle[i].handle);
    }
    #endif
#endif
    sensorCoreRegistration();
    
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

static struct sensorFsm ICM2060XFsm[] = {
    /* sample */
    sensorFsmCmd(STATE_SAMPLE, STATE_FIFO, ICM2060XSample),
    sensorFsmCmd(STATE_FIFO, STATE_CONVERT, ICM2060XReadFifo),
    sensorFsmCmd(STATE_CONVERT, STATE_FIFO_COUNT_CHECK, ICM2060XConvert),
    sensorFsmCmd(STATE_FIFO_COUNT_CHECK,STATE_SAMPLE_DONE ,ICM2060X_RereadFifo),

    sensorFsmCmd(STATE_ACC_ENABLE, STATE_ACC_ENABLE_DONE, ICM2060XgEnable),
    sensorFsmCmd(STATE_ACC_DISABLE, STATE_ACC_DISABLE_DONE, ICM2060XgDisable),
#if DEBUG_PHASE
    sensorFsmCmd(STATE_ACC_RATECHG,STATE_REGDUMP_READ , ICM2060XgRate),
    sensorFsmCmd(STATE_REGDUMP_READ,STATE_REGDUMP_RESULT , ICM2060Xregdump),
    sensorFsmCmd(STATE_REGDUMP_RESULT,STATE_ACC_RATECHG_DONE , ICM2060Xregresult),  
#else
    sensorFsmCmd(STATE_ACC_RATECHG, STATE_ACC_RATECHG_DONE, ICM2060XgRate),
#endif 
    sensorFsmCmd(STATE_ACC_CALI, STATE_ACC_CALI_DONE, ICM2060XAccCali),
    sensorFsmCmd(STATE_ACC_CFG, STATE_ACC_CFG_DONE, ICM2060XAccCfgCali),

    sensorFsmCmd(STATE_GYRO_ENABLE, STATE_GYRO_ENABLE_DONE, ICM2060XgyEnable),
    sensorFsmCmd(STATE_GYRO_DISABLE, STATE_GYRO_DISABLE_DONE, ICM2060XgyDisable),

#if DEBUG_PHASE
    sensorFsmCmd(STATE_GYRO_RATECHG,STATE_REGDUMP_READ , ICM2060XgyRate),
    sensorFsmCmd(STATE_REGDUMP_READ,STATE_REGDUMP_RESULT , ICM2060Xregdump),
    sensorFsmCmd(STATE_REGDUMP_RESULT,STATE_GYRO_RATECHG_DONE , ICM2060Xregresult),  
#else
    sensorFsmCmd(STATE_GYRO_RATECHG, STATE_GYRO_RATECHG_DONE, ICM2060XgyRate),
#endif
    sensorFsmCmd(STATE_GYRO_CALI, STATE_GYRO_CALI_DONE, ICM2060XGyroCali),
    sensorFsmCmd(STATE_GYRO_CFG, STATE_GYRO_CFG_DONE, ICM2060XGyroCfgCali),

    /* INT handle */
    sensorFsmCmd(STATE_HW_INT_STATUS_CHECK, STATE_HW_INT_HANDLING, ICM2060XIntStatusCheck),
    sensorFsmCmd(STATE_HW_INT_HANDLING, STATE_HW_INT_HANDLING_DONE, ICM2060XIntHandling),

    /* init state */
    sensorFsmCmd(STATE_SW_RESET, STATE_SW_RESET_W, ICM2060XResetRead),
    sensorFsmCmd(STATE_SW_RESET_W, STATE_INT_STATUS, ICM2060XResetWrite),
    sensorFsmCmd(STATE_INT_STATUS, STATE_RESET_CHECK, ICM2060XResetRead),
    sensorFsmCmd(STATE_RESET_CHECK, STATE_CLKSEL_REG_READ, ICM2060XResetCheck),
    sensorFsmCmd(STATE_CLKSEL_REG_READ, STATE_CLKSEL_REG_WRITE, ICM2060X_PWRMGMT_Read),
    sensorFsmCmd(STATE_CLKSEL_REG_WRITE, STATE_POWER_R, ICM2060XClkSelWrite),
    sensorFsmCmd(STATE_POWER_R, STATE_ENPOWER_W, ICM2060X_PWRMGMT_Read),
    sensorFsmCmd(STATE_ENPOWER_W,STATE_POWER2_R , ICM2060XPowerEnableWrite),
    sensorFsmCmd(STATE_POWER2_R, STATE_INIT_REG, ICM2060X_PWRMGMT2_Read),
    sensorFsmCmd(STATE_INIT_REG, STATE_SENSOR_REGISTRATION, ICM2060XInitConfig),
    sensorFsmCmd(STATE_SENSOR_REGISTRATION, STATE_EINT_REGISTRATION, ICM2060XSensorRegistration),
    sensorFsmCmd(STATE_EINT_REGISTRATION, STATE_INIT_DONE, ICM2060XEintRegistration),

#if SUPPORT_SELFTEST
    /*self test FSM ACC*/
    sensorFsmCmd(STATE_ACC_SELF_TEST, STATE_SW_RESET_W, ICM2060XResetRead),
    sensorFsmCmd(STATE_SW_RESET_W, STATE_INT_STATUS, ICM2060XResetWrite),
    sensorFsmCmd(STATE_INT_STATUS, STATE_RESET_CHECK, ICM2060XResetRead),
    sensorFsmCmd(STATE_RESET_CHECK, STATE_SELFTEST_INIT, ICM2060XResetCheck),
    sensorFsmCmd(STATE_SELFTEST_INIT, STATE_ACC_ENABLE, ICM2060XSelftest_init),
    sensorFsmCmd(STATE_ACC_ENABLE, STATE_ACC_SELFTEST_PHASE1, ICM2060XgEnable),
    sensorFsmCmd(STATE_ACC_SELFTEST_PHASE1, STATE_ACC_SELFTEST_PHASE2, icm2060XSelftest_startread_accdata),
    sensorFsmCmd(STATE_ACC_SELFTEST_PHASE2, STATE_ACC_SELFTEST_RESULT, icm2060XSelftest_startread_STaccdata), 
    sensorFsmCmd(STATE_ACC_SELFTEST_RESULT, STATE_SW_RESET, ICM2060XSelftest_acc_result), 
    sensorFsmCmd(STATE_SW_RESET, STATE_SW_RESET_W, ICM2060XResetRead),
    sensorFsmCmd(STATE_SW_RESET_W, STATE_INT_STATUS, ICM2060XResetWrite),
    sensorFsmCmd(STATE_INT_STATUS, STATE_RESET_CHECK, ICM2060XResetRead),
    sensorFsmCmd(STATE_RESET_CHECK, STATE_CLKSEL_REG_READ, ICM2060XResetCheck),
    sensorFsmCmd(STATE_CLKSEL_REG_READ, STATE_CLKSEL_REG_WRITE, ICM2060X_PWRMGMT_Read),
    sensorFsmCmd(STATE_CLKSEL_REG_WRITE, STATE_POWER_R, ICM2060XClkSelWrite),
    sensorFsmCmd(STATE_POWER_R, STATE_ENPOWER_W, ICM2060X_PWRMGMT_Read),
    sensorFsmCmd(STATE_ENPOWER_W,STATE_POWER2_R , ICM2060XPowerEnableWrite),
    sensorFsmCmd(STATE_POWER2_R, STATE_INIT_REG, ICM2060X_PWRMGMT2_Read),
    sensorFsmCmd(STATE_INIT_REG, STATE_SELFTEST_RECOVER, ICM2060XInitConfig),
    sensorFsmCmd(STATE_SELFTEST_RECOVER, STATE_ACC_SELF_TEST_DONE, ICM2060X_Selftest_Recover),

    /*self test FSM GYRO*/
    sensorFsmCmd(STATE_GYRO_SELF_TEST, STATE_SW_RESET_W, ICM2060XResetRead),
    sensorFsmCmd(STATE_SW_RESET_W, STATE_INT_STATUS, ICM2060XResetWrite),
    sensorFsmCmd(STATE_INT_STATUS, STATE_RESET_CHECK, ICM2060XResetRead),
    sensorFsmCmd(STATE_RESET_CHECK, STATE_SELFTEST_INIT, ICM2060XResetCheck),
    sensorFsmCmd(STATE_SELFTEST_INIT, STATE_GYRO_ENABLE, ICM2060XSelftest_init),
    sensorFsmCmd(STATE_GYRO_ENABLE, STATE_ACC_SELFTEST_PHASE1, ICM2060XgyEnable),
    sensorFsmCmd(STATE_GYRO_SELFTEST_PHASE1, STATE_GYRO_SELFTEST_PHASE2, icm2060XSelftest_startread_gyrodata),
    sensorFsmCmd(STATE_GYRO_SELFTEST_PHASE2, STATE_GYRO_SELFTEST_RESULT, icm2060XSelftest_startread_STgyrodata), 
    sensorFsmCmd(STATE_GYRO_SELFTEST_RESULT, STATE_SW_RESET, ICM2060XSelftest_gyro_result), 
    sensorFsmCmd(STATE_SW_RESET, STATE_SW_RESET_W, ICM2060XResetRead),
    sensorFsmCmd(STATE_SW_RESET_W, STATE_INT_STATUS, ICM2060XResetWrite),
    sensorFsmCmd(STATE_INT_STATUS, STATE_RESET_CHECK, ICM2060XResetRead),
    sensorFsmCmd(STATE_RESET_CHECK, STATE_CLKSEL_REG_READ, ICM2060XResetCheck),
    sensorFsmCmd(STATE_CLKSEL_REG_READ, STATE_CLKSEL_REG_WRITE, ICM2060X_PWRMGMT_Read),
    sensorFsmCmd(STATE_CLKSEL_REG_WRITE, STATE_POWER_R, ICM2060XClkSelWrite),
    sensorFsmCmd(STATE_POWER_R, STATE_ENPOWER_W, ICM2060X_PWRMGMT_Read),
    sensorFsmCmd(STATE_ENPOWER_W,STATE_POWER2_R , ICM2060XPowerEnableWrite),
    sensorFsmCmd(STATE_POWER2_R, STATE_INIT_REG, ICM2060X_PWRMGMT2_Read),  
    sensorFsmCmd(STATE_INIT_REG, STATE_SELFTEST_RECOVER, ICM2060XInitConfig),
    sensorFsmCmd(STATE_SELFTEST_RECOVER, STATE_GYRO_SELF_TEST_DONE, ICM2060X_Selftest_Recover),
    
#endif
    /* For Anymotion */
#if SUPPORT_ANYMO
    sensorFsmCmd(STATE_ANYMO_ENABLE, STATE_ANYMO_ENABLE_DONE, anyMotionPowerOn),
    sensorFsmCmd(STATE_ANYMO_DISABLE, STATE_ANYMO_DISABLE_DONE, anyMotionPowerOff),
#endif
};

int ICM2060XInit(void) {
    int ret = 0;
    enum SensorIndex i;
    uint8_t txData[2] = {0}, rxData[2] = {0};

    ICM2060XDebugPoint = &mTask;
    insertMagicNum(&mTask.accGyroPacket);
    mTask.hw = get_cust_accGyro("icm206xx");
    if (NULL == mTask.hw) {
        osLog(LOG_ERROR, "get_cust_acc_hw fail\n");
        return 0;
    }
    INVLOG(LOG_INFO, "icm206xx init acc spi_num: %d,task %d\n", mTask.hw->i2c_num,sizeof(mTask));
    

    if (0 != (ret = sensorDriverGetConvert(mTask.hw->direction, &mTask.cvt))) {
        osLog(LOG_ERROR, "invalid direction: %d\n", mTask.hw->direction);
    }
    INVLOG(LOG_INFO, "acc map[0]:%d, map[1]:%d, map[2]:%d, sign[0]:%d, sign[1]:%d, sign[2]:%d\n\r",
        mTask.cvt.map[AXIS_X], mTask.cvt.map[AXIS_Y], mTask.cvt.map[AXIS_Z],
        mTask.cvt.sign[AXIS_X], mTask.cvt.sign[AXIS_Y], mTask.cvt.sign[AXIS_Z]);

    mTask.latch_time_id = alloc_latch_time();
    enable_latch_time(mTask.latch_time_id, mTask.hw->eint_num);

    for (i = ACC; i < NUM_OF_SENSOR; i++) {
        initSensorStruct(&mTask.sensors[i], i);
    }
    mTask.int_src0 = 0;
    mTask.smd_cfg_reg69 = 0;
    //mTask.acc_cfg0 = 0;
    //mTask.gyro_cfg0 = 0;
    mTask.pwr_mgmr_config = 0;
    mTask.pwr_mgmr_config2 = 0;
    mTask.product = UNVALID_TYPE;

    mTask.fifo_reRead_count = 0;
    mTask.acc_report_size = 0;
    mTask.gyro_report_size = 0;
    mTask.realtime_report_ts = 0;
	mTask.last_hwSampleTime = 0;
	mTask.lastSampleTime = 0;
    
    mTask.mode.speed = 5000000;
    mTask.mode.bitsPerWord = 8;
    mTask.mode.cpol = SPI_CPOL_IDLE_LO;
    mTask.mode.cpha = SPI_CPHA_LEADING_EDGE;
    mTask.mode.nssChange = true;
    mTask.mode.format = SPI_FORMAT_MSB_FIRST;
    mTask.mWbufCnt = 0;
    mTask.mRegCnt = 0;
   
    spiMasterRequest(mTask.hw->i2c_num, &mTask.spiDev);

    txData[0] = ICM2060X_PWR_MGMT_1 | 0x80;
    ret = spiMasterRxTxSync(mTask.spiDev, rxData, txData, 2);
    INVLOG(LOG_INFO, "initial reset reg: %02x, ret: %d\n", rxData[1], ret);

    uint8_t j = 0;
    while(j < 3){
        txData[0] = ICM2060X_REG_DEVID | 0x80;
        ret = spiMasterRxTxSync(mTask.spiDev, rxData, txData, 2);
        osLog(LOG_ERROR, "ICM2060X_WHOAMI-2: %02x, ret: %d\n", rxData[1], ret);
        j++;
    }
    switch(rxData[1])
    {
    case ICM20600_WHOAMI:
        mTask.product = ICM20600;
        INVLOG(LOG_INFO, "ICM20600 detected\n");
        break;
    case ICM20607_WHOAMI:/* ICM20607 */
        mTask.product = ICM20607;
        INVLOG(LOG_INFO, "ICM20607 detected\n");
        break;
    case ICM20690_WHOAMI:/* ICM20690 */
        mTask.product = ICM20690;
        INVLOG(LOG_INFO, "ICM20690 detected\n");
        break;
    case ICM20627_WHOAMI:/* ICM20627 */
        mTask.product = ICM20627;
        INVLOG(LOG_INFO, "ICM20627 detected\n");
        break;
    case ICM20602_WHOAMI:
        mTask.product = ICM20602;
        INVLOG(LOG_INFO, "ICM20602 detected\n");
        break;
	case  MPU6887_WHOAMI:
      mTask.product = MPU6887;
       INVLOG(LOG_INFO, "MPU6887 detected\n");
      break;
    default:
        mTask.product = UNVALID_TYPE;
        INVLOG(LOG_INFO, "currently does not support this chip\n");
    }
    
    if (ret < 0 || mTask.product == UNVALID_TYPE){
        osLog(LOG_ERROR, "failed id match: %02x, ret: %d\n", rxData[1], ret);
        ret = -1;
        spiMasterRelease(mTask.spiDev);
        disable_latch_time(mTask.latch_time_id);
        free_latch_time(mTask.latch_time_id);
        goto err_out;
    }
    osLog(LOG_ERROR, "ICM2060X-2: auto detect success: %02x\n", rxData[1]);

        //Only applicable for factory calibration, refer to sensor spec
    mTask.sensors[ACC].sensitivity = (float)65536 / (8 * 2);//LSB/g sensitivity,8g acc

  //  if(mTask.product == ICM20690 || mTask.product == ICM20627)
  //      mTask.sensors[GYR].sensitivity = (float)65536 / (500 * 2);//LSB/dps sensitivity
  //  else
	mTask.sensors[GYR].sensitivity = (float)65536 / (2000 * 2);//LSB/dps sensitivity
    
    accSensorRegister();
    gyroSensorRegister();
#if MTK_VERSION_DIFF
    anyMotionSensorRegister();
#endif
   
    registerAccGyroInterruptMode(ACC_GYRO_FIFO_INTERRUPTIBLE);
    registerAccGyroDriverFsm(ICM2060XFsm, ARRAY_SIZE(ICM2060XFsm));
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
        .min_still_duration_nanos = SEC_TO_NANOS(5.9f),
        .max_still_duration_nanos = SEC_TO_NANOS(6.9f),
        .calibration_time_nanos = 0,
        .window_time_duration_nanos = SEC_TO_NANOS(1.5f),
        .bias_x = 0,
        .bias_y = 0,
        .bias_z = 0,
        .stillness_threshold = 0.95f,
        .stillness_mean_delta_limit = MDEG_TO_RAD * 40.0f,
        .gyro_var_threshold = 5e-5f,
        .gyro_confidence_delta = 1e-5f,
        .accel_var_threshold = 8e-3f,
        .accel_confidence_delta = 1.6e-3f,
        .mag_var_threshold = 1.4f,
        .mag_confidence_delta = 0.25f,
        .temperature_delta_limit_celsius = 1.5f,
        .gyro_calibration_enable = true,
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
        .over_temp_enable = false,
    };
    overTempCalInit(GetOverTempCalBase(), &gyro_otc_parameters);
#endif
err_out:
    return ret;
}
#ifndef CFG_OVERLAY_INIT_SUPPORT
MODULE_DECLARE(icm206xx, SENS_TYPE_ACCEL, ICM2060XInit);
#else
#include "mtk_overlay_init.h"
OVERLAY_DECLARE(icm206xx, OVERLAY_WORK_INV, ICM2060XInit);
#endif
