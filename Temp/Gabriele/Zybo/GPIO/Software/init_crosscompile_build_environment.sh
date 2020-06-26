#!/bin/bash
#
########################################################################################
# Configura il build envirnoment per cross compilare usando il compilatore di buildroot
########################################################################################
#
# USO
# Cambiare BUILDROOT_PATH con il path di buildroot
# dare source init_build_environment.sh

#BUILDROOT_PATH="/home/giorgio/Documents/SO_embedded/Zinco-Linux/buildroot-2019.02.5/output/host/bin"
BUILDROOT_PATH="/home/jiin995/Linux-on-Zynq/buildroot-2020.02.3/output/host/bin"


export CROSS_COMPILE=arm-buildroot-linux-uclibcgnueabi-
export PATH=$PATH:${BUILDROOT_PATH}
export CXX=${CROSS_COMPILE}g++
export CC=${CROSS_COMPILE}gcc
export LD=${CROSS_COMPILE}ld
export AR=${CROSS_COMPILE}ar
export OBJCOPY=${CROSS_COMPILE}objcopy
export SIZE=${CROSS_COMPILE}size
