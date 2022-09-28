/**
  *****************************************************************************************
  *@file    : accesshat_eeprom.c
  *@Brief   : Source file for Driving AccessHAT EEPROM (CAT24C32)
  
  *****************************************************************************************
*/



#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/fs.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include "accesshat_eeprom.h"



#define EEPROM_TYPE_UNKNOWN	          0
#define EEPROM_TYPE_8BIT_ADDR	        1
#define EEPROM_TYPE_16BIT_ADDR 	      2 


/*
 * i2c write 1byte 
 *
 */
static int i2c_write_1b(struct eeprom *e, __u8 buf)
{
	int r;
	// we must simulate a plain I2C byte write with SMBus functions
	r = i2c_smbus_write_byte(e->fd, buf);
	if(r < 0)
		fprintf(stderr, "Error i2c_write_1b: %s\n", strerror(errno));
	usleep(10);
	return r;
}



/*
 * i2c write 2byte 
 *
 */
static int i2c_write_2b(struct eeprom *e, __u8 buf[2])
{
	int r;
	// we must simulate a plain I2C byte write with SMBus functions
	r = i2c_smbus_write_byte_data(e->fd, buf[0], buf[1]);
	if(r < 0)
		fprintf(stderr, "Error i2c_write_2b: %s\n", strerror(errno));
	usleep(10);
	return r;
}



/*
 * i2c write 3byte 
 *
 */
static int i2c_write_3b(struct eeprom *e, __u8 buf[3])
{
	int r;
	// we must simulate a plain I2C byte write with SMBus functions
	// the __u16 data field will be byte swapped by the SMBus protocol
	r = i2c_smbus_write_word_data(e->fd, buf[0], buf[2] << 8 | buf[1]);
	if(r < 0)
		fprintf(stderr, "Error i2c_write_3b: %s\n", strerror(errno));
	usleep(10);
	return r;
}




#define CHECK_I2C_FUNC( var, label ) \
	do { 	if(0 == (var & label)) { \
		fprintf(stderr, "\nError: " \
			#label " function is required. Program halted.\n\n"); \
		exit(1); } \
	} while(0);



/*
 * opens the eeprom device at [dev_fqn] (i.e. /dev/i2c-N) whose address is
 * [addr] and set the eeprom_24c32 [e]
 */
static int eeprom_open(char *dev_fqn, int addr, int type, int write_cycle_time, struct eeprom* e)
{
	int funcs, fd, r;
	e->fd = e->addr = 0;
	e->dev = 0;
	
	fd = open(dev_fqn, O_RDWR);
	if(fd <= 0)
	{
		fprintf(stderr, "Error eeprom_open: %s\n", strerror(errno));
		return -1;
	}

	// get funcs list
	if((r = ioctl(fd, I2C_FUNCS, &funcs) < 0))
	{
		fprintf(stderr, "Error eeprom_open: %s\n", strerror(errno));
		return -1;
	}

	
	// check for req funcs
	CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_READ_BYTE );
	CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_WRITE_BYTE );
	CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_READ_BYTE_DATA );
	CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_WRITE_BYTE_DATA );
	CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_READ_WORD_DATA );
	CHECK_I2C_FUNC( funcs, I2C_FUNC_SMBUS_WRITE_WORD_DATA );

	// set working device
	if( ( r = ioctl(fd, I2C_SLAVE, addr)) < 0)
	{
		fprintf(stderr, "Error eeprom_open: %s\n", strerror(errno));
		return -1;
	}
	e->fd = fd;
	e->addr = addr;
	e->dev = dev_fqn;
	e->type = type;
	e->write_cycle_time = write_cycle_time;
	return 0;
}



/*
 * closees the eeprom device [e] 
 */
static int eeprom_close(struct eeprom *e)
{
	close(e->fd);
	e->fd = -1;
	e->dev = 0;
	e->type = EEPROM_TYPE_UNKNOWN;
	return 0;
}


/*
 * read and returns the eeprom byte at memory address [mem_addr] 
 * Note: eeprom must have been selected by ioctl(fd,I2C_SLAVE,address) 
 */
static int eeprom_read_byte(struct eeprom* e, __u16 mem_addr)
{
	int r;
	ioctl(e->fd, BLKFLSBUF); // clear kernel read buffer
	if(e->type == EEPROM_TYPE_8BIT_ADDR)
	{
		__u8 buf =  mem_addr & 0x0ff;
		r = i2c_write_1b(e, buf);
	} else if(e->type == EEPROM_TYPE_16BIT_ADDR) {
		__u8 buf[2] = { (mem_addr >> 8) & 0x0ff, mem_addr & 0x0ff };
		r = i2c_write_2b(e, buf);
	} else {
		fprintf(stderr, "ERR: unknown eeprom type\n");
		return -1;
	}
	if (r < 0)
		return r;
	r = i2c_smbus_read_byte(e->fd);
	return r;
}



/*
 * writes [data] at memory address [mem_addr] 
 * Note: eeprom must have been selected by ioctl(fd,I2C_SLAVE,address) 
 */
static int eeprom_write_byte(struct eeprom *e, __u16 mem_addr, __u8 data)
{
	int ret;

	if(e->type == EEPROM_TYPE_8BIT_ADDR) {
		__u8 buf[2] = { mem_addr & 0x00ff, data };
		ret = i2c_write_2b(e, buf);
		if (ret == 0 && e->write_cycle_time != 0) {
			usleep(1000 * e->write_cycle_time);
		}
		return ret;
	} else if(e->type == EEPROM_TYPE_16BIT_ADDR) {
		__u8 buf[3] = 
			{ (mem_addr >> 8) & 0x00ff, mem_addr & 0x00ff, data };
		ret = i2c_write_3b(e, buf);
		if (ret == 0 && e->write_cycle_time != 0) {
			usleep(1000 * e->write_cycle_time);
		}
		return ret;
	} 
	fprintf(stderr, "ERR: unknown eeprom type\n");
	return -1;
}





/**
 *@brief    Write a byte to given eeprom memory address
 *@param    mem_addr : memory address
            data : data to be written
 *@retval   0 : On Success
           -1 : On Error
 */
int accesshat_eeprom_write_byte(__u16 mem_addr,__u8 data)
{
	int status;
	char* device = RPI_I2C_DEVICE;
	int i2c_addr = EEPROM_I2C_DEVICE_ID;

	int eeprom_type = EEPROM_TYPE_16BIT_ADDR;
    int write_cycle_time = 5;
    struct eeprom e;

    status = eeprom_open(device, i2c_addr, eeprom_type, write_cycle_time, &e);	
    if(status == -1)
    {
    	printf("eeprom_open: error \n");
    	return -1;
    }

    status = eeprom_write_byte(&e,mem_addr,data);
    if(status == -1)
    {
    	printf("eeprom_write_byte: error \n");
    	return -1;
    }

    eeprom_close(&e);
    return 0;
}




/**
 *@brief    Read a byte from given eeprom memory address
 *@param    mem_addr : memory address

 *@retval   read data on Success
           -1 On Error
 */
int accesshat_eeprom_read_byte(__u16 mem_addr)
{
	int status, data_read;
	char* device = RPI_I2C_DEVICE;
	int i2c_addr = EEPROM_I2C_DEVICE_ID;

	int eeprom_type = EEPROM_TYPE_16BIT_ADDR;
    int write_cycle_time = 5;
    struct eeprom e;

    status = eeprom_open(device, i2c_addr, eeprom_type, write_cycle_time, &e);	
    if(status == -1)
    {
    	printf("eeprom_open: error \n");
    	return -1;
    }

    data_read = eeprom_read_byte(&e,mem_addr);
    if(data_read == -1)
    {
    	printf("eeprom_read_byte: error \n");
    	return -1;
    }

    eeprom_close(&e);

    return data_read;
}





/**
 *@brief    Write a string to given eeprom memory address
 *@param    mem_addr : memory address
            *str : pointer to string data
 *@retval   0 : On Success
           -1 : On Error
 */
int accesshat_eeprom_write_string(__u16 mem_addr,char *str)
{
    int status,i;
    char* device = RPI_I2C_DEVICE;
    int i2c_addr = EEPROM_I2C_DEVICE_ID;

    int eeprom_type = EEPROM_TYPE_16BIT_ADDR;
    int write_cycle_time = 5;
    struct eeprom e;
    
    
    status = eeprom_open(device, i2c_addr, eeprom_type, write_cycle_time, &e);  
    if(status == -1)
    {
        printf("eeprom_open: error \n");
        return -1;
    }
    
    i = 0;
    while(str[i] != '\0')
    {
       status = eeprom_write_byte(&e,mem_addr,(__u8)str[i]);
       if(status == -1)
       {
         printf("eeprom_write_byte: error \n");
         return -1;
       }
       i++;
       mem_addr++;
    }

    eeprom_close(&e);
    return 0;
}




/**
 *@brief    Read a string from given eeprom memory address
 *@param    mem_addr : memory address
 *@param    *buffer : pointer to buffer where data read is stored
 *@param    len : length of string 
 *@retval   read data on Success
           -1 On Error
 */
int accesshat_eeprom_read_string(__u16 mem_addr, char *buffer, int len)
{
    int status,i;
    char* device = RPI_I2C_DEVICE;
    int i2c_addr = EEPROM_I2C_DEVICE_ID;

    int eeprom_type = EEPROM_TYPE_16BIT_ADDR;
    int write_cycle_time = 5;
    struct eeprom e;

    status = eeprom_open(device, i2c_addr, eeprom_type, write_cycle_time, &e);  
    if(status == -1)
    {
        printf("eeprom_open: error \n");
        return -1;
    }
    
    i = 0;
    while(i < len)
    {
      buffer[i]  = eeprom_read_byte(&e,mem_addr);
      if(buffer[i] == -1)
      {
          printf("eeprom_read_byte: error \n");
          return -1;
      }
      i++;
      mem_addr++;
    }
    buffer[i] = '\0';
    eeprom_close(&e);

    return 0;
}






