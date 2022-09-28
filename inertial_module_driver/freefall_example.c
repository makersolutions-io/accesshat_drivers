/**
  ****************************************************************************************************************
  *@file    : freefall_example.c
  *@Brief   : Sample freefall example file to test freefall function in AccessHAT Inertial module Driver
  
  ****************************************************************************************************************
*/


#include <stdio.h>
#include "accesshat_inertial_module.h"

void freefall_tap_handler(void)
{
	printf("freefall detected \n");
    
    /*uncomment below, if interrupt latch is enabled*/
	//freefall_clear_interrupt_latch();
}


int main(void)
{ 
	/* define freefall config*/
	freefall_config_typedef ff_my_conf;
    
        /*set default values*/
	freefall_config_set_defaults(&ff_my_conf);

	/*Initialize freefall*/
	freefall_detection_init(&ff_my_conf);

	/* Setup freefall interrupt */
	freefall_set_interrupt(&ff_my_conf,INT1_PIN,freefall_tap_handler);

	while(1)
	{
		
	}
}
