/**
  *****************************************************************************************
  *@file    : eeprom_example.c
  *@Brief   : Sample eeprom example file to test accesshat_eeprom Driver
  
  *****************************************************************************************
*/

#include <stdio.h>
#include "accesshat_eeprom.h"

int main()
{
	
	accesshat_eeprom_write_byte((__u16)0x33,(__u8)0x11);

	int a = accesshat_eeprom_read_byte((__u16)0x33);

	printf("data = %x\n",a);

}
