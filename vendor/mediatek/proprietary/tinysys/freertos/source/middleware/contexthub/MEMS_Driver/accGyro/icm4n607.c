/*
 * Copyright (C) 2018 InvenSense Inc
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

version list:

This driver is for ICM-4n607 series, 
/version/   release record / owner name / date  /

-----------------------------------------------------------------------------------------------------------
For MTK 4G platform update to 1.0 version
v1.0.0   icm4n607 driver for MTK sensor hub , update  /2020/12/25 */

#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <stdbool.h>
#include <stdint.h>
//#include <seos.h>
#include <util.h>
//#include <atomic.h>
//#include <cpu/inc/cpuMath.h>
#include <gpio.h>
#include <heap.h>
#include <slab.h>
//#include <timer.h>
#include <variant/inc/variant.h>
//#include <limits.h>
#include <performance.h>
#include <sensors.h>
#include <plat/inc/rtc.h>
#include <plat/inc/spichre.h>
#include <spichre-plat.h>
#include <hostIntf.h>
#include <nanohub_math.h>
#include <nanohubPacket.h>
//#include <plat/inc/rtc.h>
#include <spi.h>
#include <contexthub_core.h>
#include <accGyro.h>
#include <cust_accGyro.h>
//#include <mt_gpt.h>
//#include <algos/time_sync.h>
#include "hwsen.h"
#include "eint.h"
#include <math.h>

#include "icm4n607_reg.h"

#ifndef CFG_MTK_CALIBRATION_V1_0
#include <API_sensor_calibration.h>
#else
#include "calibration/factory_calibration/factory_cal.h"
#endif

#define ACC_NAME     "icm4n607_acc"
#define GYRO_NAME    "icm4n607_gyro"


#define SUPPORT_SELFTEST      1

#ifdef SUPPORT_FSM_FREEFALL
    #define SUPPORT_HW_FREEFALL    1
    #define SUPPORT_HW_FREEFALL_HP 0
#endif

#define DEBUG_FIFO_DATA  	 0
#define DEBUG_ACC_DATA       0
#define DEBUG_GYRO_DATA      0
#define DEBUG_TEMP_DATA      0
#define DEBUG_PHASE          0
#define DEBUG_STEP_DATA      0

#define DEBUG_MCLK           0
#define DEBUG_SENSOR_STABLE  0
#define DEBUG_SELFTEST       0

#define NUM_TODISCARD        0
#define SPI_TRIM_3WIRE_DEFAULT 1
#define SPI_SLEW_RATE_CHANGE  1 

#define ALL_LOGINFO_OFF      0 

/* customer should not touch all following macro definition */

#define DEBUG_APEX_CONF      0

#define DEF_WOM_THRESHOLD			70 //default:70mg, max=1020mg
#define GYRO_TURNON_TIME            80000 //us unit
#define GYRO_RINGDOWN_TIME          150000000 // ns unit

#if ALL_LOGINFO_OFF
#define INVLOG(...) 
#else
#define INVLOG osLog
#endif

#define ICM4N607_FIFO_SIZE                       1500
#define G                                        9.80665
#define PI                                       3.141592
#define KSCALE_ACC_8G_RANGE                      0.002394202f     // ACC_range * 9.81f / 65536.0f;
#define KSCALE_GYRO_2000_RANGE                   0.001065264f  // GYR_range * M_PI / (180.0f * 65536.0f);
#define TEMP_SENSITIVITY_1B_4N607                100/207
#define TEMP_SENSITIVITY_2B_4N607                1/128
#define ROOM_TEMP_OFFSET		                 25 //Celsius degree

#define ICM4N607_MAX_ODR_800HZ (0)   /* default off 800hz, when enable 800hz, driver will do software data odr decimation */ 

#define ICM4N607_DEFAULT_WOM_THS_MG 52>>2 /* = 52mg/4 */



#define max(x, y)   (x > y ? x : y)

#define Config_fifo_sel(sel)      SPI_WRITE(ICM4N607_REG_BANK_SEL, 4);\
                                  SPI_WRITE(ICM4N607_REG_FDR_CONFIG, sel);\
	                              SPI_WRITE(ICM4N607_REG_BANK_SEL, 0);



#ifndef abs
#define abs(a)      ((a) > 0 ? (a) : (0 -(a))) /*!< Absolute value */
#endif

#define AXIS_X              0
#define AXIS_Y              1
#define AXIS_Z              2
#define AXES_NUM            3

#if SUPPORT_HW_FREEFALL
/* low */
	#define BIT_LOWG_PEAK_TH_HYST_MASK		 0x38
	#define BIT_LOWG_PEAK_TH_HYST_SHIFT 	 3
#define REG_APEX_CONFIG5            0X44
	#define BIT_LOWG_PEAK_TH_SHIFT			 3
	#define BIT_LOWG_PEAK_TH_MASK			 0xF8
	#define BIT_LOWG_TIME_TH_MASK			 0x07
	
typedef enum {
  LOWG_TIME_TH_20MS  = 0,  
  LOWG_TIME_TH_40MS  = 1,
  LOWG_TIME_TH_60MS  = 2,
  LOWG_TIME_TH_80MS  = 3,
  LOWG_TIME_TH_100MS = 4,
  LOWG_TIME_TH_120MS = 5,
  LOWG_TIME_TH_140MS = 6,
  LOWG_TIME_TH_160MS = 7
} LOWG_TIME_TH_t;

typedef enum {
  LOWG_PEAK_TH_31MG   = (0x00 << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_63MG   = (0x01 << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_94MG   = (0x02 << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_125MG  = (0x03 << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_156MG  = (0x04 << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_188MG  = (0x05 << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_219MG  = (0x06 << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_250MG  = (0x07 << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_281MG  = (0x08 << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_313MG  = (0x09 << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_344MG  = (0x0A << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_375MG  = (0x0B << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_406MG  = (0x0C << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_438MG  = (0x0D << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_469MG  = (0x0E << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_500MG  = (0x0F << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_531MG  = (0x10 << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_563MG  = (0x11 << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_594MG  = (0x12 << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_625MG  = (0x13 << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_656MG  = (0x14 << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_688MG  = (0x15 << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_719MG  = (0x16 << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_750MG  = (0x17 << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_781MG  = (0x18 << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_813MG  = (0x19 << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_844MG  = (0x1A << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_875MG  = (0x1B << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_906MG  = (0x1C << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_938MG  = (0x1D << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_969MG  = (0x1E << BIT_LOWG_PEAK_TH_SHIFT),
  LOWG_PEAK_TH_1000MG = (0x1F << BIT_LOWG_PEAK_TH_SHIFT)
} LOWG_PEAK_TH_t;

typedef enum {
  LOWG_PEAK_TH_HYST_31MG  = (0  << BIT_LOWG_PEAK_TH_HYST_SHIFT),
  LOWG_PEAK_TH_HYST_63MG  = (1  << BIT_LOWG_PEAK_TH_HYST_SHIFT),
  LOWG_PEAK_TH_HYST_94MG  = (2  << BIT_LOWG_PEAK_TH_HYST_SHIFT),
  LOWG_PEAK_TH_HYST_125MG = (3  << BIT_LOWG_PEAK_TH_HYST_SHIFT),
  LOWG_PEAK_TH_HYST_156MG = (4  << BIT_LOWG_PEAK_TH_HYST_SHIFT),
  LOWG_PEAK_TH_HYST_188MG = (5  << BIT_LOWG_PEAK_TH_HYST_SHIFT),
  LOWG_PEAK_TH_HYST_219MG = (6  << BIT_LOWG_PEAK_TH_HYST_SHIFT),
  LOWG_PEAK_TH_HYST_250MG = (7  << BIT_LOWG_PEAK_TH_HYST_SHIFT)
} LOWG_PEAK_TH_HYST_t;
#endif

/** @brief ICM4N607 APEX inputs parameters definition
 */
typedef struct {
	ICM4N607_APEX_CONFIG3_PEDO_AMP_TH_t pedo_amp_th;                        /**< Peak threshold value to be considered as a valid step (mg) */
	uint8_t pedo_step_cnt_th;                                               /**< Minimum number of steps that must be detected 
	                                                                             before the pedometer step count begins incrementing */
	uint8_t pedo_step_det_th;                                               /**< Minimum number of low latency steps that must be detected 
	                                                                             before the pedometer step count begins incrementing */
	ICM4N607_APEX_CONFIG4_PEDO_SB_TIMER_TH_t pedo_sb_timer_th;              /**< Duration of non-walk to exit the current walk mode, 
	                                                                             pedo_step_cnt_th number of steps must again be detected 
	                                                                             before step count starts to increase */
	ICM4N607_APEX_CONFIG4_PEDO_HI_ENRGY_TH_t pedo_hi_enrgy_th;              /**< Threshold to improve run detection if not steps are counted while running */
 
	ICM4N607_APEX_CONFIG5_TILT_WAIT_TIME_t tilt_wait_time;                  /**< Number of accelerometer samples to wait before triggering tilt event */
 
	ICM4N607_APEX_CONFIG2_DMP_POWER_SAVE_TIME_t power_save_time;            /**< The time after which DMP goes in power save mode according to the DMP ODR configured */
	ICM4N607_APEX_CONFIG0_DMP_POWER_SAVE_t power_save;                      /**< Power save mode for APEX algorithms. This mode will put APEX features into sleep mode, 
	                                                                             leaving only the WOM running to wake-up the DMP */
	
	ICM4N607_APEX_CONFIG9_SENSITIVITY_MODE_t sensitivity_mode;             /**< Sensitivity mode Normal(0) or Slow walk(1). The Slow walk mode improve 
	                                                                             the slow walk detection (<1Hz) but in return the number of false detection 
	                                                                             might be increase. */
	ICM4N607_APEX_CONFIG2_LOW_ENERGY_AMP_TH_t low_energy_amp_th;            /**< Peak threshold value to be considered as a valid step (mg) in Slow walk mode */
	
	ICM4N607_APEX_CONFIG9_SMD_SENSITIVITY_t smd_sensitivity;                /**< SMD algorithm resilience to false detection in rejection use case.
	                                                                             Note that a higher value will reject more transport situation */
	
	ICM4N607_APEX_CONFIG9_FF_DEBOUNCE_DURATION_t ff_debounce_duration;      /**< Duration(us) during which LowG and HighG events are not taken into account after an HighG event.
	                                                                             The goal is to avoid detecting bounces as free falls */
	
	ICM4N607_APEX_CONFIG12_FF_MAX_DURATION_t ff_max_duration_cm;            /**< Distance (cm) max crossed by the device after a LowG event, below which the detection of an HighG event triggers a freefall interrupt */
	ICM4N607_APEX_CONFIG12_FF_MIN_DURATION_t ff_min_duration_cm;            /**< Distance (cm) min crossed by the device after a LowG event, above which the detection of an HighG event triggers a freefall interrupt */
	
	ICM4N607_APEX_CONFIG10_LOWG_PEAK_TH_t lowg_peak_th;                     /**< Absolute low peak threshold (mg) to detect when it falls below on any of the accelerometer axis */
	ICM4N607_APEX_CONFIG5_LOWG_PEAK_TH_HYST_t lowg_peak_hyst;               /**< Hysteresis threshold (mg) added to the threshold after the initial threshold is met */
	ICM4N607_APEX_CONFIG10_LOWG_TIME_TH_SAMPLES_t lowg_samples_th;          /**< Time in number of samples to stay below the threshold before triggering the event (samples) */
	
	ICM4N607_APEX_CONFIG11_HIGHG_PEAK_TH_t highg_peak_th;                   /**< Absolute high peak threshold to detect when it goes above on any of the accelerometer axis */
	ICM4N607_APEX_CONFIG5_HIGHG_PEAK_TH_HYST_t highg_peak_hyst;             /**< Hysteresis threshold (mg) substracted  to the threshold after the initial threshold is met */
	ICM4N607_APEX_CONFIG11_HIGHG_TIME_TH_SAMPLES_t highg_samples_th;        /**< Time in number of samples to stay above the threshold before triggering the event (samples) */
	
} inv_icm4n607_apex_parameters_t;

/* Support odr range 25HZ - 200HZ */
static uint32_t ICM4N607HWRates[] = {
    SENSOR_HZ(12.5f),
    SENSOR_HZ(25.0f),
    SENSOR_HZ(50.0f),
    SENSOR_HZ(100.0f),
    SENSOR_HZ(200.0f),
    SENSOR_HZ(400.0f),
#if ICM4N607_MAX_ODR_800HZ
    SENSOR_HZ(800.0f),
#endif
    0,
};

static uint8_t ICM4N607_ODR_MAPPING[] = {
    ICM4N607_ACCEL_CONFIG0_ODR_12_5_HZ,
    ICM4N607_ACCEL_CONFIG0_ODR_25_HZ,
    ICM4N607_ACCEL_CONFIG0_ODR_50_HZ,
    ICM4N607_ACCEL_CONFIG0_ODR_100_HZ,
    ICM4N607_ACCEL_CONFIG0_ODR_200_HZ,
    ICM4N607_ACCEL_CONFIG0_ODR_400_HZ,
#if ICM4N607_MAX_ODR_800HZ
    ICM4N607_ACCEL_CONFIG0_ODR_800_HZ,
#endif
};

#if DEBUG_SENSOR_STABLE
static struct sensordata{
    float x;
    float y;
    float z;
}previous_acc,previous_gyro;

uint16_t data_count;
uint16_t data_count_gyro;
#endif

#define SPI_PACKET_SIZE 30
#define ICM4N607_MAX_FIFO_SIZE (2100)   // Max FIFO count size for FIFO overflow case
#define SPI_BUF_SIZE    (1024)
#define EVT_SENSOR_ANY_MOTION       sensorGetMyEventType(SENS_TYPE_ANY_MOTION)
#ifdef CFG_HW_STEP_COUNTER_SUPPORT
#define EVT_SENSOR_STEP_COUNTER     sensorGetMyEventType(SENS_TYPE_STEP_COUNT)
#endif

#define SPI_WRITE_0(addr, data) spiQueueWrite(addr, data, 2)
#define SPI_WRITE_1(addr, data, delay) spiQueueWrite(addr, data, delay)
#define GET_SPI_WRITE_MACRO(_1, _2, _3, NAME, ...) NAME
#define SPI_WRITE(...) GET_SPI_WRITE_MACRO(__VA_ARGS__, SPI_WRITE_1, SPI_WRITE_0)(__VA_ARGS__)

#define SPI_READ_0(addr, size, buf) spiQueueRead(addr, size, buf, 0)
#define SPI_READ_1(addr, size, buf, delay) spiQueueRead(addr, size, buf, delay)
#define GET_SPI_READ_MACRO(_1, _2, _3, _4, NAME, ...) NAME
#define SPI_READ(...) GET_SPI_READ_MACRO(__VA_ARGS__, SPI_READ_1, SPI_READ_0)(__VA_ARGS__)

static void ICM4N607ConfigFifo(bool odr_change);
static uint16_t ICM4N607CalcuWm(void);
static void inv_icm4n607_configure_wom(const uint8_t wom_x_th, const uint8_t wom_y_th, const uint8_t wom_z_th,
                               ICM4N607_WOM_CONFIG_WOM_INT_MODE_t wom_int, ICM4N607_WOM_CONFIG_WOM_INT_DUR_t wom_dur);
static void inv_icm4n607_read_mclk_reg( uint16_t regaddr, size_t len,uint8_t ** buf,uint32_t delay);
static void inv_icm4n607_write_mclk_reg( uint16_t regaddr, uint8_t databuf, uint32_t delay);
static void ICM4N607_set_acc_power_mode(uint8_t mode);

#if defined(CFG_HW_STEP_COUNTER_SUPPORT) || SUPPORT_HW_FREEFALL
static int inv_icm4n607_init_apex_parameters_struct(inv_icm4n607_apex_parameters_t *apex_inputs);
static void inv_icm4n607_configure_apex_parameters(const inv_icm4n607_apex_parameters_t *apex_inputs);
#endif

enum icm4n607State {
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
    STATE_INIT_REG,
    STATE_SENSOR_REGISTRATION,
    STATE_EINT_REGISTRATION,
    STATE_CP25REG_READ,
#if SUPPORT_SELFTEST
    STATE_SELFTEST_RESET_LOTP1,
    STATE_SELFTEST_LOAD_OTP2,
    STATE_SELFTEST_CONFIG_PARAMETER,
    STATE_SELFTEST_CONTROL,
    STATE_SELFTEST_RESULT_CHECK,
    STATE_SELFTEST_SEND_RESULT,
    STATE_ACC_SELFTEST_PHASE2,
    STATE_ACC_SELFTEST_RESULT,
    STATE_GYRO_SELFTEST_PHASE1,
    STATE_GYRO_SELFTEST_PHASE2,
    STATE_GYRO_SELFTEST_RESULT,
    STATE_SELFTEST_RECOVER,
#endif

#ifdef CFG_HW_STEP_COUNTER_SUPPORT
    STATE_STEPCOUNT_ENABLE_PH1,
    STATE_STEPCOUNT_ENABLE_PH2,
#endif

#if SUPPORT_HW_FREEFALL
    STATE_FREEFALL_ENABLE_PH1,
    STATE_FREEFALL_ENABLE_PH2,
#endif
#if DEBUG_PHASE | DEBUG_APEX_CONF
    STATE_REGDUMP_READ,
    STATE_REGDUMP_RESULT,
#endif
#if DEBUG_APEX_CONF
    STATE_DUMP_BANK4_READ,
    STATE_DUMP_BANK4_RESULT,
#endif

};

enum SensorIndex {
    ACC = 0,
    GYR,
    ANYMO,
#if defined(CFG_HW_STEP_COUNTER_SUPPORT) || SUPPORT_HW_FREEFALL
    STEPCNT,
    FREEFALL,
#endif   
    NUM_OF_SENSOR,
};

/* chip type */
enum chip_type {
    UNVALID_TYPE = 0,
    T1000,
    ICM42607,
    UN_IDENDITY,
  
};


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

/*  definition and meaning for some key parameter 
hwRate is the real sensor rate report to uplayer (like accgyro)
preRealRate is the rate requried from uplater 
*/
struct ICM4N607Sensor {
    float staticCali[AXES_NUM];
    uint64_t latency;
    int32_t accuracy;
    uint32_t rate;
    uint32_t hwRate;
    uint32_t fifo_rate;
    uint32_t preRealRate;
    uint32_t handle;
    float sensitivity;
    bool powered;
    bool configed;
    bool startCali;
    bool needDiscardSample;
    uint8_t samplesToDiscard;
#if ICM4N607_MAX_ODR_800HZ
    uint8_t samplediv;
    uint8_t sampledivcounter;
#endif
};

#if SUPPORT_SELFTEST
/* sensor x,y,z */
struct sensor_st_code {
    uint8_t x;
    uint8_t y;
    uint8_t z;
};

enum st_type {
    acc_st,
    gyro_st,
  
};
/* ST_OTP = (2620 / 2^FS) * 1.01^(ST_Code -1) (LSB) */
//lookup table: convert selft test code in register to otp value

#endif


static struct ICM4N607Task {
    struct ICM4N607Sensor sensors[NUM_OF_SENSOR];

    uint64_t hwSampleTime;
    uint64_t swSampleTime;
    uint64_t unmask_eint_time;
    uint64_t last_hwSampleTime;
    uint64_t lastSampleTime;
    uint64_t lastGyroDisabletime;
    uint64_t gyrodiscardStime;
    uint8_t *regBuffer;
    uint8_t *statusBuffer;
    uint8_t *statusBuffer2;
    uint8_t *statusBuffer3;
    uint8_t *statusBuffer4;
    uint8_t *statusBuffer5;
    uint8_t *fifostatus;
    uint8_t *int1status;
    uint8_t *fifoconfig5;
    uint8_t *wakeupBuffer;
#if SUPPORT_HW_FREEFALL
    uint8_t *freefallBuffer;
    uint32_t index, cur_idx;
#endif
#if SUPPORT_SELFTEST
    uint8_t *accel_st_code_buffer;
    uint8_t *gyro_st_code_buffer;
    uint8_t *rawdata_buffer;
    uint8_t otp_ctrl7;
    uint8_t selftest_mreg;
    uint8_t st_control;
#endif   
#if DEBUG_PHASE | DEBUG_APEX_CONF
    uint8_t *regdumpBuffer;
    uint8_t *debugintBuffer;
    uint8_t *regdumpBank_4;
#endif
    SpiCbkF spiCallBack;
    struct transferDataInfo dataInfo;
    struct accGyroDataPacket accGyroPacket;
    /* data for factory */
    struct TripleAxisDataPoint accFactoryData;
    struct TripleAxisDataPoint gyroFactoryData;

    int32_t accHwCali[AXES_NUM];
    int32_t gyroHwCali[AXES_NUM];
  
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
    uint32_t hw_min_rate;

    float   chip_temper;
    int latch_time_id;
    uint16_t watermark;
    /* For save reg status */
    uint8_t int_src0;
    uint8_t int_src1;
    uint8_t wom_cfg;
    uint8_t pwr_sta;
    uint8_t acc_cfg0;
    uint8_t gyro_cfg0;
    uint8_t intf_cfg0;
    uint8_t fifo_rcosc;
    uint8_t fdr_sel;
    uint8_t need_mclk_cnt;
    enum chip_type product;

#if SUPPORT_SELFTEST  
    struct sensor_st_code st_code[2];
    int32_t     avg[3];
    int32_t     acc_avg[3];
    int32_t     acc_avg_st[3];
    int32_t     gyro_avg[3];
    int32_t     gyro_avg_st[3];

    uint8_t gyro_config;
    uint8_t accel_config;
    uint8_t gyro_st_code_reg[3];
    uint8_t selftest_rawdata_reg; 
#endif

#if defined(CFG_HW_STEP_COUNTER_SUPPORT) || SUPPORT_HW_FREEFALL || SUPPORT_SELFTEST
    inv_icm4n607_apex_parameters_t apex_inputs;
    uint8_t *stepBuffer0;
    uint8_t *stepBuffer;
    uint8_t *stepSourceBuffer;
    uint8_t *stepstatusBuffer;
    uint8_t  apex_config0;
    uint8_t  apex_config1;
    uint8_t  int_source6;
    uint32_t pre_hwRate;
    uint32_t total_step_cnt;
    uint32_t last_step_cnt;
    bool     DMP_first_config;
    bool     DMP_reset_path_check;
    bool     DMP_from_sram;
#endif

#ifdef CFG_MTK_CALIBRATION_V1_0
    struct AccFactoryCal accFactoryCal;
    struct GyroFactoryCal gyroFactoryCal;
#endif
} mTask;

static uint8_t first_data = 0;
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

static void spiQueueWrite(uint8_t addr, uint8_t data, uint32_t delay)
{
    mTask.packets[mTask.mRegCnt].size = 2;
    mTask.packets[mTask.mRegCnt].txBuf = &mTask.txrxBuffer[mTask.mWbufCnt];
    mTask.packets[mTask.mRegCnt].rxBuf = &mTask.txrxBuffer[mTask.mWbufCnt];
    mTask.packets[mTask.mRegCnt].delay = delay * 1000;
    mTask.txrxBuffer[mTask.mWbufCnt++] = addr;
    mTask.txrxBuffer[mTask.mWbufCnt++] = data;
    mTask.mWbufCnt = (mTask.mWbufCnt + 3) & 0xFFFC;
    mTask.mRegCnt++;
}

static void spiQueueRead(uint8_t addr, size_t size, uint8_t **buf, uint32_t delay)
{
    *buf = &mTask.txrxBuffer[mTask.mWbufCnt];
    mTask.packets[mTask.mRegCnt].size = size + 1;  // first byte will not contain valid data
    mTask.packets[mTask.mRegCnt].txBuf = &mTask.txrxBuffer[mTask.mWbufCnt];
    mTask.packets[mTask.mRegCnt].rxBuf = *buf;
    mTask.packets[mTask.mRegCnt].delay = delay * 1000;
    mTask.txrxBuffer[mTask.mWbufCnt++] = 0x80 | addr;
    mTask.mWbufCnt = (mTask.mWbufCnt + size + 3) & 0xFFFC;
    mTask.mRegCnt++;
}

static void inv_icm4n607_read_reg(uint32_t reg, size_t len, uint8_t ** buf,uint32_t delay)
{
	uint32_t i;
		
	if(!(reg & 0x10000)) {
	    // top 1 register seems do not support busrt read & write
		for(i=0; i<len ; i++) {
			//rc |= inv_icm4n607_read_mclk_reg(s, ((reg+i)&0xFFFF), 1, &buf[i]);
			inv_icm4n607_read_mclk_reg(((reg+i)&0xFFFF), 1, buf,delay);
			}
		} else {

        if(delay)
			SPI_READ((uint8_t)reg, len, buf,delay);
        else
            SPI_READ((uint8_t)reg, len, buf);
	}	
}

// for write reg operation ,we do not support busrt write, only write one byte once time
static void inv_icm4n607_write_reg(uint32_t reg, uint8_t data,uint32_t delay)
{
	if(!(reg & 0x10000)){
		inv_icm4n607_write_mclk_reg((reg&0xFFFF), data, delay);
    }
	else if(reg & 0x10000){
		if(delay)
		    SPI_WRITE((uint8_t)reg,data,delay);
		else
		    SPI_WRITE((uint8_t)reg,data);
    }
}

static void inv_icm4n607_switch_on_mclk_nodelay()
{
	/* set IDLE bit only if it is not set yet */
	if (mTask.need_mclk_cnt == 0 || !(mTask.pwr_sta & ICM4N607_PWR_MGMT_0_IDLE_MASK)) {

	    // set IDLE bit and wait 10ms to skip always checking process
		mTask.pwr_sta |= ICM4N607_PWR_MGMT_0_IDLE_MASK; 
		SPI_WRITE((uint8_t)ICM4N607_PWR_MGMT_0,mTask.pwr_sta);

	} 

	/* Increment the counter to keep track of number of MCLK requesters */
	mTask.need_mclk_cnt++;		
}

static void inv_icm4n607_switch_on_mclk()
{
#if DEBUG_MCLK
    INVLOG(LOG_INFO, "on_mclk need_mclk_cnt %d, +1 after function, pwr_sta %x\n",mTask.need_mclk_cnt,mTask.pwr_sta);
#endif
	/* set IDLE bit only if it is not set yet */
	if (mTask.need_mclk_cnt == 0 || !(mTask.pwr_sta & ICM4N607_PWR_MGMT_0_IDLE_MASK)) {

	    // set IDLE bit and wait 5ms to skip always checking process
		mTask.pwr_sta |= ICM4N607_PWR_MGMT_0_IDLE_MASK; 
		SPI_WRITE((uint8_t)ICM4N607_PWR_MGMT_0,mTask.pwr_sta,5000);

	} 

	/* Increment the counter to keep track of number of MCLK requesters */
	mTask.need_mclk_cnt++;	
	
}

static void inv_icm4n607_switch_off_mclk()
{
#if DEBUG_MCLK
    INVLOG(LOG_INFO, "off_mclk need_mclk_cnt %d, -1 after function, pwr_sta %x\n",mTask.need_mclk_cnt,mTask.pwr_sta);
#endif
	/* Reset the IDLE but only if there is one requester left */
	if (mTask.need_mclk_cnt == 1 && (mTask.pwr_sta & ICM4N607_PWR_MGMT_0_IDLE_MASK)) {
		
		mTask.pwr_sta &= ~ICM4N607_PWR_MGMT_0_IDLE_MASK;
	    SPI_WRITE((uint8_t)ICM4N607_PWR_MGMT_0,mTask.pwr_sta);
	} 

    if(mTask.need_mclk_cnt != 0)
	    mTask.need_mclk_cnt--;   	/* Decrement the counter */
	
}

static void inv_icm4n607_write_mclk_reg( uint16_t regaddr, uint8_t databuf, uint32_t delay)
{
	uint8_t data;
	uint8_t blk_sel = (regaddr&0xFF00)>>8;

	// Have ICM not in IDLE mode to access MCLK domain
	inv_icm4n607_switch_on_mclk();

	// optimize by changing BLK_SEL only if not NULL
	if(blk_sel)
		SPI_WRITE((uint8_t)ICM4N607_BLK_SEL_W,blk_sel);
	
	data = (uint8_t)(regaddr&0x00FF);
	if(delay)
	    SPI_WRITE((uint8_t)ICM4N607_MADDR_W,data,delay);
	else
        SPI_WRITE((uint8_t)ICM4N607_MADDR_W,data);
    // wait 16 MCLK (4MHz) clock cycles
	SPI_WRITE((uint8_t)ICM4N607_M_W,databuf,4);
		
	if(blk_sel) {
	    SPI_WRITE((uint8_t)ICM4N607_BLK_SEL_W, 0);
	}

	inv_icm4n607_switch_off_mclk();
}

static void inv_icm4n607_read_mclk_reg( uint16_t regaddr, size_t len,uint8_t ** buf,uint32_t delay)
{
	uint8_t data;
	uint8_t blk_sel = (regaddr&0xFF00)>>8;

	// Have ICM not in IDLE mode to access MCLK domain
	inv_icm4n607_switch_on_mclk();

	// optimize by changing BLK_SEL only if not NULL
	if(blk_sel)
		SPI_WRITE((uint8_t)ICM4N607_BLK_SEL_R & 0xff,blk_sel);
	
	data = (uint8_t)(regaddr&0x00FF);
	// wait 16 MCLK (4MHz) clock cycles
    SPI_WRITE((uint8_t)ICM4N607_MADDR_R,data,4);
	
    // wait 16 MCLK (4MHz) clock cycles
    if(delay >=4 )
        SPI_READ((uint8_t)ICM4N607_M_R, len, buf,delay);
    else
        SPI_READ((uint8_t)ICM4N607_M_R, len, buf,4);     
		
	if(blk_sel) {
		SPI_WRITE((uint8_t)ICM4N607_BLK_SEL_R, 0);
	}

	// switch OFF MCLK if needed
	inv_icm4n607_switch_off_mclk();	
}


static int spiBatchTxRx(struct SpiMode *mode,
                        SpiCbkF callback, void *cookie, const char *src)
{
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

static void inv_imu_set_autorcosc_power_on(bool enable)
{
    mTask.fifo_rcosc &= (~ICM4N607_FIFO_CONFIG6_RCOSC_REQ_ON_FIFO_THS_DIS_MASK);
	if(!enable)
	    mTask.fifo_rcosc |= ICM4N607_FIFO_CONFIG6_RCOSC_REQ_ON_FIFO_THS_DIS_MASK;
	else
	    mTask.fifo_rcosc |= ICM4N607_FIFO_CONFIG6_RCOSC_REQ_ON_FIFO_THS_EN;
	    
	inv_icm4n607_write_reg(ICM4N607_FIFO_CONFIG6_MREG_TOP1, 1, mTask.fifo_rcosc);
}

static uint8_t ICM4N607CalcuOdr(uint32_t *rate, uint32_t *report_rate)
{
    uint8_t i;

    for (i = 0; i < (ARRAY_SIZE(ICM4N607HWRates) - 1); i++) {
        if (*rate <= ICM4N607HWRates[i]) {
            *report_rate = ICM4N607HWRates[i];
            break;
        }
    }

    if (*rate > ICM4N607HWRates[(ARRAY_SIZE(ICM4N607HWRates) - 2)]) {
        i = (ARRAY_SIZE(ICM4N607HWRates) - 2);
        *report_rate = ICM4N607HWRates[i];
    }

    return i;
}

#if defined(CFG_HW_STEP_COUNTER_SUPPORT) || SUPPORT_HW_FREEFALL||SUPPORT_SELFTEST
static void  icm4n607_sc_DMP_mem_reset()
{
    mTask.apex_config0 &= ~ICM4N607_APEX_CONFIG0_DMP_SRAM_RESET_EN_MASK;
	mTask.apex_config0 |= (ICM4N607_APEX_CONFIG0_DMP_SRAM_RESET_APEX_ST_EN & ICM4N607_APEX_CONFIG0_DMP_SRAM_RESET_EN_MASK);
	inv_icm4n607_write_reg(ICM4N607_APEX_CONFIG0,  mTask.apex_config0,50000); // wait time 50ms

    INVLOG(LOG_INFO, "icm4n607_sc_DMP_mem_reset apex_config0 %x\n", mTask.apex_config0);
    inv_icm4n607_read_reg(ICM4N607_APEX_CONFIG0, 1, &mTask.stepstatusBuffer,0);
}
#endif

#if defined(CFG_HW_STEP_COUNTER_SUPPORT) || SUPPORT_HW_FREEFALL
/*  DMP Power save mode */
static void icm4n607_sc_DMP_powersave(bool powersave)
{   
	if(true == powersave){
	    mTask.apex_config0 |= ICM4N607_APEX_CONFIG0_DMP_POWER_SAVE_EN;
    } else {
	    mTask.apex_config0 &= ~ICM4N607_APEX_CONFIG0_DMP_POWER_SAVE_EN_MASK;
    }

    inv_icm4n607_write_reg(ICM4N607_APEX_CONFIG0,mTask.apex_config0,0 );
 
    if(true == powersave){
         // enable wom
        mTask.wom_cfg &= ~ICM4N607_WOM_CONFIG_WOM_EN_MASK;
	    mTask.wom_cfg |= (uint8_t)ICM4N607_WOM_CONFIG_WOM_EN_ENABLE;
        inv_icm4n607_write_reg(ICM4N607_WOM_CONFIG, mTask.wom_cfg,0);
    } 
	INVLOG(LOG_INFO, "icm4n607_sc_DMP_powersave apex_config0 %x, wom_cfg %x \n", mTask.apex_config0,mTask.wom_cfg);
}

/*  set DMP_ODR  */
static void  icm4n607_sc_initDMP()
{
	mTask.apex_config1 &= ~ICM4N607_APEX_CONFIG1_DMP_ODR_MASK;
#if SUPPORT_HW_FREEFALL_HP
    if(mTask.sensors[ACC].hwRate >=  SENSOR_HZ(400.0f))
        mTask.apex_config1 |= ICM4N607_APEX_CONFIG1_DMP_ODR_100Hz;
#else
    if(mTask.sensors[ACC].hwRate >=  SENSOR_HZ(50.0f))
        mTask.apex_config1 |= ICM4N607_APEX_CONFIG1_DMP_ODR_50Hz;
#endif
    else
	    mTask.apex_config1 |= ICM4N607_APEX_CONFIG1_DMP_ODR_25Hz;

    INVLOG(LOG_INFO, "icm4n607_sc_initDMP apex_config1 %x\n", mTask.apex_config1);
    inv_icm4n607_write_reg(ICM4N607_APEX_CONFIG1, mTask.apex_config1,1000);
}
#endif

static int ICM4N607SoftReset(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                              void *inBuf, uint8_t inSize, uint8_t elemInSize,
                              void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint8_t val = 0;
    uint8_t saved_spi_config = mTask.regBuffer[1] ;
    
    INVLOG(LOG_INFO, "ICM4N607 spi configurebuff 0x%x\n", mTask.regBuffer[1]);
    
    val = (uint8_t)ICM4N607_SIGNAL_PATH_RESET_SOFT_RESET_CHIP_CONFIG_EN;;
    /* Wait 150ms to check reset status */
    inv_icm4n607_write_reg(ICM4N607_SIGNAL_PATH_RESET, val, 150000);

    /*restore configure spi status */
    inv_icm4n607_write_reg(ICM4N607_CHIP_CONFIG_REG, saved_spi_config,0);
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}
static int ICM4N607IntStatus(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                             void *inBuf, uint8_t inSize, uint8_t elemInSize,
                             void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    inv_icm4n607_read_reg(ICM4N607_INT_STATUS, 1, &mTask.regBuffer,0);

    //check pwr status again after reset
    inv_icm4n607_read_reg(ICM4N607_PWR_MGMT_0, 1, &mTask.statusBuffer,0);
    inv_icm4n607_read_reg(ICM4N607_GYRO_CONFIG0, 1, &mTask.statusBuffer2,0);
	inv_icm4n607_read_reg(ICM4N607_ACCEL_CONFIG0, 1, &mTask.statusBuffer3,0);

    //check intf_config0
    inv_icm4n607_read_reg(ICM4N607_INTF_CONFIG0, 1, &mTask.fifostatus,0);

    //check top1 fifo config5
    inv_icm4n607_read_reg(ICM4N607_FIFO_CONFIG5_MREG_TOP1, 1, &mTask.fifoconfig5,0);
    inv_icm4n607_read_reg(ICM4N607_INT_SOURCE0,1, &mTask.int1status,0);

    //check wom config
    inv_icm4n607_read_reg(ICM4N607_WOM_CONFIG, 1, &mTask.wakeupBuffer,0);
    inv_icm4n607_read_reg(ICM4N607_INT_SOURCE1,1, &mTask.statusBuffer4,0);

    //check FIFO rcosc config
    inv_icm4n607_read_reg(ICM4N607_FIFO_CONFIG6_MREG_TOP1, 1, &mTask.statusBuffer5,0);
    
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

static int ICM4N607ResetCheck(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                              void *inBuf, uint8_t inSize, uint8_t elemInSize,
                              void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    if (ICM4N607_INT_STATUS_INT_STATUS_RESET_DONE_MASK & mTask.regBuffer[1])
        INVLOG(LOG_INFO, "ICM4N607ResetCheck Done pwr %x\n",mTask.statusBuffer[1]);
    else
        INVLOG(LOG_ERROR, "ICM4N607ResetCheck Fail 0x%x\n", mTask.regBuffer[1]);

    //GET default PWR reg value ,gyro&acc default config value
    mTask.pwr_sta   = mTask.statusBuffer[1];
    mTask.gyro_cfg0 = mTask.statusBuffer2[1];
    mTask.acc_cfg0  = mTask.statusBuffer3[1];
    mTask.intf_cfg0 = mTask.fifostatus[1];
    mTask.int_src0  = mTask.int1status[1];
    mTask.wom_cfg   = mTask.wakeupBuffer[1];
    mTask.int_src1  = mTask.statusBuffer4[1];
    mTask.fifo_rcosc= mTask.statusBuffer5[1];

    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int ICM4N607ResetRead_SpiStatus(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                             void *inBuf, uint8_t inSize, uint8_t elemInSize,
                             void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    inv_icm4n607_read_reg(ICM4N607_CHIP_CONFIG_REG, 1, &mTask.regBuffer,0);
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

#if SUPPORT_SELFTEST 

static int icm4n607_Selftest_Recover(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                     void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint8_t odr = 0;
    uint8_t regValue = 0x00;
    uint32_t sampleRate = 0;
    uint32_t maxRate = 0;

    INVLOG(LOG_INFO, "icm4n607_Selftest_Recover\n");

    if(false == mTask.sensors[ACC].powered && 
       false == mTask.sensors[GYR].powered &&
       false == mTask.sensors[ANYMO].powered){

       INVLOG(LOG_INFO, "no sensor opened before return \n");
       sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
       return 0;
    
       }
     
    //1st reopen sensor 

    if(true == mTask.sensors[ACC].powered){
        inv_icm4n607_switch_on_mclk_nodelay();
    
        mTask.pwr_sta &= ~ICM4N607_PWR_MGMT_0_ACCEL_MODE_MASK;
        mTask.pwr_sta |= ICM4N607_PWR_MGMT_0_ACCEL_MODE_LN;

        // set 20ms for accel start-up time
        inv_icm4n607_write_reg(ICM4N607_PWR_MGMT_0,mTask.pwr_sta, 20000);
    }

    if(true == mTask.sensors[GYR].powered){
    
       inv_icm4n607_switch_on_mclk_nodelay();

       mTask.pwr_sta &= ~ICM4N607_PWR_MGMT_0_GYRO_MODE_MASK;
	   mTask.pwr_sta |= (uint8_t)ICM4N607_PWR_MGMT_0_GYRO_MODE_LN;
   
       // set 80ms for gyro start-up time, note:at least 200us here to sync fly changes

       inv_icm4n607_write_reg(ICM4N607_PWR_MGMT_0, mTask.pwr_sta, GYRO_TURNON_TIME);
       INVLOG(LOG_INFO, "GYRO turn on delay %d,pwr_sta %x\n",GYRO_TURNON_TIME,mTask.pwr_sta);
     
    }

    if(true == mTask.sensors[ACC].configed || true == mTask.sensors[GYR].configed){
    //2nd recover odr 
        INVLOG(LOG_INFO, "AcchwRate %ld GyrhwRate %ld\n", mTask.sensors[ACC].hwRate, mTask.sensors[GYR].hwRate);
        if(true == mTask.sensors[ACC].configed && true == mTask.sensors[GYR].configed) {
        	INVLOG(LOG_INFO, "recover sensor A&G application\n");
	        maxRate = max( mTask.sensors[ACC].hwRate, mTask.sensors[GYR].hwRate);
	        mTask.sensors[ACC].hwRate = maxRate;
	        mTask.sensors[GYR].hwRate = maxRate;  
	        #if ICM4N607_MAX_ODR_800HZ
            mTask.sensors[ACC].samplediv =  mTask.sensors[ACC].fifo_rate /mTask.sensors[ACC].preRealRate;
            mTask.sensors[ACC].sampledivcounter = 1;
            mTask.sensors[GYR].samplediv =  mTask.sensors[GYR].fifo_rate /mTask.sensors[GYR].preRealRate;
            mTask.sensors[GYR].sampledivcounter = 1;
            #endif
    	}
		else if(true == mTask.sensors[ACC].configed)
		{
			INVLOG(LOG_INFO, "recover sensor Acc application\n");
			maxRate = mTask.sensors[ACC].hwRate;
			#if ICM4N607_MAX_ODR_800HZ
            mTask.sensors[ACC].samplediv =  mTask.sensors[ACC].fifo_rate /mTask.sensors[ACC].preRealRate;
            mTask.sensors[ACC].sampledivcounter = 1;
            #endif
		}
		else if(true == mTask.sensors[GYR].configed)
		{
			INVLOG(LOG_INFO, "recover sensor Gyro application\n");
			maxRate = mTask.sensors[GYR].hwRate;
			#if ICM4N607_MAX_ODR_800HZ
            mTask.sensors[GYR].samplediv =  mTask.sensors[GYR].fifo_rate /mTask.sensors[GYR].preRealRate;
            mTask.sensors[GYR].sampledivcounter = 1;
            #endif
		} 

        odr = ICM4N607CalcuOdr(&maxRate, &sampleRate);
        regValue = ICM4N607_ODR_MAPPING[odr];
        mTask.gyro_cfg0 = (mTask.gyro_cfg0 & 0xF0) | regValue;
        inv_icm4n607_write_reg(ICM4N607_GYRO_CONFIG0, mTask.gyro_cfg0,0);
            
        mTask.acc_cfg0 = (mTask.acc_cfg0 & 0xF0) | regValue;
        inv_icm4n607_write_reg(ICM4N607_ACCEL_CONFIG0, mTask.acc_cfg0,0);
        
    // 3rd config fifo again
    #if !ICM4N607_MAX_ODR_800HZ
        registerAccGyroFifoInfo((mTask.sensors[ACC].hwRate == 0) ? 0 : 1024000000000 / mTask.sensors[ACC].hwRate,
                            (mTask.sensors[GYR].hwRate == 0) ? 0 : 1024000000000 / mTask.sensors[GYR].hwRate);
    #else
        registerAccGyroFifoInfo((mTask.sensors[ACC].hwRate == 0) ? 0 : 1024000000000 / mTask.sensors[ACC].preRealRate,
                            (mTask.sensors[GYR].hwRate == 0) ? 0 : 1024000000000 / mTask.sensors[GYR].preRealRate);
    #endif
        mTask.watermark = ICM4N607CalcuWm();
        ICM4N607ConfigFifo(true);

        first_data = 1;
        isrwm_after_set_rate = false;
    }

    // 4th reconfig anymo
    if(true == mTask.sensors[ANYMO].powered){
    
         // if acc is not in streaming mode & gyro is not on, set WOM in low power mode
        #if SUPPORT_HW_FREEFALL
        if ( !mTask.sensors[ACC].powered && !mTask.sensors[GYR].powered && !mTask.sensors[FREEFALL].powered) {
        #else
        if ( !mTask.sensors[ACC].powered && !mTask.sensors[GYR].powered ) {
        #endif
           ICM4N607_set_acc_power_mode(BIT_ACC_LP_MODE);
        } else {
           ICM4N607_set_acc_power_mode(BIT_ACC_LN_MODE);  
        }
    
        // enable wom
        mTask.wom_cfg &= ~ICM4N607_WOM_CONFIG_WOM_EN_MASK;
	    mTask.wom_cfg |= (uint8_t)ICM4N607_WOM_CONFIG_WOM_EN_ENABLE;
        inv_icm4n607_write_reg(ICM4N607_WOM_CONFIG, mTask.wom_cfg,0);

        mTask.int_src1 &= ~(ICM4N607_INT_SOURCE1_INT_WOM_X_INT1_EN_MASK 
			         | ICM4N607_INT_SOURCE1_INT_WOM_Y_INT1_EN_MASK 
			         | ICM4N607_INT_SOURCE1_INT_WOM_Z_INT1_EN_MASK);
			   
        mTask.int_src1 |= ICM4N607_INT_SOURCE1_INT_WOM_X_INT1_EN_MASK
                     |ICM4N607_INT_SOURCE1_INT_WOM_Y_INT1_EN_MASK
                     |ICM4N607_INT_SOURCE1_INT_WOM_Z_INT1_EN_MASK;

        inv_icm4n607_write_reg(ICM4N607_INT_SOURCE1, mTask.int_src1,0);
    }

   return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

#endif

static int ICM4N607InitConfig(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                              void *inBuf, uint8_t inSize, uint8_t elemInSize,
                              void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint8_t data = mTask.fifoconfig5[1];
    
	INVLOG(LOG_INFO, "ICM4N607InitConfig\n");
    
    /* set BANK1 bus setting */

#if SPI_SLEW_RATE_CHANGE
    inv_icm4n607_write_reg(ICM4N607_DRIVE_CONFIG_REG2, 0x0b,0);
    inv_icm4n607_write_reg(ICM4N607_DRIVE_CONFIG_REG3, 0x03,0);
#endif

    /* set INT1 push-pull, active high,pulse mode */
    inv_icm4n607_write_reg(ICM4N607_INT_CONFIG_REG, (uint8_t)ICM4N607_INT_CONFIG_INT1_DRIVE_CIRCUIT_PP |ICM4N607_INT_CONFIG_INT1_POLARITY_HIGH,0);

    /* gyro odr & fs */
    mTask.gyro_cfg0 |=  ICM4N607_GYRO_CONFIG0_FS_SEL_2000dps;
    inv_icm4n607_write_reg(ICM4N607_GYRO_CONFIG0, mTask.gyro_cfg0, 0); 
    /* acc odr & fs */
    mTask.acc_cfg0  |=  ICM4N607_ACCEL_CONFIG0_FS_SEL_8g;
    inv_icm4n607_write_reg(ICM4N607_FIFO_CONFIG1, mTask.acc_cfg0, 0);

    /*force to set WU oc clok */
    /*mTask.pwr_sta &= (~0x80);
      inv_icm4n607_write_reg(ICM4N607_PWR_MGMT_0, mTask.pwr_sta,0);  */

    /* bypass fifo */
    inv_icm4n607_write_reg(ICM4N607_FIFO_CONFIG1, ICM4N607_FIFO_CONFIG1_FIFO_BYPASS_ON,0);

    inv_icm4n607_switch_on_mclk();

    mTask.intf_cfg0 &= ~(ICM4N607_INTF_CONFIG0_FIFO_COUNT_FORMAT_MASK | ICM4N607_INTF_CONFIG0_FIFO_COUNT_ENDIAN_MASK | ICM4N607_INTF_CONFIG0_SENSOR_DATA_ENDIAN_MASK );

    /* en byte mode & little endian FIFO &  SENSOR DATA mode & disable i2c interface */
    mTask.intf_cfg0 |= ((uint8_t)ICM4N607_INTF_CONFIG0_FIFO_COUNT_REC_BYTE 
                    |(uint8_t)ICM4N607_INTF_CONFIG0_FIFO_COUNT_LITTLE_ENDIAN | ICM4N607_INTF_CONFIG0_DATA_LITTLE_ENDIAN | 0X03 );
    inv_icm4n607_write_reg(ICM4N607_INTF_CONFIG0,mTask.intf_cfg0, 0);

    /* fifo mode, by pass on */
    inv_icm4n607_write_reg(ICM4N607_FIFO_CONFIG1,(uint8_t)ICM4N607_FIFO_CONFIG1_FIFO_MODE_SNAPSHOT | (uint8_t)ICM4N607_FIFO_CONFIG1_FIFO_BYPASS_ON,0 );

    /* config fifo package 16byte fifo*/
    data &= ~(ICM4N607_FIFO_CONFIG5_FIFO_GYRO_EN_MASK | ICM4N607_FIFO_CONFIG5_FIFO_ACCEL_EN_MASK
		    | ICM4N607_FIFO_CONFIG5_FIFO_TMST_FSYNC_EN_MASK | ICM4N607_FIFO_CONFIG5_FIFO_HIRES_EN_MASK);
	data |= ((uint8_t)ICM4N607_FIFO_CONFIG5_GYRO_EN | (uint8_t)ICM4N607_FIFO_CONFIG5_ACCEL_EN 
	        | (uint8_t)ICM4N607_FIFO_CONFIG5_TMST_FSYNC_DIS | (uint8_t)ICM4N607_FIFO_CONFIG5_WM_GT_TH_EN);
    inv_icm4n607_write_reg(ICM4N607_FIFO_CONFIG5_MREG_TOP1, data,0);
			     
    /* enable fifo full & WM INT */

    mTask.int_src0 &= ~(ICM4N607_INT_SOURCE0_INT_FSYNC_INT1_EN_MASK 
			   | ICM4N607_INT_SOURCE0_INT_DRDY_INT1_EN_MASK 
			   | ICM4N607_INT_SOURCE0_INT_FIFO_THS_INT1_EN_MASK 
			   | ICM4N607_INT_SOURCE0_INT_FIFO_FULL_INT1_EN_MASK);
    mTask.int_src0 |= (ICM4N607_INT_SOURCE0_INT_FIFO_THS_INT1_EN_MASK | ICM4N607_INT_SOURCE0_INT_FIFO_FULL_INT1_EN_MASK);
    inv_icm4n607_write_reg(ICM4N607_INT_SOURCE0,  mTask.int_src0,0);

    inv_imu_set_autorcosc_power_on(false);
    
    inv_icm4n607_switch_off_mclk();

    inv_icm4n607_configure_wom(ICM4N607_DEFAULT_WOM_THS_MG, ICM4N607_DEFAULT_WOM_THS_MG,ICM4N607_DEFAULT_WOM_THS_MG, 
	    ICM4N607_WOM_CONFIG_WOM_INT_MODE_ORED,ICM4N607_WOM_CONFIG_WOM_INT_DUR_1_SMPL);
      
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

#if defined(CFG_HW_STEP_COUNTER_SUPPORT) || SUPPORT_HW_FREEFALL
static void icm4n607_reconfig_samplerate(uint32_t new_accrate,uint32_t new_gyrorate,uint32_t new_hwrate)
{
    int odr = 0;
    uint8_t regValue = 0x00;
    uint32_t sampleRate = 0;
    uint32_t maxRate = 0;

    
    if(true == mTask.sensors[ACC].configed || true == mTask.sensors[GYR].configed){
        //1st set odr 

        INVLOG(LOG_INFO, "new AcchwRate %ld GyrhwRate %ld\n", new_accrate,new_gyrorate);
        if(true == mTask.sensors[ACC].configed && true == mTask.sensors[GYR].configed) {
        	INVLOG(LOG_INFO, "recover sensor A&G application\n");
	        maxRate = max(new_accrate,new_gyrorate);  
	        mTask.sensors[ACC].fifo_rate = maxRate;
	        mTask.sensors[GYR].fifo_rate = maxRate;  
	        mTask.sensors[ACC].hwRate = new_hwrate;
	        mTask.sensors[GYR].hwRate = new_hwrate;  
#if ICM4N607_MAX_ODR_800HZ
            mTask.sensors[ACC].samplediv =  mTask.sensors[ACC].fifo_rate /mTask.sensors[ACC].preRealRate;
            mTask.sensors[ACC].sampledivcounter = 1;
            mTask.sensors[GYR].samplediv =  mTask.sensors[GYR].fifo_rate /mTask.sensors[GYR].preRealRate;
            mTask.sensors[GYR].sampledivcounter = 1;
#endif
    	}
		else if(true == mTask.sensors[ACC].configed)
		{
			INVLOG(LOG_INFO, "recover sensor Acc application\n");
			mTask.sensors[ACC].fifo_rate = new_accrate;
			mTask.sensors[ACC].hwRate = new_hwrate;
			maxRate = mTask.sensors[ACC].hwRate;
		#if ICM4N607_MAX_ODR_800HZ
            mTask.sensors[ACC].samplediv =  mTask.sensors[ACC].fifo_rate /mTask.sensors[ACC].preRealRate;
            mTask.sensors[ACC].sampledivcounter = 1;
        #endif
		}
		else if(true == mTask.sensors[GYR].configed)
		{
			INVLOG(LOG_INFO, "recover sensor Gyro application\n");
			mTask.sensors[GYR].fifo_rate = new_gyrorate;
			mTask.sensors[GYR].hwRate = new_hwrate;
			maxRate = mTask.sensors[GYR].hwRate;
		#if ICM4N607_MAX_ODR_800HZ
            mTask.sensors[GYR].samplediv =  mTask.sensors[GYR].fifo_rate /mTask.sensors[GYR].preRealRate;
            mTask.sensors[GYR].sampledivcounter = 1;
        #endif
		} 
    	
        odr = ICM4N607CalcuOdr(&new_hwrate, &sampleRate);
        regValue = ICM4N607_ODR_MAPPING[odr];
        mTask.gyro_cfg0 = (mTask.gyro_cfg0 & 0xF0) | regValue;
        inv_icm4n607_write_reg(ICM4N607_GYRO_CONFIG0, mTask.gyro_cfg0,0);
            
        mTask.acc_cfg0 = (mTask.acc_cfg0 & 0xF0) | regValue;
        inv_icm4n607_write_reg(ICM4N607_ACCEL_CONFIG0, mTask.acc_cfg0,0);
        
    // 3st config fifo again
    #if !ICM4N607_MAX_ODR_800HZ
        registerAccGyroFifoInfo((mTask.sensors[ACC].fifo_rate == 0) ? 0 : 1024000000000 / mTask.sensors[ACC].fifo_rate,
                            (mTask.sensors[GYR].fifo_rate == 0) ? 0 : 1024000000000 / mTask.sensors[GYR].fifo_rate);
    #else
        registerAccGyroFifoInfo((mTask.sensors[ACC].fifo_rate == 0) ? 0 : 1024000000000 / mTask.sensors[ACC].fifo_rate,
                            (mTask.sensors[GYR].fifo_rate == 0) ? 0 : 1024000000000 / mTask.sensors[GYR].fifo_rate);
    #endif

        mTask.watermark = ICM4N607CalcuWm();
        ICM4N607ConfigFifo(true);
        
        first_data = 1;
        isrwm_after_set_rate = false;
    }

}
#endif

static void ICM4N607ResetFifo()
{
    inv_icm4n607_switch_on_mclk();
    // reset fifo
    /* fifo mode, set by pass on */
    inv_icm4n607_write_reg(ICM4N607_FIFO_CONFIG1,(uint8_t)ICM4N607_FIFO_CONFIG1_FIFO_MODE_SNAPSHOT | (uint8_t)ICM4N607_FIFO_CONFIG1_FIFO_BYPASS_ON,0 );
     /* flush fifo */
    inv_icm4n607_write_reg(ICM4N607_SIGNAL_PATH_RESET,(uint8_t)ICM4N607_SIGNAL_PATH_RESET_FIFO_FLUSH_EN,0 );

     /* set fifo stream mode */
    inv_icm4n607_write_reg(ICM4N607_FIFO_CONFIG1,(uint8_t)ICM4N607_FIFO_CONFIG1_FIFO_MODE_SNAPSHOT | (uint8_t)ICM4N607_FIFO_CONFIG1_FIFO_BYPASS_OFF,0 );

    inv_icm4n607_switch_off_mclk();
}
        
static void ICM4N607ConfigFifo(bool odr_change)
{
    uint8_t  buffer[2];
    uint16_t watermarkReg;

    watermarkReg = mTask.watermark;

    /* default fifo byte mode*/
    if (watermarkReg < 16)
        watermarkReg = 16;

	INVLOG(LOG_INFO, "ICM4N607 watermarkReg %d mTask.fdr_sel %d\n ",watermarkReg,mTask.fdr_sel);
	
    buffer[0] = watermarkReg & 0x00ff;
    buffer[1] = (watermarkReg & 0xff00) >> 8;

    inv_icm4n607_switch_on_mclk();
    
    if (odr_change) {
         /* fifo mode, set by pass on */
        inv_icm4n607_write_reg(ICM4N607_FIFO_CONFIG1,(uint8_t)ICM4N607_FIFO_CONFIG1_FIFO_MODE_SNAPSHOT | (uint8_t)ICM4N607_FIFO_CONFIG1_FIFO_BYPASS_ON,0 );
        /* flush fifo */
        inv_icm4n607_write_reg(ICM4N607_SIGNAL_PATH_RESET,(uint8_t)ICM4N607_SIGNAL_PATH_RESET_FIFO_FLUSH_EN,0 );
        
        /* set threshold */
        inv_icm4n607_write_reg(ICM4N607_FIFO_CONFIG2, buffer[0],0);
        inv_icm4n607_write_reg(ICM4N607_FIFO_CONFIG3, buffer[1],0);
        
        if (mTask.sensors[ACC].configed || mTask.sensors[GYR].configed) {
            /* set fifo stream mode */
           inv_icm4n607_write_reg(ICM4N607_FIFO_CONFIG1,(uint8_t)ICM4N607_FIFO_CONFIG1_FIFO_MODE_SNAPSHOT | (uint8_t)ICM4N607_FIFO_CONFIG1_FIFO_BYPASS_OFF,0 );
        }
        INVLOG(LOG_INFO, "ICM4N607ConfigFifo: Reset, TH_L:0x%x, TH_H:0x%x\n",
            buffer[0], buffer[1]);
    } else {
        inv_icm4n607_write_reg(ICM4N607_FIFO_CONFIG2, buffer[0],0);
        inv_icm4n607_write_reg(ICM4N607_FIFO_CONFIG3, buffer[1],0);
        
        if (mTask.sensors[ACC].configed || mTask.sensors[GYR].configed) {
            inv_icm4n607_write_reg(ICM4N607_FIFO_CONFIG1,(uint8_t)ICM4N607_FIFO_CONFIG1_FIFO_MODE_SNAPSHOT | (uint8_t)ICM4N607_FIFO_CONFIG1_FIFO_BYPASS_OFF,0 );
        } else {
            inv_icm4n607_write_reg(ICM4N607_FIFO_CONFIG1,(uint8_t)ICM4N607_FIFO_CONFIG1_FIFO_MODE_SNAPSHOT | (uint8_t)ICM4N607_FIFO_CONFIG1_FIFO_BYPASS_ON,0 );
        }
        INVLOG(LOG_INFO, "ICM4N607ConfigFifo, TH_L:0x%x, TH_H:0x%x\n",
            buffer[0], buffer[1]);

        /*to do if necessary , add fifo disable for gyro& accel in ICM4N607_FIFO_CONFIG5_MREG_TOP1 */
    }

    inv_icm4n607_switch_off_mclk();
}

static uint16_t ICM4N607CalcuWm(void)
{
    uint8_t handle;
    uint64_t min_latency = SENSOR_LATENCY_NODATA;
    uint8_t min_watermark = 1;
    uint8_t max_watermark ;
    uint16_t watermark = 0;
    uint32_t temp_cnt, total_cnt = 0;
    uint64_t temp_delay = 0;

    //for yokohama the max fifo size is 2000byte, max fifo pakage is 20bit, so the max fifo pakage is 100
    max_watermark = 70 < (MAX_RECV_PACKET/2) ? 70 : (MAX_RECV_PACKET/2); /*60*/
    for (handle = ACC; handle <= GYR; handle++) {
        if (mTask.sensors[handle].fifo_rate && mTask.sensors[handle].latency != SENSOR_LATENCY_NODATA) {
            min_latency =
                mTask.sensors[handle].latency < min_latency ? mTask.sensors[handle].latency : min_latency;
        }
    }
    /*if acc and gyr off or acc and gyr latency = SENSOR_LATENCY_NODATA, watermark = 0 or 1 or 2*/
    if (min_latency == SENSOR_LATENCY_NODATA) {
        for (handle = ACC; handle <= GYR; handle++) {
            if (mTask.sensors[handle].fifo_rate) {
                watermark++;
            }
        }
    } else {
        for (handle = ACC; handle <= GYR; handle++) {
            if (mTask.sensors[handle].fifo_rate) {
                temp_delay = 1024000000000 / mTask.sensors[handle].fifo_rate;
                temp_cnt = min_latency / temp_delay;
                if(0 == temp_cnt)
                    temp_cnt = 1;
                min_watermark = mTask.sensors[handle].fifo_rate /SENSOR_HZ(400.0f);
                INVLOG(LOG_INFO, "ICM4N607CalcuWm, fifo_rate %d,min_watermark %d,temp_cnt %d,min_latency %lld,temp_delay %lld\n",
                     mTask.sensors[handle].fifo_rate, min_watermark,temp_cnt,min_latency,temp_delay);
                total_cnt = temp_cnt > min_watermark ? temp_cnt : min_watermark;
            }
        }

        watermark = total_cnt;
        INVLOG(LOG_INFO, "ICM4N607CalcuWm, watermark %d,min_watermark %d,total_cnt %d\n",
                     watermark, min_watermark,total_cnt);
#if ICM4N607_MAX_ODR_800HZ
        uint8_t decimation_div;
         // in acc and gyro both streaming case, we will do another water mark caculation here to get correct timestamp decimation result
        if( mTask.sensors[ACC].configed == true && mTask.sensors[GYR].configed == true){
            if (mTask.sensors[ACC].samplediv >= mTask.sensors[GYR].samplediv)
                decimation_div = mTask.sensors[ACC].samplediv / mTask.sensors[GYR].samplediv;
            else
                decimation_div = mTask.sensors[GYR].samplediv / mTask.sensors[ACC].samplediv;

            if(watermark < decimation_div)
                watermark = decimation_div;
            else
                watermark = (watermark /decimation_div) * decimation_div;
                INVLOG(LOG_INFO, "ICM4N607CalcuWm Fn watermark %d,ddiv %d, accdiv %d, gyrodiv %d\n",watermark,decimation_div,mTask.sensors[ACC].samplediv, mTask.sensors[GYR].samplediv );
        }
#endif 

         watermark = watermark < min_watermark ? min_watermark : watermark;
         watermark = watermark > max_watermark ? max_watermark : watermark;
    }
    return watermark * 16;/* byte mode */
}

#if SUPPORT_SELFTEST

static void spi_selftest_status_CallBack(void *cookie, int err)
{

    static uint16_t i = 0;
    uint8_t st_done = mTask.regBuffer[1];

    if(ICM4N607_SELFTEST_DIS == mTask.st_control) {
		/* Nothing else required if self-test is not being run */ 
		INVLOG(LOG_ERROR, "ICM4N607Check_selftest_result invalid st_control value %x !!!!\n",mTask.st_control); 
	}
    #if DEBUG_SELFTEST
	INVLOG(LOG_INFO, "R ICM4N607_INT_STATUS %x %x\n",st_done);
    #endif

	st_done &= ICM4N607_INT_STATUS_INT_STATUS_ST_DONE_MASK; 
	
    if(!st_done && i<5000){
    #if DEBUG_SELFTEST
        INVLOG(LOG_ERROR, "continue to read int_status %i!!\n",mTask.regBuffer[1],i);
    #endif
        inv_icm4n607_read_reg(ICM4N607_INT_STATUS, 1, &mTask.regBuffer,1000);

        i++;
        spiBatchTxRx(&mTask.mode, spi_selftest_status_CallBack, cookie, __FUNCTION__);
      
    }
    else{
        if(i>=5000)
             INVLOG(LOG_ERROR, "selftest could not finish because %d\n", i);
        //reopen software interrupt mask    
        mt_eint_unmask(mTask.hw->eint_num);    
        //check selftest result reg
        inv_icm4n607_read_reg(ICM4N607_ST_STATUS1_MREG_TOP1, 1, &mTask.statusBuffer,0);
        inv_icm4n607_read_reg(ICM4N607_ST_STATUS2_MREG_TOP1, 1, &mTask.statusBuffer2,0);

        spiBatchTxRx(&mTask.mode, mTask.spiCallBack, cookie, __FUNCTION__);
    }
}

static int ICM4N607Selftest_Reset(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;

    // Disable software interrupt response in selftest
    mt_eint_mask(mTask.hw->eint_num);

    INVLOG(LOG_INFO, "ICM4N607Selftest_Reset power %d,mclk cnt %d\n", mTask.sensors[ACC].powered,mTask.need_mclk_cnt);

    /* turn off previous mclk on if any */
    inv_icm4n607_switch_off_mclk();
        /* turn off acc & gyro */
    mTask.pwr_sta &= 0xF0;
    inv_icm4n607_write_reg(ICM4N607_PWR_MGMT_0, mTask.pwr_sta, 200);
    INVLOG(LOG_INFO, "ICM4N607Selftest_Reset pwr_sta %x \n", mTask.pwr_sta);

    inv_icm4n607_switch_on_mclk();
   // Reset DMP internal memories
    icm4n607_sc_DMP_mem_reset(); 
    mTask.DMP_first_config = true;

    inv_icm4n607_read_reg(ICM4N607_OTP_CONFIG_MREG_TOP1, 1, &mTask.statusBuffer2,0);
    inv_icm4n607_read_reg(ICM4N607_OTP_CTRL7_MREG_OTP, 1, &mTask.statusBuffer3,0);
    
    ret = spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);

    return ret;
}


static int ICM4N607Selftest_LoadOTP_1(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;
    uint8_t value = mTask.statusBuffer2[1];
    
    INVLOG(LOG_INFO, "ICM4N607Selftest_LoadOTP apex_config0 %x,otp_ctrl7 %x \n",mTask.stepstatusBuffer[1],mTask.statusBuffer3[1]);

     //re-asign apex_config0 value
    mTask.apex_config0 = mTask.stepstatusBuffer[1];
    mTask.otp_ctrl7    = mTask.statusBuffer3[1];
        
    if((mTask.apex_config0 & ICM4N607_APEX_CONFIG0_DMP_SRAM_RESET_EN_MASK) != ICM4N607_APEX_CONFIG0_DMP_SRAM_RESET_DIS){
        INVLOG(LOG_ERROR, "DMP may do not reset correctly !!!!\n");    
    }
       
    // Trigger OTP load for ST data
	value &= ~ICM4N607_OTP_CONFIG_OTP_COPY_MODE_MASK;
	value |= (uint8_t)ICM4N607_OTP_CONFIG_OTP_COPY_ST_DATA;
	inv_icm4n607_write_reg(ICM4N607_OTP_CONFIG_MREG_TOP1, value,0);
    INVLOG(LOG_INFO, "otp 0x2b ICM4N607_OTP_CONFIG_MREG_TOP1 %x \n", value);

	mTask.otp_ctrl7  &= ~ICM4N607_OTP_CTRL7_OTP_PWR_DOWN_MASK;
	mTask.otp_ctrl7  |= (uint8_t)ICM4N607_OTP_CTRL7_PWR_DOWN_DIS;
	inv_icm4n607_write_reg(ICM4N607_OTP_CTRL7_MREG_OTP, mTask.otp_ctrl7 ,100);
	INVLOG(LOG_INFO, "otp 0x2806 ICM4N607_OTP_CTRL7_MREG_OTP pwr dis %x \n", mTask.otp_ctrl7);

    mTask.otp_ctrl7 &= ~ICM4N607_OTP_CTRL7_OTP_RELOAD_MASK;
	mTask.otp_ctrl7 |= (uint8_t)ICM4N607_OTP_CTRL7_OTP_RELOAD_EN;
	inv_icm4n607_write_reg(ICM4N607_OTP_CTRL7_MREG_OTP, mTask.otp_ctrl7,50);
	INVLOG(LOG_INFO, "otp 0x2806 ICM4N607_OTP_CTRL7_MREG_OTP en %x \n", mTask.otp_ctrl7);

    // read to check ICM4N607_OTP_CTRL7_MREG_OTP reg again
    inv_icm4n607_read_reg(ICM4N607_OTP_CTRL7_MREG_OTP, 1, &mTask.statusBuffer3,0);
    inv_icm4n607_read_reg(ICM4N607_MISC_1, 1, &mTask.statusBuffer4,0);
    
    ret = spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);

    return ret;
}

static int ICM4N607Selftest_LoadOTP_2(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;
    uint8_t misc_otp_value = mTask.statusBuffer4[1];
    ICM4N607_OTP_CTRL7_OTP_RELOAD_t otp_reload;
    
    mTask.otp_ctrl7    = mTask.statusBuffer3[1];
    otp_reload =  (ICM4N607_OTP_CTRL7_OTP_RELOAD_t)(mTask.otp_ctrl7 & ICM4N607_OTP_CTRL7_OTP_RELOAD_MASK);

	INVLOG(LOG_INFO, "otp R ICM4N607_OTP_CTRL7_MREG_OTP %x \n", mTask.otp_ctrl7);
	
	misc_otp_value &= ICM4N607_MISC_1_OTP_DONE_MASK;
	if((misc_otp_value != ICM4N607_MISC_1_OTP_DONE_MASK) || (otp_reload != ICM4N607_OTP_CTRL7_OTP_RELOAD_DIS)){
        INVLOG(LOG_ERROR, "misc_otp_value %x , otp_reload %x  not right !!!!\n",misc_otp_value,otp_reload);    
		ret = 1;
	}

    // only enable selftest interuppt 
	inv_icm4n607_write_reg(ICM4N607_INT_SOURCE0, ICM4N607_INT_SOURCE0_INT_ST_DONE_INT1_EN_MASK, 0);

    //check selftest status
   	inv_icm4n607_read_reg(ICM4N607_SELFTEST_MREG_TOP1, 1, &mTask.statusBuffer,0);
    inv_icm4n607_read_reg(ICM4N607_ST_CONFIG_MREG_TOP1,1, &mTask.statusBuffer2,0); 
    
    ret += spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);

    return ret;
}

static int ICM4N607Selftest_configure_parameters(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;
    uint8_t value = mTask.statusBuffer[1];
    uint8_t st_config_mreg_value = mTask.statusBuffer2[1];

    uint8_t st_num_samples = ICM4N607_ST_CONFIG_16_SAMPLES;
	uint8_t accel_limit = ICM4N607_ST_CONFIG_ACCEL_ST_LIM_50;
	uint8_t gyro_limit = ICM4N607_ST_CONFIG_GYRO_ST_LIM_50;

	mTask.st_control = ICM4N607_SELFTEST_EN;

    /* Self-test configuration cannot be updated if it already running */
    if((value & ICM4N607_SELFTEST_EN) != ICM4N607_SELFTEST_DIS){ 
        INVLOG(LOG_INFO, "Self-test config cannot be updated already running %x \n", value);
		ret = 1;
    }
    
    st_config_mreg_value &= ~((uint8_t)ICM4N607_ST_CONFIG_ST_NUM_SAMPLE_MASK | (uint8_t)ICM4N607_ST_CONFIG_ACCEL_ST_LIM_MASK | (uint8_t)ICM4N607_ST_CONFIG_GYRO_ST_LIM_MASK);
	st_config_mreg_value |= (uint8_t)st_num_samples | (uint8_t)accel_limit | (uint8_t)gyro_limit;
    inv_icm4n607_write_reg(ICM4N607_ST_CONFIG_MREG_TOP1, st_config_mreg_value,0);
    INVLOG(LOG_INFO, "W ICM4N607_ST_CONFIG_MREG_TOP1 %x \n", st_config_mreg_value);

    inv_icm4n607_read_reg(ICM4N607_SELFTEST_MREG_TOP1, 1, &mTask.statusBuffer3,0);
    
    ret += spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);

    return ret;
}

static int ICM4N607Control_selftest(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;
    uint8_t value = mTask.statusBuffer3[1];

    value &= ~ICM4N607_SELFTEST_EN;
	value |= mTask.st_control;
    mTask.selftest_mreg = value;
	
	/* Both accel and gyro MUST be enabled simulaneously for DMP to take it into account properly ,delay 100ms to wait enough selftest time*/
	inv_icm4n607_write_reg(ICM4N607_SELFTEST_MREG_TOP1,value,1000);
	INVLOG(LOG_INFO, "W ICM4N607_SELFTEST_MREG_TOP1 %x \n", mTask.selftest_mreg);

    // read 2 times of int status
    inv_icm4n607_read_reg(ICM4N607_INT_STATUS, 1, &mTask.regBuffer,0);

    mTask.spiCallBack = spiCallBack;
    ret = spiBatchTxRx(&mTask.mode, spi_selftest_status_CallBack, next_state, __FUNCTION__);

    return ret;
}

static int icm4n607Selftest_acc_result_end(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                       void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint8_t test_result = 1;

    INVLOG(LOG_INFO, "Selftest Reg, %x %x\n",mTask.statusBuffer[1],mTask.statusBuffer2[1]);

    uint8_t accel_status = (mTask.statusBuffer[1] & ICM4N607_ST_STATUS1_DMP_ACCEL_ST_PASS_MASK) >> ICM4N607_ST_STATUS1_DMP_ACCEL_ST_PASS_POS;
    uint8_t gyro_status = (mTask.statusBuffer2[1] & ICM4N607_ST_STATUS2_DMP_GYRO_ST_PASS_MASK) >> ICM4N607_ST_STATUS2_DMP_GYRO_ST_PASS_POS;
	gyro_status |= ((mTask.statusBuffer[2] & ICM4N607_ST_STATUS2_DMP_ST_INCOMPLETE_MASK) >> ICM4N607_ST_STATUS2_DMP_ST_INCOMPLETE_POS) << 1;
	
    if (gyro_status == 0x1){
		INVLOG(LOG_INFO, "Gyro Selftest PASS\n");
    }
	else
		INVLOG(LOG_INFO, "Gyro Selftest FAIL\n");

	if (accel_status == 0x1){
        test_result = 1;
        INVLOG(LOG_INFO, "AccGetAccSelfvalue: %d\n", test_result);
        accGyroSendTestResult(SENS_TYPE_ACCEL, test_result); // 0 fail, 1 success, and you could define by yoursel     
	}   
	else
	    INVLOG(LOG_ERROR, "Accel Selftest FAIL\n");
	    
    mTask.selftest_mreg &= ~ICM4N607_SELFTEST_EN;
	mTask.selftest_mreg |= (uint8_t)ICM4N607_SELFTEST_DIS;
	inv_icm4n607_write_reg(ICM4N607_SELFTEST_MREG_TOP1, mTask.selftest_mreg,0);
	
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);

}

static int icm4n607Selftest_gyro_result_end(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                       void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint8_t test_result = 1;
    INVLOG(LOG_INFO, "Selftest Reg, %x %x\n",mTask.statusBuffer[1],mTask.statusBuffer2[1]);

    uint8_t accel_status = (mTask.statusBuffer[1] & ICM4N607_ST_STATUS1_DMP_ACCEL_ST_PASS_MASK) >> ICM4N607_ST_STATUS1_DMP_ACCEL_ST_PASS_POS;
    uint8_t gyro_status = (mTask.statusBuffer2[1] & ICM4N607_ST_STATUS2_DMP_GYRO_ST_PASS_MASK) >> ICM4N607_ST_STATUS2_DMP_GYRO_ST_PASS_POS;
	gyro_status |= ((mTask.statusBuffer[2] & ICM4N607_ST_STATUS2_DMP_ST_INCOMPLETE_MASK) >> ICM4N607_ST_STATUS2_DMP_ST_INCOMPLETE_POS) << 1;

    if (gyro_status == 0x1){
		INVLOG(LOG_ERROR, "Gyro Selftest PASS\n");
		test_result = 1;
        INVLOG(LOG_INFO, "GetGYROSelfvalue: %d\n", test_result);
        accGyroSendTestResult(SENS_TYPE_GYRO, test_result); // 0 fail, 1 success, and you could define by yoursel     
    }
	else
		INVLOG(LOG_ERROR, "Gyro Selftest FAIL\n");

	if (accel_status == 0x1){
        INVLOG(LOG_INFO, "AccGetAccSelfvalue: %d\n", test_result);
	}   
	else
	    INVLOG(LOG_ERROR, "Accel Selftest FAIL\n");

    mTask.selftest_mreg &= ~ICM4N607_SELFTEST_EN;
	mTask.selftest_mreg |= (uint8_t)ICM4N607_SELFTEST_DIS;
	inv_icm4n607_write_reg(ICM4N607_SELFTEST_MREG_TOP1, mTask.selftest_mreg,0);
	
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);

}
#endif //end of SUPPORT_SELFTEST

static void ICM4N607_set_acc_power_mode(uint8_t mode)
{
    mTask.pwr_sta &= 0xFC;
    mTask.pwr_sta |= mode;
    // set 10ms for accel start-up time, note:at least 1ms here to sync fly changes
    inv_icm4n607_write_reg(ICM4N607_PWR_MGMT_0,mTask.pwr_sta, 10000);

    if(BIT_ACC_LP_MODE == mode){
            /* set odr @ 50HZ in Low power mode,low power mode only enabled in no sensor streaming mode */
        mTask.gyro_cfg0 = (mTask.gyro_cfg0 & 0xF0) | ICM4N607_GYRO_CONFIG0_ODR_50_HZ;
        inv_icm4n607_write_reg(ICM4N607_GYRO_CONFIG0, mTask.gyro_cfg0,0);
           
        mTask.acc_cfg0 &= 0xF0;
        mTask.acc_cfg0 |= ICM4N607_ACCEL_CONFIG0_ODR_50_HZ;
        inv_icm4n607_write_reg(ICM4N607_ACCEL_CONFIG0, mTask.acc_cfg0,0);

        mTask.sensors[ACC].hwRate = SENSOR_HZ(50.0f);
        //mTask.sensors[GYR].hwRate = SENSOR_HZ(50.0f);   
    }

    INVLOG(LOG_INFO, "ICM4N607 set powermode=%d , pwr %x, acc_cfg0 %x\n", mode,mTask.pwr_sta,mTask.acc_cfg0);
}


// temperly we set acc always in LN Mode when work.
static int ICM4N607AccEnable(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;

    INVLOG(LOG_INFO, "ICM4N607gEnable power mode=%d, time:%lld\n", mTask.sensors[ACC].powered, rtcGetTime());

    inv_icm4n607_switch_on_mclk_nodelay();
    
    mTask.sensors[ACC].powered = true;
    mTask.pwr_sta &= ~ICM4N607_PWR_MGMT_0_ACCEL_MODE_MASK;
    mTask.pwr_sta |= ICM4N607_PWR_MGMT_0_ACCEL_MODE_LN;

    INVLOG(LOG_INFO, "ICM4N607gEnable power mode=%d, pwr %x time:%lld\n", mTask.sensors[ACC].powered, mTask.pwr_sta,rtcGetTime());
    inv_imu_set_autorcosc_power_on(true);
    
    // set 20ms for accel start-up time
    inv_icm4n607_write_reg(ICM4N607_PWR_MGMT_0,mTask.pwr_sta, 20000);
    ret = spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);

    return ret;
}

static int ICM4N607AccDisable(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;
    uint8_t odr = 0;
    uint8_t regValue = 0x00;
    uint32_t sampleRate = 0;
    struct accGyroCntlPacket cntlPacket;
    bool accelOdrChanged = false;

    INVLOG(LOG_INFO, "ICM4N607AccPowerOff\n");

    ret = rxControlInfo(&cntlPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);
    if (ret < 0) {
        sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, ERROR_EVT);
        INVLOG(LOG_ERROR, "ICM4N607AccPowerOff, rx inSize and elemSize error\n");
        return -1;
    }
    
#if DEBUG_SENSOR_STABLE
    data_count = 0;
#endif

    mTask.sensors[ACC].rate = 0;
    mTask.sensors[ACC].preRealRate = 0;
   // mTask.sensors[ACC].hwRate = 0;
    mTask.sensors[ACC].fifo_rate = 0;
    mTask.sensors[ACC].latency = SENSOR_LATENCY_NODATA;
    mTask.sensors[ACC].needDiscardSample = false;
    mTask.sensors[ACC].samplesToDiscard = 0;
#if ICM4N607_MAX_ODR_800HZ
    mTask.sensors[ACC].samplediv = 1;
    mTask.sensors[ACC].sampledivcounter = 1;
#endif
#if defined(CFG_HW_STEP_COUNTER_SUPPORT) || SUPPORT_HW_FREEFALL
    if ((mTask.sensors[GYR].powered == false) && (mTask.sensors[ANYMO].powered == false)&&(mTask.sensors[STEPCNT].powered == false)&&(mTask.sensors[FREEFALL].powered == false) ) {
#else
    if ((mTask.sensors[GYR].powered == false) && (mTask.sensors[ANYMO].powered == false)) {
#endif
    
        mt_eint_mask(mTask.hw->eint_num);

        inv_icm4n607_switch_off_mclk();
        /* turn off acc & gyro */
        mTask.pwr_sta &= 0xF0;
        inv_icm4n607_write_reg(ICM4N607_PWR_MGMT_0, mTask.pwr_sta, 200);

        INVLOG(LOG_INFO, "all off:%x\n", mTask.pwr_sta);
        mTask.sensors[ACC].hwRate = 0;
        mTask.sensors[GYR].hwRate = 0;
        registerAccGyroFifoInfo((mTask.sensors[ACC].fifo_rate == 0) ? 0 : 1024000000000 / mTask.sensors[ACC].fifo_rate,
        (mTask.sensors[GYR].fifo_rate == 0) ? 0 : 1024000000000 / mTask.sensors[GYR].fifo_rate);
        
    } else if (mTask.sensors[GYR].powered == true) {  //  update gyro old odr
        if (mTask.sensors[GYR].fifo_rate != mTask.sensors[GYR].preRealRate) {
            mTask.sensors[GYR].fifo_rate = max(mTask.sensors[GYR].preRealRate,mTask.hw_min_rate);
            
            odr = ICM4N607CalcuOdr(&mTask.sensors[GYR].hwRate, &sampleRate);

            regValue = ICM4N607_ODR_MAPPING[odr];
            mTask.gyro_cfg0 = (mTask.gyro_cfg0 & 0xF0) | regValue;
            inv_icm4n607_write_reg(ICM4N607_GYRO_CONFIG0, mTask.gyro_cfg0,0);
           
            mTask.acc_cfg0 = (mTask.acc_cfg0 & 0xF0) | regValue;
            inv_icm4n607_write_reg(ICM4N607_ACCEL_CONFIG0, mTask.acc_cfg0,0);

            mTask.sensors[GYR].hwRate = mTask.sensors[GYR].fifo_rate;
            mTask.sensors[ACC].hwRate = 0;
            accelOdrChanged = true;
        #if !ICM4N607_MAX_ODR_800HZ
            registerAccGyroFifoInfo((mTask.sensors[ACC].fifo_rate == 0) ? 0 : 1024000000000 / mTask.sensors[ACC].fifo_rate,
            (mTask.sensors[GYR].fifo_rate == 0) ? 0 : 1024000000000 / mTask.sensors[GYR].fifo_rate);
        #else
            mTask.sensors[GYR].samplediv =  mTask.sensors[GYR].fifo_rate /mTask.sensors[GYR].preRealRate;
            mTask.sensors[GYR].sampledivcounter = 1;
            registerAccGyroFifoInfo((mTask.sensors[ACC].fifo_rate == 0) ? 0 : 1024000000000 / mTask.sensors[ACC].preRealRate,
            (mTask.sensors[GYR].fifo_rate == 0) ? 0 : 1024000000000 / mTask.sensors[GYR].preRealRate);
        #endif
        }
    #if defined(CFG_HW_STEP_COUNTER_SUPPORT) || SUPPORT_HW_FREEFALL     
        if((mTask.sensors[ANYMO].powered == false)&&(mTask.sensors[STEPCNT].powered == false)&&(mTask.sensors[FREEFALL].powered == false)){
    #else
        if(mTask.sensors[ANYMO].powered == false){
    #endif
            mTask.pwr_sta &= 0xFC;  // Acc OFF
            mTask.sensors[ACC].hwRate = 0;
            inv_icm4n607_write_reg(ICM4N607_PWR_MGMT_0, mTask.pwr_sta, 200);

            INVLOG(LOG_INFO, "acc off:%x\n", mTask.pwr_sta);
            }
        }
#ifdef SUPPORT_HW_FREEFALL
        else if(mTask.sensors[FREEFALL].powered == true){
            ICM4N607_set_acc_power_mode(BIT_ACC_LN_MODE);  
        }
#endif
#ifdef CFG_HW_STEP_COUNTER_SUPPORT     
    else if((mTask.sensors[ANYMO].powered == true)||(mTask.sensors[STEPCNT].powered == true)){
#else
    else if(mTask.sensors[ANYMO].powered == true){
#endif
        ICM4N607_set_acc_power_mode(BIT_ACC_LP_MODE);        
    }else{
        ICM4N607_set_acc_power_mode(BIT_ACC_OFF);        
    }

    mTask.watermark = ICM4N607CalcuWm();
    mTask.sensors[ACC].powered = false;
    mTask.sensors[ACC].configed = false;

    ICM4N607ConfigFifo(accelOdrChanged);
    if(accelOdrChanged){
        first_data = 1;
        isrwm_after_set_rate = false;
    }

    if ((false == mTask.sensors[GYR].powered) 
     && (false == mTask.sensors[ACC].powered)) {
        //disable fifo rcosc 
        inv_imu_set_autorcosc_power_on(false);
    }
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

#if DEBUG_PHASE | DEBUG_APEX_CONF
static int ICM4N607regdump(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{                         
    SPI_READ(0x00, 0x3E - 0x00 +1, &mTask.debugintBuffer);

    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

static int ICM4N607regresult(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                             void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{

     uint8_t *reg_data = &mTask.debugintBuffer[1];
     uint8_t regaddr = 0x11 ;
     uint8_t i =0;
     while( regaddr <= 0x3E) {
           INVLOG(LOG_INFO, "regaddr 0x%x regv 0x%x\n",regaddr,reg_data[i]);
           regaddr++;
           i++;
        }
        
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}
#endif

#if DEBUG_APEX_CONF
static int ICM4N607regdump_bank4(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                 void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
                         
    /*switch to bank4, then switch back to 0  */ 
    SPI_WRITE(ICM4N607_REG_BANK_SEL, 0x04);
    SPI_READ(0x40, 0x48 - 0x40 +1, &mTask.regdumpBank_4);
    SPI_WRITE(ICM4N607_REG_BANK_SEL, 0x00);

    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

static int ICM4N607reg_bank4_result(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                    void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
     uint8_t *reg_data = &mTask.regdumpBank_4[1];
     uint8_t regaddr = 0x40 ;
     uint8_t i =0;

     //dump bank 4

    while( regaddr < 0x49) {
           INVLOG(LOG_INFO, "bank4 regaddr 0x%x regvalue 0x%x\n",regaddr,reg_data[i]);
           regaddr++;
           i++;
        }
    
     sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
     return 0;
}
#endif

static int ICM4N607AccRate(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;
    uint8_t odr = 0;
    uint8_t regValue = 0x00;
    uint32_t sampleRate = 0;
    uint32_t maxRate = 0;
    bool accelOdrChanged = false;
    struct accGyroCntlPacket cntlPacket;

    ret = rxControlInfo(&cntlPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);
    if (ret < 0) {
        sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, ERROR_EVT);
        INVLOG(LOG_ERROR, "ICM4N607gRate, rx inSize and elemSize error\n");
        return -1;
    }

    mTask.sensors[ACC].rate = cntlPacket.rate;
    mTask.sensors[ACC].latency = cntlPacket.latency;

    if (0 == mTask.sensors[ACC].preRealRate) {
      mTask.sensors[ACC].needDiscardSample = true;
      mTask.sensors[ACC].samplesToDiscard = NUM_TODISCARD;
    }

    odr = ICM4N607CalcuOdr(&mTask.sensors[ACC].rate, &sampleRate);
    mTask.sensors[ACC].preRealRate = sampleRate;

    //could not set sample rate smaller than current hw_min_rate
    INVLOG(LOG_INFO, "acc require Rate %d, min rate %d,acc hw rate %d\n",sampleRate,mTask.hw_min_rate,mTask.sensors[ACC].fifo_rate);
    sampleRate = max(sampleRate, mTask.hw_min_rate);

    //recalculate the odr again
    odr = ICM4N607CalcuOdr(&sampleRate, &sampleRate);

    /*if gyr configed ,compare maxRate with acc and gyr rate*/
    if (mTask.sensors[GYR].configed) {
        maxRate = max(sampleRate, mTask.sensors[GYR].preRealRate);   // choose with preRealRate
        if ((maxRate != mTask.sensors[ACC].fifo_rate) || (maxRate != mTask.sensors[GYR].fifo_rate)) {

            mTask.sensors[ACC].fifo_rate = maxRate;
            mTask.sensors[GYR].fifo_rate = maxRate;
            mTask.sensors[ACC].hwRate = maxRate;
            mTask.sensors[GYR].hwRate = maxRate;
            odr = ICM4N607CalcuOdr(&maxRate, &sampleRate);
            regValue = ICM4N607_ODR_MAPPING[odr];
            mTask.gyro_cfg0 = (mTask.gyro_cfg0 & 0xF0) | regValue;
            inv_icm4n607_write_reg(ICM4N607_GYRO_CONFIG0, mTask.gyro_cfg0,0);
            
            mTask.acc_cfg0 = (mTask.acc_cfg0 & 0xF0) | regValue;
            inv_icm4n607_write_reg(ICM4N607_ACCEL_CONFIG0, mTask.acc_cfg0,0);
            accelOdrChanged = true;
            INVLOG(LOG_INFO, "ICM4N607gRate: gyro config Rate %d reg %x\n",maxRate,regValue);
         } else {
            accelOdrChanged = false;
         }
    } else {
        if ((sampleRate != mTask.sensors[ACC].fifo_rate)) {
            mTask.sensors[ACC].fifo_rate = sampleRate;
            mTask.sensors[ACC].hwRate = sampleRate;
            regValue = ICM4N607_ODR_MAPPING[odr];
            mTask.gyro_cfg0 = (mTask.gyro_cfg0 & 0xF0) | regValue;
            inv_icm4n607_write_reg(ICM4N607_GYRO_CONFIG0, mTask.gyro_cfg0,0);
            
            mTask.acc_cfg0 = (mTask.acc_cfg0 & 0xF0) | regValue;
            inv_icm4n607_write_reg(ICM4N607_ACCEL_CONFIG0, mTask.acc_cfg0,0);
            accelOdrChanged = true;
            INVLOG(LOG_INFO, "ICM4N607gRate: acc config Rate %d reg %x\n",sampleRate,regValue);
        } else {
            accelOdrChanged = false;
        }
    }
    
#if !ICM4N607_MAX_ODR_800HZ

    registerAccGyroFifoInfo((mTask.sensors[ACC].fifo_rate == 0) ? 0 : 1024000000000 / mTask.sensors[ACC].fifo_rate,
                            (mTask.sensors[GYR].fifo_rate == 0) ? 0 : 1024000000000 / mTask.sensors[GYR].fifo_rate);
#else
     mTask.sensors[ACC].samplediv =  mTask.sensors[ACC].fifo_rate /mTask.sensors[ACC].preRealRate;
     mTask.sensors[ACC].sampledivcounter = 1;
    INVLOG(LOG_INFO, "acc require Rate %d, min rate %d,acc hw rate %d div %d\n",sampleRate,mTask.hw_min_rate,mTask.sensors[ACC].fifo_rate,mTask.sensors[ACC].samplediv);
    if (mTask.sensors[GYR].configed && mTask.sensors[GYR].preRealRate != 0 ) {
         mTask.sensors[GYR].samplediv =  mTask.sensors[GYR].fifo_rate /mTask.sensors[GYR].preRealRate;
          mTask.sensors[GYR].sampledivcounter = 1;
    }
    registerAccGyroFifoInfo((mTask.sensors[ACC].fifo_rate == 0) ? 0 : 1024000000000 / mTask.sensors[ACC].preRealRate,
                            (mTask.sensors[GYR].fifo_rate == 0) ? 0 : 1024000000000 / mTask.sensors[GYR].preRealRate);
#endif

    mTask.sensors[ACC].configed = true;
    mTask.watermark = ICM4N607CalcuWm();

    /* to do Fifo fdr setting here ? */    
    ICM4N607ConfigFifo(accelOdrChanged);

    if(accelOdrChanged){
        first_data = 1;
        isrwm_after_set_rate = false;
    }
        
    mt_eint_unmask(mTask.hw->eint_num);
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

static int ICM4N607GyroEnable(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;
    int RealGyroTurnOntime;
    
    INVLOG(LOG_INFO, "ICM4N607gyEnable power mode=%d, \n", mTask.sensors[GYR].powered);

#if DEBUG_SENSOR_STABLE
    data_count_gyro = 0;
#endif
    
    mTask.sensors[GYR].powered = true;
    inv_icm4n607_switch_on_mclk_nodelay();

    mTask.pwr_sta &= ~ICM4N607_PWR_MGMT_0_GYRO_MODE_MASK;
	mTask.pwr_sta |= (uint8_t)ICM4N607_PWR_MGMT_0_GYRO_MODE_LN;
    inv_imu_set_autorcosc_power_on(true);   
   
    // set 80ms for gyro start-up time, note:at least 200us here to sync fly changes
    RealGyroTurnOntime = GYRO_TURNON_TIME;
 
    if(RealGyroTurnOntime <=0){
        inv_icm4n607_write_reg(ICM4N607_PWR_MGMT_0, mTask.pwr_sta,0);
        INVLOG(LOG_INFO, "no delay after enable pwr_sta %x\n",mTask.pwr_sta);
    } else {
        inv_icm4n607_write_reg(ICM4N607_PWR_MGMT_0, mTask.pwr_sta, RealGyroTurnOntime);
        INVLOG(LOG_INFO, "turn on delay %d,pwr_sta %x\n",RealGyroTurnOntime,mTask.pwr_sta);
    }     
    ret = spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);

    return ret;
}

static int ICM4N607GyroDisable(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                             void *inBuf, uint8_t inSize, uint8_t elemInSize,
                             void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;
    uint8_t odr = 0;
    uint8_t regValue = 0x00;
    uint32_t sampleRate = 0;
    struct accGyroCntlPacket cntlPacket;
    bool gyroOdrChanged = false;
    INVLOG(LOG_INFO, "ICM4N607gyDisable\n");

    ret = rxControlInfo(&cntlPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);
    if (ret < 0) {
        sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, ERROR_EVT);
        INVLOG(LOG_ERROR, "ICM4N607AccPowerOff, rx inSize and elemSize error\n");
        return -1;
    }

    mTask.sensors[GYR].rate = 0;
    mTask.sensors[GYR].preRealRate = 0;
    mTask.sensors[GYR].fifo_rate = 0;
    mTask.sensors[GYR].latency = SENSOR_LATENCY_NODATA;
    mTask.sensors[GYR].needDiscardSample = false;
    mTask.sensors[GYR].samplesToDiscard = 0;
#if ICM4N607_MAX_ODR_800HZ
    mTask.sensors[GYR].samplediv = 1;
    mTask.sensors[GYR].sampledivcounter = 1;
#endif

#if DEBUG_SENSOR_STABLE
    data_count_gyro = 0;
#endif

    inv_icm4n607_switch_off_mclk();
    /* enter off mode */
    if ((mTask.sensors[ACC].powered == false) && (mTask.sensors[ANYMO].powered == false)) {
        mt_eint_mask(mTask.hw->eint_num);
        mTask.pwr_sta &= 0xF0;
        mTask.sensors[GYR].hwRate = 0;
        mTask.sensors[ACC].hwRate = 0;
        
        /* turn off acc & gyro */
        mTask.pwr_sta &= 0xF0;
        inv_icm4n607_write_reg(ICM4N607_PWR_MGMT_0, mTask.pwr_sta, 200);
        INVLOG(LOG_INFO, "all off:%x\n", mTask.pwr_sta);
    } else if (mTask.sensors[ACC].powered == true) {  //  update ACC old ord
        if (mTask.sensors[ACC].fifo_rate != mTask.sensors[ACC].preRealRate) {
            mTask.sensors[ACC].fifo_rate = max(mTask.sensors[ACC].preRealRate,mTask.hw_min_rate);
            odr = ICM4N607CalcuOdr(&mTask.sensors[ACC].hwRate, &sampleRate);
            regValue = ICM4N607_ODR_MAPPING[odr];
            
            mTask.gyro_cfg0 = (mTask.gyro_cfg0 & 0xF0) | regValue;
            inv_icm4n607_write_reg(ICM4N607_GYRO_CONFIG0, mTask.gyro_cfg0,0);   
            mTask.acc_cfg0 = (mTask.acc_cfg0 & 0xF0) | regValue;
            inv_icm4n607_write_reg(ICM4N607_ACCEL_CONFIG0, mTask.acc_cfg0,0);

            mTask.sensors[GYR].hwRate = 0;
            mTask.sensors[ACC].hwRate = mTask.sensors[ACC].fifo_rate;
        #if ICM4N607_MAX_ODR_800HZ    
            mTask.sensors[ACC].samplediv =  mTask.sensors[ACC].hwRate /mTask.sensors[ACC].preRealRate;
            mTask.sensors[ACC].sampledivcounter = 1;
        #endif    
            gyroOdrChanged = true;
            INVLOG(LOG_INFO, "acc change rate to preRealRate:%d\n", mTask.sensors[ACC].hwRate);
        }
        mTask.pwr_sta &= 0xF3;  // Gyro OFF  also keep acc in LN Mode
        mTask.sensors[GYR].hwRate = 0;
        inv_icm4n607_write_reg(ICM4N607_PWR_MGMT_0, mTask.pwr_sta, 200);;    
        INVLOG(LOG_INFO, "PWR %x\n", mTask.pwr_sta);
    } 
#if SUPPORT_HW_FREEFALL
    else if(mTask.sensors[FREEFALL].powered == true){
        ICM4N607_set_acc_power_mode(BIT_ACC_LN_MODE);  
    }
#endif
#ifdef CFG_HW_STEP_COUNTER_SUPPORT   
    else if ((mTask.sensors[ANYMO].powered == true) ||(mTask.sensors[STEPCNT].powered == true)){
#else
    else if (mTask.sensors[ANYMO].powered == true) {
#endif
        ICM4N607_set_acc_power_mode(BIT_ACC_LP_MODE);
    }
  
#if !ICM4N607_MAX_ODR_800HZ    
    registerAccGyroFifoInfo((mTask.sensors[ACC].fifo_rate == 0) ? 0 : 1024000000000 / mTask.sensors[ACC].fifo_rate,
        (mTask.sensors[GYR].fifo_rate == 0) ? 0 : 1024000000000 / mTask.sensors[GYR].fifo_rate);
#else
    registerAccGyroFifoInfo((mTask.sensors[ACC].fifo_rate == 0) ? 0 : 1024000000000 / mTask.sensors[ACC].preRealRate,
    (mTask.sensors[GYR].fifo_rate == 0) ? 0 : 1024000000000 / mTask.sensors[GYR].preRealRate);
#endif

    mTask.watermark = ICM4N607CalcuWm();
    mTask.sensors[GYR].powered = false;
    mTask.sensors[GYR].configed = false;

    INVLOG(LOG_INFO, "ICM4N607gyDisable power mode=%d \n", mTask.sensors[GYR].powered);

    ICM4N607ConfigFifo(gyroOdrChanged);
     if(gyroOdrChanged){
       first_data = 1;
       isrwm_after_set_rate = false;
    }

    if ((false == mTask.sensors[GYR].powered) 
    && (false == mTask.sensors[ACC].powered)) {
        //disable fifo rcosc 
        inv_imu_set_autorcosc_power_on(false);
    }
    mTask.lastGyroDisabletime = rtcGetTime();

    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

static int ICM4N607GyroRate(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                          void *inBuf, uint8_t inSize, uint8_t elemInSize,
                          void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;
    uint8_t odr = 0;
    uint8_t regValue = 0x00;
    uint32_t sampleRate,gyro_sampleRate = 0;
    uint32_t maxRate = 0;
    bool gyroOdrChanged = false;
    struct accGyroCntlPacket cntlPacket;

    ret = rxControlInfo(&cntlPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);
    if (ret < 0) {
        sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, ERROR_EVT);
        INVLOG(LOG_ERROR, "ICM4N607gRate, rx inSize and elemSize error\n");
        return -1;
    }

    mTask.sensors[GYR].rate = cntlPacket.rate;
    mTask.sensors[GYR].latency = cntlPacket.latency;

    /*  get hw sample rate */
    odr = ICM4N607CalcuOdr(&mTask.sensors[GYR].rate, &sampleRate);

    //could not set sample rate smaller than current hw_min_rate
    sampleRate = max(sampleRate, mTask.hw_min_rate);
    gyro_sampleRate = sampleRate;

    /*if gyr configed ,compare maxRate with acc and gyr rate*/
    if (mTask.sensors[ACC].configed) {
        maxRate = max(sampleRate, mTask.sensors[ACC].preRealRate);
        if ((maxRate != mTask.sensors[ACC].fifo_rate) || (maxRate != mTask.sensors[GYR].fifo_rate)) {
            mTask.sensors[ACC].fifo_rate = maxRate;
            mTask.sensors[GYR].fifo_rate = maxRate;
            mTask.sensors[ACC].hwRate = maxRate;
            mTask.sensors[GYR].hwRate = maxRate;
            /* update new odr */
            odr = ICM4N607CalcuOdr(&maxRate, &sampleRate);

            regValue = ICM4N607_ODR_MAPPING[odr];
            mTask.gyro_cfg0 = (mTask.gyro_cfg0 & 0xF0) | regValue;
            inv_icm4n607_write_reg(ICM4N607_GYRO_CONFIG0, mTask.gyro_cfg0,0);
            
            mTask.acc_cfg0 = (mTask.acc_cfg0 & 0xF0) | regValue;
            inv_icm4n607_write_reg(ICM4N607_ACCEL_CONFIG0, mTask.acc_cfg0,0);
            INVLOG(LOG_INFO, "ICM4N607gyRate: ACC config Rate %d reg %x\n",maxRate,regValue);
            gyroOdrChanged = true;
        } else {
            gyroOdrChanged = false;
        }
    } else {
        if ((sampleRate != mTask.sensors[GYR].fifo_rate)) {
            mTask.sensors[GYR].fifo_rate = sampleRate;
            mTask.sensors[GYR].hwRate = sampleRate;
            regValue = ICM4N607_ODR_MAPPING[odr];
            /* update new odr */
            mTask.gyro_cfg0 = (mTask.gyro_cfg0 & 0xF0) | regValue;
            inv_icm4n607_write_reg(ICM4N607_GYRO_CONFIG0, mTask.gyro_cfg0,0);
            
            mTask.acc_cfg0 = (mTask.acc_cfg0 & 0xF0) | regValue;
            inv_icm4n607_write_reg(ICM4N607_ACCEL_CONFIG0, mTask.acc_cfg0,0);
            gyroOdrChanged = true;
            INVLOG(LOG_INFO, "ICM4N607gyRate: gyro config Rate %d reg %x\n",sampleRate,regValue);
        } else {
            gyroOdrChanged = false;
        }
    }

    if (0 == mTask.sensors[GYR].preRealRate) {
      mTask.sensors[GYR].needDiscardSample = true;
      mTask.sensors[GYR].samplesToDiscard = NUM_TODISCARD;
    }

    mTask.sensors[GYR].preRealRate = gyro_sampleRate;
	INVLOG(LOG_INFO, "ICM4N607gyRate: [ACC].configed %d, [ACC].preRealRate %d ,mTask.sensors[GYR].rate %d\n",mTask.sensors[ACC].configed, mTask.sensors[ACC].preRealRate,mTask.sensors[GYR].rate);
    
#if !ICM4N607_MAX_ODR_800HZ
    registerAccGyroFifoInfo((mTask.sensors[ACC].fifo_rate == 0) ? 0 : 1024000000000 / mTask.sensors[ACC].fifo_rate,
                            (mTask.sensors[GYR].fifo_rate == 0) ? 0 : 1024000000000 / mTask.sensors[GYR].fifo_rate);
#else
    mTask.sensors[GYR].samplediv =  mTask.sensors[GYR].fifo_rate /mTask.sensors[GYR].preRealRate;
    mTask.sensors[GYR].sampledivcounter = 1;
    if (mTask.sensors[ACC].configed && mTask.sensors[ACC].preRealRate != 0 ) {
         mTask.sensors[ACC].samplediv =  mTask.sensors[ACC].fifo_rate /mTask.sensors[ACC].preRealRate;
         mTask.sensors[ACC].sampledivcounter = 1;
    }
    INVLOG(LOG_INFO, "ICM4N607gyRate: sample rate %d gyr fifo_rate %d , gyro div %d \n",sampleRate,mTask.sensors[GYR].fifo_rate, mTask.sensors[GYR].samplediv);
    registerAccGyroFifoInfo((mTask.sensors[ACC].fifo_rate == 0) ? 0 : 1024000000000 / mTask.sensors[ACC].preRealRate,
                            (mTask.sensors[GYR].fifo_rate == 0) ? 0 : 1024000000000 / mTask.sensors[GYR].preRealRate);
#endif
    mTask.sensors[GYR].configed = true;
    /* watermark update */
    mTask.watermark = ICM4N607CalcuWm();
    /* to do Fifo fdr setting here ? */    
    ICM4N607ConfigFifo(gyroOdrChanged);

     if(gyroOdrChanged){
       first_data = 1;
       isrwm_after_set_rate = false;
    }
        
    mt_eint_unmask(mTask.hw->eint_num);
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

static int ICM4N607AccCali(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
#ifndef CFG_MTK_CALIBRATION_V1_0 
    float bias[AXES_NUM] = {0};
#endif
    mTask.sensors[ACC].startCali = true;

    INVLOG(LOG_INFO, "ICM4N607AccCali\n");
#ifndef CFG_MTK_CALIBRATION_V1_0  
    Acc_init_calibration(bias);
#else
	accFactoryCalibrateInit(&mTask.accFactoryCal);
#endif
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int ICM4N607GyroCali(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
#ifndef CFG_MTK_CALIBRATION_V1_0
    float slope[AXES_NUM] = {0};
    float intercept[AXES_NUM] = {0};
#endif
    mTask.sensors[GYR].startCali = true;

    INVLOG(LOG_INFO, "ICM4N607GyroCali\n");
#ifndef CFG_MTK_CALIBRATION_V1_0
    Gyro_init_calibration(slope, intercept);
#else
	gyroFactoryCalibrateInit(&mTask.gyroFactoryCal);
#endif
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int ICM4N607AccCfgCali(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                              void *inBuf, uint8_t inSize, uint8_t elemInSize,
                              void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;
    struct accGyroCaliCfgPacket caliCfgPacket;

    ret = rxCaliCfgInfo(&caliCfgPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);

    if (ret < 0) {
        sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, ERROR_EVT);
        INVLOG(LOG_ERROR, "accHwCaliCheck, rx inSize and elemSize error\n");
        return -1;
    }
    INVLOG(LOG_INFO, "ICM4N607AccCfgCali: cfgData[0]:%d, cfgData[1]:%d, cfgData[2]:%d\n",
          caliCfgPacket.caliCfgData[0], caliCfgPacket.caliCfgData[1], caliCfgPacket.caliCfgData[2]);

    mTask.sensors[ACC].staticCali[0] = (float)caliCfgPacket.caliCfgData[0] / ACCELEROMETER_INCREASE_NUM_AP;
    mTask.sensors[ACC].staticCali[1] = (float)caliCfgPacket.caliCfgData[1] / ACCELEROMETER_INCREASE_NUM_AP;
    mTask.sensors[ACC].staticCali[2] = (float)caliCfgPacket.caliCfgData[2] / ACCELEROMETER_INCREASE_NUM_AP;

    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int ICM4N607GyroCfgCali(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                               void *inBuf, uint8_t inSize, uint8_t elemInSize,
                               void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;
    struct accGyroCaliCfgPacket caliCfgPacket;

    ret = rxCaliCfgInfo(&caliCfgPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);

    if (ret < 0) {
        sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, ERROR_EVT);
        INVLOG(LOG_ERROR, "gyroHwCaliCheck, rx inSize and elemSize error\n");
        return -1;
    }

    INVLOG(LOG_INFO, "ICM4N607GyroCfgCali: cfgData[0]:%d, cfgData[1]:%d, cfgData[2]:%d\n",
          caliCfgPacket.caliCfgData[0], caliCfgPacket.caliCfgData[1], caliCfgPacket.caliCfgData[2]);

    mTask.sensors[GYR].staticCali[0] = (float)caliCfgPacket.caliCfgData[0] / GYROSCOPE_INCREASE_NUM_AP;
    mTask.sensors[GYR].staticCali[1] = (float)caliCfgPacket.caliCfgData[1] / GYROSCOPE_INCREASE_NUM_AP;
    mTask.sensors[GYR].staticCali[2] = (float)caliCfgPacket.caliCfgData[2] / GYROSCOPE_INCREASE_NUM_AP;
    INVLOG(LOG_INFO, "ICM4N607GyroCfgCali: bias [0]:%f, bias[1]:%f, bias[2]:%f\n",
         (double)mTask.sensors[GYR].staticCali[0], (double)mTask.sensors[GYR].staticCali[1], (double)mTask.sensors[GYR].staticCali[2]);
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}


static void accGetCalibration(int32_t *cali, int32_t size)
{
    cali[AXIS_X]  = mTask.accHwCali[AXIS_X];
    cali[AXIS_Y]  = mTask.accHwCali[AXIS_Y];
    cali[AXIS_Z]  = mTask.accHwCali[AXIS_Z];
    //  INVLOG(LOG_INFO, "accGetCalibration cali x:%d, y:%d, z:%d\n", cali[AXIS_X], cali[AXIS_Y], cali[AXIS_Z]);
}
static void accSetCalibration(int32_t *cali, int32_t size)
{
    mTask.accHwCali[AXIS_X] = cali[AXIS_X];
    mTask.accHwCali[AXIS_Y] = cali[AXIS_Y];
    mTask.accHwCali[AXIS_Z] = cali[AXIS_Z];
    INVLOG(LOG_INFO, "accSetCalibration cali x:%d, y:%d, z:%d\n", mTask.accHwCali[AXIS_X],
          mTask.accHwCali[AXIS_Y], mTask.accHwCali[AXIS_Z]);
}
static void accGetData(void *sample)
{
    struct TripleAxisDataPoint *tripleSample = (struct TripleAxisDataPoint *)sample;
    tripleSample->ix = mTask.accFactoryData.ix;
    tripleSample->iy = mTask.accFactoryData.iy;
    tripleSample->iz = mTask.accFactoryData.iz;

    INVLOG(LOG_INFO, "accGetData x:%d, y:%d, z:%d\n", tripleSample->ix, tripleSample->iy, tripleSample->iz);
}
static void gyroGetCalibration(int32_t *cali, int32_t size)
{
    cali[AXIS_X] = mTask.gyroHwCali[AXIS_X];
    cali[AXIS_Y] = mTask.gyroHwCali[AXIS_Y];
    cali[AXIS_Z] = mTask.gyroHwCali[AXIS_Z];
    //  INVLOG(LOG_INFO, "gyroGetCalibration cali x:%d, y:%d, z:%d\n", cali[AXIS_X], cali[AXIS_Y], cali[AXIS_Z]);
}
static void gyroSetCalibration(int32_t *cali, int32_t size)
{
    mTask.gyroHwCali[AXIS_X] = cali[AXIS_X];
    mTask.gyroHwCali[AXIS_Y] = cali[AXIS_Y];
    mTask.gyroHwCali[AXIS_Z] = cali[AXIS_Z];
    INVLOG(LOG_INFO, "gyroSetCalibration cali x:%d, y:%d, z:%d\n", mTask.gyroHwCali[AXIS_X],
          mTask.gyroHwCali[AXIS_Y], mTask.gyroHwCali[AXIS_Z]);
}
static void gyroGetData(void *sample)
{
    struct TripleAxisDataPoint *tripleSample = (struct TripleAxisDataPoint *)sample;
    tripleSample->ix = mTask.gyroFactoryData.ix;
    tripleSample->iy = mTask.gyroFactoryData.iy;
    tripleSample->iz = mTask.gyroFactoryData.iz;

    INVLOG(LOG_INFO, "gyroGetData x:%d, y:%d, z:%d\n", tripleSample->ix, tripleSample->iy, tripleSample->iz);
}

static void spiIsrCallBack(void *cookie, int err)
{
    if (err != 0) {
        INVLOG(LOG_ERROR, "icm4n607: spiIsrCallBack err\n");
        sensorFsmEnqueueFakeSpiEvt(mTask.spiCallBack, cookie, ERROR_EVT);
    } else {
        mTask.swSampleTime = rtcGetTime();
        sensorFsmEnqueueFakeSpiEvt(mTask.spiCallBack, cookie, SUCCESS_EVT);
    }
}

static int ICM4N607Sample(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                          void *inBuf, uint8_t inSize, uint8_t elemInSize,
                          void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    int ret = 0;

    ret = rxTransferDataInfo(&mTask.dataInfo, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);
    if (ret < 0) {
        sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, ERROR_EVT);
        INVLOG(LOG_ERROR, "ICM4N607Sample, rx dataInfo error\n");
        return -1;
    }
    inv_icm4n607_read_reg(ICM4N607_FIFO_BYTE_COUNT1, 2, &mTask.statusBuffer,0);
    mTask.spiCallBack = spiCallBack;

    return spiBatchTxRx(&mTask.mode, spiIsrCallBack, next_state, __FUNCTION__);
}

static int ICM4N607ReadFifo(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mTask.fifoDataToRead = (mTask.statusBuffer[2] << 8) | mTask.statusBuffer[1];
    /* covert record num to bytes */
    //mTask.fifoDataToRead *= 16; /* N/A for byte mode */
    //INVLOG(LOG_ERROR, "ICM4N607ReadFifo :mTask.fifoDataToRead = %d\n", mTask.fifoDataToRead);
    if (mTask.fifoDataToRead <= 0 || mTask.fifoDataToRead > ICM4N607_MAX_FIFO_SIZE) {
        sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
        return 0;
    }
    //mTask.fifoDataToRead += 1; // read one more byte to eliminate double interrupt

    if( mTask.fifoDataToRead >= 1008)
        mTask.fifoDataToRead = 992;
        
    inv_icm4n607_read_reg(ICM4N607_FIFO_DATA_REG, mTask.fifoDataToRead ,&mTask.regBuffer,0);
    inv_icm4n607_switch_off_mclk();
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

static void parseRawData(struct accGyroData *data, uint8_t *buf, uint8_t sensorType)
{
    int16_t raw_data[AXES_NUM] = {0};
    int8_t temper_raw =0;
    int32_t remap_data[AXES_NUM] = {0};
    int32_t SwCali[AXES_NUM] = {0};

    int32_t caliResult[AXES_NUM] = {0};
    float temp_data[AXES_NUM] = {0};
    float calibrated_data_output[AXES_NUM] = {0};
#ifndef CFG_MTK_CALIBRATION_V1_0
    int32_t delta_time = 0;
#endif
    int16_t status = 0;

	if (sensorType == SENS_TYPE_ACCEL)
    	accGetCalibration(SwCali, 0);
    else if (sensorType == SENS_TYPE_GYRO)
        gyroGetCalibration(SwCali, 0);
		
    if(sensorType == SENS_TYPE_TEMP) {
        /* Use big endian mode */
         temper_raw = (int8_t) buf[0];
		/* convert to physical unit: Celsius degree */
		 mTask.chip_temper = ((float)temper_raw * TEMP_SENSITIVITY_1B_4N607) + ROOM_TEMP_OFFSET;
		return;
    }
    
    /* Use little endian mode */
    raw_data[AXIS_X] = (buf[0] | buf[1] << 8);
    raw_data[AXIS_Y] = (buf[2] | buf[3] << 8);
    raw_data[AXIS_Z] = (buf[4] | buf[5] << 8);

    //if (sensorType == SENS_TYPE_GYRO) {
    //    INVLOG(LOG_ERROR, "GYRO: %d %d %d\n", raw_data[AXIS_X], raw_data[AXIS_Y], raw_data[AXIS_Z]);
    //}

    raw_data[AXIS_X] = raw_data[AXIS_X] + SwCali[AXIS_X];
    raw_data[AXIS_Y] = raw_data[AXIS_Y] + SwCali[AXIS_Y];
    raw_data[AXIS_Z] = raw_data[AXIS_Z] + SwCali[AXIS_Z];

    remap_data[mTask.cvt.map[AXIS_X]] = (int32_t)mTask.cvt.sign[AXIS_X] * raw_data[AXIS_X];
    remap_data[mTask.cvt.map[AXIS_Y]] = (int32_t)mTask.cvt.sign[AXIS_Y] * raw_data[AXIS_Y];
    remap_data[mTask.cvt.map[AXIS_Z]] = (int32_t)mTask.cvt.sign[AXIS_Z] * raw_data[AXIS_Z];

    if (sensorType == SENS_TYPE_ACCEL) {
        temp_data[AXIS_X] = (float)remap_data[AXIS_X] * KSCALE_ACC_8G_RANGE;
        temp_data[AXIS_Y] = (float)remap_data[AXIS_Y] * KSCALE_ACC_8G_RANGE;
        temp_data[AXIS_Z] = (float)remap_data[AXIS_Z] * KSCALE_ACC_8G_RANGE;

        #if DEBUG_ACC_DATA
        INVLOG(LOG_ERROR, "A: %f %f %f\n", (double)temp_data[AXIS_X], (double)temp_data[AXIS_Y], (double)temp_data[AXIS_Z]);
        #endif

#if DEBUG_SENSOR_STABLE
        data_count ++;

        if(0.1< abs(temp_data[AXIS_X]-previous_acc.x) &&
           0.1< abs(temp_data[AXIS_Y]-previous_acc.y) &&
           0.1< abs(temp_data[AXIS_Z]-previous_acc.z)){

        INVLOG(LOG_ERROR, " Now A: %f %f %f, Pr A: %f %f %f, cnt %d\n", (double)temp_data[AXIS_X], (double)temp_data[AXIS_Y], (double)temp_data[AXIS_Z],(double)previous_acc.x,(double)previous_acc.y,(double)previous_acc.z,data_count);
        }

       previous_acc.x = temp_data[AXIS_X];
       previous_acc.y = temp_data[AXIS_Y];
       previous_acc.z = temp_data[AXIS_Z];
#endif
        
        if (UNLIKELY(mTask.sensors[ACC].startCali)) {
#ifndef CFG_MTK_CALIBRATION_V1_0
            status = Acc_run_factory_calibration_timeout(delta_time,
                     temp_data, calibrated_data_output, (int *)&mTask.sensors[ACC].accuracy, rtcGetTime());
			 INVLOG(LOG_INFO, "ACC status %d accuracy %d\n",status,(int *)&mTask.sensors[ACC].accuracy);
            if (status != 0) {
                mTask.sensors[ACC].startCali = false;
                if (status > 0) {
                    INVLOG(LOG_INFO, "ACC cali detect shake\n");
                    caliResult[AXIS_X] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_X] * 1000);
                    caliResult[AXIS_Y] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_Y] * 1000);
                    caliResult[AXIS_Z] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_Z] * 1000);
                    accGyroSendCalibrationResult(SENS_TYPE_ACCEL, (int32_t *)&caliResult[0], (uint8_t)status);
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
                accGyroSendCalibrationResult(SENS_TYPE_ACCEL, (int32_t *)&caliResult[0], (uint8_t)status);
              //  INVLOG(LOG_INFO, "ACCEL cali done:caliResult[0] %d, caliResult[1] %d, caliResult[2] %d\n",
               //                caliResult[AXIS_X], caliResult[AXIS_Y], caliResult[AXIS_Z];
            }
#else
		status = accFactoryCalibrateRun(&mTask.accFactoryCal,
                temp_data[AXIS_X], temp_data[AXIS_Y], temp_data[AXIS_Z], rtcGetTime());
				
            if (status == UNSTABLE_DETECT) {
                mTask.sensors[ACC].startCali = false;
                caliResult[AXIS_X] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_X] * ACCELEROMETER_INCREASE_NUM_AP);
                caliResult[AXIS_Y] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_Y] * ACCELEROMETER_INCREASE_NUM_AP);
                caliResult[AXIS_Z] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_Z] * ACCELEROMETER_INCREASE_NUM_AP);
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
                caliResult[AXIS_X] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_X] * ACCELEROMETER_INCREASE_NUM_AP);
                caliResult[AXIS_Y] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_Y] * ACCELEROMETER_INCREASE_NUM_AP);
                caliResult[AXIS_Z] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_Z] * ACCELEROMETER_INCREASE_NUM_AP);
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
        mTask.accFactoryData.ix = (int32_t)(data->x * ACCELEROMETER_INCREASE_NUM_AP);
        mTask.accFactoryData.iy = (int32_t)(data->y * ACCELEROMETER_INCREASE_NUM_AP);
        mTask.accFactoryData.iz = (int32_t)(data->z * ACCELEROMETER_INCREASE_NUM_AP);

    } else if (sensorType == SENS_TYPE_GYRO) {
        temp_data[AXIS_X] = (float)remap_data[AXIS_X] * KSCALE_GYRO_2000_RANGE;
        temp_data[AXIS_Y] = (float)remap_data[AXIS_Y] * KSCALE_GYRO_2000_RANGE;
        temp_data[AXIS_Z] = (float)remap_data[AXIS_Z] * KSCALE_GYRO_2000_RANGE;

        #if DEBUG_GYRO_DATA
        INVLOG(LOG_ERROR, "GY: %f %f %f\n", (double)temp_data[AXIS_X], (double)temp_data[AXIS_Y], (double)temp_data[AXIS_Z]);

      //  INVLOG(LOG_INFO, "ICM4N607GyrodataCali: bias [0]:%f, bias[1]:%f, bias[2]:%f\n",
      //   (double)mTask.sensors[GYR].staticCali[0], (double)mTask.sensors[GYR].staticCali[1], (double)mTask.sensors[GYR].staticCali[2]);
        #endif

        #if DEBUG_SENSOR_STABLE
        data_count_gyro ++;

        if(0.04< abs(temp_data[AXIS_X]-previous_gyro.x) &&
           0.04< abs(temp_data[AXIS_Y]-previous_gyro.y) &&
           0.04< abs(temp_data[AXIS_Z]-previous_gyro.z)){

        INVLOG(LOG_ERROR, " Now GY: %f %f %f, Pr GU: %f %f %f, cnt %d\n", (double)temp_data[AXIS_X], (double)temp_data[AXIS_Y], (double)temp_data[AXIS_Z],(double)previous_gyro.x,(double)previous_gyro.y,(double)previous_gyro.z,data_count_gyro);
        }

       previous_gyro.x = temp_data[AXIS_X];
       previous_gyro.y = temp_data[AXIS_Y];
       previous_gyro.z = temp_data[AXIS_Z];
       #endif
        if (UNLIKELY(mTask.sensors[GYR].startCali)) {
#ifndef CFG_MTK_CALIBRATION_V1_0
            status = Gyro_run_factory_calibration_timeout(delta_time,
                     temp_data, calibrated_data_output, (int *)&mTask.sensors[GYR].accuracy, 0, rtcGetTime());
			INVLOG(LOG_INFO, "gyro status %d, accuracy %d\n",status,(int *)&mTask.sensors[GYR].accuracy);		 
            if (status != 0) {
                mTask.sensors[GYR].startCali = false;
                if (status > 0) {
                    INVLOG(LOG_INFO, "GYRO cali detect shake\n");
                    caliResult[AXIS_X] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_X] * 1000);
                    caliResult[AXIS_Y] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_Y] * 1000);
                    caliResult[AXIS_Z] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_Z] * 1000);
                    accGyroSendCalibrationResult(SENS_TYPE_GYRO, (int32_t *)&caliResult[0], (uint8_t)status);
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
                accGyroSendCalibrationResult(SENS_TYPE_GYRO, (int32_t *)&caliResult[0], (uint8_t)status);
              //  INVLOG(LOG_INFO, "GYRO cali done: caliResult[0]:%d, caliResult[1]:%d, caliResult[2]:%d \n",
              //                         caliResult[AXIS_X], caliResult[AXIS_Y], caliResult[AXIS_Z]); 
            }
#else
            status = gyroFactoryCalibrateRun(&mTask.gyroFactoryCal,
                temp_data[AXIS_X], temp_data[AXIS_Y], temp_data[AXIS_Z], rtcGetTime());
            if (status == UNSTABLE_DETECT) {
                mTask.sensors[GYR].startCali = false;
                caliResult[AXIS_X] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_X] * GYROSCOPE_INCREASE_NUM_AP);
                caliResult[AXIS_Y] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_Y] * GYROSCOPE_INCREASE_NUM_AP);
                caliResult[AXIS_Z] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_Z] * GYROSCOPE_INCREASE_NUM_AP);
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
                caliResult[AXIS_X] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_X] * GYROSCOPE_INCREASE_NUM_AP);
                caliResult[AXIS_Y] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_Y] * GYROSCOPE_INCREASE_NUM_AP);
                caliResult[AXIS_Z] = (int32_t)(mTask.sensors[GYR].staticCali[AXIS_Z] * GYROSCOPE_INCREASE_NUM_AP);
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
}

static uint16_t icm4n607_match_ts_with_fifo_cnt(
       uint16_t packet_cnt,
       uint64_t timestamp,
       int64_t estimated_sample_intvl
       )
{
    int64_t cur_intvl = 0;
    //int32_t next_intvl = 0;
    uint32_t cur_intvl_dt = 0;
    //uint32_t next_intvl_dt = 0;
    uint32_t min_intvl_dt = (uint32_t)(estimated_sample_intvl);
    uint16_t wm_th = mTask.watermark/16;
    uint16_t cnt = 1;
    uint16_t min_intvl_cnt = cnt;
    uint16_t matched_cnt;

    //INVLOG(LOG_INFO, "wm %d pcnt %d", wm_th, packet_cnt);

    // if the packet cnt is equal to watermark cnt,it should be exactly the cnt we want.
    if (packet_cnt == wm_th) {
        matched_cnt = packet_cnt;
        return matched_cnt;
    }

    if (mTask.hwSampleTime == mTask.last_hwSampleTime) {
    // flush case , no interrupt case
        matched_cnt = packet_cnt;
        return matched_cnt;
    }

    while (cnt <= packet_cnt) {
        cur_intvl = (timestamp - mTask.lastSampleTime) / cnt;
        //next_intvl = (timestamp - state->pre_record_ts) / (cnt + 1);
        cur_intvl_dt = abs(cur_intvl - estimated_sample_intvl);
        //next_intvl_dt = abs(next_intvl - (uint32_t)(state->sampling_intvl));

        if (cur_intvl_dt < min_intvl_dt) {
            min_intvl_cnt = cnt;
            min_intvl_dt = cur_intvl_dt;
        }

    //    if(cnt >= wm_th)
    //        INVLOG(LOG_INFO,  "%d, intvl %lld intvl_dt %lu,est %lld", cnt,cur_intvl,cur_intvl_dt,estimated_sample_intvl);
            
        cnt++;

    //    INVLOG(LOG_INFO,  "cur intvl %d", cur_intvl);
    //    INVLOG(LOG_INFO,  "cur intvl_dt %d", cur_intvl_dt);

    }
    matched_cnt = min_intvl_cnt;
   // INVLOG(LOG_INFO,  "mrp %d, min_intvl_dt %lu", matched_cnt,min_intvl_dt);
    return matched_cnt;
}

static int ICM4N607Convert(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    struct accGyroData *data = mTask.accGyroPacket.outBuf;
    uint32_t i = 0;
    uint8_t accEventSize = 0;
    uint8_t gyroEventSize = 0;
    uint16_t packet_cnt,real_packet_cnt;
    uint8_t accEventSize_Discard = 0;
    uint8_t gyroEventSize_Discard = 0;
    uint64_t swSampleTime = 0, realSampleTime = 0,actual_hwSampleTime;
    uint8_t *fifo_data = &mTask.regBuffer[1];
    uint32_t minDelay = 0;
    uint64_t long_diff = 0;
    uint8_t long_EventSize = 0;
    uint32_t fifo_hwRate =  max(mTask.sensors[ACC].fifo_rate,mTask.sensors[GYR].fifo_rate);
    uint64_t estimated_sample_intvl = 1024000000000ULL /fifo_hwRate;

    for (i=0; i < mTask.fifoDataToRead; i+=16) {
        if ((accEventSize + gyroEventSize) < MAX_RECV_PACKET) {
            if (mTask.sensors[ACC].configed && mTask.sensors[ACC].powered) {
                if (mTask.sensors[ACC].samplesToDiscard) {
                    mTask.sensors[ACC].samplesToDiscard--;
                    accEventSize_Discard++;
                } else {
				#if !ICM4N607_MAX_ODR_800HZ
                    parseRawData(&data[accEventSize+gyroEventSize], &fifo_data[i+1], SENS_TYPE_ACCEL);
                    accEventSize++;
				#else
                    if(mTask.sensors[ACC].sampledivcounter >= mTask.sensors[ACC].samplediv){
                        parseRawData(&data[accEventSize+gyroEventSize], &fifo_data[i+1], SENS_TYPE_ACCEL);
                        accEventSize++;
                        mTask.sensors[ACC].sampledivcounter = 1;   //reset div counter
                    }
                    else{
                        accEventSize_Discard++;
                       // INVLOG(LOG_ERROR, "acc sampledivcounter %d, samplediv %d ds %d\n", mTask.sensors[ACC].sampledivcounter, mTask.sensors[ACC].samplediv,accEventSize_Discard);
                        mTask.sensors[ACC].sampledivcounter++;
                    }
				#endif
                }
            }
            if (mTask.sensors[GYR].configed && mTask.sensors[GYR].powered) {
                if (mTask.sensors[GYR].samplesToDiscard) {
                    mTask.sensors[GYR].samplesToDiscard--;
                    gyroEventSize_Discard++;
                } else {
				#if !ICM4N607_MAX_ODR_800HZ
                    parseRawData(&data[accEventSize+gyroEventSize], &fifo_data[i+7], SENS_TYPE_GYRO);
                    gyroEventSize++;
				#else
                    if(mTask.sensors[GYR].sampledivcounter >= mTask.sensors[GYR].samplediv){
                        parseRawData(&data[accEventSize+gyroEventSize], &fifo_data[i+7], SENS_TYPE_GYRO);
                        gyroEventSize++;
                        mTask.sensors[GYR].sampledivcounter = 1;   //reset div counter
                    }
                    else{
                        gyroEventSize_Discard++;
                      //  INVLOG(LOG_ERROR, "gyro sampledivcounter %d, samplediv %d ds %d\n", mTask.sensors[GYR].sampledivcounter, mTask.sensors[GYR].samplediv,accEventSize_Discard);
                        mTask.sensors[GYR].sampledivcounter++;
                    }
				#endif
                }
            }
            if (mTask.sensors[GYR].configed || mTask.sensors[ACC].configed) {
                parseRawData(&data[accEventSize+gyroEventSize], &fifo_data[i+13], SENS_TYPE_TEMP);
            }
        } else
            INVLOG(LOG_ERROR, "outBuf full, accEventSize = %d, gyroEventSize = %d\n", accEventSize, gyroEventSize);
    }
    
#ifndef CFG_MTK_CALIBRATION_V1_0
    /*if factory true , can't send to runtime cali in parseRawData in accGyro*/
    if (mTask.sensors[ACC].startCali || mTask.sensors[GYR].startCali) {
        accEventSize = 0;
        gyroEventSize = 0;
    #if !ICM4N607_MAX_ODR_800HZ
		registerAccGyroFifoInfo((mTask.sensors[ACC].fifo_rate == 0) ? 0 : 1024000000000 / mTask.sensors[ACC].fifo_rate,
        (mTask.sensors[GYR].fifo_rate == 0) ? 0 : 1024000000000 / mTask.sensors[GYR].fifo_rate);		
    #else
        registerAccGyroFifoInfo((mTask.sensors[ACC].fifo_rate == 0) ? 0 : 1024000000000 / mTask.sensors[ACC].preRealRate,
        (mTask.sensors[GYR].fifo_rate == 0) ? 0 : 1024000000000 / mTask.sensors[GYR].preRealRate);
    #endif
    }
#endif
    //INVLOG(LOG_INFO, "ICM4N607Convert, fifoDataToRead:%d, accEventSize:%d, gyroEventSize:%d\n",
        //mTask.fifoDataToRead, accEventSize, gyroEventSize);

    swSampleTime = mTask.swSampleTime;

    /* Pre Process lastSampleTime and hwSampleTime */
    calc_long_diff(
        mTask.sensors[ACC].fifo_rate, mTask.sensors[ACC].configed, accEventSize + accEventSize_Discard,
        mTask.sensors[GYR].fifo_rate, mTask.sensors[GYR].configed, gyroEventSize + gyroEventSize_Discard,
        &minDelay, &long_diff, &long_EventSize);

    
    // if first_data, lastSampleTime cannot use in below Fake time caculate, so assin value to handle it
    if (first_data == 1) {
        mTask.lastSampleTime = swSampleTime - long_diff;
        #if DEBUG_FIFO_DATA
        INVLOG(LOG_INFO, "lastST %llu,longdiff %llu\n",mTask.lastSampleTime,long_diff);
        #endif
        first_data = 0;
    }

    packet_cnt = max((uint16_t)(accEventSize + accEventSize_Discard),(uint16_t)(gyroEventSize + gyroEventSize_Discard));
    real_packet_cnt = icm4n607_match_ts_with_fifo_cnt(packet_cnt,mTask.hwSampleTime,(int64_t)estimated_sample_intvl);
    actual_hwSampleTime = mTask.hwSampleTime + (packet_cnt - real_packet_cnt)*estimated_sample_intvl;

    if(actual_hwSampleTime > swSampleTime)
        actual_hwSampleTime = swSampleTime;
    
    /* End Pre Process lastSampleTime and hwSampleTime */
    if(mTask.hwSampleTime != mTask.last_hwSampleTime ){
        realSampleTime = calcFakeInterruptTime(swSampleTime, actual_hwSampleTime, mTask.lastSampleTime,
                                           mTask.sensors[ACC].fifo_rate, mTask.sensors[ACC].configed,
                                           accEventSize + accEventSize_Discard,
                                           mTask.sensors[GYR].fifo_rate, mTask.sensors[GYR].configed,
                                           gyroEventSize + gyroEventSize_Discard);
    }else{
        // if hwSampleTime equal to last_hwSampleTime time, it does not need to calculate the Fake Interrupt time(because hwsample is not updated because of no current Interrupt comming)
        if(isrwm_after_set_rate){ 
            realSampleTime = mTask.lastSampleTime + long_diff;

            if(realSampleTime > swSampleTime)
                realSampleTime = swSampleTime;
                
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
    //INVLOG(LOG_INFO, "ICM4N607Convert, swSampleTime=%llu, hwSampleTime=%llu, realSampleTime=%llu, lastSampleTime=%llu, now=%llu, unmask_eint_time=%llu\n",
        //swSampleTime, mTask.hwSampleTime, realSampleTime, mTask.lastSampleTime, rtcGetTime(), mTask.unmask_eint_time);

#if DEBUG_FIFO_DATA
    INVLOG(LOG_INFO, "hwT %llu,rhwT %llu,SwT %llu rT %llu,itl %llu,aS %d,gyS %d,DbR %d", mTask.hwSampleTime,actual_hwSampleTime,swSampleTime,realSampleTime,estimated_sample_intvl,accEventSize,gyroEventSize,mTask.fifoDataToRead);
#endif
#if DEBUG_TEMP_DATA
	INVLOG(LOG_ERROR, "Temp1: %f \n",(double)mTask.chip_temper);
#endif
    mTask.hwSampleTime = realSampleTime;
    mTask.lastSampleTime = realSampleTime;
    mTask.last_hwSampleTime = realSampleTime;
    
    txTransferDataInfo(&mTask.dataInfo, accEventSize, gyroEventSize, realSampleTime, data, mTask.chip_temper);
      
    mt_eint_unmask(mTask.hw->eint_num);
    mTask.unmask_eint_time = rtcGetTime();

#if DEBUG_MCLK
    INVLOG(LOG_INFO, "ICM4N607Convert need_mclk_cnt %d, +1 after function, pwr_sta %x\n",mTask.need_mclk_cnt,mTask.pwr_sta);
#endif
    	/* set IDLE bit only if it is not set yet */
	if (mTask.need_mclk_cnt == 0 || !(mTask.pwr_sta & ICM4N607_PWR_MGMT_0_IDLE_MASK)) {

	    // set IDLE bit and wait 10ms to skip always checking process
		mTask.pwr_sta |= ICM4N607_PWR_MGMT_0_IDLE_MASK; 
		SPI_WRITE((uint8_t)ICM4N607_PWR_MGMT_0,mTask.pwr_sta);

        /* Increment the counter to keep track of number of MCLK requesters */
	    mTask.need_mclk_cnt++;	
	
        return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__); 
	} 

     sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
     return 0;
}

static void ICM4N607Isr1(int arg)
{
    if (mTask.latch_time_id < 0) {
        mTask.hwSampleTime = rtcGetTime();
        //INVLOG(LOG_ERROR, "ICM4N607Isr1, mTask.latch_time_id < 0, mTask.hwSampleTime=%llu\n", mTask.hwSampleTime);
    } else {
        mTask.hwSampleTime = get_latch_time_timestamp(mTask.latch_time_id);
      //INVLOG(LOG_ERROR, "ICM4N607Isr1, mTask.hwSampleTime=%llu, now_rtc=%llu\n", mTask.hwSampleTime, rtcGetTime());
    }
    accGyroHwIntCheckStatus();
}

static int ICM4N607IntStatusCheck(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                  void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                  void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    inv_icm4n607_read_reg(ICM4N607_INT_STATUS, 1, &mTask.statusBuffer,0);
    inv_icm4n607_read_reg(ICM4N607_INT_STATUS2, 1, &mTask.wakeupBuffer,0);
    #if SUPPORT_HW_FREEFALL
    SPI_READ(ICM4N607_REG_INT_STATUS3, 1, &mTask.freefallBuffer);
    #endif       
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

static int ICM4N607IntHandling(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                               void *inBuf, uint8_t inSize, uint8_t elemInSize,
                               void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    union EmbeddedDataPoint trigger_axies;
    uint8_t fifo_status = mTask.statusBuffer[1];
    uint8_t anymo_status = mTask.wakeupBuffer[1];
    #if SUPPORT_HW_FREEFALL
    uint8_t int_status3 = mTask.freefallBuffer[1];
    #endif   

   
    #if DEBUG_FIFO_DATA
   // INVLOG(LOG_INFO, "ICM4N607IntHandling,  WM INT:0x%x, AnyMotion:0x%x,\n", fifo_status, anymo_status);
    #endif 

    #if SUPPORT_HW_FREEFALL
    if (mTask.sensors[FREEFALL].powered) {
        if (int_status3 & BIT_LOWG_DET_INT) {
            INVLOG(LOG_INFO, "int_status3 %x\n",int_status3);
            INVLOG(LOG_INFO, "Detected freefall\n");
            #if SUPPORT_FSM_FREEFALL
            union EmbeddedDataPoint sample;
            sample.idata = 2;
            osEnqueueEvt(sensorGetMyEventType(SENS_TYPE_FREE_FALL), sample.vptr, NULL);
            #endif
        }
    }
    #endif
    
    if (mTask.sensors[ANYMO].powered) {
        if (anymo_status & 0x07) {
            trigger_axies.idata = (anymo_status & 0x07);
            INVLOG(LOG_INFO, "Detected any motion\n");
            osEnqueueEvt(EVT_SENSOR_ANY_MOTION, trigger_axies.vptr, NULL);
        }
    }
    if (fifo_status & ICM4N607_INT_STATUS_INT_STATUS_FIFO_FULL_MASK) {
        ICM4N607ResetFifo();
        mt_eint_unmask(mTask.hw->eint_num);
        return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    }else if (fifo_status & ICM4N607_INT_STATUS_INT_STATUS_FIFO_THS_MASK) {
       if (!mTask.sensors[ACC].configed && !mTask.sensors[GYR].configed) {
           INVLOG(LOG_INFO, "Unexpected FIFO WM INTR fired\n");
           // reset fifo
           ICM4N607ResetFifo();
           mt_eint_unmask(mTask.hw->eint_num);
           return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
       } else {
           isrwm_after_set_rate = true;
           accGyroInterruptOccur();
       }
    } else
        mt_eint_unmask(mTask.hw->eint_num);

    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);

    return 0;
}

static int ICM4N607EintRegistration(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                    void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                    void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    mt_eint_dis_hw_debounce(mTask.hw->eint_num);
    mt_eint_registration(mTask.hw->eint_num, EDGE_SENSITIVE, HIGH_LEVEL_TRIGGER, ICM4N607Isr1, EINT_INT_UNMASK,
        EINT_INT_AUTO_UNMASK_OFF);
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static void ICM4N607SetDebugTrace(int32_t trace)
{
 /*
    mTask.debug_trace = trace;
    INVLOG(LOG_ERROR, "%s ==> trace:%d\n", __func__, mTask.debug_trace);
 */
}


static void inv_icm4n607_configure_wom(const uint8_t wom_x_th, const uint8_t wom_y_th, const uint8_t wom_z_th,
                               ICM4N607_WOM_CONFIG_WOM_INT_MODE_t wom_int, ICM4N607_WOM_CONFIG_WOM_INT_DUR_t wom_dur)
{
	inv_icm4n607_write_reg(ICM4N607_ACCEL_WOM_X_THR_MREG_TOP1, wom_x_th, 0);
    inv_icm4n607_write_reg(ICM4N607_ACCEL_WOM_Y_THR_MREG_TOP1, wom_y_th, 0);
    inv_icm4n607_write_reg(ICM4N607_ACCEL_WOM_Z_THR_MREG_TOP1, wom_z_th, 0);
    
	// Compare current sample with the previous sample and WOM from the 3 axis are ORed or ANDed to produce WOM signal.

	mTask.wom_cfg &= ~ICM4N607_WOM_CONFIG_WOM_INT_MODE_MASK;
	mTask.wom_cfg |= (uint8_t)ICM4N607_WOM_CONFIG_WOM_MODE_CMP_PREV | (uint8_t)wom_int;
	
	// Configure the number of overthreshold event to wait before producing the WOM signal.
	mTask.wom_cfg &= ~ICM4N607_WOM_CONFIG_WOM_INT_DUR_MASK;
	mTask.wom_cfg |= (uint8_t)wom_dur;
	inv_icm4n607_write_reg(ICM4N607_WOM_CONFIG, mTask.wom_cfg,0);

}

static int anyMotionPowerOn(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    INVLOG(LOG_INFO, "Enter anyMotionPowerOn\n");

    // if acc is not in streaming mode & gyro is not on, set WOM in low power mode
    #if SUPPORT_HW_FREEFALL
    if ( !mTask.sensors[ACC].powered && !mTask.sensors[GYR].powered && !mTask.sensors[FREEFALL].powered) {
    #else
    if ( !mTask.sensors[ACC].powered && !mTask.sensors[GYR].powered ) {
    #endif
        ICM4N607_set_acc_power_mode(BIT_ACC_LP_MODE);
    } else {
        ICM4N607_set_acc_power_mode(BIT_ACC_LN_MODE);  
    }
    
   // enable wom
    mTask.wom_cfg &= ~ICM4N607_WOM_CONFIG_WOM_EN_MASK;
	mTask.wom_cfg |= (uint8_t)ICM4N607_WOM_CONFIG_WOM_EN_ENABLE;
    inv_icm4n607_write_reg(ICM4N607_WOM_CONFIG, mTask.wom_cfg,0);

    mTask.int_src1 &= ~(ICM4N607_INT_SOURCE1_INT_WOM_X_INT1_EN_MASK 
			   | ICM4N607_INT_SOURCE1_INT_WOM_Y_INT1_EN_MASK 
			   | ICM4N607_INT_SOURCE1_INT_WOM_Z_INT1_EN_MASK);
			   
    mTask.int_src1 |= ICM4N607_INT_SOURCE1_INT_WOM_X_INT1_EN_MASK
                   |ICM4N607_INT_SOURCE1_INT_WOM_Y_INT1_EN_MASK
                   |ICM4N607_INT_SOURCE1_INT_WOM_Z_INT1_EN_MASK;

    inv_icm4n607_write_reg(ICM4N607_INT_SOURCE1, mTask.int_src1,0);

    mTask.sensors[ANYMO].powered = true;

    mt_eint_unmask(mTask.hw->eint_num);
    return  spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

static int anyMotionPowerOff(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                             void *inBuf, uint8_t inSize, uint8_t elemInSize,
                             void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    INVLOG(LOG_INFO, "anyMotionPowerOff\n");

    mTask.sensors[ANYMO].configed = false;
    mTask.sensors[ANYMO].powered = false;
    /* for wom disable */
    mTask.int_src1 &= ~(ICM4N607_INT_SOURCE1_INT_WOM_X_INT1_EN_MASK 
			   | ICM4N607_INT_SOURCE1_INT_WOM_Y_INT1_EN_MASK 
			   | ICM4N607_INT_SOURCE1_INT_WOM_Z_INT1_EN_MASK);
			   
    inv_icm4n607_write_reg(ICM4N607_INT_SOURCE1, mTask.int_src1,0);

    #if defined(CFG_HW_STEP_COUNTER_SUPPORT) || SUPPORT_HW_FREEFALL
	if(mTask.sensors[STEPCNT].powered == false && mTask.sensors[FREEFALL].powered == false) {
		mTask.wom_cfg &= ~ICM4N607_WOM_CONFIG_WOM_EN_MASK;
		inv_icm4n607_write_reg(ICM4N607_WOM_CONFIG, mTask.wom_cfg,0);
	}
	#else
        mTask.wom_cfg &= ~ICM4N607_WOM_CONFIG_WOM_EN_MASK;
		inv_icm4n607_write_reg(ICM4N607_WOM_CONFIG, mTask.wom_cfg,0);
	#endif

    #ifdef CFG_HW_STEP_COUNTER_SUPPORT
    if (!mTask.sensors[ACC].powered && !mTask.sensors[ACC].configed && ! mTask.sensors[STEPCNT].powered && !mTask.sensors[FREEFALL].powered) { 
    #else
    if (!mTask.sensors[ACC].powered && !mTask.sensors[ACC].configed) {
    #endif
        mTask.pwr_sta &= ~BIT_ACC_LN_MODE;
        inv_icm4n607_write_reg(ICM4N607_PWR_MGMT_0, mTask.pwr_sta, 200);
        INVLOG(LOG_INFO, "anyMotionPowerOff, acc off, set power down mode %x\n",mTask.pwr_sta);
    }

    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
}

#ifdef SUPPORT_HW_FREEFALL

#define lowg_peak_th_sel        17
#define lowg_time_th_sel         6
#define lowg_peak_th_hyst_sel    4
#define highg_peak_th_sel        4
#define highg_time_th_sel        2
#define highg_peak_th_hyst_sel   4
#define ff_debounce_duration     1
#define ff_min_duration_cm       0
#define ff_max_duration_cm       0

/* set pedometer Threshold */
static void icm4n607_config_lowg_parameter()
{

	uint8_t databuf = 0;
	
    SPI_WRITE(ICM4N607_REG_BANK_SEL, 4);// Bank 4

    //config APEX config4 
	databuf = 2<<6 | lowg_peak_th_hyst_sel <<3 | highg_peak_th_hyst_sel;
	SPI_WRITE(ICM4N607_REG_APEX_CONFIG4, databuf);
    INVLOG(LOG_INFO,"set ICM4N607_REG_APEX_CONFIG4 %x\n", databuf);

    //config APEX config5
	databuf = (lowg_peak_th_sel << 3 ) | (lowg_time_th_sel);
	SPI_WRITE(ICM4N607_REG_APEX_CONFIG5, databuf);
 	INVLOG(LOG_INFO,"set ICM4N607_REG_APEX_CONFIG5 %x\n", databuf);

    //config APEX config10
	databuf = (ff_min_duration_cm << 5 ) | (ff_max_duration_cm <<2)| ff_debounce_duration ;
	SPI_WRITE(ICM4N607_REG_APEX_CONFIG10, databuf);
 	INVLOG(LOG_INFO,"set ICM4N607_REG_APEX_CONFIG10 %x\n", databuf);

	SPI_WRITE(ICM4N607_REG_BANK_SEL, 0);// Bank 0

}

static void icm4n607_enable_freefall_int(bool enable)
{
    SPI_WRITE(ICM4N607_REG_BANK_SEL, 4);// Bank 4
	
	if (enable)
		mTask.int_source6 |= (uint8_t)(BIT_LOWG_DET_INT1_EN);
	else
		mTask.int_source6 &= (uint8_t)(~BIT_LOWG_DET_INT1_EN);

    SPI_WRITE(ICM4N607_REG_INT_SOURCE6,mTask.int_source6);
    INVLOG(LOG_INFO, "icm406xx_enable_freefall_int (enable = %d) int6 %x\n", enable,mTask.int_source6);
	
    SPI_WRITE(ICM4N607_REG_BANK_SEL, 0);// Bank 0
}

static void icm406xx_sc_sd_EnableFreeFall_phase_2()
{  
	//enable dmp_init_en 
	SPI_WRITE(ICM4N607_REG_SIGNAL_PATH_RESET_REG,DMP_INIT_EN,60000);

    icm4n607_enable_freefall_int(true);

    mTask.apex_config1 &= (~PED_ENABLE);   //clear PED_ENABLE 
    mTask.apex_config1 |= BIT_LOWG_EN;
    SPI_WRITE(ICM4N607_REG_APEX_CONFIG0,mTask.apex_config1);
  
	INVLOG(LOG_INFO,"write freefall ENABLE %x\n",mTask.apex_config1);
}

static void icm406xx_sc_sd_EnableFreeFall_phase_1()
{
 
    if(true == mTask.DMP_first_config){
       // ret = icm406xx_set_pedometer_threshold();        
       
      	ICM4N607Set_Wom_threshold(DEF_WOM_THRESHOLD);
	
		icm406xx_sc_DMP_powersave(false);
	}  

       //set freefall thereshold
    icm4n607_config_lowg_parameter();
    
	icm406xx_sc_initDMP();
	
	if(true == mTask.DMP_first_config){
	    if(mTask.DMP_from_sram == false){
	        icm4n607_sc_DMP_mem_reset();
            mTask.DMP_reset_path_check = true;
	    }
        mTask.DMP_first_config = false;
	}
}

static int freefall_enable_phase2(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                      void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                      void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint8_t databuf = 0;

    INVLOG(LOG_INFO, "freefall_enable_phase2\n");
    if(true == mTask.DMP_reset_path_check) {
        mTask.DMP_reset_path_check = false;
        databuf = mTask.stepstatusBuffer[1];
        
        if(databuf != 0x00)
             INVLOG(LOG_ERROR, "DMP may do not reset correctly !!!!\n");    
    }
       
    icm406xx_sc_sd_EnableFreeFall_phase_2();

    inv_icm4n607_switch_off_mclk();
    mTask.sensors[FREEFALL].powered = true;

/*
    union EmbeddedDataPoint sample;
    sample.idata = 1;
    osEnqueueEvt(sensorGetMyEventType(SENS_TYPE_FREE_FALL), sample.vptr, NULL);*/

    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);

}

static int freefall_enable_phase1(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                      void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                      void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    INVLOG(LOG_INFO, "freefall_enable_phase1\n");
    /* get 0x56 apex_cnfig0 default value  */
    mTask.apex_config1 = mTask.stepBuffer[1];
    mTask.int_source6  = mTask.stepSourceBuffer[1];
     
    icm406xx_sc_sd_EnableFreeFall_phase_1();
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);

}

static int FreeFallPowerOn(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint32_t setting_hwrate;
    uint32_t fiforate;
    
    INVLOG(LOG_INFO, "Enter FreeFallPowerOn\n");

    /* 1st we force to set hw rate not slow than 50hz */
#if SUPPORT_HW_FREEFALL_HP
    mTask.hw_min_rate = SENSOR_HZ(500.0f);
#else
    mTask.hw_min_rate = SENSOR_HZ(50.0f);
#endif
    
     // if acc is not in streaming mode & gyro is not on, set acc in low power mode,config acc odr @50hz
    if ( !mTask.sensors[ACC].powered && !mTask.sensors[GYR].powered ) {
         // just set accel in ln mode in free fall mode do not touch odr setting 
        ICM4N607_set_acc_power_mode(BIT_ACC_LN_MODE);
    } else {
        // acc or gyro is on ,set hw_rate not smaller than 50hz,ln mode
#if SUPPORT_HW_FREEFALL_HP
        ICM4N607_set_acc_power_mode(BIT_ACC_LP_MODE);  
#else
        ICM4N607_set_acc_power_mode(BIT_ACC_LP_MODE);
#endif
        //record previous chip hardware_rate
        mTask.pre_hwRate = max(mTask.sensors[ACC].hwRate,mTask.sensors[GYR].hwRate);
        fiforate =  max(mTask.sensors[ACC].fifo_rate,mTask.sensors[GYR].fifo_rate);
        
        if(mTask.pre_hwRate < mTask.hw_min_rate) {
            //update new hardware rate 
            setting_hwrate = ICM4N607ConfigFDR(fiforate,mTask.hw_min_rate);
            icm4n607_reconfig_samplerate(mTask.sensors[ACC].fifo_rate,mTask.sensors[GYR].fifo_rate,setting_hwrate);
        }
    }
    
   /*check apex_config1 default value first  */
    SPI_READ(ICM4N607_REG_APEX_CONFIG0, 1, &mTask.stepBuffer);

    /*get default int_source6 value */
    SPI_WRITE(ICM4N607_REG_BANK_SEL, 4);// Bank 4
    SPI_READ(ICM4N607_REG_INT_SOURCE6, 1, &mTask.stepSourceBuffer);
    SPI_WRITE(ICM4N607_REG_BANK_SEL, 0);// Bank 0
    
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;
}
static int FreeFallPowerOff(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                             void *inBuf, uint8_t inSize, uint8_t elemInSize,
                             void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    INVLOG(LOG_INFO, "Enter stepCountPowerOff\n");
    uint32_t setting_hwrate;
    uint32_t fiforate;
    /* 1st disable related pedometer setting */

    //disable pedometer interrupt
	icm4n607_enable_freefall_int(false);

    //disable freefall_en 
	mTask.apex_config1 &= (~BIT_LOWG_EN);   //clear BIT_LOWG_EN
    SPI_WRITE(ICM4N607_REG_APEX_CONFIG0,mTask.apex_config0);

    /* 2st recover the min odr to common */
    mTask.hw_min_rate = SENSOR_HZ(12.5f);
    
    /* 3st recover acc setting  */
      /*  accel disable */
    if (!mTask.sensors[ACC].powered && !mTask.sensors[ANYMO].powered) {
        ICM4N607_set_acc_power_mode(BIT_ACC_OFF);
        INVLOG(LOG_INFO, "stepCountPowerOff, enter acc poweroff mode\n");
    }else if( mTask.sensors[ACC].powered ){
        ICM4N607_set_acc_power_mode(BIT_ACC_LN_MODE);
        INVLOG(LOG_INFO, "stepCountPowerOff, enter acc ln mode\n");
    }else{
        ICM4N607_set_acc_power_mode(BIT_ACC_LP_MODE);
        INVLOG(LOG_INFO, "stepCountPowerOff, enter acc lp mode\n");
    }

    /* 4st recover odr setting if needed when there is sensor streaming */
    if (mTask.sensors[ACC].powered || mTask.sensors[GYR].powered ) {
        
        if (mTask.pre_hwRate != max(mTask.sensors[ACC].hwRate,mTask.sensors[GYR].hwRate)) {
            //update new hardware rate 
            fiforate =  max(mTask.sensors[ACC].preRealRate,mTask.sensors[GYR].preRealRate);
            setting_hwrate = ICM4N607ConfigFDR(fiforate,mTask.hw_min_rate);
            icm4n607_reconfig_samplerate(mTask.sensors[ACC].preRealRate,mTask.sensors[GYR].preRealRate,setting_hwrate);
        }

    }

    mTask.sensors[FREEFALL].powered = false;

    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;
}

#endif

#ifdef CFG_HW_STEP_COUNTER_SUPPORT

static int inv_icm4n607_init_apex_parameters_struct(inv_icm4n607_apex_parameters_t *apex_inputs)
{
	int status = 0;
	
	/* Default parameters at POR */
	apex_inputs->pedo_amp_th          = ICM4N607_APEX_CONFIG3_PEDO_AMP_TH_2080374_MG;
	apex_inputs->pedo_step_cnt_th     = 0x5;
	apex_inputs->pedo_step_det_th     = 0x2;
	apex_inputs->pedo_sb_timer_th     = ICM4N607_APEX_CONFIG4_PEDO_SB_TIMER_TH_150_SAMPLES;
	apex_inputs->pedo_hi_enrgy_th     = ICM4N607_APEX_CONFIG4_PEDO_HI_ENRGY_TH_107;
	apex_inputs->tilt_wait_time       = ICM4N607_APEX_CONFIG5_TILT_WAIT_TIME_4S;
	apex_inputs->power_save_time      = ICM4N607_APEX_CONFIG2_DMP_POWER_SAVE_TIME_SEL_8S;
	apex_inputs->power_save           = ICM4N607_APEX_CONFIG0_DMP_POWER_SAVE_EN;
	apex_inputs->sensitivity_mode     = ICM4N607_APEX_CONFIG9_SENSITIVITY_MODE_NORMAL;
	apex_inputs->low_energy_amp_th    = ICM4N607_APEX_CONFIG2_LOW_ENERGY_AMP_TH_SEL_2684354MG;
	apex_inputs->smd_sensitivity      = ICM4N607_APEX_CONFIG9_SMD_SENSITIVITY_0;
	apex_inputs->ff_debounce_duration = ICM4N607_APEX_CONFIG9_FF_DEBOUNCE_DURATION_2000_MS;
	apex_inputs->ff_max_duration_cm   = ICM4N607_APEX_CONFIG12_FF_MAX_DURATION_204_CM;
	apex_inputs->ff_min_duration_cm   = ICM4N607_APEX_CONFIG12_FF_MIN_DURATION_10_CM;
	apex_inputs->lowg_peak_th         = ICM4N607_APEX_CONFIG10_LOWG_PEAK_TH_563MG;
	apex_inputs->lowg_peak_hyst       = ICM4N607_APEX_CONFIG5_LOWG_PEAK_TH_HYST_156MG;
	apex_inputs->lowg_samples_th      = ICM4N607_APEX_CONFIG10_LOWG_TIME_TH_1_SAMPLE;
	apex_inputs->highg_peak_th        = ICM4N607_APEX_CONFIG11_HIGHG_PEAK_TH_2500MG;
	apex_inputs->highg_peak_hyst      = ICM4N607_APEX_CONFIG5_HIGHG_PEAK_TH_HYST_156MG;
	apex_inputs->highg_samples_th     = ICM4N607_APEX_CONFIG11_HIGHG_TIME_TH_1_SAMPLE;

	return status;
}

/* set pedometer Threshold */
static void inv_icm4n607_configure_apex_parameters(const inv_icm4n607_apex_parameters_t *apex_inputs)
{
	uint8_t apexConfig[7];
	
	/* DMP cannot be configured if it is running, hence make sure all APEX algorithms are off */
	
	inv_icm4n607_switch_on_mclk();

	/* Power Save mode parameters & Additionnal parameters for Pedometer in Slow Walk mode */
	/* ICM4N607_APEX_CONFIG2_MREG_TOP1 */
	apexConfig[0] = (uint8_t)apex_inputs->power_save_time | (uint8_t)apex_inputs->low_energy_amp_th;

	/* Pedometer parameters */
	/* ICM4N607_APEX_CONFIG3_MREG_TOP1 */
	apexConfig[1] = (uint8_t)apex_inputs->pedo_amp_th | (apex_inputs->pedo_step_cnt_th & ICM4N607_APEX_CONFIG3_PEDO_STEP_CNT_TH_SEL_MASK);

	/* ICM4N607_APEX_CONFIG4_MREG_TOP1 */
	apexConfig[2] = ((apex_inputs->pedo_step_det_th << ICM4N607_APEX_CONFIG4_PEDO_STEP_DET_TH_SEL_POS) & ICM4N607_APEX_CONFIG4_PEDO_STEP_DET_TH_SEL_MASK)
					| (uint8_t)apex_inputs->pedo_sb_timer_th | (uint8_t)apex_inputs->pedo_hi_enrgy_th;

    
	inv_icm4n607_write_reg(ICM4N607_APEX_CONFIG2_MREG_TOP1, apexConfig[0],0);
	inv_icm4n607_write_reg(ICM4N607_APEX_CONFIG3_MREG_TOP1, apexConfig[1],0);
    inv_icm4n607_write_reg(ICM4N607_APEX_CONFIG4_MREG_TOP1, apexConfig[2],0);


	
	/* free fall parameter, SMD parameter and parameters for Pedometer in Slow Walk mode */
	/* ICM4N607_APEX_CONFIG9_MREG_TOP1 */
	apexConfig[3] = (uint8_t)apex_inputs->ff_debounce_duration | (uint8_t)apex_inputs->smd_sensitivity | (uint8_t)apex_inputs->sensitivity_mode;
    inv_icm4n607_write_reg(ICM4N607_APEX_CONFIG9_MREG_TOP1, apexConfig[3],0);

    INVLOG(LOG_INFO, "inv_icm4n607_configure_apex_parameters-top1-2349  %x %x %x %x\n",apexConfig[0],apexConfig[1],apexConfig[2],apexConfig[3]);

	#if SUPPORT_HW_FREEFALL
	/* Lowg and highg parameters and free fall parameters */
	/* ICM4N607_APEX_CONFIG10_MREG_TOP1 */
	apexConfig[1] = (uint8_t)apex_inputs->lowg_peak_th | (uint8_t)apex_inputs->lowg_samples_th;

	/* ICM4N607_APEX_CONFIG11_MREG_TOP1 */
	apexConfig[2] = (uint8_t)apex_inputs->highg_peak_th | (uint8_t)apex_inputs->highg_samples_th;

	inv_icm4n607_write_reg(ICM4N607_APEX_CONFIG9_MREG_TOP1, 3, &apexConfig[0]);
	
	
	/* ICM4N607_APEX_CONFIG12_MREG_TOP1 */
	apexConfig[0] = (uint8_t)apex_inputs->ff_max_duration_cm | (uint8_t)apex_inputs->ff_min_duration_cm;
	
	inv_icm4n607_write_reg(ICM4N607_APEX_CONFIG12_MREG_TOP1, 1, &apexConfig[0]);
    #endif
	inv_icm4n607_switch_off_mclk();

}

static void icm4n607_sc_sd_EnablePedometer_phase_2()
{
    //set pedometer thereshold
    //icm406xx_set_pedometer_threshold();

    
    mTask.apex_config0 &= ~ICM4N607_APEX_CONFIG0_DMP_INIT_EN_MASK;
	mTask.apex_config0 |= (uint8_t)ICM4N607_APEX_CONFIG0_DMP_INIT_EN;

    //enable dmp_init_en wait 50ms
	inv_icm4n607_write_reg(ICM4N607_APEX_CONFIG0,mTask.apex_config0,50000);
    INVLOG(LOG_INFO,"write dmp_init_en %x\n",mTask.apex_config0);

    //enable pedo_en in apex config1
    mTask.apex_config1 &= ~ICM4N607_APEX_CONFIG1_DMP_PEDO_EN_MASK;
	mTask.apex_config1 |= (uint8_t)ICM4N607_APEX_CONFIG1_PEDO_EN_EN;
	inv_icm4n607_write_reg(ICM4N607_APEX_CONFIG1,mTask.apex_config1,0);
	
    /* currently do not use step counter interrupt */
    //icm406xx_enable_pedometer_int(true);
	INVLOG(LOG_INFO,"write PED_ENABLE %x\n",mTask.apex_config1);
}

static void icm4n607_sc_sd_EnablePedometer_phase_1()
{
    if(true == mTask.DMP_first_config){
       // ret = icm406xx_set_pedometer_threshold();     
#if defined(CFG_HW_STEP_COUNTER_SUPPORT) || SUPPORT_HW_FREEFALL
    inv_icm4n607_init_apex_parameters_struct(&mTask.apex_inputs);
    inv_icm4n607_configure_apex_parameters(&mTask.apex_inputs);
#endif
		icm4n607_sc_DMP_powersave(true);
	}  

	icm4n607_sc_initDMP();
	
	if(true == mTask.DMP_first_config){
	    icm4n607_sc_DMP_mem_reset();
        mTask.DMP_first_config = false;
        mTask.DMP_reset_path_check = true;
	}
}


static int stepCount_enable_phase2(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                      void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                      void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{

    INVLOG(LOG_INFO, "stepCount_enable_phase2 %x\n",mTask.stepstatusBuffer[1]);
    if(true == mTask.DMP_reset_path_check) {
        mTask.DMP_reset_path_check = false;

        //re-asign apex_config0 value
        mTask.apex_config0 = mTask.stepstatusBuffer[1];
        
        if((mTask.apex_config0 & ICM4N607_APEX_CONFIG0_DMP_SRAM_RESET_EN_MASK) != ICM4N607_APEX_CONFIG0_DMP_SRAM_RESET_DIS)
             INVLOG(LOG_ERROR, "DMP may do not reset correctly !!!!\n");    
    }
       
    icm4n607_sc_sd_EnablePedometer_phase_2();

    inv_icm4n607_switch_off_mclk();
    mTask.sensors[STEPCNT].powered = true;
    
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);

}

static int stepCount_enable_phase1(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                      void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                      void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    /* get  apex_cnfig0 apex_config1 default value  */

    mTask.apex_config0  = mTask.stepBuffer0[1];
    mTask.apex_config1 = mTask.stepBuffer[1];

    INVLOG(LOG_INFO, "stepCount_enable_phase1 apex_config0 %x apex_config1 %x\n",mTask.apex_config0,mTask.apex_config1);
    //mTask.int_source6  = mTask.stepSourceBuffer[1];
     
    icm4n607_sc_sd_EnablePedometer_phase_1();
    return spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);

}

static int stepCountPowerOn(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint32_t setting_hwrate;
    //uint32_t fiforate;
    INVLOG(LOG_INFO, "Enter stepCountPowerOn\n");

    /* 1st we force to set hw rate not slow than 50hz, because DMP @ 50hz */
    mTask.hw_min_rate = SENSOR_HZ(50.0f);
    
     // if acc is not in streaming mode & gyro is not on, set acc in low power mode,config acc odr @50hz
    if ( !mTask.sensors[ACC].powered && !mTask.sensors[GYR].powered ) {
         // direct config odr to 50hz lp because no straming sensor
        ICM4N607_set_acc_power_mode(BIT_ACC_LP_MODE);
    } else {
        // acc or gyro is on ,set hw_rate not smaller than 50hz,ln mode
        ICM4N607_set_acc_power_mode(BIT_ACC_LN_MODE);  
          //record previous chip hardware_rate
        mTask.pre_hwRate = max(mTask.sensors[ACC].hwRate,mTask.sensors[GYR].hwRate);
        //fiforate =  max(mTask.sensors[ACC].fifo_rate,mTask.sensors[GYR].fifo_rate);
        
        if(mTask.pre_hwRate < mTask.hw_min_rate) {
            setting_hwrate = mTask.hw_min_rate;//ICM4N607ConfigFDR(fiforate,mTask.hw_min_rate);  currently there is no information about ICM4N607 FDR function
            icm4n607_reconfig_samplerate(mTask.sensors[ACC].fifo_rate,mTask.sensors[GYR].fifo_rate,setting_hwrate);
        }
    }

  //  mTask.sensors[STEPCNT].powered = true;   //

    inv_icm4n607_switch_on_mclk();
    
   /*check apex_config1 & apex_config0 default value first  */
    inv_icm4n607_read_reg(ICM4N607_APEX_CONFIG1, 1, &mTask.stepBuffer,0);
    inv_icm4n607_read_reg(ICM4N607_APEX_CONFIG0, 1, &mTask.stepBuffer0,0);
    
    /*get default int_source6 value  if necessary later....*/
    
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;
}

static int stepCountPowerOff(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                             void *inBuf, uint8_t inSize, uint8_t elemInSize,
                             void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    uint32_t setting_hwrate;
    uint32_t fiforate;
    INVLOG(LOG_INFO, "Enter stepCountPowerOff\n");

    /* 1st disable related pedometer setting */

    //disable pedometer interrupt
	//icm406xx_enable_pedometer_int(false);

    //disable pedo_en 
	mTask.apex_config1 &= ~ICM4N607_APEX_CONFIG1_DMP_PEDO_EN_MASK;
	mTask.apex_config1 |= (uint8_t)ICM4N607_APEX_CONFIG1_PEDO_EN_DIS;  //clear PED_ENABLE
	inv_icm4n607_write_reg(ICM4N607_APEX_CONFIG1, mTask.apex_config1,0);

    /* 2st recover the min odr to common */
    mTask.hw_min_rate = SENSOR_HZ(12.5f);
    
    /* 3st recover acc setting  */
      /*  accel disable */
    if (!mTask.sensors[ACC].powered && !mTask.sensors[ANYMO].powered) {
        ICM4N607_set_acc_power_mode(BIT_ACC_OFF);
        INVLOG(LOG_INFO, "stepCountPowerOff, enter acc poweroff mode\n");
    }else if( mTask.sensors[ACC].powered ){
        ICM4N607_set_acc_power_mode(BIT_ACC_LN_MODE);
        INVLOG(LOG_INFO, "stepCountPowerOff, enter acc ln mode\n");
    }else{
        ICM4N607_set_acc_power_mode(BIT_ACC_LP_MODE);
        INVLOG(LOG_INFO, "stepCountPowerOff, enter acc lp mode\n");
    }

    /* 4st recover odr setting if needed when there is sensor streaming */
    if (mTask.sensors[ACC].powered || mTask.sensors[GYR].powered ) {
        
        if (mTask.pre_hwRate != max(mTask.sensors[ACC].fifo_rate,mTask.sensors[GYR].fifo_rate)) {
            //update new hardware rate 
            fiforate =  max(mTask.sensors[ACC].preRealRate,mTask.sensors[GYR].preRealRate);
            setting_hwrate = fiforate; //ICM4N607ConfigFDR(fiforate,mTask.hw_min_rate);
            icm4n607_reconfig_samplerate(mTask.sensors[ACC].preRealRate,mTask.sensors[GYR].preRealRate,setting_hwrate);
        }

    }

    /*currently we let DMP always reset again in next enable when step counter was disable before */
    mTask.DMP_first_config = true;
    //clear previous step counter if dmp is reset
    mTask.last_step_cnt = 0;

    mTask.sensors[STEPCNT].powered = false;

    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;
}

static int StepCntSendData(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                           void *inBuf, uint8_t inSize, uint8_t elemInSize,
                           void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    union EmbeddedDataPoint step_cnt;
    uint32_t cur_step_cnt;
   
    cur_step_cnt = (mTask.stepBuffer[1] | (mTask.stepBuffer[2] << 8));
#if DEBUG_STEP_DATA
    INVLOG(LOG_INFO, "StepCntSendData buf1 0x%x,buf2 0x%x\n", mTask.stepBuffer[1], mTask.stepBuffer[2]);
#endif
    if (cur_step_cnt != mTask.last_step_cnt) {
        /* Check for possible overflow */
        if (cur_step_cnt < mTask.last_step_cnt) {
            mTask.total_step_cnt += cur_step_cnt + (0xFFFF - mTask.last_step_cnt);
        } else {
            mTask.total_step_cnt += (cur_step_cnt - mTask.last_step_cnt);
        }

        mTask.last_step_cnt = cur_step_cnt;
        step_cnt.idata = mTask.total_step_cnt;
#if DEBUG_STEP_DATA
        INVLOG(LOG_INFO, "step count =%d\n", step_cnt.idata);
#endif
        osEnqueueEvt(EVT_SENSOR_STEP_COUNTER, step_cnt.vptr, NULL);
    }
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;
}
static int stepCntGetData(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                          void *inBuf, uint8_t inSize, uint8_t elemInSize,
                          void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    //INVLOG(LOG_INFO, "stepCntGetData\n");
    inv_icm4n607_read_reg(ICM4N607_APEX_DATA0, 2, &mTask.stepBuffer,0);
    spiBatchTxRx(&mTask.mode, spiCallBack, next_state, __FUNCTION__);
    return 0;
}

#endif

static void accGetSensorInfo(struct sensorInfo_t *data)
{
    strncpy(data->name, ACC_NAME, sizeof(data->name));
}

static void gyroGetSensorInfo(struct sensorInfo_t *data)
{
    strncpy(data->name, GYRO_NAME, sizeof(data->name));
}


static void sensorCoreRegistration(void)
{
    struct sensorCoreInfo mInfo;
    INVLOG(LOG_INFO, "ICM4N607RegisterCore\n");

    /* Register sensor Core */
    memset(&mInfo, 0x00, sizeof(struct sensorCoreInfo));
    mInfo.sensType = SENS_TYPE_ACCEL;
    mInfo.gain = GRAVITY_EARTH_1000;
    mInfo.sensitivity = mTask.sensors[ACC].sensitivity;
    mInfo.cvt = mTask.cvt;
    mInfo.getCalibration = accGetCalibration;
    mInfo.setCalibration = accSetCalibration;
    mInfo.getData = accGetData;
    mInfo.setDebugTrace = ICM4N607SetDebugTrace;
    mInfo.getSensorInfo = accGetSensorInfo;
    sensorCoreRegister(&mInfo);

    memset(&mInfo, 0x00, sizeof(struct sensorCoreInfo));
    mInfo.sensType = SENS_TYPE_GYRO;
    mInfo.gain = GYROSCOPE_INCREASE_NUM_AP;
    mInfo.sensitivity = mTask.sensors[GYR].sensitivity;
    mInfo.cvt = mTask.cvt;
    mInfo.getCalibration = gyroGetCalibration;
    mInfo.setCalibration = gyroSetCalibration;
    mInfo.getData = gyroGetData;
    mInfo.getSensorInfo = gyroGetSensorInfo;
    sensorCoreRegister(&mInfo);
}

static int icm4n607SensorRegistration(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                                      void *inBuf, uint8_t inSize, uint8_t elemInSize,
                                      void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    sensorCoreRegistration();   
    sensorFsmEnqueueFakeSpiEvt(spiCallBack, next_state, SUCCESS_EVT);
    return 0;

}

static struct sensorFsm ICM4N607Fsm[] = {
    /* INT handle */
    sensorFsmCmd(STATE_HW_INT_STATUS_CHECK, STATE_HW_INT_HANDLING, ICM4N607IntStatusCheck),
    sensorFsmCmd(STATE_HW_INT_HANDLING, STATE_HW_INT_HANDLING_DONE, ICM4N607IntHandling),
    /* sample */
    sensorFsmCmd(STATE_SAMPLE, STATE_FIFO, ICM4N607Sample),
    sensorFsmCmd(STATE_FIFO, STATE_CONVERT, ICM4N607ReadFifo),
    sensorFsmCmd(STATE_CONVERT, STATE_SAMPLE_DONE, ICM4N607Convert),

    sensorFsmCmd(STATE_ACC_ENABLE, STATE_ACC_ENABLE_DONE, ICM4N607AccEnable),
    sensorFsmCmd(STATE_ACC_DISABLE, STATE_ACC_DISABLE_DONE, ICM4N607AccDisable),
#if DEBUG_PHASE
    sensorFsmCmd(STATE_ACC_RATECHG,STATE_REGDUMP_READ , ICM4N607AccRate),
    sensorFsmCmd(STATE_REGDUMP_READ,STATE_REGDUMP_RESULT , ICM4N607regdump),
    sensorFsmCmd(STATE_REGDUMP_RESULT,STATE_ACC_RATECHG_DONE , ICM4N607regresult),  
#else
    sensorFsmCmd(STATE_ACC_RATECHG, STATE_ACC_RATECHG_DONE, ICM4N607AccRate),
#endif
    sensorFsmCmd(STATE_ACC_CALI, STATE_ACC_CALI_DONE, ICM4N607AccCali),
    sensorFsmCmd(STATE_ACC_CFG, STATE_ACC_CFG_DONE, ICM4N607AccCfgCali),

    sensorFsmCmd(STATE_GYRO_ENABLE, STATE_GYRO_ENABLE_DONE, ICM4N607GyroEnable),
    sensorFsmCmd(STATE_GYRO_DISABLE, STATE_GYRO_DISABLE_DONE, ICM4N607GyroDisable),
    
#if DEBUG_PHASE
    sensorFsmCmd(STATE_GYRO_RATECHG,STATE_REGDUMP_READ , ICM4N607GyroRate),
    sensorFsmCmd(STATE_REGDUMP_READ,STATE_REGDUMP_RESULT , ICM4N607regdump),
    sensorFsmCmd(STATE_REGDUMP_RESULT,STATE_GYRO_RATECHG_DONE , ICM4N607regresult),  
#else
    sensorFsmCmd(STATE_GYRO_RATECHG, STATE_GYRO_RATECHG_DONE, ICM4N607GyroRate),
#endif
    sensorFsmCmd(STATE_GYRO_CALI, STATE_GYRO_CALI_DONE, ICM4N607GyroCali),
    sensorFsmCmd(STATE_GYRO_CFG, STATE_GYRO_CFG_DONE, ICM4N607GyroCfgCali),

    /* init state */
    sensorFsmCmd(STATE_SW_RESET, STATE_SW_RESET_W, ICM4N607ResetRead_SpiStatus),
    sensorFsmCmd(STATE_SW_RESET_W, STATE_INT_STATUS, ICM4N607SoftReset),
    sensorFsmCmd(STATE_INT_STATUS, STATE_RESET_CHECK, ICM4N607IntStatus),
    sensorFsmCmd(STATE_RESET_CHECK, STATE_INIT_REG, ICM4N607ResetCheck),
    sensorFsmCmd(STATE_INIT_REG, STATE_SENSOR_REGISTRATION, ICM4N607InitConfig),
    sensorFsmCmd(STATE_SENSOR_REGISTRATION, STATE_EINT_REGISTRATION, icm4n607SensorRegistration),
    sensorFsmCmd(STATE_EINT_REGISTRATION, STATE_INIT_DONE, ICM4N607EintRegistration),

#if SUPPORT_SELFTEST

    /*self test FSM ACC*/
    sensorFsmCmd(STATE_ACC_SELF_TEST, STATE_SELFTEST_RESET_LOTP1, ICM4N607Selftest_Reset),
    sensorFsmCmd(STATE_SELFTEST_RESET_LOTP1, STATE_SELFTEST_LOAD_OTP2, ICM4N607Selftest_LoadOTP_1),
    sensorFsmCmd(STATE_SELFTEST_LOAD_OTP2, STATE_SELFTEST_CONFIG_PARAMETER, ICM4N607Selftest_LoadOTP_2),
    sensorFsmCmd(STATE_SELFTEST_CONFIG_PARAMETER, STATE_SELFTEST_CONTROL, ICM4N607Selftest_configure_parameters),
    sensorFsmCmd(STATE_SELFTEST_CONTROL, STATE_SELFTEST_SEND_RESULT, ICM4N607Control_selftest),
    sensorFsmCmd(STATE_SELFTEST_SEND_RESULT,STATE_SW_RESET , icm4n607Selftest_acc_result_end),
    sensorFsmCmd(STATE_SW_RESET, STATE_SW_RESET_W, ICM4N607ResetRead_SpiStatus),
    sensorFsmCmd(STATE_SW_RESET_W, STATE_INT_STATUS, ICM4N607SoftReset),
    sensorFsmCmd(STATE_INT_STATUS, STATE_RESET_CHECK, ICM4N607IntStatus),
    sensorFsmCmd(STATE_RESET_CHECK, STATE_INIT_REG, ICM4N607ResetCheck),
    sensorFsmCmd(STATE_INIT_REG, STATE_SELFTEST_RECOVER, ICM4N607InitConfig),
    sensorFsmCmd(STATE_SELFTEST_RECOVER, STATE_ACC_SELF_TEST_DONE, icm4n607_Selftest_Recover),

    /*self test FSM GYRO*/
    sensorFsmCmd(STATE_GYRO_SELF_TEST, STATE_SELFTEST_RESET_LOTP1, ICM4N607Selftest_Reset),
    sensorFsmCmd(STATE_SELFTEST_RESET_LOTP1, STATE_SELFTEST_LOAD_OTP2, ICM4N607Selftest_LoadOTP_1),
    sensorFsmCmd(STATE_SELFTEST_LOAD_OTP2, STATE_SELFTEST_CONFIG_PARAMETER, ICM4N607Selftest_LoadOTP_2),
    sensorFsmCmd(STATE_SELFTEST_CONFIG_PARAMETER, STATE_SELFTEST_CONTROL, ICM4N607Selftest_configure_parameters),
    sensorFsmCmd(STATE_SELFTEST_CONTROL, STATE_SELFTEST_SEND_RESULT, ICM4N607Control_selftest),
    sensorFsmCmd(STATE_SELFTEST_SEND_RESULT,STATE_SW_RESET , icm4n607Selftest_gyro_result_end),
    sensorFsmCmd(STATE_SW_RESET, STATE_SW_RESET_W, ICM4N607ResetRead_SpiStatus),
    sensorFsmCmd(STATE_SW_RESET_W, STATE_INT_STATUS, ICM4N607SoftReset),
    sensorFsmCmd(STATE_INT_STATUS, STATE_RESET_CHECK, ICM4N607IntStatus),
    sensorFsmCmd(STATE_RESET_CHECK, STATE_INIT_REG, ICM4N607ResetCheck),
    sensorFsmCmd(STATE_INIT_REG, STATE_SELFTEST_RECOVER, ICM4N607InitConfig),
    sensorFsmCmd(STATE_SELFTEST_RECOVER, STATE_GYRO_SELF_TEST_DONE, icm4n607_Selftest_Recover),


#endif

#ifdef SUPPORT_FSM_FREEFALL
    sensorFsmCmd(CHIP_FREE_FALL_ENABLE, STATE_FREEFALL_ENABLE_PH1, FreeFallPowerOn),
    sensorFsmCmd(STATE_FREEFALL_ENABLE_PH1, STATE_FREEFALL_ENABLE_PH2, freefall_enable_phase1),
    sensorFsmCmd(STATE_FREEFALL_ENABLE_PH2, CHIP_FREE_FALL_DONE, freefall_enable_phase2),
  
    sensorFsmCmd(CHIP_FREE_FALL_DISABLE, CHIP_FREE_FALL_DISABLE_DONE, FreeFallPowerOff),

#endif

#ifdef CFG_HW_STEP_COUNTER_SUPPORT
   /*
    sensorFsmCmd(CHIP_STEP_COUNT_ENABLE, STATE_FREEFALL_ENABLE_PH1, FreeFallPowerOn),
    sensorFsmCmd(STATE_FREEFALL_ENABLE_PH1, STATE_FREEFALL_ENABLE_PH2, freefall_enable_phase1),
    sensorFsmCmd(STATE_FREEFALL_ENABLE_PH2, CHIP_STEP_COUNT_DONE, freefall_enable_phase2),
   
    sensorFsmCmd(CHIP_STEP_COUNT_DISABLE, CHIP_STEP_COUNT_DISABLE_DONE, FreeFallPowerOff),
 
    sensorFsmCmd(CHIP_STEP_COUNT_SAMPLE, CHIP_STEP_COUNT_SEND, stepCntGetData),
    sensorFsmCmd(CHIP_STEP_COUNT_SEND, CHIP_STEP_COUNT_SAMPLE_DONE, StepCntSendData),
   */
   
    /* for stepCounter */
  
    sensorFsmCmd(CHIP_STEP_COUNT_ENABLE, STATE_STEPCOUNT_ENABLE_PH1, stepCountPowerOn),
    sensorFsmCmd(STATE_STEPCOUNT_ENABLE_PH1, STATE_STEPCOUNT_ENABLE_PH2, stepCount_enable_phase1),
    #if DEBUG_APEX_CONF
    sensorFsmCmd(STATE_STEPCOUNT_ENABLE_PH2,STATE_REGDUMP_READ , stepCount_enable_phase2),
    sensorFsmCmd(STATE_REGDUMP_READ,STATE_REGDUMP_RESULT , ICM4N607regdump),
    sensorFsmCmd(STATE_REGDUMP_RESULT,STATE_DUMP_BANK4_READ,ICM4N607regresult),
    sensorFsmCmd(STATE_DUMP_BANK4_READ,STATE_DUMP_BANK4_RESULT,ICM4N607regdump_bank4),
    sensorFsmCmd(STATE_DUMP_BANK4_RESULT,CHIP_STEP_COUNT_DONE,ICM4N607reg_bank4_result),
    #else
    sensorFsmCmd(STATE_STEPCOUNT_ENABLE_PH2, CHIP_STEP_COUNT_DONE, stepCount_enable_phase2),
    #endif 
    sensorFsmCmd(CHIP_STEP_COUNT_DISABLE, CHIP_STEP_COUNT_DISABLE_DONE, stepCountPowerOff),
 
    sensorFsmCmd(CHIP_STEP_COUNT_SAMPLE, CHIP_STEP_COUNT_SEND, stepCntGetData),
    sensorFsmCmd(CHIP_STEP_COUNT_SEND, CHIP_STEP_COUNT_SAMPLE_DONE, StepCntSendData),
  
#endif

    /* For Anymotion */
    sensorFsmCmd(STATE_ANYMO_ENABLE, STATE_ANYMO_ENABLE_DONE, anyMotionPowerOn),
    sensorFsmCmd(STATE_ANYMO_DISABLE, STATE_ANYMO_DISABLE_DONE, anyMotionPowerOff),
};

static void initSensorStruct(struct ICM4N607Sensor *sensor, enum SensorIndex idx)
{
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
    sensor->fifo_rate = 0;
    sensor->preRealRate = 0;
#if ICM4N607_MAX_ODR_800HZ
    sensor->samplediv = 1;
    sensor->sampledivcounter = 1;
#endif
}

static int ICM4N607Init(void)
{
    int ret = 0;
    enum SensorIndex i;
    uint8_t txData[2] = {0}, rxData[2] = {0};
    INVLOG(LOG_INFO, "icm4n607Init\n");
   // ICM4N607DebugPoint = &mTask;
    insertMagicNum(&mTask.accGyroPacket);
    mTask.hw = get_cust_accGyro("icm4n607");
	
	  INVLOG(LOG_INFO, "ICM4N607Init\n");
    if (NULL == mTask.hw) {
        INVLOG(LOG_ERROR, "get_cust_acc_hw fail\n");
        return 0;
    }
    INVLOG(LOG_INFO, "acc spi_num: %d,eint %d\n", mTask.hw->i2c_num, mTask.hw->eint_num);

    if (0 != (ret = sensorDriverGetConvert(mTask.hw->direction, &mTask.cvt))) {
        INVLOG(LOG_ERROR, "invalid direction: %d\n", mTask.hw->direction);
    }
    INVLOG(LOG_INFO, "acc map[0]:%d, map[1]:%d, map[2]:%d, sign[0]:%d, sign[1]:%d, sign[2]:%d\n\r",
          mTask.cvt.map[AXIS_X], mTask.cvt.map[AXIS_Y], mTask.cvt.map[AXIS_Z],
          mTask.cvt.sign[AXIS_X], mTask.cvt.sign[AXIS_Y], mTask.cvt.sign[AXIS_Z]);

    //Only applicable for factory calibration, refer to sensor spec
    mTask.sensors[ACC].sensitivity = (float)65536 / (8 * 2);//LSB/g sensitivity
    mTask.sensors[GYR].sensitivity = (float)65536 / (2000 * 2);//LSB/dps sensitivity

    mTask.latch_time_id = alloc_latch_time();
    enable_latch_time(mTask.latch_time_id, mTask.hw->eint_num);

    for (i = ACC; i < NUM_OF_SENSOR; i++) {
        initSensorStruct(&mTask.sensors[i], i);
    }

#if DEBUG_SENSOR_STABLE
    previous_acc.x = 0.0;
    previous_acc.y = 0.0;
    previous_acc.z = 0.0;
    previous_gyro.x = 0.0;
    previous_gyro.y = 0.0;
    previous_gyro.z = 0.0;
#endif

    mTask.int_src0 = 0;
    mTask.int_src1 = 0;
    mTask.wom_cfg = 0;
    mTask.pwr_sta = 0;
    mTask.acc_cfg0 = 0;
    mTask.gyro_cfg0 = 0;
    mTask.intf_cfg0 = 0;
    mTask.last_hwSampleTime = 0;
	mTask.lastSampleTime = 0;
	mTask.lastGyroDisabletime = 0;
    mTask.gyrodiscardStime = 0;

	mTask.hw_min_rate = SENSOR_HZ(12.5f);
 #if defined(CFG_HW_STEP_COUNTER_SUPPORT) || SUPPORT_HW_FREEFALL 
	mTask.DMP_first_config = true;
	mTask.DMP_reset_path_check = false;
	mTask.DMP_from_sram = false;
	mTask.total_step_cnt = 0;
	mTask.last_step_cnt = 0;
#endif    
    mTask.mode.speed = 5000000;
    mTask.mode.bitsPerWord = 8;
    mTask.mode.cpol = SPI_CPOL_IDLE_LO;
    mTask.mode.cpha = SPI_CPHA_LEADING_EDGE;
    mTask.mode.nssChange = true;
    mTask.mode.format = SPI_FORMAT_MSB_FIRST;
    mTask.mWbufCnt = 0;
    mTask.mRegCnt = 0;

    spiMasterRequest(mTask.hw->i2c_num, &mTask.spiDev);

    #if SPI_TRIM_3WIRE_DEFAULT
	/*change SPI 3 wire setting to 4 wire setting , and we only support SPI mode 0&3*/ 
	txData[0] = (uint8_t)(ICM4N607_CHIP_CONFIG_REG);
	txData[1] = 1<<ICM4N607_CHIP_CONFIG_REG_SPI_AP_4WIRES_POS;
    ret = spiMasterRxTxSync(mTask.spiDev, rxData, txData, 2);
    INVLOG(LOG_INFO, "icm4n607 spi config 4wire: %02x, ret: %d\n", rxData[1], ret);
    #endif

    #if SPI_SLEW_RATE_CHANGE
    txData[0] = (uint8_t)(ICM4N607_DRIVE_CONFIG_REG2);
	txData[1] = 0x0b;
    ret = spiMasterRxTxSync(mTask.spiDev, rxData, txData, 2);
    INVLOG(LOG_INFO, "icm4n607 spi config spi slew rate1: %02x, ret: %d\n", rxData[1], ret);

    txData[0] = (uint8_t)(ICM4N607_DRIVE_CONFIG_REG3);
	txData[1] = 0x03;
    ret = spiMasterRxTxSync(mTask.spiDev, rxData, txData, 2);
    INVLOG(LOG_INFO, "icm4n607 spi config spi slew rate2: %02x, ret: %d\n", rxData[1], ret);

    #endif
    
    txData[0] = (uint8_t)(ICM4N607_WHO_AM_I_REG) | 0x80;
    txData[1] = 0x00;
    
    int j =0;
    for(j =0; j<5;j++){
    ret = spiMasterRxTxSync(mTask.spiDev, rxData, txData, 2);
    INVLOG(LOG_ERROR, "ICM4N607_WHOAMI-1: %02x, ret: %d\n", rxData[1], ret);
	}
    switch(rxData[1]) {
 
    case T1000_WHO_AM_I:
        mTask.product = T1000;
        INVLOG(LOG_INFO, "t1000 detected\n");
        txData[0] = (uint8_t)(ICM4N607_PWR_MGMT_0) | 0x80;
        ret = spiMasterRxTxSync(mTask.spiDev, rxData, txData, 2);
        INVLOG(LOG_INFO, "t1000 PWR: %02x, ret: %d\n", rxData[1], ret);
        // record 1st default pwr value
        mTask.pwr_sta = rxData[1];
        break;
    case ICM42607_WHO_AM_I:
        mTask.product = ICM42607;
        INVLOG(LOG_INFO, "ICM42607 detected\n");
        txData[0] = (uint8_t)(ICM4N607_PWR_MGMT_0) | 0x80;
        ret = spiMasterRxTxSync(mTask.spiDev, rxData, txData, 2);
        INVLOG(LOG_INFO, "ICM42607 PWR: %02x, ret: %d\n", rxData[1], ret);
        // record 1st default pwr value
        mTask.pwr_sta = rxData[1];
        break;
   
    default:
        mTask.product = UNVALID_TYPE;
        INVLOG(LOG_INFO, "currently does not support this unknow chip %x\n",rxData[1]|0x0F);     
    }
    
    if (ret < 0 || mTask.product == UNVALID_TYPE){
        INVLOG(LOG_ERROR, "failed id match: %02x, ret: %d\n", rxData[1], ret);
        ret = -1;
		sendSensorErrToAp(ERR_SENSOR_ACC_GYR, ERR_CASE_ACC_GYR_INIT, ACC_NAME);
        sendSensorErrToAp(ERR_SENSOR_ACC_GYR, ERR_CASE_ACC_GYR_INIT, GYRO_NAME);
        spiMasterRelease(mTask.spiDev);
        disable_latch_time(mTask.latch_time_id);
        free_latch_time(mTask.latch_time_id);
        goto err_out;
    }
   
    accSensorRegister();
    gyroSensorRegister();
    anyMotionSensorRegister();
#ifdef CFG_HW_STEP_COUNTER_SUPPORT
    stepCntSensorRegister();
#endif
#ifdef SUPPORT_FSM_FREEFALL
    freeFallSensorRegister();
#endif
    registerAccGyroInterruptMode(ACC_GYRO_FIFO_INTERRUPTIBLE);
    registerAccGyroDriverFsm(ICM4N607Fsm, ARRAY_SIZE(ICM4N607Fsm));
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
MODULE_DECLARE(icm4n607, SENS_TYPE_ACCEL, ICM4N607Init);
#else
#include "mtk_overlay_init.h"
OVERLAY_DECLARE(icm4n607, OVERLAY_ID_ACCGYRO, ICM4N607Init);
#endif

