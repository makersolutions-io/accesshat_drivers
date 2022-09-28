/**
  ***************************************************************************************************************************
  *@file    : temperature_example.c
  *@Brief   : Sample temperatureexample file to test temperature function in AccessHAT Inertial module Driver
  
  ***************************************************************************************************************************
*/


#include <stdio.h>
#include "accesshat_inertial_module.h"
#include "wiringPi.h"


int main(void)
{
	uint16_t raw_data[1];

	/*define accelerometer config*/
	accelerometer_config_typedef my_xl;

	/* Set default values */
	accelerometer_config_set_defaults(&my_xl);

	/*Inialize accelerometer */
	accelerometer_init(&my_xl);

	while(1)
	{
		temperature_get_raw_data(raw_data);
		printf("Temperature = %f C\n", lsm6ds3_from_lsb_to_celsius(raw_data[0]));
		delay(1000);
	}


}
