ifdef MTK_TEE_SUPPORT
LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/../products/sileadConfig.mk

include $(call all-makefiles-under, $(LOCAL_PATH))
endif