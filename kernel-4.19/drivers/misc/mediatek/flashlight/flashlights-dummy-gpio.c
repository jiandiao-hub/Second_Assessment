/*
 * Copyright (C) 2015 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": %s: " fmt, __func__

#include <linux/types.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/workqueue.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/list.h>
#include <linux/delay.h>
#include <linux/pinctrl/consumer.h>
#ifdef CONFIG_AGENEW_SGM3785_PWM_CONTROL
#include <mt-plat/mtk_pwm.h>
#endif
#include "flashlight-core.h"
#include "flashlight-dt.h"

/* define device tree */
/* TODO: modify temp device tree name */
#ifndef DUMMY_GPIO_DTNAME
#define DUMMY_GPIO_DTNAME "mediatek,flashlights_dummy_gpio"
#endif

/* TODO: define driver name */
#define DUMMY_NAME "flashlights-dummy-gpio"

/* define registers */
/* TODO: define register */

/* define mutex and work queue */
static DEFINE_MUTEX(dummy_mutex);
static struct work_struct dummy_work;

/* define pinctrl */
/* TODO: define pinctrl */
#define DUMMY_PINCTRL_PIN_TORCH 0
#define DUMMY_PINCTRL_PIN_FLASH 1

//add
#if defined(CONFIG_CHARGER_LED_CTL)
#define DUMMY_PINCTRL_PIN_LED_GREEN 2
#define DUMMY_PINCTRL_PIN_LED_BLUE 3
#if defined(CONFIG_CHARGER_LED_CTL_SCAN)
#define DUMMY_PINCTRL_PIN_LED_RED_SCAN 4
#define DUMMY_PINCTRL_PIN_LED_GREEN_SCAN 5
#define DUMMY_PINCTRL_PIN_LED_BLUE_SCAN 6
#endif
#endif
#define DUMMY_PINCTRL_PINSTATE_LOW 0
#define DUMMY_PINCTRL_PINSTATE_HIGH 1
#define DUMMY_PINCTRL_STATE_TORCH_HIGH "torch_high"
#define DUMMY_PINCTRL_STATE_TORCH_LOW  "torch_low"
//add
#define DUMMY_PINCTRL_STATE_FLASH_HIGH "flash_high"
#define DUMMY_PINCTRL_STATE_FLASH_LOW  "flash_low"

//add
#if defined(CONFIG_CHARGER_LED_CTL)
#define DUMMY_PINCTRL_STATE_GREEN_LED_HIGH "green_led_high"
#define DUMMY_PINCTRL_STATE_GREEN_LED_LOW  "green_led_low"
#define DUMMY_PINCTRL_STATE_BLUE_LED_HIGH "blue_led_high"
#define DUMMY_PINCTRL_STATE_BLUE_LED_LOW  "blue_led_low"
#if defined(CONFIG_CHARGER_LED_CTL_SCAN)	
#define DUMMY_PINCTRL_STATE_RED_SCAN_LED_HIGH "red_scan_led_high"
#define DUMMY_PINCTRL_STATE_RED_SCAN_LED_LOW  "red_scan_led_low"
#define DUMMY_PINCTRL_STATE_GREEN_SCAN_LED_HIGH "green_scan_led_high"
#define DUMMY_PINCTRL_STATE_GREEN_SCAN_LED_LOW  "green_scan_led_low"
#define DUMMY_PINCTRL_STATE_BLUE_SCAN_LED_HIGH "blue_scan_led_high"
#define DUMMY_PINCTRL_STATE_BLUE_SCAN_LED_LOW  "blue_scan_led_low"
#endif
#endif
static struct pinctrl *dummy_pinctrl;
static struct pinctrl_state *dummy_torch_high;
static struct pinctrl_state *dummy_torch_low;
//add
static struct pinctrl_state *dummy_flash_high;
static struct pinctrl_state *dummy_flash_low;

//add
#if defined(CONFIG_CHARGER_LED_CTL)
static struct pinctrl_state *dummy_green_led_high;
static struct pinctrl_state *dummy_green_led_low;
static struct pinctrl_state *dummy_blue_led_high;
static struct pinctrl_state *dummy_blue_led_low;
#if defined(CONFIG_CHARGER_LED_CTL_SCAN)	
static struct pinctrl_state *dummy_red_scan_led_high;
static struct pinctrl_state *dummy_red_scan_led_low;
static struct pinctrl_state *dummy_green_scan_led_high;
static struct pinctrl_state *dummy_green_scan_led_low;
static struct pinctrl_state *dummy_blue_scan_led_high;
static struct pinctrl_state *dummy_blue_scan_led_low;

#endif
#endif

#if defined(CONFIG_BIRD_SUB_FLASH_EN_GPIO_SUPPORT)   
#define DUMMY_PINCTRL_PIN_SUB_FLASH_EN 2
#define DUMMY_PINCTRL_STATE_SUB_FLASH_EN_HIGH "sub_flashlight_en_high"
#define DUMMY_PINCTRL_STATE_SUB_FLASH_EN_LOW  "sub_flashlight_en_low"
static struct pinctrl_state *dummy_sub_flash_en_high;
static struct pinctrl_state *dummy_sub_flash_en_low;
#endif
static int g_channel = -1; //global channel //change in dummy_ioctl() function
/* define usage count */
static int use_count;
static int g_duty = -1;

/* platform data */
struct dummy_platform_data {
	int channel_num;
	struct flashlight_device_id *dev_id;
};


/******************************************************************************
 * Pinctrl configuration
 *****************************************************************************/
static int dummy_pinctrl_init(struct platform_device *pdev)
{
	int ret = 0;

	/* get pinctrl */
	dummy_pinctrl = devm_pinctrl_get(&pdev->dev);
	if (IS_ERR(dummy_pinctrl)) {
		pr_err("Failed to get flashlight pinctrl.\n");
		ret = PTR_ERR(dummy_pinctrl);
		return ret;
	}

	/* TODO: Flashlight XXX pin initialization */
	dummy_torch_high = pinctrl_lookup_state(dummy_pinctrl, DUMMY_PINCTRL_STATE_TORCH_HIGH);
	if (IS_ERR(dummy_torch_high)) {
		pr_err("Failed to init (%s)\n", DUMMY_PINCTRL_STATE_TORCH_HIGH);
		ret = PTR_ERR(dummy_torch_high);
	}
	dummy_torch_low = pinctrl_lookup_state(dummy_pinctrl, DUMMY_PINCTRL_STATE_TORCH_LOW);
	if (IS_ERR(dummy_torch_low)) {
		pr_err("Failed to init (%s)\n", DUMMY_PINCTRL_STATE_TORCH_LOW);
		ret = PTR_ERR(dummy_torch_low);
	}
	//add
	dummy_flash_high = pinctrl_lookup_state(dummy_pinctrl, DUMMY_PINCTRL_STATE_FLASH_HIGH);
	if (IS_ERR(dummy_flash_high)) {
		pr_err("Failed to init (%s)\n", DUMMY_PINCTRL_STATE_FLASH_HIGH);
		ret = PTR_ERR(dummy_flash_high);
	}
	dummy_flash_low = pinctrl_lookup_state(dummy_pinctrl, DUMMY_PINCTRL_STATE_FLASH_LOW);
	if (IS_ERR(dummy_flash_low)) {
		pr_err("Failed to init (%s)\n", DUMMY_PINCTRL_STATE_FLASH_LOW);
		ret = PTR_ERR(dummy_flash_low);
	}
#if defined(CONFIG_BIRD_SUB_FLASH_EN_GPIO_SUPPORT)   
	dummy_sub_flash_en_high = pinctrl_lookup_state(dummy_pinctrl, DUMMY_PINCTRL_STATE_SUB_FLASH_EN_HIGH);//wdong
	if (IS_ERR(dummy_sub_flash_en_high)) {
		pr_err("Failed to init (%s)\n", DUMMY_PINCTRL_STATE_SUB_FLASH_EN_HIGH);
		ret = PTR_ERR(dummy_sub_flash_en_high);
	}
	dummy_sub_flash_en_low = pinctrl_lookup_state(dummy_pinctrl, DUMMY_PINCTRL_STATE_SUB_FLASH_EN_LOW);
	if (IS_ERR(dummy_sub_flash_en_low)) {
		pr_err("Failed to init (%s)\n", DUMMY_PINCTRL_STATE_SUB_FLASH_EN_LOW);
		ret = PTR_ERR(dummy_sub_flash_en_low);
	}
#endif

	//add
#if defined(CONFIG_CHARGER_LED_CTL)
	dummy_green_led_high = pinctrl_lookup_state(dummy_pinctrl, DUMMY_PINCTRL_STATE_GREEN_LED_HIGH);
	if (IS_ERR(dummy_green_led_high)) {
		pr_err("Failed to init (%s)\n", DUMMY_PINCTRL_STATE_GREEN_LED_HIGH);
		ret = PTR_ERR(dummy_green_led_high);
	}
	dummy_green_led_low = pinctrl_lookup_state(dummy_pinctrl, DUMMY_PINCTRL_STATE_GREEN_LED_LOW);
	if (IS_ERR(dummy_green_led_low)) {
		pr_err("Failed to init (%s)\n", DUMMY_PINCTRL_STATE_GREEN_LED_LOW);
		ret = PTR_ERR(dummy_green_led_low);
	}
	
	dummy_blue_led_high = pinctrl_lookup_state(dummy_pinctrl, DUMMY_PINCTRL_STATE_BLUE_LED_HIGH);
	if (IS_ERR(dummy_blue_led_high)) {
		pr_err("Failed to init (%s)\n", DUMMY_PINCTRL_STATE_BLUE_LED_HIGH);
		ret = PTR_ERR(dummy_blue_led_high);
	}
	dummy_blue_led_low = pinctrl_lookup_state(dummy_pinctrl, DUMMY_PINCTRL_STATE_BLUE_LED_LOW);
	if (IS_ERR(dummy_blue_led_low)) {
		pr_err("Failed to init (%s)\n", DUMMY_PINCTRL_STATE_BLUE_LED_LOW);
		ret = PTR_ERR(dummy_blue_led_low);
	}
#if defined(CONFIG_CHARGER_LED_CTL_SCAN)	
	dummy_red_scan_led_high = pinctrl_lookup_state(dummy_pinctrl, DUMMY_PINCTRL_STATE_RED_SCAN_LED_HIGH);
	if (IS_ERR(dummy_red_scan_led_high)) {
		pr_err("Failed to init (%s)\n", DUMMY_PINCTRL_STATE_RED_SCAN_LED_HIGH);
		ret = PTR_ERR(dummy_red_scan_led_high);
	}
	dummy_red_scan_led_low = pinctrl_lookup_state(dummy_pinctrl, DUMMY_PINCTRL_STATE_RED_SCAN_LED_LOW);
	if (IS_ERR(dummy_red_scan_led_low)) {
		pr_err("Failed to init (%s)\n", DUMMY_PINCTRL_STATE_RED_SCAN_LED_LOW);
		ret = PTR_ERR(dummy_red_scan_led_low);
	}
	
	dummy_green_scan_led_high = pinctrl_lookup_state(dummy_pinctrl, DUMMY_PINCTRL_STATE_GREEN_SCAN_LED_HIGH);
	if (IS_ERR(dummy_green_scan_led_high)) {
		pr_err("Failed to init (%s)\n", DUMMY_PINCTRL_STATE_GREEN_SCAN_LED_HIGH);
		ret = PTR_ERR(dummy_green_scan_led_high);
	}
	dummy_green_scan_led_low = pinctrl_lookup_state(dummy_pinctrl, DUMMY_PINCTRL_STATE_GREEN_SCAN_LED_LOW);
	if (IS_ERR(dummy_green_scan_led_low)) {
		pr_err("Failed to init (%s)\n", DUMMY_PINCTRL_STATE_GREEN_SCAN_LED_LOW);
		ret = PTR_ERR(dummy_green_scan_led_low);
	}
	
	dummy_blue_scan_led_high = pinctrl_lookup_state(dummy_pinctrl, DUMMY_PINCTRL_STATE_BLUE_SCAN_LED_HIGH);
	if (IS_ERR(dummy_blue_scan_led_high)) {
		pr_err("Failed to init (%s)\n", DUMMY_PINCTRL_STATE_BLUE_SCAN_LED_HIGH);
		ret = PTR_ERR(dummy_blue_scan_led_high);
	}
	dummy_blue_scan_led_low = pinctrl_lookup_state(dummy_pinctrl, DUMMY_PINCTRL_STATE_BLUE_SCAN_LED_LOW);
	if (IS_ERR(dummy_blue_scan_led_low)) {
		pr_err("Failed to init (%s)\n", DUMMY_PINCTRL_STATE_BLUE_SCAN_LED_LOW);
		ret = PTR_ERR(dummy_blue_scan_led_low);
	}
#endif
#endif

	return ret;
}

static int dummy_pinctrl_set(int pin, int state)
{
	int ret = 0;

	if (IS_ERR(dummy_pinctrl)) {
		pr_err("pinctrl is not available\n");
		return -1;
	}

	switch (pin) {
	case DUMMY_PINCTRL_PIN_TORCH:
		if (state == DUMMY_PINCTRL_PINSTATE_LOW && !IS_ERR(dummy_torch_low))
			pinctrl_select_state(dummy_pinctrl, dummy_torch_low);
		else if (state == DUMMY_PINCTRL_PINSTATE_HIGH && !IS_ERR(dummy_torch_high))
			pinctrl_select_state(dummy_pinctrl, dummy_torch_high);
		else
			pr_err("set err, pin(%d) state(%d)\n", pin, state);
		break;
	case DUMMY_PINCTRL_PIN_FLASH:
		if (state == DUMMY_PINCTRL_PINSTATE_LOW && !IS_ERR(dummy_flash_low))
			pinctrl_select_state(dummy_pinctrl, dummy_flash_low);
		else if (state == DUMMY_PINCTRL_PINSTATE_HIGH && !IS_ERR(dummy_flash_high))
			pinctrl_select_state(dummy_pinctrl, dummy_flash_high);
		else
			pr_err("set err, pin(%d) state(%d)\n", pin, state);
		break;
#if defined(CONFIG_BIRD_SUB_FLASH_EN_GPIO_SUPPORT)  
#pragma message("CONFIG_BIRD_SUB_FLASH_EN_GPIO_SUPPORT is defined!!!\n") 
	case DUMMY_PINCTRL_PIN_SUB_FLASH_EN:
		if (state == DUMMY_PINCTRL_PINSTATE_LOW && !IS_ERR(dummy_sub_flash_en_low))
			pinctrl_select_state(dummy_pinctrl, dummy_sub_flash_en_low);
		else if (state == DUMMY_PINCTRL_PINSTATE_HIGH && !IS_ERR(dummy_sub_flash_en_high))
			pinctrl_select_state(dummy_pinctrl, dummy_sub_flash_en_high);
		else
			pr_err("set err, pin(%d) state(%d)\n", pin, state);
		break;
#endif
#if defined(CONFIG_CHARGER_LED_CTL)
	case DUMMY_PINCTRL_PIN_LED_GREEN:
		if (state == DUMMY_PINCTRL_PINSTATE_LOW && !IS_ERR(dummy_green_led_low))
			pinctrl_select_state(dummy_pinctrl, dummy_green_led_low);
		else if (state == DUMMY_PINCTRL_PINSTATE_HIGH && !IS_ERR(dummy_green_led_high))
			pinctrl_select_state(dummy_pinctrl, dummy_green_led_high);
		else
			pr_err("set err, pin(%d) state(%d)\n", pin, state);
		break;
		
	case DUMMY_PINCTRL_PIN_LED_BLUE:
		if (state == DUMMY_PINCTRL_PINSTATE_LOW && !IS_ERR(dummy_blue_led_low))
			pinctrl_select_state(dummy_pinctrl, dummy_blue_led_low);
		else if (state == DUMMY_PINCTRL_PINSTATE_HIGH && !IS_ERR(dummy_blue_led_high))
			pinctrl_select_state(dummy_pinctrl, dummy_blue_led_high);
		else
			pr_err("set err, pin(%d) state(%d)\n", pin, state);
		break;
#if defined(CONFIG_CHARGER_LED_CTL_SCAN)		
	case DUMMY_PINCTRL_PIN_LED_RED_SCAN:
		if (state == DUMMY_PINCTRL_PINSTATE_LOW && !IS_ERR(dummy_red_scan_led_low))
			pinctrl_select_state(dummy_pinctrl, dummy_red_scan_led_low);
		else if (state == DUMMY_PINCTRL_PINSTATE_HIGH && !IS_ERR(dummy_red_scan_led_high))
			pinctrl_select_state(dummy_pinctrl, dummy_red_scan_led_high);
		else
			pr_err("set err, pin(%d) state(%d)\n", pin, state);
		break;
		
	case DUMMY_PINCTRL_PIN_LED_GREEN_SCAN:
		if (state == DUMMY_PINCTRL_PINSTATE_LOW && !IS_ERR(dummy_green_scan_led_low))
			pinctrl_select_state(dummy_pinctrl, dummy_green_scan_led_low);
		else if (state == DUMMY_PINCTRL_PINSTATE_HIGH && !IS_ERR(dummy_green_scan_led_high))
			pinctrl_select_state(dummy_pinctrl, dummy_green_scan_led_high);
		else
			pr_err("set err, pin(%d) state(%d)\n", pin, state);
		break;
		
	case DUMMY_PINCTRL_PIN_LED_BLUE_SCAN:
		if (state == DUMMY_PINCTRL_PINSTATE_LOW && !IS_ERR(dummy_blue_scan_led_low))
			pinctrl_select_state(dummy_pinctrl, dummy_blue_scan_led_low);
		else if (state == DUMMY_PINCTRL_PINSTATE_HIGH && !IS_ERR(dummy_blue_scan_led_high))
			pinctrl_select_state(dummy_pinctrl, dummy_blue_scan_led_high);
		else
			pr_err("set err, pin(%d) state(%d)\n", pin, state);
		break;
#endif
#endif
	default:
		pr_err("set err, pin(%d) state(%d)\n", pin, state);
		break;
	}
	pr_info("pin(%d) state(%d)\n", pin, state);

	return ret;
}

#ifdef CONFIG_AGENEW_SGM3785_PWM_CONTROL
void agenew_pinctrl_pin_torch_pwm_mode(int pwm_num,int width,int thresh,int on_off)
{
	struct pwm_spec_config pwm_setting;
	
	printk("[PWM%d] TEST:OLD CLK SOURCE 26M\n", pwm_num);
	memset(&pwm_setting, 0, sizeof(struct pwm_spec_config));
	pwm_setting.pwm_no = pwm_num;
	pwm_setting.mode = PWM_MODE_OLD;
	pwm_setting.clk_src = PWM_CLK_OLD_MODE_32K;
	pwm_setting.pmic_pad = 0;
	if(on_off)
	{
		pwm_setting.PWM_MODE_OLD_REGS.THRESH = thresh;
		pwm_setting.clk_div = CLK_DIV32;
		pwm_setting.PWM_MODE_OLD_REGS.DATA_WIDTH = width;
	}
	else
	{
		pwm_setting.PWM_MODE_OLD_REGS.THRESH = 0;
		pwm_setting.clk_div = CLK_DIV32;
		pwm_setting.PWM_MODE_OLD_REGS.DATA_WIDTH = 100 / 2;
	}
	pwm_setting.PWM_MODE_FIFO_REGS.IDLE_VALUE = 0;
	pwm_setting.PWM_MODE_FIFO_REGS.GUARD_VALUE = 0;
	pwm_setting.PWM_MODE_FIFO_REGS.GDURATION = 0;
	pwm_setting.PWM_MODE_FIFO_REGS.WAVE_NUM = 0;
	pwm_set_spec_config(&pwm_setting);
}
#endif

/******************************************************************************
 * dummy operations
 *****************************************************************************/
#if defined(CONFIG_CHARGER_LED_CTL)
int led_color=0;
int dummy_charger_led_enable(unsigned int  level)
{
	/* TODO: wrap enable function */
	if(led_color == 0)
	{
			if(level==0)
			{
					dummy_pinctrl_set(DUMMY_PINCTRL_PIN_LED_GREEN, 0);//enable
			}
			else
			{
					dummy_pinctrl_set(DUMMY_PINCTRL_PIN_LED_GREEN, 1);//enable
			}
	}
	else if(led_color == 1)
	{
			if(level==0)
			{
					dummy_pinctrl_set(DUMMY_PINCTRL_PIN_LED_BLUE, 0);//enable
			}
			else
			{
					dummy_pinctrl_set(DUMMY_PINCTRL_PIN_LED_BLUE, 1);//enable
			}
	}
#if defined(CONFIG_CHARGER_LED_CTL_SCAN)
	else if(led_color == 2)
	{
			if(level==0)
			{
					dummy_pinctrl_set(DUMMY_PINCTRL_PIN_LED_RED_SCAN, 0);//enable
			}
			else
			{
					dummy_pinctrl_set(DUMMY_PINCTRL_PIN_LED_RED_SCAN, 1);//enable
			}
	}else if(led_color == 3)
	{
			if(level==0)
			{
					dummy_pinctrl_set(DUMMY_PINCTRL_PIN_LED_GREEN_SCAN, 0);//enable
			}
			else
			{
					dummy_pinctrl_set(DUMMY_PINCTRL_PIN_LED_GREEN_SCAN, 1);//enable
			}
	}
	else if(led_color == 4)
	{
			if(level==0)
			{
					dummy_pinctrl_set(DUMMY_PINCTRL_PIN_LED_BLUE_SCAN, 0);//enable
			}
			else
			{
					dummy_pinctrl_set(DUMMY_PINCTRL_PIN_LED_BLUE_SCAN, 1);//enable
			}
	}
#endif
	else
		pr_err("led_color no find!");
	return 0;	
}
#endif

/* flashlight enable function */
static int dummy_enable(void)
{
	//int pin = 0, state = 1;

	/* TODO: wrap enable function */
	pr_info("dummy_enable() g_channel:%d, g_duty: %d\n", g_channel, g_duty);
	switch (g_channel) {
#if defined(CONFIG_BIRD_SUB_FLASH_EN_GPIO_SUPPORT)   
	case BIRD_SUB_FLASH_EN_CHANNEL: //flashlight-device.c const struct flashlight_device_id flashlight_id[]  device channel
			dummy_pinctrl_set(DUMMY_PINCTRL_PIN_SUB_FLASH_EN, 1);//enable
#if defined(CONFIG_BIRD_SUB_FLASH_COMPATIBLE_GPIO_ISINK1_SUPPORT)
			mt_brightness_set_pmic(MT65XX_LED_PMIC_NLED_ISINK1,1,0);
#endif
		break;
#endif
	default:
#ifdef CONFIG_AGENEW_SGM3785_PWM_CONTROL
	if(g_duty==0)
	{
			dummy_pinctrl_set(DUMMY_PINCTRL_PIN_TORCH, 1);//enable
			dummy_pinctrl_set(DUMMY_PINCTRL_PIN_FLASH, 0);	
			agenew_pinctrl_pin_torch_pwm_mode(0,100,100,1);
			mdelay(5);
			agenew_pinctrl_pin_torch_pwm_mode(0,100,20,1);
			
	}
	else
	{
			dummy_pinctrl_set(DUMMY_PINCTRL_PIN_TORCH, 1);//enable
			agenew_pinctrl_pin_torch_pwm_mode(0,100,40,1);
			dummy_pinctrl_set(DUMMY_PINCTRL_PIN_FLASH, 1);
	}
#else
	if(g_duty==0)
	{
	#ifdef CONFIG_H231_FLASHLIGHT_CURRENT
			dummy_pinctrl_set(DUMMY_PINCTRL_PIN_TORCH, 0);//enable
			dummy_pinctrl_set(DUMMY_PINCTRL_PIN_FLASH, 1);	
			mdelay(1);
			dummy_pinctrl_set(DUMMY_PINCTRL_PIN_TORCH, 0);//enable
			dummy_pinctrl_set(DUMMY_PINCTRL_PIN_FLASH, 0);	
			mdelay(1);
			dummy_pinctrl_set(DUMMY_PINCTRL_PIN_TORCH, 0);//enable
			dummy_pinctrl_set(DUMMY_PINCTRL_PIN_FLASH, 1);	
			mdelay(1);
			dummy_pinctrl_set(DUMMY_PINCTRL_PIN_TORCH, 0);//enable
			dummy_pinctrl_set(DUMMY_PINCTRL_PIN_FLASH, 0);
			mdelay(1);
			dummy_pinctrl_set(DUMMY_PINCTRL_PIN_TORCH, 0);//enable
			dummy_pinctrl_set(DUMMY_PINCTRL_PIN_FLASH, 1);
	#else
					dummy_pinctrl_set(DUMMY_PINCTRL_PIN_TORCH, 1);//enable
			dummy_pinctrl_set(DUMMY_PINCTRL_PIN_FLASH, 0);	
	#endif
	}
	else
	{

#if defined CONFIG_AGENEW_FLASHLIGHT_AW3641_IC
		dummy_pinctrl_set(DUMMY_PINCTRL_PIN_FLASH, 1);
		dummy_pinctrl_set(DUMMY_PINCTRL_PIN_TORCH, 1);
		{
			u8 i=CONFIG_AGENEW_FLASHLIGHT_AW3641_IC_PULSE;
			printk("CONFIG_AGENEW_FLASHLIGHT_AW3641_IC_PULSE =%d\n",i);
			while(i--){
				udelay(8);
				dummy_pinctrl_set(DUMMY_PINCTRL_PIN_TORCH, 0);
				udelay(8);
				dummy_pinctrl_set(DUMMY_PINCTRL_PIN_TORCH, 1);
			}
		}
#else
		#ifdef CONFIG_H231_FLASHLIGHT_CURRENT
			dummy_pinctrl_set(DUMMY_PINCTRL_PIN_FLASH, 0);
			dummy_pinctrl_set(DUMMY_PINCTRL_PIN_TORCH, 1);//enable
			mdelay(1);
			dummy_pinctrl_set(DUMMY_PINCTRL_PIN_TORCH, 0);//enable
			mdelay(1);
			dummy_pinctrl_set(DUMMY_PINCTRL_PIN_TORCH, 1);//enable
			mdelay(1);
			dummy_pinctrl_set(DUMMY_PINCTRL_PIN_TORCH, 0);//enable
			mdelay(1);
			dummy_pinctrl_set(DUMMY_PINCTRL_PIN_TORCH, 1);//enable
			#else
			dummy_pinctrl_set(DUMMY_PINCTRL_PIN_TORCH, 1);//enable
			dummy_pinctrl_set(DUMMY_PINCTRL_PIN_FLASH, 1);
		
	#endif	
#endif
	}
#endif
	}
	return 0;
}

/* flashlight disable function */
static int dummy_disable(void)
{
	//int pin = 0, state = 0;

	/* TODO: wrap disable function */
	pr_info("dummy_disable() g_channel:%d, g_duty: %d\n", g_channel, g_duty);
#ifdef CONFIG_AGENEW_SGM3785_PWM_CONTROL
	agenew_pinctrl_pin_torch_pwm_mode(0,0,0,0);
#endif
	dummy_pinctrl_set(DUMMY_PINCTRL_PIN_TORCH, 0);//enable
	dummy_pinctrl_set(DUMMY_PINCTRL_PIN_FLASH, 0);
#if defined(CONFIG_BIRD_SUB_FLASH_EN_GPIO_SUPPORT)   
	dummy_pinctrl_set(DUMMY_PINCTRL_PIN_SUB_FLASH_EN, 0);
#if defined(CONFIG_BIRD_SUB_FLASH_COMPATIBLE_GPIO_ISINK1_SUPPORT)
			mt_brightness_set_pmic(MT65XX_LED_PMIC_NLED_ISINK1,0,0);
#endif
#endif
	return 0;
}

/* set flashlight level */
static int dummy_set_level(int level)
{
	//int pin = 0, state = 0;

	/* TODO: wrap set level function */
	g_duty = level;
	return 0;
}

/* flashlight init */
static int dummy_init(void)
{
	//int pin = 0, state = 0;

	/* TODO: wrap init function */

	dummy_pinctrl_set(DUMMY_PINCTRL_PIN_TORCH, 0);//enable
	dummy_pinctrl_set(DUMMY_PINCTRL_PIN_FLASH, 0);
#if defined(CONFIG_BIRD_SUB_FLASH_EN_GPIO_SUPPORT)   
	dummy_pinctrl_set(DUMMY_PINCTRL_PIN_SUB_FLASH_EN, 0);
#if defined(CONFIG_BIRD_SUB_FLASH_COMPATIBLE_GPIO_ISINK1_SUPPORT)
			mt_brightness_set_pmic(MT65XX_LED_PMIC_NLED_ISINK1,0,0);
#endif
#endif
	return 0;
}

/* flashlight uninit */
static int dummy_uninit(void)
{
	//int pin = 0, state = 0;

	/* TODO: wrap init function */

	dummy_pinctrl_set(DUMMY_PINCTRL_PIN_TORCH, 0);//enable
	dummy_pinctrl_set(DUMMY_PINCTRL_PIN_FLASH, 0);
#if defined(CONFIG_BIRD_SUB_FLASH_EN_GPIO_SUPPORT)   
	dummy_pinctrl_set(DUMMY_PINCTRL_PIN_SUB_FLASH_EN, 0);
#if defined(CONFIG_BIRD_SUB_FLASH_COMPATIBLE_GPIO_ISINK1_SUPPORT)
			mt_brightness_set_pmic(MT65XX_LED_PMIC_NLED_ISINK1,0,0);
#endif
#endif
	return 0;
}

/******************************************************************************
 * Timer and work queue
 *****************************************************************************/
static struct hrtimer dummy_timer;
static unsigned int dummy_timeout_ms;

static void dummy_work_disable(struct work_struct *data)
{
	pr_info("work queue callback\n");
	dummy_disable();
}

static enum hrtimer_restart dummy_timer_func(struct hrtimer *timer)
{
	schedule_work(&dummy_work);
	return HRTIMER_NORESTART;
}


/******************************************************************************
 * Flashlight operations
 *****************************************************************************/
static int dummy_ioctl(unsigned int cmd, unsigned long arg)
{
	struct flashlight_dev_arg *fl_arg;
	int channel;
	ktime_t ktime;
	unsigned int s;
	unsigned int ns;

	fl_arg = (struct flashlight_dev_arg *)arg;
	channel = fl_arg->channel;
	g_channel = channel; //channel = fl_arg->channel;

	switch (cmd) {
	case FLASH_IOC_SET_TIME_OUT_TIME_MS:
		pr_info("FLASH_IOC_SET_TIME_OUT_TIME_MS(%d): %d\n",
				channel, (int)fl_arg->arg);
		dummy_timeout_ms = fl_arg->arg;
		break;

	case FLASH_IOC_SET_DUTY:
		pr_info("FLASH_IOC_SET_DUTY(%d): %d\n",
				channel, (int)fl_arg->arg);
		dummy_set_level(fl_arg->arg);
		break;

	case FLASH_IOC_SET_ONOFF:
		pr_info("FLASH_IOC_SET_ONOFF(%d): %d\n",
				channel, (int)fl_arg->arg);
		if (fl_arg->arg == 1) {
			if (dummy_timeout_ms) {
				s = dummy_timeout_ms / 1000;
				ns = dummy_timeout_ms % 1000 * 1000000;
				ktime = ktime_set(s, ns);
				hrtimer_start(&dummy_timer, ktime,
						HRTIMER_MODE_REL);
			}
			dummy_enable();
		} else {
			dummy_disable();
			hrtimer_cancel(&dummy_timer);
		}
		break;
	default:
		pr_info("No such command and arg(%d): (%d, %d)\n",
				channel, _IOC_NR(cmd), (int)fl_arg->arg);
		return -ENOTTY;
	}

	return 0;
}

static int dummy_open(void)
{
	/* Move to set driver for saving power */
	return 0;
}

static int dummy_release(void)
{
	/* Move to set driver for saving power */
	return 0;
}

static int dummy_set_driver(int set)
{
	int ret = 0;

	/* set chip and usage count */
	mutex_lock(&dummy_mutex);
	if (set) {
		if (!use_count)
			ret = dummy_init();
		use_count++;
		pr_info("Set driver: %d\n", use_count);
	} else {
		use_count--;
		if (!use_count)
			ret = dummy_uninit();
		if (use_count < 0)
			use_count = 0;
		pr_info("Unset driver: %d\n", use_count);
	}
	mutex_unlock(&dummy_mutex);

	return ret;
}

static ssize_t dummy_strobe_store(struct flashlight_arg arg)
{
	dummy_set_driver(1);
	dummy_set_level(arg.level);
	dummy_timeout_ms = 0;
	dummy_enable();
	msleep(arg.dur);
	dummy_disable();
	dummy_set_driver(0);

	return 0;
}

static struct flashlight_operations dummy_ops = {
	dummy_open,
	dummy_release,
	dummy_ioctl,
	dummy_strobe_store,
	dummy_set_driver
};


/******************************************************************************
 * Platform device and driver
 *****************************************************************************/
static int dummy_chip_init(void)
{
	/* NOTE: Chip initialication move to "set driver" for power saving.
	 * dummy_init();
	 */

	return 0;
}

static int dummy_parse_dt(struct device *dev,
		struct dummy_platform_data *pdata)
{
	struct device_node *np, *cnp;
	u32 decouple = 0;
	int i = 0;

	if (!dev || !dev->of_node || !pdata)
		return -ENODEV;

	np = dev->of_node;

	pdata->channel_num = of_get_child_count(np);
	if (!pdata->channel_num) {
		pr_info("Parse no dt, node.\n");
		return 0;
	}
	pr_info("Channel number(%d).\n", pdata->channel_num);

	if (of_property_read_u32(np, "decouple", &decouple))
		pr_info("Parse no dt, decouple.\n");

	pdata->dev_id = devm_kzalloc(dev,
			pdata->channel_num *
			sizeof(struct flashlight_device_id),
			GFP_KERNEL);
	if (!pdata->dev_id)
		return -ENOMEM;

	for_each_child_of_node(np, cnp) {
		if (of_property_read_u32(cnp, "type", &pdata->dev_id[i].type))
			goto err_node_put;
		if (of_property_read_u32(cnp, "ct", &pdata->dev_id[i].ct))
			goto err_node_put;
		if (of_property_read_u32(cnp, "part", &pdata->dev_id[i].part))
			goto err_node_put;
		snprintf(pdata->dev_id[i].name, FLASHLIGHT_NAME_SIZE,
				DUMMY_NAME);
		pdata->dev_id[i].channel = i;
		pdata->dev_id[i].decouple = decouple;

		pr_info("Parse dt (type,ct,part,name,channel,decouple)=(%d,%d,%d,%s,%d,%d).\n",
				pdata->dev_id[i].type, pdata->dev_id[i].ct,
				pdata->dev_id[i].part, pdata->dev_id[i].name,
				pdata->dev_id[i].channel,
				pdata->dev_id[i].decouple);
		i++;
	}

	return 0;

err_node_put:
	of_node_put(cnp);
	return -EINVAL;
}

static int dummy_probe(struct platform_device *pdev)
{
	struct dummy_platform_data *pdata = dev_get_platdata(&pdev->dev);
	int err;
	int i;

	pr_info("Probe start.\n");

	/* init pinctrl */
	if (dummy_pinctrl_init(pdev)) {
		pr_info("Failed to init pinctrl.\n");
		err = -EFAULT;
		goto err;
	}

	/* init platform data */
	if (!pdata) {
		pdata = devm_kzalloc(&pdev->dev, sizeof(*pdata), GFP_KERNEL);
		if (!pdata) {
			err = -ENOMEM;
			goto err;
		}
		pdev->dev.platform_data = pdata;
		err = dummy_parse_dt(&pdev->dev, pdata);
		if (err)
			goto err;
	}

	/* init work queue */
	INIT_WORK(&dummy_work, dummy_work_disable);

	/* init timer */
	hrtimer_init(&dummy_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	dummy_timer.function = dummy_timer_func;
	dummy_timeout_ms = 100;

	/* init chip hw */
	dummy_chip_init();

	/* clear usage count */
	use_count = 0;

	/* register flashlight device */
	if (pdata->channel_num) {
		for (i = 0; i < pdata->channel_num; i++)
			if (flashlight_dev_register_by_device_id(
						&pdata->dev_id[i],
						&dummy_ops)) {
				err = -EFAULT;
				goto err;
			}
	} else {
		if (flashlight_dev_register(DUMMY_NAME, &dummy_ops)) {
			err = -EFAULT;
			goto err;
		}
	}

	pr_info("Probe done.\n");

	return 0;
err:
	return err;
}

static int dummy_remove(struct platform_device *pdev)
{
	struct dummy_platform_data *pdata = dev_get_platdata(&pdev->dev);
	int i;

	pr_info("Remove start.\n");

	pdev->dev.platform_data = NULL;

	/* unregister flashlight device */
	if (pdata && pdata->channel_num)
		for (i = 0; i < pdata->channel_num; i++)
			flashlight_dev_unregister_by_device_id(
					&pdata->dev_id[i]);
	else
		flashlight_dev_unregister(DUMMY_NAME);

	/* flush work queue */
	flush_work(&dummy_work);

	pr_info("Remove done.\n");

	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id dummy_gpio_of_match[] = {
	{.compatible = DUMMY_GPIO_DTNAME},
	{},
};
MODULE_DEVICE_TABLE(of, dummy_gpio_of_match);
#else
static struct platform_device dummy_gpio_platform_device[] = {
	{
		.name = DUMMY_NAME,
		.id = 0,
		.dev = {}
	},
	{}
};
MODULE_DEVICE_TABLE(platform, dummy_gpio_platform_device);
#endif

static struct platform_driver dummy_platform_driver = {
	.probe = dummy_probe,
	.remove = dummy_remove,
	.driver = {
		.name = DUMMY_NAME,
		.owner = THIS_MODULE,
#ifdef CONFIG_OF
		.of_match_table = dummy_gpio_of_match,
#endif
	},
};

static int __init flashlight_dummy_init(void)
{
	int ret;

	pr_info("Init start.\n");

#ifndef CONFIG_OF
	ret = platform_device_register(&dummy_gpio_platform_device);
	if (ret) {
		pr_err("Failed to register platform device\n");
		return ret;
	}
#endif

	ret = platform_driver_register(&dummy_platform_driver);
	if (ret) {
		pr_err("Failed to register platform driver\n");
		return ret;
	}

	pr_info("Init done.\n");

	return 0;
}

static void __exit flashlight_dummy_exit(void)
{
	pr_info("Exit start.\n");

	platform_driver_unregister(&dummy_platform_driver);

	pr_info("Exit done.\n");
}

module_init(flashlight_dummy_init);
module_exit(flashlight_dummy_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Simon Wang <Simon-TCH.Wang@mediatek.com>");
MODULE_DESCRIPTION("MTK Flashlight DUMMY GPIO Driver");

