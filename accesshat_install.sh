#!/bin/bash

#This script will build all the accesshat driver code and creates the shared c library 
#Shared library and all header files are copied to appropriate directories

BRED='\033[1;31m'          # Bold Red
BGREEN='\033[1;32m'        # BoldGreen
BYELLOW='\033[1;33m'       # Bold Yello
Color_Off='\033[0m'        # Reset


#command to create object files
OBJ_CMD="gcc -c -Wall -Werror -fpic"

#Command to create library files
LIB_CMD="gcc -shared -o"

#System header file include path
INCLUDE_PATH="/usr/local/include"

#Sys lib path
LIB_PATH="/usr/local/lib"

#Create object files for all driver codes
echo -e "${BYELLOW}\nCreating Object files ....${Color_Off} \n"
${OBJ_CMD} ./gpio_driver/accesshat_gpio.c
${OBJ_CMD} ./relay_driver/accesshat_relay.c
${OBJ_CMD} ./inertial_module_driver/accesshat_inertial_module.c
${OBJ_CMD} ./eeprom_driver/accesshat_eeprom.c 
${OBJ_CMD} ./rtc_driver/accesshat_rtc.c
${OBJ_CMD} ./wiegand_driver/accesshat_wiegand.c

#Create C shared library
echo -e "${BYELLOW}\nCreating Accesshat Library ....${Color_Off} \n"
${LIB_CMD} libaccesshat.so *.o

#Installing Accesshat library
cp ./gpio_driver/*.h ${INCLUDE_PATH}
cp ./relay_driver/*.h ${INCLUDE_PATH}
cp ./inertial_module_driver/*.h ${INCLUDE_PATH}
cp ./eeprom_driver/*.h ${INCLUDE_PATH}
cp ./rtc_driver/*.h ${INCLUDE_PATH}
cp ./wiegand_driver/*.h ${INCLUDE_PATH}

cp ./libaccesshat.so ${LIB_PATH}

#Remove all object files
rm -r *.o

ldconfig

echo -e "${BGREEN}\nAccesshat Library Successfully Installed.....!!!!${Color_Off} \n"
