LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    cust_aal.cpp

LOCAL_SHARED_LIBRARIES := \
    libcutils \


LOCAL_C_INCLUDES += \
    $(MTK_PATH_SOURCE)/hardware/aal/include

ifeq ($(strip $(MTK_ULTRA_DIMMING_SUPPORT)),yes)
    LOCAL_CFLAGS += -DMTK_ULTRA_DIMMING_SUPPORT
endif

LOCAL_MODULE:= libaal_config
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk

include $(BUILD_STATIC_LIBRARY)
