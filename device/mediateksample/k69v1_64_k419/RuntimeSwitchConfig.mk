#For customer to configure RSC, internal keep this only default
#when remove all RSC, please also remove this line
PRODUCT_CUSTOMER_RSC_NAMES := default

#For internal RSC configuration
PRODUCT_MTK_RSC_NAMES := default

#include device/mediatek/common/rsc/rsc_cnop_rules.mk
PRODUCT_MTK_RSC_NAMES += op01
PRODUCT_MTK_RSC_NAMES += op01_lwtg
PRODUCT_MTK_RSC_NAMES += op02_lwctg
PRODUCT_MTK_RSC_NAMES += op02_lwg
PRODUCT_MTK_RSC_NAMES += op02_lwtg
PRODUCT_MTK_RSC_NAMES += op09
PRODUCT_MTK_RSC_NAMES += cmcc_om
PRODUCT_MTK_RSC_NAMES += MtkRadioOnoff
PRODUCT_MTK_RSC_NAMES += MtkSimOnoff
PRODUCT_MTK_RSC_NAMES += vsimdsds

