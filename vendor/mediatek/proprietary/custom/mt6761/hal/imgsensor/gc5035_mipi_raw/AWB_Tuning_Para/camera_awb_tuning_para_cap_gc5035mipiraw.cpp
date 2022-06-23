/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
*/
/* MediaTek Inc. (C) 2016. All rights reserved.
*
* BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
* THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
* RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
* AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
* NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
* SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
* SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
* THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
* THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
* CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
* SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
* STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
* CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
* AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
* OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
* MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
* The following software/firmware and/or related documentation ("MediaTek Software")
* have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
*/

/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
#include "camera_custom_nvram.h"

#include "camera_awb_tuning_para_gc5035mipiraw.h"

#define NVRAM_TUNING_PARAM_NUM  6511001

template <>
AWB_NVRAM_T const&
getAWBNVRAM_gc5035mipiraw<CAM_SCENARIO_CAPTURE>()
{
	static AWB_NVRAM_T  g_rAWBNVRAM =
	AWB_NVRAM_START
	{
		// rAlgoCalParam
		{
			// rCalData
			{
				// rUnitGain
				{
					0,  // i4R
					0,  // i4G
					0  // i4B
				},
				// rGoldenGain
				{
					0,  // i4R
					0,  // i4G
					0  // i4B
				},
				// rUnitGainM
				{
					0,  // i4R
					0,  // i4G
					0  // i4B
				},
				// rGoldenGainM
				{
					0,  // i4R
					0,  // i4G
					0  // i4B
				},
				// rUnitGainL
				{
					0,  // i4R
					0,  // i4G
					0  // i4B
				},
				// rGoldenGainL
				{
					0,  // i4R
					0,  // i4G
					0  // i4B
				},
				// rTuningUnitGain
				{
					0,  // i4R
					0,  // i4G
					0  // i4B
				},
				// rD65Gain
				{
					869,  // i4R
					512,  // i4G
					682  // i4B
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
					-461,  // i4X
					-320  // i4Y
				},
				// rA
				{
					-308,  // i4X
					-329  // i4Y
				},
				// rTL84
				{
					-174,  // i4X
					-318  // i4Y
				},
				// rCWF
				{
					-144,  // i4X
					-379  // i4Y
				},
				// rDNP
				{
					-19,  // i4X
					-327  // i4Y
				},
				// rD65
				{
					90,  // i4X
					-301  // i4Y
				},
				// rDF
				{
					28,  // i4X
					-324  // i4Y
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
					-477,  // i4X
					-297  // i4Y
				},
				// rA
				{
					-325,  // i4X
					-313  // i4Y
				},
				// rTL84
				{
					-190,  // i4X
					-309  // i4Y
				},
				// rCWF
				{
					-163,  // i4X
					-372  // i4Y
				},
				// rDNP
				{
					-36,  // i4X
					-326  // i4Y
				},
				// rD65
				{
					75,  // i4X
					-306  // i4Y
				},
				// rDF
				{
					12,  // i4X
					-325  // i4Y
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
					620,  // i4G
					1786  // i4B
				},
				// rA
				{
					527,  // i4R
					512,  // i4G
					1212  // i4B
				},
				// rTL84
				{
					622,  // i4R
					512,  // i4G
					996  // i4B
				},
				// rCWF
				{
					704,  // i4R
					512,  // i4G
					1039  // i4B
				},
				// rDNP
				{
					777,  // i4R
					512,  // i4G
					818  // i4B
				},
				// rD65
				{
					869,  // i4R
					512,  // i4G
					682  // i4B
				},
				// rDF
				{
					825,  // i4R
					512,  // i4G
					764  // i4B
				}
			},
			// rRotationMatrix
			{
				3,  // i4RotationAngle
				256,  // i4Cos
				13  // i4Sin
			},
			// rDaylightLocus
			{
				-141,  // i4SlopeNumerator
				128  // i4SlopeDenominator
			},
			// rPredictorGain
			{
				101,  // i4PrefRatio100
				// rSpatial_DaylightLocus_L
				{
					839,  // i4R
					530,  // i4G
					702  // i4B
				},
				// rSpatial_DaylightLocus_H
				{
					710,  // i4R
					512,  // i4G
					852  // i4B
				},
				// rTemporal_General
				{
					869,  // i4R
					512,  // i4G
					682  // i4B
				},
				// rAWBGain_LSC
				{
					740,  // i4R
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
					-244,  // i4RightBound
					-877,  // i4LeftBound
					-242,  // i4UpperBound
					-311  // i4LowerBound
				},
				// rWarmFluorescent
				{
					-244,  // i4RightBound
					-877,  // i4LeftBound
					-311,  // i4UpperBound
					-462  // i4LowerBound
				},
				// rFluorescent
				{
					-86,  // i4RightBound
					-244,  // i4LeftBound
					-246,  // i4UpperBound
					-341  // i4LowerBound
				},
				// rCWF
				{
					-86,  // i4RightBound
					-244,  // i4LeftBound
					-341,  // i4UpperBound
					-427  // i4LowerBound
				},
				// rDaylight
				{
					105,  // i4RightBound
					-86,  // i4LeftBound
					-246,  // i4UpperBound
					-341  // i4LowerBound
				},
				// rShade
				{
					435,  // i4RightBound
					105,  // i4LeftBound
					-246,  // i4UpperBound
					-332  // i4LowerBound
				},
				// rDaylightFluorescent
				{
					105,  // i4RightBound
					-86,  // i4LeftBound
					-341,  // i4UpperBound
					-427  // i4LowerBound
				}
			},
			// rPWBLightArea
			{
				// rReferenceArea
				{
					435,  // i4RightBound
					-877,  // i4LeftBound
					-217,  // i4UpperBound
					-462  // i4LowerBound
				},
				// rDaylight
				{
					130,  // i4RightBound
					-86,  // i4LeftBound
					-246,  // i4UpperBound
					-341  // i4LowerBound
				},
				// rCloudyDaylight
				{
					230,  // i4RightBound
					55,  // i4LeftBound
					-246,  // i4UpperBound
					-341  // i4LowerBound
				},
				// rShade
				{
					330,  // i4RightBound
					55,  // i4LeftBound
					-246,  // i4UpperBound
					-341  // i4LowerBound
				},
				// rTwilight
				{
					-86,  // i4RightBound
					-246,  // i4LeftBound
					-246,  // i4UpperBound
					-341  // i4LowerBound
				},
				// rFluorescent
				{
					125,  // i4RightBound
					-290,  // i4LeftBound
					-256,  // i4UpperBound
					-422  // i4LowerBound
				},
				// rWarmFluorescent
				{
					-225,  // i4RightBound
					-425,  // i4LeftBound
					-256,  // i4UpperBound
					-422  // i4LowerBound
				},
				// rIncandescent
				{
					-225,  // i4RightBound
					-425,  // i4LeftBound
					-246,  // i4UpperBound
					-341  // i4LowerBound
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
					799,  // i4R
					512,  // i4G
					723  // i4B
				},
				// rCloudyDaylight
				{
					932,  // i4R
					512,  // i4G
					609  // i4B
				},
				// rShade
				{
					994,  // i4R
					512,  // i4G
					568  // i4B
				},
				// rTwilight
				{
					628,  // i4R
					512,  // i4G
					944  // i4B
				},
				// rFluorescent
				{
					745,  // i4R
					512,  // i4G
					889  // i4B
				},
				// rWarmFluorescent
				{
					546,  // i4R
					512,  // i4G
					1254  // i4B
				},
				// rIncandescent
				{
					512,  // i4R
					512,  // i4G
					1183  // i4B
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
					4551  // i4OffsetThr
				},
				// rWarmFluorescent
				{
					40,  // i4SliderValue
					4551  // i4OffsetThr
				},
				// rShade
				{
					50,  // i4SliderValue
					909  // i4OffsetThr
				},
				// rSunsetArea
				{
					3,  // i4BoundXrThr
					-326  // i4BoundYrThr
				},
				// rShadeFArea
				{
					-244,  // i4BoundXrThr
					-313  // i4BoundYrThr
				},
				// rShadeFVerTex
				{
					-165,  // i4BoundXrThr
					-327  // i4BoundYrThr
				},
				// rShadeCWFArea
				{
					-244,  // i4BoundXrThr
					-376  // i4BoundYrThr
				},
				// rShadeCWFVerTex
				{
					-165,  // i4BoundXrThr
					-402  // i4BoundYrThr
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
					-552, -400, -265, -111, 0
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
					0, 350, 700, 1000, 1444, 1667, 1889, 2111, 2333, 2556,
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
					0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500,
					5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500,
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
			}
		}
	};

	return g_rAWBNVRAM;
};
