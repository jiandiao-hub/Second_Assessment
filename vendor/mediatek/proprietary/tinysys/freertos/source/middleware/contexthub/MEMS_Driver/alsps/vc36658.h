/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly
 * prohibited.
 */
/* MediaTek Inc. (C) 2018. All rights reserved.
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
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY
 * ACKNOWLEDGES THAT IT IS RECEIVER\'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY
 * THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK
 * SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO
 * RECEIVER\'S SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN
 * FORUM. RECEIVER\'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK\'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER
 * WILL BE, AT MEDIATEK\'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE
 * AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY
 * RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation
 * ("MediaTek Software") have been modified by MediaTek Inc. All revisions are
 * subject to any receiver\'s applicable license agreements with MediaTek Inc.
 */

#ifndef __VC36658_H__
#define __VC36658_H__
#include "cust_alsps.h"

/*VC36658 als/ps sensor register related macro*/
#define VC36658_REG_CS_CONF         0X00
#define VC36658_REG_CS_THDH         0X01
#define VC36658_REG_CS_THDL         0X02
#define VC36658_REG_PS_CONF1_2      0X03
#define VC36658_REG_PS_CONF3_MS     0X04
#define VC36658_REG_PS_THDL         0X05
#define VC36658_REG_PS_THDH         0X06
#define VC36658_REG_PS_CANC         0X07
#define VC36658_REG_PS_AC           0X08
#define VC36658_REG_CS_R_DATA       0XF0
#define VC36658_REG_CS_G_DATA       0XF1
#define VC36658_REG_CS_B_DATA       0XF2
#define VC36658_REG_CS_IR_DATA      0xF3
#define VC36658_REG_PS_DATA         0XF4
#define VC36658_REG_INT_FLAG        0XF5
#define VC36658_REG_ID              0XF6

#define CCTI_A_DIVIDE_POINT   2.85
#define ICF_CCT_DIVIDE_POINT  0
#define IR_RATIO              0.11

#define CCT_FA3    0
#define CCT_FA2    0
#define CCT_FA1    0
#define CCT_FA0    0
#define CCT_FB3   -4.6703
#define CCT_FB2    1.2263
#define CCT_FB1   -1107.5
#define CCT_FB0    6260.8
#define CCT_FC3    0
#define CCT_FC2   1710.7
#define CCT_FC1  -11130
#define CCT_FC0  21820

#define CCT_MAX 10000
#define CCT_MIN  2000

#define ICF_LUX_DIVIDE_POINT 0.5
#define LPB_C                0.71
#define LPB_A2               0.0104
#define LPB_A1              -0.1591
#define LPB_A0               0.7789
#define Lin_A                1
#define Lin_C                1

#define LUX_GAIN             1.0 //LUX_GAIN = IT_GAIN*CS_GAIN*CS_HS
#define CORRECTION_FACTOR    5.0

#endif
