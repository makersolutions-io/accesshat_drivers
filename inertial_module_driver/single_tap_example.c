/**
  ****************************************************************************************************************
  *@file    : single_tap_example.c
  *@Brief   : Sample single-tap example file to test single-tap  function in AccessHAT Inertial module Driver
  
  ****************************************************************************************************************
*/

#include <stdio.h>
#include "accesshat_inertial_module.h"

void single_tap_handler(void)
{
	printf("Single tap detected \n");
    
    /*uncomment below, if interrupt latch is enabled*/
	//single_tap_clear_interrupt_latch();
}


int main(void)
{ 
	/* define single tap config*/
	single_tap_config_typedef st_my_conf;
    
        /*set default values*/
	single_tap_config_set_defaults(&st_my_conf);

	/*Initialize single tap*/
	single_tap_detection_init(&st_my_conf);

	/* Setup single tap interrupt */
	single_tap_set_interrupt(&st_my_conf,INT1_PIN,single_tap_handler);

	while(1)
	{
		
	}
}
