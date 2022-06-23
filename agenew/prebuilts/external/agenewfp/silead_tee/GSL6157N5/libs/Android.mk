ifdef MTK_TEE_SUPPORT
ifeq ($(SILEAD_FP_SUPPORT), yes)

LOCAL_PATH := $(call my-dir)

silead_lib_vendor := $(shell if [ $(PLATFORM_SDK_VERSION) -ge 26 ]; then echo yes; else echo no; fi)
silead_lib_no_check := $(shell if [ $(PLATFORM_SDK_VERSION) -ge 30 ]; then echo yes; else echo no; fi)

silead_lib_sw_mode :=
ifeq ($(SILEAD_TEST_TA), yes)
silead_lib_sw_mode := -test
endif

silead_lib_algo_dir :=
silead_lib_algo_type :=
ifneq ($(SID), )
silead_lib_algo_dir :=/$(SID)
silead_lib_algo_type := -$(SID)
endif

#------------------------------------------------
# algo
ifeq ($(SILEAD_FP_TEE_TYPE), nosec)
include $(CLEAR_VARS)
LOCAL_MODULE = libsl_fp_algo_silead
LOCAL_MODULE_CLASS = SHARED_LIBRARIES
LOCAL_MODULE_OWNER = silead
LOCAL_MODULE_SUFFIX = .so
LOCAL_MODULE_TAGS = optional
LOCAL_MULTILIB = both
LOCAL_SRC_FILES_32 = arm$(silead_lib_algo_dir)/$(LOCAL_MODULE)$(LOCAL_MODULE_SUFFIX)
LOCAL_SRC_FILES_64 = arm64$(silead_lib_algo_dir)/$(LOCAL_MODULE)$(LOCAL_MODULE_SUFFIX)
ifeq ($(silead_lib_vendor), yes)
    LOCAL_VENDOR_MODULE := true
endif
ifeq ($(silead_lib_no_check), yes)
     LOCAL_CHECK_ELF_FILES := false
endif
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE = libsl_fp_algo
LOCAL_MODULE_CLASS = SHARED_LIBRARIES
LOCAL_MODULE_OWNER = silead
LOCAL_MODULE_SUFFIX = .so
LOCAL_MODULE_TAGS = optional
ifeq ($(SILEAD_RA), yes)
    LOCAL_REQUIRED_MODULES := libsl_fp_algo_silead
endif
LOCAL_MULTILIB = both
LOCAL_SRC_FILES_32 = arm$(silead_lib_algo_dir)/$(LOCAL_MODULE)$(LOCAL_MODULE_SUFFIX)
LOCAL_SRC_FILES_64 = arm64$(silead_lib_algo_dir)/$(LOCAL_MODULE)$(LOCAL_MODULE_SUFFIX)
ifeq ($(silead_lib_vendor), yes)
    LOCAL_VENDOR_MODULE := true
endif
ifeq ($(silead_lib_no_check), yes)
     LOCAL_CHECK_ELF_FILES := false
endif
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE = libsl_fp_prepro
LOCAL_MODULE_CLASS = SHARED_LIBRARIES
LOCAL_MODULE_OWNER = silead
LOCAL_MODULE_SUFFIX = .so
LOCAL_MODULE_TAGS = optional
LOCAL_MULTILIB = both
LOCAL_SRC_FILES_32 = arm$(silead_lib_algo_dir)/$(LOCAL_MODULE)$(LOCAL_MODULE_SUFFIX)
LOCAL_SRC_FILES_64 = arm64$(silead_lib_algo_dir)/$(LOCAL_MODULE)$(LOCAL_MODULE_SUFFIX)
ifeq ($(silead_lib_vendor), yes)
    LOCAL_VENDOR_MODULE := true
endif
ifeq ($(silead_lib_no_check), yes)
     LOCAL_CHECK_ELF_FILES := false
endif
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE = libsl_fp_nosec
LOCAL_MODULE_CLASS = SHARED_LIBRARIES
LOCAL_MODULE_OWNER = silead
LOCAL_MODULE_SUFFIX = .so
LOCAL_MODULE_TAGS = optional
LOCAL_REQUIRED_MODULES := libsl_fp_algo libsl_fp_prepro
LOCAL_MULTILIB = both
LOCAL_SRC_FILES_32 = arm/$(LOCAL_MODULE)$(silead_lib_algo_type)$(silead_lib_sw_mode)$(LOCAL_MODULE_SUFFIX)
LOCAL_SRC_FILES_64 = arm64/$(LOCAL_MODULE)$(silead_lib_algo_type)$(silead_lib_sw_mode)$(LOCAL_MODULE_SUFFIX)
ifeq ($(silead_lib_vendor), yes)
    LOCAL_VENDOR_MODULE := true
endif
ifeq ($(silead_lib_no_check), yes)
     LOCAL_CHECK_ELF_FILES := false
endif
include $(BUILD_PREBUILT)

endif #SILEAD_FP_TEE_TYPE=nosec

#------------------------------------------------
ifneq ($(SILEAD_FP_HAL_SRC), yes)
include $(CLEAR_VARS)
LOCAL_MODULE = libsl_fp_impl
LOCAL_MODULE_CLASS = SHARED_LIBRARIES
LOCAL_MODULE_OWNER = silead
LOCAL_MODULE_SUFFIX = .so
LOCAL_MODULE_TAGS = optional
LOCAL_MULTILIB = both
LOCAL_SRC_FILES_32 = arm/$(LOCAL_MODULE)-$(SILEAD_FP_TEE_TYPE)$(silead_lib_sw_mode)$(LOCAL_MODULE_SUFFIX)
LOCAL_SRC_FILES_64 = arm64/$(LOCAL_MODULE)-$(SILEAD_FP_TEE_TYPE)$(silead_lib_sw_mode)$(LOCAL_MODULE_SUFFIX)
ifeq ($(SILEAD_FP_TEE_TYPE), nosec)
LOCAL_REQUIRED_MODULES += libsl_fp_nosec
endif
LOCAL_STRIP_MODULE := false
ifeq ($(silead_lib_vendor), yes)
    LOCAL_VENDOR_MODULE := true
endif
ifeq ($(silead_lib_no_check), yes)
     LOCAL_CHECK_ELF_FILES := false
endif
include $(BUILD_PREBUILT)
endif #SILEAD_FP_HAL_SRC

endif #SILEAD_FP_SUPPORT
endif