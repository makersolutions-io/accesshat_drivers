/**
  *****************************************************************************************
  *@file    : date_example.c
  *@Brief   : Sample date example  file to test date function in AccessHAT RTC Driver
  
  *****************************************************************************************
*/

#include <stdio.h>
#include <stdint.h>
#include "accesshat_rtc.h"
#include <wiringPi.h>


int main()
{
  
  uint8_t date[4];
  rtc_set_date(0x31,0x12,0x99,0x07);

  delay(2000);

  rtc_get_date(date);

  printf("D:%x M:%x Y:20%x Wkday:%x\n",date[0],date[1],date[2],date[3]);
	
}
