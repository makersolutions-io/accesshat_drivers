/**
  *****************************************************************************************
  *@file    : relay_example.c
  *@Brief   : Sample relay example file to test AccessHAT Relay Driver
  
  *****************************************************************************************
*/

#include <stdio.h>
#include <wiringPi.h>
#include "accesshat_relay.h"





int main()
{
  int relay_stat;

  relay_stat = relay_open(RELAY_1);
  if(relay_stat == 0)
  {
    printf("Successfully opened relay 1 \n");
  }
  else if(relay_stat == -1)
  {
    printf("Failed to open relay 1 \n");
  }

  /*Check relay state */
  if(relay_get_state(RELAY_1) == OPEN_STATE)
  {
    printf("Relay1 State: open\n");
  }
  else if (relay_get_state(RELAY_1) == CLOSED_STATE)
  {
    printf("Relay1 state : closed\n");
  }
  else
  {
   printf("Relay1 state : Unknown\n");
  }

  /*3 Second delay */
  delay(3000); 

  relay_stat = relay_close(RELAY_1);
  if(relay_stat == 0)
  {
    printf("Successfully closed relay 1 \n");
  }
  else if(relay_stat == -1)
  {
    printf("Failed to close relay 1 \n");
  }
  
  /*check relay state*/
  if(relay_get_state(RELAY_1) == CLOSED_STATE)
  {
    printf("Relay1 State: closed\n");
  }
  else if(relay_get_state(RELAY_1) == OPEN_STATE)
  {
    printf("Relay1 State : open\n");
  }
  else
  {
    printf("Relay1 state : unknown\n");
  }
