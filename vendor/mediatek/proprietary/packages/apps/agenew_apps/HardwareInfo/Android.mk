ifeq ($(strip $(AGN_HARDWARE_INFO)), yes)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_PACKAGE_NAME := HardwareInfoAgenew

ifneq ($(strip $(AGN_HARDWARE_INFO_CONFIG)), )
    LOCAL_RESOURCE_DIR := \
        $(LOCAL_PATH)/config/$(strip $(AGN_HARDWARE_INFO_CONFIG))/res \
        $(LOCAL_PATH)/res
else
    LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/res
endif

LOCAL_CERTIFICATE := platform
LOCAL_PRIVILEGED_MODULE := true
LOCAL_PRIVATE_PLATFORM_APIS := true
LOCAL_PROGUARD_ENABLED:= disabled

include $(BUILD_PACKAGE)

# This finds and builds the test apk as well, so a single make does both.
include $(call all-makefiles-under,$(LOCAL_PATH))

endif
