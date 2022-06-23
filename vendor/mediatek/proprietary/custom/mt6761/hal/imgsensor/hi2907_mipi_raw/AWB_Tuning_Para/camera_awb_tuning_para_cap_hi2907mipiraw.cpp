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

#include "camera_awb_tuning_para_hi2907mipiraw.h"

#define NVRAM_TUNING_PARAM_NUM  6511001

template <>
AWB_NVRAM_T const&
getAWBNVRAM_hi2907mipiraw<CAM_SCENARIO_CAPTURE>()
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
					687,  // i4R
					512,  // i4G
					622  // i4B
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
					-392,  // i4X
					-202  // i4Y
				},
				// rA
				{
					-296,  // i4X
					-209  // i4Y
				},
				// rTL84
				{
					-211,  // i4X
					-262  // i4Y
				},
				// rCWF
				{
					-151,  // i4X
					-304  // i4Y
				},
				// rDNP
				{
					-65,  // i4X
					-273  // i4Y
				},
				// rD65
				{
					37,  // i4X
					-180  // i4Y
				},
				// rDF
				{
					-99,  // i4X
					-259  // i4Y
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
					-412,  // i4X
					-160  // i4Y
				},
				// rA
				{
					-317,  // i4X
					-177  // i4Y
				},
				// rTL84
				{
					-238,  // i4X
					-239  // i4Y
				},
				// rCWF
				{
					-182,  // i4X
					-287  // i4Y
				},
				// rDNP
				{
					-94,  // i4X
					-265  // i4Y
				},
				// rD65
				{
					18,  // i4X
					-183  // i4Y
				},
				// rDF
				{
					-126,  // i4X
					-248  // i4Y
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
					662,  // i4G
					1481  // i4B
				},
				// rA
				{
					512,  // i4R
					576,  // i4G
					1142  // i4B
				},
				// rTL84
				{
					548,  // i4R
					512,  // i4G
					971  // i4B
				},
				// rCWF
				{
					630,  // i4R
					512,  // i4G
					947  // i4B
				},
				// rDNP
				{
					679,  // i4R
					512,  // i4G
					809  // i4B
				},
				// rD65
				{
					687,  // i4R
					512,  // i4G
					622  // i4B
				},
				// rDF
				{
					636,  // i4R
					512,  // i4G
					830  // i4B
				}
			},
			// rRotationMatrix
			{
				6,  // i4RotationAngle
				255,  // i4Cos
				27  // i4Sin
			},
			// rDaylightLocus
			{
				-156,  // i4SlopeNumerator
				128  // i4SlopeDenominator
			},
			// rPredictorGain
			{
				101,  // i4PrefRatio100
				// rSpatial_DaylightLocus_L
				{
					657,  // i4R
					530,  // i4G
					642  // i4B
				},
				// rSpatial_DaylightLocus_H
				{
					560,  // i4R
					512,  // i4G
					798  // i4B
				},
				// rTemporal_General
				{
					687,  // i4R
					512,  // i4G
					622  // i4B
				},
				// rAWBGain_LSC
				{
					589,  // i4R
					512,  // i4G
					751  // i4B
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
					-270,  // i4RightBound
					-812,  // i4LeftBound
					-105,  // i4UpperBound
					-208  // i4LowerBound
				},
				// rWarmFluorescent
				{
					-270,  // i4RightBound
					-812,  // i4LeftBound
					-208,  // i4UpperBound
					-377  // i4LowerBound
				},
				// rFluorescent
				{
					-141,  // i4RightBound
					-270,  // i4LeftBound
					-123,  // i4UpperBound
					-263  // i4LowerBound
				},
				// rCWF
				{
					-157,  // i4RightBound
					-270,  // i4LeftBound
					-263,  // i4UpperBound
					-342  // i4LowerBound
				},
				// rDaylight
				{
					48,  // i4RightBound
					-141,  // i4LeftBound
					-123,  // i4UpperBound
					-263  // i4LowerBound
				},
				// rShade
				{
					378,  // i4RightBound
					48,  // i4LeftBound
					-123,  // i4UpperBound
					-225  // i4LowerBound
				},
				// rDaylightFluorescent
				{
					48,  // i4RightBound
					-157,  // i4LeftBound
					-263,  // i4UpperBound
					-342  // i4LowerBound
				}
			},
			// rPWBLightArea
			{
				// rReferenceArea
				{
					378,  // i4RightBound
					-812,  // i4LeftBound
					-80,  // i4UpperBound
					-377  // i4LowerBound
				},
				// rDaylight
				{
					73,  // i4RightBound
					-141,  // i4LeftBound
					-123,  // i4UpperBound
					-263  // i4LowerBound
				},
				// rCloudyDaylight
				{
					173,  // i4RightBound
					-2,  // i4LeftBound
					-123,  // i4UpperBound
					-263  // i4LowerBound
				},
				// rShade
				{
					273,  // i4RightBound
					-2,  // i4LeftBound
					-123,  // i4UpperBound
					-263  // i4LowerBound
				},
				// rTwilight
				{
					-141,  // i4RightBound
					-301,  // i4LeftBound
					-123,  // i4UpperBound
					-263  // i4LowerBound
				},
				// rFluorescent
				{
					68,  // i4RightBound
					-338,  // i4LeftBound
					-133,  // i4UpperBound
					-337  // i4LowerBound
				},
				// rWarmFluorescent
				{
					-217,  // i4RightBound
					-417,  // i4LeftBound
					-133,  // i4UpperBound
					-337  // i4LowerBound
				},
				// rIncandescent
				{
					-217,  // i4RightBound
					-417,  // i4LeftBound
					-123,  // i4UpperBound
					-263  // i4LowerBound
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
					655,  // i4R
					512,  // i4G
					679  // i4B
				},
				// rCloudyDaylight
				{
					756,  // i4R
					512,  // i4G
					569  // i4B
				},
				// rShade
				{
					803,  // i4R
					512,  // i4G
					528  // i4B
				},
				// rTwilight
				{
					523,  // i4R
					512,  // i4G
					897  // i4B
				},
				// rFluorescent
				{
					617,  // i4R
					512,  // i4G
					830  // i4B
				},
				// rWarmFluorescent
				{
					496,  // i4R
					512,  // i4G
					1088  // i4B
				},
				// rIncandescent
				{
					466,  // i4R
					512,  // i4G
					1034  // i4B
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
					4147  // i4OffsetThr
				},
				// rWarmFluorescent
				{
					40,  // i4SliderValue
					4147  // i4OffsetThr
				},
				// rShade
				{
					50,  // i4SliderValue
					911  // i4OffsetThr
				},
				// rSunsetArea
				{
					-58,  // i4BoundXrThr
					-265  // i4BoundYrThr
				},
				// rShadeFArea
				{
					-270,  // i4BoundXrThr
					-243  // i4BoundYrThr
				},
				// rShadeFVerTex
				{
					-206,  // i4BoundXrThr
					-253  // i4BoundYrThr
				},
				// rShadeCWFArea
				{
					-270,  // i4BoundXrThr
					-291  // i4BoundYrThr
				},
				// rShadeCWFVerTex
				{
					-214,  // i4BoundXrThr
					-317  // i4BoundYrThr
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
					-430, -335, -256, -112, 0
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
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0
				},
				// i4LUTOut
				{
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0
				}
			},
			// rDaylightLocusOffsetLUTs_WF
			{
				21,  // i4LUTSize
				// i4LUTIn
				{
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0
				},
				// i4LUTOut
				{
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0
				}
			},
			// rDaylightLocusOffsetLUTs_Shade
			{
				21,  // i4LUTSize
				// i4LUTIn
				{
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0
				},
				// i4LUTOut
				{
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0
				}
			},
			// rLightPrefGain
			{
				{
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					}
				},
				{
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					}
				},
				{
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					}
				},
				{
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					}
				},
				{
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					}
				},
				{
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					}
				},
				{
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					}
				},
				{
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
					},
					{
						0,  // i4R
						0,  // i4G
						0  // i4B
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
