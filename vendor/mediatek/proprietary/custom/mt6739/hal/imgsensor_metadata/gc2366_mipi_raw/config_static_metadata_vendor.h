/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2017. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */


STATIC_METADATA2_BEGIN(PLATFORM, VENDOR, SENSOR_DRVNAME_GC2366_MIPI_RAW)
//------------------------------------------------------------------------------
//  vendor cshot
//------------------------------------------------------------------------------
    //==========================================================================
    CONFIG_METADATA_BEGIN(MTK_CSHOT_FEATURE_AVAILABLE_MODES)
        CONFIG_ENTRY_VALUE(MTK_CSHOT_FEATURE_AVAILABLE_MODE_OFF, MINT32)
        CONFIG_ENTRY_VALUE(MTK_CSHOT_FEATURE_AVAILABLE_MODE_ON, MINT32)
    CONFIG_METADATA_END()
    //==========================================================================

//------------------------------------------------------------------------------
//  vendor p2done callback
//------------------------------------------------------------------------------
    //==========================================================================
    CONFIG_METADATA_BEGIN(MTK_CONTROL_CAPTURE_EARLY_NOTIFICATION_SUPPORT)
        CONFIG_ENTRY_VALUE(MTK_CONTROL_CAPTURE_EARLY_NOTIFICATION_SUPPORT_OFF, MINT32)
        CONFIG_ENTRY_VALUE(MTK_CONTROL_CAPTURE_EARLY_NOTIFICATION_SUPPORT_ON, MINT32)
    CONFIG_METADATA_END()
    //==========================================================================
//------------------------------------------------------------------------------
//  vendor: VHDR
//------------------------------------------------------------------------------
    //==========================================================================
#if (1 == MTKCAM_HAVE_VHDR_SUPPORT)
    CONFIG_METADATA_BEGIN(MTK_HDR_FEATURE_AVAILABLE_VHDR_MODES)
        CONFIG_ENTRY_VALUE(MTK_HDR_FEATURE_VHDR_MODE_OFF,  MINT32) // MUST Add this mode
    CONFIG_METADATA_END()
#endif

//------------------------------------------------------------------------------
//  vendor: bg service
//------------------------------------------------------------------------------
    //==========================================================================
    CONFIG_METADATA_BEGIN(MTK_BGSERVICE_FEATURE_PRERELEASE_AVAILABLE_MODES)
        CONFIG_ENTRY_VALUE(MTK_BGSERVICE_FEATURE_PRERELEASE_MODE_OFF, MINT32)
        //CONFIG_ENTRY_VALUE(MTK_BGSERVICE_FEATURE_PRERELEASE_MODE_ON, MINT32)
    CONFIG_METADATA_END()
    //==========================================================================

//------------------------------------------------------------------------------

STATIC_METADATA_END()

