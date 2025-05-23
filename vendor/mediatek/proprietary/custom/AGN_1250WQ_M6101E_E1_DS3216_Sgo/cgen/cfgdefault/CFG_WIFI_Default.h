/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

#ifndef _CFG_WIFI_D_H
#define _CFG_WIFI_D_H


#include "../cfgfileinc/CFG_Wifi_File.h"
#include "../inc/wifi_custom.h"
// the default value of Wi-Fi nvram file


WIFI_CFG_PARAM_STRUCT stWifiCfgDefault =
{
    0x0001, /* Part1OwnVersion | offset:0x0000 *//* Part1OwnVersion | offset:0x0001 */
    0x0008, /* Part1PeerVersion | offset:0x0002 *//* Part1PeerVersion | offset:0x0003 */
    0x00, /* MAC address 0 | offset:0x0004 */
    0x00, /* MAC address 1 | offset:0x0005 */
    0x00, /* MAC address 2 | offset:0x0006 */
    0x00, /* MAC address 3 | offset:0x0007 */
    0x00, /* MAC address 4 | offset:0x0008 */
    0x00, /* MAC address 5 | offset:0x0009 */
    0x00, /* Country code for regulatory domain | offset:0x000A */
    0x00, /* Country code for regulatory domain | offset:0x000B */
    0x00, /* OldTXPwer - cTxPwr2G4Cck | offset:0x000C */
    0x00, /* OldTxPwer - cTxPwr2G4Dsss | offset:0x000D */
    0x00, /* OldTxPwer - acReserved[0] | offset:0x000E */
    0x00, /* OldTxPwer - acReserved[1] | offset:0x000F */
    0x00, /* OldTXPwer - cTxPwr2G4OFDM_BPSK | offset:0x0010 */
    0x00, /* OldTXPwer - cTxPwr2G4OFDM_QPSK | offset:0x0011 */
    0x00, /* OldTXPwer - cTxPwr2G4OFDM_16QAM | offset:0x0012 */
    0x00, /* OldTXPwer - cTxPwr2G4OFDM_Reserved | offset:0x0013 */
    0x00, /* OldTXPwer - cTxPwr2G4OFDM_48Mbps | offset:0x0014 */
    0x00, /* OldTXPwer - cTxPwr2G4OFDM_54Mbps | offset:0x0015 */
    0x00, /* OldTXPwer - cTxPwr2G4HT20_BPSK | offset:0x0016 */
    0x00, /* OldTXPwer - cTxPwr2G4HT20_QPSK | offset:0x0017 */
    0x00, /* OldTXPwer - cTxPwr2G4HT20_16QAM | offset:0x0018 */
    0x00, /* OldTXPwer - cTxPwr2G4HT20_MCS5 | offset:0x0019 */
    0x00, /* OldTXPwer - cTxPwr2G4HT20_MCS6 | offset:0x001A */
    0x00, /* OldTXPwer - cTxPwr2G4HT20_MCS7 | offset:0x001B */
    0x00, /* OldTXPwer - cTxPwr2G4HT40_BPSK | offset:0x001C */
    0x00, /* OldTXPwer - cTxPwr2G4HT40_QPSK | offset:0x001D */
    0x00, /* OldTXPwer - cTxPwr2G4HT40_16QAM | offset:0x001E */
    0x00, /* OldTXPwer - cTxPwr2G4HT40_MCS5 | offset:0x001F */
    0x00, /* OldTXPwer - cTxPwr2G4HT40_MCS6 | offset:0x0020 */
    0x00, /* OldTXPwer - cTxPwr2G4HT40_MCS7 | offset:0x0021 */
    0x00, /* OldTXPwer - cTxPwr5GOFDM_BPSK | offset:0x0022 */
    0x00, /* OldTXPwer - cTxPwr5GOFDM_QPSK | offset:0x0023 */
    0x00, /* OldTXPwer - cTxPwr5GOFDM_16QAM | offset:0x0024 */
    0x00, /* OldTXPwer - cTxPwr5GOFDM_Reserved | offset:0x0025 */
    0x00, /* OldTXPwer - cTxPwr5GOFDM_48Mbps | offset:0x0026 */
    0x00, /* OldTXPwer - cTxPwr5GOFDM_54Mbps | offset:0x0027 */
    0x00, /* OldTXPwer - cTxPwr5GHT20_BPSK | offset:0x0028 */
    0x00, /* OldTXPwer - cTxPwr5GHT20_QPSK | offset:0x0029 */
    0x00, /* OldTXPwer - cTxPwr5GHT20_16QAM | offset:0x002A */
    0x00, /* OldTXPwer - cTxPwr5GHT20_MCS5 | offset:0x002B */
    0x00, /* OldTXPwer - cTxPwr5GHT20_MCS6 | offset:0x002C */
    0x00, /* OldTXPwer - cTxPwr5GHT20_MCS7 | offset:0x002D */
    0x00, /* OldTXPwer - cTxPwr5GHT40_BPSK | offset:0x002E */
    0x00, /* OldTXPwer - cTxPwr5GHT40_QPSK | offset:0x002F */
    0x00, /* OldTXPwer - cTxPwr5GHT40_16QAM | offset:0x0030 */
    0x00, /* OldTXPwer - cTxPwr5GHT40_MCS5 | offset:0x0031 */
    0x00, /* OldTXPwer - cTxPwr5GHT40_MCS6 | offset:0x0032 */
    0x00, /* OldTXPwer - cTxPwr5GHT40_MCS7 | offset:0x0033 */
    0x00, /* aEFUSE[0] | offset:0x0034 */
    0x00, /* aEFUSE[1] | offset:0x0035 */
    0x00, /* aEFUSE[2] | offset:0x0036 */
    0x00, /* aEFUSE[3] | offset:0x0037 */
    0x00, /* aEFUSE[4] | offset:0x0038 */
    0x00, /* aEFUSE[5] | offset:0x0039 */
    0x00, /* aEFUSE[6] | offset:0x003A */
    0x00, /* aEFUSE[7] | offset:0x003B */
    0x00, /* aEFUSE[8] | offset:0x003C */
    0x00, /* aEFUSE[9] | offset:0x003D */
    0x00, /* aEFUSE[10] | offset:0x003E */
    0x00, /* aEFUSE[11] | offset:0x003F */
    0x00, /* aEFUSE[12] | offset:0x0040 */
    0x00, /* aEFUSE[13] | offset:0x0041 */
    0x00, /* aEFUSE[14] | offset:0x0042 */
    0x00, /* aEFUSE[15] | offset:0x0043 */
    0x00, /* aEFUSE[16] | offset:0x0044 */
    0x00, /* aEFUSE[17] | offset:0x0045 */
    0x00, /* aEFUSE[18] | offset:0x0046 */
    0x00, /* aEFUSE[19] | offset:0x0047 */
    0x00, /* aEFUSE[20] | offset:0x0048 */
    0x00, /* aEFUSE[21] | offset:0x0049 */
    0x00, /* aEFUSE[22] | offset:0x004A */
    0x00, /* aEFUSE[23] | offset:0x004B */
    0x00, /* aEFUSE[24] | offset:0x004C */
    0x00, /* aEFUSE[25] | offset:0x004D */
    0x00, /* aEFUSE[26] | offset:0x004E */
    0x00, /* aEFUSE[27] | offset:0x004F */
    0x00, /* aEFUSE[28] | offset:0x0050 */
    0x00, /* aEFUSE[29] | offset:0x0051 */
    0x00, /* aEFUSE[30] | offset:0x0052 */
    0x00, /* aEFUSE[31] | offset:0x0053 */
    0x00, /* aEFUSE[32] | offset:0x0054 */
    0x00, /* aEFUSE[33] | offset:0x0055 */
    0x00, /* aEFUSE[34] | offset:0x0056 */
    0x00, /* aEFUSE[35] | offset:0x0057 */
    0x00, /* aEFUSE[36] | offset:0x0058 */
    0x00, /* aEFUSE[37] | offset:0x0059 */
    0x00, /* aEFUSE[38] | offset:0x005A */
    0x00, /* aEFUSE[39] | offset:0x005B */
    0x00, /* aEFUSE[40] | offset:0x005C */
    0x00, /* aEFUSE[41] | offset:0x005D */
    0x00, /* aEFUSE[42] | offset:0x005E */
    0x00, /* aEFUSE[43] | offset:0x005F */
    0x00, /* aEFUSE[44] | offset:0x0060 */
    0x00, /* aEFUSE[45] | offset:0x0061 */
    0x00, /* aEFUSE[46] | offset:0x0062 */
    0x00, /* aEFUSE[47] | offset:0x0063 */
    0x00, /* aEFUSE[48] | offset:0x0064 */
    0x00, /* aEFUSE[49] | offset:0x0065 */
    0x00, /* aEFUSE[50] | offset:0x0066 */
    0x00, /* aEFUSE[51] | offset:0x0067 */
    0x00, /* aEFUSE[52] | offset:0x0068 */
    0x00, /* aEFUSE[53] | offset:0x0069 */
    0x00, /* aEFUSE[54] | offset:0x006A */
    0x00, /* aEFUSE[55] | offset:0x006B */
    0x00, /* aEFUSE[56] | offset:0x006C */
    0x00, /* aEFUSE[57] | offset:0x006D */
    0x00, /* aEFUSE[58] | offset:0x006E */
    0x00, /* aEFUSE[59] | offset:0x006F */
    0x00, /* aEFUSE[60] | offset:0x0070 */
    0x00, /* aEFUSE[61] | offset:0x0071 */
    0x00, /* aEFUSE[62] | offset:0x0072 */
    0x00, /* aEFUSE[63] | offset:0x0073 */
    0x00, /* aEFUSE[64] | offset:0x0074 */
    0x00, /* aEFUSE[65] | offset:0x0075 */
    0x00, /* aEFUSE[66] | offset:0x0076 */
    0x00, /* aEFUSE[67] | offset:0x0077 */
    0x00, /* aEFUSE[68] | offset:0x0078 */
    0x00, /* aEFUSE[69] | offset:0x0079 */
    0x00, /* aEFUSE[70] | offset:0x007A */
    0x00, /* aEFUSE[71] | offset:0x007B */
    0x00, /* aEFUSE[72] | offset:0x007C */
    0x00, /* aEFUSE[73] | offset:0x007D */
    0x00, /* aEFUSE[74] | offset:0x007E */
    0x00, /* aEFUSE[75] | offset:0x007F */
    0x00, /* aEFUSE[76] | offset:0x0080 */
    0x00, /* aEFUSE[77] | offset:0x0081 */
    0x00, /* aEFUSE[78] | offset:0x0082 */
    0x00, /* aEFUSE[79] | offset:0x0083 */
    0x00, /* aEFUSE[80] | offset:0x0084 */
    0x00, /* aEFUSE[81] | offset:0x0085 */
    0x00, /* aEFUSE[82] | offset:0x0086 */
    0x00, /* aEFUSE[83] | offset:0x0087 */
    0x00, /* aEFUSE[84] | offset:0x0088 */
    0x00, /* aEFUSE[85] | offset:0x0089 */
    0x00, /* aEFUSE[86] | offset:0x008A */
    0x00, /* aEFUSE[87] | offset:0x008B */
    0x00, /* aEFUSE[88] | offset:0x008C */
    0x00, /* aEFUSE[89] | offset:0x008D */
    0x00, /* aEFUSE[90] | offset:0x008E */
    0x00, /* aEFUSE[91] | offset:0x008F */
    0x00, /* aEFUSE[92] | offset:0x0090 */
    0x00, /* aEFUSE[93] | offset:0x0091 */
    0x00, /* aEFUSE[94] | offset:0x0092 */
    0x00, /* aEFUSE[95] | offset:0x0093 */
    0x00, /* aEFUSE[96] | offset:0x0094 */
    0x00, /* aEFUSE[97] | offset:0x0095 */
    0x00, /* aEFUSE[98] | offset:0x0096 */
    0x00, /* aEFUSE[99] | offset:0x0097 */
    0x00, /* aEFUSE[100] | offset:0x0098 */
    0x00, /* aEFUSE[101] | offset:0x0099 */
    0x00, /* aEFUSE[102] | offset:0x009A */
    0x00, /* aEFUSE[103] | offset:0x009B */
    0x00, /* aEFUSE[104] | offset:0x009C */
    0x00, /* aEFUSE[105] | offset:0x009D */
    0x00, /* aEFUSE[106] | offset:0x009E */
    0x00, /* aEFUSE[107] | offset:0x009F */
    0x00, /* aEFUSE[108] | offset:0x00A0 */
    0x00, /* aEFUSE[109] | offset:0x00A1 */
    0x00, /* aEFUSE[110] | offset:0x00A2 */
    0x00, /* aEFUSE[111] | offset:0x00A3 */
    0x00, /* aEFUSE[112] | offset:0x00A4 */
    0x00, /* aEFUSE[113] | offset:0x00A5 */
    0x00, /* aEFUSE[114] | offset:0x00A6 */
    0x00, /* aEFUSE[115] | offset:0x00A7 */
    0x00, /* aEFUSE[116] | offset:0x00A8 */
    0x00, /* aEFUSE[117] | offset:0x00A9 */
    0x00, /* aEFUSE[118] | offset:0x00AA */
    0x00, /* aEFUSE[119] | offset:0x00AB */
    0x00, /* aEFUSE[120] | offset:0x00AC */
    0x00, /* aEFUSE[121] | offset:0x00AD */
    0x00, /* aEFUSE[122] | offset:0x00AE */
    0x00, /* aEFUSE[123] | offset:0x00AF */
    0x00, /* aEFUSE[124] | offset:0x00B0 */
    0x00, /* aEFUSE[125] | offset:0x00B1 */
    0x00, /* aEFUSE[126] | offset:0x00B2 */
    0x00, /* aEFUSE[127] | offset:0x00B3 */
    0x00, /* aEFUSE[128] | offset:0x00B4 */
    0x00, /* aEFUSE[129] | offset:0x00B5 */
    0x00, /* aEFUSE[130] | offset:0x00B6 */
    0x00, /* aEFUSE[131] | offset:0x00B7 */
    0x00, /* aEFUSE[132] | offset:0x00B8 */
    0x00, /* aEFUSE[133] | offset:0x00B9 */
    0x00, /* aEFUSE[134] | offset:0x00BA */
    0x00, /* aEFUSE[135] | offset:0x00BB */
    0x00, /* aEFUSE[136] | offset:0x00BC */
    0x00, /* aEFUSE[137] | offset:0x00BD */
    0x00, /* aEFUSE[138] | offset:0x00BE */
    0x00, /* aEFUSE[139] | offset:0x00BF */
    0x00, /* aEFUSE[140] | offset:0x00C0 */
    0x00, /* aEFUSE[141] | offset:0x00C1 */
    0x00, /* aEFUSE[142] | offset:0x00C2 */
    0x00, /* aEFUSE[143] | offset:0x00C3 */
    0x00, /* TxPwrValid | offset:0x00C4 */
    0x01, /* Support5GBand | offset:0x00C5 */
    0x00, /* fg2G4BandEdgePwrUsed | offset:0x00C6 */
    0x00, /* cBandEdgeMaxPwrCCK | offset:0x00C7 */
    0x00, /* cBandEdgeMaxPwrOFDM20 | offset:0x00C8 */
    0x00, /* cBandEdgeMaxPwrOFDM40 | offset:0x00C9 */
    0x00, /* RegChannelListMap | offset:0x00CA */
    0x00, /* RegChannelListIndex | offset:0x00CB */
    0x00, /* aRegSubbandInfo[0] | offset:0x00CC */
    0x00, /* aRegSubbandInfo[1] | offset:0x00CD */
    0x00, /* aRegSubbandInfo[2] | offset:0x00CE */
    0x00, /* aRegSubbandInfo[3] | offset:0x00CF */
    0x00, /* aRegSubbandInfo[4] | offset:0x00D0 */
    0x00, /* aRegSubbandInfo[5] | offset:0x00D1 */
    0x00, /* aRegSubbandInfo[6] | offset:0x00D2 */
    0x00, /* aRegSubbandInfo[7] | offset:0x00D3 */
    0x00, /* aRegSubbandInfo[8] | offset:0x00D4 */
    0x00, /* aRegSubbandInfo[9] | offset:0x00D5 */
    0x00, /* aRegSubbandInfo[10] | offset:0x00D6 */
    0x00, /* aRegSubbandInfo[11] | offset:0x00D7 */
    0x00, /* aRegSubbandInfo[12] | offset:0x00D8 */
    0x00, /* aRegSubbandInfo[13] | offset:0x00D9 */
    0x00, /* aRegSubbandInfo[14] | offset:0x00DA */
    0x00, /* aRegSubbandInfo[15] | offset:0x00DB */
    0x00, /* aRegSubbandInfo[16] | offset:0x00DC */
    0x00, /* aRegSubbandInfo[17] | offset:0x00DD */
    0x00, /* aRegSubbandInfo[18] | offset:0x00DE */
    0x00, /* aRegSubbandInfo[19] | offset:0x00DF */
    0x00, /* aRegSubbandInfo[20] | offset:0x00E0 */
    0x00, /* aRegSubbandInfo[21] | offset:0x00E1 */
    0x00, /* aRegSubbandInfo[22] | offset:0x00E2 */
    0x00, /* aRegSubbandInfo[23] | offset:0x00E3 */
    0x00, /* aRegSubbandInfo[24] | offset:0x00E4 */
    0x00, /* aRegSubbandInfo[25] | offset:0x00E5 */
    0x00, /* aRegSubbandInfo[26] | offset:0x00E6 */
    0x00, /* aRegSubbandInfo[27] | offset:0x00E7 */
    0x00, /* aRegSubbandInfo[28] | offset:0x00E8 */
    0x00, /* aRegSubbandInfo[29] | offset:0x00E9 */
    0x00, /* aRegSubbandInfo[30] | offset:0x00EA */
    0x00, /* aRegSubbandInfo[31] | offset:0x00EB */
    0x00, /* aRegSubbandInfo[32] | offset:0x00EC */
    0x00, /* aRegSubbandInfo[33] | offset:0x00ED */
    0x00, /* aRegSubbandInfo[34] | offset:0x00EE */
    0x00, /* aRegSubbandInfo[35] | offset:0x00EF */
    0x01, /* Enable5GBand | offset:0x00F0 */
    0x00, /* ucNeedCheckLDO | offset:0x00F1 */
    0x00, /* ucDefaultTestMode | offset:0x00F2 */
    0x00, /* aReserved0[0] | offset:0x00F3 */
    0x00, /* aReserved0[1] | offset:0x00F4 */
    0x00, /* aReserved0[2] | offset:0x00F5 */
    0x00, /* aReserved0[3] | offset:0x00F6 */
    0x00, /* aReserved0[4] | offset:0x00F7 */
    0x00, /* aReserved0[5] | offset:0x00F8 */
    0x00, /* aReserved0[6] | offset:0x00F9 */
    0x00, /* aReserved0[7] | offset:0x00FA */
    0x00, /* aReserved0[8] | offset:0x00FB */
    0x00, /* aReserved0[9] | offset:0x00FC */
    0x00, /* aReserved0[10] | offset:0x00FD */
    0x00, /* aReserved0[11] | offset:0x00FE */
    0x00, /* aReserved0[12] | offset:0x00FF */
    0x00, /* TYPE ID = 0x00 | offset:0x0100 */
    0x03, /* length = 0x3 | offset:0x0101 */
    0x01, /* Tx power Control | offset:0x0102 */
    0x02, /* TX power Total Size :671, 0x29F (Big Endian format) | offset:0x0103 */
    0x9f, /*  | offset:0x0104 */
    0x01, /* 2.4G Txpower, TYPE ID = 0x01 | offset:0x0105 */
    0x32, /* Length = 0x32 | offset:0x0106 */
    0x27, /* 2G4TxPwrCck1M | offset:0x0107 */
    0x27, /* 2G4TxPwrCck2M | offset:0x0108 */
    0x27, /* 2G4TxPwrCck5M | offset:0x0109 */
    0x27, /* 2G4TxPwrCck11M | offset:0x010A */
    0x23, /* 2G4TxPwrOfdm6M | offset:0x010B */
    0x23, /* 2G4TxPwrOfdm9M | offset:0x010C */
    0x23, /* 2G4TxPwrOfdm12M | offset:0x010D */
    0x23, /* 2G4TxPwrOfdm18M | offset:0x010E */
    0x23, /* 2G4TxPwrOfdm24M | offset:0x010F */
    0x23, /* 2G4TxPwrOfdm36M | offset:0x0110 */
    0x22, /* 2G4TxPwrOfdm48M | offset:0x0111 */
    0x22, /* 2G4TxPwrOfdm54M | offset:0x0112 */
    0x23, /* 2G4TxPwrHt20Mcs0 | offset:0x0113 */
    0x23, /* 2G4TxPwrHt20Mcs1 | offset:0x0114 */
    0x23, /* 2G4TxPwrHt20Mcs2 | offset:0x0115 */
    0x23, /* 2G4TxPwrHt20Mcs3 | offset:0x0116 */
    0x23, /* 2G4TxPwrHt20Mcs4 | offset:0x0117 */
    0x21, /* 2G4TxPwrHt20Mcs5 | offset:0x0118 */
    0x21, /* 2G4TxPwrHt20Mcs6 | offset:0x0119 */
    0x21, /* 2G4TxPwrHt20Mcs7 | offset:0x011A */
    0x21, /* 2G4TxPwrHt40Mcs0 | offset:0x011B */
    0x21, /* 2G4TxPwrHt40Mcs1 | offset:0x011C */
    0x21, /* 2G4TxPwrHt40Mcs2 | offset:0x011D */
    0x21, /* 2G4TxPwrHt40Mcs3 | offset:0x011E */
    0x21, /* 2G4TxPwrHt40Mcs4 | offset:0x011F */
    0x1E, /* 2G4TxPwrHt40Mcs5 | offset:0x0120 */
    0x1E, /* 2G4TxPwrHt40Mcs6 | offset:0x0121 */
    0x1E, /* 2G4TxPwrHt40Mcs7 | offset:0x0122 */
    0x23, /* 2G4TxPwrVht20Mcs0 | offset:0x0123 */
    0x23, /* 2G4TxPwrVht20Mcs1 | offset:0x0124 */
    0x23, /* 2G4TxPwrVht20Mcs2 | offset:0x0125 */
    0x23, /* 2G4TxPwrVht20Mcs3 | offset:0x0126 */
    0x23, /* 2G4TxPwrVht20Mcs4 | offset:0x0127 */
    0x21, /* 2G4TxPwrVht20Mcs5 | offset:0x0128 */
    0x21, /* 2G4TxPwrVht20Mcs6 | offset:0x0129 */
    0x21, /* 2G4TxPwrVht20Mcs7 | offset:0x012A */
    0x1E, /* 2G4TxPwrVht20Mcs8 | offset:0x012B */
    0x1E, /* 2G4TxPwrVht20Mcs9 | offset:0x012C */
    0x21, /* 2G4TxPwrVht40Mcs0 | offset:0x012D */
    0x21, /* 2G4TxPwrVht40Mcs1 | offset:0x012E */
    0x21, /* 2G4TxPwrVht40Mcs2 | offset:0x012F */
    0x21, /* 2G4TxPwrVht40Mcs3 | offset:0x0130 */
    0x21, /* 2G4TxPwrVht40Mcs4 | offset:0x0131 */
    0x1E, /* 2G4TxPwrVht40Mcs5 | offset:0x0132 */
    0x1E, /* 2G4TxPwrVht40Mcs6 | offset:0x0133 */
    0x1E, /* 2G4TxPwrVht40Mcs7 | offset:0x0134 */
    0x1E, /* 2G4TxPwrVht40Mcs8 | offset:0x0135 */
    0x1E, /* 2G4TxPwrVht40Mcs9 | offset:0x0136 */
    0x14, /* 2G4TxPwrHt40Mcs32 | offset:0x0137 */
    0x00, /* 2G4TxPwrLGBW40DuplicateMode | offset:0x0138 */
    0x02, /* 5G TxPower, TYPE ID = 0x02 | offset:0x0139 */
    0x46, /* Length = 0x46 | offset:0x013A */
    0x27, /* 5GTxPwrOfdm6M | offset:0x013B */
    0x27, /* 5GTxPwrOfdm9M | offset:0x013C */
    0x27, /* 5GTxPwrOfdm12M | offset:0x013D */
    0x26, /* 5GTxPwrOfdm18M | offset:0x013E */
    0x26, /* 5GTxPwrOfdm24M | offset:0x013F */
    0x26, /* 5GTxPwrOfdm36M | offset:0x0140 */
    0x25, /* 5GTxPwrOfdm48M | offset:0x0141 */
    0x25, /* 5GTxPwrOfdm54M | offset:0x0142 */
    0x27, /* 5GTxPwrHt20Mcs0 | offset:0x0143 */
    0x27, /* 5GTxPwrHt20Mcs1 | offset:0x0144 */
    0x27, /* 5GTxPwrHt20Mcs2 | offset:0x0145 */
    0x26, /* 5GTxPwrHt20Mcs3 | offset:0x0146 */
    0x26, /* 5GTxPwrHt20Mcs4 | offset:0x0147 */
    0x26, /* 5GTxPwrHt20Mcs5 | offset:0x0148 */
    0x25, /* 5GTxPwrHt20Mcs6 | offset:0x0149 */
    0x25, /* 5GTxPwrHt20Mcs7 | offset:0x014A */
    0x26, /* 5GTxPwrHt40Mcs0 | offset:0x014B */
    0x26, /* 5GTxPwrHt40Mcs1 | offset:0x014C */
    0x26, /* 5GTxPwrHt40Mcs2 | offset:0x014D */
    0x25, /* 5GTxPwrHt40Mcs3 | offset:0x014E */
    0x25, /* 5GTxPwrHt40Mcs4 | offset:0x014F */
    0x24, /* 5GTxPwrHt40Mcs5 | offset:0x0150 */
    0x24, /* 5GTxPwrHt40Mcs6 | offset:0x0151 */
    0x24, /* 5GTxPwrHt40Mcs7 | offset:0x0152 */
    0x27, /* 5GTxPwrVht20Mcs0 | offset:0x0153 */
    0x27, /* 5GTxPwrVht20Mcs1 | offset:0x0154 */
    0x27, /* 5GTxPwrVht20Mcs2 | offset:0x0155 */
    0x26, /* 5GTxPwrVht20Mcs3 | offset:0x0156 */
    0x26, /* 5GTxPwrVht20Mcs4 | offset:0x0157 */
    0x26, /* 5GTxPwrVht20Mcs5 | offset:0x0158 */
    0x25, /* 5GTxPwrVht20Mcs6 | offset:0x0159 */
    0x25, /* 5GTxPwrVht20Mcs7 | offset:0x015A */
    0x24, /* 5GTxPwrVht20Mcs8 | offset:0x015B */
    0x24, /* 5GTxPwrVht20Mcs9 | offset:0x015C */
    0x27, /* 5GTxPwrVht40Mcs0 | offset:0x015D */
    0x26, /* 5GTxPwrVht40Mcs1 | offset:0x015E */
    0x26, /* 5GTxPwrVht40Mcs2 | offset:0x015F */
    0x25, /* 5GTxPwrVht40Mcs3 | offset:0x0160 */
    0x25, /* 5GTxPwrVht40Mcs4 | offset:0x0161 */
    0x25, /* 5GTxPwrVht40Mcs5 | offset:0x0162 */
    0x24, /* 5GTxPwrVht40Mcs6 | offset:0x0163 */
    0x23, /* 5GTxPwrVht40Mcs7 | offset:0x0164 */
    0x23, /* 5GTxPwrVht40Mcs8 | offset:0x0165 */
    0x22, /* 5GTxPwrVht40Mcs9 | offset:0x0166 */
    0x26, /* 5GTxPwrVht80Mcs0 | offset:0x0167 */
    0x26, /* 5GTxPwrVht80Mcs1 | offset:0x0168 */
    0x25, /* 5GTxPwrVht80Mcs2 | offset:0x0169 */
    0x25, /* 5GTxPwrVht80Mcs3 | offset:0x016A */
    0x24, /* 5GTxPwrVht80Mcs4 | offset:0x016B */
    0x24, /* 5GTxPwrVht80Mcs5 | offset:0x016C */
    0x23, /* 5GTxPwrVht80Mcs6 | offset:0x016D */
    0x22, /* 5GTxPwrVht80Mcs7 | offset:0x016E */
    0x21, /* 5GTxPwrVht80Mcs8 | offset:0x016F */
    0x21, /* 5GTxPwrVht80Mcs9 | offset:0x0170 */
    0x20, /* 5GTxPwrVht160Mcs0 | offset:0x0171 */
    0x20, /* 5GTxPwrVht160Mcs1 | offset:0x0172 */
    0x18, /* 5GTxPwrVht160Mcs2 | offset:0x0173 */
    0x18, /* 5GTxPwrVht160Mcs3 | offset:0x0174 */
    0x16, /* 5GTxPwrVht160Mcs4 | offset:0x0175 */
    0x16, /* 5GTxPwrVht160Mcs5 | offset:0x0176 */
    0x15, /* 5GTxPwrVht160Mcs6 | offset:0x0177 */
    0x15, /* 5GTxPwrVht160Mcs7 | offset:0x0178 */
    0x14, /* 5GTxPwrVht160Mcs8 | offset:0x0179 */
    0x14, /* 5GTxPwrVht160Mcs9 | offset:0x017A */
    0x14, /* 5GTxPwrHt40Mcs32 | offset:0x017B */
    0x00, /* 5GTxPwrLGBW40DuplicateMode | offset:0x017C */
    0x00, /* 5GTxPwrLGBW80DuplicateMode | offset:0x017D */
    0x00, /* 5GTxPwrLGBW1600DuplicateMode | offset:0x017E */
    0x00, /* 5GBw5MTxPwrDelta | offset:0x017F */
    0x00, /* 5GBw10MTxPwrDelta | offset:0x0180 */
    0x03, /* 2.4G WF0 path, Type ID =0x03 | offset:0x0181 */
    0x89, /* Length 0x89 | offset:0x0182 */
    0x00, /* 2G4_FE_Loss_TX_CH1 | offset:0x0183 */
    0x00, /* 2G4_FE_Loss_RX_CH1 | offset:0x0184 */
    0x00, /* 2G4_FE_Loss_TX_CH2 | offset:0x0185 */
    0x00, /* 2G4_FE_Loss_RX_CH2 | offset:0x0186 */
    0x00, /* 2G4_FE_Loss_TX_CH3 | offset:0x0187 */
    0x00, /* 2G4_FE_Loss_RX_CH3 | offset:0x0188 */
    0x00, /* 2G4_FE_Loss_TX_CH4 | offset:0x0189 */
    0x00, /* 2G4_FE_Loss_RX_CH4 | offset:0x018A */
    0x00, /* 2G4_FE_Loss_TX_CH5 | offset:0x018B */
    0x00, /* 2G4_FE_Loss_RX_CH5 | offset:0x018C */
    0x00, /* 2G4_FE_Loss_TX_CH6 | offset:0x018D */
    0x00, /* 2G4_FE_Loss_RX_CH6 | offset:0x018E */
    0x00, /* 2G4_FE_Loss_TX_CH7 | offset:0x018F */
    0x00, /* 2G4_FE_Loss_RX_CH7 | offset:0x0190 */
    0x00, /* 2G4_FE_Loss_TX_CH8 | offset:0x0191 */
    0x00, /* 2G4_FE_Loss_RX_CH8 | offset:0x0192 */
    0x00, /* 2G4_FE_Loss_TX_CH9 | offset:0x0193 */
    0x00, /* 2G4_FE_Loss_RX_CH9 | offset:0x0194 */
    0x00, /* 2G4_FE_Loss_TX_CH10 | offset:0x0195 */
    0x00, /* 2G4_FE_Loss_RX_CH10 | offset:0x0196 */
    0x00, /* 2G4_FE_Loss_TX_CH11 | offset:0x0197 */
    0x00, /* 2G4_FE_Loss_RX_CH11 | offset:0x0198 */
    0x00, /* 2G4_FE_Loss_TX_CH12 | offset:0x0199 */
    0x00, /* 2G4_FE_Loss_RX_CH12 | offset:0x019A */
    0x00, /* 2G4_FE_Loss_TX_CH13 | offset:0x019B */
    0x00, /* 2G4_FE_Loss_RX_CH13 | offset:0x019C */
    0x00, /* 2G4_FE_Loss_TX_CH14 | offset:0x019D */
    0x00, /* 2G4_FE_Loss_RX_CH14 | offset:0x019E */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiSlope | offset:0x019F */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetLSB | offset:0x01A0 */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetMSB | offset:0x01A1 */
    0x00, /* r2G4WF0patch.rTssi[0].TxTargetPower | offset:0x01A2 */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiSlope | offset:0x01A3 */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetLSB | offset:0x01A4 */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetMSB | offset:0x01A5 */
    0x00, /* r2G4WF0patch.rTssi[0].TxTargetPower | offset:0x01A6 */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiSlope | offset:0x01A7 */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetLSB | offset:0x01A8 */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetMSB | offset:0x01A9 */
    0x00, /* r2G4WF0patch.rTssi[0].TxTargetPower | offset:0x01AA */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiSlope | offset:0x01AB */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetLSB | offset:0x01AC */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetMSB | offset:0x01AD */
    0x00, /* r2G4WF0patch.rTssi[0].TxTargetPower | offset:0x01AE */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiSlope | offset:0x01AF */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetLSB | offset:0x01B0 */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetMSB | offset:0x01B1 */
    0x00, /* r2G4WF0patch.rTssi[0].TxTargetPower | offset:0x01B2 */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiSlope | offset:0x01B3 */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetLSB | offset:0x01B4 */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetMSB | offset:0x01B5 */
    0x00, /* r2G4WF0patch.rTssi[0].TxTargetPower | offset:0x01B6 */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiSlope | offset:0x01B7 */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetLSB | offset:0x01B8 */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetMSB | offset:0x01B9 */
    0x00, /* r2G4WF0patch.rTssi[0].TxTargetPower | offset:0x01BA */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiSlope | offset:0x01BB */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetLSB | offset:0x01BC */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetMSB | offset:0x01BD */
    0x00, /* r2G4WF0patch.rTssi[0].TxTargetPower | offset:0x01BE */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiSlope | offset:0x01BF */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetLSB | offset:0x01C0 */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetMSB | offset:0x01C1 */
    0x00, /* r2G4WF0patch.rTssi[0].TxTargetPower | offset:0x01C2 */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiSlope | offset:0x01C3 */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetLSB | offset:0x01C4 */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetMSB | offset:0x01C5 */
    0x00, /* r2G4WF0patch.rTssi[0].TxTargetPower | offset:0x01C6 */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiSlope | offset:0x01C7 */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetLSB | offset:0x01C8 */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetMSB | offset:0x01C9 */
    0x00, /* r2G4WF0patch.rTssi[0].TxTargetPower | offset:0x01CA */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiSlope | offset:0x01CB */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetLSB | offset:0x01CC */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetMSB | offset:0x01CD */
    0x00, /* r2G4WF0patch.rTssi[0].TxTargetPower | offset:0x01CE */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiSlope | offset:0x01CF */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetLSB | offset:0x01D0 */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetMSB | offset:0x01D1 */
    0x00, /* r2G4WF0patch.rTssi[0].TxTargetPower | offset:0x01D2 */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiSlope | offset:0x01D3 */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetLSB | offset:0x01D4 */
    0x00, /* r2G4WF0patch.rTssi[0].PATssiOffsetMSB | offset:0x01D5 */
    0x00, /* r2G4WF0patch.rTssi[0].TxTargetPower | offset:0x01D6 */
    0x00, /* 2G4Tx0TssiOffTxpower | offset:0x01D7 */
    0x00, /* 2G4_TSSI_Channel_Offset_CH1 | offset:0x01D8 */
    0x00, /* 2G4_TSSI_Channel_Offset_CH2 | offset:0x01D9 */
    0x00, /* 2G4_TSSI_Channel_Offset_CH3 | offset:0x01DA */
    0x00, /* 2G4_TSSI_Channel_Offset_CH4 | offset:0x01DB */
    0x00, /* 2G4_TSSI_Channel_Offset_CH5 | offset:0x01DC */
    0x00, /* 2G4_TSSI_Channel_Offset_CH6 | offset:0x01DD */
    0x00, /* 2G4_TSSI_Channel_Offset_CH7 | offset:0x01DE */
    0x00, /* 2G4_TSSI_Channel_Offset_CH8 | offset:0x01DF */
    0x00, /* 2G4_TSSI_Channel_Offset_CH9 | offset:0x01E0 */
    0x00, /* 2G4_TSSI_Channel_Offset_CH10 | offset:0x01E1 */
    0x00, /* 2G4_TSSI_Channel_Offset_CH11 | offset:0x01E2 */
    0x00, /* 2G4_TSSI_Channel_Offset_CH12 | offset:0x01E3 */
    0x00, /* 2G4_TSSI_Channel_Offset_CH13 | offset:0x01E4 */
    0x00, /* 2G4_TSSI_Channel_Offset_CH14 | offset:0x01E5 */
    0x00, /* aTx2G4DpdG0OffsetrateH_L[0] | offset:0x01E6 */
    0x00, /* aTx2G4DpdG0OffsetrateH_L[1] | offset:0x01E7 */
    0x00, /* aTx2G4DpdG0OffsetrateH_L[2] | offset:0x01E8 */
    0x00, /* aTx2G4DpdG0OffsetrateH_L[3] | offset:0x01E9 */
    0x00, /* aTx2G4DpdG0OffsetrateH_L[4] | offset:0x01EA */
    0x00, /* aTx2G4DpdG0OffsetrateH_L[5] | offset:0x01EB */
    0x00, /* 2.4G  G0 Delta  Channel 1 | offset:0x01EC */
    0x00, /* 2.4G  G0 Delta  Channel 2 | offset:0x01ED */
    0x00, /* 2.4G  G0 Delta  Channel 3 | offset:0x01EE */
    0x00, /* 2.4G  G0 Delta  Channel 4 | offset:0x01EF */
    0x00, /* 2.4G  G0 Delta  Channel 5 | offset:0x01F0 */
    0x00, /* 2.4G  G0 Delta  Channel 6 | offset:0x01F1 */
    0x00, /* 2.4G  G0 Delta  Channel 7 | offset:0x01F2 */
    0x00, /* 2.4G  G0 Delta  Channel 8 | offset:0x01F3 */
    0x00, /* 2.4G  G0 Delta  Channel 9 | offset:0x01F4 */
    0x00, /* 2.4G  G0 Delta  Channel 10 | offset:0x01F5 */
    0x00, /* 2.4G  G0 Delta  Channel 11 | offset:0x01F6 */
    0x00, /* 2.4G  G0 Delta  Channel 12 | offset:0x01F7 */
    0x00, /* 2.4G  G0 Delta  Channel 13 | offset:0x01F8 */
    0x00, /* 2.4G  G0 Delta  Channel 14 | offset:0x01F9 */
    0x00, /* a2G4Wf0Lna_Gain_CalDiff[0] | offset:0x01FA */
    0x00, /* a2G4Wf0Lna_Gain_CalDiff[1] | offset:0x01FB */
    0x00, /* 2G4_TX_CVT_Offset_CH1 | offset:0x01FC */
    0x00, /* 2G4_TX_CVT_Offset_CH2 | offset:0x01FD */
    0x00, /* 2G4_TX_CVT_Offset_CH3 | offset:0x01FE */
    0x00, /* 2G4_TX_CVT_Offset_CH4 | offset:0x01FF */
    0x00, /* 2G4_TX_CVT_Offset_CH5 | offset:0x0200 */
    0x00, /* 2G4_TX_CVT_Offset_CH6 | offset:0x0201 */
    0x00, /* 2G4_TX_CVT_Offset_CH7 | offset:0x0202 */
    0x00, /* 2G4_TX_CVT_Offset_CH8 | offset:0x0203 */
    0x00, /* 2G4_TX_CVT_Offset_CH9 | offset:0x0204 */
    0x00, /* 2G4_TX_CVT_Offset_CH10 | offset:0x0205 */
    0x00, /* 2G4_TX_CVT_Offset_CH11 | offset:0x0206 */
    0x00, /* 2G4_TX_CVT_Offset_CH12 | offset:0x0207 */
    0x00, /* 2G4_TX_CVT_Offset_CH13 | offset:0x0208 */
    0x00, /* 2G4_TX_CVT_Offset_CH14 | offset:0x0209 */
    0x00, /* reserved | offset:0x020A */
    0x00, /* reserved | offset:0x020B */
    0x04, /* 5G WF0 path, TYPE ID =0x04 | offset:0x020C */
    0x9d, /* Length =0x9D | offset:0x020D */
    0x00, /* 5G_FE_Loss_TX_Group1 | offset:0x020E */
    0x00, /* 5G_FE_Loss_RX_Group1 | offset:0x020F */
    0x00, /* 5G_FE_Loss_TX_Group2 | offset:0x0210 */
    0x00, /* 5G_FE_Loss_RX_Group2 | offset:0x0211 */
    0x00, /* 5G_FE_Loss_TX_Group3 | offset:0x0212 */
    0x00, /* 5G_FE_Loss_RX_Group3 | offset:0x0213 */
    0x00, /* 5G_FE_Loss_TX_Group4 | offset:0x0214 */
    0x00, /* 5G_FE_Loss_RX_Group4 | offset:0x0215 */
    0x00, /* 5G_FE_Loss_TX_Group5 | offset:0x0216 */
    0x00, /* 5G_FE_Loss_RX_Group5 | offset:0x0217 */
    0x00, /* 5G_FE_Loss_TX_Group6 | offset:0x0218 */
    0x00, /* 5G_FE_Loss_RX_Group6 | offset:0x0219 */
    0x00, /* 5G_FE_Loss_TX_Group7 | offset:0x021A */
    0x00, /* 5G_FE_Loss_RX_Group7 | offset:0x021B */
    0x00, /* 5G_FE_Loss_TX_Group8 | offset:0x021C */
    0x00, /* 5G_FE_Loss_RX_Group8 | offset:0x021D */
    0x00, /* r5GWF0patch.rTssi[0].PATssiSlope | offset:0x021E */
    0x00, /* r5GWF0patch.rTssi[0].PATssiOffsetLSB | offset:0x021F */
    0x00, /* r5GWF0patch.rTssi[0].PATssiOffsetMSB | offset:0x0220 */
    0x00, /* r5GWF0patch.rTssi[0].TxTargetPower | offset:0x0221 */
    0x00, /* r5GWF0patch.rTssi[1].PATssiSlope | offset:0x0222 */
    0x00, /* r5GWF0patch.rTssi[1].PATssiOffsetLSB | offset:0x0223 */
    0x00, /* r5GWF0patch.rTssi[1].PATssiOffsetMSB | offset:0x0224 */
    0x00, /* r5GWF0patch.rTssi[1].TxTargetPower | offset:0x0225 */
    0x00, /* r5GWF0patch.rTssi[2].PATssiSlope | offset:0x0226 */
    0x00, /* r5GWF0patch.rTssi[2].PATssiOffsetLSB | offset:0x0227 */
    0x00, /* r5GWF0patch.rTssi[2].PATssiOffsetMSB | offset:0x0228 */
    0x00, /* r5GWF0patch.rTssi[2].TxTargetPower | offset:0x0229 */
    0x00, /* r5GWF0patch.rTssi[3].PATssiSlope | offset:0x022A */
    0x00, /* r5GWF0patch.rTssi[3].PATssiOffsetLSB | offset:0x022B */
    0x00, /* r5GWF0patch.rTssi[3].PATssiOffsetMSB | offset:0x022C */
    0x00, /* r5GWF0patch.rTssi[3].TxTargetPower | offset:0x022D */
    0x00, /* r5GWF0patch.rTssi[4].PATssiSlope | offset:0x022E */
    0x00, /* r5GWF0patch.rTssi[4].PATssiOffsetLSB | offset:0x022F */
    0x00, /* r5GWF0patch.rTssi[4].PATssiOffsetMSB | offset:0x0230 */
    0x00, /* r5GWF0patch.rTssi[4].TxTargetPower | offset:0x0231 */
    0x00, /* r5GWF0patch.rTssi[5].PATssiSlope | offset:0x0232 */
    0x00, /* r5GWF0patch.rTssi[5].PATssiOffsetLSB | offset:0x0233 */
    0x00, /* r5GWF0patch.rTssi[5].PATssiOffsetMSB | offset:0x0234 */
    0x00, /* r5GWF0patch.rTssi[5].TxTargetPower | offset:0x0235 */
    0x00, /* r5GWF0patch.rTssi[6].PATssiSlope | offset:0x0236 */
    0x00, /* r5GWF0patch.rTssi[6].PATssiOffsetLSB | offset:0x0237 */
    0x00, /* r5GWF0patch.rTssi[6].PATssiOffsetMSB | offset:0x0238 */
    0x00, /* r5GWF0patch.rTssi[7].TxTargetPower | offset:0x0239 */
    0x00, /* r5GWF0patch.rTssi[7].PATssiSlope | offset:0x023A */
    0x00, /* r5GWF0patch.rTssi[7].PATssiOffsetLSB | offset:0x023B */
    0x00, /* r5GWF0patch.rTssi[7].PATssiOffsetMSB | offset:0x023C */
    0x00, /* r5GWF0patch.rTssi[7].TxTargetPower | offset:0x023D */
    0x00, /* 5GTx0TssiOffTxPower | offset:0x023E */
    0x00, /* 5G_TSSI_Channel_Offset_Group1_Low | offset:0x023F */
    0x00, /* 5G_TSSI_Channel_Offset_Group1_High | offset:0x0240 */
    0x00, /* 5G_TSSI_Channel_Offset_Group2_Low | offset:0x0241 */
    0x00, /* 5G_TSSI_Channel_Offset_Group2_High | offset:0x0242 */
    0x00, /* 5G_TSSI_Channel_Offset_Group3_Low | offset:0x0243 */
    0x00, /* 5G_TSSI_Channel_Offset_Group3_High | offset:0x0244 */
    0x00, /* 5G_TSSI_Channel_Offset_Group4_Low | offset:0x0245 */
    0x00, /* 5G_TSSI_Channel_Offset_Group4_High | offset:0x0246 */
    0x00, /* 5G_TSSI_Channel_Offset_Group5_Low | offset:0x0247 */
    0x00, /* 5G_TSSI_Channel_Offset_Group5_High | offset:0x0248 */
    0x00, /* 5G_TSSI_Channel_Offset_Group6_Low | offset:0x0249 */
    0x00, /* 5G_TSSI_Channel_Offset_Group6_High | offset:0x024A */
    0x00, /* 5G_TSSI_Channel_Offset_Group7_Low | offset:0x024B */
    0x00, /* 5G_TSSI_Channel_Offset_Group7_High | offset:0x024C */
    0x00, /* 5G_TSSI_Channel_Offset_Group8_Low | offset:0x024D */
    0x00, /* 5G_TSSI_Channel_Offset_Group8_High | offset:0x024E */
    0x00, /* aTx5GDpdG0OffsetrateH_L[0] | offset:0x024F */
    0x00, /* aTx5GDpdG0OffsetrateH_L[1] | offset:0x0250 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[2] | offset:0x0251 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[3] | offset:0x0252 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[4] | offset:0x0253 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[5] | offset:0x0254 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[6] | offset:0x0255 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[7] | offset:0x0256 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[8] | offset:0x0257 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[9] | offset:0x0258 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[10] | offset:0x0259 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[11] | offset:0x025A */
    0x00, /* aTx5GDpdG0OffsetrateH_L[12] | offset:0x025B */
    0x00, /* aTx5GDpdG0OffsetrateH_L[13] | offset:0x025C */
    0x00, /* aTx5GDpdG0OffsetrateH_L[14] | offset:0x025D */
    0x00, /* aTx5GDpdG0OffsetrateH_L[15] | offset:0x025E */
    0x00, /* aTx5GDpdG0OffsetrateH_L[16] | offset:0x025F */
    0x00, /* aTx5GDpdG0OffsetrateH_L[17] | offset:0x0260 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[18] | offset:0x0261 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[19] | offset:0x0262 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[20] | offset:0x0263 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[21] | offset:0x0264 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[22] | offset:0x0265 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[23] | offset:0x0266 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[24] | offset:0x0267 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[25] | offset:0x0268 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[26] | offset:0x0269 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[27] | offset:0x026A */
    0x00, /* aTx5GDpdG0OffsetrateH_L[28] | offset:0x026B */
    0x00, /* aTx5GDpdG0OffsetrateH_L[29] | offset:0x026C */
    0x00, /* aTx5GDpdG0OffsetrateH_L[30] | offset:0x026D */
    0x00, /* aTx5GDpdG0OffsetrateH_L[31] | offset:0x026E */
    0x00, /* aTx5GDpdG0OffsetrateH_L[32] | offset:0x026F */
    0x00, /* aTx5GDpdG0OffsetrateH_L[33] | offset:0x0270 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[34] | offset:0x0271 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[35] | offset:0x0272 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[36] | offset:0x0273 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[37] | offset:0x0274 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[38] | offset:0x0275 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[39] | offset:0x0276 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[40] | offset:0x0277 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[41] | offset:0x0278 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[42] | offset:0x0279 */
    0x00, /* aTx5GDpdG0OffsetrateH_L[43] | offset:0x027A */
    0x00, /* aTx5GDpdG0OffsetrateH_L[44] | offset:0x027B */
    0x00, /* aTx5GDpdG0OffsetrateH_L[45] | offset:0x027C */
    0x00, /* aTx5GDpdG0OffsetrateH_L[46] | offset:0x027D */
    0x00, /* aTx5GDpdG0OffsetrateH_L[47] | offset:0x027E */
    0x00, /* 5G  G0 Delta  Group 1 | offset:0x027F */
    0x00, /* 5G  G0 Delta  Group 2 | offset:0x0280 */
    0x00, /* 5G  G0 Delta  Group 3 | offset:0x0281 */
    0x00, /* 5G  G0 Delta  Group 4 | offset:0x0282 */
    0x00, /* 5G  G0 Delta  Group 5 | offset:0x0283 */
    0x00, /* 5G  G0 Delta  Group 6 | offset:0x0284 */
    0x00, /* 5G  G0 Delta  Group 7 | offset:0x0285 */
    0x00, /* 5G  G0 Delta  Group 8 | offset:0x0286 */
    0x00, /* 5G Group1 WF0 LNA_gain_cal(Diff_UH_H, Diff_H_M ) | offset:0x0287 */
    0x00, /* 5G Group1 WF0 LNA_gain_cal( Diff_M_L, Diff_L_UL ) | offset:0x0288 */
    0x00, /* 5G Group2 WF0 LNA_gain_cal(Diff_UH_H, Diff_H_M ) | offset:0x0289 */
    0x00, /* 5G Group2 WF0 LNA_gain_cal( Diff_M_L, Diff_L_UL ) | offset:0x028A */
    0x00, /* 5G Group3 WF0 LNA_gain_cal(Diff_UH_H, Diff_H_M ) | offset:0x028B */
    0x00, /* 5G Group3 WF0 LNA_gain_cal( Diff_M_L, Diff_L_UL ) | offset:0x028C */
    0x00, /* 5G Group4 WF0 LNA_gain_cal(Diff_UH_H, Diff_H_M ) | offset:0x028D */
    0x00, /* 5G Group4 WF0 LNA_gain_cal( Diff_M_L, Diff_L_UL ) | offset:0x028E */
    0x00, /* 5G Group5 WF0 LNA_gain_cal(Diff_UH_H, Diff_H_M ) | offset:0x028F */
    0x00, /* 5G Group5 WF0 LNA_gain_cal( Diff_M_L, Diff_L_UL ) | offset:0x0290 */
    0x00, /* 5G Group6 WF0 LNA_gain_cal(Diff_UH_H, Diff_H_M ) | offset:0x0291 */
    0x00, /* 5G Group6 WF0 LNA_gain_cal( Diff_M_L, Diff_L_UL ) | offset:0x0292 */
    0x00, /* 5G Group7 WF0 LNA_gain_cal(Diff_UH_H, Diff_H_M ) | offset:0x0293 */
    0x00, /* 5G Group7 WF0 LNA_gain_cal( Diff_M_L, Diff_L_UL ) | offset:0x0294 */
    0x00, /* 5G Group8 WF0 LNA_gain_cal(Diff_UH_H, Diff_H_M ) | offset:0x0295 */
    0x00, /* 5G Group8 WF0 LNA_gain_cal( Diff_M_L, Diff_L_UL ) | offset:0x0296 */
    0x00, /* 5G_TX_CVT_offset_Group1_Low | offset:0x0297 */
    0x00, /* 5G_TX_CVT_offset_Group1_High | offset:0x0298 */
    0x00, /* 5G_TX_CVT_offset_Group2_Low | offset:0x0299 */
    0x00, /* 5G_TX_CVT_offset_Group2_High | offset:0x029A */
    0x00, /* 5G_TX_CVT_offset_Group3_Low | offset:0x029B */
    0x00, /* 5G_TX_CVT_offset_Group3_High | offset:0x029C */
    0x00, /* 5G_TX_CVT_offset_Group4_Low | offset:0x029D */
    0x00, /* 5G_TX_CVT_offset_Group4_High | offset:0x029E */
    0x00, /* 5G_TX_CVT_offset_Group5_Low | offset:0x029F */
    0x00, /* 5G_TX_CVT_offset_Group5_High | offset:0x02A0 */
    0x00, /* 5G_TX_CVT_offset_Group6_Low | offset:0x02A1 */
    0x00, /* 5G_TX_CVT_offset_Group6_High | offset:0x02A2 */
    0x00, /* 5G_TX_CVT_offset_Group7_Low | offset:0x02A3 */
    0x00, /* 5G_TX_CVT_offset_Group7_High | offset:0x02A4 */
    0x00, /* 5G_TX_CVT_offset_Group8_Low | offset:0x02A5 */
    0x00, /* 5G_TX_CVT_offset_Group8_High | offset:0x02A6 */
    0x00, /* reserved | offset:0x02A7 */
    0x00, /* reserved | offset:0x02A8 */
    0x00, /* reserved | offset:0x02A9 */
    0x00, /* reserved | offset:0x02AA */
    0x05, /* 2.4G Common, TYPE ID =0x05 | offset:0x02AB */
    0x64, /* Length = 0x64 | offset:0x02AC */
    0x00, /* 2.4G Temperature  for TSSI on :TSSI ON: WF0/1 TSSI PA Cell Offset_HPA(2G) | offset:0x02AD */
    0x00, /* 2.4G Temperature  for TSSI on: TSSI ON: WF0/1 TSSI PA Cell Offset_MPA(2G) | offset:0x02AE */
    0x00, /* 2.4G Dymaic iPA switch : PA CCK Threshold (chip out) for FPA | offset:0x02AF */
    0x00, /* 2.4G Dymaic iPA switch : PA Low Rate Threshold(chip out) for FPA | offset:0x02B0 */
    0x00, /* 2.4G Dymaic iPA switch : PA Middle Rate Threshold(chip out) for FPA | offset:0x02B1 */
    0x00, /* 2.4G Dymaic iPA switch : PA High Rate Threshold(chip out) for FPA | offset:0x02B2 */
    0x00, /* 2.4G Dymaic iPA switch : PA CCK Threshold (chip out) for HPA | offset:0x02B3 */
    0x00, /* 2.4G Dymaic iPA switch : PA Low Rate Threshold(chip out) for HPA | offset:0x02B4 */
    0x00, /* 2.4G Dymaic iPA switch : PA Middle Rate Threshold(chip out) for HPA | offset:0x02B5 */
    0x00, /* 2.4G Dymaic iPA switch : PA High Rate Threshold(chip out) for HPA | offset:0x02B6 */
    0x00, /* 2.4G Dymaic iPA switch : PA CCK Threshold (chip out) for MPA | offset:0x02B7 */
    0x00, /* 2.4G Dymaic iPA switch : PA Low Rate Threshold(chip out) for MPA | offset:0x02B8 */
    0x00, /* 2.4G Dymaic iPA switch : PA Middle Rate Threshold(chip out) for MPA | offset:0x02B9 */
    0x00, /* 2.4G Dymaic iPA switch : PA High Rate Threshold(chip out) for MPA | offset:0x02BA */
    0x00, /* 2.4G eLNA Received Mode (Gain) | offset:0x02BB */
    0x00, /* 2.4G eLNA Received Mode (P1dB) | offset:0x02BC */
    0x00, /* 2.4G eLNA Bypass Mode (Gain) | offset:0x02BD */
    0x00, /* 2.4G eLNA Bypass Mode (P1dB) | offset:0x02BE */
    0x00, /* 2.4G TSSI OFF: -7 step number; | offset:0x02BF */
    0x00, /* 2.4G TSSI OFF:-6 step number; | offset:0x02C0 */
    0x00, /* 2.4G TSSI OFF:-5 step number | offset:0x02C1 */
    0x00, /* 2.4G TSSI OFF:-4 step number | offset:0x02C2 */
    0x00, /* 2.4G TSSI OFF:-3 step number | offset:0x02C3 */
    0x00, /* 2.4G TSSI OFF:-2 step number | offset:0x02C4 */
    0x00, /* 2.4G TSSI OFF:-1 step number | offset:0x02C5 */
    0x00, /* 2.4G TSSI OFF:-0 step number | offset:0x02C6 */
    0x00, /* 2.4G TSSI OFF:2.4G reference step | offset:0x02C7 */
    0x00, /* 2.4G TSSI OFF:2.4G reference temp | offset:0x02C8 */
    0x00, /* 2.4G TSSI OFF:+1 step number | offset:0x02C9 */
    0x00, /* 2.4G TSSI OFF:+2 step number | offset:0x02CA */
    0x00, /* 2.4G TSSI OFF:+3 step number | offset:0x02CB */
    0x00, /* 2.4G TSSI OFF:+4 step number | offset:0x02CC */
    0x00, /* 2.4G TSSI OFF:+5 step number | offset:0x02CD */
    0x00, /* 2.4G TSSI OFF:+6 step number | offset:0x02CE */
    0x00, /* 2.4G TSSI OFF:+7 step number | offset:0x02CF */
    0x00, /* 2.4G ePA DPD on lowr bound for NT | offset:0x02D0 */
    0x00, /* 2.4G ePA DPD on upper bound for NT | offset:0x02D1 */
    0x00, /* 2.4G ePA DPD protection 1 for NT | offset:0x02D2 */
    0x00, /* 2.4G ePA DPD protection 2 for NT | offset:0x02D3 */
    0x00, /* 2.4G  ePA DPD on lowr bound for LT | offset:0x02D4 */
    0x00, /* 2.4G ePA DPD on upper bound for LT | offset:0x02D5 */
    0x00, /* 2.4G ePA DPD protection 1 for LT | offset:0x02D6 */
    0x00, /* 2.4G ePA DPD protection 2 for LT | offset:0x02D7 */
    0x00, /* 2.4G  ePA DPD on lowr bound for HT | offset:0x02D8 */
    0x00, /* 2.4G ePA DPD on upper bound for HT | offset:0x02D9 */
    0x00, /* 2.4G ePA DPD protection 1 for HT | offset:0x02DA */
    0x00, /* 2.4G ePA DPD protection 2 for HT | offset:0x02DB */
    0x00, /* 2.4G ePA Tx FE  Gain | offset:0x02DC */
    0x00, /* 2.4G DesenseMaskLowCh | offset:0x02DD */
    0x00, /* 2.4G DesenseMaskHighCh | offset:0x02DE */
    0x00, /* 2.4G PA BIAS 1 High LSP | offset:0x02DF */
    0x00, /* 2.4G PA BIAS 1 High MSP | offset:0x02E0 */
    0x00, /* 2.4G PA BIAS 1 Low LSP | offset:0x02E1 */
    0x00, /* 2.4G PA BIAS 1 Low MSP | offset:0x02E2 */
    0x00, /* 2.4G PA BIAS 2 High LSP | offset:0x02E3 */
    0x00, /* 2.4G PA BIAS 2 High MSP | offset:0x02E4 */
    0x00, /* 2.4G PA BIAS 2 Low LSP | offset:0x02E5 */
    0x00, /* 2.4G PA BIAS 2 Low MSP | offset:0x02E6 */
    0x00, /* 2.4G Shaping Right CH 1 | offset:0x02E7 */
    0x00, /* 2.4G Shaping Lefit CH 1 | offset:0x02E8 */
    0x00, /* 2.4G Shaping DC CH 1 | offset:0x02E9 */
    0x00, /* 2.4G Shaping Right CH 2 | offset:0x02EA */
    0x00, /* 2.4G Shaping Lefit CH 2 | offset:0x02EB */
    0x00, /* 2.4G Shaping DC CH 2 | offset:0x02EC */
    0x00, /* 2.4G Shaping Right CH 3 | offset:0x02ED */
    0x00, /* 2.4G Shaping Lefit CH 3 | offset:0x02EE */
    0x00, /* 2.4G Shaping DC CH 3 | offset:0x02EF */
    0x00, /* 2.4G Shaping Right CH 4 | offset:0x02F0 */
    0x00, /* 2.4G Shaping Lefit CH 4 | offset:0x02F1 */
    0x00, /* 2.4G Shaping DC CH 4 | offset:0x02F2 */
    0x00, /* 2.4G Shaping Right CH 5 | offset:0x02F3 */
    0x00, /* 2.4G Shaping Lefit CH 5 | offset:0x02F4 */
    0x00, /* 2.4G Shaping DC CH 5 | offset:0x02F5 */
    0x00, /* 2.4G Shaping Right CH 6 | offset:0x02F6 */
    0x00, /* 2.4G Shaping Lefit CH 6 | offset:0x02F7 */
    0x00, /* 2.4G Shaping DC CH 6 | offset:0x02F8 */
    0x00, /* 2.4G Shaping Right CH 7 | offset:0x02F9 */
    0x00, /* 2.4G Shaping Lefit CH 7 | offset:0x02FA */
    0x00, /* 2.4G Shaping DC CH 7 | offset:0x02FB */
    0x00, /* 2.4G Shaping Right CH 8 | offset:0x02FC */
    0x00, /* 2.4G Shaping Lefit CH 8 | offset:0x02FD */
    0x00, /* 2.4G Shaping DC CH 8 | offset:0x02FE */
    0x00, /* 2.4G Shaping Right CH 9 | offset:0x02FF */
    0x00, /* 2.4G Shaping Lefit CH 9 | offset:0x0300 */
    0x00, /* 2.4G Shaping DC CH 9 | offset:0x0301 */
    0x00, /* 2.4G Shaping Right CH 10 | offset:0x0302 */
    0x00, /* 2.4G Shaping Lefit CH 10 | offset:0x0303 */
    0x00, /* 2.4G Shaping DC CH 10 | offset:0x0304 */
    0x00, /* 2.4G Shaping Right CH 11 | offset:0x0305 */
    0x00, /* 2.4G Shaping Lefit CH 11 | offset:0x0306 */
    0x00, /* 2.4G Shaping DC CH 11 | offset:0x0307 */
    0x00, /* 2.4G Shaping Right CH 12 | offset:0x0308 */
    0x00, /* 2.4G Shaping Lefit CH 12 | offset:0x0309 */
    0x00, /* 2.4G Shaping DC CH 12 | offset:0x030A */
    0x00, /* 2.4G Shaping Right CH 13 | offset:0x030B */
    0x00, /* 2.4G Shaping Lefit CH 13 | offset:0x030C */
    0x00, /* 2.4G Shaping DC CH 13 | offset:0x030D */
    0x00, /* 2.4G Shaping Right CH 14 | offset:0x030E */
    0x00, /* 2.4G Shaping Lefit CH 14 | offset:0x030F */
    0x00, /* 2.4G Shaping DC CH 14 | offset:0x0310 */
    0x06, /* 5G Common, TYPE ID = 0x06 | offset:0x0311 */
    0x55, /* Length = 0x55 | offset:0x0312 */
    0x00, /* (5G Group1) TSSI ON: WF0/1 TSSI PA Cell Offset_HPA | offset:0x0313 */
    0x00, /* (5G Group1) TSSI ON: WF0/1 TSSI PA Cell Offset_MPA | offset:0x0314 */
    0x00, /* (5G Group2) TSSI ON: WF0/1 TSSI PA Cell Offset_HPA | offset:0x0315 */
    0x00, /* (5G Group2) TSSI ON: WF0/1 TSSI PA Cell Offset_MPA | offset:0x0316 */
    0x00, /* (5G Group3) TSSI ON: WF0/1 TSSI PA Cell Offset_HPA | offset:0x0317 */
    0x00, /* (5G Group3) TSSI ON: WF0/1 TSSI PA Cell Offset_MPA | offset:0x0318 */
    0x00, /* (5G Group4) TSSI ON: WF0/1 TSSI PA Cell Offset_HPA | offset:0x0319 */
    0x00, /* (5G Group4) TSSI ON: WF0/1 TSSI PA Cell Offset_MPA | offset:0x031A */
    0x00, /* (5G Group5) TSSI ON: WF0/1 TSSI PA Cell Offset_HPA | offset:0x031B */
    0x00, /* (5G Group5) TSSI ON: WF0/1 TSSI PA Cell Offset_MPA | offset:0x031C */
    0x00, /* (5G Group6) TSSI ON: WF0/1 TSSI PA Cell Offset_HPA | offset:0x031D */
    0x00, /* (5G Group6) TSSI ON: WF0/1 TSSI PA Cell Offset_MPA | offset:0x031E */
    0x00, /* (5G Group7) TSSI ON: WF0/1 TSSI PA Cell Offset_HPA | offset:0x031F */
    0x00, /* (5G Group7) TSSI ON: WF0/1 TSSI PA Cell Offset_MPA | offset:0x0320 */
    0x00, /* (5G Group8) TSSI ON: WF0/1 TSSI PA Cell Offset_HPA | offset:0x0321 */
    0x00, /* (5G Group8) TSSI ON: WF0/1 TSSI PA Cell Offset_MPA | offset:0x0322 */
    0x00, /* 5G Dymaic iPA switch : PA CCK Threshold (chip out) for FPA | offset:0x0323 */
    0x00, /* 5G Dymaic iPA switch : PA Low Rate Threshold(chip out) for FPA | offset:0x0324 */
    0x00, /* 5G Dymaic iPA switch : PA Middle Rate Threshold(chip out) for FPA | offset:0x0325 */
    0x00, /* 5G Dymaic iPA switch : PA High Rate Threshold(chip out) for FPA | offset:0x0326 */
    0x00, /* 5G Dymaic iPA switch : PA CCK Threshold (chip out) for HPA | offset:0x0327 */
    0x00, /* 5G Dymaic iPA switch : PA Low Rate Threshold(chip out) for HPA | offset:0x0328 */
    0x00, /* 5G Dymaic iPA switch : PA Middle Rate Threshold(chip out) for HPA | offset:0x0329 */
    0x00, /* 5G Dymaic iPA switch : PA High Rate Threshold(chip out) for HPA | offset:0x032A */
    0x00, /* 5G Dymaic iPA switch : PA CCK Threshold (chip out) for MPA | offset:0x032B */
    0x00, /* 5G Dymaic iPA switch : PA Low Rate Threshold(chip out) for MPA | offset:0x032C */
    0x00, /* 5G Dymaic iPA switch : PA Middle Rate Threshold(chip out) for MPA | offset:0x032D */
    0x00, /* 5G Dymaic iPA switch : PA High Rate Threshold(chip out) for MPA | offset:0x032E */
    0x00, /* 5G LB eLNA Received Mode (Gain) | offset:0x032F */
    0x00, /* 5G LB eLNA Received Mode (NF) | offset:0x0330 */
    0x00, /* 5G LB eLNA Received Mode (P1dB) | offset:0x0331 */
    0x00, /* 5G LB eLNA Bypass Mode (Gain) | offset:0x0332 */
    0x00, /* 5G LB eLNA Bypass Mode (NF) | offset:0x0333 */
    0x00, /* 5G LB eLNA Bypass Mode (P1dB) | offset:0x0334 */
    0x00, /* 5G MB eLNA Received Mode (Gain) | offset:0x0335 */
    0x00, /* 5G MB eLNA Received Mode (NF) | offset:0x0336 */
    0x00, /* 5G MB eLNA Received Mode (P1dB) | offset:0x0337 */
    0x00, /* 5G MB eLNA Bypass Mode (Gain) | offset:0x0338 */
    0x00, /* 5G MB eLNA Bypass Mode (NF) | offset:0x0339 */
    0x00, /* 5G MB eLNA Bypass Mode (P1dB) | offset:0x033A */
    0x00, /* 5G HB eLNA Received Mode (Gain) | offset:0x033B */
    0x00, /* 5G HB eLNA Received Mode (NF) | offset:0x033C */
    0x00, /* 5G HB eLNA Received Mode (P1dB) | offset:0x033D */
    0x00, /* 5G HB eLNA Bypass Mode (Gain) | offset:0x033E */
    0x00, /* 5G HB eLNA Bypass Mode (NF) | offset:0x033F */
    0x00, /* 5G HB eLNA Bypass Mode (P1dB) | offset:0x0340 */
    0x00, /* 5G TSSI OFF: -7 step number; | offset:0x0341 */
    0x00, /* 5G TSSI OFF:-6 step number; | offset:0x0342 */
    0x00, /* 5G TSSI OFF:-5 step number | offset:0x0343 */
    0x00, /* 5G TSSI OFF:-4 step number | offset:0x0344 */
    0x00, /* 5G TSSI OFF:-3 step number | offset:0x0345 */
    0x00, /* 5G TSSI OFF:-2 step number | offset:0x0346 */
    0x00, /* 5G TSSI OFF:-1 step number | offset:0x0347 */
    0x00, /* 5G TSSI OFF:-0 step number | offset:0x0348 */
    0x00, /* 5G TSSI OFF: 5G reference step | offset:0x0349 */
    0x00, /* 5G TSSI OFF: 5G reference temp | offset:0x034A */
    0x00, /* 5G TSSI OFF:+1 step number | offset:0x034B */
    0x00, /* 5G TSSI OFF:+2 step number | offset:0x034C */
    0x00, /* 5G TSSI OFF:+3 step number | offset:0x034D */
    0x00, /* 5G TSSI OFF:+4 step number | offset:0x034E */
    0x00, /* 5G TSSI OFF:+5 step number | offset:0x034F */
    0x00, /* 5G TSSI OFF:+6 step number | offset:0x0350 */
    0x00, /* 5G TSSI OFF:+7 step number | offset:0x0351 */
    0x00, /* 5G  ePA DPD on lowr bound for NT | offset:0x0352 */
    0x00, /* 5G ePA DPD on upper bound for NT | offset:0x0353 */
    0x00, /* 5G ePA DPD protection 1 for NT | offset:0x0354 */
    0x00, /* 5G ePA DPD protection 2 for NT | offset:0x0355 */
    0x00, /* 5G  ePA DPD on lowr bound for LT | offset:0x0356 */
    0x00, /* 5G ePA DPD on upper bound for LT | offset:0x0357 */
    0x00, /* 5G ePA DPD protection 1 for LT | offset:0x0358 */
    0x00, /* 5G ePA DPD protection 2 for LT | offset:0x0359 */
    0x00, /* 5G  ePA DPD on lowr bound for HT | offset:0x035A */
    0x00, /* 5G ePA DPD on upper bound for HT | offset:0x035B */
    0x00, /* 5G ePA DPD protection 1 for HT | offset:0x035C */
    0x00, /* 5G ePA DPD protection 2 for HT | offset:0x035D */
    0x00, /* 5G ePA Tx FE  Gain | offset:0x035E */
    0x00, /* 5G DesenseMask | offset:0x035F */
    0x00, /* 5G PA BIAS 1 High LSP | offset:0x0360 */
    0x00, /* 5G PA BIAS 1 High MSP | offset:0x0361 */
    0x00, /* 5G PA BIAS 1 Low LSP | offset:0x0362 */
    0x00, /* 5G PA BIAS 1 Low MSP | offset:0x0363 */
    0x00, /* Reserved | offset:0x0364 */
    0x00, /* Reserved | offset:0x0365 */
    0x00, /* Reserved | offset:0x0366 */
    0x00, /* Reserved | offset:0x0367 */
    0x07, /* System, TYPE ID = 0x07 | offset:0x0368 */
    0x3a, /* Length = 0x3A | offset:0x0369 */
    0x00, /* TxPath | offset:0x036A */
    0x00, /* RxPath | offset:0x036B */
    0x00, /* BandSelectBand0 | offset:0x036C */
    0x00, /* CckTxStream | offset:0x036D */
    0x00, /* OfdmTxStream | offset:0x036E */
    0x00, /* BandSelectBand1 | offset:0x036F */
    0x00, /* RxStream | offset:0x0370 */
    0x00, /* DbdcForceMode | offset:0x0371 */
    0x00, /* TxSW_N_G | offset:0x0372 */
    0x00, /* TxSW_P_G | offset:0x0373 */
    0x00, /* TxSW_N_A | offset:0x0374 */
    0x00, /* TxSW_P_A | offset:0x0375 */
    0x00, /* Dbdc0TxPath | offset:0x0376 */
    0x00, /* Dbdc0RxPath | offset:0x0377 */
    0x00, /* Dbdc1TxPath | offset:0x0378 */
    0x00, /* Dbdc1RxPath | offset:0x0379 */
    0x00, /* TxFiler_G        | offset:0x037A */
    0x00, /* TxFiler_A        | offset:0x037B */
    0x00, /* IpaDPD_G         | offset:0x037C */
    0x00, /* EpaDPD_G         | offset:0x037D */
    0x00, /* IpaDPD_A         | offset:0x037E */
    0x00, /* EpaDPD_A         | offset:0x037F */
    0x00, /* DPDTable_G       | offset:0x0380 */
    0x00, /* DPDTable_A       | offset:0x0381 */
    0x00, /* PaEn_G           | offset:0x0382 */
    0x00, /* PaEn_A           | offset:0x0383 */
    0x00, /* FixPa_G          | offset:0x0384 */
    0x00, /* FixPa_A          | offset:0x0385 */
    0x00, /* FixPaCell_G      | offset:0x0386 */
    0x00, /* FixPaCell_A      | offset:0x0387 */
    0x00, /* LnaEn_G          | offset:0x0388 */
    0x00, /* LnaEn_A          | offset:0x0389 */
    0x00, /* LnaPaSelect_G    | offset:0x038A */
    0x00, /* LnaPaSelect_A    | offset:0x038B */
    0x00, /* RxDCOC_G         | offset:0x038C */
    0x00, /* RxDCOC_A         | offset:0x038D */
    0x00, /* TssiSet_G         | offset:0x038E */
    0x00, /* TempCompSel_G     | offset:0x038F */
    0x00, /* TssiSet_A         | offset:0x0390 */
    0x00, /* TempCompSel_A     | offset:0x0391 */
    0x00, /* TssiSet_BAND1     | offset:0x0392 */
    0x00, /* TempCompSel_BAND1 | offset:0x0393 */
    0x00, /* CoRx              | offset:0x0394 */
    0x00, /* THADCAnalogPart   | offset:0x0395 */
    0x00, /* THADCSlopPart     | offset:0x0396 */
    0x00, /* THADCCalPart      | offset:0x0397 */
    0x00, /* ThermalCompOffset | offset:0x0398 */
    0x00, /* WF0RcalResult     | offset:0x0399 */
    0x00, /* WF2RcalResult     | offset:0x039A */
    0x00, /* RFTxLcLoop        | offset:0x039B */
    0x00, /* CoexIsolation     | offset:0x039C */
    0x00, /* RxAntDiversity_G  | offset:0x039D */
    0x00, /* RxAntDiversity_A  | offset:0x039E */
    0x00, /* Reserved | offset:0x039F */
    0x00, /* Reserved | offset:0x03A0 */
    0x00, /* Reserved | offset:0x03A1 */
    0x00, /* Reserved | offset:0x03A2 */
    0x00, /* Reserved | offset:0x03A3 */
    0x00, /*  | offset:0x03A4 */
    0x00, /*  | offset:0x03A5 */
    0x00, /*  | offset:0x03A6 */
    0x00, /*  | offset:0x03A7 */
    0x00, /*  | offset:0x03A8 */
    0x00, /*  | offset:0x03A9 */
    0x00, /*  | offset:0x03AA */
    0x00, /*  | offset:0x03AB */
    0x00, /*  | offset:0x03AC */
    0x00, /*  | offset:0x03AD */
    0x00, /*  | offset:0x03AE */
    0x00, /*  | offset:0x03AF */
    0x00, /*  | offset:0x03B0 */
    0x00, /*  | offset:0x03B1 */
    0x00, /*  | offset:0x03B2 */
    0x00, /*  | offset:0x03B3 */
    0x00, /*  | offset:0x03B4 */
    0x00, /*  | offset:0x03B5 */
    0x00, /*  | offset:0x03B6 */
    0x00, /*  | offset:0x03B7 */
    0x00, /*  | offset:0x03B8 */
    0x00, /*  | offset:0x03B9 */
    0x00, /*  | offset:0x03BA */
    0x00, /*  | offset:0x03BB */
    0x00, /*  | offset:0x03BC */
    0x00, /*  | offset:0x03BD */
    0x00, /*  | offset:0x03BE */
    0x00, /*  | offset:0x03BF */
    0x00, /*  | offset:0x03C0 */
    0x00, /*  | offset:0x03C1 */
    0x00, /*  | offset:0x03C2 */
    0x00, /*  | offset:0x03C3 */
    0x00, /*  | offset:0x03C4 */
    0x00, /*  | offset:0x03C5 */
    0x00, /*  | offset:0x03C6 */
    0x00, /*  | offset:0x03C7 */
    0x00, /*  | offset:0x03C8 */
    0x00, /*  | offset:0x03C9 */
    0x00, /*  | offset:0x03CA */
    0x00, /*  | offset:0x03CB */
    0x00, /*  | offset:0x03CC */
    0x00, /*  | offset:0x03CD */
    0x00, /*  | offset:0x03CE */
    0x00, /*  | offset:0x03CF */
    0x00, /*  | offset:0x03D0 */
    0x00, /*  | offset:0x03D1 */
    0x00, /*  | offset:0x03D2 */
    0x00, /*  | offset:0x03D3 */
    0x00, /*  | offset:0x03D4 */
    0x00, /*  | offset:0x03D5 */
    0x00, /*  | offset:0x03D6 */
    0x00, /*  | offset:0x03D7 */
    0x00, /*  | offset:0x03D8 */
    0x00, /*  | offset:0x03D9 */
    0x00, /*  | offset:0x03DA */
    0x00, /*  | offset:0x03DB */
    0x00, /*  | offset:0x03DC */
    0x00, /*  | offset:0x03DD */
    0x00, /*  | offset:0x03DE */
    0x00, /*  | offset:0x03DF */
    0x00, /*  | offset:0x03E0 */
    0x00, /*  | offset:0x03E1 */
    0x00, /*  | offset:0x03E2 */
    0x00, /*  | offset:0x03E3 */
    0x00, /*  | offset:0x03E4 */
    0x00, /*  | offset:0x03E5 */
    0x00, /*  | offset:0x03E6 */
    0x00, /*  | offset:0x03E7 */
    0x00, /*  | offset:0x03E8 */
    0x00, /*  | offset:0x03E9 */
    0x00, /*  | offset:0x03EA */
    0x00, /*  | offset:0x03EB */
    0x00, /*  | offset:0x03EC */
    0x00, /*  | offset:0x03ED */
    0x00, /*  | offset:0x03EE */
    0x00, /*  | offset:0x03EF */
    0x00, /*  | offset:0x03F0 */
    0x00, /*  | offset:0x03F1 */
    0x00, /*  | offset:0x03F2 */
    0x00, /*  | offset:0x03F3 */
    0x00, /*  | offset:0x03F4 */
    0x00, /*  | offset:0x03F5 */
    0x00, /*  | offset:0x03F6 */
    0x00, /*  | offset:0x03F7 */
    0x00, /*  | offset:0x03F8 */
    0x00, /*  | offset:0x03F9 */
    0x00, /*  | offset:0x03FA */
    0x00, /*  | offset:0x03FB */
    0x00, /*  | offset:0x03FC */
    0x00, /*  | offset:0x03FD */
    0x00, /*  | offset:0x03FE */
    0x00, /*  | offset:0x03FF */
    0x00, /*  | offset:0x0400 */
    0x00, /*  | offset:0x0401 */
    0x00, /*  | offset:0x0402 */
    0x00, /*  | offset:0x0403 */
    0x00, /*  | offset:0x0404 */
    0x00, /*  | offset:0x0405 */
    0x00, /*  | offset:0x0406 */
    0x00, /*  | offset:0x0407 */
    0x00, /*  | offset:0x0408 */
    0x00, /*  | offset:0x0409 */
    0x00, /*  | offset:0x040A */
    0x00, /*  | offset:0x040B */
    0x00, /*  | offset:0x040C */
    0x00, /*  | offset:0x040D */
    0x00, /*  | offset:0x040E */
    0x00, /*  | offset:0x040F */
    0x00, /*  | offset:0x0410 */
    0x00, /*  | offset:0x0411 */
    0x00, /*  | offset:0x0412 */
    0x00, /*  | offset:0x0413 */
    0x00, /*  | offset:0x0414 */
    0x00, /*  | offset:0x0415 */
    0x00, /*  | offset:0x0416 */
    0x00, /*  | offset:0x0417 */
    0x00, /*  | offset:0x0418 */
    0x00, /*  | offset:0x0419 */
    0x00, /*  | offset:0x041A */
    0x00, /*  | offset:0x041B */
    0x00, /*  | offset:0x041C */
    0x00, /*  | offset:0x041D */
    0x00, /*  | offset:0x041E */
    0x00, /*  | offset:0x041F */
    0x00, /*  | offset:0x0420 */
    0x00, /*  | offset:0x0421 */
    0x00, /*  | offset:0x0422 */
    0x00, /*  | offset:0x0423 */
    0x00, /*  | offset:0x0424 */
    0x00, /*  | offset:0x0425 */
    0x00, /*  | offset:0x0426 */
    0x00, /*  | offset:0x0427 */
    0x00, /*  | offset:0x0428 */
    0x00, /*  | offset:0x0429 */
    0x00, /*  | offset:0x042A */
    0x00, /*  | offset:0x042B */
    0x00, /*  | offset:0x042C */
    0x00, /*  | offset:0x042D */
    0x00, /*  | offset:0x042E */
    0x00, /*  | offset:0x042F */
    0x00, /*  | offset:0x0430 */
    0x00, /*  | offset:0x0431 */
    0x00, /*  | offset:0x0432 */
    0x00, /*  | offset:0x0433 */
    0x00, /*  | offset:0x0434 */
    0x00, /*  | offset:0x0435 */
    0x00, /*  | offset:0x0436 */
    0x00, /*  | offset:0x0437 */
    0x00, /*  | offset:0x0438 */
    0x00, /*  | offset:0x0439 */
    0x00, /*  | offset:0x043A */
    0x00, /*  | offset:0x043B */
    0x00, /*  | offset:0x043C */
    0x00, /*  | offset:0x043D */
    0x00, /*  | offset:0x043E */
    0x00, /*  | offset:0x043F */
    0x00, /*  | offset:0x0440 */
    0x00, /*  | offset:0x0441 */
    0x00, /*  | offset:0x0442 */
    0x00, /*  | offset:0x0443 */
    0x00, /*  | offset:0x0444 */
    0x00, /*  | offset:0x0445 */
    0x00, /*  | offset:0x0446 */
    0x00, /*  | offset:0x0447 */
    0x00, /*  | offset:0x0448 */
    0x00, /*  | offset:0x0449 */
    0x00, /*  | offset:0x044A */
    0x00, /*  | offset:0x044B */
    0x00, /*  | offset:0x044C */
    0x00, /*  | offset:0x044D */
    0x00, /*  | offset:0x044E */
    0x00, /*  | offset:0x044F */
    0x00, /*  | offset:0x0450 */
    0x00, /*  | offset:0x0451 */
    0x00, /*  | offset:0x0452 */
    0x00, /*  | offset:0x0453 */
    0x00, /*  | offset:0x0454 */
    0x00, /*  | offset:0x0455 */
    0x00, /*  | offset:0x0456 */
    0x00, /*  | offset:0x0457 */
    0x00, /*  | offset:0x0458 */
    0x00, /*  | offset:0x0459 */
    0x00, /*  | offset:0x045A */
    0x00, /*  | offset:0x045B */
    0x00, /*  | offset:0x045C */
    0x00, /*  | offset:0x045D */
    0x00, /*  | offset:0x045E */
    0x00, /*  | offset:0x045F */
    0x00, /*  | offset:0x0460 */
    0x00, /*  | offset:0x0461 */
    0x00, /*  | offset:0x0462 */
    0x00, /*  | offset:0x0463 */
    0x00, /*  | offset:0x0464 */
    0x00, /*  | offset:0x0465 */
    0x00, /*  | offset:0x0466 */
    0x00, /*  | offset:0x0467 */
    0x00, /*  | offset:0x0468 */
    0x00, /*  | offset:0x0469 */
    0x00, /*  | offset:0x046A */
    0x00, /*  | offset:0x046B */
    0x00, /*  | offset:0x046C */
    0x00, /*  | offset:0x046D */
    0x00, /*  | offset:0x046E */
    0x00, /*  | offset:0x046F */
    0x00, /*  | offset:0x0470 */
    0x00, /*  | offset:0x0471 */
    0x00, /*  | offset:0x0472 */
    0x00, /*  | offset:0x0473 */
    0x00, /*  | offset:0x0474 */
    0x00, /*  | offset:0x0475 */
    0x00, /*  | offset:0x0476 */
    0x00, /*  | offset:0x0477 */
    0x00, /*  | offset:0x0478 */
    0x00, /*  | offset:0x0479 */
    0x00, /*  | offset:0x047A */
    0x00, /*  | offset:0x047B */
    0x00, /*  | offset:0x047C */
    0x00, /*  | offset:0x047D */
    0x00, /*  | offset:0x047E */
    0x00, /*  | offset:0x047F */
    0x00, /*  | offset:0x0480 */
    0x00, /*  | offset:0x0481 */
    0x00, /*  | offset:0x0482 */
    0x00, /*  | offset:0x0483 */
    0x00, /*  | offset:0x0484 */
    0x00, /*  | offset:0x0485 */
    0x00, /*  | offset:0x0486 */
    0x00, /*  | offset:0x0487 */
    0x00, /*  | offset:0x0488 */
    0x00, /*  | offset:0x0489 */
    0x00, /*  | offset:0x048A */
    0x00, /*  | offset:0x048B */
    0x00, /*  | offset:0x048C */
    0x00, /*  | offset:0x048D */
    0x00, /*  | offset:0x048E */
    0x00, /*  | offset:0x048F */
    0x00, /*  | offset:0x0490 */
    0x00, /*  | offset:0x0491 */
    0x00, /*  | offset:0x0492 */
    0x00, /*  | offset:0x0493 */
    0x00, /*  | offset:0x0494 */
    0x00, /*  | offset:0x0495 */
    0x00, /*  | offset:0x0496 */
    0x00, /*  | offset:0x0497 */
    0x00, /*  | offset:0x0498 */
    0x00, /*  | offset:0x0499 */
    0x00, /*  | offset:0x049A */
    0x00, /*  | offset:0x049B */
    0x00, /*  | offset:0x049C */
    0x00, /*  | offset:0x049D */
    0x00, /*  | offset:0x049E */
    0x00, /*  | offset:0x049F */
    0x00, /*  | offset:0x04A0 */
    0x00, /*  | offset:0x04A1 */
    0x00, /*  | offset:0x04A2 */
    0x00, /*  | offset:0x04A3 */
    0x00, /*  | offset:0x04A4 */
    0x00, /*  | offset:0x04A5 */
    0x00, /*  | offset:0x04A6 */
    0x00, /*  | offset:0x04A7 */
    0x00, /*  | offset:0x04A8 */
    0x00, /*  | offset:0x04A9 */
    0x00, /*  | offset:0x04AA */
    0x00, /*  | offset:0x04AB */
    0x00, /*  | offset:0x04AC */
    0x00, /*  | offset:0x04AD */
    0x00, /*  | offset:0x04AE */
    0x00, /*  | offset:0x04AF */
    0x00, /*  | offset:0x04B0 */
    0x00, /*  | offset:0x04B1 */
    0x00, /*  | offset:0x04B2 */
    0x00, /*  | offset:0x04B3 */
    0x00, /*  | offset:0x04B4 */
    0x00, /*  | offset:0x04B5 */
    0x00, /*  | offset:0x04B6 */
    0x00, /*  | offset:0x04B7 */
    0x00, /*  | offset:0x04B8 */
    0x00, /*  | offset:0x04B9 */
    0x00, /*  | offset:0x04BA */
    0x00, /*  | offset:0x04BB */
    0x00, /*  | offset:0x04BC */
    0x00, /*  | offset:0x04BD */
    0x00, /*  | offset:0x04BE */
    0x00, /*  | offset:0x04BF */
    0x00, /*  | offset:0x04C0 */
    0x00, /*  | offset:0x04C1 */
    0x00, /*  | offset:0x04C2 */
    0x00, /*  | offset:0x04C3 */
    0x00, /*  | offset:0x04C4 */
    0x00, /*  | offset:0x04C5 */
    0x00, /*  | offset:0x04C6 */
    0x00, /*  | offset:0x04C7 */
    0x00, /*  | offset:0x04C8 */
    0x00, /*  | offset:0x04C9 */
    0x00, /*  | offset:0x04CA */
    0x00, /*  | offset:0x04CB */
    0x00, /*  | offset:0x04CC */
    0x00, /*  | offset:0x04CD */
    0x00, /*  | offset:0x04CE */
    0x00, /*  | offset:0x04CF */
    0x00, /*  | offset:0x04D0 */
    0x00, /*  | offset:0x04D1 */
    0x00, /*  | offset:0x04D2 */
    0x00, /*  | offset:0x04D3 */
    0x00, /*  | offset:0x04D4 */
    0x00, /*  | offset:0x04D5 */
    0x00, /*  | offset:0x04D6 */
    0x00, /*  | offset:0x04D7 */
    0x00, /*  | offset:0x04D8 */
    0x00, /*  | offset:0x04D9 */
    0x00, /*  | offset:0x04DA */
    0x00, /*  | offset:0x04DB */
    0x00, /*  | offset:0x04DC */
    0x00, /*  | offset:0x04DD */
    0x00, /*  | offset:0x04DE */
    0x00, /*  | offset:0x04DF */
    0x00, /*  | offset:0x04E0 */
    0x00, /*  | offset:0x04E1 */
    0x00, /*  | offset:0x04E2 */
    0x00, /*  | offset:0x04E3 */
    0x00, /*  | offset:0x04E4 */
    0x00, /*  | offset:0x04E5 */
    0x00, /*  | offset:0x04E6 */
    0x00, /*  | offset:0x04E7 */
    0x00, /*  | offset:0x04E8 */
    0x00, /*  | offset:0x04E9 */
    0x00, /*  | offset:0x04EA */
    0x00, /*  | offset:0x04EB */
    0x00, /*  | offset:0x04EC */
    0x00, /*  | offset:0x04ED */
    0x00, /*  | offset:0x04EE */
    0x00, /*  | offset:0x04EF */
    0x00, /*  | offset:0x04F0 */
    0x00, /*  | offset:0x04F1 */
    0x00, /*  | offset:0x04F2 */
    0x00, /*  | offset:0x04F3 */
    0x00, /*  | offset:0x04F4 */
    0x00, /*  | offset:0x04F5 */
    0x00, /*  | offset:0x04F6 */
    0x00, /*  | offset:0x04F7 */
    0x00, /*  | offset:0x04F8 */
    0x00, /*  | offset:0x04F9 */
    0x00, /*  | offset:0x04FA */
    0x00, /*  | offset:0x04FB */
    0x00, /*  | offset:0x04FC */
    0x00, /*  | offset:0x04FD */
    0x00, /*  | offset:0x04FE */
    0x00, /*  | offset:0x04FF */
    0x00, /*  | offset:0x0500 */
    0x00, /*  | offset:0x0501 */
    0x00, /*  | offset:0x0502 */
    0x00, /*  | offset:0x0503 */
    0x00, /*  | offset:0x0504 */
    0x00, /*  | offset:0x0505 */
    0x00, /*  | offset:0x0506 */
    0x00, /*  | offset:0x0507 */
    0x00, /*  | offset:0x0508 */
    0x00, /*  | offset:0x0509 */
    0x00, /*  | offset:0x050A */
    0x00, /*  | offset:0x050B */
    0x00, /*  | offset:0x050C */
    0x00, /*  | offset:0x050D */
    0x00, /*  | offset:0x050E */
    0x00, /*  | offset:0x050F */
    0x00, /*  | offset:0x0510 */
    0x00, /*  | offset:0x0511 */
    0x00, /*  | offset:0x0512 */
    0x00, /*  | offset:0x0513 */
    0x00, /*  | offset:0x0514 */
    0x00, /*  | offset:0x0515 */
    0x00, /*  | offset:0x0516 */
    0x00, /*  | offset:0x0517 */
    0x00, /*  | offset:0x0518 */
    0x00, /*  | offset:0x0519 */
    0x00, /*  | offset:0x051A */
    0x00, /*  | offset:0x051B */
    0x00, /*  | offset:0x051C */
    0x00, /*  | offset:0x051D */
    0x00, /*  | offset:0x051E */
    0x00, /*  | offset:0x051F */
    0x00, /*  | offset:0x0520 */
    0x00, /*  | offset:0x0521 */
    0x00, /*  | offset:0x0522 */
    0x00, /*  | offset:0x0523 */
    0x00, /*  | offset:0x0524 */
    0x00, /*  | offset:0x0525 */
    0x00, /*  | offset:0x0526 */
    0x00, /*  | offset:0x0527 */
    0x00, /*  | offset:0x0528 */
    0x00, /*  | offset:0x0529 */
    0x00, /*  | offset:0x052A */
    0x00, /*  | offset:0x052B */
    0x00, /*  | offset:0x052C */
    0x00, /*  | offset:0x052D */
    0x00, /*  | offset:0x052E */
    0x00, /*  | offset:0x052F */
    0x00, /*  | offset:0x0530 */
    0x00, /*  | offset:0x0531 */
    0x00, /*  | offset:0x0532 */
    0x00, /*  | offset:0x0533 */
    0x00, /*  | offset:0x0534 */
    0x00, /*  | offset:0x0535 */
    0x00, /*  | offset:0x0536 */
    0x00, /*  | offset:0x0537 */
    0x00, /*  | offset:0x0538 */
    0x00, /*  | offset:0x0539 */
    0x00, /*  | offset:0x053A */
    0x00, /*  | offset:0x053B */
    0x00, /*  | offset:0x053C */
    0x00, /*  | offset:0x053D */
    0x00, /*  | offset:0x053E */
    0x00, /*  | offset:0x053F */
    0x00, /*  | offset:0x0540 */
    0x00, /*  | offset:0x0541 */
    0x00, /*  | offset:0x0542 */
    0x00, /*  | offset:0x0543 */
    0x00, /*  | offset:0x0544 */
    0x00, /*  | offset:0x0545 */
    0x00, /*  | offset:0x0546 */
    0x00, /*  | offset:0x0547 */
    0x00, /*  | offset:0x0548 */
    0x00, /*  | offset:0x0549 */
    0x00, /*  | offset:0x054A */
    0x00, /*  | offset:0x054B */
    0x00, /*  | offset:0x054C */
    0x00, /*  | offset:0x054D */
    0x00, /*  | offset:0x054E */
    0x00, /*  | offset:0x054F */
    0x00, /*  | offset:0x0550 */
    0x00, /*  | offset:0x0551 */
    0x00, /*  | offset:0x0552 */
    0x00, /*  | offset:0x0553 */
    0x00, /*  | offset:0x0554 */
    0x00, /*  | offset:0x0555 */
    0x00, /*  | offset:0x0556 */
    0x00, /*  | offset:0x0557 */
    0x00, /*  | offset:0x0558 */
    0x00, /*  | offset:0x0559 */
    0x00, /*  | offset:0x055A */
    0x00, /*  | offset:0x055B */
    0x00, /*  | offset:0x055C */
    0x00, /*  | offset:0x055D */
    0x00, /*  | offset:0x055E */
    0x00, /*  | offset:0x055F */
    0x00, /*  | offset:0x0560 */
    0x00, /*  | offset:0x0561 */
    0x00, /*  | offset:0x0562 */
    0x00, /*  | offset:0x0563 */
    0x00, /*  | offset:0x0564 */
    0x00, /*  | offset:0x0565 */
    0x00, /*  | offset:0x0566 */
    0x00, /*  | offset:0x0567 */
    0x00, /*  | offset:0x0568 */
    0x00, /*  | offset:0x0569 */
    0x00, /*  | offset:0x056A */
    0x00, /*  | offset:0x056B */
    0x00, /*  | offset:0x056C */
    0x00, /*  | offset:0x056D */
    0x00, /*  | offset:0x056E */
    0x00, /*  | offset:0x056F */
    0x00, /*  | offset:0x0570 */
    0x00, /*  | offset:0x0571 */
    0x00, /*  | offset:0x0572 */
    0x00, /*  | offset:0x0573 */
    0x00, /*  | offset:0x0574 */
    0x00, /*  | offset:0x0575 */
    0x00, /*  | offset:0x0576 */
    0x00, /*  | offset:0x0577 */
    0x00, /*  | offset:0x0578 */
    0x00, /*  | offset:0x0579 */
    0x00, /*  | offset:0x057A */
    0x00, /*  | offset:0x057B */
    0x00, /*  | offset:0x057C */
    0x00, /*  | offset:0x057D */
    0x00, /*  | offset:0x057E */
    0x00, /*  | offset:0x057F */
    0x00, /*  | offset:0x0580 */
    0x00, /*  | offset:0x0581 */
    0x00, /*  | offset:0x0582 */
    0x00, /*  | offset:0x0583 */
    0x00, /*  | offset:0x0584 */
    0x00, /*  | offset:0x0585 */
    0x00, /*  | offset:0x0586 */
    0x00, /*  | offset:0x0587 */
    0x00, /*  | offset:0x0588 */
    0x00, /*  | offset:0x0589 */
    0x00, /*  | offset:0x058A */
    0x00, /*  | offset:0x058B */
    0x00, /*  | offset:0x058C */
    0x00, /*  | offset:0x058D */
    0x00, /*  | offset:0x058E */
    0x00, /*  | offset:0x058F */
    0x00, /*  | offset:0x0590 */
    0x00, /*  | offset:0x0591 */
    0x00, /*  | offset:0x0592 */
    0x00, /*  | offset:0x0593 */
    0x00, /*  | offset:0x0594 */
    0x00, /*  | offset:0x0595 */
    0x00, /*  | offset:0x0596 */
    0x00, /*  | offset:0x0597 */
    0x00, /*  | offset:0x0598 */
    0x00, /*  | offset:0x0599 */
    0x00, /*  | offset:0x059A */
    0x00, /*  | offset:0x059B */
    0x00, /*  | offset:0x059C */
    0x00, /*  | offset:0x059D */
    0x00, /*  | offset:0x059E */
    0x00, /*  | offset:0x059F */
    0x00, /*  | offset:0x05A0 */
    0x00, /*  | offset:0x05A1 */
    0x00, /*  | offset:0x05A2 */
    0x00, /*  | offset:0x05A3 */
    0x00, /*  | offset:0x05A4 */
    0x00, /*  | offset:0x05A5 */
    0x00, /*  | offset:0x05A6 */
    0x00, /*  | offset:0x05A7 */
    0x00, /*  | offset:0x05A8 */
    0x00, /*  | offset:0x05A9 */
    0x00, /*  | offset:0x05AA */
    0x00, /*  | offset:0x05AB */
    0x00, /*  | offset:0x05AC */
    0x00, /*  | offset:0x05AD */
    0x00, /*  | offset:0x05AE */
    0x00, /*  | offset:0x05AF */
    0x00, /*  | offset:0x05B0 */
    0x00, /*  | offset:0x05B1 */
    0x00, /*  | offset:0x05B2 */
    0x00, /*  | offset:0x05B3 */
    0x00, /*  | offset:0x05B4 */
    0x00, /*  | offset:0x05B5 */
    0x00, /*  | offset:0x05B6 */
    0x00, /*  | offset:0x05B7 */
    0x00, /*  | offset:0x05B8 */
    0x00, /*  | offset:0x05B9 */
    0x00, /*  | offset:0x05BA */
    0x00, /*  | offset:0x05BB */
    0x00, /*  | offset:0x05BC */
    0x00, /*  | offset:0x05BD */
    0x00, /*  | offset:0x05BE */
    0x00, /*  | offset:0x05BF */
    0x00, /*  | offset:0x05C0 */
    0x00, /*  | offset:0x05C1 */
    0x00, /*  | offset:0x05C2 */
    0x00, /*  | offset:0x05C3 */
    0x00, /*  | offset:0x05C4 */
    0x00, /*  | offset:0x05C5 */
    0x00, /*  | offset:0x05C6 */
    0x00, /*  | offset:0x05C7 */
    0x00, /*  | offset:0x05C8 */
    0x00, /*  | offset:0x05C9 */
    0x00, /*  | offset:0x05CA */
    0x00, /*  | offset:0x05CB */
    0x00, /*  | offset:0x05CC */
    0x00, /*  | offset:0x05CD */
    0x00, /*  | offset:0x05CE */
    0x00, /*  | offset:0x05CF */
    0x00, /*  | offset:0x05D0 */
    0x00, /*  | offset:0x05D1 */
    0x00, /*  | offset:0x05D2 */
    0x00, /*  | offset:0x05D3 */
    0x00, /*  | offset:0x05D4 */
    0x00, /*  | offset:0x05D5 */
    0x00, /*  | offset:0x05D6 */
    0x00, /*  | offset:0x05D7 */
    0x00, /*  | offset:0x05D8 */
    0x00, /*  | offset:0x05D9 */
    0x00, /*  | offset:0x05DA */
    0x00, /*  | offset:0x05DB */
    0x00, /*  | offset:0x05DC */
    0x00, /*  | offset:0x05DD */
    0x00, /*  | offset:0x05DE */
    0x00, /*  | offset:0x05DF */
    0x00, /*  | offset:0x05E0 */
    0x00, /*  | offset:0x05E1 */
    0x00, /*  | offset:0x05E2 */
    0x00, /*  | offset:0x05E3 */
    0x00, /*  | offset:0x05E4 */
    0x00, /*  | offset:0x05E5 */
    0x00, /*  | offset:0x05E6 */
    0x00, /*  | offset:0x05E7 */
    0x00, /*  | offset:0x05E8 */
    0x00, /*  | offset:0x05E9 */
    0x00, /*  | offset:0x05EA */
    0x00, /*  | offset:0x05EB */
    0x00, /*  | offset:0x05EC */
    0x00, /*  | offset:0x05ED */
    0x00, /*  | offset:0x05EE */
    0x00, /*  | offset:0x05EF */
    0x00, /*  | offset:0x05F0 */
    0x00, /*  | offset:0x05F1 */
    0x00, /*  | offset:0x05F2 */
    0x00, /*  | offset:0x05F3 */
    0x00, /*  | offset:0x05F4 */
    0x00, /*  | offset:0x05F5 */
    0x00, /*  | offset:0x05F6 */
    0x00, /*  | offset:0x05F7 */
    0x00, /*  | offset:0x05F8 */
    0x00, /*  | offset:0x05F9 */
    0x00, /*  | offset:0x05FA */
    0x00, /*  | offset:0x05FB */
    0x00, /*  | offset:0x05FC */
    0x00, /*  | offset:0x05FD */
    0x00, /*  | offset:0x05FE */
    0x00, /*  | offset:0x05FF */
    0x00, /*  | offset:0x0600 */
    0x00, /*  | offset:0x0601 */
    0x00, /*  | offset:0x0602 */
    0x00, /*  | offset:0x0603 */
    0x00, /*  | offset:0x0604 */
    0x00, /*  | offset:0x0605 */
    0x00, /*  | offset:0x0606 */
    0x00, /*  | offset:0x0607 */
    0x00, /*  | offset:0x0608 */
    0x00, /*  | offset:0x0609 */
    0x00, /*  | offset:0x060A */
    0x00, /*  | offset:0x060B */
    0x00, /*  | offset:0x060C */
    0x00, /*  | offset:0x060D */
    0x00, /*  | offset:0x060E */
    0x00, /*  | offset:0x060F */
    0x00, /*  | offset:0x0610 */
    0x00, /*  | offset:0x0611 */
    0x00, /*  | offset:0x0612 */
    0x00, /*  | offset:0x0613 */
    0x00, /*  | offset:0x0614 */
    0x00, /*  | offset:0x0615 */
    0x00, /*  | offset:0x0616 */
    0x00, /*  | offset:0x0617 */
    0x00, /*  | offset:0x0618 */
    0x00, /*  | offset:0x0619 */
    0x00, /*  | offset:0x061A */
    0x00, /*  | offset:0x061B */
    0x00, /*  | offset:0x061C */
    0x00, /*  | offset:0x061D */
    0x00, /*  | offset:0x061E */
    0x00, /*  | offset:0x061F */
    0x00, /*  | offset:0x0620 */
    0x00, /*  | offset:0x0621 */
    0x00, /*  | offset:0x0622 */
    0x00, /*  | offset:0x0623 */
    0x00, /*  | offset:0x0624 */
    0x00, /*  | offset:0x0625 */
    0x00, /*  | offset:0x0626 */
    0x00, /*  | offset:0x0627 */
    0x00, /*  | offset:0x0628 */
    0x00, /*  | offset:0x0629 */
    0x00, /*  | offset:0x062A */
    0x00, /*  | offset:0x062B */
    0x00, /*  | offset:0x062C */
    0x00, /*  | offset:0x062D */
    0x00, /*  | offset:0x062E */
    0x00, /*  | offset:0x062F */
    0x00, /*  | offset:0x0630 */
    0x00, /*  | offset:0x0631 */
    0x00, /*  | offset:0x0632 */
    0x00, /*  | offset:0x0633 */
    0x00, /*  | offset:0x0634 */
    0x00, /*  | offset:0x0635 */
    0x00, /*  | offset:0x0636 */
    0x00, /*  | offset:0x0637 */
    0x00, /*  | offset:0x0638 */
    0x00, /*  | offset:0x0639 */
    0x00, /*  | offset:0x063A */
    0x00, /*  | offset:0x063B */
    0x00, /*  | offset:0x063C */
    0x00, /*  | offset:0x063D */
    0x00, /*  | offset:0x063E */
    0x00, /*  | offset:0x063F */
    0x00, /*  | offset:0x0640 */
    0x00, /*  | offset:0x0641 */
    0x00, /*  | offset:0x0642 */
    0x00, /*  | offset:0x0643 */
    0x00, /*  | offset:0x0644 */
    0x00, /*  | offset:0x0645 */
    0x00, /*  | offset:0x0646 */
    0x00, /*  | offset:0x0647 */
    0x00, /*  | offset:0x0648 */
    0x00, /*  | offset:0x0649 */
    0x00, /*  | offset:0x064A */
    0x00, /*  | offset:0x064B */
    0x00, /*  | offset:0x064C */
    0x00, /*  | offset:0x064D */
    0x00, /*  | offset:0x064E */
    0x00, /*  | offset:0x064F */
    0x00, /*  | offset:0x0650 */
    0x00, /*  | offset:0x0651 */
    0x00, /*  | offset:0x0652 */
    0x00, /*  | offset:0x0653 */
    0x00, /*  | offset:0x0654 */
    0x00, /*  | offset:0x0655 */
    0x00, /*  | offset:0x0656 */
    0x00, /*  | offset:0x0657 */
    0x00, /*  | offset:0x0658 */
    0x00, /*  | offset:0x0659 */
    0x00, /*  | offset:0x065A */
    0x00, /*  | offset:0x065B */
    0x00, /*  | offset:0x065C */
    0x00, /*  | offset:0x065D */
    0x00, /*  | offset:0x065E */
    0x00, /*  | offset:0x065F */
    0x00, /*  | offset:0x0660 */
    0x00, /*  | offset:0x0661 */
    0x00, /*  | offset:0x0662 */
    0x00, /*  | offset:0x0663 */
    0x00, /*  | offset:0x0664 */
    0x00, /*  | offset:0x0665 */
    0x00, /*  | offset:0x0666 */
    0x00, /*  | offset:0x0667 */
    0x00, /*  | offset:0x0668 */
    0x00, /*  | offset:0x0669 */
    0x00, /*  | offset:0x066A */
    0x00, /*  | offset:0x066B */
    0x00, /*  | offset:0x066C */
    0x00, /*  | offset:0x066D */
    0x00, /*  | offset:0x066E */
    0x00, /*  | offset:0x066F */
    0x00, /*  | offset:0x0670 */
    0x00, /*  | offset:0x0671 */
    0x00, /*  | offset:0x0672 */
    0x00, /*  | offset:0x0673 */
    0x00, /*  | offset:0x0674 */
    0x00, /*  | offset:0x0675 */
    0x00, /*  | offset:0x0676 */
    0x00, /*  | offset:0x0677 */
    0x00, /*  | offset:0x0678 */
    0x00, /*  | offset:0x0679 */
    0x00, /*  | offset:0x067A */
    0x00, /*  | offset:0x067B */
    0x00, /*  | offset:0x067C */
    0x00, /*  | offset:0x067D */
    0x00, /*  | offset:0x067E */
    0x00, /*  | offset:0x067F */
    0x00, /*  | offset:0x0680 */
    0x00, /*  | offset:0x0681 */
    0x00, /*  | offset:0x0682 */
    0x00, /*  | offset:0x0683 */
    0x00, /*  | offset:0x0684 */
    0x00, /*  | offset:0x0685 */
    0x00, /*  | offset:0x0686 */
    0x00, /*  | offset:0x0687 */
    0x00, /*  | offset:0x0688 */
    0x00, /*  | offset:0x0689 */
    0x00, /*  | offset:0x068A */
    0x00, /*  | offset:0x068B */
    0x00, /*  | offset:0x068C */
    0x00, /*  | offset:0x068D */
    0x00, /*  | offset:0x068E */
    0x00, /*  | offset:0x068F */
    0x00, /*  | offset:0x0690 */
    0x00, /*  | offset:0x0691 */
    0x00, /*  | offset:0x0692 */
    0x00, /*  | offset:0x0693 */
    0x00, /*  | offset:0x0694 */
    0x00, /*  | offset:0x0695 */
    0x00, /*  | offset:0x0696 */
    0x00, /*  | offset:0x0697 */
    0x00, /*  | offset:0x0698 */
    0x00, /*  | offset:0x0699 */
    0x00, /*  | offset:0x069A */
    0x00, /*  | offset:0x069B */
    0x00, /*  | offset:0x069C */
    0x00, /*  | offset:0x069D */
    0x00, /*  | offset:0x069E */
    0x00, /*  | offset:0x069F */
    0x00, /*  | offset:0x06A0 */
    0x00, /*  | offset:0x06A1 */
    0x00, /*  | offset:0x06A2 */
    0x00, /*  | offset:0x06A3 */
    0x00, /*  | offset:0x06A4 */
    0x00, /*  | offset:0x06A5 */
    0x00, /*  | offset:0x06A6 */
    0x00, /*  | offset:0x06A7 */
    0x00, /*  | offset:0x06A8 */
    0x00, /*  | offset:0x06A9 */
    0x00, /*  | offset:0x06AA */
    0x00, /*  | offset:0x06AB */
    0x00, /*  | offset:0x06AC */
    0x00, /*  | offset:0x06AD */
    0x00, /*  | offset:0x06AE */
    0x00, /*  | offset:0x06AF */
    0x00, /*  | offset:0x06B0 */
    0x00, /*  | offset:0x06B1 */
    0x00, /*  | offset:0x06B2 */
    0x00, /*  | offset:0x06B3 */
    0x00, /*  | offset:0x06B4 */
    0x00, /*  | offset:0x06B5 */
    0x00, /*  | offset:0x06B6 */
    0x00, /*  | offset:0x06B7 */
    0x00, /*  | offset:0x06B8 */
    0x00, /*  | offset:0x06B9 */
    0x00, /*  | offset:0x06BA */
    0x00, /*  | offset:0x06BB */
    0x00, /*  | offset:0x06BC */
    0x00, /*  | offset:0x06BD */
    0x00, /*  | offset:0x06BE */
    0x00, /*  | offset:0x06BF */
    0x00, /*  | offset:0x06C0 */
    0x00, /*  | offset:0x06C1 */
    0x00, /*  | offset:0x06C2 */
    0x00, /*  | offset:0x06C3 */
    0x00, /*  | offset:0x06C4 */
    0x00, /*  | offset:0x06C5 */
    0x00, /*  | offset:0x06C6 */
    0x00, /*  | offset:0x06C7 */
    0x00, /*  | offset:0x06C8 */
    0x00, /*  | offset:0x06C9 */
    0x00, /*  | offset:0x06CA */
    0x00, /*  | offset:0x06CB */
    0x00, /*  | offset:0x06CC */
    0x00, /*  | offset:0x06CD */
    0x00, /*  | offset:0x06CE */
    0x00, /*  | offset:0x06CF */
    0x00, /*  | offset:0x06D0 */
    0x00, /*  | offset:0x06D1 */
    0x00, /*  | offset:0x06D2 */
    0x00, /*  | offset:0x06D3 */
    0x00, /*  | offset:0x06D4 */
    0x00, /*  | offset:0x06D5 */
    0x00, /*  | offset:0x06D6 */
    0x00, /*  | offset:0x06D7 */
    0x00, /*  | offset:0x06D8 */
    0x00, /*  | offset:0x06D9 */
    0x00, /*  | offset:0x06DA */
    0x00, /*  | offset:0x06DB */
    0x00, /*  | offset:0x06DC */
    0x00, /*  | offset:0x06DD */
    0x00, /*  | offset:0x06DE */
    0x00, /*  | offset:0x06DF */
    0x00, /*  | offset:0x06E0 */
    0x00, /*  | offset:0x06E1 */
    0x00, /*  | offset:0x06E2 */
    0x00, /*  | offset:0x06E3 */
    0x00, /*  | offset:0x06E4 */
    0x00, /*  | offset:0x06E5 */
    0x00, /*  | offset:0x06E6 */
    0x00, /*  | offset:0x06E7 */
    0x00, /*  | offset:0x06E8 */
    0x00, /*  | offset:0x06E9 */
    0x00, /*  | offset:0x06EA */
    0x00, /*  | offset:0x06EB */
    0x00, /*  | offset:0x06EC */
    0x00, /*  | offset:0x06ED */
    0x00, /*  | offset:0x06EE */
    0x00, /*  | offset:0x06EF */
    0x00, /*  | offset:0x06F0 */
    0x00, /*  | offset:0x06F1 */
    0x00, /*  | offset:0x06F2 */
    0x00, /*  | offset:0x06F3 */
    0x00, /*  | offset:0x06F4 */
    0x00, /*  | offset:0x06F5 */
    0x00, /*  | offset:0x06F6 */
    0x00, /*  | offset:0x06F7 */
    0x00, /*  | offset:0x06F8 */
    0x00, /*  | offset:0x06F9 */
    0x00, /*  | offset:0x06FA */
    0x00, /*  | offset:0x06FB */
    0x00, /*  | offset:0x06FC */
    0x00, /*  | offset:0x06FD */
    0x00, /*  | offset:0x06FE */
    0x00, /*  | offset:0x06FF */
    0x00, /*  | offset:0x0700 */
    0x00, /*  | offset:0x0701 */
    0x00, /*  | offset:0x0702 */
    0x00, /*  | offset:0x0703 */
    0x00, /*  | offset:0x0704 */
    0x00, /*  | offset:0x0705 */
    0x00, /*  | offset:0x0706 */
    0x00, /*  | offset:0x0707 */
    0x00, /*  | offset:0x0708 */
    0x00, /*  | offset:0x0709 */
    0x00, /*  | offset:0x070A */
    0x00, /*  | offset:0x070B */
    0x00, /*  | offset:0x070C */
    0x00, /*  | offset:0x070D */
    0x00, /*  | offset:0x070E */
    0x00, /*  | offset:0x070F */
    0x00, /*  | offset:0x0710 */
    0x00, /*  | offset:0x0711 */
    0x00, /*  | offset:0x0712 */
    0x00, /*  | offset:0x0713 */
    0x00, /*  | offset:0x0714 */
    0x00, /*  | offset:0x0715 */
    0x00, /*  | offset:0x0716 */
    0x00, /*  | offset:0x0717 */
    0x00, /*  | offset:0x0718 */
    0x00, /*  | offset:0x0719 */
    0x00, /*  | offset:0x071A */
    0x00, /*  | offset:0x071B */
    0x00, /*  | offset:0x071C */
    0x00, /*  | offset:0x071D */
    0x00, /*  | offset:0x071E */
    0x00, /*  | offset:0x071F */
    0x00, /*  | offset:0x0720 */
    0x00, /*  | offset:0x0721 */
    0x00, /*  | offset:0x0722 */
    0x00, /*  | offset:0x0723 */
    0x00, /*  | offset:0x0724 */
    0x00, /*  | offset:0x0725 */
    0x00, /*  | offset:0x0726 */
    0x00, /*  | offset:0x0727 */
    0x00, /*  | offset:0x0728 */
    0x00, /*  | offset:0x0729 */
    0x00, /*  | offset:0x072A */
    0x00, /*  | offset:0x072B */
    0x00, /*  | offset:0x072C */
    0x00, /*  | offset:0x072D */
    0x00, /*  | offset:0x072E */
    0x00, /*  | offset:0x072F */
    0x00, /*  | offset:0x0730 */
    0x00, /*  | offset:0x0731 */
    0x00, /*  | offset:0x0732 */
    0x00, /*  | offset:0x0733 */
    0x00, /*  | offset:0x0734 */
    0x00, /*  | offset:0x0735 */
    0x00, /*  | offset:0x0736 */
    0x00, /*  | offset:0x0737 */
    0x00, /*  | offset:0x0738 */
    0x00, /*  | offset:0x0739 */
    0x00, /*  | offset:0x073A */
    0x00, /*  | offset:0x073B */
    0x00, /*  | offset:0x073C */
    0x00, /*  | offset:0x073D */
    0x00, /*  | offset:0x073E */
    0x00, /*  | offset:0x073F */
    0x00, /*  | offset:0x0740 */
    0x00, /*  | offset:0x0741 */
    0x00, /*  | offset:0x0742 */
    0x00, /*  | offset:0x0743 */
    0x00, /*  | offset:0x0744 */
    0x00, /*  | offset:0x0745 */
    0x00, /*  | offset:0x0746 */
    0x00, /*  | offset:0x0747 */
    0x00, /*  | offset:0x0748 */
    0x00, /*  | offset:0x0749 */
    0x00, /*  | offset:0x074A */
    0x00, /*  | offset:0x074B */
    0x00, /*  | offset:0x074C */
    0x00, /*  | offset:0x074D */
    0x00, /*  | offset:0x074E */
    0x00, /*  | offset:0x074F */
    0x00, /*  | offset:0x0750 */
    0x00, /*  | offset:0x0751 */
    0x00, /*  | offset:0x0752 */
    0x00, /*  | offset:0x0753 */
    0x00, /*  | offset:0x0754 */
    0x00, /*  | offset:0x0755 */
    0x00, /*  | offset:0x0756 */
    0x00, /*  | offset:0x0757 */
    0x00, /*  | offset:0x0758 */
    0x00, /*  | offset:0x0759 */
    0x00, /*  | offset:0x075A */
    0x00, /*  | offset:0x075B */
    0x00, /*  | offset:0x075C */
    0x00, /*  | offset:0x075D */
    0x00, /*  | offset:0x075E */
    0x00, /*  | offset:0x075F */
    0x00, /*  | offset:0x0760 */
    0x00, /*  | offset:0x0761 */
    0x00, /*  | offset:0x0762 */
    0x00, /*  | offset:0x0763 */
    0x00, /*  | offset:0x0764 */
    0x00, /*  | offset:0x0765 */
    0x00, /*  | offset:0x0766 */
    0x00, /*  | offset:0x0767 */
    0x00, /*  | offset:0x0768 */
    0x00, /*  | offset:0x0769 */
    0x00, /*  | offset:0x076A */
    0x00, /*  | offset:0x076B */
    0x00, /*  | offset:0x076C */
    0x00, /*  | offset:0x076D */
    0x00, /*  | offset:0x076E */
    0x00, /*  | offset:0x076F */
    0x00, /*  | offset:0x0770 */
    0x00, /*  | offset:0x0771 */
    0x00, /*  | offset:0x0772 */
    0x00, /*  | offset:0x0773 */
    0x00, /*  | offset:0x0774 */
    0x00, /*  | offset:0x0775 */
    0x00, /*  | offset:0x0776 */
    0x00, /*  | offset:0x0777 */
    0x00, /*  | offset:0x0778 */
    0x00, /*  | offset:0x0779 */
    0x00, /*  | offset:0x077A */
    0x00, /*  | offset:0x077B */
    0x00, /*  | offset:0x077C */
    0x00, /*  | offset:0x077D */
    0x00, /*  | offset:0x077E */
    0x00, /*  | offset:0x077F */
    0x00, /*  | offset:0x0780 */
    0x00, /*  | offset:0x0781 */
    0x00, /*  | offset:0x0782 */
    0x00, /*  | offset:0x0783 */
    0x00, /*  | offset:0x0784 */
    0x00, /*  | offset:0x0785 */
    0x00, /*  | offset:0x0786 */
    0x00, /*  | offset:0x0787 */
    0x00, /*  | offset:0x0788 */
    0x00, /*  | offset:0x0789 */
    0x00, /*  | offset:0x078A */
    0x00, /*  | offset:0x078B */
    0x00, /*  | offset:0x078C */
    0x00, /*  | offset:0x078D */
    0x00, /*  | offset:0x078E */
    0x00, /*  | offset:0x078F */
    0x00, /*  | offset:0x0790 */
    0x00, /*  | offset:0x0791 */
    0x00, /*  | offset:0x0792 */
    0x00, /*  | offset:0x0793 */
    0x00, /*  | offset:0x0794 */
    0x00, /*  | offset:0x0795 */
    0x00, /*  | offset:0x0796 */
    0x00, /*  | offset:0x0797 */
    0x00, /*  | offset:0x0798 */
    0x00, /*  | offset:0x0799 */
    0x00, /*  | offset:0x079A */
    0x00, /*  | offset:0x079B */
    0x00, /*  | offset:0x079C */
    0x00, /*  | offset:0x079D */
    0x00, /*  | offset:0x079E */
    0x00, /*  | offset:0x079F */
    0x00, /*  | offset:0x07A0 */
    0x00, /*  | offset:0x07A1 */
    0x00, /*  | offset:0x07A2 */
    0x00, /*  | offset:0x07A3 */
    0x00, /*  | offset:0x07A4 */
    0x00, /*  | offset:0x07A5 */
    0x00, /*  | offset:0x07A6 */
    0x00, /*  | offset:0x07A7 */
    0x00, /*  | offset:0x07A8 */
    0x00, /*  | offset:0x07A9 */
    0x00, /*  | offset:0x07AA */
    0x00, /*  | offset:0x07AB */
    0x00, /*  | offset:0x07AC */
    0x00, /*  | offset:0x07AD */
    0x00, /*  | offset:0x07AE */
    0x00, /*  | offset:0x07AF */
    0x00, /*  | offset:0x07B0 */
    0x00, /*  | offset:0x07B1 */
    0x00, /*  | offset:0x07B2 */
    0x00, /*  | offset:0x07B3 */
    0x00, /*  | offset:0x07B4 */
    0x00, /*  | offset:0x07B5 */
    0x00, /*  | offset:0x07B6 */
    0x00, /*  | offset:0x07B7 */
    0x00, /*  | offset:0x07B8 */
    0x00, /*  | offset:0x07B9 */
    0x00, /*  | offset:0x07BA */
    0x00, /*  | offset:0x07BB */
    0x00, /*  | offset:0x07BC */
    0x00, /*  | offset:0x07BD */
    0x00, /*  | offset:0x07BE */
    0x00, /*  | offset:0x07BF */
    0x00, /*  | offset:0x07C0 */
    0x00, /*  | offset:0x07C1 */
    0x00, /*  | offset:0x07C2 */
    0x00, /*  | offset:0x07C3 */
    0x00, /*  | offset:0x07C4 */
    0x00, /*  | offset:0x07C5 */
    0x00, /*  | offset:0x07C6 */
    0x00, /*  | offset:0x07C7 */
    0x00, /*  | offset:0x07C8 */
    0x00, /*  | offset:0x07C9 */
    0x00, /*  | offset:0x07CA */
    0x00, /*  | offset:0x07CB */
    0x00, /*  | offset:0x07CC */
    0x00, /*  | offset:0x07CD */
    0x00, /*  | offset:0x07CE */
    0x00, /*  | offset:0x07CF */
    0x00, /*  | offset:0x07D0 */
    0x00, /*  | offset:0x07D1 */
    0x00, /*  | offset:0x07D2 */
    0x00, /*  | offset:0x07D3 */
    0x00, /*  | offset:0x07D4 */
    0x00, /*  | offset:0x07D5 */
    0x00, /*  | offset:0x07D6 */
    0x00, /*  | offset:0x07D7 */
    0x00, /*  | offset:0x07D8 */
    0x00, /*  | offset:0x07D9 */
    0x00, /*  | offset:0x07DA */
    0x00, /*  | offset:0x07DB */
    0x00, /*  | offset:0x07DC */
    0x00, /*  | offset:0x07DD */
    0x00, /*  | offset:0x07DE */
    0x00, /*  | offset:0x07DF */
    0x00, /*  | offset:0x07E0 */
    0x00, /*  | offset:0x07E1 */
    0x00, /*  | offset:0x07E2 */
    0x00, /*  | offset:0x07E3 */
    0x00, /*  | offset:0x07E4 */
    0x00, /*  | offset:0x07E5 */
    0x00, /*  | offset:0x07E6 */
    0x00, /*  | offset:0x07E7 */
    0x00, /*  | offset:0x07E8 */
    0x00, /*  | offset:0x07E9 */
    0x00, /*  | offset:0x07EA */
    0x00, /*  | offset:0x07EB */
    0x00, /*  | offset:0x07EC */
    0x00, /*  | offset:0x07ED */
    0x00, /*  | offset:0x07EE */
    0x00, /*  | offset:0x07EF */
    0x00, /*  | offset:0x07F0 */
    0x00, /*  | offset:0x07F1 */
    0x00, /*  | offset:0x07F2 */
    0x00, /*  | offset:0x07F3 */
    0x00, /*  | offset:0x07F4 */
    0x00, /*  | offset:0x07F5 */
    0x00, /*  | offset:0x07F6 */
    0x00, /*  | offset:0x07F7 */
    0x00, /*  | offset:0x07F8 */
    0x00, /*  | offset:0x07F9 */
    0x00, /*  | offset:0x07FA */
    0x00, /*  | offset:0x07FB */
    0x00, /*  | offset:0x07FC */
    0x00, /*  | offset:0x07FD */
    0x00, /*  | offset:0x07FE */
    0x00, /*  | offset:0x07FF */
};

WIFI_CUSTOM_PARAM_STRUCT stWifiCustomDefault =
{
    0x0, // Reserved
};


#endif
