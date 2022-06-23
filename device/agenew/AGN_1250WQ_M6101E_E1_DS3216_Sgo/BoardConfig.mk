# Use the non-open-source part, if present
-include vendor/mediatek/AGN_1250WQ_M6101E_E1_DS3216_Sgo/BoardConfigVendor.mk

# Use the 6761 common part
include device/mediatek/mt6761/BoardConfig.mk
AGENEW_ANDROIDBP_MACRO:= AGENEW_GOOGLE_KEY_PROJECT=AGN_1250WQ_M6101E_E1_DS3216_Sgo

MTK_INTERNAL_CDEFS := $(foreach t,$(AUTO_ADD_GLOBAL_DEFINE_BY_NAME),$(if $(filter-out no NO none NONE false FALSE,$($(t))),-D$(t)))
MTK_INTERNAL_CDEFS += $(foreach t,$(AUTO_ADD_GLOBAL_DEFINE_BY_VALUE),$(if $(filter-out no NO none NONE false FALSE,$($(t))),$(foreach v,$(shell echo $($(t)) | tr '[a-z]' '[A-Z]'),-D$(v))))
MTK_INTERNAL_CDEFS += $(foreach t,$(AUTO_ADD_GLOBAL_DEFINE_BY_NAME_VALUE),$(if $(filter-out no NO none NONE false FALSE,$($(t))),-D$(t)=\"$(strip $($(t)))\"))

MTK_GLOBAL_CFLAGS += $(MTK_INTERNAL_CDEFS)

ifneq ($(MTK_K64_SUPPORT), yes)
BOARD_KERNEL_CMDLINE = bootopt=64S3,32S1,32S1
else
BOARD_KERNEL_CMDLINE = bootopt=64S3,32N2,64N2
endif

#W1923 Android Q new feature UDC
BOARD_ROOT_EXTRA_FOLDERS += metadata
BOARD_USES_METADATA_PARTITION := true

# ptgen
MTK_PTGEN_CHIP := $(shell echo $(TARGET_BOARD_PLATFORM) | tr '[a-z]' '[A-Z]')
-include vendor/mediatek/proprietary/tools/ptgen/common/ptgen.mk

#Config partition size
-include $(MTK_PTGEN_OUT)/partition_size.mk
ifneq ($(strip $(MTK_AB_OTA_UPDATER)), yes)
BOARD_CACHEIMAGE_FILE_SYSTEM_TYPE := ext4
endif
BOARD_FLASH_BLOCK_SIZE := 4096

-include device/mediatek/build/core/soong_config.mk

# Precautions as below
# 1. Put common kernel modules in a group
# 2. Put Feature kernel modules individually, add feature by yourself
# 3. BOARD_VENDOR_KERNEL_MODULES no ordering here
# 4. BOARD_VENDOR_RAMDISK_KERNEL_MODULES will be loaded by list ordering + modules.dep

KERNEL_OUT ?= $(OUT_DIR)/target/project/$(TARGET_DEVICE)/obj/KERNEL_OBJ
# in-tree kernel modules installed to vendor
# For Common
BOARD_VENDOR_KERNEL_MODULES += $(KERNEL_OUT)/sound/soc/codecs/mt6357-accdet.ko \
			       $(KERNEL_OUT)/kernel/trace/trace_mmstat.ko \
# For Camera
BOARD_VENDOR_KERNEL_MODULES +=
# For Wifi
BOARD_VENDOR_KERNEL_MODULES +=

# in-tree kernel module installed to ramdisk
# For Common
BOARD_VENDOR_RAMDISK_KERNEL_MODULES +=
# For Camera
BOARD_VENDOR_RAMDISK_KERNEL_MODULES +=
# For Wifi
BOARD_VENDOR_RAMDISK_KERNEL_MODULES +=

#BOARD_SHIPPING_API_LEVEL := 31
