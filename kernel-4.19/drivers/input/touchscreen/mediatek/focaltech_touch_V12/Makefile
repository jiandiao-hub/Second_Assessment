
# Linux driver folder
ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/
ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/$(CONFIG_TOUCHSCREEN_MTK_FTS_DIRECTORY)
ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/touchpanel/FT8716/
ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/focaltech_touch_V12/
ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/focaltech_touch_V12/include/
ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/touchpanel/FT8716/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(MTK_PLATFORM)/include/
ccflags-y += -I$(srctree)/drivers/base
ccflags-y += -I$(srctree)/drivers/misc/mediatek/hwmon/include
ccflags-y += -I$(srctree)/drivers/misc/mediatek/sensors-1.0/alsps/inc
ccflags-y += -I$(srctree)/drivers/misc/mediatek/sensors-1.0/alsps/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/sensors-1.0/hwmon/include/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/sensors-1.0/hwmon/hwmsen/

obj-$(CONFIG_TOUCHSCREEN_MTK_FOCALTECH_TS_V12)    +=  focaltech_ex_fun.o
obj-$(CONFIG_TOUCHSCREEN_MTK_FOCALTECH_TS_V12)    +=  focaltech_flash.o
obj-$(CONFIG_TOUCHSCREEN_MTK_FOCALTECH_TS_V12)    +=  focaltech_gesture.o
obj-$(CONFIG_TOUCHSCREEN_MTK_FOCALTECH_TS_V12)    +=  focaltech_core.o
obj-$(CONFIG_TOUCHSCREEN_MTK_FOCALTECH_TS_V12)    +=  focaltech_esdcheck.o
obj-$(CONFIG_TOUCHSCREEN_MTK_FOCALTECH_TS_V12)    +=  focaltech_ex_mode.o
obj-$(CONFIG_TOUCHSCREEN_MTK_FOCALTECH_TS_V12)    +=  focaltech_proximity.o
obj-$(CONFIG_TOUCHSCREEN_MTK_FOCALTECH_TS_V12)    +=  focaltech_i2c.o
obj-$(CONFIG_TOUCHSCREEN_MTK_FOCALTECH_TS_V12)    +=  focaltech_point_report_check.o

#obj-$(CONFIG_TOUCHSCREEN_MTK_FOCALTECH_TS_V12) 		+=  focaltech_test/
obj-$(CONFIG_TOUCHSCREEN_MTK_FOCALTECH_TS_V12)    +=  focaltech_flash/
