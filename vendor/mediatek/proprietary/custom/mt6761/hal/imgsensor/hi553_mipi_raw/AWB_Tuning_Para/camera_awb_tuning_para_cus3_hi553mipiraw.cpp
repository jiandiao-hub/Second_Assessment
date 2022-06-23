#include "camera_custom_nvram.h"
#include "camera_awb_tuning_para_hi553mipiraw.h"

#define NVRAM_TUNING_PARAM_NUM  6511001

template <>
AWB_NVRAM_T const&
getAWBNVRAM_hi553mipiraw<CAM_SCENARIO_CUSTOM3>() // VT
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
					1131,  // i4R
					512,  // i4G
					743  // i4B
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
					-423,  // i4X
					-446  // i4Y
				},
				// rA
				{
					-328,  // i4X
					-468  // i4Y
				},
				// rTL84
				{
					-187,  // i4X
					-489  // i4Y
				},
				// rCWF
				{
					-164,  // i4X
					-540  // i4Y
				},
				// rDNP
				{
					42,  // i4X
					-422  // i4Y
				},
				// rD65
				{
					155,  // i4X
					-431  // i4Y
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
					-453,  // i4X
					-415  // i4Y
				},
				// rA
				{
					-360,  // i4X
					-443  // i4Y
				},
				// rTL84
				{
					-221,  // i4X
					-474  // i4Y
				},
				// rCWF
				{
					-201,  // i4X
					-526  // i4Y
				},
				// rDNP
				{
					12,  // i4X
					-423  // i4Y
				},
				// rD65
				{
					124,  // i4X
					-440  // i4Y
				},
				// rDF
				{
					111,  // i4X
					-516  // i4Y
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
					528,  // i4R
					512,  // i4G
					1660  // i4B
				},
				// rA
				{
					619,  // i4R
					512,  // i4G
					1504  // i4B
				},
				// rTL84
				{
					771,  // i4R
					512,  // i4G
					1280  // i4B
				},
				// rCWF
				{
					852,  // i4R
					512,  // i4G
					1327  // i4B
				},
				// rDNP
				{
					959,  // i4R
					512,  // i4G
					856  // i4B
				},
				// rD65
				{
					1131,  // i4R
					512,  // i4G
					743  // i4B
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
				4,  // i4RotationAngle
				255,  // i4Cos
				18  // i4Sin
			},
			// rDaylightLocus
			{
				-146,  // i4SlopeNumerator
				128  // i4SlopeDenominator
			},
			// rPredictorGain
			{
				101,  // i4PrefRatio100
				// rSpatial_DaylightLocus_L
				{
					1101,  // i4R
					530,  // i4G
					763  // i4B
				},
				// rSpatial_DaylightLocus_H
				{
					891,  // i4R
					512,  // i4G
					979  // i4B
				},
				// rTemporal_General
				{
					1131,  // i4R
					512,  // i4G
					743  // i4B
				},
				// rAWBGain_LSC
				{
					963,  // i4R
					512,  // i4G
					895  // i4B
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
					-277,  // i4RightBound
					-853,  // i4LeftBound
					-360,  // i4UpperBound
					-459  // i4LowerBound
				},
				// rWarmFluorescent
				{
					-277,  // i4RightBound
					-853,  // i4LeftBound
					-459,  // i4UpperBound
					-616  // i4LowerBound
				},
				// rFluorescent
				{
					-64,  // i4RightBound
					-277,  // i4LeftBound
					-380,  // i4UpperBound
					-500  // i4LowerBound
				},
				// rCWF
				{
					-64,  // i4RightBound
					-277,  // i4LeftBound
					-500,  // i4UpperBound
					-581  // i4LowerBound
				},
				// rDaylight
				{
					154,  // i4RightBound
					-64,  // i4LeftBound
					-380,  // i4UpperBound
					-500  // i4LowerBound
				},
				// rShade
				{
					484,  // i4RightBound
					154,  // i4LeftBound
					-380,  // i4UpperBound
					-474  // i4LowerBound
				},
				// rDaylightFluorescent
				{
					154,  // i4RightBound
					-64,  // i4LeftBound
					-500,  // i4UpperBound
					-581  // i4LowerBound
				}
			},
			// rPWBLightArea
			{
				// rReferenceArea
				{
					484,  // i4RightBound
					-853,  // i4LeftBound
					-335,  // i4UpperBound
					-616  // i4LowerBound
				},
				// rDaylight
				{
					179,  // i4RightBound
					-64,  // i4LeftBound
					-380,  // i4UpperBound
					-500  // i4LowerBound
				},
				// rCloudyDaylight
				{
					279,  // i4RightBound
					104,  // i4LeftBound
					-380,  // i4UpperBound
					-500  // i4LowerBound
				},
				// rShade
				{
					379,  // i4RightBound
					104,  // i4LeftBound
					-380,  // i4UpperBound
					-500  // i4LowerBound
				},
				// rTwilight
				{
					-64,  // i4RightBound
					-224,  // i4LeftBound
					-380,  // i4UpperBound
					-500  // i4LowerBound
				},
				// rFluorescent
				{
					174,  // i4RightBound
					-321,  // i4LeftBound
					-390,  // i4UpperBound
					-576  // i4LowerBound
				},
				// rWarmFluorescent
				{
					-260,  // i4RightBound
					-460,  // i4LeftBound
					-390,  // i4UpperBound
					-576  // i4LowerBound
				},
				// rIncandescent
				{
					-260,  // i4RightBound
					-460,  // i4LeftBound
					-380,  // i4UpperBound
					-500  // i4LowerBound
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
					1041,  // i4R
					512,  // i4G
					819  // i4B
				},
				// rCloudyDaylight
				{
					1232,  // i4R
					512,  // i4G
					675  // i4B
				},
				// rShade
				{
					1311,  // i4R
					512,  // i4G
					627  // i4B
				},
				// rTwilight
				{
					808,  // i4R
					512,  // i4G
					1096  // i4B
				},
				// rFluorescent
				{
					939,  // i4R
					512,  // i4G
					1045  // i4B
				},
				// rWarmFluorescent
				{
					655,  // i4R
					512,  // i4G
					1582  // i4B
				},
				// rIncandescent
				{
					616,  // i4R
					512,  // i4G
					1499  // i4B
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
					5675  // i4OffsetThr
				},
				// rWarmFluorescent
				{
					40,  // i4SliderValue
					5675  // i4OffsetThr
				},
				// rShade
				{
					50,  // i4SliderValue
					910  // i4OffsetThr
				},
				// rSunsetArea
				{
					70,  // i4BoundXrThr
					-423  // i4BoundYrThr
				},
				// rShadeFArea
				{
					-277,  // i4BoundXrThr
					-478  // i4BoundYrThr
				},
				// rShadeFVerTex
				{
					-171,  // i4BoundXrThr
					-489  // i4BoundYrThr
				},
				// rShadeCWFArea
				{
					-277,  // i4BoundXrThr
					-530  // i4BoundYrThr
				},
				// rShadeCWFVerTex
				{
					-171,  // i4BoundXrThr
					-556  // i4BoundYrThr
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
					-577, -484, -345, -112, 0
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
				0,  // bEnable
				0  // i4ScalingFactor
			},
			// rPredictorLVThr
			{
				0,  // i4InitLVThr_L
				0,  // i4InitLVThr_H
				0  // i4EnqueueLVThr
			},
			// rTemporalEnqueueThr
			{
				0,  // i4Neutral_ParentBlk_Thr
				// i4CWFDF_LUTThr
				{
					0, 0, 0, 0, 0, 0, 100, 100, 100, 100,
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
