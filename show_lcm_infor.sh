#!/bin/bash

if [ -z "$1" ]   #����Ƿ����˲���
then
    echo "usage $0 <project_name>"
    exit 1
fi
project_name=$1

#��һ���������ں˰汾
kernel_version=$(grep -r "LINUX_KERNEL_VERSION" device/agenew/*${project_name}*/ProjectConfig.mk | awk -F '=' '{print $2}' | tr -d ' ')

# ����Ƿ��ҵ��ں˰汾
if [ -z "$kernel_version" ]
then
    echo "Cannot find LINUX_KERNEL_VERSION for project ${project_name}"
    exit 1
fi

#�ڶ��������Ҷ�Ӧ��defconfig�ļ���������CONFIG_CUSTOM_KERNEL_LCM ����
defconfig_path="${kernel_version}/arch/arm/configs/*${project_name}*_defconfig"     #�ļ�·��
lcm_config_line=$(grep "CONFIG_CUSTOM_KERNEL_LCM" ${defconfig_path})        #���Ҳ���ȡlcm��ֵ
lcm_config=$(echo $lcm_config_line | awk -F '"' '{print $2}')       #��ȡ������
if [ -z "lcm_config" ]
then
    echo "Cannot find LCM configuration for project ${project_name}"
    exit 1
fi

#����������һ���������������Ϣ  
#ʹ������洢��������
ifs=' ' read -a addr <<< "$lcm_config"

for i in ${addr[@]}
do
    ic=${i%%_*} #ȥ�����_* �õ�icֵ
    module_addr=${i#*_}; module=${module_addr%%_*}     #��ȥ��i����̵�*_  ��ȥ����ʣ���ֶ������_*
    lane_addr=${module_addr#*_}; lane=${lane_addr%%_*}   #ȥ����module_addr�ֶ�����̵�*_  ��ȥ�������_*
    resolution=${i##*_} #ȥ�������*_

    #��ӡ���
    echo "LCM_NAME is: $i"
    echo "IC is : $ic"
    echo "Module : $module"
    echo "lane is: $lane"
    echo "resolution: $resolution"
    echo ""
done