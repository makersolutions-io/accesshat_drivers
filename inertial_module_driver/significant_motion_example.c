/**
  ***************************************************************************************************************************
  *@file    : significant_motion_example.c
  *@Brief   : Sample significant motion example file to test significant motion function in AccessHAT Inertial module Driver
  
  ***************************************************************************************************************************
*/


#include <stdio.h>
#include "accesshat_inertial_module.h"

void significant_motion_handler(void)
{
	printf("significant motion detected \n");

	/* Uncomment below if interrupt latch is enabled*/
	//significant_motion_clear_interrupt_latch();
}



int main(void)
{
	/* define significant motion config */
	significant_motion_config_typedef sm_my_conf;

	/* Set default values */
	significant_motion_config_set_defaults(&sm_my_conf);
 
	/* initialize wake up detection*/
	significant_motion_detection_init(&sm_my_conf);

	/* Set up wake up interrupt */
	significant_motion_set_interrupt(&sm_my_conf,significant_motion_handler);

	while(1)
	{
		
	}
}
