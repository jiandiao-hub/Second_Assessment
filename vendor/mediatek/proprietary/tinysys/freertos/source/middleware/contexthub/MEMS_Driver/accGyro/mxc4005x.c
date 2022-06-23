/*md
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
#include "mxc4005x.h"
#include "AccCali.h"

#define DRIVER_VERSION				"1.1.20325"

static  struct MXC4005XTask mTask;

static void accGetCalibration(int32_t *cali, int32_t size) {
    cali[AXIS_X] = mTask.accSwCali[AXIS_X];
    cali[AXIS_Y] = mTask.accSwCali[AXIS_Y];
    cali[AXIS_Z] = mTask.accSwCali[AXIS_Z];
    //osLog(LOG_INFO, "accGetCalibration cali x:%d, y:%d, z:%d\n", cali[AXIS_X], cali[AXIS_Y], cali[AXIS_Z]); 
}

static void accSetCalibration(int32_t *cali, int32_t size) {
    mTask.accSwCali[AXIS_X] = cali[AXIS_X];
    mTask.accSwCali[AXIS_Y] = cali[AXIS_Y];
    mTask.accSwCali[AXIS_Z] = 0;//cali[AXIS_Z];
    //osLog(LOG_INFO, "accSetCalibration cali x:%d, y:%d, z:%d\n", mTask.accSwCali[AXIS_X],
//        mTask.accSwCali[AXIS_Y], mTask.accSwCali[AXIS_Z]); 
}

static void accGetData(void *sample) {
    struct TripleAxisDataPoint *tripleSample = (struct TripleAxisDataPoint *)sample;
    tripleSample->ix = mTask.accFactoryData.ix;
    tripleSample->iy = mTask.accFactoryData.iy;
    tripleSample->iz = mTask.accFactoryData.iz;
    osLog(LOG_INFO, "accGetData x:%d, y:%d, z:%d\n", tripleSample->ix,
      tripleSample->iy, tripleSample->iz); 
}


static void mxc4005xSetDebugTrace(int32_t trace) {
    mTask.debug_trace = trace;
    //osLog(LOG_ERROR, "%s ==> trace:%d\n", __func__, mTask.debug_trace);
}

static void mxc4005xGetSensorInfo(struct sensorInfo_t *data)
{
    strncpy(data->name, "mxc4005x", sizeof(data->name));
}

static void sensorCoreRegistration(void) {
    struct sensorCoreInfo mInfo;
    osLog(LOG_INFO, "mxc4005xRegisterCore\n");
    memset(&mInfo, 0x00, sizeof(struct sensorCoreInfo));
    /* Register sensor Core */
    mInfo.sensType = SENS_TYPE_ACCEL;
    mInfo.gain = GRAVITY_EARTH_1000;
    mInfo.sensitivity = mTask.sensors[ACC].sensitivity;
    mInfo.cvt = mTask.cvt;
    mInfo.getCalibration = accGetCalibration;
    mInfo.setCalibration = accSetCalibration;
    mInfo.getData = accGetData;
    mInfo.setDebugTrace = mxc4005xSetDebugTrace;
    mInfo.getSensorInfo = mxc4005xGetSensorInfo;
    sensorCoreRegister(&mInfo);
}

static int mxc4005xSensorRegistration(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    sensorCoreRegistration();

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    //osLog(LOG_ERROR, "%s ==> \n", __func__);
    return 0;
}


static int mxc4005xAccPowerOn(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
	int ret = 0;
    if(!mTask.sensors[ACC].powered) 
    {
        CleanBuffer();     
        mTask.startTime = rtcGetTime();
        mTask.sensors[ACC].powered = true;
        mTask.txBuf[0] = MXC400X_REG_CTRL;
        mTask.txBuf[1] = MXC400X_AWAKE;
    
        ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2, NULL, NULL);
   } 
   osLog(LOG_INFO, "mxc4005xAccPowerOn any :%d, acc:%d\n",mTask.sensors[ANYMO].powered, mTask.sensors[ACC].powered);    
   sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);

        
   return ret;
}

static int mxc4005xAccPowerOff(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
	int ret = 0;
 	
    mTask.sensors[ACC].configed = false;
    if (mTask.sensors[ACC].powered) 
	{
         // //slepp mode
        mTask.sensors[ACC].powered = false;
     	mTask.txBuf[0] = MXC400X_REG_CTRL;
    	mTask.txBuf[1] = MXC400X_SLEEP;
    	ret = i2cMasterTx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 2, NULL, NULL);
	}
	
    osLog(LOG_INFO, "mxc4005xAccPowerOff any :%d, acc:%d\n",mTask.sensors[ANYMO].powered, mTask.sensors[ACC].powered);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
	return ret;
}


static void caliRawData(struct accGyroData *data,int32_t *raw_data)
{
  	int32_t swCali[AXES_NUM] = {0};
  	int32_t caliResult[AXES_NUM] = {0};
  	float temp_data[AXES_NUM] = {0};
        int32_t remap_data[AXES_NUM];

    accGetCalibration(swCali,0);
    //osLog(LOG_ERROR, "ACCGetCalibration cali x:%d, y:%d, z:%d\n", swCali[AXIS_X], swCali[AXIS_Y], swCali[AXIS_Z]); 
    raw_data[0] = raw_data[0] + swCali[0];
    raw_data[1] = raw_data[1] + swCali[1];
    raw_data[2] = raw_data[2];
	remap_data[mTask.cvt.map[AXIS_X]] = mTask.cvt.sign[AXIS_X] * raw_data[AXIS_X];
	remap_data[mTask.cvt.map[AXIS_Y]] = mTask.cvt.sign[AXIS_Y] * raw_data[AXIS_Y];
	remap_data[mTask.cvt.map[AXIS_Z]] = mTask.cvt.sign[AXIS_Z] * raw_data[AXIS_Z];
    
        temp_data[AXIS_X] = (float)remap_data[AXIS_X] * GRAVITY_EARTH_SCALAR / 256.0;
        temp_data[AXIS_Y] = (float)remap_data[AXIS_Y] * GRAVITY_EARTH_SCALAR / 256.0;
        temp_data[AXIS_Z] = (float)remap_data[AXIS_Z] * GRAVITY_EARTH_SCALAR / 256.0;
        //osLog(LOG_ERROR, "ACC startCali %d\n", mTask.sensors[ACC].startCali);
    //if (UNLIKELY(mTask.sensors[ACC].startCali))
    if (mTask.sensors[ACC].startCali == true)
    {
        int32_t delta_time = 0;
        float calibrated_data_output[AXES_NUM] = {0};
        int accuracy = 0;
        int16_t status =
            Acc_run_factory_calibration_timeout(delta_time, temp_data,
                                                calibrated_data_output, &accuracy, rtcGetTime());
        if (0 != status)
        {
            mTask.sensors[ACC].startCali = false;

            if (0 < status)
            {
                osLog(LOG_INFO, "ACC cali detect shake \n");
                caliResult[AXIS_X] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_X] * 1000);
                caliResult[AXIS_Y] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_Y] * 1000);
                caliResult[AXIS_Z] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_Z] * 1000);
                accGyroSendCalibrationResult(SENS_TYPE_ACCEL,
                                          (int32_t *)&caliResult[0], (uint8_t)status);
            }
            else
            {
                osLog(LOG_ERROR, "ACC cali time out \n");
            }
        }
        else if (3 == accuracy)
        {
            mTask.sensors[ACC].startCali = false;
            mTask.sensors[ACC].staticCali[AXIS_X] = calibrated_data_output[AXIS_X] - temp_data[AXIS_X];
            mTask.sensors[ACC].staticCali[AXIS_Y] = calibrated_data_output[AXIS_Y] - temp_data[AXIS_Y];
            mTask.sensors[ACC].staticCali[AXIS_Z] = calibrated_data_output[AXIS_Z] - temp_data[AXIS_Z];
            caliResult[AXIS_X] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_X] * 1000);
            caliResult[AXIS_Y] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_Y] * 1000);
            caliResult[AXIS_Z] = (int32_t)(mTask.sensors[ACC].staticCali[AXIS_Z] * 1000);
            accGyroSendCalibrationResult(SENS_TYPE_ACCEL,
                                      (int32_t *)&caliResult[0], (uint8_t)status);
            osLog(LOG_ERROR, "ACC cali done, caliResult:%d %d %d, offset:%f %f %f \n",
                  caliResult[AXIS_X], caliResult[AXIS_Y], caliResult[AXIS_Z],
                  (double)mTask.sensors[ACC].staticCali[AXIS_X],
                  (double)mTask.sensors[ACC].staticCali[AXIS_Y],
                  (double)mTask.sensors[ACC].staticCali[AXIS_Z]);
        }
    }
    

    data[0].x = temp_data[AXIS_X] + mTask.sensors[ACC].staticCali[AXIS_X];
    data[0].y = temp_data[AXIS_Y] + mTask.sensors[ACC].staticCali[AXIS_Y];
    //data[0].z = temp_data[AXIS_Z] + mTask.sensors[ACC].staticCali[AXIS_Z];
    //osLog(LOG_ERROR, "ACC: staticcali x:%f, y:%f, z:%f \n",(double)mTask.sensors[ACC].staticCali[AXIS_X],(double)mTask.sensors[ACC].staticCali[AXIS_Y],(double)mTask.sensors[ACC].staticCali[AXIS_Z]);
    data->sensType = SENS_TYPE_ACCEL;
        mTask.accFactoryData.ix = (int32_t)(data[0].x * ACCELEROMETER_INCREASE_NUM_AP);
        mTask.accFactoryData.iy = (int32_t)(data[0].y * ACCELEROMETER_INCREASE_NUM_AP);
        mTask.accFactoryData.iz = (int32_t)(data[0].z * ACCELEROMETER_INCREASE_NUM_AP);

    //osLog(LOG_ERROR, "%s ACC: cali data_x:%f, calidata_y:%f, calidata_z:%f \n",
     //                      __func__, (double)data[0].x, (double)data[0].y, (double)data[0].z);
}

static int mxc4005xSample(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) 
{
   int ret = 0;
    ret = rxTransferDataInfo(&mTask.dataInfo, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);
    if (ret < 0) {
       osLog(LOG_ERROR, "mxc4005xSample, rx inSize and elemSize error\n");
       sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
       return ret;
   }
   mTask.swSampleTime = rtcGetTime();//mayun
    mTask.txBuf[0] = MXC400X_REG_X;
   ret = i2cMasterTxRx(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1, mTask.rxBuf,7,i2cCallBack,next_state);
    if (ret < 0) {
       osLog(LOG_ERROR, "mxc4005xSample, tx inSize and elemSize error\n");
       sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
       return ret;
	}		   
    return ret;
}


static int mxc4005xConvert(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) 
{
   struct accGyroData *data = mTask.accGyroPacket.outBuf;
    int32_t idata[AXES_NUM];
    int32_t remap_data[AXES_NUM];
    uint64_t timestamp = 0;
	float fAccRawData[3] = {0.0};
	float cal_data[3] = {0.0};
	static float last_data = {0.0};
  static uint64_t last_timestamp = 0;

    uint8_t accEventSize = 0;
    int ret = -1;

    data[0].sensType = SENS_TYPE_ACCEL;
#if 0
    osLog(LOG_ERROR, "%s mxc4005x raw reg data: \n"
                  "[0]:%d, [1]:%d\n"
                  "[2]:%d, [3]:%d\n"
                  "[4]:%d, [5]:%d\n",
          __func__, mTask.rxBuf[0], mTask.rxBuf[1],mTask.rxBuf[2],mTask.rxBuf[3],mTask.rxBuf[4],mTask.rxBuf[5]);
#endif


	idata[AXIS_X] = (s16)((s16)mTask.rxBuf[0] << 8 | (s16)mTask.rxBuf[1]) >> 4;
	idata[AXIS_Y] = (s16)((s16)mTask.rxBuf[2] << 8 | (s16)mTask.rxBuf[3]) >> 4;
	idata[AXIS_Z] = (s16)((s16)mTask.rxBuf[4] << 8 | (s16)mTask.rxBuf[5]) >> 4;


	remap_data[mTask.cvt.map[AXIS_X]] = mTask.cvt.sign[AXIS_X] * idata[AXIS_X];
	remap_data[mTask.cvt.map[AXIS_Y]] = mTask.cvt.sign[AXIS_Y] * idata[AXIS_Y];
	remap_data[mTask.cvt.map[AXIS_Z]] = mTask.cvt.sign[AXIS_Z] * idata[AXIS_Z];

    data[0].x = remap_data[AXIS_X]*9.807f/256.0f;
    data[0].y = remap_data[AXIS_Y]*9.807f/256.0f;
    data[0].z = remap_data[AXIS_Z]*9.807f/256.0f;
 	
    timestamp = mTask.swSampleTime;
    //osLog(LOG_ERROR, "%s tsnow: %lld  tstart: %lld\n",__func__,timestamp, mTask.startTime);
    if(timestamp - mTask.startTime < 300000000)
    { 
      data[0].x = 0.0;
      data[0].y = 0.0;
      data[0].z = 0.0;
      accEventSize = 0;
    }else {
        
        
	 osLog(LOG_ERROR, "%s raw data:%f %f %f  ts : %lld\n",__func__,(double)data[0].x,(double)data[0].y,(double)data[0].z,mTask.swSampleTime);
        
	caliRawData(data,idata);
	fAccRawData[0]=data[0].x/9.807f;
	fAccRawData[1]=data[0].y/9.807f;
    fAccRawData[2]=data[0].z/9.807f;

    if (timestamp - last_timestamp > 15000000)   //15ms
    {
			DynamicCali(fAccRawData,cal_data);
			last_timestamp = timestamp;
			last_data = cal_data[2];
    }
    else
    {
			cal_data[2] = last_data;
    }

	data[0].z = cal_data[2]*9.807f;//modified by nan deliver original z
               
        mTask.accFactoryData.ix = (int32_t)(data[0].x * ACCELEROMETER_INCREASE_NUM_AP);
        mTask.accFactoryData.iy = (int32_t)(data[0].y * ACCELEROMETER_INCREASE_NUM_AP);
        mTask.accFactoryData.iz = (int32_t)(data[0].z * ACCELEROMETER_INCREASE_NUM_AP);

        //osLog(LOG_ERROR, "cal_data[2] = %f, cal data_z=%f fTemperature %f\n",(double)cal_data[2],(double) data[0].z,(double)fTemperature);
        osLog(LOG_ERROR, "convert data x = %f, y = %f z =  %f\n",(double)data[0].x,(double)data[0].y,(double)data[0].z);
     	accEventSize = 1;
        
	}
	if(mTask.sensors[ACC].startCali)
    {
       accEventSize = 0;
    }
    
   
    txTransferDataInfo(&mTask.dataInfo, accEventSize,0, timestamp, data,0);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return ret;
}


static int mxc4005xAccCali(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
    float bias[AXES_NUM] = {0};
    mTask.sensors[ACC].startCali = true;

    osLog(LOG_INFO, "mxc4005xAccCali\n");
    Acc_init_calibration(bias);

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int mxc4005xAccCfgCali(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize)
{
   int ret = 0;
   struct accGyroCaliCfgPacket caliCfgPacket ;
   osLog(LOG_INFO, "mxc4005xAccCfgCali\n");


    ret = rxCaliCfgInfo(&caliCfgPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);
    if (ret < 0) {
       sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, ERROR_EVT);
       osLog(LOG_ERROR, "mxc4005xAccCfgCali, rx inSize and elemSize error\n");
       return -1;
    }
  
    osLog(LOG_INFO, "%s, cfgData[0]:%d, cfgData[1]:%d, cfgData[2]:%d \n",__FUNCTION__,
          caliCfgPacket.caliCfgData[0], caliCfgPacket.caliCfgData[1], caliCfgPacket.caliCfgData[2]);

    mTask.sensors[ACC].staticCali[0] = (float)caliCfgPacket.caliCfgData[0] / 1000;
    mTask.sensors[ACC].staticCali[1] = (float)caliCfgPacket.caliCfgData[1] / 1000;
    mTask.sensors[ACC].staticCali[2] = (float)caliCfgPacket.caliCfgData[2] / 1000;

    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return ret;
}

static int mxc4005xAccRate(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                         void *inBuf, uint8_t inSize, uint8_t elemInSize,
                         void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    int ret = 0;

    struct accGyroCntlPacket cntlPacket;

    ret = rxControlInfo(&cntlPacket, inBuf, inSize, elemInSize, outBuf, outSize, elemOutSize);
    if (ret < 0) {
        sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, ERROR_EVT);
        //osLog(LOG_ERROR, "mxc4005xAccRate, rx inSize and elemSize error\n");
        return -1;
    }
    mTask.sensors[ACC].rate = cntlPacket.rate;
    mTask.sensors[ACC].latency = cntlPacket.latency;
    mTask.watermark = cntlPacket.waterMark;
    //osLog(LOG_ERROR, "mxc4005xAccRate acc rate:%d, latency:%lld, watermark:%d\n",mTask.sensors[ACC].rate, mTask.sensors[ACC].latency,  mTask.watermark);

    


    //registerAccGyroFifoInfo((mTask.sensors[ACC].hwRate == 0) ? 0 : 1024000000000 / mTask.sensors[ACC].hwRate, 0);
    mTask.sensors[ACC].configed = true;
    // osLog(LOG_ERROR, "mxc4005xAccRate acc hwRate:%d\n",mTask.sensors[ACC].hwRate);

  sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
        return 0;
}
#if 0
static int mxc4005xAmdPowerOn(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                            void *inBuf, uint8_t inSize, uint8_t elemInSize,
                            void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    
    mTask.sensors[ANYMO].powered = true;
    osLog(LOG_INFO, "mxc4005xAmdPowerOn any :%d, acc:%d\n",mTask.sensors[ANYMO].powered, mTask.sensors[ACC].powered);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}

static int mxc4005xAmdPowerOff(I2cCallbackF i2cCallBack, SpiCbkF spiCallBack, void *next_state,
                             void *inBuf, uint8_t inSize, uint8_t elemInSize,
                             void *outBuf, uint8_t *outSize, uint8_t *elemOutSize) {
    
	mTask.sensors[ANYMO].configed = false;
    mTask.sensors[ANYMO].powered = false;
    osLog(LOG_INFO, "mxc4005xAmdPowerOff any :%d, acc:%d\n",mTask.sensors[ANYMO].powered, mTask.sensors[ACC].powered);
    sensorFsmEnqueueFakeI2cEvt(i2cCallBack, next_state, SUCCESS_EVT);
    return 0;
}
#endif

static struct sensorFsm mxc4005xFsm[] = {

    sensorFsmCmd(STATE_SW_RESET, STATE_INIT_DONE, mxc4005xSensorRegistration),
    sensorFsmCmd(STATE_ACC_ENABLE, STATE_ACC_ENABLE_DONE, mxc4005xAccPowerOn),
    sensorFsmCmd(STATE_ACC_DISABLE, STATE_ACC_DISABLE_DONE, mxc4005xAccPowerOff),
    sensorFsmCmd(STATE_ACC_RATECHG, STATE_ACC_RATECHG_DONE, mxc4005xAccRate),
	sensorFsmCmd(STATE_SAMPLE, STATE_CONVERT, mxc4005xSample),
    sensorFsmCmd(STATE_CONVERT, STATE_SAMPLE_DONE, mxc4005xConvert),
    sensorFsmCmd(STATE_ACC_CALI, STATE_ACC_CALI_DONE, mxc4005xAccCali),
    sensorFsmCmd(STATE_ACC_CFG, STATE_ACC_CFG_DONE, mxc4005xAccCfgCali),
    /* INT handle */
   // sensorFsmCmd(STATE_HW_INT_STATUS_CHECK, STATE_HW_INT_HANDLING, mxc4005xIntStatusCheck),
    //sensorFsmCmd(STATE_HW_INT_HANDLING, STATE_HW_INT_HANDLING_DONE, mxc4005xIntHandling),
    /* For Anymotion */
    //sensorFsmCmd(STATE_ANYMO_ENABLE, STATE_ANYMO_ENABLE_DONE, mxc4005xAmdPowerOn),
    //sensorFsmCmd(STATE_ANYMO_DISABLE, STATE_ANYMO_DISABLE_DONE, mxc4005xAmdPowerOff),


};

static void initSensorStruct(struct mxc4005xSensor *sensor, enum SensorIndex idx) {
    sensor->powered = false;
    sensor->configed = false;
    sensor->rate = 0;
    sensor->latency = SENSOR_LATENCY_NODATA;
    sensor->hwRate = 0;  // rate set in hw
    
    sensor->startCali = false;
    sensor->staticCali[AXIS_X] = 0;
    sensor->staticCali[AXIS_Y] = 0;
    sensor->staticCali[AXIS_Z] = 0;
    sensor->accuracy = 0;
}

void mxc4005xTimerCbkF(void)
{
    mTask.hwSampleTime = rtcGetTime();
}

int mxc4005xInit(void) {
    int ret = 0;
    //uint8_t txData[2] = {0}, rxData[4] = {0};
    enum SensorIndex i;
    insertMagicNum(&mTask.accGyroPacket);
    osLog(LOG_ERROR, "DRIVER_VERSION: %s\n", DRIVER_VERSION);
    mTask.hw = get_cust_accGyro("mxc4005x");
    if (NULL == mTask.hw) {
        osLog(LOG_ERROR, "get_cust_acc_hw fail\n");
        ret = -1;
     //   goto err_out;
    }
    mTask.i2c_addr = mTask.hw->i2c_addr[0];
    mTask.i2c_addr = MXC4005X_BUS_I2C;	
//	mTask.hw->i2c_num = 1;
   // printf("start linhui acc spi_num: %d\n", mTask.hw->i2c_num);
    if (0 != (ret = sensorDriverGetConvert(mTask.hw->direction, &mTask.cvt))) {
        osLog(LOG_ERROR, "invalid direction: %d\n", mTask.hw->direction);
    }
    osLog(LOG_ERROR, "acc map[0]:%d, map[1]:%d, map[2]:%d, sign[0]:%d, sign[1]:%d, sign[2]:%d\n\r",
        mTask.cvt.map[AXIS_X], mTask.cvt.map[AXIS_Y], mTask.cvt.map[AXIS_Z],
        mTask.cvt.sign[AXIS_X], mTask.cvt.sign[AXIS_Y], mTask.cvt.sign[AXIS_Z]);
    
	mTask.sensors[ACC].sensitivity =  256;
 
    for (i = ACC; i < NUM_OF_SENSOR; i++) {
        initSensorStruct(&mTask.sensors[i], i);
    }
 
    i2cMasterRequest(mTask.hw->i2c_num, I2C_SPEED);
    mTask.txBuf[0] = MXC4005X_REG_CHIP_ID;



   ret =  i2cMasterTxRxSync(mTask.hw->i2c_num, mTask.i2c_addr, mTask.txBuf, 1,
                         &mTask.rxBuf, 1, NULL, NULL);
   osLog(LOG_INFO, "mxc4005x: i2cMasterTxRx return=%d  id=%d\n",ret,mTask.rxBuf[0]);

    if (ret < 0) {
        osLog(LOG_ERROR, "mxc4005x i2cMasterTxRxSync fail!!!\n");
        ret = -1;
        i2cMasterRelease(mTask.hw->i2c_num);
        goto err_out;
    }
    if ( (mTask.rxBuf[0] == MXC400X_ID_1) || (mTask.rxBuf[0] == MXC400X_ID_2)) {  // 
       
    accSensorRegister();
    //anyMotionSensorRegister();
    registerAccGyroInterruptMode(ACC_GYRO_FIFO_UNINTERRUPTIBLE);
    registerAccGyroDriverFsm(mxc4005xFsm, ARRAY_SIZE(mxc4005xFsm));
    registerAccGyroTimerCbk(mxc4005xTimerCbkF);
    } else {
        i2cMasterRelease(mTask.hw->i2c_num);
        osLog(LOG_ERROR, "mxc4005x read id fail!!!\n");
        ret = -1;
        goto err_out;
    }
//osLog(LOG_ERROR, "mxc4005xInit done !\n");
err_out:
    return ret;
}


#ifndef CFG_OVERLAY_INIT_SUPPORT
MODULE_DECLARE(mxc4005x, SENS_TYPE_ACCEL, mxc4005xInit);
#else
#include "mtk_overlay_init.h"
OVERLAY_DECLARE(mxc4005x, OVERLAY_WORK_00, mxc4005xInit);
#endif

