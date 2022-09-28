#include <stdio.h>
#include "accesshat_inertial_module.h"

void wake_up_handler(void)
{
	printf("Wake up detected \n");

	/* Uncomment below if interrupt latch is enabled */
	//wake_up_clear_interrupt_latch();
}



int main(void)
{
	/* define wake up config */
	wake_up_config_typedef wu_my_conf;

	/* Set default values */
	wake_up_config_set_defaults(&wu_my_conf);

	/* initialize wake up detection*/
	wake_up_detection_init(&wu_my_conf);

	/* Set up wake up interrupt */
	wake_up_set_interrupt(&wu_my_conf,INT1_PIN,wake_up_handler);

	while(1)
	{

	}
}
