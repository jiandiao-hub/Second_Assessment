#include <cutils/log.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>

#include "camera_custom_nvram.h"
#include "camera_custom_sensor.h"
#include "image_sensor.h"
#include "kd_imgsensor_define.h"
#include "camera_AE_PLineTable_gc5035mipiraw.h"
#include "camera_info_gc5035mipiraw.h"
#include "camera_custom_AEPlinetable.h"
#include "camera_custom_tsf_tbl.h"
#include "AE_Tuning_Para/camera_ae_tuning_para_gc5035mipiraw.h"
#include "AWB_Tuning_Para/camera_awb_tuning_para_gc5035mipiraw.h"

#define NVRAM_TUNING_PARAM_NUM  5341001
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
    .ISPPca= {
#include INCLUDE_FILENAME_ISP_PCA_PARAM
    },
    .ISPRegs={
#include INCLUDE_FILENAME_ISP_REGS_PARAM
    },
    .ISPMfbMixer={{
      0x01FF0001, // MIX3_CTRL_0
      0x00FF0000, // MIX3_CTRL_1
      0xFFFF0000  // MIX3_SPARE
    }},
    .ISPMulitCCM={
      .Poly22={
				67775,  // i4R_AVG
				17159,  // i4R_STD
				91025,  // i4B_AVG
				20621,  // i4B_STD
        4095,      // i4R_MAX
        512,      // i4R_MIN
        4095,      // i4G_MAX
        512,      // i4G_MIN
        4095,      // i4B_MAX
        512,      // i4B_MIN
                { // i4P00[9]
					8515000, -3352500, -45000, -1655000, 7642500, -862500, -350000, -2840000, 8317500
                },
                { // i4P10[9]
					936001, -1632819, 706905, 178424, -69564, -84577, 29256, -541534, 492963
                },
                { // i4P01[9]
					-43646, -741008, 785380, 29401, -571742, 558922, 9184, -1196324, 1157746
                },
                { // i4P20[9]
                788014,-983900,196062,-43050,119624,-76574,281758,-1043902,762090
                },
                { // i4P11[9]
                -71500,-689612,761476,243148,119000,-362148,286776,-619070,332618
                },
                { // i4P02[9]
                -631502,130466,501236,302926,68298,-371224,43616,-17274,-25994
                }

      },
      .AWBGain={
        // Strobe
        {
          810,    // i4R
          512,    // i4G
          677    // i4B
        },
        // A
        {
						556,  // i4R
						512,  // i4G
						1142  // i4B
        },
        // TL84
        {
						556,  // i4R
						512,  // i4G
						923  // i4B
        },
        // CWF
        {
						679,  // i4R
						512,  // i4G
						936  // i4B
        },
        // D65
        {
						920,  // i4R
						512,  // i4G
						640  // i4B
        },
        // Reserved 1
        {
            512,    // i4R
            512,    // i4G
            512    // i4B
        },
        // Reserved 2
        {
            512,    // i4R
            512,    // i4G
            512    // i4B
        },
        // Reserved 3
        {
            512,    // i4R
            512,    // i4G
            512    // i4B
        }
      },
      .Weight={
        1, // Strobe
        1, // A
        1, // TL84
        1, // CWF
        1, // D65
        1, // Reserved 1
        1, // Reserved 2
        1  // Reserved 3
      }
    },

          MDPMulitCCM:{
                .Poly22={
                  125225, // i4R_AVG
                   32141, // i4R_STD
                  165675, // i4B_AVG
                   30249, // i4B_STD
                     646, // i4R_MAX
                     527, // i4R_MIN
                     789, // i4G_MAX
                     670, // i4G_MIN
                     856, // i4B_MAX
                     721, // i4B_MIN
                  {  // i4P00[9]
                     5995000,   -505000,   -370000,  -1387500,   7165000,   -657500,   -532500,  -2045000,   7697500
                  },
                  {  // i4P10[9]
                      725905,  -1081879,    355974,    627096,   -389018,   -238079,    626246,    -86040,   -540207
                  },
                  {  // i4P01[9]
                      257857,   -576218,    318361,    -96902,   -138571,    235473,     24959,    193148,   -218107
                  },
                  {  // i4P20[9]
                      0, 0, 0, 0, 0, 0, 0, 0, 0
                          },
                          { // i4P11[9]
                      0, 0, 0, 0, 0, 0, 0, 0, 0
                          },
                          { // i4P02[9]
                      0, 0, 0, 0, 0, 0, 0, 0, 0
                          }

                },
                .AWBGain={
                  // Strobe
                  {
                    1016,    // i4R
                    512,    // i4G
                    1101    // i4B
                  },
                  // A
                  {
                    878,    // i4R
                      512,    // i4G
                    1971    // i4B
                  },
                  // TL84
                  {
                    1192,    // i4R
                      512,    // i4G
                    1628    // i4B
                  },
                  // CWF
                  {
                    1280,    // i4R
                      512,    // i4G
                    1773    // i4B
                  },
                  // D65
                  {
                    1659,    // i4R
                      512,    // i4G
                    1255    // i4B
                  },
                  // Reserved 1
                  {
                      512,    // i4R
                      512,    // i4G
                      512    // i4B
                  },
                  // Reserved 2
                  {
                      512,    // i4R
                      512,    // i4G
                      512    // i4B
                  },
                  // Reserved 3
                  {
                      512,    // i4R
                      512,    // i4G
                      512    // i4B
                  }
                },
                .Weight={
                  1, // Strobe
                  1, // A
                  1, // TL84
                  1, // CWF
                  1, // D65
                  1, // Reserved 1
                  1, // Reserved 2
                  1  // Reserved 3
                }
              },
    .isp_ccm_ratio= 0.5,    
    //bInvokeSmoothCCM
    .bInvokeSmoothCCM= MTRUE,
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
    },
    .ANR_TBL=
    {0}
}};

const NVRAM_CAMERA_3A_STRUCT CAMERA_3A_NVRAM_DEFAULT_VALUE =
{
    NVRAM_CAMERA_3A_FILE_VERSION, // u4Version
    SENSOR_ID, // SensorId

    // AE NVRAM
    {
        getAENVRAM_gc5035mipiraw<CAM_SCENARIO_PREVIEW>(),
        getAENVRAM_gc5035mipiraw<CAM_SCENARIO_VIDEO>(),
        getAENVRAM_gc5035mipiraw<CAM_SCENARIO_CAPTURE>(),
        getAENVRAM_gc5035mipiraw<CAM_SCENARIO_CUSTOM1>(),
        getAENVRAM_gc5035mipiraw<CAM_SCENARIO_CUSTOM2>(),
        getAENVRAM_gc5035mipiraw<CAM_SCENARIO_CUSTOM3>(),
        getAENVRAM_gc5035mipiraw<CAM_SCENARIO_CUSTOM4>()
    },

        // AWB NVRAM
    AWB_NVRAM_START
    {
        getAWBNVRAM_gc5035mipiraw<CAM_SCENARIO_PREVIEW>(),
        getAWBNVRAM_gc5035mipiraw<CAM_SCENARIO_VIDEO>(),
        getAWBNVRAM_gc5035mipiraw<CAM_SCENARIO_CAPTURE>(),
        getAWBNVRAM_gc5035mipiraw<CAM_SCENARIO_CUSTOM1>(),
        getAWBNVRAM_gc5035mipiraw<CAM_SCENARIO_CUSTOM2>(),
        getAWBNVRAM_gc5035mipiraw<CAM_SCENARIO_CUSTOM3>(),
        getAWBNVRAM_gc5035mipiraw<CAM_SCENARIO_CUSTOM4>()
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
        1,  // isTsfEn
        2,  // tsfCtIdx
        {20, 2000, -110, -110, 512, 512, 512, 0}    // rAWBInput[8]
    },

#include INCLUDE_FILENAME_TSF_PARA
#include INCLUDE_FILENAME_TSF_DATA
};

#include INCLUDE_FILENAME_BPCI_PARA

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
        sizeof(CAMERA_BPCI_STRUCT),
        0,
        sizeof(NVRAM_CAMERA_FEATURE_STRUCT)
    };

    if (CameraDataType > CAMERA_NVRAM_DATA_FEATURE || NULL == pDataBuf || (size != dataSize[CameraDataType]))
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
      case CAMERA_DATA_PDC_TABLE:
        memcpy(pDataBuf,&CAMERA_BPCI_DEFAULT_VALUE, sizeof(CAMERA_BPCI_STRUCT));
        break;
      default:
        break;
    }
    return 0;
  }};  //  NSFeature


