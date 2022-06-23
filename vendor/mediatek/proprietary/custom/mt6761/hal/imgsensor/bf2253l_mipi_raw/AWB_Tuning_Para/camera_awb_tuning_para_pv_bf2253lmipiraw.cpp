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

#include "camera_awb_tuning_para_bf2253lmipiraw.h"

#define NVRAM_TUNING_PARAM_NUM  6511001

template <>
AWB_NVRAM_T const&
getAWBNVRAM_bf2253lmipiraw<CAM_SCENARIO_PREVIEW>()
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
					827,  // i4R
					513,  // i4G
					512  // i4B
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
					-299,  // i4X
					-163  // i4Y
				},
				// rA
				{
					-195,  // i4X
					-179  // i4Y
				},
				// rTL84
				{
					-74,  // i4X
					-206  // i4Y
				},
				// rCWF
				{
					-46,  // i4X
					-224  // i4Y
				},
				// rDNP
				{
					36,  // i4X
					-212  // i4Y
				},
				// rD65
				{
					177,  // i4X
					-176  // i4Y
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
					-299,  // i4X
					-163  // i4Y
				},
				// rA
				{
					-195,  // i4X
					-179  // i4Y
				},
				// rTL84
				{
					-74,  // i4X
					-206  // i4Y
				},
				// rCWF
				{
					-46,  // i4X
					-224  // i4Y
				},
				// rDNP
				{
					36,  // i4X
					-212  // i4Y
				},
				// rD65
				{
					177,  // i4X
					-176  // i4Y
				},
				// rDF
				{
					162,  // i4X
					-218  // i4Y
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
					616,  // i4G
					1152  // i4B
				},
				// rA
				{
					512,  // i4R
					524,  // i4G
					869  // i4B
				},
				// rTL84
				{
					613,  // i4R
					512,  // i4G
					748  // i4B
				},
				// rCWF
				{
					651,  // i4R
					512,  // i4G
					737  // i4B
				},
				// rDNP
				{
					716,  // i4R
					512,  // i4G
					650  // i4B
				},
				// rD65
				{
					827,  // i4R
					513,  // i4G
					512  // i4B
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
				0,  // i4RotationAngle
				256,  // i4Cos
				0  // i4Sin
			},
			// rDaylightLocus
			{
				-129,  // i4SlopeNumerator
				128  // i4SlopeDenominator
			},
			// rPredictorGain
			{
				101,  // i4PrefRatio100
				// rSpatial_DaylightLocus_L
				{
					797,  // i4R
					530,  // i4G
					532  // i4B
				},
				// rSpatial_DaylightLocus_H
				{
					665,  // i4R
					512,  // i4G
					635  // i4B
				},
				// rTemporal_General
				{
					827,  // i4R
					513,  // i4G
					512  // i4B
				},
				// rAWBGain_LSC
				{
					664,  // i4R
					512,  // i4G
					636  // i4B
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
					-122,  // i4RightBound
					-699,  // i4LeftBound
					-60,  // i4UpperBound
					-150  // i4LowerBound
				},
				// rWarmFluorescent
				{
					-122,  // i4RightBound
					-699,  // i4LeftBound
					-150,  // i4UpperBound
					-314  // i4LowerBound
				},
				// rFluorescent
				{
					0,  // i4RightBound
					-122,  // i4LeftBound
					-116,  // i4UpperBound
					-215  // i4LowerBound
				},
				// rCWF
				{
					46,  // i4RightBound
					-122,  // i4LeftBound
					-215,  // i4UpperBound
					-279  // i4LowerBound
				},
				// rDaylight
				{
					207,  // i4RightBound
					0,  // i4LeftBound
					-116,  // i4UpperBound
					-215  // i4LowerBound
				},
				// rShade
				{
					537,  // i4RightBound
					207,  // i4LeftBound
					-116,  // i4UpperBound
					-195  // i4LowerBound
				},
				// rDaylightFluorescent
				{
					207,  // i4RightBound
					46,  // i4LeftBound
					-215,  // i4UpperBound
					-279  // i4LowerBound
				}
			},
			// rPWBLightArea
			{
				// rReferenceArea
				{
					537,  // i4RightBound
					-699,  // i4LeftBound
					0,  // i4UpperBound
					-314  // i4LowerBound
				},
				// rDaylight
				{
					232,  // i4RightBound
					0,  // i4LeftBound
					-116,  // i4UpperBound
					-215  // i4LowerBound
				},
				// rCloudyDaylight
				{
					332,  // i4RightBound
					157,  // i4LeftBound
					-116,  // i4UpperBound
					-215  // i4LowerBound
				},
				// rShade
				{
					432,  // i4RightBound
					157,  // i4LeftBound
					-116,  // i4UpperBound
					-215  // i4LowerBound
				},
				// rTwilight
				{
					0,  // i4RightBound
					-160,  // i4LeftBound
					-116,  // i4UpperBound
					-215  // i4LowerBound
				},
				// rFluorescent
				{
					227,  // i4RightBound
					-174,  // i4LeftBound
					-126,  // i4UpperBound
					-274  // i4LowerBound
				},
				// rWarmFluorescent
				{
					-95,  // i4RightBound
					-295,  // i4LeftBound
					-126,  // i4UpperBound
					-274  // i4LowerBound
				},
				// rIncandescent
				{
					-95,  // i4RightBound
					-295,  // i4LeftBound
					-116,  // i4UpperBound
					-215  // i4LowerBound
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
					750,  // i4R
					512,  // i4G
					547  // i4B
				},
				// rCloudyDaylight
				{
					892,  // i4R
					512,  // i4G
					460  // i4B
				},
				// rShade
				{
					954,  // i4R
					512,  // i4G
					430  // i4B
				},
				// rTwilight
				{
					575,  // i4R
					512,  // i4G
					714  // i4B
				},
				// rFluorescent
				{
					696,  // i4R
					512,  // i4G
					648  // i4B
				},
				// rWarmFluorescent
				{
					515,  // i4R
					512,  // i4G
					874  // i4B
				},
				// rIncandescent
				{
					492,  // i4R
					512,  // i4G
					834  // i4B
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
					4800  // i4OffsetThr
				},
				// rWarmFluorescent
				{
					40,  // i4SliderValue
					4800  // i4OffsetThr
				},
				// rShade
				{
					50,  // i4SliderValue
					409  // i4OffsetThr
				},
				// rSunsetArea
				{
					63,  // i4BoundXrThr
					-212  // i4BoundYrThr
				},
				// rShadeFArea
				{
					-122,  // i4BoundXrThr
					-210  // i4BoundYrThr
				},
				// rShadeFVerTex
				{
					-61,  // i4BoundXrThr
					-213  // i4BoundYrThr
				},
				// rShadeCWFArea
				{
					-122,  // i4BoundXrThr
					-240  // i4BoundYrThr
				},
				// rShadeCWFVerTex
				{
					-38,  // i4BoundXrThr
					-254  // i4BoundYrThr
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
					-476, -372, -251, -141, 0
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
						502,  // i4R
						512,  // i4G
						512  // i4B
					},
					{
						502,  // i4R
						512,  // i4G
						512  // i4B
					},
					{
						502,  // i4R
						512,  // i4G
						512  // i4B
					},
					{
						502,  // i4R
						512,  // i4G
						512  // i4B
					},
					{
						502,  // i4R
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
						502,  // i4R
						512,  // i4G
						512  // i4B
					},
					{
						502,  // i4R
						512,  // i4G
						512  // i4B
					},
					{
						502,  // i4R
						512,  // i4G
						512  // i4B
					},
					{
						502,  // i4R
						512,  // i4G
						512  // i4B
					},
					{
						502,  // i4R
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
						100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
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
