#!/bin/bash

if [ -z "$1" ]   #检测是否传入了参数
then
    echo "usage $0 <project_name>"
    exit 1
fi
project_name=$1

#第一步：查找内核版本
kernel_version=$(grep -r "LINUX_KERNEL_VERSION" device/agenew/*${project_name}*/ProjectConfig.mk | awk -F '=' '{print $2}' | tr -d ' ')

# 检查是否找到内核版本
if [ -z "$kernel_version" ]
then
    echo "Cannot find LINUX_KERNEL_VERSION for project ${project_name}"
    exit 1
fi

#第二步：查找对应的defconfig文件，并解析CONFIG_CUSTOM_KERNEL_LCM 配置
defconfig_path="${kernel_version}/arch/arm/configs/*${project_name}*_defconfig"     #文件路径
lcm_config_line=$(grep "CONFIG_CUSTOM_KERNEL_LCM" ${defconfig_path})        #查找并读取lcm的值
lcm_config=$(echo $lcm_config_line | awk -F '"' '{print $2}')       #提取配置项
if [ -z "lcm_config" ]
then
    echo "Cannot find LCM configuration for project ${project_name}"
    exit 1
fi

#第三步：进一步解析处理，输出信息  
#使用数组存储变量内容
ifs=' ' read -a addr <<< "$lcm_config"

for i in ${addr[@]}
do
    ic=${i%%_*} #去除最长的_* 得到ic值
    module_addr=${i#*_}; module=${module_addr%%_*}     #先去除i中最短的*_  再去除掉剩下字段中最长的_*
    lane_addr=${module_addr#*_}; lane=${lane_addr%%_*}   #去除掉module_addr字段中最短的*_  再去除掉最长的_*
    resolution=${i##*_} #去除掉最长的*_

    #打印结果
    echo "LCM_NAME is: $i"
    echo "IC is : $ic"
    echo "Module : $module"
    echo "lane is: $lane"
    echo "resolution: $resolution"
    echo ""
done