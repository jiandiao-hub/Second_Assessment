#For customer to configure RSC, internal keep this only default
#when remove all RSC, please also remove this line
PRODUCT_CUSTOMER_RSC_NAMES := default

#For internal RSC configuration
PRODUCT_MTK_RSC_NAMES := default

#include device/mediatek/common/rsc/rsc_cnop_rules.mk
PRODUCT_MTK_RSC_NAMES += MtkRadioOnoff
PRODUCT_MTK_RSC_NAMES += MtkSimOnoff