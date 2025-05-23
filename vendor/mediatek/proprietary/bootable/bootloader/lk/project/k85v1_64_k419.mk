#
LOCAL_DIR := $(GET_LOCAL_DIR)
TARGET := k85v1_64_k419
MODULES += app/mt_boot \
           dev/lcm
MTK_UFS_SUPPORT = yes
MTK_UFS_OTP = yes
MTK_EMMC_SUPPORT = yes
MTK_EMMC_SUPPORT_OTP = yes
MTK_MMC_COMBO_DRV = yes
MTK_KERNEL_POWER_OFF_CHARGING = yes
MTK_SMI_SUPPORT = yes
DEFINES += MTK_NEW_COMBO_EMMC_SUPPORT
DEFINES += MTK_GPT_SCHEME_SUPPORT
MTK_CHARGER_NEW_ARCH := yes
MTK_PUMP_EXPRESS_PLUS_SUPPORT := no
MTK_CHARGER_INTERFACE := yes
MTK_MT6360_PMU_CHARGER_SUPPORT := yes
MTK_LCM_PHYSICAL_ROTATION = 0
CUSTOM_LK_LCM="otm1911a_fhdp_dsi_vdo_truly_rt4801 hx83112b_fhdp_dsi_vdo_auo_rt4801"
DEFINES += MTK_ROUND_CORNER_SUPPORT
#nt35595_fhd_dsi_cmd_truly_nt50358 = yes
MTK_VERIFIED_BOOT_SUPPORT = no
MTK_SEC_FASTBOOT_UNLOCK_SUPPORT = yes
SPM_FW_USE_PARTITION = yes
BOOT_LOGO := fhdplus
DEBUG := 2
#DEFINES += WITH_DEBUG_DCC=1
DEFINES += WITH_DEBUG_UART=1
#DEFINES += WITH_DEBUG_FBCON=1
CUSTOM_LK_USB_UNIQUE_SERIAL=no
MTK_TINYSYS_SCP_SUPPORT = yes
MTK_PROTOCOL1_RAT_CONFIG = C/Lf/Lt/W/T/G
MTK_GOOGLE_TRUSTY_SUPPORT=no
DEFINES += MTK_MT6370_PMU
DEVELOP_STAGE = SB
MTK_TINYSYS_SSPM_SUPPORT = yes
MTK_VPU_SUPPORT = yes
MTK_AUDIODSP_SUPPORT = yes
MTK_SMC_ID_MGMT = yes
TYPEC_MT6360 = yes
MTK_ILDO_SUPPORT = yes
MTK_DYNAMIC_CCB_BUFFER_GEAR_ID=
MTK_MINIMUM_SCP_DRAM_SIZE = yes
MTK_AB_OTA_UPDATER = yes
MTK_ENABLE_GENIEZONE = yes
MTK_FAST_CHARGER_TECH = no