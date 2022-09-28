/**
  *****************************************************************************************
  *@file    : alarm_example.c
  *@Brief   : Sample alarm example file to test alarm function in AccessHAT RTC Driver
  
  *****************************************************************************************
*/


#include <stdio.h>
#include <stdint.h>
#include "accesshat_rtc.h"
#include <wiringPi.h>


void alarm_handler()
{
	printf("alarm_handler:alarm triggered\n");

	/*clear the alarm interrupt flag*/
	rtc_clear_alarm_flag(ALARM0);
}


int main()
{
	/*Set date*/
	rtc_set_date(0x17,0x08,0x21,0x02);

	/*Set time*/
	rtc_set_time(0x00,0x05,0x04,TIME_12H,PM);

	/*Set Alarm*/
	rtc_set_alarm(0x04,0x06,0x00, 0x08,0x017,0x02,ALARM0, ALL);

	/*Set Alarm interrupt onRPi Pin*/
	rtc_set_alarm_interrupt(alarm_handler);

	printf("alarm setup done\n");

	while(1)
	{

	}
}