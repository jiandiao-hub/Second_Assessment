SL_FP_ROOT := agenew/prebuilts/external/agenewfp/silead_tee/GSL6157N5

include $(SL_FP_ROOT)/products/sileadConfig.mk
$(warning $(shell echo "SILEAD_FP_SUPPORT is $(SILEAD_FP_SUPPORT)"))
$(warning $(shell echo "SILEAD_FP_TEE_TYPE is $(SILEAD_FP_TEE_TYPE)"))
$(warning $(shell echo "MTK_PLATFORM is $(MTK_PLATFORM)"))
$(warning $(shell echo "PLATFORM_VERSION is $(PLATFORM_VERSION)"))
ifeq ($(strip $(SILEAD_FP_SUPPORT)),yes)
    BOARD_SEPOLICY_DIRS += $(SL_FP_ROOT)/sepolicy/android8/vendor/common

    ifeq ($(strip $(SILEAD_FP_TEE_TYPE)),qsee)
        BOARD_SEPOLICY_DIRS += $(SL_FP_ROOT)/sepolicy/android8/vendor/qsee
    else ifeq ($(strip $(SILEAD_FP_TEE_TYPE)),tee)
        BOARD_SEPOLICY_DIRS += $(SL_FP_ROOT)/sepolicy/android8/vendor/tee
    else ifeq ($(strip $(SILEAD_FP_TEE_TYPE)),gp)
        BOARD_SEPOLICY_DIRS += $(SL_FP_ROOT)/sepolicy/android8/vendor/tee
    else ifeq ($(strip $(SILEAD_FP_TEE_TYPE)),trusty)
        BOARD_SEPOLICY_DIRS += $(SL_FP_ROOT)/sepolicy/android8/vendor/trusty
    endif

    ifeq ($(strip $(SILEAD_FP_TEST_SUPPORT)),yes)
      ifeq ($(strip $(shell expr $(PLATFORM_VERSION) \>= 12)),1)
        SYSTEM_EXT_PUBLIC_SEPOLICY_DIRS += $(SL_FP_ROOT)/sepolicy/android8/public
       ifneq ($(strip $(MTK_PLATFORM)),)
        SYSTEM_EXT_PRIVATE_SEPOLICY_DIRS += $(SL_FP_ROOT)/sepolicy/android8/private_mtk
       else
        SYSTEM_EXT_PRIVATE_SEPOLICY_DIRS += $(SL_FP_ROOT)/sepolicy/android8/private
       endif
      else
        BOARD_PLAT_PUBLIC_SEPOLICY_DIR += $(SL_FP_ROOT)/sepolicy/android8/public
        BOARD_PLAT_PRIVATE_SEPOLICY_DIR += $(SL_FP_ROOT)/sepolicy/android8/private
      endif
    endif
endif
