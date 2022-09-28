
#include "accesshat_wiegand.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <time.h>
#include <unistd.h>
#include <memory.h>
#include <sys/time.h>
#include <errno.h>
#include <signal.h>


#define POUND_KEY 0x0D
#define STAR_KEY 0x1B
#define MAX_NUMBER_KEYS 6

volatile uint32_t card_temp_high=0;
volatile uint32_t card_temp=0;
uint32_t wiegand_data=0;
volatile int bit_count=0;	
char wiegand_keys[10];
int keys_counter = 0;

struct t_eventData{
    int myData;
};


static void generic_timer_handler(int sig, siginfo_t *si, void *uc);

timer_t timer_wiegand_conversion = 0;
timer_t timer_code_entry_done = 0;
struct t_eventData eventData = {.myData = 0};


static void print_and_reset_wiegand_keys();

/**
 *@brief    Sets the timer that executes a wiegand conversion from D0/D1 bit coming in.
 *@param    sec : seconds , usec : micro seconds
 *@retval   none 
 */
static int set_conversion_timer(int sec, long usec)
{
  struct itimerspec its;
  its.it_interval.tv_nsec = 0;
  its.it_interval.tv_sec = 0;
  its.it_value.tv_nsec = (long int) usec * 1000;
  its.it_value.tv_sec = (long int) sec;

  timer_settime(timer_wiegand_conversion, 0, &its, NULL);

  return 0;
}

/**
 *@brief    Sets the timer that executes a wiegand conversion is done to check if the user stops entering key presses.
 *@param    sec : seconds , usec : micro seconds
 *@retval   none 
 */
static int set_entry_done_timer(int sec, long usec)
{
  struct itimerspec its;
  its.it_interval.tv_nsec = 0;
  its.it_interval.tv_sec = 0;
  its.it_value.tv_nsec = (long int) usec * 1000;
  its.it_value.tv_sec = (long int) sec;

  timer_settime(timer_code_entry_done, 0, &its, NULL);

  return 0;
}

int wiegand_initialize(int d0pin, int d1pin)
{
	card_temp_high = 0;
	card_temp = 0;
	wiegand_data = 0;
	bit_count = 0;  

	// /*  sigevent specifies behaviour on expiration  */
	struct sigaction sa;
	struct sigevent timer_event_definition;

	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = generic_timer_handler;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGRTMIN, &sa, NULL) == -1)
	{
		printf("Failed to set up signal\n");
		return -1;
	}

	// This creates the timer that handles the D0/D1 conversion into wiegand bits
	// Started by a D0/D1.
	timer_event_definition.sigev_notify = SIGEV_SIGNAL;
	timer_event_definition.sigev_signo = SIGRTMIN;
	timer_event_definition.sigev_value.sival_ptr = &timer_wiegand_conversion;

	int res = timer_create(CLOCK_REALTIME, &timer_event_definition, &timer_wiegand_conversion);
	if (res != 0)
	{
		printf("Error creating D0/D1 timer: %s\n", strerror(errno));
		return -1;
	}

	// This creates the timer after a bit has been processed that waits if the user stops data entry.
	timer_event_definition.sigev_notify = SIGEV_SIGNAL;
	timer_event_definition.sigev_signo = SIGRTMIN;
	timer_event_definition.sigev_value.sival_ptr = &timer_code_entry_done;

	res = timer_create(CLOCK_REALTIME, &timer_event_definition, &timer_code_entry_done);
	if (res != 0)
	{
		printf("Error creating code entry done timer: %s\n", strerror(errno));
		return -1;
	}

	/* Setup wiring Pi */
	wiringPiSetup(); 

	/*Config data pins as input */
	pinMode(d0pin, INPUT);
	pinMode(d1pin, INPUT);

	/* Setup Interrupt on d0pin and d1pin */
	if(wiringPiISR(d0pin,INT_EDGE_FALLING, w_get_data0) < 0)
	{
		printf("wiegand_init : wiringPiISR : error\n");
		return -1;
	}


	if(wiringPiISR(d1pin,INT_EDGE_FALLING, w_get_data1) < 0)
	{
		printf("wiegand_init : wiringPiISR : error\n");
		return -1;
	}

	/* Set SIGALRM interrupt */
  	//signal(SIGALRM, generic_timer_handler); 

	return 0;
}

void w_get_data0(void)
{
	bit_count++;				// Increment bit count for Interrupt connected to D0
	if (bit_count>31)			// If bit count more than 31, process high bits
	{
		card_temp_high |= ((0x80000000 & card_temp)>>31);	//	shift value to high bits
		card_temp_high <<= 1;
		card_temp <<=1;
	}
	else
	{
		card_temp <<= 1;		// D0 represent binary 0, so just left shift card data
	}


	set_conversion_timer(0,25000);
  	set_entry_done_timer(0,0);
}

void w_get_data1(void)
{
	bit_count ++;				// Increment bit count for Interrupt connected to D1
	if (bit_count>31)			// If bit count more than 31, process high bits
	{
		card_temp_high |= ((0x80000000 & card_temp)>>31);	// shift value to high bits
		card_temp_high <<= 1;
		card_temp |= 1;
		card_temp <<=1;
	}
	else
	{
		card_temp |= 1;			// D1 represent binary 1, so OR card data with 1 then
		card_temp <<= 1;		// left shift card data
	}


	set_conversion_timer(0,25000);
  	set_entry_done_timer(0,0);
}

uint32_t get_card_id(volatile uint32_t *code_high, volatile uint32_t *code_low, char bit_length)
{
	if (bit_length==26)								// EM tag
		return (*code_low & 0x1FFFFFE) >>1;

	if (bit_length==24)
		return (*code_low & 0x7FFFFE) >>1;

	if (bit_length==34)								// Mifare 
	{
		*code_high = *code_high & 0x03;				// only need the 2 LSB of the code_high
		*code_high <<= 30;							// shift 2 LSB to MSB		
		*code_low >>=1;
		return *code_high | *code_low;
	}

	if (bit_length==32) {
		return (*code_low & 0x7FFFFFFE ) >>1;
	}
	return *code_low;								// EM tag or Mifare without parity bits
}

char translate_enter_escape_key_press(char original_keypress) {
	switch(original_keypress) {
	case 0x0b:        // 11 or * key
		return 0x0d;  // 13 or ASCII ENTER

	case 0x0a:        // 10 or # key
		return 0x1b;  // 27 or ASCII ESCAPE

	default:
		return original_keypress;
	}
}

static void print_and_reset_wiegand_keys()
{
	printf("Wiegand keys: %s\n", wiegand_keys);
	memset(wiegand_keys,0,10);
	keys_counter = 0;			
}

static void generic_timer_handler(int sig, siginfo_t *si, void *uc)
{
	timer_t *tidp;
	tidp = si->si_value.sival_ptr;

	if (*tidp == timer_wiegand_conversion)
	{
		do_wiegand_conversion();
	}
	else if (*tidp == timer_code_entry_done)
	{
		//return what was already collected
		print_and_reset_wiegand_keys();			
	}
	else
	{
		printf("Did not find Wiegand timer.\n");
	}
}

// Side effect -- uses and changes wiegand_keys
int do_wiegand_conversion()
{
	uint32_t card_ID;

	if (bit_count == 37)
	{
		card_temp >>= 2;
		card_temp &= 0xFFFFF;
		
	       	printf("%X\n", card_temp);

		bit_count = 0;
		card_temp = 0;
		card_temp_high = 0;
	}
	

	else if ((bit_count==24) || (bit_count==26) || (bit_count==32) || (bit_count==34) || (bit_count==8) || (bit_count==4)) 	// bitCount for keypress=4 or 8, Wiegand 26=24 or 26, Wiegand 34=32 or 34
	{
		if (bit_count != 37)
		{
			card_temp >>= 1;	// shift right 1 bit to get back the real value - interrupt done 1 left shift in advance
			if (bit_count>32)
			{		// bit count more than 32 bits, shift high bits right to make adjustment
				card_temp_high >>= 1;
			}			
		}

		if (bit_count==8)		// keypress wiegand with integrity
		{
			// 8-bit Wiegand keyboard data, high nibble is the "NOT" of low nibble
			// eg if key 1 pressed, data=E1 in binary 11100001 , high nibble=1110 , low nibble = 0001 
			char high_nibble = (card_temp & 0xf0) >>4;
			char low_nibble = (card_temp & 0x0f);
			bit_count=0;
			card_temp=0;
			card_temp_high=0;
			
			if (low_nibble == (~high_nibble & 0x0f))		// check if low nibble matches the "NOT" of high nibble.
			{
				wiegand_data = (int)translate_enter_escape_key_press(low_nibble);
				printf("Wiegand data: %d\n", wiegand_data);
				return 1;
			}
			else 
			{
				
				bit_count=0;
				card_temp=0;
				card_temp_high=0;
				return 0;
			}

			// TODO: Handle validation failure case!
		}
		else if (bit_count==4) 
		{
			// 4-bit Wiegand codes have no data integrity check so we just
			// read the LOW nibble.
			wiegand_data = (int)translate_enter_escape_key_press(card_temp & 0x0000000F);

			if (wiegand_data == STAR_KEY)
			{
				//erase what was collected
				memset(wiegand_keys,0,10);
				printf("Wiegand keys erased.\n");
				keys_counter = 0;
			}
			else if (wiegand_data == POUND_KEY)
			{
				//return what was already collected
				print_and_reset_wiegand_keys();
			}
			else // number key
			{
				wiegand_keys[keys_counter] = wiegand_data + '0'; //+ '0' converts 0-9 int to char
				keys_counter++;

				if (keys_counter == MAX_NUMBER_KEYS)
				{
					//return what was already collected
					print_and_reset_wiegand_keys();				
				}
				else
				{
					// Are we done? We don't know. Set a timer.
					//TODO: May change logic so that timer is not set until four keys are pressed
					set_entry_done_timer(1,500000); //Expires after 1.5 sec
				}
			}
	
			bit_count = 0;
			card_temp = 0;
			card_temp_high = 0;

			return 1;
		}
		else	// wiegand 26 or wiegand 34
		{

			card_ID = get_card_id(&card_temp_high, &card_temp, bit_count);
			bit_count=0;
			card_temp=0;
			card_temp_high=0;
			wiegand_data=card_ID;

			printf("Wiegand data: %d decimal   %X hex\n", wiegand_data, wiegand_data);

			return 1;
		}
	}
	else 
	{
		printf("No match in do_wiegand_conversion for bit_count = %d\n", bit_count);
		bit_count=0;			
		card_temp=0;
		card_temp_high=0;

		return 0;
	}	
		
	return 0;
}

