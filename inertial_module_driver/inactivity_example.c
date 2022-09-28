/**
  ****************************************************************************************************************
  *@file    : inactivity_example.c
  *@Brief   : Sample inactivity example file to test inactivity function in AccessHAT Inertial module Driver
  
  ****************************************************************************************************************
*/

#include <stdio.h>
#include "accesshat_inertial_module.h"

void inactivity_handler(void)
{
	if(inactivity_get_event())
	{
		printf("inactivity detected \n");
	}
	else
	{
		printf("activity detected \n");
	}
	

}



int main(void)
{
	/* define inactivity config */
	inactivity_config_typedef in_my_conf;

	/* Set default values */
	inactivity_config_set_defaults(&in_my_conf);

	/* initialize inactivity detection*/
	inactivity_detection_init(&in_my_conf);

	/* Set up inactivity interrupt */
	inactivity_set_interrupt(INT1_PIN,inactivity_handler);

	while(1)
	{
		
	}
}
