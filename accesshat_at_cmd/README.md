This document is a reference guide to “AT” commands available for the Observables AccessHAT module. These commands utilize the Observables AccessHAT: Software Driver.


### To execute an AT command, do the following 
1. Install **[Accesshat Software Driver](https://github.com/SecurePRO/hardware_drivers/blob/master/accesshat_drivers/README.md)**  
2. Navigate to AT command application  
   **$ cd accesshat_drivers/accesshat_at_cmd** 
3. Compile the AT command program  
   **$ gcc accesshat_at_cmd.c -laccesshat -lwiringPi**    
4. Execute the program by given AT command as input argument  
   **$ sudo ./a.out \<AT commands\>**
   
   
### AT Command List

1. **AT**  
This command simply replies with an “OK” to confirm AT commands have been properly set up.  

2. **ATI3**  
This command retrieves the manufacturer name. This will reply with “Observables” then an “OK”.  

3. **ATI4**  
Retrieves the product name. Will reply with “ACCESSHAT” then an “OK”.  

4. **ATI5**  
Retrieves the product version. Replies with “1.0.0” and then an “OK”.  

5. **AT#CONFIG=gpios|gpio1,gpio2,gpio3,gpio4,gpio5,gpio6,gpio7,gpio8,gpio9**  
    Sets up pin configuration.  
    This command take a parameter for each EX_GPIOx.  

    **EX**: “AT#CONFIG=gpios|0,1,1,-1,1,1,2,2,0”  

    Here, EX_GPIO20 = Input, EX_GPIO17& EX_GPIO16 = Output (Set Low initially), EX_GPIO15 = Don’t Care,EX_GPIO14&EX_GPIO13 =Output (Set Low initially),  EX_GPIO12&EX_GPIO11 =  Output (Set High initially) & EX_GPIO10 = Input.  
    
      **gpio**x | **EX_GPIO**x 
      --- | --- 
      gpio0 | EX_GPIO10  
      gpio1 | EX_GPIO11
      gpio2 | EX_GPIO12   
      gpio3 | EX_GPIO13 
      gpio4 | EX_GPIO14   
      gpio5 | EX_GPIO15
      gpio6 | EX_GPIO16   
      gpio7 | EX_GPIO17   
      gpio8 | EX_GPIO20  
      
     **Gpio**X parameters will be an integer value   
			     0 = Input  
			     1 = Output (Set Low initially)  
			     2 = Output (Set High initially)  
		            -1 = Don’t Care (GPIO state remains unchanged) 

     **NOTE**: Must have Gpio parameters even if no Gpios will be utilized in the program. Will simply all be -1. 
 
6. **AT#GPIO?index**  
   This command performs a digital read of a EX_GPIOx value. This returns the state of the GPIO regardless if the GPIO was set as an Input or Output.Will return 1 for HIGH and 0 for LOW followed by OK.  
       Index refers to which pin to read, **0** for the EX_GPIO20 and **1 to 8** for EX_GPIO17 to EX_GPIO10.  
       **EX** : AT#GPIO?5
       
7. **AT#GPIO=type, index, mode, time_interval**  
   Does a digital write to specified pin.  
   
   **Type** refers to using a GPIO or a Relay, **1** is for GPIO and **2** is for Relay  
   **Index** refers to which pin/relay to use, **0-8** for the EX_GPIOs and **0-1** for Relays  
   
   **Mode** refers the action you want the pins / relays to take  
   * **0 = TOGGLE** - changes GPIO from HIGH to LOW or vice versa depending on current state, Or either closes or opens the Relay depending on current state  
   * **1 =  set GPIO LOW or CLOSE Relay** - pretty self explanatory, this mode sets a GPIO state to LOW or closes a Relay  
   * **2 = set GPIO HIGH or OPEN Relay** - same as mode 1, sets a GPIO state to HIGH or opens a Relay  
   * **3 = MOMENTARY ON** - Closes the Relay for specified time_interval in milliseconds, then opens after that time limit reached. In case of GPIO, pin state will  be made HIGH for the specified time interval, after which it is made LOW.    
   * **4 = MOMENTARY OFF** - Opens the Relay for specified time_interval in milliseconds, then closes after that time limit reached. In case of GPIO, pin state will be made LOW for the specified time interval, after which it is made HIGH. 
   
   **Time_interval** refers to the time limit for MOMENTARY ON or OFF in milliseconds. This value is don’t care unless mode == 3 or 4  
   
   Any value that goes outside the specified ranges for these inputs will respond with “ERROR”, else if everything is successful it will respond with “OK”  
   
8. **AT#RTC=date|day,month,year,weekday**  
   Sets the RCT Date , date parameters will be integer values.  
   
    **day** | 1 to 31
      --- | --- 
      **month** | 1 to 12 
      **year** | 00 to 99
      **Weekday** | 1 to 7  
      
      **EX**: AT#RTC=date|20,09,21,01   
      Will Set the date as **20th September 2021 and Weekday as 1** (i.e Monday or Sunday depends on user assumption)

9. **AT#RTC?date**  
    This command returns the date followed by “OK”
 
 
10. **AT#RTC=time|hour,minute,seconds,format,am/pm**  
    This command sets the time, all paramters take integer values and any value beyond general time parameter will result in error. 
    
    **format** refers to time-format  
     **0**  = 12 hour format   
     **1** =  24 hour format  

    **am/pm** referes to Ante meridiem and Post meridiem   
    **0** = AM  
    **1** = PM  
    for 24 hour format am/pm value is ignored.  

11. **AT#RTC?time**  
    This command returns the time followed by “OK”  
  
12. **AT#TEMP?**  
    This command returns the value of ambient temperature measured in °C, followed by “OK”. 
 
    **Note** : This command will automatically intializes the accelerometer parameter to its default value. 
    
13. **AT#HWCHK?**   
    This command does the basic hardware check. It is recomended to execute this command after reboot without any modifications. It checks the default state of the hardware after boot up.  
    
    Following hardware tests are made:    
    * Inertial module (LSM6DS33) Check  
    * RTC (MCP7940N) Check  
    * GPIO Expander (TCA6424A) Check  
    * EEPROM (CAT24C32) Check 
    * Raspberry Pi GPIO Physical Header Pins (11,13,36,37 should be HIGH & 16,18 should be LOW)
    * Relay Check
    * Front Pannel Check (Toggles all the EX_GPIO pins with 500ms delay)  
   
14. **AT#CELLCM?**  
    This command checks if the CellCM module is attached to AccessHAT.  

15. **AT#LORA?**  
    This command checks if the LoRA module is attached to AccessHAT and prints its verison details followed by an "OK".  
  
16. **AT#HATCHK?**  
    This command checks if the AccessHAT board is connected. It returns **HAT_PRESENT** with HAT Name if it is connected, if not then it return **HAT_MISSING**



If there is any input that the AccessHAT receives that is not specified above, it will reply with “ERROR”.
