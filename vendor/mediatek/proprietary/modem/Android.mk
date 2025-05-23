define mtk-install-modem
include $$(CLEAR_VARS)
LOCAL_MODULE := $$(notdir $(1))
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_CLASS := ETC
LOCAL_POST_INSTALL_CMD := $(3)
LOCAL_MODULE_PATH := $(2)
LOCAL_SRC_FILES := $(1)
include $$(BUILD_PREBUILT)
MTK_MODEM_INSTALLED_MODULES += $$(LOCAL_INSTALLED_MODULE)
MTK_MODEM_BUILT_MODULES += $$(LOCAL_BUILT_MODULE)
endef

define mtk-install-mdimg
include $$(CLEAR_VARS)
LOCAL_MODULE := $(3)
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(2)
LOCAL_SRC_FILES := $(1)
include $$(BUILD_PREBUILT)
MTK_MODEM_INSTALLED_MODULES += $$(LOCAL_INSTALLED_MODULE)
MTK_MODEM_BUILT_MODULES += $$(LOCAL_BUILT_MODULE)
endef

define mtk-install-ims
include $$(CLEAR_VARS)
LOCAL_MODULE := $(strip $(1))
LOCAL_MODULE_SUFFIX := $(strip $(2))
LOCAL_MODULE_CLASS := $(strip $(3))
LOCAL_SRC_FILES_arm64 := $(strip $(4))$(strip $(1))$(strip $(2))
LOCAL_SRC_FILES_arm   := $(strip $(5))$(strip $(1))$(strip $(2))
LOCAL_INIT_RC := $(patsubst init_rc/%,init_rc/$(MTK_MODEM_INIT_RC_VERSION)/%,$(6))
LOCAL_MULTILIB := $(if $(filter EXECUTABLES,$(3)),$(if $(strip $(4)),first,32),$(if $(strip $(4)),both,32))
LOCAL_SHARED_LIBRARIES := $(if $(filter 1,$(8)),$(strip $(7)))
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_CHECK_ELF_FILES := false
include $$(BUILD_PREBUILT)
endef

define mtk-install-mdota
include $$(CLEAR_VARS)
LOCAL_MODULE := $(notdir $(strip $(1)))
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_RELATIVE_PATH := mdota
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
include $$(BUILD_SYSTEM)/base_rules.mk
$$(LOCAL_BUILT_MODULE): $(2)
	mkdir -p $$(dir $$@)
	PATH=prebuilts/build-tools/path/linux-x86 tar -C $$(dir $$@) -zmxf $(2) $(1)
endef

define mtk-install-mdota-img
include $$(CLEAR_VARS)
LOCAL_MODULE := $(subst MCF_OTA_IMG_FILES,mcf_ota,$(subst .tar.gz,,$(notdir $(1)))).img
LOCAL_MODULE_PATH := $(PRODUCT_OUT)
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_OWNER := mtk
LOCAL_POST_INSTALL_CMD := ln -sf /mnt/vendor/mdota $(TARGET_OUT_VENDOR_ETC)/
LOCAL_PROPRIETARY_MODULE := true
LOCAL_REQUIRED_MODULES := mkuserimg_mke2fs
include $$(BUILD_SYSTEM)/base_rules.mk
$$(LOCAL_BUILT_MODULE): $(MKEXTUSERIMG)
$$(LOCAL_BUILT_MODULE): $(1)
	mkdir -p $$(dir $$@)$(subst .tar.gz,,$(notdir $(1)))
	PATH=prebuilts/build-tools/path/linux-x86 tar -C $$(dir $$@)$(subst .tar.gz,,$(notdir $(1))) -zmxf $(1)
	rm -f $$(dir $$@)$(subst .tar.gz,,$(notdir $(1)))/*/*.xml
	$(MKEXTUSERIMG) $$(dir $$@)$(subst .tar.gz,,$(notdir $(1))) $$(dir $$@)$(subst MCF_OTA_IMG_FILES,mcf_ota,$(subst .tar.gz,,$(notdir $(1)))).img ext4 vendor 3409600
ALL_DEFAULT_INSTALLED_MODULES += $$(LOCAL_INSTALLED_MODULE)
endef


# $(1): output binary
# $(2): input binary
# $(3): input cfg
define sign-one-modem-binary
$(1): $(2) vendor/mediatek/proprietary/scripts/sign-image_v2/mkimage20/mkimage $(3)
	@mkdir -p $$(dir $$@)
	./vendor/mediatek/proprietary/scripts/sign-image_v2/mkimage20/mkimage $(2) $(3) > $$@

endef

$(1): output binary
$(2): image name
define sign-one-modem-cfg
$(1):
	@mkdir -p $$(dir $$@)
	@echo "NAME = $(2)" > $$@
	@echo "IMG_LIST_END = 0" >> $$@
	@echo "ALIGN_SIZE = 16" >> $$@

endef

MTK_MODEM_INIT_RC_VERSION := s0
MTK_MODEM_INSTALLED_MODULES :=
MTK_MODEM_BUILT_MODULES :=
MTK_MODEM_DATABASE_FILES :=
MTK_MODEM_APPS_FILES :=
MTK_MODEM_MCF_OTA_FILES :=
MTK_MD1_SINGLEBIN_FILES :=
MTK_MD1_DSP_FILES :=
MTK_MD1_IMG_FILES :=
MTK_MD1_FILTER_FILES :=
MTK_MD1_MDDB_FILES :=
MTK_MD1_META_FILES :=
MTK_MD1_DBGINFO_DSP_FILES :=
MTK_MD1_DBGINFO_FILES :=
MTK_MD1_EM_FILTER_FILES :=
MTK_MD1_M_LAYOUT_FILES :=
MTK_MD1_FILE_MAP_LINES :=
MTK_MD3_PACK_FILES :=
MTK_MD3_IMG_FILES :=
MTK_MD3_MDDB_FILES :=
MTK_MD3_META_FILES :=

LOCAL_PATH := $(call my-dir)
MTK_PATH_MODEM := $(LOCAL_PATH)

ifneq ($(strip $(CUSTOM_MODEM)),)
  MTK_MODEM_USER_SUFFIX ?= _prod
  ifeq ($(strip $(TARGET_BUILD_VARIANT)),eng)
    MTK_MODEM_MODULE_MAKEFILES := $(foreach item,$(CUSTOM_MODEM),$(firstword $(wildcard $(LOCAL_PATH)/$(patsubst %$(MTK_MODEM_USER_SUFFIX),%,$(item))/Android.mk $(LOCAL_PATH)/$(item)/Android.mk)))
  else
    MTK_MODEM_MODULE_MAKEFILES := $(foreach item,$(CUSTOM_MODEM),$(firstword $(wildcard $(LOCAL_PATH)/$(patsubst %$(MTK_MODEM_USER_SUFFIX),%,$(item))$(MTK_MODEM_USER_SUFFIX)/Android.mk $(LOCAL_PATH)/$(item)/Android.mk)))
  endif

$(info including $(MTK_MODEM_MODULE_MAKEFILES) ...)
include $(wildcard $(MTK_MODEM_MODULE_MAKEFILES))
MTK_MODEM_DATABASE_FILES := $(MTK_MODEM_INSTALLED_MODULES)

ifeq ($(strip $(MTK_SINGLE_BIN_MODEM_SUPPORT)),yes)
intermediates := $(call intermediates-dir-for,PACKAGING,md1img)
INSTALLED_MD1IMG_TARGET := $(PRODUCT_OUT)/md1img.img

ifneq ($(strip $(MTK_MD1_IMG_FILES)),)
MTK_MODEM_INSTALLED_MODULES += $(INSTALLED_MD1IMG_TARGET)
MTK_MODEM_DATABASE_FILES += $(INSTALLED_MD1IMG_TARGET)

ifeq ($(words $(MTK_MD1_SINGLEBIN_FILES)),1)

$(INSTALLED_MD1IMG_TARGET): $(MTK_MD1_SINGLEBIN_FILES)
	@mkdir -p $(dir $@)
	cp -f $< $@

else#MTK_MD1_SINGLEBIN_FILES

MTK_MD1_DSP_FILES := $(strip $(MTK_MD1_DSP_FILES))
MTK_MD1_IMG_FILES := $(strip $(MTK_MD1_IMG_FILES))
MTK_MD1_FILTER_FILES := $(strip $(MTK_MD1_FILTER_FILES))
MTK_MD1_MDDB_FILES := $(strip $(MTK_MD1_MDDB_FILES))
MTK_MD1_META_FILES := $(strip $(MTK_MD1_META_FILES))
MTK_MD1_DBGINFO_DSP_FILES := $(strip $(MTK_MD1_DBGINFO_DSP_FILES))
MTK_MD1_DBGINFO_FILES := $(strip $(MTK_MD1_DBGINFO_FILES))
MTK_MD1_EM_FILTER_FILES := $(strip $(MTK_MD1_EM_FILTER_FILES))
MTK_MD1_M_LAYOUT_FILES := $(strip $(MTK_MD1_M_LAYOUT_FILES))
ifneq ($(words $(MTK_MD1_IMG_FILES)),1)
$(error malformed MD1 ROM file: $(MTK_MD1_IMG_FILES))
endif
ifneq ($(words $(MTK_MD1_DSP_FILES)),1)
$(error malformed MD1 DSP file: $(MTK_MD1_DSP_FILES))
endif
ifneq ($(words $(MTK_MD1_FILTER_FILES)),1)
$(error malformed MD1 FILTER file: $(MTK_MD1_FILTER_FILES))
endif
ifneq ($(words $(MTK_MD1_MDDB_FILES)),1)
$(error malformed MD1 MDDB file: $(MTK_MD1_MDDB_FILES))
endif
ifneq ($(words $(MTK_MD1_META_FILES)),1)
$(error malformed MD1 META file: $(MTK_MD1_META_FILES))
endif
ifneq ($(words $(MTK_MD1_DBGINFO_DSP_FILES)),1)
$(error malformed MD1 DBGINFO_DSP file: $(MTK_MD1_DBGINFO_DSP_FILES))
endif
ifneq ($(words $(MTK_MD1_DBGINFO_FILES)),1)
$(error malformed MD1 DBGINFO file: $(MTK_MD1_DBGINFO_FILES))
endif
ifneq ($(words $(MTK_MD1_EM_FILTER_FILES)),1)
$(error malformed MD1 EM_FILTER file: $(MTK_MD1_EM_FILTER_FILES))
endif
ifneq ($(words $(MTK_MD1_M_LAYOUT_FILES)),1)
$(error malformed MD1 M_LAYOUT file: $(MTK_MD1_M_LAYOUT_FILES))
endif
ifneq ($(words $(MTK_MD1_MDDBMETAODB_FILES)),1)
$(error malformed MD1 MDDBMETAODB file: $(MTK_MD1_MDDBMETAODB_FILES))
endif

INTERNAL_MD1IMG_FILES := \
	$(MTK_MD1_IMG_FILES) \
	$(MTK_MD1_DSP_FILES)

INTERNAL_MD1IMG_FILES += $(intermediates)/$(notdir $(MTK_MD1_FILTER_FILES))
MTK_MD1_FILE_MAP_LINES += md1_filter=$(notdir $(MTK_MD1_FILTER_FILES))
$(eval $(call sign-one-modem-binary,\
	$(intermediates)/$(notdir $(MTK_MD1_FILTER_FILES)),\
	$(MTK_MD1_FILTER_FILES),\
	device/mediatek/build/build/tools/modem/md1_filter_hdr.cfg))

INTERNAL_MD1IMG_FILES += $(intermediates)/$(notdir $(MTK_MD1_MDDB_FILES))
MTK_MD1_FILE_MAP_LINES += md1_mddb=$(notdir $(MTK_MD1_MDDB_FILES))
$(eval $(call sign-one-modem-binary,\
	$(intermediates)/$(notdir $(MTK_MD1_MDDB_FILES)),\
	$(MTK_MD1_MDDB_FILES),\
	device/mediatek/build/build/tools/modem/md1_mddb_hdr.cfg))

INTERNAL_MD1IMG_FILES += $(intermediates)/$(notdir $(MTK_MD1_META_FILES))
MTK_MD1_FILE_MAP_LINES += md1_mddbmeta=$(notdir $(MTK_MD1_META_FILES))
$(eval $(call sign-one-modem-binary,\
	$(intermediates)/$(notdir $(MTK_MD1_META_FILES)),\
	$(MTK_MD1_META_FILES),\
	device/mediatek/build/build/tools/modem/md1_mddbmeta_hdr.cfg))

$(intermediates)/$(notdir $(MTK_MD1_DBGINFO_DSP_FILES)).xz: $(MTK_MD1_DBGINFO_DSP_FILES) ./device/mediatek/build/build/tools/modem/LzmaUtil
	./device/mediatek/build/build/tools/modem/LzmaUtil e $< $@

INTERNAL_MD1IMG_FILES += $(intermediates)/$(notdir $(MTK_MD1_DBGINFO_DSP_FILES))
MTK_MD1_FILE_MAP_LINES += md1_dbginfodsp=$(notdir $(MTK_MD1_DBGINFO_DSP_FILES))
$(eval $(call sign-one-modem-binary,\
	$(intermediates)/$(notdir $(MTK_MD1_DBGINFO_DSP_FILES)),\
	$(intermediates)/$(notdir $(MTK_MD1_DBGINFO_DSP_FILES)).xz,\
	device/mediatek/build/build/tools/modem/md1_dbginfodsp_hdr.cfg))

$(intermediates)/$(notdir $(MTK_MD1_DBGINFO_FILES)).xz: $(MTK_MD1_DBGINFO_FILES) ./device/mediatek/build/build/tools/modem/LzmaUtil
	./device/mediatek/build/build/tools/modem/LzmaUtil e $< $@

INTERNAL_MD1IMG_FILES += $(intermediates)/$(notdir $(MTK_MD1_DBGINFO_FILES))
MTK_MD1_FILE_MAP_LINES += md1_dbginfo=$(notdir $(MTK_MD1_DBGINFO_FILES))
$(eval $(call sign-one-modem-binary,\
	$(intermediates)/$(notdir $(MTK_MD1_DBGINFO_FILES)),\
	$(intermediates)/$(notdir $(MTK_MD1_DBGINFO_FILES)).xz,\
	device/mediatek/build/build/tools/modem/md1_dbginfo_hdr.cfg))

INTERNAL_MD1IMG_FILES += $(intermediates)/$(notdir $(MTK_MD1_EM_FILTER_FILES))
MTK_MD1_FILE_MAP_LINES += md1_emfilter=$(notdir $(MTK_MD1_EM_FILTER_FILES))
$(eval $(call sign-one-modem-binary,\
	$(intermediates)/$(notdir $(MTK_MD1_EM_FILTER_FILES)),\
	$(MTK_MD1_EM_FILTER_FILES),\
	device/mediatek/build/build/tools/modem/md1_emfilter_hdr.cfg))

INTERNAL_MD1IMG_FILES += $(intermediates)/$(notdir $(MTK_MD1_M_LAYOUT_FILES))
MTK_MD1_FILE_MAP_LINES += md1_mdmlayout=$(notdir $(MTK_MD1_M_LAYOUT_FILES))
$(eval $(call sign-one-modem-binary,\
	$(intermediates)/$(notdir $(MTK_MD1_M_LAYOUT_FILES)),\
	$(MTK_MD1_M_LAYOUT_FILES),\
	device/mediatek/build/build/tools/modem/md1_mdmlayout_hdr.cfg))

INTERNAL_MD1IMG_FILES += $(intermediates)/$(notdir $(MTK_MD1_MDDBMETAODB_FILES))
MTK_MD1_FILE_MAP_LINES += md1_mddbmetaodb=$(notdir $(MTK_MD1_MDDBMETAODB_FILES))
$(eval $(call sign-one-modem-binary,\
	$(intermediates)/$(notdir $(MTK_MD1_MDDBMETAODB_FILES)),\
	$(MTK_MD1_MDDBMETAODB_FILES),\
	device/mediatek/build/build/tools/modem/md1_mddbmetaodb_hdr.cfg))

ifneq ($(MTK_MD1_CUSTOM_FILTER_RULES),)
$(foreach r,$(MTK_MD1_CUSTOM_FILTER_RULES),\
	$(eval f := $(call word-colon,1,$(r)))\
	$(eval k := $(call word-colon,2,$(r)))\
	$(eval INTERNAL_MD1IMG_FILES += $(intermediates)/$(notdir $(f)))\
	$(eval MTK_MD1_FILE_MAP_LINES += $(k)=$(notdir $(f)))\
	$(eval $(call sign-one-modem-cfg,\
		$(intermediates)/$(notdir $(f)).cfg,$(k)))\
	$(eval $(call sign-one-modem-binary,\
		$(intermediates)/$(notdir $(f)),$(f),$(intermediates)/$(notdir $(f)).cfg))\
)
endif


INTERNAL_MD1IMG_FILES += $(intermediates)/md1_file_map.img
$(eval $(call sign-one-modem-binary,\
	$(intermediates)/md1_file_map.img,\
	$(intermediates)/md1_file_map.log,\
	device/mediatek/build/build/tools/modem/md1_file_map_hdr.cfg))

$(intermediates)/md1_file_map.log: PRIVATE_LINES := $(MTK_MD1_FILE_MAP_LINES)
$(intermediates)/md1_file_map.log: $(filter-out $(intermediates)/md1_file_map.img,$(INTERNAL_MD1IMG_FILES))
	@mkdir -p $(dir $@)
	@rm -f $@
	@$(foreach l,$(PRIVATE_LINES), echo "$(l)" >>$@;)

$(INSTALLED_MD1IMG_TARGET): PRIVATE_FILES := $(INTERNAL_MD1IMG_FILES)
$(INSTALLED_MD1IMG_TARGET): $(INTERNAL_MD1IMG_FILES)
$(INSTALLED_MD1IMG_TARGET): ./device/mediatek/build/build/tools/modem/pack_img
	@mkdir -p $(dir $@)
	./device/mediatek/build/build/tools/modem/pack_img pack $@ $(PRIVATE_FILES)

endif#MTK_MD1_SINGLEBIN_FILES
endif#MTK_MD1_IMG_FILES
endif#MTK_SINGLE_BIN_MODEM_SUPPORT
endif#CUSTOM_MODEM

ifneq ($(strip $(MTK_MODEM_MCF_OTA_FILES)),)
ifeq ($(notdir $(strip $(MTK_MODEM_MCF_OTA_FILES))),MCF_OTA_FILES.tar.gz)
MCF_OTA_LIST := $(filter %.mcfota %.mcfopota %.ini,$(shell PATH=prebuilts/build-tools/path/linux-x86 tar -zmtf $(MTK_MODEM_MCF_OTA_FILES)))
$(foreach f,$(MCF_OTA_LIST),\
    $(eval $(call mtk-install-mdota,$(f),$(MTK_MODEM_MCF_OTA_FILES)))\
)

include $(CLEAR_VARS)
LOCAL_MODULE := mcf_ota
LOCAL_MODULE_OWNER := mtk
LOCAL_REQUIRED_MODULES := $(notdir $(MCF_OTA_LIST))
include $(BUILD_PHONY_PACKAGE)
else
$(foreach ota_file,$(MTK_MODEM_MCF_OTA_FILES), \
  $(eval $(info ota_file=$(ota_file))) \
  $(eval MCF_OTA_LIST := $(filter %.mcfota %.mcfopota %.ini,$(shell PATH=prebuilts/build-tools/path/linux-x86 tar -zmtf $(ota_file))))\
  $(eval $(call mtk-install-mdota-img,$(ota_file)))\
  )
endif#MCF_OTA_FILES
else
MCF_OTA_LIST :=
endif#MTK_MODEM_MCF_OTA_FILES

LOCAL_PATH := device/mediatek/build/build/tools/modem
MTK_MODEM_PARTITION_FILES := $(foreach item,md1img.img $(if $(filter yes,$(MTK_SINGLE_BIN_MODEM_SUPPORT)),,md1dsp.img) md1arm7.img md3img.img,$(if $(filter %/$(item),$(MTK_MODEM_DATABASE_FILES)),,$(if $(wildcard $(LOCAL_PATH)/$(item)),$(item))))
$(info Use default MTK_MODEM_PARTITION_FILES for $(strip $(MTK_MODEM_PARTITION_FILES)))
$(foreach item,$(MTK_MODEM_PARTITION_FILES),$(eval $(call mtk-install-modem,$(item),$(PRODUCT_OUT))))
ALL_DEFAULT_INSTALLED_MODULES += $(MTK_MODEM_INSTALLED_MODULES)

ifneq ($(MTK_BUILD_IGNORE_IMS_REPO),yes)
$(foreach m,$(PRODUCT_PACKAGES),\
    $(if $(filter $(MTK_PATH_MODEM)/%,$(ALL_MODULES.$(m).PATH)),\
        $(eval MTK_MODEM_APPS_FILES += $(ALL_MODULES.$(m).INSTALLED))\
    )\
    $(if $(filter $(MTK_PATH_MODEM)/%,$(ALL_MODULES.$(m)$(TARGET_2ND_ARCH_MODULE_SUFFIX).PATH)),\
        $(eval MTK_MODEM_APPS_FILES += $(ALL_MODULES.$(m)$(TARGET_2ND_ARCH_MODULE_SUFFIX).INSTALLED))\
    )\
)
$(foreach cf,$(PRODUCT_COPY_FILES),\
    $(eval _src := $(call word-colon,1,$(cf))) \
    $(if $(filter $(MTK_PATH_MODEM)/%,$(_src)),\
        $(eval _dest := $(call word-colon,2,$(cf))) \
        $(eval _fulldest := $(call append-path,$(PRODUCT_OUT),$(_dest))) \
        $(eval MTK_MODEM_APPS_FILES += $(_fulldest))\
    )\
)
endif

.PHONY: update-modem clean-modem
update-modem: $(MTK_MODEM_INSTALLED_MODULES) $(MTK_MODEM_APPS_FILES)

clean-modem:

