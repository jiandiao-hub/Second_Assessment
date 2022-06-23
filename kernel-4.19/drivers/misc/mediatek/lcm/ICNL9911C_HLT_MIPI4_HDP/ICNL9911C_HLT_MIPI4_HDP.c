#ifdef BUILD_LK
#include <platform/upmu_common.h>
#include <platform/mt_gpio.h>
#include <platform/mt_i2c.h>
#include <platform/mt_pmic.h>

#define Lcd_Log printf
#include <string.h>

#include <malloc.h>
#else
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/i2c.h>
#include <linux/irq.h>
/* #include <linux/jiffies.h> */
/* #include <linux/delay.h> */
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include "disp_dts_gpio.h"
#define Lcd_Log printk
#endif

#include "lcm_drv.h"
#include "../../video/mt6765/videox/ktd2151/ktd2151.h"
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH                                         (720)
#define FRAME_HEIGHT                                        (1560)

#define REGFLAG_DELAY           				(0XFFE)
#define REGFLAG_END_OF_TABLE    				(0xFFF)	// END OF REGISTERS MARKER

#define LCM_ID 												(0x9911)
#define LCM_DSI_CMD_MODE                                    0

#define GPIO_LCD_ID_PIN       (91 | 0x80000000)


// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static struct LCM_UTIL_FUNCS lcm_util = {0};


#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))


#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))

// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg											lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

struct LCM_setting_table {
    unsigned int cmd;
    unsigned char count;
    unsigned char para_list[120];
};

static struct LCM_setting_table lcm_initialization_setting[] =
{
{0xF0,2,{0x5A,0x59}},
{0xF1,2,{0xA5,0xA6}},
{0xB0,30,{0x82,0x81,0x05,0x04,0x87,0x86,0x84,0x85,0x66,0x66,0x33,0x33,0x20,0x01,0x01,0x78,0x01,0x01,0x0F,0x05,0x04,0x03,0x02,0x01,0x02,0x03,0x04,0x00,0x00,0x00}},
{0xB1,29,{0x11,0x44,0x86,0x00,0x01,0x00,0x01,0x7C,0x01,0x01,0x04,0x08,0x54,0x00,0x00,0x00,0x44,0x40,0x02,0x01,0x40,0x02,0x01,0x40,0x02,0x01,0x40,0x02,0x01}},
{0xB2,17,{0x54,0xC4,0x82,0x05,0x40,0x02,0x01,0x40,0x02,0x01,0x05,0x05,0x54,0x0C,0x0C,0x0D,0x0B}},
{0xB3,31,{0x02,0x00,0x00,0x00,0x00,0x26,0x26,0x91,0xA2,0x33,0x44,0x00,0x26,0x00,0x18,0x01,0x02,0x08,0x20,0x30,0x08,0x09,0x44,0x20,0x40,0x20,0x40,0x08,0x09,0x22,0x33}},
// Forword xscan
{0xB4,28,{0x0A,0x02,0xDC,0x1D,0x00,0x02,0x02,0x02,0x02,0x12,0x10,0x02,0x02,0x0E,0x0C,0x04,0x03,0x03,0x03,0x03,0x03,0x03,0xFF,0xFF,0xFC,0x00,0x00,0x00}},
{0xB5,28,{0x0B,0x02,0xDC,0x1D,0x00,0x02,0x02,0x02,0x02,0x13,0x11,0x02,0x02,0x0F,0x0D,0x05,0x03,0x03,0x03,0x03,0x03,0x03,0xFF,0xFF,0xFC,0x00,0x00,0x00}},
{0xB8,24,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
{0xBB,13,{0x01,0x05,0x09,0x11,0x0D,0x19,0x1D,0x55,0x25,0x69,0x00,0x21,0x25}},
{0xBC,14,{0x00,0x00,0x00,0x00,0x02,0x20,0xFF,0x00,0x03,0x33,0x01,0x73,0x33,0x02}},
{0xBD,10,{0xE9,0x02,0x4F,0xCF,0x72,0xA4,0x08,0x44,0xAE,0x15}},
{0xBE,10,{0x65,0x65,0x50,0x46,0x0C,0x77,0x43,0x07,0x0E,0x0E}},
{0xBF,8,{0x07,0x25,0x07,0x25,0x7F,0x00,0x11,0x04}},
{0xC0,9,{0x10,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0x00}},
{0xC1,19,{0xC0,0x20,0x20,0x96,0x04,0x60,0x60,0x04,0x2A,0x18,0x36,0x00,0x07,0xCF,0xFF,0xFF,0xC0,0x00,0xC0}},
{0xC2,1,{0x00}},
{0xC3,9,{0x06,0x00,0xFF,0x00,0xFF,0x00,0x00,0x81,0x01}},
{0xC4,10,{0x84,0x01,0x2B,0x41,0x00,0x3C,0x00,0x03,0x03,0x2E}},
{0xC5,11,{0x03,0x1C,0xC0,0xB8,0x50,0x10,0x64,0x44,0x08,0x09,0x26}},
{0xC6,10,{0x87,0x9A,0x2A,0x29,0x29,0x33,0x7F,0x04,0x08,0x00}},
{0xC7,22,{0xF7,0xBA,0x93,0x76,0x49,0x28,0xF9,0x50,0x1E,0xF6,0xCF,0xA3,0xFD,0xD2,0xB4,0x8B,0x71,0x4C,0x1A,0x7E,0xC0,0x00}},
{0xC8,22,{0xF7,0xBA,0x93,0x76,0x49,0x28,0xF9,0x50,0x1E,0xF6,0xCF,0xA3,0xFD,0xD2,0xB4,0x8B,0x71,0x4C,0x1A,0x7E,0xC0,0x00}},
{0xCB,1,{0x00}},
{0xD0,5,{0x80,0x0D,0xFF,0x0F,0x61}},
{0xD2,1,{0x42}},
{0xFE,4,{0xFF,0xFF,0xFF,0x40}},//64M
{0xF1,2,{0x5A,0x59}},
{0xF0,2,{0xA5,0xA6}},
{0x51,2,{0x0F,0xFF}},//百分百亮度
{0x53,1,{0x24}},//PWM TurnOn
{0x55,1,{0x02}},//Still Picture
{0x35,1,{0x00}},
{0x11,1,{0x00}},
{REGFLAG_DELAY, 100, {}},
{0x29,1,{0x00}},
{REGFLAG_DELAY, 20, {}},
{0x26,1,{0x01}},//出睡眠增加的代码
{REGFLAG_END_OF_TABLE, 0x00, {}}

};

static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] =
{
	// Display off sequence
	{0x26, 1,{0x03}},
	{REGFLAG_DELAY, 5, {}},
	{0x28, 0, {0x00}},
	{REGFLAG_DELAY, 50, {}},
	// Sleep Mode On
	{0x10, 0, {0x00}},
	{REGFLAG_DELAY, 120, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
    unsigned int i;

    for(i = 0; i < count; i++) {
		
        unsigned int cmd;
        cmd = table[i].cmd;

        switch (cmd) {

        case REGFLAG_DELAY :
            MDELAY(table[i].count);
            break;

        case REGFLAG_END_OF_TABLE :
            break;

        default:
            dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
        }
    }
}

// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------
static void lcm_set_util_funcs(const struct LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(struct LCM_UTIL_FUNCS));
}

static void lcm_get_params(struct LCM_PARAMS *params)
{
	memset(params, 0, sizeof(struct LCM_PARAMS));

    params->type   = LCM_TYPE_DSI;

    params->width  = FRAME_WIDTH;
    params->height = FRAME_HEIGHT;


    // enable tearing-free
    //params->dbi.te_mode                 = LCM_DBI_TE_MODE_DISABLED;  //LCM_DBI_TE_MODE_VSYNC_ONLY;
    //params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

#if (LCM_DSI_CMD_MODE)
    params->dsi.mode   = CMD_MODE;
#else
    params->dsi.mode   = SYNC_PULSE_VDO_MODE;
#endif
	
		// DSI
		/* Command mode setting */
	params->dsi.LANE_NUM				= LCM_FOUR_LANE;
		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

    // Highly depends on LCD driver capability.
    // Not support in MT6573
    params->dsi.packet_size = 256;

    // Video mode setting
    params->dsi.intermediat_buffer_num = 2;

	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

    params->dsi.vertical_sync_active				= 4;//8   4
    params->dsi.vertical_backporch					= 32;//20;//12  10
    params->dsi.vertical_frontporch					= 150;//28;//2  18
    params->dsi.vertical_active_line				= FRAME_HEIGHT;

    params->dsi.horizontal_sync_active				= 4;//8
    params->dsi.horizontal_backporch				= 96;//40;//30
    params->dsi.horizontal_frontporch				= 96;//40;//30
    params->dsi.horizontal_active_pixel			= FRAME_WIDTH;

    params->dsi.PLL_CLOCK = 304;//196;//160
    params->dsi.ssc_disable = true;


	params->dsi.esd_check_enable = 1; //enable ESD check
	params->dsi.customization_esd_check_enable = 1; //0 TE ESD CHECK  1 LCD REG CHECK
	params->dsi.lcm_esd_check_table[0].cmd            = 0x0A;
	params->dsi.lcm_esd_check_table[0].count        = 1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9C;
	params->dsi.noncont_clock                       = 1;
	
    params->physical_width=65;
    params->physical_height=140;
}

static void lcm_init(void)
{
	u8 addr;
	u8 val;
	
	Lcd_Log("ILI9881P_lcm %s %d \n",__FUNCTION__,__LINE__);
	gpio_direction_output(329+164,1);//LCD_V5P_EN
	gpio_direction_output(329+165,1);//LCD_V5N_EN	
	
	addr=0x00;
	val=0x14;
	ktd2151_write_reg(ktd2151_client, addr , val);
 
 	addr=0x01;
	val=0x14;
	ktd2151_write_reg(ktd2151_client, addr , val);
	
    SET_RESET_PIN(1);
	MDELAY(10);
    SET_RESET_PIN(0);
    MDELAY(20);
    SET_RESET_PIN(1);
    MDELAY(120);

    push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_init_power(void)
{
	pr_info("ILI9881P_lcm %s %d \n",__FUNCTION__,__LINE__);
}

static void lcm_suspend(void)
{
	Lcd_Log("ILI9881P_lcm %s %d \n",__FUNCTION__,__LINE__);
    push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting)/sizeof(struct LCM_setting_table),1);
	

    MDELAY(120);

	
	gpio_direction_output(329+164,0);//LCD_V5P_EN
	gpio_direction_output(329+165,0);//LCD_V5N_EN
	
}

static void lcm_resume(void)
{
    lcm_init();
	Lcd_Log("ILI9881P_lcm %s %d \n",__FUNCTION__,__LINE__);
	
	//push_table(lcm_deep_sleep_mode_out_setting, sizeof(lcm_deep_sleep_mode_out_setting)/sizeof(struct LCM_setting_table),1);
}


// static int get_lcd_id(void)
// {
	// Lcd_Log("ILI9881P_lcm %s %d \n",__FUNCTION__,__LINE__);
// #if defined(BUILD_LK)
    // mt_set_gpio_mode(GPIO_LCD_ID_PIN, 0);
    // mt_set_gpio_dir(GPIO_LCD_ID_PIN, 0);
    //mt_set_gpio_pull_enable(GPIO_LCD_ID_PIN,1);
    //mt_set_gpio_pull_select(GPIO_LCD_ID_PIN,0);
    // MDELAY(5);

	// Lcd_Log("ILI9881P_lcm : ILI9881P %s, %d\n", __func__, mt_get_gpio_in(GPIO_LCD_ID_PIN));
    // return mt_get_gpio_in(GPIO_LCD_ID_PIN);

// #endif
// }


static unsigned int lcm_compare_id(void)
{
	// unsigned int id = 0,id_high = 0,id_low = 0;

	// unsigned char buffer[2];
	// unsigned int array[16];
	// struct LCM_setting_table switch_table_page1[] = {
		// { 0xFF, 0x03, {0x98, 0x81, 0x06} }
	// };
	// struct LCM_setting_table switch_table_page0[] = {
		// { 0xFF, 0x03, {0x98, 0x81, 0x00} }
	// };

	// SET_RESET_PIN(1);
	// SET_RESET_PIN(0);
	// MDELAY(1);

	// SET_RESET_PIN(1);
	// MDELAY(20);

	// push_table(switch_table_page1, sizeof(switch_table_page1) / sizeof(struct LCM_setting_table), 1);

	// array[0] = 0x00023700;	/* read id return two byte,version and id */
	// dsi_set_cmdq(array, 1, 1);

	// read_reg_v2(0xF0, buffer, 1);
	// id_high = buffer[0]; 	/* we only need ID */

	// read_reg_v2(0xF1, buffer, 1);
	// id_low = buffer[0];

	// printf("%s,ili9881c_id_high=0x%08x,id_low=0x%x\n", __func__, id_high, id_low);

	// push_table(switch_table_page0, sizeof(switch_table_page0) / sizeof(struct LCM_setting_table), 1);
	
	// id = (id_high << 8) | id_low;
	// Lcd_Log("[ %s %d]ILI9881P_RUIHENG_PANDA_ZJJ055117_MIPI4_FWVGA_PLUS  id:0x%08x \n",__FUNCTION__,__LINE__, id);
	
	// if(id == LCM_ID)
	// {
		// Lcd_Log("[ %s %d]ILI9881P_RUIHENG_PANDA_ZJJ055117_MIPI4_FWVGA_PLUS  id:0x%08x \n",__FUNCTION__,__LINE__, id);
		// return 1;
	// }
	// else
		// return 0;

	return 1;
}

#ifdef CONFIG_AGENEW_LCM_ATA_CHECK
extern unsigned int lcm_ata_check_id;
static unsigned int lcm_ata_check(unsigned char *buffer)
{
         Lcd_Log("mycat lcm_ata_check lcm_ata_check_id =0x%x\n", lcm_ata_check_id);
		 Lcd_Log("mycat lcm_ata_check LCM_ID =0x%x\n", LCM_ID);
			return ((LCM_ID == lcm_ata_check_id))?1:0;
}
#endif//CONFIG_AGENEW_LCM_ATA_CHECK
struct LCM_DRIVER ICNL9911C_HLT_MIPI4_HDP_lcm_drv =
{
    .name			= "ICNL9911C_HLT_MIPI4_HDP",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params     = lcm_get_params,
    .init           = lcm_init,
	.init_power		= lcm_init_power,
    .suspend        = lcm_suspend,
    .resume         = lcm_resume,
    .compare_id     = lcm_compare_id,
#ifdef CONFIG_AGENEW_LCM_ATA_CHECK
    .ata_check	    = lcm_ata_check,
#endif//CONFIG_AGENEW_LCM_ATA_CHECK
};
