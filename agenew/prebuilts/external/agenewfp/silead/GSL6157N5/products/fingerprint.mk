SL_FP_ROOT := agenew/prebuilts/external/agenewfp/silead/GSL6157N5

include $(SL_FP_ROOT)/products/sileadConfig.mk
$(warning $(shell echo "SILEAD_FP_SUPPORT is $(SILEAD_FP_SUPPORT)"))
$(warning $(shell echo "SILEAD_FP_HAL_LIB_NAME is $(SILEAD_FP_HAL_LIB_NAME)"))
$(warning $(shell echo "SILEAD_FP_TEST_SUPPORT is $(SILEAD_FP_TEST_SUPPORT)"))
ifeq ($(strip $(SILEAD_FP_SUPPORT)),yes)

#-------------------------------------------------------------
# system default, may already be added, if so please remove
PRODUCT_PACKAGES += \
    android.hardware.biometrics.fingerprint@2.1-service

PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.fingerprint.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.fingerprint.xml

PRODUCT_COPY_FILES += $(SL_FP_ROOT)/init.silead.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/init.silead.rc

# silead manifest.xml
#DEVICE_MANIFEST_FILE += $(SL_FP_ROOT)/manifest.xml

#-------------------------------------------------------------
# silead fp
ifneq ($(strip $(SILEAD_FP_HAL_LIB_NAME)),)
PRODUCT_PACKAGES += \
    $(SILEAD_FP_HAL_LIB_NAME)
else
PRODUCT_PACKAGES += \
    fingerprint.silead.default
endif

ifeq ($(strip $(SILEAD_FP_TEST_SUPPORT)),yes)
# for fingerprint test
PRODUCT_PACKAGES += \
    vendor.silead.hardware.fingerprintext@1.0-service

PRODUCT_PACKAGES += \
    sileadManager
endif

SYSPARAM_FOLDER_PATH := $(SL_FP_ROOT)/etc/silead/sysparms
SYSPARAM_ALL_FOLDER  := $(shell find $(SYSPARAM_FOLDER_PATH)/* ! -path "." -type d 2>/dev/null)
ifneq ($(strip $(SYSPARAM_ALL_FOLDER)),)
    SYSPARAM_ALL_FOLDER := $(SYSPARAM_ALL_FOLDER:$(SYSPARAM_FOLDER_PATH)/%=%)
    PRODUCT_COPY_FILES += $(foreach cfg_path, $(SYSPARAM_ALL_FOLDER), \
                    $(SYSPARAM_FOLDER_PATH)/$(cfg_path)/silead_config.xml:$(TARGET_COPY_OUT_VENDOR)/etc/silead/sysparms/$(cfg_path)/silead_config.xml \
                    $(SYSPARAM_FOLDER_PATH)/$(cfg_path)/silead_param.xml:$(TARGET_COPY_OUT_VENDOR)/etc/silead/sysparms/$(cfg_path)/silead_param.xml)
endif

endif