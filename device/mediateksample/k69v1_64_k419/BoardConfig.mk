# Use the non-open-source part, if present
-include vendor/mediatek/k69v1_64_k419/BoardConfigVendor.mk

# Use the 6768 common part
include device/mediatek/mt6768/BoardConfig.mk

#Config partition size
-include $(MTK_PTGEN_OUT)/partition_size.mk
ifneq ($(strip $(MTK_AB_OTA_UPDATER)), yes)
BOARD_CACHEIMAGE_FILE_SYSTEM_TYPE := ext4
endif
BOARD_FLASH_BLOCK_SIZE := 4096

MTK_INTERNAL_CDEFS := $(foreach t,$(AUTO_ADD_GLOBAL_DEFINE_BY_NAME),$(if $(filter-out no NO none NONE false FALSE,$($(t))),-D$(t)))
MTK_INTERNAL_CDEFS += $(foreach t,$(AUTO_ADD_GLOBAL_DEFINE_BY_VALUE),$(if $(filter-out no NO none NONE false FALSE,$($(t))),$(foreach v,$(shell echo $($(t)) | tr '[a-z]' '[A-Z]'),-D$(v))))
MTK_INTERNAL_CDEFS += $(foreach t,$(AUTO_ADD_GLOBAL_DEFINE_BY_NAME_VALUE),$(if $(filter-out no NO none NONE false FALSE,$($(t))),-D$(t)=\"$(strip $($(t)))\"))

MTK_GLOBAL_CFLAGS += $(MTK_INTERNAL_CDEFS)

ifneq ($(MTK_K64_SUPPORT), yes)
BOARD_KERNEL_CMDLINE = bootopt=64S3,32S1,32S1
else
BOARD_KERNEL_CMDLINE = bootopt=64S3,32N2,64N2
endif


-include device/mediatek/build/core/soong_config.mk

KERNEL_OUT ?= $(OUT_DIR)/target/project/$(TARGET_DEVICE)/obj/KERNEL_OBJ
# in-tree kernel modules installed to vendor
BOARD_VENDOR_KERNEL_MODULES += $(KERNEL_OUT)/kernel/trace/trace_mmstat.ko
