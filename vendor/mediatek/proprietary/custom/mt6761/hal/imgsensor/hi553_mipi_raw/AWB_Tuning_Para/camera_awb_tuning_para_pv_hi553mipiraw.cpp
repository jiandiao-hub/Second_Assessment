#include "camera_custom_nvram.h"
#include "camera_awb_tuning_para_hi553mipiraw.h"

#define NVRAM_TUNING_PARAM_NUM  6511001

template <>
AWB_NVRAM_T const&
getAWBNVRAM_hi553mipiraw<CAM_SCENARIO_PREVIEW>() // PREVIEW
{
    static AWB_NVRAM_T  g_rAWBNVRAM =
    AWB_NVRAM_START
    {
   {
  // AWB calibration data
  {
    // rUnitGain (unit gain: 1.0 = 512)
    {
      0,  // i4R
      0,  // i4G
      0 // i4B
    },
    // rGoldenGain (golden sample gain: 1.0 = 512)
    {
      0,  // i4R
      0,  // i4G
      0 // i4B
    },
                            // rUnitGain TL84 (unit gain: 1.0 = 512)
                            {
                                0,    // i4R
                                0,    // i4G
                                0     // i4B
                            },
                            // rGoldenGain TL84 (golden sample gain: 1.0 = 512)
                            {
                                0,    // i4R
                                0,    // i4G
                                0     // i4B
                            },
                             // rUnitGain Alight (unit gain: 1.0 = 512)
                            {
                                0,    // i4R
                                0,    // i4G
                                0     // i4B
                            },
                            // rGoldenGain Alight (golden sample gain: 1.0 = 512)
                            {
                                0,    // i4R
                                0,    // i4G
                                0     // i4B
                            },
    // rTuningUnitGain (Tuning sample unit gain: 1.0 = 512)
    {
      0,  // i4R
      0,  // i4G
      0 // i4B
    },
    // rD65Gain (D65 WB gain: 1.0 = 512)
				{
					847,  // i4R
					512,  // i4G
					816  // i4B
				}
			},
			// rOriginalXY
			{
				// rStrobe
				{
					0,  // i4X
					0  // i4Y
				},
				// rHorizon
				{
					-492,  // i4X
					-339  // i4Y
				},
				// rA
				{
					-401,  // i4X
					-357  // i4Y
				},
				// rTL84
				{
					-237,  // i4X
					-331  // i4Y
				},
				// rCWF
				{
					-204,  // i4X
					-388  // i4Y
				},
				// rDNP
				{
					14,  // i4X
					-356  // i4Y
				},
				// rD65
				{
					14,  // i4X
					-358  // i4Y
				},
				// rDF
				{
					0,  // i4X
					0  // i4Y
				}
			},
			// rRotatedXY
			{
				// rStrobe
				{
					0,  // i4X
					0  // i4Y
				},
				// rHorizon
				{
					-480,  // i4X
					-356  // i4Y
				},
				// rA
				{
					-388,  // i4X
					-371  // i4Y
				},
				// rTL84
				{
					-225,  // i4X
					-339  // i4Y
				},
				// rCWF
				{
					-190,  // i4X
					-395  // i4Y
				},
				// rDNP
				{
					27,  // i4X
					-356  // i4Y
				},
				// rD65
				{
					27,  // i4X
					-358  // i4Y
				},
				// rDF
				{
					30,  // i4X
					-391  // i4Y
				}
			},
			// rLightAWBGain
			{
				// rStrobe
				{
					512,  // i4R
					512,  // i4G
					512  // i4B
				},
				// rHorizon
				{
					512,  // i4R
					631,  // i4G
					1943  // i4B
				},
				// rA
				{
					512,  // i4R
					543,  // i4G
					1515  // i4B
				},
				// rTL84
				{
					581,  // i4R
					512,  // i4G
					1105  // i4B
				},
				// rCWF
				{
					657,  // i4R
					512,  // i4G
					1140  // i4B
				},
				// rDNP
				{
					845,  // i4R
					512,  // i4G
					813  // i4B
				},
				// rD65
				{
					847,  // i4R
					512,  // i4G
					816  // i4B
				},
				// rDF
				{
					512,  // i4R
					512,  // i4G
					512  // i4B
				}
			},
			// rRotationMatrix
			{
				-2,  // i4RotationAngle
				256,  // i4Cos
				-9  // i4Sin
			},
			// rDaylightLocus
			{
				-119,  // i4SlopeNumerator
				128  // i4SlopeDenominator
			},
			// rPredictorGain
			{
				101,  // i4PrefRatio100
				// rSpatial_DaylightLocus_L
				{
					817,  // i4R
					530,  // i4G
					836  // i4B
				},
				// rSpatial_DaylightLocus_H
				{
					879,  // i4R
					512,  // i4G
					789  // i4B
				},
				// rTemporal_General
				{
					847,  // i4R
					512,  // i4G
					816  // i4B
				},
				// rAWBGain_LSC
				{
					848,  // i4R
					512,  // i4G
					816  // i4B
				}
			},
			// rAWBLightArea
			{
				// rStrobe
				{
					0,  // i4RightBound
					0,  // i4LeftBound
					0,  // i4UpperBound
					0  // i4LowerBound
				},
				// rTungsten
				{
					-290,  // i4RightBound
					-880,  // i4LeftBound
					-301,  // i4UpperBound
					-355  // i4LowerBound
				},
				// rWarmFluorescent
				{
					-290,  // i4RightBound
					-880,  // i4LeftBound
					-355,  // i4UpperBound
					-520  // i4LowerBound
				},
				// rFluorescent
				{
					-55,  // i4RightBound
					-290,  // i4LeftBound
					-298,  // i4UpperBound
					-367  // i4LowerBound
				},
				// rCWF
				{
					-93,  // i4RightBound
					-290,  // i4LeftBound
					-367,  // i4UpperBound
					-520  // i4LowerBound
				},
				// rDaylight
				{
					150,  // i4RightBound
					-55,  // i4LeftBound
					-298,  // i4UpperBound
					-367  // i4LowerBound
				},
				// rShade
				{
					387,  // i4RightBound
					150,  // i4LeftBound
					-298,  // i4UpperBound
					-373  // i4LowerBound
				},
				// rDaylightFluorescent
				{
					150,  // i4RightBound
					-93,  // i4LeftBound
					-367,  // i4UpperBound
					-520  // i4LowerBound
				}
			},
			// rPWBLightArea
			{
				// rReferenceArea
				{
					387,  // i4RightBound
					-880,  // i4LeftBound
					0,  // i4UpperBound
					-520  // i4LowerBound
				},
				// rDaylight
				{
					175,  // i4RightBound
					-55,  // i4LeftBound
					-298,  // i4UpperBound
					-367  // i4LowerBound
				},
				// rCloudyDaylight
				{
					275,  // i4RightBound
					100,  // i4LeftBound
					-298,  // i4UpperBound
					-367  // i4LowerBound
				},
				// rShade
				{
					375,  // i4RightBound
					100,  // i4LeftBound
					-298,  // i4UpperBound
					-367  // i4LowerBound
				},
				// rTwilight
				{
					-55,  // i4RightBound
					-215,  // i4LeftBound
					-298,  // i4UpperBound
					-367  // i4LowerBound
				},
				// rFluorescent
				{
					77,  // i4RightBound
					-325,  // i4LeftBound
					-289,  // i4UpperBound
					-445  // i4LowerBound
				},
				// rWarmFluorescent
				{
					-288,  // i4RightBound
					-488,  // i4LeftBound
					-289,  // i4UpperBound
					-445  // i4LowerBound
				},
				// rIncandescent
				{
					-288,  // i4RightBound
					-488,  // i4LeftBound
					-298,  // i4UpperBound
					-367  // i4LowerBound
				},
				// rGrayWorld
				{
					5000,  // i4RightBound
					-5000,  // i4LeftBound
					5000,  // i4UpperBound
					-5000  // i4LowerBound
				}
			},
			// rPWBDefaultGain
			{
				// rDaylight
				{
					859,  // i4R
					512,  // i4G
					754  // i4B
				},
				// rCloudyDaylight
				{
					1027,  // i4R
					512,  // i4G
					638  // i4B
				},
				// rShade
				{
					1102,  // i4R
					512,  // i4G
					598  // i4B
				},
				// rTwilight
				{
					654,  // i4R
					512,  // i4G
					972  // i4B
				},
				// rFluorescent
				{
					695,  // i4R
					512,  // i4G
					1006  // i4B
				},
				// rWarmFluorescent
				{
					480,  // i4R
					512,  // i4G
					1420  // i4B
				},
				// rIncandescent
				{
					459,  // i4R
					512,  // i4G
					1353  // i4B
				},
				// rGrayWorld
				{
					512,  // i4R
					512,  // i4G
					512  // i4B
				}
			},
			// rPreferenceColor
			{
				// rTungsten
				{
					40,  // i4SliderValue
					4766  // i4OffsetThr
				},
				// rWarmFluorescent
				{
					40,  // i4SliderValue
					4521  // i4OffsetThr
				},
				// rShade
				{
					50,  // i4SliderValue
					1677  // i4OffsetThr
				},
				// rSunsetArea
				{
					90,  // i4BoundXrThr
					-356  // i4BoundYrThr
				},
				// rShadeFArea
				{
					-290,  // i4BoundXrThr
					-343  // i4BoundYrThr
				},
				// rShadeFVerTex
				{
					-173,  // i4BoundXrThr
					-355  // i4BoundYrThr
				},
				// rShadeCWFArea
				{
					-290,  // i4BoundXrThr
					-399  // i4BoundYrThr
				},
				// rShadeCWFVerTex
				{
					-192,  // i4BoundXrThr
					-425  // i4BoundYrThr
				}
			},
			// rCCTEstimation
			{
				// i4CCT
				{
					2300, 2850, 3750, 5100, 6500
				},
				// i4RotatedXCoordinate
				{
					-507, -415, -252, 0, 0
				}
			}
		},
		// rAlgoTuningParam
		{
			0,  // bAWBBackupEnable
			// rDaylightLocusOffsetLUTs_Tungsten
			{
				21,  // i4LUTSize
				// i4LUTIn
				{
					0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500,
					5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500,
					10000
				},
				// i4LUTOut
				{
					0, 350, 800, 1222, 1444, 1667, 1889, 2111, 2333, 2556,
					2778, 3000, 3222, 3444, 3667, 3889, 4111, 4333, 4556, 4778,
					5000
				}
			},
			// rDaylightLocusOffsetLUTs_WF
			{
				21,  // i4LUTSize
				// i4LUTIn
				{
					0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500,
					5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500,
					10000
				},
				// i4LUTOut
				{
					0, 350, 700, 850, 1200, 1667, 1889, 2111, 2333, 2556,
					2778, 3000, 3222, 3444, 3667, 3889, 4111, 4333, 4556, 4778,
					5000
				}
			},
			// rDaylightLocusOffsetLUTs_Shade
			{
				21,  // i4LUTSize
				// i4LUTIn
				{
					0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500,
					5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500,
					10000
				},
				// i4LUTOut
				{
					0, 500, 1000, 1500, 2500, 3500, 4000, 4500, 5000, 5500,
					6000, 6500, 7000, 7500, 8000, 8000, 8000, 8500, 9000, 9500,
					10000
				}
			},
			// rLightPrefGain
			{
				{
					{
						512,  // i4R
						512,  // i4G
						512  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						512  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						512  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						512  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						512  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						512  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						512  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						512  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						512  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						512  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						512  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						512  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						512  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						512  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						512  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						512  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						512  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						512  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						512  // i4B
					}
				},
				{
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					}
				},
				{
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					}
				},
				{
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					}
				},
				{
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					}
				},
				{
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					}
				},
				{
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					}
				},
				{
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					},
					{
						512,  // i4R
						512,  // i4G
						500  // i4B
					}
				}
			},
			// rParentBlkWeightParam
			{
				1,  // bEnable
				6  // i4ScalingFactor
			},
			// rPredictorLVThr
			{
				115,  // i4InitLVThr_L
				155,  // i4InitLVThr_H
				100  // i4EnqueueLVThr
			},
			// rTemporalEnqueueThr
			{
				65,  // i4Neutral_ParentBlk_Thr
				// i4CWFDF_LUTThr
				{
					100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
					50, 25, 2, 2, 2, 2, 2, 2, 2
				}
			},
			// rNeutralBlkThr
			{
				// m_i4NonNeutral
				{
					3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
					5, 10, 10, 10, 10, 10, 10, 10, 10
				},
				// m_i4F
				{
					0, 0, 0, 0, 0, 3, 5, 5, 5, 5,
					5, 10, 10, 10, 10, 10, 10, 10, 10
				},
				// m_i4CWF
				{
					0, 0, 0, 0, 0, 3, 5, 5, 5, 5,
					5, 10, 10, 10, 10, 10, 10, 10, 10
				},
				// m_i4Daylight
				{
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 2, 2, 2, 2, 2, 2, 2, 2
				},
				// m_i4DF
				{
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					5, 10, 10, 10, 10, 10, 10, 10, 10
				}
			},
			// rFeaturePropThr
			{
				// rSunsetProp
				{
					1,  // i4Enable
					120,  // i4LVThr_L
					130,  // i4LVThr_H
					10,  // i4SunsetCountThr
					0,  // i4SunsetCountRatio_L
					171  // i4SunsetCountRatio_H
				},
				// rShadeFProp
				{
					1,  // i4Enable
					50,  // i4LVThr_L
					90,  // i4LVThr_H
					128  // i4DaylightProb
				},
				// rShadeCWFProp
				{
					1,  // i4Enable
					50,  // i4LVThr_L
					90,  // i4LVThr_H
					192  // i4DaylightProb
				}
			},
			// rNonNeutralProb
			{
				// i4LUT
				{
					0, 33, 66, 100, 100, 100, 100, 100, 100, 100,
					100, 70, 30, 20, 10, 0, 0, 0, 0
				}
			},
			// rDaylightLocusProb
			{
				{
					// i4LUT
					{
						100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
						100, 100, 100, 50, 25, 0, 0, 0, 0
					}
				},
				{
					// i4LUT
					{
						100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
						100, 100, 100, 75, 50, 25, 0, 0, 0
					}
				},
				{
					// i4LUT
					{
						100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
						100, 75, 50, 25, 25, 25, 0, 0, 0
					}
				},
				{
					// i4LUT
					{
						100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
						95, 75, 50, 25, 25, 25, 0, 0, 0
					}
				},
				{
					// i4LUT
					{
						90, 90, 90, 90, 90, 90, 90, 90, 90, 90,
						80, 55, 30, 30, 30, 30, 0, 0, 0
					}
				},
				{
					// i4LUT
					{
						100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
						100, 100, 100, 100, 75, 50, 40, 30, 20
					}
				},
				{
					// i4LUT
					{
						100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
						100, 100, 75, 50, 25, 0, 0, 0, 0
					}
				},
				{
					// i4LUT
					{
						90, 90, 90, 90, 90, 90, 90, 90, 90, 90,
						80, 55, 30, 30, 30, 30, 0, 0, 0
					}
				}
			},
			// rTuningInfo
			{
				0,  // i4ProjCode
				0,  // i4Model
				0,  // i4Date
				0,  // i4Res0
				0,  // i4Res1
				0,  // i4Res2
				0,  // i4Res3
				0  // i4Res4
                },
       }
};
    return g_rAWBNVRAM;
};
