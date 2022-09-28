/**
  *****************************************************************************************
  *@file    : time_example.c
  *@Brief   : Sample time example  file to test time function in AccessHAT RTC Driver
  
  *****************************************************************************************
*/

#include <stdio.h>
#include <stdint.h>
#include "accesshat_rtc.h"
#include <wiringPi.h>


int main()
{
  
  uint8_t time[5];

  int format;

  char *ptr;

  rtc_set_time(0x00,0x55,0x09,TIME_12H,PM);

  while(1)
  {
  	rtc_get_time(time);
    if(time[3] == 0x40)
    {
      format = 12;
    }
    else if(time[3]== 0x00)
    {
      format = 24;
    }

    if(time[4] == 0x20)
    {
      ptr = "PM";
    }
    else if(time[4] == 0x00)
    {
      ptr = "AM";
    }
    
  	printf("H:%x M:%x S:%x format:%dH %s\n",time[2],time[1],time[0],format,ptr);
  	delay(1000);
  }
	
}
