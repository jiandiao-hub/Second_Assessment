
#ifndef __STK3X3X_H__
#define __STK3X3X_H__
#include "cust_alsps.h"
#include <FreeRTOS.h>
#include <semphr.h>

/*ALSPS REGS*/
#define STK_STATE_REG                   0x00
#define STK_PSCTRL_REG                  0x01
#define STK_ALSCTRL_REG                 0x02
#define STK_LEDCTRL_REG                 0x03
#define STK_INTCTRL1_REG                0x04
#define STK_WAIT_REG                    0x05
#define STK_THDH1_PS_REG                0x06
#define STK_THDH2_PS_REG                0x07
#define STK_THDL1_PS_REG                0x08
#define STK_THDL2_PS_REG                0x09
#define STK_THDH1_ALS_REG               0x0A
#define STK_THDH2_ALS_REG               0x0B
#define STK_THDL1_ALS_REG               0x0C
#define STK_THDL2_ALS_REG               0x0D
#define STK_FLAG_REG                    0x10
#define STK_DATA1_PS_REG                0x11
#define STK_DATA2_PS_REG                0x12
#define STK_DATA1_ALS_REG               0x13
#define STK_DATA2_ALS_REG               0x14
#define STK_DATA1_OFFSET_REG            0x15
#define STK_DATA2_OFFSET_REG            0x16
#define STK_DATA1_IR_REG                0x17
#define STK_DATA2_IR_REG                0x18

#define STK_PDT_ID_REG                  0x3E
#define STK_RESERVED5_REG               0x3F
#define STK_ALSCTRL2_REG                0x4E
#define STK_INTELLI_WAIT_PS_REG         0x4F
#define STK_SW_RESET_REG                0x80
#define STK_BGIR_REG                    0xA0
#define STK_PD_REG                      0xA1
#define STK_AGAIN_REG                   0xDB
#define STK_SPEC1_REG                   0xFA

/* Register  default Value define */
#define STK_PSCTRL_VAL                  0xB1
#define STK_ALSCTRL_VAL                 0x72
#define STK_WAIT_50MS                   0x1F
#define STK_WAIT_RST                    0x7F
#define STK_INT_CTRL                    0x01
#define STK_INT_CTRL2                   0x00
#define STK_SPEC1_VAL                   0x01

#define STK3311SA_PID                   0x1E
#define STK3310SA_PID                   0x17

/*********************************************
* Register control                           *
*********************************************/

/* Define state reg */
#define STK_STATE_EN_WAIT_SHIFT                         2
#define STK_STATE_EN_ALS_SHIFT                          1
#define STK_STATE_EN_PS_SHIFT                           0

#define STK_STATE_EN_INTELL_PRST_MASK                   0x08
#define STK_STATE_EN_WAIT_MASK                          0x04
#define STK_STATE_EN_ALS_MASK                           0x02
#define STK_STATE_EN_PS_MASK                            0x01

/* Define PS ctrl reg */
#define STK_PS_PRST_SHIFT               6
#define STK_PS_GAIN_SHIFT               4
#define STK_PS_IT_SHIFT                 0

#define STK_PS_PRS_MASK                 0xC0
#define STK_PS_GAIN_MASK                0x30
#define STK_PS_IT_MASK                  0x0F

/* Define ALS ctrl-1 reg */
#define STK_ALS_PRST_SHIFT              6
#define STK_ALS_GAIN_SHIFT              4
#define STK_ALS_IT_SHIFT                0

#define STK_ALS_PRS_MASK                0xC0
#define STK_ALS_GAIN_MASK               0x30
#define STK_ALS_IT_MASK                 0x0F

/* Define LED ctrl reg */
#define STK_LED_12_5MA                  0x40
#define STK_LED_25MA                    0x60


/* Define interrupt-1 reg */
#define STK_INT_CTRL_SHIFT              7
#define STK_INT_INVALID_PS_SHIFT        5
#define STK_INT_ALS_SHIFT               3
#define STK_INT_PS_SHIFT                0

#define STK_INT_CTRL_MASK               0x80
#define STK_INT_OUI_MASK                0x20
#define STK_INT_ALS_MASK                0x08
#define STK_INT_PS_MASK                 0x07

#define STK_INT_PS_MODE1                0x01
#define STK_INT_PS_MODE3                0x03
#define STK_INT_PS_MODE4                0x04
#define STK_INT_ALS                     0x08

/* Define flag reg */
#define STK_FLG_ALSDR_SHIFT             7
#define STK_FLG_PSDR_SHIFT              6
#define STK_FLG_ALSINT_SHIFT            5
#define STK_FLG_PSINT_SHIFT             4
#define STK_FLG_OUI_SHIFT               2
#define STK_FLG_IR_RDY_SHIFT            1
#define STK_FLG_NF_SHIFT                0

#define STK_FLG_ALSDR_MASK              0x80
#define STK_FLG_PSDR_MASK               0x40
#define STK_FLG_ALSINT_MASK             0x20
#define STK_FLG_PSINT_MASK              0x10
#define STK_FLG_OUI_MASK                0x04
#define STK_FLG_IR_RDY_MASK             0x02
#define STK_FLG_NF_MASK                 0x01

/* Define ALS ctrl-2 reg */
#define STK_C_GAIN_SHIFT                4
#define STK_C_GAIN_MASK                 0x30

/* Define interrupt-2 reg */
#define STK_INT_ALS_DR_SHIFT            1
#define STK_INT_PS_DR_SHIFT             0

#define STK_INT_ALS_DR_MASK             0x02
#define STK_INT_PS_DR_MASK              0x01

/* Define flag2 reg */
#define STK_FLG2_INT_GS_SHIFT           6
#define STK_FLG2_GS10_SHIFT             5
#define STK_FLG2_GS01_SHIFT             4

#define STK_FLG2_INT_GS_MASK            0x40
#define STK_FLG2_GS10_MASK              0x20
#define STK_FLG2_GS01_MASK              0x10

#endif
