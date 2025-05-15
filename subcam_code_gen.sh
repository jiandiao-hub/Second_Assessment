#!/bin/bash

if [ -z "$1" ]   #检测是否传入了参数
then
    echo "usage $0 <cream_ic_name>"
    exit 1
fi

#设置用于后续更换文件名和文件中的部分关键字
MAIN_CAM=$1    #主摄芯片名
SUB_CAM="${MAIN_CAM}s"  #前摄芯片名
MAIN_CAM_UPPER=$(echo "$MAIN_CAM" | tr 'a-z' 'A-Z')     #主摄大写
SUB_CAM_UPPER=$(echo "$MAIN_CAM" | tr 'a-z' 'A-Z')S      #前摄大写

#定义目标路径
KERNEL_DIR="kernel-4.19/drivers/misc/mediatek/imgsensor/src/common/v1"  #内核驱动代码路径
VENDOR_HAL_DIR="vendor/mediatek/proprietary/custom/mt6739/hal/imgsensor"    #hal层实现代码路径
VENDOR_META_DIR="vendor/mediatek/proprietary/custom/mt6739/hal/imgsensor_metadata"  #metadata 实现代码路径

#处理函数 :生成并且处理一个目录下的子目录和文件
generate_code(){
    local base_dir="$1"  
    local src_folder="$base_dir/${MAIN_CAM}_mipi_raw"   #源摄像头代码目录
    local dst_folder="$base_dir/${SUB_CAM}_mipi_raw"    #目标目录

    #检查源文件存在与否
    if [ ! -d "$src_folder" ]
    then   
        echo "source folder ${src_folder} does not exist"
        exit 1  
    fi

    #检查是否存在目标文件
    if [ -d "$dst_folder" ]
    then
        rm -r "$dst_folder"
    fi 

    #复制文件
    cp -r "$src_folder" "$dst_folder"  
    cd "$dst_folder" 

    for file in *
    do
        if [ -f "$file" ]
        then
            new_file=`echo "$file" | sed "s/${MAIN_CAM}/${SUB_CAM}/g"`  # 替换出现的文件名
            mv "$file" "$new_file"      #重命名文件

            # 替换文件内容中的关键字
            sed -i "s/${MAIN_CAM_UPPER}/${SUB_CAM_UPPER}/g" "$new_file"
            sed -i "s/${MAIN_CAM}/${SUB_CAM}/g" "$new_file" 
        fi
        
        if [ -f "$Makefile" ] 
        then
            sed -i "s/${MAIN_CAM}/${SUB_CAM}/g" "$Makefile"
        fi
    done   
}

generate_code "$KERNEL_DIR"
generate_code "$VENDOR_HAL_DIR"
generate_code "$VENDOR_META_DIR"  

echo "自动生成前摄代码完成 : ${SUB_CAM}"
cd ..