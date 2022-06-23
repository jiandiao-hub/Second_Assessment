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
getAWBNVRAM_bf2253lmipiraw<CAM_SCENARIO_CUSTOM1>()
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
					0,  // i4R
					0,  // i4G
					0  // i4B
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
					0,  // i4X
					0  // i4Y
				},
				// rA
				{
					0,  // i4X
					0  // i4Y
				},
				// rTL84
				{
					0,  // i4X
					0  // i4Y
				},
				// rCWF
				{
					0,  // i4X
					0  // i4Y
				},
				// rDNP
				{
					0,  // i4X
					0  // i4Y
				},
				// rD65
				{
					0,  // i4X
					0  // i4Y
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
					0,  // i4X
					0  // i4Y
				},
				// rA
				{
					0,  // i4X
					0  // i4Y
				},
				// rTL84
				{
					0,  // i4X
					0  // i4Y
				},
				// rCWF
				{
					0,  // i4X
					0  // i4Y
				},
				// rDNP
				{
					0,  // i4X
					0  // i4Y
				},
				// rD65
				{
					0,  // i4X
					0  // i4Y
				},
				// rDF
				{
					0,  // i4X
					0  // i4Y
				}
			},
			// rLightAWBGain
			{
				// rStrobe
				{
					0,  // i4R
					0,  // i4G
					0  // i4B
				},
				// rHorizon
				{
					0,  // i4R
					0,  // i4G
					0  // i4B
				},
				// rA
				{
					0,  // i4R
					0,  // i4G
					0  // i4B
				},
				// rTL84
				{
					0,  // i4R
					0,  // i4G
					0  // i4B
				},
				// rCWF
				{
					0,  // i4R
					0,  // i4G
					0  // i4B
				},
				// rDNP
				{
					0,  // i4R
					0,  // i4G
					0  // i4B
				},
				// rD65
				{
					0,  // i4R
					0,  // i4G
					0  // i4B
				},
				// rDF
				{
					0,  // i4R
					0,  // i4G
					0  // i4B
				}
			},
			// rRotationMatrix
			{
				0,  // i4RotationAngle
				0,  // i4Cos
				0  // i4Sin
			},
			// rDaylightLocus
			{
				0,  // i4SlopeNumerator
				0  // i4SlopeDenominator
			},
			// rPredictorGain
			{
				0,  // i4PrefRatio100
				// rSpatial_DaylightLocus_L
				{
					0,  // i4R
					0,  // i4G
					0  // i4B
				},
				// rSpatial_DaylightLocus_H
				{
					0,  // i4R
					0,  // i4G
					0  // i4B
				},
				// rTemporal_General
				{
					0,  // i4R
					0,  // i4G
					0  // i4B
				},
				// rAWBGain_LSC
				{
					0,  // i4R
					0,  // i4G
					0  // i4B
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
					0,  // i4RightBound
					0,  // i4LeftBound
					0,  // i4UpperBound
					0  // i4LowerBound
				},
				// rWarmFluorescent
				{
					0,  // i4RightBound
					0,  // i4LeftBound
					0,  // i4UpperBound
					0  // i4LowerBound
				},
				// rFluorescent
				{
					0,  // i4RightBound
					0,  // i4LeftBound
					0,  // i4UpperBound
					0  // i4LowerBound
				},
				// rCWF
				{
					0,  // i4RightBound
					0,  // i4LeftBound
					0,  // i4UpperBound
					0  // i4LowerBound
				},
				// rDaylight
				{
					0,  // i4RightBound
					0,  // i4LeftBound
					0,  // i4UpperBound
					0  // i4LowerBound
				},
				// rShade
				{
					0,  // i4RightBound
					0,  // i4LeftBound
					0,  // i4UpperBound
					0  // i4LowerBound
				},
				// rDaylightFluorescent
				{
					0,  // i4RightBound
					0,  // i4LeftBound
					0,  // i4UpperBound
					0  // i4LowerBound
				}
			},
			// rPWBLightArea
			{
				// rReferenceArea
				{
					0,  // i4RightBound
					0,  // i4LeftBound
					0,  // i4UpperBound
					0  // i4LowerBound
				},
				// rDaylight
				{
					0,  // i4RightBound
					0,  // i4LeftBound
					0,  // i4UpperBound
					0  // i4LowerBound
				},
				// rCloudyDaylight
				{
					0,  // i4RightBound
					0,  // i4LeftBound
					0,  // i4UpperBound
					0  // i4LowerBound
				},
				// rShade
				{
					0,  // i4RightBound
					0,  // i4LeftBound
					0,  // i4UpperBound
					0  // i4LowerBound
				},
				// rTwilight
				{
					0,  // i4RightBound
					0,  // i4LeftBound
					0,  // i4UpperBound
					0  // i4LowerBound
				},
				// rFluorescent
				{
					0,  // i4RightBound
					0,  // i4LeftBound
					0,  // i4UpperBound
					0  // i4LowerBound
				},
				// rWarmFluorescent
				{
					0,  // i4RightBound
					0,  // i4LeftBound
					0,  // i4UpperBound
					0  // i4LowerBound
				},
				// rIncandescent
				{
					0,  // i4RightBound
					0,  // i4LeftBound
					0,  // i4UpperBound
					0  // i4LowerBound
				},
				// rGrayWorld
				{
					0,  // i4RightBound
					0,  // i4LeftBound
					0,  // i4UpperBound
					0  // i4LowerBound
				}
			},
			// rPWBDefaultGain
			{
				// rDaylight
				{
					0,  // i4R
					0,  // i4G
					0  // i4B
				},
				// rCloudyDaylight
				{
					0,  // i4R
					0,  // i4G
					0  // i4B
				},
				// rShade
				{
					0,  // i4R
					0,  // i4G
					0  // i4B
				},
				// rTwilight
				{
					0,  // i4R
					0,  // i4G
					0  // i4B
				},
				// rFluorescent
				{
					0,  // i4R
					0,  // i4G
					0  // i4B
				},
				// rWarmFluorescent
				{
					0,  // i4R
					0,  // i4G
					0  // i4B
				},
				// rIncandescent
				{
					0,  // i4R
					0,  // i4G
					0  // i4B
				},
				// rGrayWorld
				{
					0,  // i4R
					0,  // i4G
					0  // i4B
				}
			},
			// rPreferenceColor
			{
				// rTungsten
				{
					0,  // i4SliderValue
					0  // i4OffsetThr
				},
				// rWarmFluorescent
				{
					0,  // i4SliderValue
					0  // i4OffsetThr
				},
				// rShade
				{
					0,  // i4SliderValue
					0  // i4OffsetThr
				},
				// rSunsetArea
				{
					0,  // i4BoundXrThr
					0  // i4BoundYrThr
				},
				// rShadeFArea
				{
					0,  // i4BoundXrThr
					0  // i4BoundYrThr
				},
				// rShadeFVerTex
				{
					0,  // i4BoundXrThr
					0  // i4BoundYrThr
				},
				// rShadeCWFArea
				{
					0,  // i4BoundXrThr
					0  // i4BoundYrThr
				},
				// rShadeCWFVerTex
				{
					0,  // i4BoundXrThr
					0  // i4BoundYrThr
				}
			},
			// rCCTEstimation
			{
				// i4CCT
				{
					0, 0, 0, 0, 0
				},
				// i4RotatedXCoordinate
				{
					0, 0, 0, 0, 0
				}
			}
		},
		// rAlgoTuningParam
		{
			0,  // bAWBBackupEnable
			// rDaylightLocusOffsetLUTs_Tungsten
			{
				0,  // i4LUTSize
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
				0,  // i4LUTSize
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
				0,  // i4LUTSize
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
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0
				}
			},
			// rNeutralBlkThr
			{
				// m_i4NonNeutral
				{
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0
				},
				// m_i4F
				{
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0
				},
				// m_i4CWF
				{
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0
				},
				// m_i4Daylight
				{
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0
				},
				// m_i4DF
				{
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0
				}
			},
			// rFeaturePropThr
			{
				// rSunsetProp
				{
					0,  // i4Enable
					0,  // i4LVThr_L
					0,  // i4LVThr_H
					0,  // i4SunsetCountThr
					0,  // i4SunsetCountRatio_L
					0  // i4SunsetCountRatio_H
				},
				// rShadeFProp
				{
					0,  // i4Enable
					0,  // i4LVThr_L
					0,  // i4LVThr_H
					0  // i4DaylightProb
				},
				// rShadeCWFProp
				{
					0,  // i4Enable
					0,  // i4LVThr_L
					0,  // i4LVThr_H
					0  // i4DaylightProb
				}
			},
			// rNonNeutralProb
			{
				// i4LUT
				{
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0
				}
			},
			// rDaylightLocusProb
			{
				{
					// i4LUT
					{
						0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						0, 0, 0, 0, 0, 0, 0, 0, 0
					}
				},
				{
					// i4LUT
					{
						0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						0, 0, 0, 0, 0, 0, 0, 0, 0
					}
				},
				{
					// i4LUT
					{
						0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						0, 0, 0, 0, 0, 0, 0, 0, 0
					}
				},
				{
					// i4LUT
					{
						0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						0, 0, 0, 0, 0, 0, 0, 0, 0
					}
				},
				{
					// i4LUT
					{
						0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						0, 0, 0, 0, 0, 0, 0, 0, 0
					}
				},
				{
					// i4LUT
					{
						0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						0, 0, 0, 0, 0, 0, 0, 0, 0
					}
				},
				{
					// i4LUT
					{
						0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						0, 0, 0, 0, 0, 0, 0, 0, 0
					}
				},
				{
					// i4LUT
					{
						0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						0, 0, 0, 0, 0, 0, 0, 0, 0
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
