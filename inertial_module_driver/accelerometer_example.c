/**
  ****************************************************************************************************************
  *@file    : accelerometer_example.c
  *@Brief   : Sample accelerometer example file to test accelerometer function in AccessHAT Inertial module Driver
  
  ****************************************************************************************************************
*/


#include <stdio.h>
#include <stdint.h>
#include "accesshat_inertial_module.h"
#include <wiringPi.h>


int main(void)
{
	uint16_t raw_data[3];

	/*define accelerometer config*/
	accelerometer_config_typedef my_xl;

	/* Set default values */
	accelerometer_config_set_defaults(&my_xl);

	/*Inialize accelerometer */
	accelerometer_init(&my_xl);

	while(1)
	{
		accelerometer_get_raw_data(raw_data);
		printf("X = %f mg, Y = %f mg, Z = %f mg\n",lsm6ds3_from_fs2g_to_mg(raw_data[0]),lsm6ds3_from_fs2g_to_mg(raw_data[1]),lsm6ds3_from_fs2g_to_mg(raw_data[2]) );
		delay(100);
	}


}
