/**
  *****************************************************************************************
  *@file    : accesshat_relay.c
  *@Brief   : Source file for Driving AccessHAT Relays. 
  
  *****************************************************************************************
*/


/*****************************************************************************************
              Pin details
  _________________________________
  |GPIO Expander |      Relay     |
  |______________|________________|
  |    P02       |    RLY_CTL1    |
  |    P03       |    RLY_CTL2    |
  |    P04       |    RLY_CTL3    |
  |    P05       |    RLY_CTL4    |
  |______________|________________|

******************************************************************************************/



#include <stdio.h>
#include <stdint.h>
#include <wiringPiI2C.h> 
#include <wiringPi.h>
#include "accesshat_relay.h"
#include <unistd.h>

/*Relay state variables*/
static int relay_1_state;
static int relay_2_state;

/* Relay Pins Typedef */
typedef enum{RLY_CTL1,
             RLY_CTL2,
             RLY_CTL3,
             RLY_CTL4
           } relay_pin_typedef;


/**
 *@brief    Check the I2C communication status
 *@param    i2c_device_id 
 *@retval   fd : returns i2c setup status 
 */
static int check_i2c_setup(uint8_t i2c_device_id)
{
	int fd = wiringPiI2CSetup(i2c_device_id);
	if (fd == -1)
	{
		printf("Failed to init I2C communication.\n");
		return -1;
	}
	
	int config_reg = wiringPiI2CReadReg8(fd,CONFIG_PORT_0);
	if(config_reg == -1)
	{
		return -1;
	}

	//printf("I2C communication successfully setup.\n");
	return fd;
}





/**
 *@brief    Configure the relay pins P02,P03,P04,P05 as OUTPUT
 *@param    fd : i2c file discriptor 
 *@retval   0 : On Success
           -1 : On Error
 */
static int config_relay_pins_output(int fd)
{
	int config_reg,i2c_cmd,status;

	/* Read Configuration Port 0 contents */
	config_reg = wiringPiI2CReadReg8(fd,CONFIG_PORT_0);

	/* form i2c command such that P02,P03,P04,P05 are configured as output*/
	i2c_cmd = (config_reg & 0xC3);

	/*Send the i2c command byte */
	status = wiringPiI2CWriteReg8(fd,CONFIG_PORT_0,i2c_cmd);

	return status;
  
}






/**
 *@brief    Set the relay pins P02,P03,P04,P05 to LOW
 *@param    fd : i2c file discriptor 
 *@retval   0 : On Success
           -1 : On Error
 */
static int set_relay_pins_low(int fd)
{
	int config_reg,i2c_cmd, status;

	/* Read Output Port 0 contents */
	config_reg = wiringPiI2CReadReg8(fd,OUTPUT_PORT_0);

	/* form i2c command such that P02,P03,P04,P05 are driven LOW*/
	i2c_cmd = (config_reg & 0xC3);

	/*Send the i2c command byte */
	status= wiringPiI2CWriteReg8(fd, OUTPUT_PORT_0, i2c_cmd);

	return status;

}






/**
 *@brief    Set the given relay pin as HIGH
 *@param    fd : i2c file discriptor re, relay_pin 
 *@retval   0 : On Success
           -1 : On Error
 */
static int set_relay_pin_high(int fd, relay_pin_typedef relay_pin)
{
	int config_reg, i2c_cmd, status;

	/* Read Output Port 0 contents */
	config_reg = wiringPiI2CReadReg8(fd,OUTPUT_PORT_0);	

  /* Make the appropriate i2c commands based on RLY_CTL pin */
  switch (relay_pin)
  {
      case 0: i2c_cmd = (config_reg | 0x04); // Make RLY_CT1 = HIGH
              break;

      case 1: i2c_cmd = (config_reg | 0x08); // Make RLY_CT2 = HIGH
              break;

      case 2: i2c_cmd = (config_reg | 0x10); // Make RLY_CT3 = HIGH
              break;

      case 3: i2c_cmd = (config_reg | 0x20); // Make RLY_CT4 = HIGH
              break;   

      default: 
      printf("Error in set_relay_pin_high.\n");
      return -2;
  }

 	/*Send the i2c command byte */
	status= wiringPiI2CWriteReg8(fd, OUTPUT_PORT_0, i2c_cmd);

  return status;

}





/**
 *@brief    Open the Realy 
 *@param    relay_num : relay number 
 *@retval   0 : On Success
           -1 : On Error
 */
int relay_open(relay_typedef relay_num)
{
	int status;

	/*Check the I2C Setup status */
	int fd = check_i2c_setup(GPIO_EXP_ID);
	if (fd == -1)
	{
		printf("I2C Setup for Relay Failed \n");
		return -1;
	}

	/*Set the GPIO Exapander pins PO2,P03,P04,P05 as output */
	config_relay_pins_output(fd);


	/*Set the relay pins PO2,P03,P04,P05 to LOW */
	set_relay_pins_low(fd);


	if (relay_num == 0)
	{
		/*Send a 3ms pulse to open relay 1 (RLY_CTL1 = HIGH, RLY_CTL2 = LOW) */
		status = set_relay_pin_high(fd, RLY_CTL1);

		delay(3);

		/*Set the relay pins PO2,P03,P04,P05 to LOW */
		set_relay_pins_low(fd);
               
               /*Set the relay_1 state to open*/
               relay_1_state = OPEN_STATE;

	}

	else if (relay_num == 1)
	{
		/*Send a 3ms pulse to open relay 2 (RLY_CTL3 = HIGH, RLY_CTL4 = LOW) */
		status = set_relay_pin_high(fd, RLY_CTL3);

		delay(3);

		/*Set the relay pins PO2,P03,P04,P05 to LOW */
		set_relay_pins_low(fd);

                /*Set the relay_2 state to open*/
                relay_2_state = OPEN_STATE;
	}

	else
	{
		printf("Error in relay_open.\n");
		close(fd);
		return -2;
	}

  
  return status;

}






/**
 *@brief    Close the Realy 
 *@param    relay_num : relay number
 *@retval   0 : On Success
           -1 : On Error
 */
int relay_close(relay_typedef relay_num)
{
	int status;

	/*Check the I2C Setup status */
	int fd = check_i2c_setup(GPIO_EXP_ID);
	if (fd == -1)
	{
		printf("I2C Setup for Relay Failed \n");
		return -1;
	}

	/*Set the GPIO Exapander pins PO2,P03,P04,P05 as output */
	config_relay_pins_output(fd);

	/*Set the relay pins PO2,P03,P04,P05 to LOW */
	set_relay_pins_low(fd);

	if (relay_num == 0)
	{
		/*Send a 3ms pulse to open relay 1 (RLY_CTL1 = LOW, RLY_CTL2 = HIGH) */
		status = set_relay_pin_high(fd, RLY_CTL2);

		delay(3);

	        /*Set the relay pins PO2,P03,P04,P05 to LOW */
	        set_relay_pins_low(fd);

                /*Set the relay_1 state to closed*/
                relay_1_state = CLOSED_STATE;
	}

	else if (relay_num == 1)
	{
		/*Send a 3ms pulse to open relay 2 (RLY_CTL3 = LOW, RLY_CTL4 = HIGH) */
		status = set_relay_pin_high(fd, RLY_CTL4);

		delay(3);

	        /*Set the relay pins PO2,P03,P04,P05 to LOW */
	        set_relay_pins_low(fd);

                /*Set the relay_2 state to closed*/
                relay_2_state = CLOSED_STATE;
	}

	else
	{
		printf("Error in relay_close.\n");
		close(fd);
		return -2;
	}
	

  return status;

}




/**
 *@brief    Get the given relay state 
 *@param    relay_num : relay number
 *@retval   0 : if relay state unknown
            1 : if relay is open
	    2 : if relay is closed
 */
int relay_get_state(relay_typedef relay_num)
{
	int state;
	if(relay_num == 0)
    {
       	state =  relay_1_state;
    }
    else if(relay_num == 1)
    {
		state = relay_2_state;
	}

	else
	{
		printf("Error in relay_get_state.\n");
		return -2;
	}
	
	return state;
}

