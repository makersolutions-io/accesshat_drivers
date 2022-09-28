/**
  *****************************************************************************************
  *@file    : gpio_example.c
  *@Brief   : Sample gpio example file to test accesshat_gpio Driver
  
  *****************************************************************************************
*/

#include <stdio.h>
#include <wiringPi.h>
#include "accesshat_gpio.h"


int main()
{

  int status, val;

  status = gpio_set_output(EX_GPIO10,true);
  if(status == -1)
  {
    printf("GPIO set output Failed \n");
  }
  else
  {
    printf("GPIO Set output for GPIO10 is successfull\n");
  }
  
  /*gpio_set_output(EX_GPIO11,true);
  gpio_set_output(EX_GPIO12,true);
  gpio_set_output(EX_GPIO13,true);
  gpio_set_output(EX_GPIO14,true);
  gpio_set_output(EX_GPIO15,true);
  gpio_set_output(EX_GPIO16,true);
  gpio_set_output(EX_GPIO17,true);
  gpio_set_output(EX_GPIO20,true); */



  /*gpio_set_input(EX_GPIO10);
  gpio_set_input(EX_GPIO11);
  gpio_set_input(EX_GPIO12);
  gpio_set_input(EX_GPIO13);
  gpio_set_input(EX_GPIO14);
  gpio_set_input(EX_GPIO15);
  gpio_set_input(EX_GPIO16);
  gpio_set_input(EX_GPIO17);
  gpio_set_input(EX_GPIO20); */

   
  /* Read a gpio pin */
  val = gpio_read(EX_GPIO15);
  printf("%d\n", val);
  
}
