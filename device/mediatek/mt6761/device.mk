
# Do not copy rc files before this line !!
# RC files should goto /vendor since O-MR1
TARGET_BOARD_PLATFORM ?= mt6761
MTK_TARGET_VENDOR_RC = $(TARGET_COPY_OUT_VENDOR)/etc/init/hw
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.vendor.rc=/vendor/etc/init/hw/

ifeq ($(strip $(MTK_EMMC_SUPPORT)),yes)
  PRODUCT_COPY_FILES += device/mediatek/mt6761/ueventd.mt6761.emmc.rc:$(TARGET_COPY_OUT_VENDOR)/etc/ueventd.rc
else ifeq ($(strip $(MTK_UFS_BOOTING)),yes)
  PRODUCT_COPY_FILES += device/mediatek/mt6761/ueventd.mt6761.ufs.rc:$(TARGET_COPY_OUT_VENDOR)/etc/ueventd.rc
endif

# disable system as root by platform
# will move it to common/device.mk after all platform ready
BOARD_BUILD_SYSTEM_ROOT_IMAGE := false

PRODUCT_SUPPORT_DYNAMIC_PARTITION ?= true

PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.location.gps.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.location.gps.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.wifi.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.wifi.direct.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.direct.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.bluetooth.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.bluetooth.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.bluetooth_le.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.bluetooth_le.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.touchscreen.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.touchscreen.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.software.midi.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.midi.xml
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.software.managed_users.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.managed_users.xml

# IRTX over PWM
ifeq ($(strip $(MTK_IRTX_PWM_SUPPORT)),yes)
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.consumerir.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.consumerir.xml
PRODUCT_PACKAGES += consumerir.common
endif


#GPU collection
PRODUCT_PACKAGES += libI420colorconvert
PRODUCT_PACKAGES += libvcodec_utility
PRODUCT_PACKAGES += libvcodec_oal
PRODUCT_PACKAGES += libvcodecdrv
PRODUCT_PACKAGES += libh264enc_sa.ca7
PRODUCT_PACKAGES += libh264enc_sb.ca7
PRODUCT_PACKAGES += libhevce_sb.ca7.android
PRODUCT_PACKAGES += libHEVCdec_sa.ca7.android
PRODUCT_PACKAGES += libmp4enc_sa.ca7
PRODUCT_PACKAGES += libmp4enc_xa.ca7
PRODUCT_PACKAGES += libvc1dec_sa.ca7
PRODUCT_PACKAGES += libvp8dec_sa.ca7
PRODUCT_PACKAGES += libvp8enc_sa.ca7
PRODUCT_PACKAGES += libvp9dec_sa.ca7
PRODUCT_PACKAGES += libvideoeditorplayer
PRODUCT_PACKAGES += libvideoeditor_osal
PRODUCT_PACKAGES += libvideoeditor_3gpwriter
PRODUCT_PACKAGES += libvideoeditor_mcs
PRODUCT_PACKAGES += libvideoeditor_core
PRODUCT_PACKAGES += libvideoeditor_stagefrightshells
PRODUCT_PACKAGES += libvideoeditor_videofilters
PRODUCT_PACKAGES += libvideoeditor_jni
PRODUCT_PACKAGES += audio.primary.default
PRODUCT_PACKAGES += audio.primary.mt6761
PRODUCT_PACKAGES += audio_policy.stub
PRODUCT_PACKAGES += local_time.default
PRODUCT_PACKAGES += libaudiocustparam
#PRODUCT_PACKAGES += libaudiocomponentengine
PRODUCT_PACKAGES += libaudiocomponentengine_vendor
PRODUCT_PACKAGES += libh264dec_xa.ca9
PRODUCT_PACKAGES += libh264dec_xb.ca9
PRODUCT_PACKAGES += libh264dec_sa.ca7
PRODUCT_PACKAGES += libh264dec_sd.ca7
PRODUCT_PACKAGES += libh264dec_se.ca7
PRODUCT_PACKAGES += libh264dec_customize
PRODUCT_PACKAGES += libmp4dec_sa.ca9
PRODUCT_PACKAGES += libmp4dec_sb.ca9
PRODUCT_PACKAGES += libmp4dec_customize
PRODUCT_PACKAGES += libvp8dec_xa.ca9
PRODUCT_PACKAGES += libmp4enc_xa.ca9
PRODUCT_PACKAGES += libmp4enc_xb.ca9
PRODUCT_PACKAGES += libh264enc_sa.ca9
PRODUCT_PACKAGES += libh264enc_sb.ca9
PRODUCT_PACKAGES += libvcodec_oal
PRODUCT_PACKAGES += libvc1dec_sa.ca9
PRODUCT_PACKAGES += liblic_s263
PRODUCT_PACKAGES += init.factory.rc
PRODUCT_PACKAGES += libaudio.primary.default
PRODUCT_PACKAGES += audio_policy.default
PRODUCT_PACKAGES += audio_policy.mt6761
PRODUCT_PACKAGES += libaudio.a2dp.default
PRODUCT_PACKAGES += libMtkVideoTranscoder
PRODUCT_PACKAGES += libMtkOmxCore
PRODUCT_PACKAGES += libMtkOmxOsalUtils
PRODUCT_PACKAGES += libMtkOmxVdecEx
PRODUCT_PACKAGES += libMtkOmxVenc
#PRODUCT_PACKAGES += libaudiodcrflt
PRODUCT_PACKAGES += libaudiosetting
#PRODUCT_PACKAGES += librtp_jni
PRODUCT_PACKAGES += libstagefrighthw
PRODUCT_PACKAGES += libstagefright_memutil
PRODUCT_PACKAGES += factory.ini
PRODUCT_PACKAGES += libmtdutil
PRODUCT_PACKAGES += libminiui
PRODUCT_PACKAGES += factory
PRODUCT_PACKAGES += drvbd
PRODUCT_PACKAGES += libaudio.usb.default
PRODUCT_PACKAGES += audio.usb.default
PRODUCT_PACKAGES += audio.usb.mt6761
PRODUCT_PACKAGES += AccountAndSyncSettings
#PRODUCT_PACKAGES += DeskClock
PRODUCT_PACKAGES += AlarmProvider
#PRODUCT_PACKAGES += Bluetooth
PRODUCT_PACKAGES += Calculator
#PRODUCT_PACKAGES += Calendar
#PRODUCT_PACKAGES += CertInstaller
ifeq ($(strip $(MTK_OMADRM_SUPPORT)), yes)
  #PRODUCT_PACKAGES += DrmProvider
endif
#PRODUCT_PACKAGES += Email
#PRODUCT_PACKAGES += FusedLocation
#PRODUCT_PACKAGES += TelephonyProvider
PRODUCT_PACKAGES += Exchange2
#PRODUCT_PACKAGES += LatinIME
#PRODUCT_PACKAGES += Music
#PRODUCT_PACKAGES += MusicFX
#PRODUCT_PACKAGES += Protips
#PRODUCT_PACKAGES += Settings
PRODUCT_PACKAGES += Sync
#PRODUCT_PACKAGES += SystemUI
#PRODUCT_PACKAGES += Updater
#PRODUCT_PACKAGES += CalendarProvider
PRODUCT_PACKAGES += ccci_mdinit
PRODUCT_PACKAGES += ccci_rpcd
#PRODUCT_PACKAGES += batterywarning
PRODUCT_PACKAGES += SyncProvider
#PRODUCT_PACKAGES += Launcher3
PRODUCT_PACKAGES += disableapplist.txt
PRODUCT_PACKAGES += resmonwhitelist.txt
ifeq ($(strip $(MTK_GMO_RAM_OPTIMIZE)),yes)
else
  #PRODUCT_PACKAGES += MTKThermalManager
  #PRODUCT_PACKAGES += thermald
endif
PRODUCT_PACKAGES += libmtcloader
PRODUCT_PACKAGES += thermal_manager
PRODUCT_PACKAGES += thermal
PRODUCT_PACKAGES += thermal_hal
PRODUCT_PACKAGES += thermalloadalgod
PRODUCT_PACKAGES += libthermalalgo
PRODUCT_PACKAGES += CellConnService
PRODUCT_PACKAGES += libthha
PRODUCT_PACKAGES += ami304d
PRODUCT_PACKAGES += akmd8963
PRODUCT_PACKAGES += akmd09911
PRODUCT_PACKAGES += akmd09912
PRODUCT_PACKAGES += akmd8975
PRODUCT_PACKAGES += istd8303
PRODUCT_PACKAGES += AcdApiDaemon
PRODUCT_PACKAGES += calib.dat
PRODUCT_PACKAGES += param.dat
PRODUCT_PACKAGES += geomagneticd
PRODUCT_PACKAGES += orientationd
PRODUCT_PACKAGES += memsicd
PRODUCT_PACKAGES += msensord
PRODUCT_PACKAGES += lsm303md
PRODUCT_PACKAGES += memsicd3416x
PRODUCT_PACKAGES += s62xd smartsensor
PRODUCT_PACKAGES += bmm050d
PRODUCT_PACKAGES += mc6420d
PRODUCT_PACKAGES += magd
PRODUCT_PACKAGES += lights.mt6761
PRODUCT_PACKAGES += vibrator.mt6761
PRODUCT_PACKAGES += meta_tst
PRODUCT_PACKAGES += dhcp6c
PRODUCT_PACKAGES += dhcp6ctl
PRODUCT_PACKAGES += dhcp6c.conf
PRODUCT_PACKAGES += dhcp6cDNS.conf
PRODUCT_PACKAGES += dhcp6s
PRODUCT_PACKAGES += dhcp6s.conf
PRODUCT_PACKAGES += dhcp6c.script
PRODUCT_PACKAGES += dhcp6cctlkey
#PRODUCT_PACKAGES += libblisrc
PRODUCT_PACKAGES += libifaddrs
PRODUCT_PACKAGES += libmobilelog_jni
PRODUCT_PACKAGES += libaudio.r_submix.default
PRODUCT_PACKAGES += audio.r_submix.mt6761
PRODUCT_PACKAGES += audio.r_submix.default
PRODUCT_PACKAGES += libaudio.usb.default
#PRODUCT_PACKAGES += libnbaio
#PRODUCT_PACKAGES += libaudioflinger
PRODUCT_PACKAGES += libmeta_audio
PRODUCT_PACKAGES += liba3m
PRODUCT_PACKAGES += libja3m
PRODUCT_PACKAGES += libmmprofile
PRODUCT_PACKAGES += libmmprofile_jni
PRODUCT_PACKAGES += libtvoutjni
PRODUCT_PACKAGES += libtvoutpattern
PRODUCT_PACKAGES += libmtkhdmi_jni
PRODUCT_PACKAGES += libmtkcam_modulefactory_custom
PRODUCT_PACKAGES += libmtkcam_modulefactory_drv
PRODUCT_PACKAGES += libmtkcam_modulefactory_aaa
PRODUCT_PACKAGES += libmtkcam_modulefactory_feature
PRODUCT_PACKAGES += libmtkcam_modulefactory_utils
PRODUCT_PACKAGES += libcam_platform
PRODUCT_PACKAGES += vendor.mediatek.hardware.camera.advcam@1.0-impl
PRODUCT_PACKAGES += camerahalserver
PRODUCT_PACKAGES += android.hardware.camera.provider@2.6-service-mediatek
PRODUCT_PACKAGES += android.hardware.camera.provider@2.6-impl-mediatek
PRODUCT_PACKAGES += libmtkcam_device1
PRODUCT_PACKAGES += libmtkcam_device3
PRODUCT_PACKAGES += camera.mt6761
#PRODUCT_PACKAGES += liblog
PRODUCT_PACKAGES += shutdown
PRODUCT_PACKAGES += muxreport
PRODUCT_PACKAGES += mtkrild
PRODUCT_PACKAGES += mtk-ril
PRODUCT_PACKAGES += librilmtk
PRODUCT_PACKAGES += libutilrilmtk
PRODUCT_PACKAGES += gsm0710muxd
PRODUCT_PACKAGES += md_minilog_util
#PRODUCT_PACKAGES += wbxml
PRODUCT_PACKAGES += wappush
PRODUCT_PACKAGES += thememap.xml
PRODUCT_PACKAGES += libBLPP.so
PRODUCT_PACKAGES += rc.fac
PRODUCT_PACKAGES += mtkGD
#PRODUCT_PACKAGES += libdrm
PRODUCT_PACKAGES += libdrm.vendor
PRODUCT_PACKAGES += libGLES_meow
PRODUCT_PACKAGES += libMEOW_trace
PRODUCT_PACKAGES += libEGL_mtk
PRODUCT_PACKAGES += libGLESv1_CM_mtk
PRODUCT_PACKAGES += libGLESv2_mtk
PRODUCT_PACKAGES += libglslcompiler
PRODUCT_PACKAGES += libIMGegl
#PRODUCT_PACKAGES += libmemtrack_GL
PRODUCT_PACKAGES += libPVROCL
PRODUCT_PACKAGES += libPVRScopeServices
PRODUCT_PACKAGES += libsrv_um
PRODUCT_PACKAGES += libmpvr
PRODUCT_PACKAGES += libPVRMtkutils
PRODUCT_PACKAGES += libufwriter
PRODUCT_PACKAGES += libusc
PRODUCT_PACKAGES += libtqvalidate
PRODUCT_PACKAGES += rgx.fw
PRODUCT_PACKAGES += rgx.sh
ifneq ($(strip $(MTK_GMO_RAM_OPTIMIZE)),yes)
  PRODUCT_PACKAGES += vulkan.mt6761
endif
PRODUCT_PACKAGES += pvrhwperfd
PRODUCT_PACKAGES += pvrsrvctl
PRODUCT_PACKAGES += thermalindicator
PRODUCT_PACKAGES += libged.so
ifneq ($(MTK_BASIC_PACKAGE), yes)
  PRODUCT_PACKAGES += libgas.so
endif
PRODUCT_PACKAGES += libFraunhoferAAC
PRODUCT_PACKAGES += audiocmdservice_atci
PRODUCT_PACKAGES += libphonemotiondetector_jni
PRODUCT_PACKAGES += libphonemotiondetector
PRODUCT_PACKAGES += libmotionrecognition
PRODUCT_PACKAGES += audio.primary.default
PRODUCT_PACKAGES += audio_policy.stub
PRODUCT_PACKAGES += audio_policy.default
PRODUCT_PACKAGES += libaudio.primary.default
PRODUCT_PACKAGES += libaudio.a2dp.default
#PRODUCT_PACKAGES += audio.a2dp.default
#PRODUCT_PACKAGES += libaudio-resampler
PRODUCT_PACKAGES += local_time.default
PRODUCT_PACKAGES += libaudiocustparam
#PRODUCT_PACKAGES += libaudiodcrflt
PRODUCT_PACKAGES += libaudiosetting
#PRODUCT_PACKAGES += librtp_jni
PRODUCT_PACKAGES += libmatv_cust
#PRODUCT_PACKAGES += libmtkplayer
PRODUCT_PACKAGES += libatvctrlservice
PRODUCT_PACKAGES += matv
PRODUCT_PACKAGES += ppp_dt
PRODUCT_PACKAGES += libdiagnose
#PRODUCT_PACKAGES += libsonivox
PRODUCT_PACKAGES += iAmCdRom.iso
PRODUCT_PACKAGES += libmemorydumper
PRODUCT_PACKAGES += memorydumper
PRODUCT_PACKAGES += libvt_custom
PRODUCT_PACKAGES += libamrvt
PRODUCT_PACKAGES += libvtmal
PRODUCT_PACKAGES += libipsec_ims
#PRODUCT_PACKAGES += racoon
PRODUCT_PACKAGES += libipsec
#PRODUCT_PACKAGES += libpcap
#PRODUCT_PACKAGES += mtpd
PRODUCT_PACKAGES += netcfg
#PRODUCT_PACKAGES += pppd
PRODUCT_PACKAGES += pppd_via
PRODUCT_PACKAGES += pppd_dt
PRODUCT_PACKAGES += dhcpcd
PRODUCT_PACKAGES += dhcpcd.conf
PRODUCT_PACKAGES += dhcpcd-run-hooks
PRODUCT_PACKAGES += 20-dns.conf
PRODUCT_PACKAGES += 95-configured
PRODUCT_PACKAGES += radvd
PRODUCT_PACKAGES += radvd.conf
#PRODUCT_PACKAGES += dnsmasq
#PRODUCT_PACKAGES += netd
#PRODUCT_PACKAGES += ndc
#PRODUCT_PACKAGES += libiprouteutil
#PRODUCT_PACKAGES += libnetlink
#PRODUCT_PACKAGES += tc
#PRODUCT_PACKAGES += e2fsck
#PRODUCT_PACKAGES += libext2_blkid
#PRODUCT_PACKAGES += libext2_e2p
#PRODUCT_PACKAGES += libext2_com_err
#PRODUCT_PACKAGES += libext2fs
#PRODUCT_PACKAGES += libext2_uuid
#PRODUCT_PACKAGES += mke2fs
#PRODUCT_PACKAGES += tune2fs
#PRODUCT_PACKAGES += badblocks
#PRODUCT_PACKAGES += resize2fs
PRODUCT_PACKAGES += resize.f2fs
PRODUCT_PACKAGES += make_ext4fs
#PRODUCT_PACKAGES += sdcard
PRODUCT_PACKAGES += libext
PRODUCT_PACKAGES += libext4
PRODUCT_PACKAGES += libext6
PRODUCT_PACKAGES += libxtables
PRODUCT_PACKAGES += libip4tc
PRODUCT_PACKAGES += libip6tc
PRODUCT_PACKAGES += ipod
PRODUCT_PACKAGES += libipod
PRODUCT_PACKAGES += fuelgauged
PRODUCT_PACKAGES += fuelgauged_nvram
ifeq ($(MTK_GAUGE_VERSION), 30)
PRODUCT_PACKAGES += libfgauge_gm30
else
PRODUCT_PACKAGES += libfgauge
endif
PRODUCT_PACKAGES += android.hardware.health@2.1-service
PRODUCT_PACKAGES += android.hardware.health@2.1-impl
PRODUCT_PACKAGES += android.hardware.health@2.1-impl.recovery
PRODUCT_PACKAGES += gatord
PRODUCT_PACKAGES += gator.ko
#PRODUCT_PACKAGES += boot_logo_updater
PRODUCT_PACKAGES += boot_logo
#PRODUCT_PACKAGES += bootanimation
ifneq (,$(filter yes, $(MTK_KERNEL_POWER_OFF_CHARGING)))
#PRODUCT_PACKAGES += kpoc_charger
endif
PRODUCT_PACKAGES += libtvoutjni
PRODUCT_PACKAGES += libtvoutpattern
PRODUCT_PACKAGES += libmtkhdmi_jni
PRODUCT_PACKAGES += libhissage.so
PRODUCT_PACKAGES += libhpe.so
PRODUCT_PACKAGES += sdiotool
PRODUCT_PACKAGES += superumount
PRODUCT_PACKAGES += libsched
PRODUCT_PACKAGES += fsck_msdos_mtk
PRODUCT_PACKAGES += cmmbsp
PRODUCT_PACKAGES += libcmmb_jni
PRODUCT_PACKAGES += robotium
PRODUCT_PACKAGES += libc_malloc_debug_mtk
PRODUCT_PACKAGES += dpfd
PRODUCT_PACKAGES += SchedulePowerOnOff
#PRODUCT_PACKAGES += BatteryWarning
PRODUCT_PACKAGES += libpq_cust_base
PRODUCT_PACKAGES += libpq_cust
PRODUCT_PACKAGES += librgbwlightsensor
#PRODUCT_PACKAGES += libPQjni
#PRODUCT_PACKAGES += libPQDCjni
#PRODUCT_PACKAGES += libMiraVision_jni
PRODUCT_PACKAGES += hald
PRODUCT_PACKAGES += dmlog
PRODUCT_PACKAGES += mtk_msr.ko

PRODUCT_PACKAGES += cpu_stress_cache_miss.ko
PRODUCT_PACKAGES += cpu_stress_dhry.ko
PRODUCT_PACKAGES += cpu_stress_maxpower.ko
PRODUCT_PACKAGES += cpu_stress_maxtrans.ko
PRODUCT_PACKAGES += cpu_stress_saxpy.ko

PRODUCT_PACKAGES += send_bug
#PRODUCT_PACKAGES += sspm_log_writer
#PRODUCT_PACKAGES += met-cmd
#PRODUCT_PACKAGES += met_log_d
PRODUCT_PACKAGES += met.ko
PRODUCT_PACKAGES += met_plf.ko
PRODUCT_PACKAGES += libmet-tag
PRODUCT_PACKAGES += trace-cmd
PRODUCT_PACKAGES += fteh.cfg
PRODUCT_PACKAGES += sn
#PRODUCT_PACKAGES += lcdc_screen_cap
PRODUCT_PACKAGES += libJniAtvService
PRODUCT_PACKAGES += GoogleKoreanIME
PRODUCT_PACKAGES += memtrack.default
PRODUCT_PACKAGES += android.hardware.memtrack@1.0-impl

#ifeq ($(ENABLE_TREBLE), true)
ifneq ($(strip $(MTK_HIDL_PROCESS_CONSOLIDATION_ENABLED)), yes)
    PRODUCT_PACKAGES += android.hardware.memtrack@1.0-service
endif
#endif
# sound trigger HAL
PRODUCT_PACKAGES += \
    android.hardware.soundtrigger@2.3-impl

PRODUCT_PACKAGES += mbimd

ifndef MTK_TB_WIFI_3G_MODE
  PRODUCT_PACKAGES += Mms
else
  ifeq ($(strip $(MTK_TB_WIFI_3G_MODE)), 3GDATA_SMS)
    PRODUCT_PACKAGES += Mms
  endif
endif

PRODUCT_PROPERTY_OVERRIDES += ro.vendor.enable.geo.fencing=1

PRODUCT_PACKAGES += libjni_koreanime.so
PRODUCT_PACKAGES += wpa_supplicant
PRODUCT_PACKAGES += wpa_cli
PRODUCT_PACKAGES += wpa_supplicant.conf
PRODUCT_PACKAGES += wpa_supplicant_overlay.conf
PRODUCT_PACKAGES += p2p_supplicant_overlay.conf
PRODUCT_PACKAGES += hostapd
PRODUCT_PACKAGES += hostapd_cli
PRODUCT_PACKAGES += lib_driver_cmd_mt66xx.a
#PRODUCT_PACKAGES += Dialer
#PRODUCT_PACKAGES += CallLogBackup
PRODUCT_PACKAGES += libacdk

# camtool
PRODUCT_PACKAGES += libcam.hal3a.cctsvr
#PRODUCT_PACKAGES += camtool
PRODUCT_PACKAGES += jpegtool
#PRODUCT_PACKAGES += libmtkcam_cct

PRODUCT_PACKAGES += hwcomposer.mt6761
PRODUCT_PACKAGES += md_ctrl
#PRODUCT_PACKAGES += storagemanagerd

PRODUCT_PROPERTY_OVERRIDES += ro.vendor.composer_version=2.1

PRODUCT_PACKAGES += \
    android.hardware.graphics.composer@2.1-impl \
    android.hardware.graphics.composer@2.1-service

$(call inherit-product, device/mediatek/vendor/common/project_hal_mk/hwcomposer_2_1.mk)

ifeq ($(strip $(MTK_CCCI_PERMISSION_CHECK_SUPPORT)),yes)
PRODUCT_PACKAGES += permission_check
PRODUCT_PROPERTY_OVERRIDES += persist.md.perm.checked=to_upgrade
endif

PRODUCT_PROPERTY_OVERRIDES += ro.vendor.camera.isp-version.major=3
PRODUCT_PROPERTY_OVERRIDES += vendor.camera.mdp.cz.enable=1
#PRODUCT_PACKAGES += libGLES_android

ifneq ($(MTK_BASIC_PACKAGE), yes)
PRODUCT_PACKAGES += guiext-server
endif

PRODUCT_PACKAGES += fstab.mt6761
PRODUCT_PACKAGES += fstab.mt6761.ramdisk

ifeq ($(strip $(BUILD_MTK_LDVT)),yes)
  #PRODUCT_PACKAGES += ts_uvvf
endif

ifeq ($(strip $(MTK_APP_GUIDE)),yes)
  PRODUCT_PACKAGES += ApplicationGuide
endif

ifneq ($(strip $(foreach value,$(DFO_NVRAM_SET),$(filter yes,$($(value))))),)
  PRODUCT_PACKAGES += featured
  PRODUCT_PACKAGES += libdfo
  PRODUCT_PACKAGES += libdfo_jni
endif

ifeq ($(strip $(MTK_EMMC_SUPPORT)), yes)
else ifeq ($(strip $(MTK_UFS_BOOTING)),yes)
else
  ifeq ($(strip $(MTK_NAND_UBIFS_SUPPORT)), yes)
  else
  endif
endif

ifeq ($(strip $(MTK_EMULATOR_SUPPORT)),yes)
  PRODUCT_PACKAGES += SDKGallery
else
  #PRODUCT_PACKAGES += Gallery2
  #PRODUCT_PACKAGES += Gallery2Root
  #PRODUCT_PACKAGES += Gallery2Drm
  #PRODUCT_PACKAGES += Gallery2Gif
  #PRODUCT_PACKAGES += Gallery2Pq
  #PRODUCT_PACKAGES += Gallery2PqTool
  #PRODUCT_PACKAGES += Gallery2Raw
  #PRODUCT_PACKAGES += Gallery2SlowMotion
  #PRODUCT_PACKAGES += Gallery2StereoEntry
  #PRODUCT_PACKAGES += Gallery2StereoCopyPaste
  #PRODUCT_PACKAGES += Gallery2StereoBackground
  #PRODUCT_PACKAGES += Gallery2StereoFancyColor
  #PRODUCT_PACKAGES += Gallery2StereoRefocus
  #PRODUCT_PACKAGES += Gallery2PhotoPicker
endif

ifneq ($(strip $(MTK_EMULATOR_SUPPORT)),yes)
  #PRODUCT_PACKAGES += Provision
endif

ifeq ($(strip $(HAVE_CMMB_FEATURE)), yes)
  PRODUCT_PACKAGES += CMMBPlayer
endif

ifeq ($(strip $(MTK_DATA_TRANSFER_APP)), yes)
  PRODUCT_PACKAGES += DataTransfer
endif

ifeq ($(strip $(MTK_MDM_APP)),yes)
  PRODUCT_PACKAGES += MediatekDM
endif

ifeq ($(strip $(MTK_VT3G324M_SUPPORT)),yes)
  PRODUCT_PACKAGES += libmtk_vt_client
  PRODUCT_PACKAGES += libmtk_vt_em
  PRODUCT_PACKAGES += libmtk_vt_utils
  #PRODUCT_PACKAGES += libmtk_vt_service
  PRODUCT_PACKAGES += libmtk_vt_swip
  #PRODUCT_PACKAGES += vtservice
endif

ifeq ($(strip $(MTK_OOBE_APP)),yes)
  PRODUCT_PACKAGES += OOBE
endif

ifdef MTK_WEATHER_PROVIDER_APP
  ifneq ($(strip $(MTK_WEATHER_PROVIDER_APP)), no)
    PRODUCT_PACKAGES += MtkWeatherProvider
  endif
endif

ifeq ($(strip $(MTK_ENABLE_VIDEO_EDITOR)),yes)
  PRODUCT_PACKAGES += VideoEditor
endif

ifeq ($(strip $(MTK_CALENDAR_IMPORTER_APP)), yes)
  ifneq ($(strip $(BUILD_GMS)), yes)
    #PRODUCT_PACKAGES += CalendarImporter
  endif
endif

ifeq ($(strip $(MTK_LOG2SERVER_APP)), yes)
  PRODUCT_PACKAGES += Log2Server
  PRODUCT_PACKAGES += Excftpcommonlib
  PRODUCT_PACKAGES += Excactivationlib
  PRODUCT_PACKAGES += Excadditionnallib
  PRODUCT_PACKAGES += Excmaillib
endif

ifeq ($(strip $(MTK_CAMERA_APP)), yes)
  PRODUCT_PACKAGES += CameraOpen
else
  #PRODUCT_PACKAGES += Camera
  PRODUCT_PACKAGES += Panorama
  PRODUCT_PACKAGES += NativePip
  PRODUCT_PACKAGES += SlowMotion
  PRODUCT_PACKAGES += CameraRoot
endif

ifeq ($(strip $(MTK_VIDEO_FAVORITES_WIDGET_APP)), yes)
  ifneq ($(strip $(MTK_TABLET_PLATFORM)), yes)
    PRODUCT_PACKAGES += VideoFavorites
    PRODUCT_PACKAGES += libjtranscode
  endif
endif

ifeq ($(strip $(MTK_VIDEOWIDGET_APP)),yes)
  PRODUCT_PACKAGES += MtkVideoWidget
endif

ifeq ($(strip $(MTK_RCSE_SUPPORT)), yes)
  #PRODUCT_PACKAGES += Rcse
  PRODUCT_PACKAGES += Provisioning
endif

ifeq ($(strip $(MTK_GPS_SUPPORT)), yes)
  PRODUCT_PACKAGES += BGW
endif

ifeq ($(strip $(MTK_GPS_SUPPORT)), yes)
  ifeq ($(strip $(MTK_GPS_CHIP)), MTK_GPS_MT6620)
    PRODUCT_PROPERTY_OVERRIDES += gps.solution.combo.chip=1
  endif
  ifeq ($(strip $(MTK_GPS_CHIP)), MTK_GPS_MT6628)
    PRODUCT_PROPERTY_OVERRIDES += gps.solution.combo.chip=1
  endif
  ifeq ($(strip $(MTK_GPS_CHIP)), MTK_GPS_MT3332)
    PRODUCT_PROPERTY_OVERRIDES += gps.solution.combo.chip=0
  endif
endif

PRODUCT_COPY_FILES += $(LOCAL_PATH)/MNL_Config.xml:$(TARGET_COPY_OUT_VENDOR)/etc/MNL_Config.xml:mtk

ifeq ($(strip $(MTK_NAND_UBIFS_SUPPORT)),yes)
  PRODUCT_PACKAGES += mkfs_ubifs
  PRODUCT_PACKAGES += ubinize
  PRODUCT_PACKAGES += mtdinfo
  PRODUCT_PACKAGES += ubiupdatevol
  PRODUCT_PACKAGES += ubirmvol
  PRODUCT_PACKAGES += ubimkvol
  PRODUCT_PACKAGES += ubidetach
  PRODUCT_PACKAGES += ubiattach
  PRODUCT_PACKAGES += ubinfo
  PRODUCT_PACKAGES += ubiformat
endif

ifeq ($(strip $(MTK_EXTERNAL_MODEM_SLOT)),2)
else
  ifeq ($(strip $(MTK_EXTERNAL_MODEM_SLOT)),1)
  else
  endif
endif

ifeq ($(strip $(MTK_LIVEWALLPAPER_APP)), yes)
  PRODUCT_PACKAGES += LiveWallpapers
  #PRODUCT_PACKAGES += LiveWallpapersPicker
  PRODUCT_PACKAGES += MagicSmokeWallpapers
  PRODUCT_PACKAGES += VisualizationWallpapers
  PRODUCT_PACKAGES += Galaxy4
  PRODUCT_PACKAGES += HoloSpiralWallpaper
  PRODUCT_PACKAGES += NoiseField
  PRODUCT_PACKAGES += PhaseBeam
endif

ifeq ($(strip $(MTK_SNS_SUPPORT)), yes)
  PRODUCT_PACKAGES += SNSCommon
  PRODUCT_PACKAGES += SnsContentProvider
  PRODUCT_PACKAGES += SnsWidget
  PRODUCT_PACKAGES += SnsWidget24
  PRODUCT_PACKAGES += SocialStream
  ifeq ($(strip $(MTK_SNS_KAIXIN_APP)), yes)
    PRODUCT_PACKAGES += KaiXinAccountService
  endif
  ifeq ($(strip $(MTK_SNS_RENREN_APP)), yes)
    PRODUCT_PACKAGES += RenRenAccountService
  endif
  ifeq ($(strip $(MTK_SNS_FACEBOOK_APP)), yes)
    PRODUCT_PACKAGES += FacebookAccountService
  endif
  ifeq ($(strip $(MTK_SNS_FLICKR_APP)), yes)
    PRODUCT_PACKAGES += FlickrAccountService
  endif
  ifeq ($(strip $(MTK_SNS_TWITTER_APP)), yes)
    PRODUCT_PACKAGES += TwitterAccountService
  endif
  ifeq ($(strip $(MTK_SNS_SINAWEIBO_APP)), yes)
    PRODUCT_PACKAGES += WeiboAccountService
  endif
endif

ifeq ($(strip $(MTK_DATADIALOG_APP)), yes)
  PRODUCT_PACKAGES += DataDialog
endif

ifeq ($(strip $(MTK_DATA_TRANSFER_APP)), yes)
  PRODUCT_PACKAGES += DataTransfer
endif

ifeq ($(strip $(MTK_FM_SUPPORT)), yes)
  #PRODUCT_PACKAGES += FMRadio
endif

ifeq (MT6620_FM,$(strip $(MTK_FM_CHIP)))
  PRODUCT_PROPERTY_OVERRIDES += fmradio.driver.chip=1
endif

ifeq (MT6626_FM,$(strip $(MTK_FM_CHIP)))
  PRODUCT_PROPERTY_OVERRIDES += fmradio.driver.chip=2
endif

ifeq (MT6628_FM,$(strip $(MTK_FM_CHIP)))
  PRODUCT_PROPERTY_OVERRIDES += fmradio.driver.chip=3
endif


RAT_CONFIG = $(strip $(MTK_PROTOCOL1_RAT_CONFIG))
ifneq (, $(RAT_CONFIG))
  ifneq (,$(findstring C,$(RAT_CONFIG)))
    #C2K is supported
    RAT_CONFIG_C2K_SUPPORT=yes
  endif
endif

#for continuous shot
PRODUCT_PROPERTY_OVERRIDES += vendor.mtkcamapp.cshot.platform=-1

ifeq ($(strip $(MTK_DT_SUPPORT)),yes)
  ifneq ($(strip $(RAT_CONFIG_C2K_SUPPORT)),yes)
    ifeq ($(strip $(MTK_MDLOGGER_SUPPORT)),yes)
      PRODUCT_PACKAGES += ExtModemLog
      PRODUCT_PACKAGES += libextmdlogger_ctrl_jni
      PRODUCT_PACKAGES += libextmdlogger_ctrl
      PRODUCT_PACKAGES += extmdlogger
    endif
  endif
endif

ifneq ($(strip $(MTK_LCM_PHYSICAL_ROTATION)),)
endif

ifeq ($(strip $(MTK_FM_TX_SUPPORT)), yes)
  PRODUCT_PACKAGES += FMTransmitter
endif

ifeq ($(strip $(MTK_LOCKSCREEN_TYPE)),2)
  PRODUCT_PACKAGES += MtkWallPaper
endif

ifneq ($(strip $(MTK_LOCKSCREEN_TYPE)),)
  PRODUCT_PROPERTY_OVERRIDES += curlockscreen=$(MTK_LOCKSCREEN_TYPE)
endif


  PRODUCT_PACKAGES += Browser
  #PRODUCT_PACKAGES += DownloadProvider

ifeq ($(strip $(MTK_WIFI_P2P_SUPPORT)),yes)
  PRODUCT_PACKAGES += WifiContactSync
  PRODUCT_PACKAGES += WifiP2PWizardy
  PRODUCT_PACKAGES += FileSharingServer
  PRODUCT_PACKAGES += FileSharingClient
  PRODUCT_PACKAGES += UPnPAV
  PRODUCT_PACKAGES += WifiWsdsrv
  PRODUCT_PACKAGES += bonjourExplorer
endif

ifneq ($(findstring OP03, $(strip $(OPTR_SPEC_SEG_DEF))),)
  PRODUCT_PACKAGES += SimCardAuthenticationService
endif

ifeq ($(strip $(MTK_APKINSTALLER_APP)), yes)
  PRODUCT_PACKAGES += APKInstaller
endif

ifeq ($(strip $(MTK_SMSREG_APP)), yes)
  PRODUCT_PACKAGES += SmsReg
endif

ifeq ($(MTK_BACKUPANDRESTORE_APP),yes)
  PRODUCT_PACKAGES += BackupAndRestore
endif

ifeq ($(strip $(MTK_BWC_SUPPORT)), yes)
  PRODUCT_PACKAGES += libbwc
endif

PRODUCT_PROPERTY_OVERRIDES += ro.hardware.egl?=mtk

ifeq ($(strip $(MTK_GPU_SUPPORT)), yes)
    PRODUCT_PACKAGES += gralloc.rogue
    PRODUCT_PACKAGES += libOpenCL
    PRODUCT_PACKAGES += libGLES_mali
    PRODUCT_PACKAGES += libgpu_aux
    PRODUCT_PACKAGES += libgpud
    PRODUCT_PACKAGES += libgralloc_metadata
    PRODUCT_PACKAGES += libgralloctypes_mtk
    PRODUCT_PACKAGES += libRSDriver_mtk
    PRODUCT_PACKAGES += rs2spir
    PRODUCT_PACKAGES += spir2cl
    PRODUCT_PACKAGES += android.hardware.graphics.mapper@4.0-impl-mediatek
    PRODUCT_PACKAGES += android.hardware.graphics.allocator@4.0-service-mediatek
    PRODUCT_PACKAGES += android.hardware.renderscript@1.0-impl
    # Vulkan: only enable for non-AGO project
    ifneq ($(strip $(MTK_GMO_RAM_OPTIMIZE)),yes)
      PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.vulkan.version-1_1.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.vulkan.version.xml
      PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.vulkan.level-0.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.vulkan.level.xml
      PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.vulkan.compute-0.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.vulkan.compute.xml
      PRODUCT_COPY_FILES += frameworks/native/data/etc/android.software.vulkan.deqp.level-2020-03-01.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.vulkan.deqp.level.xml
    endif
    PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.opengles.aep.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.opengles.aep.xml
    PRODUCT_COPY_FILES += frameworks/native/data/etc/android.software.opengles.deqp.level-2020-03-01.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.opengles.deqp.level.xml
    PRODUCT_PROPERTY_OVERRIDES += ro.opengles.version=196610
    PRODUCT_COPY_FILES += device/mediatek/mt6761/egl.cfg:$(TARGET_COPY_OUT_VENDOR)/lib/egl/egl.cfg:mtk
else
    PRODUCT_PROPERTY_OVERRIDES += ro.kernel.qemu=1
    PRODUCT_PACKAGES += android.hardware.graphics.allocator@2.0-impl
    PRODUCT_PACKAGES += android.hardware.graphics.mapper@2.0-impl-2.1
    PRODUCT_PACKAGES += android.hardware.graphics.allocator@2.0-service
    PRODUCT_PACKAGES += android.hardware.renderscript@1.0-impl
endif


ifeq ($(strip $(MTK_DT_SUPPORT)),yes)
  PRODUCT_PACKAGES += ip-up
  PRODUCT_PACKAGES += ip-down
  PRODUCT_PACKAGES += ppp_options
  PRODUCT_PACKAGES += chap-secrets
  PRODUCT_PACKAGES += init.gprs-pppd
endif

# OEM Unlock reporting
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
    ro.oem_unlock_supported=1


ifeq (yes,$(strip $(MTK_FD_SUPPORT)))
  # Only support the format: n.m (n:1 or 1+ digits, m:Only 1 digit) or n (n:integer)
  # Time Unit:0.1 sec
  PRODUCT_PROPERTY_OVERRIDES += persist.vendor.radio.fd.counter=150
  PRODUCT_PROPERTY_OVERRIDES += persist.vendor.radio.fd.off.counter=50
  PRODUCT_PROPERTY_OVERRIDES += persist.vendor.radio.fd.r8.counter=150
  PRODUCT_PROPERTY_OVERRIDES += persist.vendor.radio.fd.off.r8.counter=50
endif

#################################################
#OMA DRM part
ifeq ($(strip $(MTK_OMADRM_SUPPORT)), yes)
  #PRODUCT_PACKAGES += libdrmmtkutil
  #Media framework reads this property
  PRODUCT_PROPERTY_OVERRIDES += drm.service.enabled=true
  #PRODUCT_PACKAGES += libdcfdecoderjni
  #PRODUCT_PACKAGES += libdrmmtkplugin
  PRODUCT_PACKAGES += drm_chmod
endif
ifeq ($(strip $(MTK_CTA_SET)), yes)
  #PRODUCT_PACKAGES += libdrmmtkutil
  #Media framework reads this property
  PRODUCT_PROPERTY_OVERRIDES += drm.service.enabled=true
  #PRODUCT_PACKAGES += libdcfdecoderjni
endif

################################################

############find all modules under custom folder
#define find-all-my-module
all_sensor_modules := $(addsuffix _tuning,$(notdir $(foreach f,$(CUSTOM_HAL_IMGSENSOR),$(wildcard vendor/mediatek/proprietary/custom/mt6761/hal/imgsensor/$(f)))))

PRODUCT_PACKAGES += $(all_sensor_modules)
###############################################
ifeq (yes,$(strip $(MTK_FM_SUPPORT)))
else
endif

ifeq ($(strip $(MTK_WEATHER_WIDGET_APP)), yes)
  PRODUCT_PACKAGES += MtkWeatherWidget
endif

ifeq ($(strip $(MTK_ECCCI_C2K)),yes)
endif

ifeq ($(strip $(MTK_FIRST_MD)),1)
endif

ifeq ($(strip $(MTK_FIRST_MD)),2)
endif

ifeq ($(strip $(MTK_FLIGHT_MODE_POWER_OFF_MD)),yes)
else
endif

ifeq ($(strip $(MTK_FIRST_MD)),1)
endif

ifeq ($(strip $(MTK_FIRST_MD)),2)
endif

ifeq ($(strip $(MTK_TELEPHONY_MODE)),0)
endif

ifeq ($(strip $(MTK_TELEPHONY_MODE)),1)
endif

ifeq ($(strip $(MTK_TELEPHONY_MODE)),2)
endif

ifeq ($(strip $(MTK_TELEPHONY_MODE)),3)
endif

ifeq ($(strip $(MTK_TELEPHONY_MODE)),4)
endif

ifeq ($(strip $(MTK_TELEPHONY_MODE)),5)
endif

ifeq ($(strip $(MTK_TELEPHONY_MODE)),6)
endif

ifeq ($(strip $(MTK_TELEPHONY_MODE)),7)
endif

ifeq ($(strip $(MTK_TELEPHONY_MODE)),8)
endif

PRODUCT_PACKAGES += wifi2agps

ifeq ($(strip $(MTK_WFD_SINK_SUPPORT)), yes)
  PRODUCT_PACKAGES += MtkFloatMenu
endif

PRODUCT_PACKAGES += libsec
PRODUCT_PACKAGES += sbchk

PRODUCT_BRAND := alps
PRODUCT_MANUFACTURER := alps

# for USB Accessory Library/permission
# Mark for early porting in JB
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.usb.accessory.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.usb.accessory.xml
#PRODUCT_PACKAGES += com.android.future.usb.accessory

# System property for MediaTek ANR pre-dump.
PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.mtk-stack-trace-file=/data/anr/mtk_traces.txt

ifeq ($(strip $(MTK_WLAN_SUPPORT)),yes)
endif

ifeq ($(strip $(MTK_RILD_READ_IMSI)),yes)
endif

ifeq ($(strip $(MTK_RADIOOFF_POWER_OFF_MD)),yes)
else
endif

ifeq ($(strip $(MTK_FACTORY_RESET_PROTECTION_SUPPORT)),yes)
ifeq ($(strip $(MTK_EMMC_SUPPORT)),yes)
  PRODUCT_PROPERTY_OVERRIDES += ro.frp.pst=/dev/block/platform/bootdevice/by-name/frp
else ifeq ($(strip $(MTK_UFS_BOOTING)),yes)
  PRODUCT_PROPERTY_OVERRIDES += ro.frp.pst=/dev/block/platform/bootdevice/by-name/frp
endif
endif

PRODUCT_COPY_FILES += device/mediatek/mt6761/init.mt6761.rc:$(MTK_TARGET_VENDOR_RC)/init.mt6761.rc
PRODUCT_COPY_FILES += device/mediatek/mt6761/factory_init.rc:$(MTK_TARGET_VENDOR_RC)/factory_init.rc
PRODUCT_COPY_FILES += device/mediatek/mt6761/init.modem.rc:$(MTK_TARGET_VENDOR_RC)/init.modem.rc
PRODUCT_COPY_FILES += device/mediatek/mt6761/meta_init.modem.rc:$(MTK_TARGET_VENDOR_RC)/meta_init.modem.rc

PRODUCT_COPY_FILES += device/mediatek/mt6761/meta_init.rc:$(MTK_TARGET_VENDOR_RC)/meta_init.rc

PRODUCT_COPY_FILES += device/mediatek/mt6761/init.mt6761.usb.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/hw/init.mt6761.usb.rc
PRODUCT_COPY_FILES += device/mediatek/mt6761/init.recovery.mt6761.rc:recovery/root/init.recovery.mt6761.rc
ifneq ($(strip $(MTK_EMMC_SUPPORT)), yes)
ifneq ($(strip $(MTK_UFS_BOOTING)),yes)
PRODUCT_COPY_FILES += $(LOCAL_PATH)/fstab.mt6761.nand:root/fstab.mt6761
endif
endif



ifeq ($(TARGET_BUILD_VARIANT),eng)
    PRODUCT_COPY_FILES += $(LOCAL_PATH)/thermal.eng.conf:$(TARGET_COPY_OUT_VENDOR)/etc/.tp/thermal.conf:mtk
else
    PRODUCT_COPY_FILES += $(LOCAL_PATH)/thermal.conf:$(TARGET_COPY_OUT_VENDOR)/etc/.tp/thermal.conf:mtk
endif
    PRODUCT_COPY_FILES += $(LOCAL_PATH)/thermal.wfd.6761.conf:$(TARGET_COPY_OUT_VENDOR)/etc/.tp/.thermal_policy_00:mtk
    PRODUCT_COPY_FILES += $(LOCAL_PATH)/thermal.vsdof.6761.conf:$(TARGET_COPY_OUT_VENDOR)/etc/.tp/.thermal_policy_03:mtk


PRODUCT_COPY_FILES += $(LOCAL_PATH)/thermal.off.conf:$(TARGET_COPY_OUT_VENDOR)/etc/.tp/thermal.off.conf:mtk
PRODUCT_COPY_FILES += $(LOCAL_PATH)/ht120.mtc:$(TARGET_COPY_OUT_VENDOR)/etc/.tp/.ht120.mtc:mtk





PRODUCT_COPY_FILES += $(LOCAL_PATH)/partition_permission.sh:$(TARGET_COPY_OUT_VENDOR)/etc/partition_permission.sh:mtk
PRODUCT_COPY_FILES += $(LOCAL_PATH)/throttle.sh:$(TARGET_COPY_OUT_VENDOR)/etc/throttle.sh:mtk

PRODUCT_COPY_FILES += device/mediatek/mt6761/media_codecs.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs.xml:mtk

ifneq ($(MTK_BASIC_PACKAGE), yes)
    ifeq ($(strip $(MTK_AUDIO_CODEC_SUPPORT_TABLET)), yes)
        ifeq ($(strip $(MTK_WMA_PLAYBACK_SUPPORT)), yes)
            PRODUCT_COPY_FILES += device/mediatek/mt6761/media_codecs_mediatek_audio_tablet.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_mediatek_audio.xml:mtk
        else
            PRODUCT_COPY_FILES += device/mediatek/mt6761/media_codecs_mediatek_audio_tablet_no_wma.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_mediatek_audio.xml:mtk
        endif
    else ifeq ($(strip $(MTK_GMO_RAM_OPTIMIZE)), yes)
        PRODUCT_COPY_FILES += device/mediatek/mt6761/media_codecs_mediatek_audio_phone_ago.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_mediatek_audio.xml:mtk
    else
        PRODUCT_COPY_FILES += device/mediatek/mt6761/media_codecs_mediatek_audio_phone.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_mediatek_audio.xml:mtk
    endif
else
    PRODUCT_COPY_FILES += device/mediatek/mt6761/media_codecs_mediatek_audio_basic.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_mediatek_audio.xml:mtk
endif

ifeq (yes,$(strip $(MTK_SEC_VIDEO_PATH_SUPPORT)))
    PRODUCT_COPY_FILES += device/mediatek/mt6761/media_codecs_mediatek_video_svp.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_mediatek_video.xml:mtk
    PRODUCT_COPY_FILES += device/mediatek/mt6761/media_codecs_c2_svp.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_c2.xml:mtk
    ifeq ($(strip $(MTK_AUDIO_CODEC_SUPPORT_TABLET)), yes)
        PRODUCT_COPY_FILES += device/mediatek/mt6761/mtk_omx_core_svp_tablet.cfg:$(TARGET_COPY_OUT_VENDOR)/etc/mtk_omx_core.cfg:mtk
    else
        PRODUCT_COPY_FILES += device/mediatek/mt6761/mtk_omx_core_svp.cfg:$(TARGET_COPY_OUT_VENDOR)/etc/mtk_omx_core.cfg:mtk
    endif
else
    ifeq (yes,$(strip $(MTK_GMO_RAM_OPTIMIZE)))
        ifeq (0x40000000,$(strip $(CUSTOM_CONFIG_MAX_DRAM_SIZE)))
            PRODUCT_COPY_FILES += device/mediatek/mt6761/media_codecs_mediatek_video_ago_1g.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_mediatek_video.xml:mtk
            PRODUCT_COPY_FILES += device/mediatek/mt6761/media_codecs_c2_ago_1g.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_c2.xml:mtk
        else ifeq (0x80000000,$(strip $(CUSTOM_CONFIG_MAX_DRAM_SIZE)))
            PRODUCT_COPY_FILES += device/mediatek/mt6761/media_codecs_mediatek_video_ago_2g.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_mediatek_video.xml:mtk
            PRODUCT_COPY_FILES += device/mediatek/mt6761/media_codecs_c2_ago_2g.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_c2.xml:mtk
        endif
    else
        PRODUCT_COPY_FILES += device/mediatek/mt6761/media_codecs_mediatek_video.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_mediatek_video.xml:mtk
        PRODUCT_COPY_FILES += device/mediatek/mt6761/media_codecs_c2.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_c2.xml:mtk
    endif
    ifeq ($(strip $(MTK_AUDIO_CODEC_SUPPORT_TABLET)), yes)
        ifeq ($(strip $(MTK_WMA_PLAYBACK_SUPPORT)), yes)
            PRODUCT_COPY_FILES += device/mediatek/mt6761/mtk_omx_core_tablet.cfg:$(TARGET_COPY_OUT_VENDOR)/etc/mtk_omx_core.cfg:mtk
        else
            PRODUCT_COPY_FILES += device/mediatek/mt6761/mtk_omx_core_tablet_no_wma.cfg:$(TARGET_COPY_OUT_VENDOR)/etc/mtk_omx_core.cfg:mtk
        endif

    else
        ifeq ($(strip $(MTK_GMO_RAM_OPTIMIZE)), yes)
            PRODUCT_COPY_FILES += device/mediatek/mt6761/mtk_omx_core_gmo.cfg:$(TARGET_COPY_OUT_VENDOR)/etc/mtk_omx_core.cfg:mtk
        else
            PRODUCT_COPY_FILES += device/mediatek/mt6761/mtk_omx_core.cfg:$(TARGET_COPY_OUT_VENDOR)/etc/mtk_omx_core.cfg:mtk
        endif

    endif
endif
ifeq (yes,$(strip $(MTK_GMO_RAM_OPTIMIZE)))
    PRODUCT_COPY_FILES += device/mediatek/mt6761/mtk_platform_codecs_config_gmo.xml:$(TARGET_COPY_OUT_VENDOR)/etc/mtk_platform_codecs_config.xml:mtk
else
    PRODUCT_COPY_FILES += device/mediatek/mt6761/mtk_platform_codecs_config.xml:$(TARGET_COPY_OUT_VENDOR)/etc/mtk_platform_codecs_config.xml:mtk
endif
PRODUCT_COPY_FILES += device/mediatek/mt6761/media_codecs_performance.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_performance.xml:mtk
PRODUCT_COPY_FILES += frameworks/av/media/libstagefright/data/media_codecs_google_audio.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_google_audio.xml
PRODUCT_COPY_FILES += frameworks/av/media/libstagefright/data/media_codecs_google_video_le.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_google_video_le.xml

ifeq (yes,$(strip $(MTK_GMO_RAM_OPTIMIZE)))
    ifeq (0x40000000,$(strip $(CUSTOM_CONFIG_MAX_DRAM_SIZE)))
        PRODUCT_COPY_FILES += device/mediatek/mt6761/media_codecs_c2_ago_1g.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_c2.xml:mtk
    else
        PRODUCT_COPY_FILES += device/mediatek/mt6761/media_codecs_c2.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_c2.xml:mtk
    endif
else
    PRODUCT_COPY_FILES += device/mediatek/mt6761/media_codecs_c2.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_c2.xml:mtk
endif

PRODUCT_COPY_FILES += vendor/mediatek/proprietary/hardware/omx/mediacodec/android.hardware.media.omx@1.0-service.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/android.hardware.media.omx@1.0-service.rc

ifneq ($(MTK_BASIC_PACKAGE), yes)
PRODUCT_COPY_FILES += vendor/mediatek/proprietary/hardware/libc2/service/android.hardware.media.c2@1.2-mediatek.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/android.hardware.media.c2@1.2-mediatek.rc
PRODUCT_COPY_FILES += vendor/mediatek/proprietary/hardware/libc2/service/seccomp_policy/android.hardware.media.c2@1.2-extended-seccomp-policy:$(TARGET_COPY_OUT_VENDOR)/etc/seccomp_policy/android.hardware.media.c2@1.2-extended-seccomp-policy
endif

# media_profiles.xml for media profile support
ifeq ($(strip $(MTK_CAM_MAX_NUMBER_OF_CAMERA)), 5)
    PRODUCT_COPY_FILES += device/mediatek/mt6761/media_profiles_main3.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_profiles_V1_0.xml:mtk
else
ifeq ($(MTK_CAM_STEREO_CAMERA_SUPPORT),yes)
    PRODUCT_COPY_FILES += device/mediatek/mt6761/media_profiles_ducam.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_profiles_V1_0.xml:mtk
else
    ifeq (yes,$(strip $(MTK_GMO_RAM_OPTIMIZE)))
        ifeq (0x40000000,$(strip $(CUSTOM_CONFIG_MAX_DRAM_SIZE)))
            PRODUCT_COPY_FILES += device/mediatek/mt6761/media_profiles_ago_1g.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_profiles_V1_0.xml:mtk
        else ifeq (0x80000000,$(strip $(CUSTOM_CONFIG_MAX_DRAM_SIZE)))
            PRODUCT_COPY_FILES += device/mediatek/mt6761/media_profiles_ago_2g.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_profiles_V1_0.xml:mtk
        endif
    else
        PRODUCT_COPY_FILES += device/mediatek/mt6761/media_profiles.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_profiles_V1_0.xml:mtk
    endif
endif
endif

ifneq ($(wildcard vendor/mediatek/proprietary/custom/mt6761/hal/pd_buf_mgr/pd_calibration/default_pd_calibration.bin),)
    PRODUCT_COPY_FILES += vendor/mediatek/proprietary/custom/mt6761/hal/pd_buf_mgr/pd_calibration/default_pd_calibration.bin:$(TARGET_COPY_OUT_VENDOR)/etc/default_pd_calibration.bin:mtk
endif

ifeq ($(MTK_CLEARMOTION_SUPPORT),yes)
PRODUCT_COPY_FILES += device/mediatek/mt6761/mtk_clear_motion.cfg:$(TARGET_COPY_OUT_VENDOR)/etc/mtk_clear_motion.cfg:mtk
endif

#Audio config
PRODUCT_COPY_FILES += device/mediatek/mt6761/audio_device.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_device.xml:mtk

#Audio low latency
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.hardware.audio.low_latency.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.audio.low_latency.xml

#aurisys library parameters
ifeq ($(MTK_AURISYS_PHONE_CALL_SUPPORT),yes)
PRODUCT_PACKAGES += libfvaudio
#PRODUCT_PACKAGES += AudioSetParam
PRODUCT_COPY_FILES += $(LOCAL_PATH)/aurisys_param/phone_call/forte_media/FV-SAM-MTK2.dat:$(TARGET_COPY_OUT_VENDOR)/etc/FV-SAM-MTK2.dat:mtk
endif

# AUDIO kernel buffer size
AUDIO_PARAM_OPTIONS_LIST += KERNEL_BUFFER_SIZE_NORMAL=32768
AUDIO_PARAM_OPTIONS_LIST += KERNEL_BUFFER_SIZE_DEEP=32768

# Audio primary and fast using the same output
AUDIO_PARAM_OPTIONS_LIST += HAL_PRIMARY_FAST_SUPPORT=yes

ifeq ($(strip $(TRUSTONIC_TEE_SUPPORT)), yes)
  PRODUCT_PACKAGES += keystore.mt6761
  PRODUCT_PACKAGES += libMcGatekeeper
  PRODUCT_PACKAGES += rda
endif

# USB ADB
PRODUCT_PROPERTY_OVERRIDES += \
    persist.adb.nonblocking_ffs=1

ifneq ($(MTK_AUDIO_TUNING_TOOL_VERSION),)
    ifneq ($(strip $(MTK_AUDIO_TUNING_TOOL_VERSION)),V1)
        MTK_AUDIO_PARAM_DIR_LIST += device/mediatek/mt6761/audio_param
        # Speech Parameter Tuning
        # SPH_PARAM_VERSION: 0 support single network(MD ability related)
        # SPH_PARAM_VERSION: 1.0 support multiple networks(MD ability related)
        # SPH_PARAM_VERSION: 2.0 support IIR and fix WBFIR(Gen93)
        AUDIO_PARAM_OPTIONS_LIST += SPH_PARAM_VERSION=2.0
    endif
endif


ifeq ($(strip $(MTK_TEE_TRUSTED_UI_SUPPORT)), yes)
  PRODUCT_PACKAGES += libTui
  PRODUCT_PACKAGES += TuiService
  PRODUCT_PACKAGES += SamplePinpad
  PRODUCT_PACKAGES += libTlcPinpad
endif

ifeq ($(strip $(MICROTRUST_TEE_SUPPORT)), yes)
PRODUCT_PACKAGES   += 	keystore.mt6761
PRODUCT_PACKAGES   += 	gatekeeper.mt6761
PRODUCT_PACKAGES   += 	kmsetkey.mt6761
endif

# userspace sysenv
PRODUCT_PACKAGES += libsysenv
PRODUCT_PACKAGES += sysenv_daemon

ifeq ($(strip $(MTK_AOD_SUPPORT)), yes)
#PRODUCT_PACKAGES += AlwaysOnDisplay
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += ro.vendor.mtk_aod_support=1
endif

# camera hal3 zsl default backtrace timestamp
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.camera3.zsl.default=140

PRODUCT_PROPERTY_OVERRIDES += ro.vendor.mediatek.platform=MT6761

# heap parameters
PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.heapmaxfree=8m
PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.heapminfree=512k
PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.heaptargetutilization=0.75

# for logd filter
ifeq ($(OPTR_SPEC_SEG_DEF),OP03_SPEC0200_SEGDEFAULT)
ifeq ($(TARGET_BUILD_VARIANT),user)
PRODUCT_PROPERTY_OVERRIDES += persist.log.tag=I
endif
endif

# touch related file for copy firmware
ifeq (GT1XX,$(strip $(CUSTOM_KERNEL_TOUCHPANEL)))
  PRODUCT_COPY_FILES += vendor/mediatek/proprietary/custom/touch/GT1151/gt1151_default_firmware2.img:$(TARGET_COPY_OUT_VENDOR)/firmware/gt1151_default_firmware2.img:mtk
endif

#CCU dependencies
PRODUCT_PACKAGES += lib3a.ccu.dm
PRODUCT_PACKAGES += lib3a.ccu.pm

# add vintf utility
#PRODUCT_PACKAGES += \
#    vintf

# Bluetooth HAL
PRODUCT_PACKAGES += \
    android.hardware.bluetooth@1.1-impl-mediatek \
    android.hardware.bluetooth@1.1-service-mediatek

PRODUCT_PACKAGES += \
    fs_config_files

# vendor hidl process merging together to save memory
ifeq ($(strip $(MTK_HIDL_PROCESS_CONSOLIDATION_ENABLED)), yes)
    PRODUCT_PACKAGES += merged_hal_service
endif

# vibrator HAL
PRODUCT_PACKAGES += \
   libvibratormediatekimpl \
   android.hardware.vibrator-service.mediatek

# light HAL
PRODUCT_PACKAGES += \
    android.hardware.lights-service.mediatek

# thermal HIDL
PRODUCT_PACKAGES += \
    android.hardware.thermal@1.0-impl
PRODUCT_PACKAGES += \
    android.hardware.thermal@2.0-impl
ifneq ($(strip $(MTK_HIDL_PROCESS_CONSOLIDATION_ENABLED)), yes)
PRODUCT_PACKAGES += \
    android.hardware.thermal@2.0-service.mtk
endif
PRODUCT_PACKAGES += \
        android.hardware.oemlock@1.0-service \
        android.hardware.oemlock@1.0-impl

# mtk av enhance
#PRODUCT_PACKAGES += libmtkavenhancements

# for fpsgo fbt game
PRODUCT_PACKAGES += fpsgo-user.ko fpsgo-eng.ko
PRODUCT_PACKAGES += fpsgo.ko

# for 4cell flow
PRODUCT_PACKAGES += libcameracustom.plugin

# mtk virtual display enhance
PRODUCT_PACKAGES += libvd_enhance

# Usb HAL
PRODUCT_PACKAGES += \
    android.hardware.usb@1.2-service-mediatekv2

# mtk c2 hal service
PRODUCT_PACKAGES += android.hardware.media.c2@1.2-mediatek

#tinyalsa tool
ifneq ($(filter $(TARGET_BUILD_VARIANT),eng userdebug),)
#PRODUCT_PACKAGES += \
    #tinyplay \
    #tinycap \
    #tinymix \
    #tinyhostless \
    #tinypcminfo
endif

# lmkd parameters
ifneq ($(strip $(MTK_GMO_RAM_OPTIMIZE)), yes)
PRODUCT_PROPERTY_OVERRIDES += \
     ro.lmk.swap_free_low_percentage=20 \
     ro.lmk.kill_timeout_ms=100

PRODUCT_PROPERTY_OVERRIDES += ro.lmk.psi_complete_stall_ms?=150
PRODUCT_PROPERTY_OVERRIDES += ro.lmk.thrashing_limit?=30
PRODUCT_PROPERTY_OVERRIDES += ro.lmk.thrashing_limit_decay?=50

endif

# for Gen93 log tag
$(call inherit-product, device/mediatek/vendor/common/RilGen93LogTag.mk)

#inherit common platform
$(call inherit-product, device/mediatek/vendor/common/device.mk)

# Touch feature: touch ko
ifeq (kernel-4.19,$(strip $(LINUX_KERNEL_VERSION)))
PRODUCT_PACKAGES += gt1151.ko
endif

# fingerprint feature: fingerprint ko
PRODUCT_PACKAGES += gf_spi.ko

#PRODUCT_DEFAULT_PROPERTY_OVERRIDES += debug.sf.late.sf.duration=27600000
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += debug.sf.late.app.duration=25500000
#PRODUCT_DEFAULT_PROPERTY_OVERRIDES += debug.sf.early.sf.duration=27600000
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += debug.sf.early.app.duration=25500000
#PRODUCT_DEFAULT_PROPERTY_OVERRIDES += debug.sf.earlyGl.sf.duration=27600000
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += debug.sf.earlyGl.app.duration=25500000

#inherit prebuilt
$(call inherit-product-if-exists, vendor/mediatek/mt6761/device-vendor.mk)

PRODUCT_COPY_FILES += device/mediatek/mt6761/init.cgroup-$(LINUX_KERNEL_VERSION).rc:$(MTK_TARGET_VENDOR_RC)/init.cgroup.rc
PRODUCT_COPY_FILES += device/mediatek/mt6761/task_profiles-$(LINUX_KERNEL_VERSION).json:$(TARGET_COPY_OUT_VENDOR)/etc/task_profiles.json
ifeq ($(LINUX_KERNEL_VERSION),kernel-4.19)
    PRODUCT_COPY_FILES += device/mediatek/mt6761/cgroups-$(LINUX_KERNEL_VERSION).json:$(TARGET_COPY_OUT_VENDOR)/etc/cgroups.json
endif
