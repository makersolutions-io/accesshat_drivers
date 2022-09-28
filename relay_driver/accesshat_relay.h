/**
  *****************************************************************************************
  *@file    : accesshat_relay.h
  *@Brief   : AceesHAT Realy Driver Header file 
  
  *****************************************************************************************
*/

#ifndef ACCESSHAT_RELAY_H
#define ACCESSHAT_RELAY_H

#include <stdbool.h>

/*GPIO Expander Device ID*/
#define GPIO_EXP_ID 0x22   


/*GPIO Expander Registers */
#define CONFIG_PORT_0      0x0C
#define OUTPUT_PORT_0      0x04


/*Relay state enum */
enum {
       UNKNOWN_STATE,
       OPEN_STATE,
       CLOSED_STATE
     };

/* Relay Typedef */
typedef enum {RELAY_1, RELAY_2} relay_typedef;


/**
 *@brief    Open the Realy 
 *@param    relay_num : relay number
 *@retval   0 : On Success
           -1 : On Error
 */
int relay_open(relay_typedef relay_num);


/**
 *@brief    Close the Realy 
 *@param    relay_num : relay number
 *@retval   0 : On Success
           -1 : On Error
 */
int relay_close(relay_typedef relay_num);



/**
 *@brief    Get the given relay state 
 *@param    relay_num : relay number
 *@retval   0 : if relay closed
            1 : if relay is open
 */
int relay_get_state(relay_typedef relay_num);


#endif
