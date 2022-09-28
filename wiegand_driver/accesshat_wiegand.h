#ifndef _ACCESSHAT_WIEGAND_H
#define _ACCESSHAT_WIEGAND_H


#include <stdint.h>

#define WIEGAND_D0               25 // GPIO Pin 26 | Green cable | Data0 | WiringPi 
#define WIEGAND_D1               27 // GPIO Pin 16 | White cable | Data1 | WiringPi


int wiegand_initialize(int d0pin, int d1pin);
// uint32_t get_wiegand_data();
// int get_wiegand_type();

void w_get_data0(void);
void w_get_data1(void);

int do_wiegand_conversion();
uint32_t get_card_id(volatile uint32_t *code_high, volatile uint32_t *code_low, char bit_length);
char translate_enter_escape_key_press(char original_keypress);

#endif

