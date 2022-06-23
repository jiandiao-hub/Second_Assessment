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

#define LCM_ID                                              0x9881
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
	{0xFF, 3,{0x98, 0x81, 0x01}},
	{0x00, 1,{0x46}},
	{0x01, 1,{0x16}},
	{0x02, 1,{0x10}},
	{0x03, 1,{0x10}},
	{0x04, 1,{0xCC}},
	{0x05, 1,{0x13}},
	{0x08, 1,{0x80}},
	{0x09, 1,{0x12}},
	{0x0a, 1,{0x71}},
	{0x0b, 1,{0x00}},
	{0x14, 1,{0x8A}},
	{0x15, 1,{0x8A}},
	{0x0c, 1,{0x10}},
	{0x0d, 1,{0x10}},
	{0x0e, 1,{0x00}},
	{0x0f, 1,{0x00}},
	{0x10, 1,{0x01}},
	{0x11, 1,{0x01}},
	{0x12, 1,{0x01}},
	{0x24, 1,{0x00}},
	{0x25, 1,{0x09}},
	{0x26, 1,{0x10}},
	{0x27, 1,{0x10}},
	{0x2D, 1,{0x60}},
	{0x31, 1,{0x21}},
	{0x32, 1,{0x07}},
	{0x33, 1,{0x01}},
	{0x34, 1,{0x00}},
	{0x35, 1,{0x02}},
	{0x36, 1,{0x07}},
	{0x37, 1,{0x07}},
	{0x38, 1,{0x07}},
	{0x39, 1,{0x07}},
	{0x3a, 1,{0x17}},
	{0x3b, 1,{0x15}},
	{0x3c, 1,{0x07}},
	{0x3d, 1,{0x07}},
	{0x3e, 1,{0x13}},
	{0x3f, 1,{0x11}},
	{0x40, 1,{0x09}},
	{0x41, 1,{0x07}},
	{0x42, 1,{0x07}},
	{0x43, 1,{0x07}},
	{0x44, 1,{0x07}},
	{0x45, 1,{0x07}},
	{0x46, 1,{0x07}},
	{0x47, 1,{0x20}},
	{0x48, 1,{0x07}},
	{0x49, 1,{0x01}},
	{0x4a, 1,{0x00}},
	{0x4b, 1,{0x02}},
	{0x4c, 1,{0x07}},
	{0x4d, 1,{0x07}},
	{0x4e, 1,{0x07}},
	{0x4f, 1,{0x07}},
	{0x50, 1,{0x16}},
	{0x51, 1,{0x14}},
	{0x52, 1,{0x07}},
	{0x53, 1,{0x07}},
	{0x54, 1,{0x12}},
	{0x55, 1,{0x10}},
	{0x56, 1,{0x08}},
	{0x57, 1,{0x07}},
	{0x58, 1,{0x07}},
	{0x59, 1,{0x07}},
	{0x5a, 1,{0x07}},
	{0x5b, 1,{0x07}},
	{0x5c, 1,{0x07}},
	{0x61, 1,{0x08}},
	{0x62, 1,{0x07}},
	{0x63, 1,{0x01}},
	{0x64, 1,{0x00}},
	{0x65, 1,{0x02}},
	{0x66, 1,{0x07}},
	{0x67, 1,{0x07}},
	{0x68, 1,{0x07}},
	{0x69, 1,{0x07}},
	{0x6a, 1,{0x10}},
	{0x6b, 1,{0x12}},
	{0x6c, 1,{0x07}},
	{0x6d, 1,{0x07}},
	{0x6e, 1,{0x14}},
	{0x6f, 1,{0x16}},
	{0x70, 1,{0x20}},
	{0x71, 1,{0x07}},
	{0x72, 1,{0x07}},
	{0x73, 1,{0x07}},
	{0x74, 1,{0x07}},
	{0x75, 1,{0x07}},
	{0x76, 1,{0x07}},
	{0x77, 1,{0x09}},
	{0x78, 1,{0x07}},
	{0x79, 1,{0x01}},
	{0x7a, 1,{0x00}},
	{0x7b, 1,{0x02}},
	{0x7c, 1,{0x07}},
	{0x7d, 1,{0x07}},
	{0x7e, 1,{0x07}},
	{0x7f, 1,{0x07}},
	{0x80, 1,{0x11}},
	{0x81, 1,{0x13}},
	{0x82, 1,{0x07}},
	{0x83, 1,{0x07}},
	{0x84, 1,{0x15}},
	{0x85, 1,{0x17}},
	{0x86, 1,{0x21}},
	{0x87, 1,{0x07}},
	{0x88, 1,{0x07}},
	{0x89, 1,{0x07}},
	{0x8a, 1,{0x07}},
	{0x8b, 1,{0x07}},
	{0x8c, 1,{0x07}},
	{0xA0, 1,{0x01}},
	{0xA1, 1,{0x10}},
	{0xA2, 1,{0x08}},
	{0xA5, 1,{0x10}},
	{0xA6, 1,{0x10}},
	{0xA7, 1,{0x00}},
	{0xA8, 1,{0x00}},
	{0xA9, 1,{0x09}},
	{0xAa, 1,{0x09}},
	{0xb9, 1,{0x40}},
	{0xd0, 1,{0x01}},
	{0xd1, 1,{0x00}},
	{0xdc, 1,{0x35}},
	{0xdd, 1,{0x42}},
	{0xe2, 1,{0x00}},
	{0xe6, 1,{0x22}},
	{0xe7, 1,{0x54}},
	{0xF9, 1,{0x02}},
	{0xFF, 3,{0x98, 0x81, 0x05}},
	{0x58, 1,{0x62}},
	{0x63, 1,{0x78}},
	{0x64, 1,{0x7A}},
	{0x68, 1,{0xAA}},
	{0x69, 1,{0xB1}},
	{0x6A, 1,{0x86}},
	{0x6B, 1,{0x78}},
	{0xFF, 3,{0x98, 0x81, 0x06}},
	{0x0F, 1,{0x40}},
	{0x11, 1,{0x03}},
	{0x13, 1,{0x54}},
	{0x14, 1,{0x41}},
	{0x15, 1,{0x01}},
	{0x16, 1,{0x41}},
	{0x17, 1,{0xFF}},
	{0x18, 1,{0x00}},
	{0x48, 1,{0x0F}},
	{0x4D, 1,{0x80}},
	{0x4E, 1,{0x40}},
	{0xFF, 3,{0x98, 0x81, 0x08}},
	{0xE0, 27,{0x00, 0x24, 0x6D, 0x9E, 0xDC, 0x55, 0x0E, 0x34, 0x5F, 0x83, 0xA5, 0xBB, 0xE6, 0x0E, 0x34, 0xAA, 0x5D, 0x91, 0xB2, 0xDD, 0xFF, 0x02, 0x30, 0x6A, 0x99, 0x03, 0xFF}},
	{0xE1, 27,{0x00, 0x24, 0x6D, 0x9E, 0xDC, 0x55, 0x0E, 0x34, 0x5F, 0x83, 0xA5, 0xBB, 0xE6, 0x0E, 0x34, 0xAA, 0x5D, 0x91, 0xB2, 0xDD, 0xFF, 0x02, 0x30, 0x6A, 0x99, 0x03, 0xFF}},
	{0xFF, 3,{0x98, 0x81, 0x06}},
	{0xD6, 1,{0x85}},
	{0x27, 1,{0xFF}},
	{0x28, 1,{0x20}},
	{0x2E, 1,{0x01}},
	{0xC0, 1,{0x0B}},
	{0xC1, 1,{0x03}},
	{0xC2, 1,{0x04}},
	{0xDD, 1,{0x18}},
	{0xFF, 3,{0x98, 0x81, 0x0E}},
	{0x00, 1,{0xA0}},
	{0x01, 1,{0x28}},
	{0x11, 1,{0x90}},
	{0x13, 1,{0x14}},
	{0xFF, 3,{0x98, 0x81, 0x02}},
	{0x02, 1,{0x0A}},
	{0x01, 1,{0x75}},
	{0x40, 1,{0x46}},
	{0x42, 1,{0x00}},
	{0x4A, 1,{0x08}},
	{0x4D, 1,{0x4E}},
	{0x4E, 1,{0x00}},
	{0x1A, 1,{0x48}},
	{0x02, 1,{0x90}},
	{0xFF, 3,{0x98, 0x81, 0x07}},
	{0x0F, 1,{0x02}},
	{0xFF, 3,{0x98, 0x81, 0x04}},
	{0xD0, 1,{0x04}},
	{0xFF, 3,{0x98, 0x81, 0x03}},
	{0x83, 1,{0x60}},
	{0x84, 1,{0x03}},
	{0xFF, 3,{0x98, 0x81, 0x00}},
	{0x35, 1,{0x00}},


	{0x11,1,{0x00}},
	{REGFLAG_DELAY, 100, {}}, 
	{0x29,1,{0x00}},
	{REGFLAG_DELAY, 20, {}}, 
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] =
{
    // Display off sequence
    {0x28, 0, {0x00}},
    {REGFLAG_DELAY, 120, {}},
	
    // Sleep Mode On
    {0x10, 0, {0x00}},
    {REGFLAG_DELAY, 120, {}},//20
    {REGFLAG_END_OF_TABLE, 0x00, {}},
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

    params->dsi.vertical_sync_active				= 1;//8   4
    params->dsi.vertical_backporch					= 32;//20;//12  10
    params->dsi.vertical_frontporch					= 240;//28;//2  18
    params->dsi.vertical_active_line				= FRAME_HEIGHT;

    params->dsi.horizontal_sync_active				= 20;//8
    params->dsi.horizontal_backporch				= 65;//40;//30
    params->dsi.horizontal_frontporch				= 84;//40;//30
    params->dsi.horizontal_active_pixel			= FRAME_WIDTH;

    params->dsi.PLL_CLOCK = 300;//196;//160
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
	gpio_direction_output(329+164,1);						//LCD_V5P_EN
	gpio_direction_output(329+165,1);						//LCD_V5N_EN
	
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

extern int ilitek_tddi_sleep_handler(int mode);

static void lcm_suspend(void)
{
	Lcd_Log("ILI9881P_lcm %s %d \n",__FUNCTION__,__LINE__);
    
	if (ilitek_tddi_sleep_handler(1) < 0)
		Lcd_Log("ILI9881P_lcm %s %d tp sleep failed \n",__FUNCTION__,__LINE__);
	
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
	// Lcd_Log("[ %s %d] 9881 id:0x%08x \n",__FUNCTION__,__LINE__, id);
	
	// if(id == LCM_ID)
	// {
		// Lcd_Log("[ %s %d] 9881 id:0x%08x \n",__FUNCTION__,__LINE__, id);
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
struct LCM_DRIVER ILI9881H_HY_MIPI4_HDP_lcm_drv =
{
    .name			= "ILI9881H_HY_MIPI4_HDP",
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
