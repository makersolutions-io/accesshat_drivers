/**
  ****************************************************************************************************************
  *@file    : gyroscope_example.c
  *@Brief   : Sample gyroscope example file to test gyroscope function in AccessHAT Inertial module Driver
  
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
	gyroscope_config_typedef my_gy;

	/* Set default values */
	gyroscope_config_set_defaults(&my_gy);

	/*Inialize accelerometer */
	gyroscope_init(&my_gy);

	while(1)
	{
		gyroscope_get_raw_data(raw_data);
		printf("X = %f mdps, Y = %f mdps, Z = %f mdps\n",lsm6ds3_from_fs250dps_to_mdps(raw_data[0]),lsm6ds3_from_fs250dps_to_mdps(raw_data[1]),lsm6ds3_from_fs250dps_to_mdps(raw_data[2]) );
		delay(100);
	}


}
