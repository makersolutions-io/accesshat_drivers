/**
  ****************************************************************************************************************
  *@file    : double_tap_example.c
  *@Brief   : Sample double-tap example file to test double-tap  function in AccessHAT Inertial module Driver
  
  ****************************************************************************************************************
*/


#include <stdio.h>
#include "accesshat_inertial_module.h"

void double_tap_handler(void)
{
	printf("double tap detected \n");
    
    /*uncomment below, if interrupt latch is enabled*/
	//double_tap_clear_interrupt_latch();
}


int main(void)
{ 
	/* define double tap config*/
	double_tap_config_typedef dt_my_conf;
    
        /*set default values*/
	double_tap_config_set_defaults(&dt_my_conf);

	/*Initialize double tap*/
	double_tap_detection_init(&dt_my_conf);

	/* Setup double tap interrupt */
	double_tap_set_interrupt(&dt_my_conf,INT1_PIN,double_tap_handler);

	while(1)
	{
		
	}
}
