#!/bin/bash

if [ -z "$1" ]   #����Ƿ����˲���
then
    echo "usage $0 <cream_ic_name>"
    exit 1
fi

#�������ں��������ļ������ļ��еĲ��ֹؼ���
MAIN_CAM=$1    #����оƬ��
SUB_CAM="${MAIN_CAM}s"  #ǰ��оƬ��
MAIN_CAM_UPPER=$(echo "$MAIN_CAM" | tr 'a-z' 'A-Z')     #�����д
SUB_CAM_UPPER=$(echo "$MAIN_CAM" | tr 'a-z' 'A-Z')S      #ǰ���д

#����Ŀ��·��
KERNEL_DIR="kernel-4.19/drivers/misc/mediatek/imgsensor/src/common/v1"  #�ں���������·��
VENDOR_HAL_DIR="vendor/mediatek/proprietary/custom/mt6739/hal/imgsensor"    #hal��ʵ�ִ���·��
VENDOR_META_DIR="vendor/mediatek/proprietary/custom/mt6739/hal/imgsensor_metadata"  #metadata ʵ�ִ���·��

#������ :���ɲ��Ҵ���һ��Ŀ¼�µ���Ŀ¼���ļ�
generate_code(){
    local base_dir="$1"  
    local src_folder="$base_dir/${MAIN_CAM}_mipi_raw"   #Դ����ͷ����Ŀ¼
    local dst_folder="$base_dir/${SUB_CAM}_mipi_raw"    #Ŀ��Ŀ¼

    #���Դ�ļ��������
    if [ ! -d "$src_folder" ]
    then   
        echo "source folder ${src_folder} does not exist"
        exit 1  
    fi

    #����Ƿ����Ŀ���ļ�
    if [ -d "$dst_folder" ]
    then
        rm -r "$dst_folder"
    fi 

    #�����ļ�
    cp -r "$src_folder" "$dst_folder"  
    cd "$dst_folder" 

    for file in *
    do
        if [ -f "$file" ]
        then
            new_file=`echo "$file" | sed "s/${MAIN_CAM}/${SUB_CAM}/g"`  # �滻���ֵ��ļ���
            mv "$file" "$new_file"      #�������ļ�

            # �滻�ļ������еĹؼ���
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

echo "�Զ�����ǰ�������� : ${SUB_CAM}"
cd ..