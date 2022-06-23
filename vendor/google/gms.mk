ifdef BUILD_GMS
$(warning $BUILD_GMS=$(BUILD_GMS),AGN_CUSTOM_GMS_MK=$(AGN_CUSTOM_GMS_MK),BUILD_AGO_GMS=$(BUILD_AGO_GMS),EEA_TYPE=$(EEA_TYPE))
ifeq ($(strip $(BUILD_GMS)), yes)
#add by AGN lihan on 20210319 begin
ifneq ($(strip $(AGN_CUSTOM_GMS_MK)),)
$(call inherit-product-if-exists, vendor/google/products/$(AGN_CUSTOM_GMS_MK).mk)
else 
#add by AGN lihan on 20210319 end
ifeq ($(strip $(MTK_GMO_RAM_OPTIMIZE)), yes)
ifeq ($(strip $(EEA_TYPE)), type1)
$(call inherit-product-if-exists, vendor/google/products/gms_go_eea_type1.mk)
else ifeq ($(strip $(EEA_TYPE)), type2)
$(call inherit-product-if-exists, vendor/google/products/gms_go_eea_type2.mk)
else ifeq ($(strip $(EEA_TYPE)), type3a)
$(call inherit-product-if-exists, vendor/google/products/gms_go_eea_type3a.mk)
else ifeq ($(strip $(EEA_TYPE)), type3b)
$(call inherit-product-if-exists, vendor/google/products/gms_go_eea_type3b.mk)
else ifeq ($(strip $(EEA_TYPE)), type4a)
$(call inherit-product-if-exists, vendor/google/products/gms_go_eea_type4a.mk)
else ifeq ($(strip $(EEA_TYPE)), type4b)
$(call inherit-product-if-exists, vendor/google/products/gms_go_eea_type4b.mk)
else ifeq ($(strip $(EEA_TYPE)), type4c)
$(call inherit-product-if-exists, vendor/google/products/gms_go_eea_type4c.mk)
else ifeq ($(strip $(EEA_TYPE)), eea_v2)
$(call inherit-product-if-exists, vendor/google/products/gms_go_eea_v2_type4c.mk)
else
ifeq ($(strip $(MSSI_CUSTOM_CONFIG_MAX_DRAM_SIZE)), 0x80000000)
$(call inherit-product-if-exists, vendor/google/products/gms_go_2gb.mk)
else ifeq ($(strip $(MSSI_CUSTOM_CONFIG_MAX_DRAM_SIZE))$(strip $(CUSTOM_CONFIG_MAX_DRAM_SIZE)), 0x80000000)
$(call inherit-product-if-exists, vendor/google/products/gms_go_2gb.mk)
else
$(call inherit-product-if-exists, vendor/google/products/gms_go.mk)
endif
endif
else
ifeq ($(strip $(EEA_TYPE)), type1)
$(call inherit-product-if-exists, vendor/google/products/gms_eea_type1.mk)
else ifeq ($(strip $(EEA_TYPE)), type2)
$(call inherit-product-if-exists, vendor/google/products/gms_eea_type2.mk)
else ifeq ($(strip $(EEA_TYPE)), type3a)
$(call inherit-product-if-exists, vendor/google/products/gms_eea_type3a.mk)
else ifeq ($(strip $(EEA_TYPE)), type3b)
$(call inherit-product-if-exists, vendor/google/products/gms_eea_type3b.mk)
else ifeq ($(strip $(EEA_TYPE)), type4a)
$(call inherit-product-if-exists, vendor/google/products/gms_eea_type4a.mk)
else ifeq ($(strip $(EEA_TYPE)), type4b)
$(call inherit-product-if-exists, vendor/google/products/gms_eea_type4b.mk)
else ifeq ($(strip $(EEA_TYPE)), type4c)
$(call inherit-product-if-exists, vendor/google/products/gms_eea_type4c.mk)
else ifeq ($(strip $(EEA_TYPE)), eea_v2)
$(call inherit-product-if-exists, vendor/google/products/gms_eea_v2_type4c.mk)
else
$(call inherit-product-if-exists, vendor/google/products/gms.mk)
endif
endif
endif
endif
endif