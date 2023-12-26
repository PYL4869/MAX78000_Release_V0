# This file can be used to set build configuration
# variables.  These variables are defined in a file called 
# "Makefile" that is located next to this one.

# For instructions on how to use this system, see
# https://analog-devices-msdk.github.io/msdk/USERGUIDE/#build-system

# **********************************************************

# Add your config here!
CAMERA=OV7692

#使能SD库
LIB_SDHC=1
#使能FreeRTOS库
LIB_FREERTOS = 1

#设置优化等级
MXC_OPTIMIZE_CFLAGS = -O2

#设置Bsp路径
PROJ_CFLAGS+=-DBSP_ENABLE
IPATH += User/Bsp/inc
VPATH += User/Bsp/src

#设置App路径
PROJ_CFLAGS+=-DAPP_ENABLE
IPATH += User/App/inc
VPATH += User/App/src

#设置Cnn路径
PROJ_CFLAGS+=-DCNN_ENABLE
IPATH += User/Cnn/inc
VPATH += User/Cnn/src
