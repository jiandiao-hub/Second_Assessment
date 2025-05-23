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


STATIC_METADATA2_BEGIN(PLATFORM, PROJECT, SENSOR_DRVNAME_GC2375H_MIPI_RAW)
//------------------------------------------------------------------------------
//  android.request
//------------------------------------------------------------------------------
    //==========================================================================
    CONFIG_METADATA_BEGIN(MTK_REQUEST_AVAILABLE_CAPABILITIES)//new
        CONFIG_ENTRY_VALUE(MTK_REQUEST_AVAILABLE_CAPABILITIES_BACKWARD_COMPATIBLE, MUINT8)
        //CONFIG_ENTRY_VALUE(MTK_REQUEST_AVAILABLE_CAPABILITIES_MANUAL_SENSOR, MUINT8)
        //CONFIG_ENTRY_VALUE(MTK_REQUEST_AVAILABLE_CAPABILITIES_MANUAL_POST_PROCESSING, MUINT8)
        CONFIG_ENTRY_VALUE(MTK_REQUEST_AVAILABLE_CAPABILITIES_READ_SENSOR_SETTINGS, MUINT8)
        // CONFIG_ENTRY_VALUE(MTK_REQUEST_AVAILABLE_CAPABILITIES_RAW, MUINT8)
// For low memory project, set YUV up to 1920x1080,
// which equals to maximum VR stream size.
//This sensor max YUV is 1600x1200,so remove the limitation 		
//#if !MTKCAM_LOW_MEM
        CONFIG_ENTRY_VALUE(MTK_REQUEST_AVAILABLE_CAPABILITIES_BURST_CAPTURE, MUINT8)
//#endif
        // CONFIG_ENTRY_VALUE(MTK_REQUEST_AVAILABLE_CAPABILITIES_PRIVATE_REPROCESSING, MUINT8)
        // CONFIG_ENTRY_VALUE(MTK_REQUEST_AVAILABLE_CAPABILITIES_YUV_REPROCESSING, MUINT8)
        // CONFIG_ENTRY_VALUE(MTK_REQUEST_AVAILABLE_CAPABILITIES_CONSTRAINED_HIGH_SPEED_VIDEO, MUINT8)
    CONFIG_METADATA_END()
//------------------------------------------------------------------------------
//  android.scaler
//------------------------------------------------------------------------------
    //==========================================================================
    CONFIG_METADATA_BEGIN(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_WITH_DURATIONS)

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_BLOB, MINT64)
        CONFIG_ENTRY_VALUE(1600, MINT64)        // width
        CONFIG_ENTRY_VALUE(1200, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_BLOB, MINT64)
        CONFIG_ENTRY_VALUE(1600, MINT64)        // width
        CONFIG_ENTRY_VALUE(960, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_BLOB, MINT64)
        CONFIG_ENTRY_VALUE(1024, MINT64)        // width
        CONFIG_ENTRY_VALUE(768, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_BLOB, MINT64)
        CONFIG_ENTRY_VALUE(960, MINT64)        // width
        CONFIG_ENTRY_VALUE(720, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_BLOB, MINT64)
        CONFIG_ENTRY_VALUE(800, MINT64)        // width
        CONFIG_ENTRY_VALUE(480, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64)
        CONFIG_ENTRY_VALUE(1600, MINT64)        // width
        CONFIG_ENTRY_VALUE(1200, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(0, MINT64)    // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64)
        CONFIG_ENTRY_VALUE(1600, MINT64)        // width
        CONFIG_ENTRY_VALUE(960, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(0       , MINT64)    // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64)
        CONFIG_ENTRY_VALUE(1024, MINT64)        // width
        CONFIG_ENTRY_VALUE(768, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(0       , MINT64)    // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64)
        CONFIG_ENTRY_VALUE(960, MINT64)        // width
        CONFIG_ENTRY_VALUE(720, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(0       , MINT64)    // stall duration

        CONFIG_ENTRY_VALUE(HAL_PIXEL_FORMAT_YCbCr_420_888, MINT64)
        CONFIG_ENTRY_VALUE(800, MINT64)        // width
        CONFIG_ENTRY_VALUE(480, MINT64)        // height
        CONFIG_ENTRY_VALUE(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT, MINT64)   // output
        CONFIG_ENTRY_VALUE(33333333, MINT64)    // frame duration
        CONFIG_ENTRY_VALUE(0       , MINT64)    // stall duration
    CONFIG_METADATA_END()
    //==========================================================================

//------------------------------------------------------------------------------
//  android.reprocess
//------------------------------------------------------------------------------
    //==========================================================================
    /*CONFIG_METADATA_BEGIN(MTK_REPROCESS_MAX_CAPTURE_STALL)
        CONFIG_ENTRY_VALUE(4 , MINT32)
    CONFIG_METADATA_END()*/
    //==========================================================================
//------------------------------------------------------------------------------
//  android.jpeg
//------------------------------------------------------------------------------
    //==========================================================================
    CONFIG_METADATA_BEGIN(MTK_JPEG_MAX_SIZE)//
        CONFIG_ENTRY_VALUE(2304000, MINT32) //1600*1200*2*0.6
    CONFIG_METADATA_END()
    //==========================================================================
//------------------------------------------------------------------------------
STATIC_METADATA_END()


STATIC_METADATA2_BEGIN(PLATFORM, PRO_OVERWRITE, SENSOR_DRVNAME_GC2375H_MIPI_RAW)
// preserver for future use
STATIC_METADATA_END()
