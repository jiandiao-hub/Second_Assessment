
#ifndef __STK3x2x_H__
#define __STK3x2x_H__
#include "cust_alsps.h"
#include <FreeRTOS.h>
#include <semphr.h>
#include "alsps.h"

#define STK3321_ALS_ENABLE
#define STK3321_PS_ENABLE
//#define ALS_MID_FIR
#define STK3321_VERSION "2.0.0"

#define STK_IRS
#ifdef STK_IRS
uint32_t als_comperator_A = 49;
uint32_t als_comperator_D = 15;
uint32_t als_comperator_HZ = 70;
#endif // STK_IRS

#ifdef STK3321_PS_ENABLE
#define STK3321_PS_OC_REPORT
#endif

typedef enum
{
    STK3310SA_PID = 0x17,
    STK3311SA_PID = 0x1E,
    STK3311WV_PID = 0x1D,
} stk3321_sensor_pid;

#ifdef ALS_PS_SEPARATE
    #undef STK3321_ALS_ENABLE
#endif

#ifdef ALS_MID_FIR
    #ifndef STK3321_ALS_ENABLE
        #undef ALS_MID_FIR
    #endif
#endif

#if (defined(STK_TUNE0) && defined(STK3321_PS_ENABLE))
    #define STK3321_TRACKING_QUANTI         4
    #define STK3321_QUANTI_RANGE            10
    #define STK3321_SMUDGE_DIFF             800
    #define STK3321_SMUDGE_NT               300
    #define STK3321_SMUDGE_FT               200
    #define STK3321_MAX_MIN_DIFF            200
    #define STK3321_LT_N_CT                 170
    #define STK3321_HT_N_CT                 220
#endif // STK_TUNE0

#ifdef ALS_MID_FIR
#define STK_ALS_MID_FIR_LEN              5
#define MAX_ALS_FIR_LEN                  32

typedef struct
{
    uint16_t raw[STK_ALS_MID_FIR_LEN];
    uint32_t number;
    uint32_t index;
} STK3321_data_filter;
#endif // ALS_MID_FIR

/*ALSPS REGS*/
#define STK_STATE_REG                   0x00
#define STK_PSCTRL_REG                  0x01
#define STK_ALSCTRL_REG                 0x02
#define STK_LEDCTRL_REG                 0x03
#define STK_INTCTRL_REG                 0x04
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
#define STK_DATA1_IR                    0x17
#define STK_DATA2_IR                    0x18
#define STK_PDT_ID_REG                  0x3E
#define STK_RSRVD_REG                   0xDB
#define STK_SW_RESET_REG                0x80

/*********************************************
* Register control                           *
*********************************************/

/* Define state reg */
#define STK_STATE_EN_WAIT_SHIFT                         2
#define STK_STATE_EN_ALS_SHIFT                          1
#define STK_STATE_EN_PS_SHIFT                           0
#define STK_STATE_EN_IRS_MASK                           0x80
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
#define STK_LED_IRDR_SHIFT              5
#define STK_LED_IRDR_MASK               0xE0

/* Define interrupt-1 reg */
#define STK_INT_CTRL_SHIFT              7
#define STK_INT_ALS_SHIFT               3
#define STK_INT_PS_SHIFT                0

#define STK_INT_CTRL_MASK               0x80
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
#define STK_FLG_ALSSAT_SHIFT            2
#define STK_FLG_INVALID_PS_SHIFT        1
#define STK_FLG_NF_SHIFT                0

#define STK_FLG_ALSDR_MASK              0x80
#define STK_FLG_PSDR_MASK               0x40
#define STK_FLG_ALSINT_MASK             0x20
#define STK_FLG_PSINT_MASK              0x10
#define STK_FLG_OUI_MASK                0x04
#define STK_FLG_IR_RDY_MASK             0x02
#define STK_FLG_NF_MASK                 0x01

/* Define pdctrl reg */
#define STK_PDCTRL_ALS_SEL_SHIFT        4
#define STK_PDCTRL_PS_SEL_SHIFT         0
#define STK_PDCTRL_ALS_SEL_MASK         0x70
#define STK_PDCTRL_PS_SEL_MASK          0x0F

/*********************************************
* Parameters control                         *
*********************************************/
#define STK_PS_PRS1                     0x00
#define STK_PS_PRS4                     0x01
#define STK_PS_GAIN1                    0x00
#define STK_PS_GAIN4                    0x01
#define STK_PS_GAIN16                   0x02
#define STK_PS_GAIN64                   0x03
#define STK_PS_IT200                    0x00
#define STK_PS_IT400                    0x01
#define STK_PS_IT800                    0x02

#define STK_ALS_PRS1                    0x00
#define STK_ALS_PRS4                    0x01
#define STK_ALS_GAIN1                   0x00
#define STK_ALS_GAIN4                   0x01
#define STK_ALS_GAIN16                  0x02
#define STK_ALS_GAIN64                  0x03
#define STK_ALS_IT50                    0x08
#define STK_ALS_IT100                   0x09

#define STK_LED_12_5mA                  0x00
#define STK_LED_25mA                    0x40
#define STK_LED_50mA                    0x80
#define STK_LED_100mA                   0xC0
#define STK_LED_DT_64_64                0x3F

#define STK_WAIT_20                     0x02
#define STK_WAIT_50                     0x07
#define STK_WAIT_100                    0x0F

/* Register  default Value define */
#define STK_PSCTRL_0X_64X               0x32
#define STK_ALSCTRL_0X_64X              0x31
#define STK_WAIT_50MS                   0x20
#define STK_WAIT_RST                    0x7F
#define STK_INT_CTRL                    0x01
#define STK_INT_CTRL2                   0x00
#define STK_SPEC1_ADDR                  0x10
#define STK_SPEC2_ADDR                  0x64

#ifdef STK_IRS
	#define STK_IRC_MIN_ALS_CODE		5
	#define STK_IRC_MAX_ALS_CODE		60000
	#define STK_IRC_MIN_IR_CODE			1
	#define STK_IRC_ALS_DENOMI			1
	#define STK_IRC_ALS_NUMERA			10
	#define STK_IRC_ALS_CORREC			1000
#endif

#ifdef STK3321_ALS_ENABLE
    void stk_low_pass_fir(uint16_t *raw);
#endif
#ifdef STK3321_PS_ENABLE
    static void psSetThreshold(uint32_t threshold_high, uint32_t threshold_low);
    static void stk3321_ps_set_thd(uint32_t threshold_high, uint32_t threshold_low);
#endif
#endif

