###################################################################
# override the default HEAP size value (configTOTAL_HEAP_SIZE) in FreeRTOSConfig.h
###################################################################
# CFLAGS += -DconfigTOTAL_HEAP_SIZE='( ( size_t ) ( 200 * 1024 ) )'
# CFLAGS += -DconfigLOWPOWER_DISABLE=1

#CFLAGS += -DconfigUSE_TICKLESS_IDLE=1 TS suggest to only config this in platform.mk
PRIVATE_SENDRV_DIR  := $(SOURCE_DIR)/../secure/middleware/sensor

###################################################################
# Setting Feature Compiler Option
###################################################################
SENDRV_DIR        := $(SOURCE_DIR)/middleware/contexthub/MEMS_Driver
VIRTDRV_DIR       := $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver
SENLIB_DIR        := $(SOURCE_DIR)/middleware/contexthub/MEMS_Driver/magnetometer/lib
SENCUST_DIR       := $(SOURCE_DIR)/project/CM4_A/$(PLATFORM)/$(PROJECT)/cust

###################################################################
# Core chre resources
###################################################################
INCLUDES += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/external/freebsd/inc
INCLUDES += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/inc
INCLUDES += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/links
INCLUDES += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/

INCLUDES += $(SOURCE_DIR)/../../../hardware/contexthub/lib/include
INCLUDES += $(SOURCE_DIR)/middleware/contexthub/

C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/links/plat/src/mtk_start.c
C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/links/plat/src/hostIntfIPI.c
C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/links/plat/src/platform.c
C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/links/plat/src/rtc.c
C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/links/plat/src/bl.c
C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/links/plat/src/pwr.c
C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/links/plat/src/apInt.c
C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/links/plat/src/hostIntf.c
C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/links/plat/src/mtkQ.c
ifeq ($(CFG_I2C_SUPPORT),yes)
INCLUDES += $(DRIVERS_PLATFORM_DIR)/i2c/inc
C_FILES  += $(DRIVERS_PLATFORM_DIR)/i2c/src/i2cchre-plat.c
endif
ifeq ($(CFG_SPI_SUPPORT),yes)
INCLUDES += $(DRIVERS_PLATFORM_DIR)/spi/inc
C_FILES  += $(DRIVERS_PLATFORM_DIR)/spi/src/spichre-plat.c
C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/links/plat/src/spichre.c
endif

C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/src/syscall.c
C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/src/osApi.c
C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/src/seos.c
C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/src/slab.c
C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/src/eventQ.c
C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/src/hostIntf.c
#C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/src/simpleQ.c
C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/src/sensors.c
C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/src/nanohubCommand.c
C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/src/timer.c
#C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/src/printf.c
ifeq ($(CFG_SPI_SUPPORT),yes)
C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/src/spi.c
endif

#C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/lib/nanohub/aes.c
#C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/lib/nanohub/nanoapp.c
#C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/lib/nanohub/rsa.c
#C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/lib/nanohub/sha2.c
C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/lib/nanohub/softcrc.c

C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/links/cpu/src/appSupport.c
C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/links/cpu/src/atomicBitset.c
C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/links/cpu/src/atomic.c
C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/links/cpu/src/cpu.c
ifneq ($(CFG_CONTEXTHUB_FW_SUPPORT),yes)
C_FILES  += $(SOURCE_DIR)/../../../hardware/contexthub/firmware/src/drivers/sample_sensor/sample_sensor.c
endif
###################################################################
# Required resources while CFG_SENSOR_SUPPORT is enabled
###################################################################
ifeq ($(CFG_CHRE_SUPPORT),yes)
LIBFLAGS += -lm
######## contexthub firmware support ########
ifeq ($(CFG_CONTEXTHUB_FW_SUPPORT),yes)
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/contexthub_fw.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/contexthub_fw.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/contexthub_core.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/virtual_core.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/hwsen.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/sensorFsm.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/performance.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/sensorCust.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/sensorErrInfo.c
endif

######## overlay cust ########
ifeq ($(CFG_OVERLAY_INIT_SUPPORT),yes)
C_FILES  += $(SENCUST_DIR)/overlay/overlay.c
endif

######## alsps support ########
ifeq ($(CFG_ALSPS_SUPPORT),yes)
INCLUDES += -I$(SOURCE_DIR)/drivers/common/cache/v01/inc
INCLUDES += -I$(SENDRV_DIR)/alsps
ifeq ($(CFG_STK3321_NEW_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/alsps/alsps_new.c
else 
C_FILES  += $(SENDRV_DIR)/alsps/alsps.c
endif
C_FILES  += $(SENDRV_DIR)/alsps/alsps_separate.c
C_FILES  += $(SENDRV_DIR)/alsps/fakeAls.c
C_FILES  += $(SENDRV_DIR)/alsps/fakePs.c
C_FILES  += $(SENCUST_DIR)/alsps/cust_alsps.c
ifeq ($(CFG_CM36558_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/alsps/cm36558.c
endif
ifeq ($(CFG_STK3A5X_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/alsps/stk3a5x.c
endif
ifeq ($(CFG_CM36652_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/alsps/cm36652.c
endif
ifeq ($(CFG_APDS993X_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/alsps/apds993x.c
endif
ifeq ($(CFG_APDS9922_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/alsps/apds9922.c
endif
ifeq ($(CFG_AP3426_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/alsps/ap3426.c
endif
ifeq ($(CFG_STK3X3X_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/alsps/stk3x3x.c
endif
ifeq ($(CFG_STK3X2X_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/alsps/stk3x2x.c
endif
ifeq ($(CFG_BH1726_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/alsps/bh1726.c
endif
ifeq ($(CFG_TSL2591_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/alsps/tsl2591.c
endif
ifeq ($(CFG_LTR578_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/alsps/ltr578.c
endif
ifeq ($(CFG_STK3321_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/alsps/stk3321.c
endif
ifeq ($(CFG_STK3321_NEW_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/alsps/stk3321_new.c
endif
ifeq ($(CFG_TMD2725_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/alsps/tmd2725.c
endif
ifeq ($(CFG_TMD2702_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/alsps/tmd2702.c
endif
ifeq ($(CFG_VCNL36832_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/alsps/vcnl36832.c
endif
ifeq ($(CFG_MN66233_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/alsps/epl_sensor.c
endif
ifeq ($(CFG_EM30918_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/alsps/em30918.c
endif
endif

######## barometer support ########
ifeq ($(CFG_BAROMETER_SUPPORT),yes)
INCLUDES += -I$(SOURCE_DIR)/drivers/common/cache/v01/inc
INCLUDES += -I$(SENDRV_DIR)/barometer
C_FILES  += $(SENDRV_DIR)/barometer/barometer.c
C_FILES  += $(SENCUST_DIR)/barometer/cust_baro.c
ifeq ($(CFG_BMP280_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/barometer/bosch_bmp280.c
endif
endif

######## sar support ########
ifeq ($(CFG_SAR_SUPPORT),yes)
INCLUDES += -I$(SOURCE_DIR)/drivers/common/cache/v01/inc
INCLUDES += -I$(SENDRV_DIR)/sar
C_FILES  += $(SENDRV_DIR)/sar/sar.c
C_FILES  += $(SENCUST_DIR)/sar/cust_sar.c
ifeq ($(CFG_SX9310_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/sar/sx9310.c
endif
ifeq ($(CFG_SAR_MODEM_SUPPORT),yes)
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/sar_modem.c
endif
endif

######## magnetometer support ########
ifeq ($(CFG_MAGNETOMETER_SUPPORT),yes)
INCLUDES += -I$(SOURCE_DIR)/drivers/common/cache/v01/inc
INCLUDES += -I$(SENDRV_DIR)/magnetometer
C_FILES  += $(SENDRV_DIR)/magnetometer/magnetometer.c
C_FILES  += $(SENCUST_DIR)/magnetometer/cust_mag.c

ifeq ($(CFG_AKM09918_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/magnetometer/akm09918.c
endif

ifeq ($(CFG_MMC3630_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/magnetometer/mmc3630.c
endif
ifeq ($(CFG_AKM09915_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/magnetometer/akm09915.c
endif
endif

######## accgyro support ########
ifeq ($(CFG_MXC4005X_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/accGyro/mxc4005x.c
LDFLAGS += $(SENDRV_DIR)/accGyro/AccCali.o
endif
ifeq ($(CFG_ACCGYRO_SUPPORT),yes)
INCLUDES += -I$(SENDRV_DIR)/accGyro/
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/auto_cali
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/timestamp_cali
C_FILES  += $(SENDRV_DIR)/accGyro/accGyro.c
C_FILES  += $(SENCUST_DIR)/accGyro/cust_accGyro.c

ifeq ($(CFG_HWGYRO_SUPPORT),yes)
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/auto_cali -lksensor
else
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/auto_cali -lksensor_nogyro
endif

LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/timestamp_cali -lktimestamp
ifeq ($(CFG_BMI160_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/accGyro/bmi160.c
endif
ifeq ($(CFG_LIS2HH12_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/accGyro/lis2hh12.c
endif
ifeq ($(CFG_SC7A20_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/accGyro/sc7a20.c
endif
ifeq ($(CFG_ICM20600_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/accGyro/icm20600.c
endif
ifeq ($(CFG_LIS3DH_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/accGyro/lis3dh.c
endif
ifeq ($(CFG_ICM40605_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/accGyro/icm40605.c
endif
ifeq ($(CFG_STK832X_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/accGyro/stk832x.c
C_FILES  += $(SENDRV_DIR)/accGyro/stk_hand_detect.c
endif
ifeq ($(CFG_MC3416_SUPPORT),yes)
C_FILES  += $(SENDRV_DIR)/accGyro/mc3416.c
endif
endif

######## fusion sensor support ########
ifeq ($(CFG_FUSION_SUPPORT),yes)
C_FILES  += $(VIRTDRV_DIR)/sensorFusion.c
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/fusion -lmpe
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/fusion/inc
endif

######## vendor fusion sensor support ########
ifeq ($(CFG_VENDOR_FUSION_SUPPORT),yes)
ifeq ($(CFG_VIRTUAL_GYRO_SUPPORT),yes)
CFLAGS += -DCFG_AKM_PG_SUPPORT
endif
CFLAGS += -DCFG_AKM_FUSION_SUPPORT
C_FILES  += $(VIRTDRV_DIR)/vendor_fusion.c
INCLUDES += -I$(VIRTDRV_DIR)/
endif

######## FLP support ########
ifeq ($(CFG_FLP_SUPPORT),yes)
INCLUDES += -I$(DRIVERS_PLATFORM_DIR)/ccci
INCLUDES += -I$(SOURCE_DIR)/drivers/common/ccci
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/service
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/service/contexthub_flp.c
CFG_CNN_TO_SCP_BUF_SIZE = 0x8c
CFG_SCP_TO_CNN_BUF_SIZE = 0x8c
endif

######## geofence sensor support ########
ifeq ($(CFG_GEOFENCE_SUPPORT),yes)
INCLUDES += -I$(DRIVERS_PLATFORM_DIR)/ccci
INCLUDES += -I$(SOURCE_DIR)/drivers/common/ccci
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/geofence_adaptor.c
endif

######## stepRecognition--STEP_COUNTER ########
ifeq ($(CFG_STEP_COUNTER_SUPPORT),yes)
CFLAGS += -D_STEP_COUNTER_
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/pedometer/v2
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/VIRT_Driver
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/stepRecognition.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/algoDataResample.c
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/pedometer/v2 -lpedometer
endif

######## stepRecognition--STEP_DETECTOR ########
ifeq ($(CFG_STEP_DETECTOR_SUPPORT),yes)
CFLAGS += -D_STEP_DETECTOR_
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/pedometer/v2
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/VIRT_Driver
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/stepRecognition.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/algoDataResample.c
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/pedometer/v2 -lpedometer
endif

######## stepRecognition--SIGNIFICANT_MOTION ########
ifeq ($(CFG_SIGNIFICANT_MOTION_SUPPORT),yes)
CFLAGS += -D_SMD_ENABLE_
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/pedometer/v2
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/VIRT_Driver
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/stepRecognition.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/algoDataResample.c
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/pedometer/v2 -lpedometer
endif

######## gestureRecognition--SHAKE ########
ifeq ($(CFG_SHAKE_SUPPORT),yes)
CFLAGS += -D_SHAKE_ENABLE_
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/common
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/gesture
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/VIRT_Driver
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/gestureRecognition.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/algoDataResample.c
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/common -llecommon
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/gesture -llegesture
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/gesture -llegestureshake
endif

######## gestureRecognition--SNAPSHOT ########
ifeq ($(CFG_SNAPSHOT_SUPPORT),yes)
CFLAGS += -D_SNAPSHOT_ENABLE_
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/common
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/gesture
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/activity
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/VIRT_Driver
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/gestureRecognition.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/algoDataResample.c
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/common -lmath
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/common -llecommon
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/gesture -llegesture
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/gesture -llegesturesnapshot
endif

######## gestureRecognition--ANSWERCALL ########
ifeq ($(CFG_ANSWERCALL_SUPPORT),yes)
CFLAGS += -D_ANSWERCALL_ENABLE_
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/answercall
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/tilt
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/VIRT_Driver
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/answercall_detector.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/algoDataResample.c
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/answercall -lanswercall
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/tilt -ldircommon
endif

######## gestureRecognition--TILT ########
ifeq ($(CFG_TILT_SUPPORT),yes)
CFLAGS += -D_TILT_ENABLE_
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/tilt
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/VIRT_Driver
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/tilt_detector.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/algoDataResample.c
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/tilt -ltilt
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/tilt -ldircommon
endif

######### activity_baro ############
ifeq ($(CFG_ACTIVITY_BARO_SUPPORT), yes)
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/activity/
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/tilt
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/VIRT_Driver
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/common
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/activity.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/algoDataResample.c
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/common -lmath
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/common -llecommon
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/tilt -ltilt
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/tilt -ldircommon
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/activity -llecontext
endif

######### activity_no_baro ############
ifeq ($(CFG_ACTIVITY_NO_BARO_SUPPORT), yes)
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/activity_no_baro -llecontext_no_baro
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/activity_no_baro
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/activity_no_baro.c
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/VIRT_Driver
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/common
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/algoDataResample.c
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/common -llecommon
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/common -lmath
endif

######## stationary ########
ifeq ($(CFG_STATIONARY_SUPPORT),yes)
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/common
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/tilt
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/situation
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/VIRT_Driver
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/stationary_adaptor.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/algoDataResample.c
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/common -lmath
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/tilt -ldircommon
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/situation -lstationary
endif

######## winOrientation ########
ifeq ($(CFG_WIN_ORIENTATION_SUPPORT),yes)
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/VIRT_Driver
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/win_orientation.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/algoDataResample.c
endif

######## freefallRecognition ########
ifeq ($(CFG_FREEFALL_SUPPORT),yes)
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/common
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/gesture
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/VIRT_Driver
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/freefallRecognition.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/algoDataResample.c
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/common -llecommon
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/gesture -llegesture
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/freefall -llegesturefreefall
endif

######## inPocket ########
ifeq ($(CFG_INPOCKET_SUPPORT),yes)
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/common
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/in_pocket.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/algoDataResample.c
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/common -lmath
endif

######## flat ########
ifeq ($(CFG_FLAT_SUPPORT),yes)
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/common
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/flat.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/algoDataResample.c
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/common -lmath
endif

######## anyMotion/noMotion ########
ifeq ($(CFG_MOTION_SUPPORT),yes)
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/common
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/motion
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/motion_adaptor.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/algoDataResample.c
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/common -lmath
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/motion -lmotion
endif

######## wakeup ########
ifeq ($(CFG_WAKEUP_SUPPORT),yes)
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/wakeup.c
endif

######## glance ########
ifeq ($(CFG_GLANCE_SUPPORT),yes)
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/wakeup_gesture
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/VIRT_Driver
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/glanceDetect.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/algoDataResample.c
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/wakeup_gesture -lwakeupgesture
endif

######## e_glance ########
ifeq ($(CFG_E_GLANCE_SUPPORT),yes)
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/wakeup_gesture
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/VIRT_Driver
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/eGlanceDetect.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/algoDataResample.c
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/wakeup_gesture -lwakeupgesture
endif

######## pickup ########
ifeq ($(CFG_PICKUP_SUPPORT),yes)
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/pickup.c
endif

######## Lift and putdown ########
ifeq ($(CFG_LIFT_PUTDOWN_SUPPORT),yes)
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/wakeup_gesture
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/VIRT_Driver
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/liftpdDetect.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/algoDataResample.c
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/wakeup_gesture -lwakeupgesture
endif

######## floorCount ########
ifeq ($(CFG_FLOOR_COUNT_SUPPORT),yes)
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/pedometer
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/VIRT_Driver
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/floor_count
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/floor_count.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/algoDataResample.c
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/pedometer -lpedometer
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/floor_count -lfloorcount
endif

######## gestureRecognition--LIFT ########
ifeq ($(CFG_LIFT_SUPPORT),yes)
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/wakeup_gesture
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/VIRT_Driver
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/liftDetect.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/algoDataResample.c
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/wakeup_gesture -lwakeupgesture
endif

######## gestureRecognition--FLIP ########
ifeq ($(CFG_FLIP_SUPPORT),yes)
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/flip
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/algo/tilt
INCLUDES += -I$(SOURCE_DIR)/middleware/contexthub/VIRT_Driver
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/flipDetect.c
C_FILES  += $(SOURCE_DIR)/middleware/contexthub/VIRT_Driver/algoDataResample.c
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/flip -lflip
LIBFLAGS += -L$(SOURCE_DIR)/middleware/contexthub/algo/tilt -ldircommon
endif

endif
