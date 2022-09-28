/**
  *****************************************************************************************
  *@file    : accesshat_gpio.h
  *@Brief   : AceesHAT GPIO Driver Header file 
  
  *****************************************************************************************
*/

#ifndef ACCESSHAT_GPIO_H
#define ACCESSHAT_GPIO_H

#include <stdbool.h>

/*GPIO Expander Device ID*/
#define GPIO_EXP_ID 0x22 

/*GPIO Expander Registers */
#define CONFIG_PORT_1      0x0D
#define CONFIG_PORT_2      0x0E
#define OUTPUT_PORT_2      0x06
#define OUTPUT_PORT_1      0x05
#define OUTPUT_PORT_2      0x06
#define INPUT_PORT_1       0x01
#define INPUT_PORT_2       0x02


/* Expander GPIO Typedef (J7 Header on AccessHAT)*/
typedef enum {EX_GPIO10,
              EX_GPIO11,
              EX_GPIO12,
              EX_GPIO13,
              EX_GPIO14,
              EX_GPIO15,
              EX_GPIO16,
              EX_GPIO17,
              EX_GPIO20} gpio_typedef;


/**
 *@brief    Set the given GPIO pin as OUTPUT and set to HIGH/LOW 
 *@param    gpio_num : GPIO Pin Number 
 *@retval   0 : On Success
           -1 : On Error
 */
int gpio_set_output(gpio_typedef gpio_num, bool output_state);


/**
 *@brief    Set the given GPIO pin as input
 *@param    gpio_num : GPIO Pin Number 
 *@retval   0 : On Success
           -1 : On Error
 */
int gpio_set_input(gpio_typedef gpio_num);


/**
 *@brief    Read the given gpio pin value
 *@param    gpio_num : GPIO Pin Number  
 *@retval   input_value: returns logic leven on given pin
 */
int gpio_read(gpio_typedef gpio_num);



#endif
