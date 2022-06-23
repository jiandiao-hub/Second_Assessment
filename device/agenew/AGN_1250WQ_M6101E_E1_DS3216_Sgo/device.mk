PRODUCT_COPY_FILES += $(LOCAL_PATH)/factory_init.project.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/hw/factory_init.project.rc
PRODUCT_COPY_FILES += $(LOCAL_PATH)/init.project.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/hw/init.project.rc
PRODUCT_COPY_FILES += $(LOCAL_PATH)/meta_init.project.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/hw/meta_init.project.rc

DEVICE_PACKAGE_OVERLAYS += $(LOCAL_PATH)/overlay

PRODUCT_PROPERTY_OVERRIDES += ro.sf.lcd_density=320

# audio codec priority OMX > C2
PRODUCT_PROPERTY_OVERRIDES += debug.stagefright.omx_default_rank.sw-audio=4

# touch related file for CTS
ifeq ($(strip $(CUSTOM_KERNEL_TOUCHPANEL)),generic)
  PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.touchscreen.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.touchscreen.xml
else
  PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.faketouch.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.faketouch.xml
  PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.touchscreen.multitouch.distinct.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.touchscreen.multitouch.distinct.xml
  PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml
  PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.touchscreen.multitouch.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.touchscreen.multitouch.xml
  PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.touchscreen.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.touchscreen.xml
endif

# USB OTG
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.usb.host.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.usb.host.xml



# alps/mediatek/config/$project
PRODUCT_COPY_FILES += $(LOCAL_PATH)/android.hardware.telephony.gsm.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.telephony.gsm.xml

# Set default USB interface


# meta tool


# Audio Related Resource
PRODUCT_COPY_FILES += vendor/mediatek/proprietary/custom/AGN_1250WQ_M6101E_E1_DS3216_Sgo/factory/res/sound/testpattern1.wav:$(TARGET_COPY_OUT_VENDOR)/res/sound/testpattern1.wav:mtk
PRODUCT_COPY_FILES += vendor/mediatek/proprietary/custom/AGN_1250WQ_M6101E_E1_DS3216_Sgo/factory/res/sound/ringtone.wav:$(TARGET_COPY_OUT_VENDOR)/res/sound/ringtone.wav:mtk


# Microphone
PRODUCT_COPY_FILES += $(LOCAL_PATH)/android.hardware.microphone.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.microphone.xml

# Camera
PRODUCT_COPY_FILES += $(LOCAL_PATH)/android.hardware.camera.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.camera.xml

# Audio Policy
PRODUCT_COPY_FILES += $(LOCAL_PATH)/audio_policy.conf:$(TARGET_COPY_OUT_VENDOR)/etc/audio_policy.conf:mtk


#Images for LCD test in factory mode
PRODUCT_COPY_FILES += vendor/mediatek/proprietary/custom/AGN_1250WQ_M6101E_E1_DS3216_Sgo/factory/res/images/lcd_test_00.png:$(TARGET_COPY_OUT_VENDOR)/res/images/lcd_test_00.png:mtk
PRODUCT_COPY_FILES += vendor/mediatek/proprietary/custom/AGN_1250WQ_M6101E_E1_DS3216_Sgo/factory/res/images/lcd_test_01.png:$(TARGET_COPY_OUT_VENDOR)/res/images/lcd_test_01.png:mtk
PRODUCT_COPY_FILES += vendor/mediatek/proprietary/custom/AGN_1250WQ_M6101E_E1_DS3216_Sgo/factory/res/images/lcd_test_02.png:$(TARGET_COPY_OUT_VENDOR)/res/images/lcd_test_02.png:mtk


PRODUCT_COPY_FILES += $(LOCAL_PATH)/voicemail_conf.xml:system/etc/voicemail-conf.xml:mtk

# F2FS filesystem
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.mtk_f2fs_enable=1

# Camera disable 4-cell Remosaic ALG for 32 bit projects
PRODUCT_PROPERTY_OVERRIDES +=  debug.camera.threshold_4Cell=1

BST_FACE_UNLOCK_SUPPORT := true
PRODUCT_COPY_FILES += $(LOCAL_PATH)/overlay/frameworks/native/data/etc/android.hardware.usb.host.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.usb.host.xml
PRODUCT_COPY_FILES += $(LOCAL_PATH)/overlay/frameworks/native/data/etc/android.hardware.sensor.barometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.barometer.xml
PRODUCT_COPY_FILES += $(LOCAL_PATH)/overlay/frameworks/native/data/etc/android.hardware.sensor.compass.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.compass.xml
PRODUCT_COPY_FILES += $(LOCAL_PATH)/overlay/frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.gyroscope.xml
PRODUCT_COPY_FILES += $(LOCAL_PATH)/overlay/frameworks/native/data/etc/android.hardware.usb.host.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.usb.host.xml
PRODUCT_COPY_FILES += $(LOCAL_PATH)/overlay/frameworks/native/data/etc/go_handheld_core_hardware.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/go_handheld_core_hardware.xml
PRODUCT_COPY_FILES += $(LOCAL_PATH)/overlay/frameworks/native/data/etc/handheld_core_hardware.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/handheld_core_hardware.xml
PRODUCT_COPY_FILES += $(LOCAL_PATH)/overlay/frameworks/native/data/etc/wearable_core_hardware.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/wearable_core_hardware.xml
PRODUCT_COPY_FILES += $(LOCAL_PATH)/overlay/frameworks/native/data/etc/android.software.opengles.deqp.level-2020-03-01.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.opengles.deqp.level.xml
PRODUCT_COPY_FILES += $(LOCAL_PATH)/overlay/frameworks/native/data/etc/android.hardware.sensor.stepcounter.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepcounter.xml
PRODUCT_COPY_FILES += $(LOCAL_PATH)/overlay/frameworks/native/data/etc/android.hardware.sensor.stepdetector.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepdetector.xml

$(call inherit-product, device/mediatek/mt6761/device.mk)

$(call inherit-product-if-exists, vendor/mediatek/libs/$(MTK_TARGET_PROJECT)/device-vendor.mk)

MTK_TELEPHONY_BUILD := yes

MTK_AUDIO_PARAM_DIR_LIST := $(strip $(MTK_AUDIO_PARAM_DIR_LIST))
MTK_AUDIO_PARAM_DIR_LIST += $(LOCAL_PATH)/audio_param

