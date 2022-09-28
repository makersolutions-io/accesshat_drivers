#!/bin/bash

#This script will uninstall accesshat library

BRED='\033[1;31m'          # Bold Red
BGREEN='\033[1;32m'        # BoldGreen
BYELLOW='\033[1;33m'       # Bold Yello
Color_Off='\033[0m'        # Reset

#System header file include path
INCLUDE_PATH="/usr/local/include"

#Sys lib path
LIB_PATH="/usr/local/lib"

echo -e "${BYELLOW}Uninstalling AccessHAT Library ....${Color_Off} \n"

sudo rm -r ${LIB_PATH}/libaccesshat.so

sudo rm -r ${INCLUDE_PATH}/accesshat*

sudo rm -r ${INCLUDE_PATH}/i2c-dev.h

ldconfig

echo -e "${BGREEN}Uninstall Successfull........!!!!${Color_Off} \n"
