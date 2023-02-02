# Observables AccessHAT: Software Driver #

This repository contain source code of AccessHAT Software Driver.  All of the driver source code uses **[WiringPi Library](http://wiringpi.com/)** I2C APIs, except EEPROM driver where it re-uses the source code of **[Rasberry Pi I2C EEPROM Program](https://www.richud.com/wiki/Rasberry_Pi_I2C_EEPROM_Program)**. 


## Prerequisite  
### To Purchase Hardware

Visit **[makersolutions.io](https://makersolutions.io)**

### For Raspbian OS

1. Enable I2C and UART(Serial Port) Interface in Raspberry Pi  
   **$ sudo raspi-config**  
   
   For UART to work correctly, the console access over the UART has to be disabled, hence when enabling the UART using raspi-config utility select the answer as given below.
   * At the prompt Would you like a login shell to be accessible overserial?  **answer 'No'**.  
   * At the prompt Would you like the serial port hardware to be enabled?  **answer 'Yes'**.    
   
   
2. Install/Update the WiringPi Library  
   **$  sudo apt-get install wiringpi**  
   
3. The following line must be added in **/boot/firmware/usercfg.txt** file in order to enable i2c communication with EEPROM   
    **dtoverlay=i2c-gpio,i2c_gpio_sda=0,i2c_gpio_scl=1,bus=9**   
    
4. Check if the below commad will give all pin details  
    **$ gpio readall**  
    
    If the above command results in any error, then **[update wiringPi](http://wiringpi.com/wiringpi-updated-to-2-52-for-the-raspberry-pi-4b/)** library as given below  
    **$ cd /tmp**  
    **$ wget https://project-downloads.drogon.net/wiringpi-latest.deb**  
    **$ sudo dpkg -i wiringpi-latest.deb**  
    
5. Restart the Raspberry Pi

### For Ubuntu OS

1. The following line must be added in **/boot/firmware/usercfg.txt** file in order to enable i2c communication with EEPROM   
    **dtoverlay=i2c-gpio,i2c_gpio_sda=0,i2c_gpio_scl=1,bus=9**  
2. Add **enable_uart=1** in the file **/boot/firmware/usercfg.txt** to enable UART. 
3. For UART to work correctly, the console access over the UART has to be disabled. Follow the steps given below to disable console access over UART. 
    * Edit the file **/boot/firmware/nobtcmd.txt**  
    * Remove **"console=ttyAMA0,115200"** from the existing line ***net.ifnames=0 dwc_otg.lpm_enable=0 console=ttyAMA0,115200 console=tty1 root=LABEL=writable rootfstype=ext4 elevator=deadline rootwait fixrtc***  
    * Save the file **/boot/firmware/nobtcmd.txt**
      
4. Install WiringPi Library.
    * You'll need git (may be installed by default). If git is not installed, enter the following into the command line.  
      **$ sudo apt-get install git-core**  
    * It is recommended to use the latest version of wiringPi, so enter the following to remove the wiringPi and configuration files.(If its already installed)  
      **$ sudo apt-get purge wiringpi**  
    * Then type the following for the Pi to remove all locations that remember wiringPi.  
      **$ hash -r**  
    * Download the Wiring Pi Library  
      **$ git clone https://github.com/WiringPi/WiringPi.git**        
    * This will make a folder in your current directory called WiringPi. Head to the Wiring Pi directory.  
      **$ cd WiringPi**     
    * Then pull the latest changes from the origin.  
      **$ git pull origin**    
    * Then enter the following command. The ./build is a script to build Wiring Pi from the source files. This builds the helper files, modifies some paths in Linux.  
     **$ ./build**
    * At this point, the library should work. Run the gpio command shown below to view some information about the wiringPi version and the Pi that it is running on.  
    **$ gpio -v**  
    * Entering the following command will draw a table illustrating the configuration for the pins in the 40-pin connector.  
      **$ gpio readall**
5. Restart the Raspberry Pi. 
   
   

## Installation 
The accesshat_drivers folder has an **accesshat_install.sh** script, which will compile and create the c library and also copies the library and header file to the appropriate location so that it is available globally in the entire system.

Follow these steps to compile and install the accesshat c library :  

1. copy **hardware_drivers** folder to raspberry pi.
2. navigate to accesshat_drivers directory  
   **$ cd hardware_drivers/accesshat_drivers/**  
3. Give execute permission to install script  
   **$ sudo chmod +x accesshat_install.sh**
4. Execute the script  
   **$ sudo ./accesshat_install.sh**
   
After completing these steps, we can write an application code in any directory of user choice and include the header files directly without worrying about its location like  #include <driver_file.h> (all the driver header files will be available globally).

**Example**:
 The [Single tap detection](https://github.com/makersolutions-io/accesshat_drivers/blob/main/inertial_module_driver/single_tap_example.c) example code can be compiled and executed as follows.
 
 **$ gcc single_tap_example.c -laccesshat -lwiringPi**  
 **$ sudo ./a.out**
 
 ## Uninstall Library
 To remove accesshat library and relevant files, execute the **accesshat_uninstall.sh** script.  
 **$ sudo chmod +x accesshat_uninstall.sh**  
 **$ sudo ./accesshat_uninstall.sh**
