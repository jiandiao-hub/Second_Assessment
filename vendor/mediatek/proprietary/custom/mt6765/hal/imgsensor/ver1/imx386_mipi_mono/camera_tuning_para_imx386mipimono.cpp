#include <log/log.h>
#include <fcntl.h>
#include <math.h>
#include <log/log.h>
#include <string.h>
#include "camera_custom_nvram.h"
#include "camera_custom_sensor.h"
#include "image_sensor.h"
#include "kd_imgsensor_define.h"
#include "camera_AE_PLineTable_imx386mipimono.h"
#include "camera_info_imx386mipimono.h"
#include <custom/aaa/AEPlinetable.h>
#include "camera_custom_tsf_tbl.h"
#include "AE_Tuning_Para/camera_ae_tuning_para_imx386mipimono.h"
#include "AWB_Tuning_Para/camera_awb_tuning_para_imx386mipimono.h"

#define NVRAM_TUNING_PARAM_NUM  6181001

extern "C" fptrDefault getDefaultDataFunc(MUINT32 sensorType)
{
    if(sensorType==0)
        return NSFeature::RAWSensorInfo<SENSOR_ID>::getDefaultData;
    else
        return NSFeature::YUVSensorInfo<SENSOR_ID>::getDefaultData;
}

const NVRAM_CAMERA_ISP_PARAM_STRUCT CAMERA_ISP_DEFAULT_VALUE =
{{
    //Version
    .Version= NVRAM_CAMERA_PARA_FILE_VERSION,

    //SensorId
    .SensorId= SENSOR_ID,
    .ISPComm={
      {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      }
    },
    .ISPRegs={
#include INCLUDE_FILENAME_ISP_REGS_PARAM
    },
    .ISPColorTbl= {
#include INCLUDE_FILENAME_ISP_COLORTBL_PARAM
    },
    .ISPToneMap={
#include INCLUDE_FILENAME_ISP_TONEMAP_PARAM
    },
    .DngMetadata=
    {
        0,  //i4RefereceIlluminant1
        3,  //i4RefereceIlluminant2
        {
            // rNoiseProfile[4]
            {
                {3.530980e-6, 3.357493e-4},
                {8.098436e-8, -2.336529e-4},
            },
            {
                {3.530980e-6, 3.357493e-4},
                {8.098436e-8, -2.336529e-4},
            },
            {
                {3.530980e-6, 3.357493e-4},
                {8.098436e-8, -2.336529e-4},
            },
            {
                {3.530980e-6, 3.357493e-4},
                {8.098436e-8, -2.336529e-4},
            },
        },
    }
}};

const NVRAM_CAMERA_3A_STRUCT CAMERA_3A_NVRAM_DEFAULT_VALUE =
{
    NVRAM_CAMERA_3A_FILE_VERSION, // u4Version
    SENSOR_ID, // SensorId

    // AE NVRAM
    {
        getAENVRAM_imx386mipimono<CAM_SCENARIO_PREVIEW>(),
        getAENVRAM_imx386mipimono<CAM_SCENARIO_VIDEO>(),
        getAENVRAM_imx386mipimono<CAM_SCENARIO_CAPTURE>(),
        getAENVRAM_imx386mipimono<CAM_SCENARIO_CUSTOM1>(),
        getAENVRAM_imx386mipimono<CAM_SCENARIO_CUSTOM2>(),
        getAENVRAM_imx386mipimono<CAM_SCENARIO_CUSTOM3>(),
        getAENVRAM_imx386mipimono<CAM_SCENARIO_CUSTOM4>()
    },

    // AWB NVRAM
    AWB_NVRAM_START
    {
        getAWBNVRAM_imx386mipimono<CAM_SCENARIO_PREVIEW>(),
        getAWBNVRAM_imx386mipimono<CAM_SCENARIO_VIDEO>(),
        getAWBNVRAM_imx386mipimono<CAM_SCENARIO_CAPTURE>(),
        getAWBNVRAM_imx386mipimono<CAM_SCENARIO_CUSTOM1>(),
        getAWBNVRAM_imx386mipimono<CAM_SCENARIO_CUSTOM2>(),
        getAWBNVRAM_imx386mipimono<CAM_SCENARIO_CUSTOM3>(),
        getAWBNVRAM_imx386mipimono<CAM_SCENARIO_CUSTOM4>()
    },

    // Flash AWB NVRAM
    {
#include INCLUDE_FILENAME_FLASH_AWB_PARA
    },

    {0}
};

#include INCLUDE_FILENAME_ISP_LSC_PARAM
//};  //  namespace

const CAMERA_TSF_TBL_STRUCT CAMERA_TSF_DEFAULT_VALUE =
{
    {
        0,  // isTsfEn
        2,  // tsfCtIdx
        {20, 2000, -110, -110, 512, 512, 512, 0}    // rAWBInput[8]
    },

#include INCLUDE_FILENAME_TSF_PARA
#include INCLUDE_FILENAME_TSF_DATA
};

const NVRAM_CAMERA_FEATURE_STRUCT CAMERA_FEATURE_DEFAULT_VALUE =
{
#include INCLUDE_FILENAME_FEATURE_PARA
};

typedef NSFeature::RAWSensorInfo<SENSOR_ID> SensorInfoSingleton_T;


namespace NSFeature {
  template <>
  UINT32
  SensorInfoSingleton_T::
  impGetDefaultData(CAMERA_DATA_TYPE_ENUM const CameraDataType, VOID*const pDataBuf, UINT32 const size) const
  {
    UINT32 dataSize[CAMERA_DATA_TYPE_NUM] = {sizeof(NVRAM_CAMERA_ISP_PARAM_STRUCT),
        sizeof(NVRAM_CAMERA_3A_STRUCT),
        sizeof(NVRAM_CAMERA_SHADING_STRUCT),
        sizeof(NVRAM_LENS_PARA_STRUCT),
        sizeof(AE_PLINETABLE_T),
        0,
        sizeof(CAMERA_TSF_TBL_STRUCT),
        0,
        0,
        sizeof(NVRAM_CAMERA_FEATURE_STRUCT)
    };

    if (CameraDataType > CAMERA_NVRAM_DATA_FEATURE || NULL == pDataBuf || (size < dataSize[CameraDataType]))
    {
      return 1;
    }

    switch(CameraDataType)
    {
      case CAMERA_NVRAM_DATA_ISP:
        memcpy(pDataBuf,&CAMERA_ISP_DEFAULT_VALUE,sizeof(NVRAM_CAMERA_ISP_PARAM_STRUCT));
        break;
      case CAMERA_NVRAM_DATA_3A:
        memcpy(pDataBuf,&CAMERA_3A_NVRAM_DEFAULT_VALUE,sizeof(NVRAM_CAMERA_3A_STRUCT));
        break;
      case CAMERA_NVRAM_DATA_SHADING:
        memcpy(pDataBuf,&CAMERA_SHADING_DEFAULT_VALUE,sizeof(NVRAM_CAMERA_SHADING_STRUCT));
        break;
      case CAMERA_DATA_AE_PLINETABLE:
        memcpy(pDataBuf,&g_PlineTableMapping,sizeof(AE_PLINETABLE_T));
        break;
      case CAMERA_DATA_TSF_TABLE:
        memcpy(pDataBuf,&CAMERA_TSF_DEFAULT_VALUE,sizeof(CAMERA_TSF_TBL_STRUCT));
        break;
      case CAMERA_NVRAM_DATA_FEATURE:
        memcpy(pDataBuf,&CAMERA_FEATURE_DEFAULT_VALUE,sizeof(NVRAM_CAMERA_FEATURE_STRUCT));
        break;
      default:
        break;
    }
    return 0;
  }};  //  NSFeature


