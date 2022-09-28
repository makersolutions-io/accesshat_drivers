/**
  *****************************************************************************************
  *@file    : accesshat_gpio.c
  *@Brief   : Source file for Driving AccessHAT GPIO Pins at J7 header. 
  
  *****************************************************************************************
*/

/*****************************************************************************************
              Pin details
  _____________________________________________________
  |GPIO Expander |    J7 header   | GPIO Expander Port|
  |______________|________________|___________________|
  |    P10       |    EX_GPIO10   |                   |
  |    P11       |    EX_GPIO11   |                   |
  |    P12       |    EX_GPIO12   |                   |
  |    P13       |    EX_GPIO13   |  Ouput Port 1     |
  |    P14       |    EX_GPIO14   |                   |
  |    P15       |    EX_GPIO15   |                   |
  |    P16       |    EX_GPIO16   |                   |
  |    P17       |    EX_GPIO17   |                   |
  |______________|________________|___________________|
  |    P20       |    EX_GPIO20   |  Output Port 2    |
  |______________|________________|___________________|

******************************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <wiringPiI2C.h> 
#include <wiringPi.h>
#include "accesshat_gpio.h"
#include <unistd.h>




/**
 *@brief    Check the I2C communication status
 *@param    i2c_device_id 
 *@retval   fd : returns i2c setup status 
 */
static int check_i2c_setup(uint8_t i2c_device_id)
{
	int config_reg;
	int fd = wiringPiI2CSetup(i2c_device_id);
	if (fd == -1)
	{
		printf("Failed to init I2C communication.\n");
		return -1;
	}
	
	config_reg = wiringPiI2CReadReg8(fd,CONFIG_PORT_1);
	if(config_reg == -1)
	{
		printf("Failed to communicate with IO Expander\n");
		return -1;
	}

	//printf("I2C communication successfully setup.\n");
	return fd;
}





/**
 *@brief    Configure given GPIO pin as OUTPUT
 *@param    fd : file discriptor from i2c setup
 *          gpio_num : GPIO Pin Number
 *@retval   0 : On Success
           -1 : On Error
 */
static int config_gpio_output(uint8_t fd, gpio_typedef gpio_num)
{
	int status, config_reg, i2c_cmd;

	if(gpio_num == EX_GPIO20)
	{
	  /*for EX_GPIO20, Setup Configuration Port 2 Register*/

		/* Read the Configuration Port 2 */
		config_reg = wiringPiI2CReadReg8(fd,CONFIG_PORT_2);

		/* form i2c command such that Pin P20 is configured as output */
		i2c_cmd = (config_reg & 0xFE);

	  /*Send the i2c command byte */
	  status = wiringPiI2CWriteReg8(fd,CONFIG_PORT_2,i2c_cmd);

	  return status;
	}
	else
	{
		/* for EX_GPI10 ..EX_GPIO17, setup Configuration Port 1 Register*/

		/* Read the Configuration Port 1 */
		config_reg = wiringPiI2CReadReg8(fd,CONFIG_PORT_1);

    /* Make the appropriate i2c commands based on gpio_num pin */
    switch (gpio_num)
    {

      case 0: i2c_cmd = (config_reg & 0xFE); // Make EX_GPIO10 as Output
              break;

      case 1: i2c_cmd = (config_reg & 0xFD); // Make EX_GPIO11 as Output
              break;

      case 2: i2c_cmd = (config_reg & 0xFB); // Make EX_GPIO12 as Output
              break;

      case 3: i2c_cmd = (config_reg & 0xF7); // Make EX_GPIO13 as Output
              break; 

      case 4: i2c_cmd = (config_reg & 0xEF); // Make EX_GPIO14 as Output
              break;

      case 5: i2c_cmd = (config_reg & 0xDF); // Make EX_GPIO15 as Output
              break;

      case 6: i2c_cmd = (config_reg & 0xBF); // Make EX_GPIO16 as Output
              break;

      case 7: i2c_cmd = (config_reg & 0x7F); // Make EX_GPIO17 as Output
              break;   

      default: 
        printf("Error in config_gpio_output.\n");
        return -2;
    }

	  /*Send the i2c command byte */
	  status = wiringPiI2CWriteReg8(fd,CONFIG_PORT_1,i2c_cmd); 

	  return status;

  }

}






/**
 *@brief    Configure given GPIO Pin as INPUT
 *@param    fd : file discriptor from i2c setup
 *          gpio_num : GPIO Pin Number
 *@retval   0 : On Success
           -1 : On Error
 */
static int config_gpio_input(uint8_t fd, gpio_typedef gpio_num)
{
	int status, config_reg, i2c_cmd;

	if(gpio_num == EX_GPIO20)
	{
	  /*for EX_GPIO20, Setup Configuration Port 2 Register*/

		/* Read the Configuration Port 2 */
		config_reg = wiringPiI2CReadReg8(fd,CONFIG_PORT_2);

		/* form i2c command such that Pin P20 is configured as input */
		i2c_cmd = (config_reg | 0x01);

	  /*Send the i2c command byte */
	  status = wiringPiI2CWriteReg8(fd,CONFIG_PORT_2,i2c_cmd);

	  return status;
	}
	else
	{
		/* for EX_GPI10 ..EX_GPIO17, setup Configuration Port 1 Register*/

		/* Read the Configuration Port 1 */
		config_reg = wiringPiI2CReadReg8(fd,CONFIG_PORT_1);

    /* Make the appropriate i2c commands based on gpio_num pin */
    switch (gpio_num)
    {

      case 0: i2c_cmd = (config_reg | 0x01); // Make EX_GPIO10 as input
              break;

      case 1: i2c_cmd = (config_reg | 0x02); // Make EX_GPIO11 as input
              break;

      case 2: i2c_cmd = (config_reg | 0x04); // Make EX_GPIO12 as input
              break;

      case 3: i2c_cmd = (config_reg | 0x08); // Make EX_GPIO13 as input
              break; 

      case 4: i2c_cmd = (config_reg | 0x10); // Make EX_GPIO14 as input
              break;

      case 5: i2c_cmd = (config_reg | 0x20); // Make EX_GPIO15 as input
              break;

      case 6: i2c_cmd = (config_reg | 0x40); // Make EX_GPIO16 as input
              break;

      case 7: i2c_cmd = (config_reg | 0x80); // Make EX_GPIO17 as input
              break;   

      default:
        printf("Error in config_gpio_input.\n");
        return -2;
    }

	  /*Send the i2c command byte */
	  status = wiringPiI2CWriteReg8(fd,CONFIG_PORT_1,i2c_cmd); 

	  return status;
  }

}







/**
 *@brief    Set the Given GPIO pin to HIGH
 *@param    fd : file discriptor from i2c setup
 *          gpio_num : GPIO Pin Number
 *@retval   0 : On Success
           -1 : On Error
 */
static int set_gpio_high(uint8_t fd, gpio_typedef gpio_num)
{
	int status, config_reg, i2c_cmd;

	if(gpio_num == EX_GPIO20)
	{
	  /*for EX_GPIO20, Setup Output Port 2 Register*/

		/* Read the Output Port 2 register */
		config_reg = wiringPiI2CReadReg8(fd,OUTPUT_PORT_2);

		/* form i2c command such that Pin P20 is set as HIGH */
		i2c_cmd = (config_reg | 0x01);

	  /*Send the i2c command byte */
	  status = wiringPiI2CWriteReg8(fd,OUTPUT_PORT_2,i2c_cmd);

	  return status;
	}
	else
	{
		/* for EX_GPI10 ..EX_GPIO17, setup Ouput Port 1 Register*/

		/* Read the Ouput Port 1 */
		config_reg = wiringPiI2CReadReg8(fd,OUTPUT_PORT_1);

    /* Make the appropriate i2c commands based on gpio_num pin */
    switch (gpio_num)
    {

      case 0: i2c_cmd = (config_reg | 0x01); // Set EX_GPIO10 as HIGH
              break;

      case 1: i2c_cmd = (config_reg | 0x02); // Set EX_GPIO11 as HIGH
              break;

      case 2: i2c_cmd = (config_reg | 0x04); // Set EX_GPIO12 as HIGH
              break;

      case 3: i2c_cmd = (config_reg | 0x08); // Set EX_GPIO13 as HIGH
              break; 

      case 4: i2c_cmd = (config_reg | 0x10); // Set EX_GPIO14 as HIGH
              break;

      case 5: i2c_cmd = (config_reg | 0x20); // Set EX_GPIO15 as HIGH
              break;

      case 6: i2c_cmd = (config_reg | 0x40); // Set EX_GPIO16 as HIGH
              break;

      case 7: i2c_cmd = (config_reg | 0x80); // Set EX_GPIO17 as HIGH
              break;   

      default: 
        printf("Error in set_gpio_high.\n");
        return -2;
    }

	  /*Send the i2c command byte */
	  status = wiringPiI2CWriteReg8(fd,OUTPUT_PORT_1,i2c_cmd); 

	  return status;
  }

}






/**
 *@brief    Set the given GPIO pin to LOW
 *@param    fd : file discriptor from i2c setup
 *          gpio_num : GPIO Pin Number
 *@retval   0 : On Success
           -1 : On Error
 */
static int set_gpio_low(uint8_t fd, gpio_typedef gpio_num)
{
	int status, config_reg, i2c_cmd;

	if(gpio_num == EX_GPIO20)
	{
	  /*for EX_GPIO20, Setup Output Port 2 Register*/

		/* Read the Output Port 2 */
		config_reg = wiringPiI2CReadReg8(fd,OUTPUT_PORT_2);

		/* form i2c command such that Pin P20 is set to LOW */
		i2c_cmd = (config_reg & 0xFE);

	  /*Send the i2c command byte */
	  status = wiringPiI2CWriteReg8(fd,OUTPUT_PORT_2,i2c_cmd);

	  return status;
	}
	else
	{
		/* for EX_GPI10 ..EX_GPIO17, setup Configuration Port 1 Register*/

		/* Read the Configuration Port 1 */
		config_reg = wiringPiI2CReadReg8(fd,OUTPUT_PORT_1);

    /* Make the appropriate i2c commands based on gpio_num pin */
    switch (gpio_num)
    {

      case 0: i2c_cmd = (config_reg & 0xFE); // Set EX_GPIO10 as LOW
              break;

      case 1: i2c_cmd = (config_reg & 0xFD); // Set EX_GPIO11 as LOW
              break;

      case 2: i2c_cmd = (config_reg & 0xFB); // Set EX_GPIO12 as LOW
              break;

      case 3: i2c_cmd = (config_reg & 0xF7); // Set EX_GPIO13 as LOW
              break; 

      case 4: i2c_cmd = (config_reg & 0xEF); // Set EX_GPIO14 as LOW
              break;

      case 5: i2c_cmd = (config_reg & 0xDF); // Set EX_GPIO15 as LOW
              break;

      case 6: i2c_cmd = (config_reg & 0xBF); // Set EX_GPIO16 as LOW
              break;

      case 7: i2c_cmd = (config_reg & 0x7F); // Set EX_GPIO17 as LOW
              break;   

      default: 
        printf("Error in set_gpio_high.\n");
        return -2;
    }

	  /*Send the i2c command byte */
	  status = wiringPiI2CWriteReg8(fd,OUTPUT_PORT_1,i2c_cmd); 

	  return status;

  }

}







/**
 *@brief    Read the logic level from given gpio pin
 *@param    fd : file discriptor from i2c setup
 *          gpio_num : GPIO Pin Number
 *@retval   0 : On Success
           -1 : On Error
 */
static bool read_gpio_val(uint8_t fd, gpio_typedef gpio_num)
{
	int config_reg;
	bool value;

	if(gpio_num == EX_GPIO20)
	{
	  /*for EX_GPIO20, Read Input Port 2 Register*/
		config_reg = wiringPiI2CReadReg8(fd,INPUT_PORT_2);


    /* Read the specific bit 0 from input port 0 for P20 value */
		value = (config_reg & 0x01);

		return value;
	}
	else
	{
		/* for EX_GPI10 ..EX_GPIO17, read input Port 1 Register*/

		/* Read the Configuration Port 1 */
		config_reg = wiringPiI2CReadReg8(fd,INPUT_PORT_1);

    /* read the specific bit from input port 1 register base on given pin*/
    switch (gpio_num)
    {

      case 0: value = (config_reg & 0x01); // Read EX_GPIO10 
              break;

      case 1: value = (config_reg & 0x02); // Read EX_GPIO11 
              break;

      case 2: value = (config_reg & 0x04); // Read EX_GPIO12 
              break;

      case 3: value = (config_reg & 0x08); // Read EX_GPIO13 
              break; 

      case 4: value = (config_reg & 0x10); // Read EX_GPIO14 
              break;

      case 5: value = (config_reg & 0x20); // Read EX_GPIO15 
              break;

      case 6: value = (config_reg & 0x40); // Read EX_GPIO16
              break;

      case 7: value = (config_reg & 0x80); // Read EX_GPIO17
              break;   

      default: 
        printf("Error in read_gpio_val.\n");
        return -2;
    }

	  return value;

  }

}






/**
 *@brief    Set the given GPIO pin as OUTPUT and set to HIGH/LOW 
 *@param    gpio_num : GPIO Pin Number 
 *@retval   0 : On Success
           -1 : On Error
 */
int gpio_set_output(gpio_typedef gpio_num, bool output_state)
{
	int status,fd;

	/*Check the I2C Setup status for GPIO*/
	fd = check_i2c_setup(GPIO_EXP_ID);
	if (fd == -1)
	{
		printf("I2C Setup for GPIO Failed \n");
		return -1;
	}

  /* Set given gpio pin as output */
	config_gpio_output(fd,gpio_num);
  

  /*Set the HIGH or LOW Logic Level on given pin */
  if(output_state == true)
  {
  	status = set_gpio_high(fd,gpio_num);
  }
  else
  {
  	status = set_gpio_low(fd,gpio_num);
  }

  close(fd);

  return status;
  
}





/**
 *@brief    Set the given GPIO pin as input
 *@param    gpio_num : GPIO Pin Number 
 *@retval   0 : On Success
           -1 : On Error
 */
int gpio_set_input(gpio_typedef gpio_num)
{
  int status,fd;

  /*Check the I2C Setup status for GPIO*/
  fd = check_i2c_setup(GPIO_EXP_ID);
  if (fd == -1)
  {
    printf("I2C Setup for GPIO Failed \n");
    return -1;
  }

  /* Set given gpio pin as input */
  status = config_gpio_input(fd,gpio_num);
  

  close(fd);

  return status;
  
}







/**
 *@brief    Read from the given gpio pin 
 *@param    gpio_num : GPIO Pin Number  
 *@retval   input_value: returns logic leven on given pin
 */
int gpio_read(gpio_typedef gpio_num)
{
	int input_value,fd;

	/*Check the I2C Setup status for GPIO*/
	fd = check_i2c_setup(GPIO_EXP_ID);
	if (fd == -1)
	{
		printf("I2C Setup for GPIO Failed \n");
		return -1;
	}

  /*Read the given gpio pin value */
  input_value = read_gpio_val(fd,gpio_num);

  close(fd);

  return input_value;

}

