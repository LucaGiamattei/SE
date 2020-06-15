#!/bin/bash
export CROSS_COMPILE=arm-buildroot-linux-uclibcgnueabihf-
export PATH=$PATH:/home/giorgio/Documents/SO_embedded/Zinco-Linux/buildroot-2019.02.5/output/host/bin
export CXX=${CROSS_COMPILE}g++
export CC=${CROSS_COMPILE}gcc
export LD=${CROSS_COMPILE}ld
export AR=${CROSS_COMPILE}ar
export OBJCOPY=${CROSS_COMPILE}objcopy
export SIZE=${CROSS_COMPILE}size
