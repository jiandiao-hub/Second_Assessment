ifeq ($(strip $(AGN_MMITEST)), yes)
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_JAVA_LIBRARIES := framework
LOCAL_JAVA_LIBRARIES += telephony-common
LOCAL_JAVA_LIBRARIES += mediatek-common \
                        org.apache.http.legacy 


LOCAL_STATIC_JAVA_LIBRARIES += nfc_settings_adapter

LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_PACKAGE_NAME := MMITest
LOCAL_PROGUARD_ENABLED:= disabled

LOCAL_CERTIFICATE := platform
LOCAL_PRIVILEGED_MODULE := true

ifdef AGN_MMITEST_CONFIG
    LOCAL_RESOURCE_DIR := \
        $(LOCAL_PATH)/agn/$(AGN_MMITEST_CONFIG)/res \
        $(LOCAL_PATH)/res
else
    LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/res
endif

ifneq ($(strip $(AGN_FACTORY_TEST)), yes)
    EXCLUDE_MANIFESTS := \
    $(LOCAL_PATH)/factorytest/AndroidManifest.xml
endif

## TODO...just for debug
# LOCAL_MANIFEST_FILE := ./AndroidManifest.xml

# EXCLUDE_MANIFESTS := \
# 	$(LOCAL_PATH)/dialer/binary/aosp/testing/AndroidManifest.xml \
# 	$(LOCAL_PATH)/dialer/binary/google/AndroidManifest.xml \
# 	$(LOCAL_PATH)/incallui/calllocation/impl/AndroidManifest.xml \
# 	$(LOCAL_PATH)/incallui/maps/impl/AndroidManifest.xml \

# Dialer manifest files to merge.
MMITEST_MANIFEST_FILES := $(call all-named-files-under,AndroidManifest.xml,.)
MMITEST_MANIFEST_FILES := $(filter-out $(EXCLUDE_MANIFESTS),$(MMITEST_MANIFEST_FILES))
# Merge all manifest files.
LOCAL_FULL_LIBS_MANIFEST_FILES := \
	$(addprefix $(LOCAL_PATH)/, $(MMITEST_MANIFEST_FILES))

LOCAL_STATIC_JAVA_LIBRARIES += vendor.mediatek.hardware.nvram-V1.0-java \
                               vendor.mediatek.hardware.nvram-V1.1-java

LOCAL_PRIVATE_PLATFORM_APIS := true

LOCAL_ENFORCE_USES_LIBRARIES := false

include vendor/mediatek/proprietary/packages/apps/SettingsLib/common.mk

include $(BUILD_PACKAGE)

include $(call all-makefiles-under,$(LOCAL_PATH))
endif
