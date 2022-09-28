/**
  *****************************************************************************************
  *@file    : accesshat_eeprom.h
  *@Brief   : Header file for Driving AccessHAT EEPROM (CAT24C32)
  
  *****************************************************************************************
*/


#ifndef ACCESSHAT_EEPROM_H
#define ACCESSHAT_EEPROM_H

#include "i2c-dev.h"


#define RPI_I2C_DEVICE                "/dev/i2c-9"

#define EEPROM_I2C_DEVICE_ID          0x50




struct eeprom
{
	char *dev; 	// device file i.e. /dev/i2c-N
	int addr;	// i2c address
	int fd;		// file descriptor
	int type; 	// eeprom type
	int write_cycle_time;
};


/**
 *@brief    Write a byte to given eeprom memory address
 *@param    mem_addr : memory address
            data : data to be written
 *@retval   0 : On Success
           -1 : On Error
 */
int accesshat_eeprom_write_byte(__u16 mem_addr,__u8 data);



/**
 *@brief    Read a byte from given eeprom memory address
 *@param    mem_addr : memory address

 *@retval   read data on Success
           -1 On Error
 */
int accesshat_eeprom_read_byte(__u16 mem_addr);



/**
 *@brief    Write a string to given eeprom memory address
 *@param    mem_addr : memory address
            *str : pointer to string data
 *@retval   0 : On Success
           -1 : On Error
 */
int accesshat_eeprom_write_string(__u16 mem_addr,char *str);



/**
 *@brief    Read a string from given eeprom memory address
 *@param    mem_addr : memory address
 *@param    *buffer : pointer to buffer where data read is stored
 *@param    len : length of string 
 *@retval   read data on Success
           -1 On Error
 */
int accesshat_eeprom_read_string(__u16 mem_addr, char *buffer, int len);


#endif

