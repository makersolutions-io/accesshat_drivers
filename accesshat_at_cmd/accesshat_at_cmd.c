/**
  *****************************************************************************************
  *@File    : accesshat_at_cmd.c
  *@Brief   : Source code of AccessHAT  AT command application  
  
  *****************************************************************************************
*/


#include <stdio.h>
#include <string.h>  
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wiringPiI2C.h> 
#include <wiringSerial.h>
#include <accesshat_gpio.h>
#include <accesshat_relay.h>
#include <accesshat_eeprom.h>
#include <accesshat_rtc.h>
#include <accesshat_inertial_module.h>


/*Raw temperature data buffer */
uint16_t temp_raw_data[1];

/*Date Buffer*/
uint8_t date[4];

/*Time buffer */
uint8_t time[5];

/*Vendor name*/
char vendor_name[20];

/*Product name*/
char prod_name[20];

/*Product version*/
char version[20];




/**
 *@brief    Get the state of Given Raspberry Pi GPIO Pin
 *@param    pin_num : gpio number
 *          gpio_num : GPIO Pin Number
 *@retval   Gpio state : On Success
            -1 : On Error
 */
int pi_gpio_get_state(char pin_num[])
{

  FILE *fp;
  char data[20];
  int gpio_state;
  
  char cmd[12]="gpio read ";
  strcat(cmd,pin_num);

  /* Open the command for reading. */
  fp = popen(cmd, "r");

  if (fp == NULL) 
  {
    printf("Failed to run command\n" );
    return -1;
  }

  /* Read the output a line at a time - output it. */
  while (fgets(data, sizeof(data), fp) != NULL);
  
  if(data[0] == '1')
  {
   gpio_state = 1;
  }
  else if(data[0] == '0')
  {
   gpio_state = 0;
  }
  
  /* close */
  pclose(fp);
  return gpio_state;
}




/**
 *@brief    Set the given EX_GPIOx pin as Input/Output
            with given pin state
 *@param    pin:EX_GPIOx number, val: input/output 
 *@retval   none
 */
void at_gpio_set(gpio_typedef pin, int val)
{
   if(val == 0)
   {
    /*Set GPIO as input*/
    gpio_set_input(pin);
   }
   else if(val == 1)
   { 
    /*Set GPIO as ouput(Set Low initially*/
    gpio_set_output(pin,false);
   }
   else if(val == 2)
   {
    /*Set GPIO as ouput(Set High initially*/
    gpio_set_output(pin,true);
   }
   else if(val == -1)
   {
    /*Don’t Care*/
    /*Do nothing, GPIO state remain unchanged */
   }
}



/**
 *@brief    Parse the GPIO AT commands and configure EX_GPIOx
 *@param    argc: argument count, argv : argument 
 *@retval    0 : On Success
            -1 : On Error
 */
int at_gpio_conf(int argc, char** argv)
{
  int i, val, error_flag = 0, count = 0;
  unsigned char *str = strtok(argv[1],"|");
  str = strtok(NULL,"|");

  for(i=0;str[i]!='\0';i++)
  {
    if(str[i] == ',')
    {
        continue;
    } 
    else if(str[i] == '0')
    {
        count++;
        val = 0;
    }
    else if(str[i] == '1')
    {
        count++;
        val = 1;
    }
    else if(str[i] == '2')
    {
        count++;
        val = 2;
    }
    else if(str[i] == '-')
    {
        count++;
        i++;
        if(str[i] == '1')
        {
            val = -1;
        }
    }
    else
    {
        count++;
        printf("Invalid input : %c\n",str[i]);
        error_flag = 1;
    }

    switch (count)
    {
        case 1: at_gpio_set(EX_GPIO10,val);
                break;
        case 2: at_gpio_set(EX_GPIO11,val);
                break;
        case 3: at_gpio_set(EX_GPIO12,val);
                break;   
        case 4: at_gpio_set(EX_GPIO13,val);
                break;  
        case 5: at_gpio_set(EX_GPIO14,val);
                break;  
        case 6: at_gpio_set(EX_GPIO15,val);
                break;  
        case 7: at_gpio_set(EX_GPIO16,val);
                break; 
        case 8: at_gpio_set(EX_GPIO17,val);
                break;
        case 9: at_gpio_set(EX_GPIO20,val);
                break;                                                                                      
        default: printf("GPIO Number exceeded : GPIO Num = %d\n ", count);
                 error_flag = 1;
                break;
    }
  }

  if(error_flag == 1)
  {
    printf("ERROR\n");
    return -1;
  }
  else
  {
    printf("OK\n");
  }
  return 0;
}




/**
 *@brief    Read EX_GPIOx Pin state
 *@param    argc: argument count, argv : argument
 *@retval    0 : On Success
            -1 : On Error
 */
int at_gpio_read(int argc, char **argv)
{
  char *str = strtok(argv[1],"?");
  str = strtok(NULL,"?");
  /*check for input values range */ 
  if(!((str[0] >= 48) && (str[0] < 57)))
  {
    printf("Invalid Input\n");
    printf("ERROR\n");
    return -1;
  }

  /* Read the GPIOs */
  char c = str[0];
  char *ptr = &c;
  int pin = atoi(ptr);
  bool data_read;

  switch (pin)
  {
    case 0: data_read = gpio_read(EX_GPIO10);
            break;
    case 1: data_read = gpio_read(EX_GPIO11);
            break;  
    case 2: data_read = gpio_read(EX_GPIO12);
            break; 
    case 3: data_read = gpio_read(EX_GPIO13);
            break; 
    case 4: data_read = gpio_read(EX_GPIO14);
            break;   
    case 5: data_read = gpio_read(EX_GPIO15);
            break; 
    case 6: data_read = gpio_read(EX_GPIO16);
            break;  
    case 7: data_read = gpio_read(EX_GPIO17);
            break;
    case 8: data_read = gpio_read(EX_GPIO20);
            break;                                                                              
    default:
      break;
    }
    
    printf("%d\n",data_read);
    printf("OK\n");
    return 0;
}





/**
 *@brief    Does a digital write to specified pin (EX_GPIOx and RLY_CTLx pins)
 *@param    argc: argument count, argv : argument
 *@retval    0 : On Success
            -1 : On Error
 */
int at_gpio_write(int argc, char **argv)
{
  int gpio_pin, delay_time;
  char *str = strtok(argv[1],"=");
  str = strtok(NULL,"=");  
  char *temp = str;
  /*Check the input ranage and values */
  if((str[0] == '1' || str[0] == '2') && (str[1] == ',') && ( (str[2] >= 48) && (str[2] < 57)) && (str[3] == ',') && ((str[4] >= 48) && (str[4] < 53)))
  {
    if(str[4] == '3' || str[4] == '4')
    {
      if((str[5] == ',') && (str[6] != '\0'))
      {
        /*Extract delay value*/
        temp = strtok(temp,",");
        temp = strtok(NULL,",");
        temp = strtok(NULL,",");
        temp = strtok(NULL,",");
        delay_time = atoi(temp);
      }
      else
      {
        printf("Invalid Input\n");
        printf("ERROR\n");
        return -1;
      }
    }
  }
  else
  {
    printf("Invalid Input\n");
    printf("ERROR\n");
    return -1;
  }

  /*Set gpio_pin value based on index*/
  switch (str[2])
  {
    case '0': gpio_pin = EX_GPIO10;
              break;
    case '1': gpio_pin = EX_GPIO11;
              break; 
    case '2': gpio_pin = EX_GPIO12;
              break;              
    case '3': gpio_pin = EX_GPIO13;
              break;              
    case '4': gpio_pin = EX_GPIO14;
              break;              
    case '5': gpio_pin = EX_GPIO15;
              break;              
    case '6': gpio_pin = EX_GPIO16;
              break;          
    case '7': gpio_pin = EX_GPIO17;
              break;              
    case '8': gpio_pin = EX_GPIO20;
              break;              
    default:
    break;
  }


  if(str[0] == '1')
  {
    /*GPIO Handling*/
    if(str[4] == '0')
    {
      /*Toggle GPIO*/
      if(gpio_read(gpio_pin))
      {
        gpio_set_output(gpio_pin, false);
      }
      else
      {
        gpio_set_output(gpio_pin, true);
      }
    }
    else if(str[4] == '1')
    {
      /*Set GPIO to LOW*/
      gpio_set_output(gpio_pin, false);
    }
    else if(str[4] == '2')
    {
      /*Set GPIO to HIGH*/
      gpio_set_output(gpio_pin, true);
    }
    else if(str[4] == '3')
    {
      /*Set GPIO HIGH*/
      gpio_set_output(gpio_pin, true);

      /* Delay*/
      delay(delay_time);

      /*Set GPIO LOW*/
      gpio_set_output(gpio_pin, false);
    }
    else if(str[4] == '4')
    {
      /*Set GPIO LOW*/
      gpio_set_output(gpio_pin, false);

      /* Delay*/
      delay(delay_time);

      /*Set GPIO HIGH*/  
      gpio_set_output(gpio_pin, true);    
    }    
  }
  else if (str[0] == '2')
  {
    /*Relay Handling */
    if(str[2] == '0')
    {
      /*Relay1 handling*/
      if(str[4] == '0')
      {
        /*Toggle*/
        /* do nothing as relay state is known*/
      }
      else if(str[4] == '1')
      {
        /*close Relay1*/
        relay_close(RELAY_1);
      }
      else if(str[4] == '2')
      {
        /*Open Relay1*/
        relay_open(RELAY_1);
      }
      else if(str[4] == '3')
      {
        /*Open Relay1*/
        relay_open(RELAY_1);

        /* Delay*/
        delay(delay_time);

        /*Close Relay1*/
        relay_close(RELAY_1);
      }
      else if(str[4] == '4')
      {
        /*Close Relay1*/
        relay_close(RELAY_1);

        /* Delay*/
        delay(delay_time);

        /*Open Relay1*/   
        relay_open(RELAY_1);     
      }
    }
    else if(str[2] == '1')
    {
      /*Relay2 handling*/
      if(str[4] == '0')
      {
        /*Toggle*/
        /* do nothing as relay state is known*/
      }
      else if(str[4] == '1')
      {
        /*close Relay2*/
        relay_close(RELAY_2); 
      }
      else if(str[4] == '2')
      {
        /*Open Relay2*/
        relay_open(RELAY_2); 
      }
      else if(str[4] == '3')
      {
        /*Open Relay2*/
        relay_open(RELAY_2);

        /* Delay*/
        delay(delay_time);

        /*Close Relay2*/
        relay_close(RELAY_2);
      }
      else if(str[4] == '4')
      {
        /*Close Relay2*/
        relay_close(RELAY_2);

        /* Delay*/
        delay(delay_time);

        /*Open Relay2*/
        relay_open(RELAY_2);        
      }      
    }
  }
  printf("OK\n");
  return 0;
}







/**
 *@brief    Read the temperature data from Inertial module
 *@param    none
 *@retval   none
 */
void at_get_temp(void)
{
  /*define accelerometer config*/
  accelerometer_config_typedef my_xl;

  /* Set default values */
  accelerometer_config_set_defaults(&my_xl);

  /*Inialize accelerometer */
  accelerometer_init(&my_xl);

  temperature_get_raw_data(temp_raw_data);
  printf("%f °C\n", lsm6ds3_from_lsb_to_celsius(temp_raw_data[0]));
  
  printf("OK\n");
}



/**
 *@brief    Read the EEPROM data byte
 *@param    argc: argument count, argv : argument
 *@retval    0 : On Success
            -1 : On Error
 */
int at_eeprom_read(int argc, char **argv)
{
  int data;
  char *str = strtok(argv[1],"?");
  str = strtok(NULL,"?");
  int addr = (int)strtol(str,NULL,16);

  data = accesshat_eeprom_read_byte((__u16)addr);
  if(data == -1)
  {
    printf("Error Reading EEPROM\n");
    return -1;
  }

  printf("%X\n",data);
  printf("OK\n");
  return 0;
}



/**
 *@brief    Read data byte from EEPROM
 *@param    argc: argument count, argv : argument
 *@retval    0 : On Success
            -1 : On Error
 */
int at_eeprom_write(int argc, char **argv)
{
  int status;
  char *str = strtok(argv[1],"=");
  str = strtok(NULL,"=");

  /*Get address val*/
  str = strtok(str,",");
  int addr = (int)strtol(str,NULL,16);

  /*Get data value*/
  str = strtok(NULL,",");
  int data = (int)strtol(str,NULL,16);

  /*Write the data to EEPROM*/
  status = accesshat_eeprom_write_byte((__u16)addr,(__u8)data);

  if(status == -1)
  {
    printf("Error Writing to EEPROM\n");
    return -1;
  }

  printf("OK\n");
  return 0;
}




/**
 *@brief    Set RTC Date
 *@param    argc: argument count, argv : argument
 *@retval    0 : On Success
            -1 : On Error
 */
int at_rtc_set_date(int argc, char **argv)
{
  int status;
  char day_buff[4] = "0x";
  char month_buff[4] = "0x";
  char year_buff[4] = "0x";
  char week_buff[4] = "0x";

  char *str = strtok(argv[1],"|");
  str = strtok(NULL,"|");
  str = strtok(str,",");
  
  /*Get day*/
  strcat(day_buff,str);
  int day = (int)strtol(day_buff,NULL,16);

  if(day > 0x31 || day < 0x01)
  {
    printf("Invalid Input : Day = %x\n",day);
    printf("ERROR\n");
    return -1;
  }
  
  /*Get month*/
  str = strtok(NULL,",");
  strcat(month_buff,str);
  int month = (int)strtol(month_buff,NULL,16);

  if(month > 0x12 || month < 0x01)
  {
    printf("Invalid Input : month = %x\n",month);
    printf("ERROR\n");
    return -1;
  }

  /*Get Year*/
  str = strtok(NULL,",");
  strcat(year_buff,str);
  int year = (int)strtol(year_buff,NULL,16);

  if(year > 0x99 || year < 0x00)
  {
    printf("Invalid Input : year = %x\n",year);
    printf("ERROR\n");
    return -1;
  }

  /*Get Weekday*/
  str = strtok(NULL,",");
  strcat(week_buff,str);
  int weekday = (int)strtol(week_buff,NULL,16);
  
  if(weekday > 0x07 || weekday < 0x01)
  {
    printf("Invalid Input : weekday = %x\n",weekday);
    printf("ERROR\n");
    return -1;
  }

  status = rtc_set_date(day,month,year,weekday);
  if(status == -1)
  {
    printf("Error Setting Date\n");
    return -1;
  }

  printf("OK\n");

  return 0;
}




/**
 *@brief    Read RTC Date
 *@param    argc: argument count, argv : argument
 *@retval    0 : On Success
            -1 : On Error
 */
int at_rtc_get_date(int argc, char **argv)
{
  int status;
  status = rtc_get_date(date);
  if(status == -1)
  {
    printf("Error Reading Date\n");
    return -1;
  }

  printf("Day=%x Month=%x Year=20%x Weekday=%x\n",date[0],date[1],date[2],date[3]); 
  printf("OK\n");
  return 0;
}


/**
 *@brief    Set RTC Time
 *@param    argc: argument count, argv : argument
 *@retval    0 : On Success
            -1 : On Error
 */
int at_rtc_set_time(int argc, char **argv)
{
  uint8_t format, ampm;
  char sec_buff[4] = "0x";
  char min_buff[4] = "0x";
  char hour_buff[4] = "0x";

  char *str = strtok(argv[1],"|");
  str = strtok(NULL,"|");
  str = strtok(str,",");
  
  /*Get hour*/
  strcat(hour_buff,str);
  int hour = (int)strtol(hour_buff,NULL,16);

  if(hour > 0x24 || hour < 0x00)
  {
    printf("Invalid Input : hour = %x\n",hour);
    printf("ERROR\n");
    return -1;
  }

  /*Get minute*/
  str = strtok(NULL,",");
  strcat(min_buff,str);
  int min = (int)strtol(min_buff,NULL,16);

  if(min > 0x60 || min < 0x00)
  {
    printf("Invalid Input : minute = %x\n",min);
    printf("ERROR\n");
    return -1;
  }

  /*Get Seconds*/
  str = strtok(NULL,",");
  strcat(sec_buff,str);
  int sec = (int)strtol(sec_buff,NULL,16);

  if(sec > 0x60 || sec < 0x00)
  {
    printf("Invalid Input : Seconds = %x\n",sec);
    printf("ERROR\n");
    return -1;
  }
  


  /*Get Format*/
  str = strtok(NULL,",");
  if(!(strcmp(str,"0")))
  {
    format = TIME_12H;
  }
  else if(!(strcmp(str,"1")))
  {
    format = TIME_24H;
  }
  else
  {
    printf("Invalid input: format = %s",str);
    return -1;
  }
  
  /*if fomrat is 12H then get the AM/PM details*/
  if(format == TIME_12H)
  { 
    /*Get AM/PM details*/
    str = strtok(NULL,",");
    if(!(strcmp(str,"0")))
    {
      ampm = AM;
    }
    else if(!(strcmp(str,"1")))
    {
      ampm = PM;
    }
    else
    {
      printf("Invalid input: AM/PM = %s",str);
      return -1;
    }
  }
  else if(format == TIME_24H)
  {
    ampm = 0;
  }
   

  rtc_set_time(sec,min,hour,format,ampm);
  
  printf("OK\n");

  return 0;  
}


/**
 *@brief    Read RTC Time
 *@param    argc: argument count, argv : argument
 *@retval    0 : On Success
            -1 : On Error
 */
int at_rtc_get_time(int argc, char **argv)
{
  int format, status;
  char *ptr;
    
  status = rtc_get_time(time);

  if(status == -1)
  {
    printf("Error Reading Time\n");
    return -1;
  }
  if(time[3] == 0x40)
  {
    format = 12;
  }
  else if(time[3]== 0x00)
  {
    format = 24;
  }

  if(time[4] == 0x20)
  {
    ptr = "PM";
  }
  else if(time[4] == 0x00)
  {
    ptr = "AM";
  }
  
  if(format == 24)
  {
   printf("Hour=%x Minute=%x Second=%x format=%dH\n",time[2],time[1],time[0],format);
  }
  else if(format == 12)
  {
   printf("Hour=%x Minute=%x Second=%x format=%dH %s\n",time[2],time[1],time[0],format,ptr);
  }
 
  printf("OK\n");
  return 0;

}






/**
 *@brief    Perform Basic AccessHAT hardware checks. 
 *@param    argc: argument count, argv : argument
 *@retval   -1 if any error.
 */
int at_hwchk(int argc, char **argv)
{ 
  int imu_state, rtc_state, io_exp_state, eeprom_state, header_state, relay_state;
  imu_state = rtc_state = io_exp_state = eeprom_state = header_state = relay_state = 0;
 
  /*Inertial Module Check*/
  if(!(read_who_am_i()))
  {
    printf("Inertial Module Check = OK\n");
  }
  else
  {
    printf("Inertial Module check = Failed\n");
    imu_state = -1;
  }

  /*RTC check*/
  rtc_get_date(date);
  if((date[0] == 0xff) && (date[1] == 0xff) && (date[2] == 0xff))
  {
    printf("RTC Check = Failed \n");
    rtc_state = -1;
  }
  else
  {
   printf("RTC Check = OK \n");
  }

  /*IO Expander Check*/
  int val;
  val = gpio_read(EX_GPIO10);
  if(val == -1)
  {
    printf("IO Expander Check = Failed\n");
    io_exp_state = -1;
  }
  else
  {
    printf("IO Expander Check = OK\n");
  }

  /*EEPROM Check*/
  uint8_t eeprom_data;
  accesshat_eeprom_write_byte((__u16)0x7fe1,(__u8)0xAB);
  eeprom_data = accesshat_eeprom_read_byte((__u16)0x7fe1);
  if(eeprom_data == 0xAB)
  {
    printf("EEPROM Check = OK\n");
  }
  else
  {
    printf("EEPROM Check = Failed\n");
    eeprom_state = -1;
  }

  /*RPi Physical GPIO Header check*/
  int gpios_state[6];
  gpios_state[0] = pi_gpio_get_state("0"); //Pi Physical Header pin 11
  gpios_state[1] = pi_gpio_get_state("2"); //Pi Physical Header pin 13
  gpios_state[2] = pi_gpio_get_state("4"); //Pi Physical Header pin 16
  gpios_state[3] = pi_gpio_get_state("5"); //Pi Physical Header pin 18
  gpios_state[4] = pi_gpio_get_state("27"); //Pi Physical Header pin 36
  gpios_state[5] = pi_gpio_get_state("25"); //Pi Physical Header pin 37

  if((gpios_state[0] == 1) && (gpios_state[1] == 1) && (gpios_state[2] == 0) && (gpios_state[3] == 0) && (gpios_state[4] == 1) && (gpios_state[5] == 1) )
  {
    printf("Rpi GPIO Header State Check = OK\n");
  }
  else
  {
    printf("Rpi GPIO Header state Check = Failed\n");
    header_state = -1;
  }
  
  /*Relay Check */
  int r1_open, r1_close, r2_open,r2_close;
  if(relay_open(RELAY_1) == -1)
  {
    r1_open = UNKNOWN_STATE;
  }
  else
  {
   r1_open = relay_get_state(RELAY_1);
   delay(500);
  }

  if(relay_open(RELAY_2) == -1)
  {
    r2_open = UNKNOWN_STATE;
  }
  else
  {
   r2_open = relay_get_state(RELAY_2);
   delay(500);
  }

  if(relay_close(RELAY_1) == -1)
  {
    r1_close = UNKNOWN_STATE;
  }
  else
  {
   r1_close = relay_get_state(RELAY_1);
   delay(500);
  }

  if(relay_close(RELAY_2) == -1)
  {
    r2_close = UNKNOWN_STATE;
  }
  else
  {
   r2_close = relay_get_state(RELAY_2); 
  }

  if(((r1_open == OPEN_STATE) && (r2_open == OPEN_STATE)) && ((r1_close == CLOSED_STATE)  && (r2_close == CLOSED_STATE)))
  {
    printf("Relay Check = OK\n");
  }
  else
  {
    printf("Relay Check = Failed\n");
    relay_state = -1;
  }
  
  if((imu_state == -1) && (rtc_state == -1) && (io_exp_state == -1) && (eeprom_state == -1) && (header_state == -1) && (relay_state == -1))
  {
   return -1;
  }
 
  /*Front Pannel Check */
  int i;
  printf("Front Pannel Check = Testing ...\n");
  for(i = 0; i < 9; i++)
  {
    gpio_set_output(i, true);
    delay(500);
    gpio_set_output(i, false);
    delay(500);
  }
  printf("OK\n");
  return 0;

}




/**
 *@brief    Test Cell CM module 
 *@param    argc: argument count, argv : argument
 *@retval   0 : On Success
           -1 : On Error
 */
int at_cell_cm(int argc, char **argv)
{
  int fd,c,i,byte_count;
  printf("Testing Cell CM Module ...\n");

  /******** Set the RESET_CM pin to High *********/

  /*Check the I2C Setup status */
  fd = wiringPiI2CSetup(GPIO_EXP_ID);
  if (fd == -1)
  {
     printf("I2C Setup for Cell CM Failed \n");
     return -1;
  }

  int config_reg,i2c_cmd,status;

  /* Read Configuration Port 0 contents */
  config_reg = wiringPiI2CReadReg8(fd,CONFIG_PORT_0);

  /* form i2c command such that P00 is configured as output*/
  i2c_cmd = (config_reg & 0xFD);

  /*Send the i2c command byte */
  status = wiringPiI2CWriteReg8(fd,CONFIG_PORT_0,i2c_cmd);
  if(status == -1)
  {
    printf("I2C Send Command Failed\n");
    return -1;
  }

  config_reg = wiringPiI2CReadReg8(fd,OUTPUT_PORT_0);

  i2c_cmd = (config_reg | 0x02);

  status = wiringPiI2CWriteReg8(fd, OUTPUT_PORT_0, i2c_cmd);
  if(status == -1)
  {
    printf("I2C Send Command Failed\n");
    return -1;
  }
  
  close(fd);

  /*wait 10 seconds so that Cell CM boots up successfully*/
  delay(10000);

  
  char data[60];
  fd = serialOpen ("/dev/ttyS0", 115200);
  if( fd == -1)
  {
    printf("Error opening /dev/ttyS0 \n");
    return -1;
  }
        
  serialPrintf(fd, "AT+GMM\r\n");
  c = i = 0;
  
  delay(500);
  byte_count = serialDataAvail(fd);

  while (byte_count > 0)
  {       
    c = serialGetchar(fd);
    data[i] = c;
    byte_count--;
    i++;          
  }
  
  data[i] = '\0';
  char *str;
 
  if(strstr(data,"OK"))
  {
     printf("Cell CM Detected = ");
     char *str = strtok(data,"\n");
     str = strtok(NULL,"\n");
     str = strtok(NULL,"\n");
     printf("%s\n",str);
  }
  else
  {
    printf("No Communication Module Detected\n");
  }

  serialFlush(fd); 
  serialClose(fd);

  return 0;

}


/**
 *@brief    Test LoRa module 
 *@param    argc: argument count, argv : argument
 *@retval   0 : On Success
           -1 : On Error
 */
int at_lora(int argc, char **argv)
{
  int fd, c,i, byte_count;
  char data[60];
        
  fd = serialOpen ("/dev/ttyS0", 57600);
  if( fd == -1)
  {
    printf("Error Opening /dev/ttyS0 \n");
    return -1;
  }

  serialPrintf (fd, "sys get ver\r\n");
  c = i = 0;
 
  delay(500);
  byte_count = serialDataAvail(fd);
  
  while(byte_count > 0)
  {
    c = serialGetchar(fd);
    data[i] = c;
    byte_count--;
    i++;
  }
 
  data[i] = '\0';

  if(strstr(data,"RN"))
  {
    printf("LoRa Module Detected = %s\n",data);
  }
  else
  {
    printf("No Communication Module Detected\n");
  }

  serialFlush(fd); 
  serialClose(fd);
  return 0;
}

/**
 *@brief    Check if AccessHAT is connected. 
 *@param    argc: argument count, argv : argument
 *@retval   0 : On Success
           -1 : On Error
 */
int at_hat_check(int argc, char **argv)
{
  int fd;
  char data[20];
  ssize_t nr;
 
  // Open file
  fd = open ("/proc/device-tree/hat/product", O_RDONLY);

  if (fd == -1) 
  {
    printf("HAT_MISSING\n");
    return -1;
  }
  
  //Read file
  nr =  read (fd, data, sizeof (data));
  if(nr == -1)
  {
    printf("HAT_MISSING\n");
    return -1;
  }

  printf("HAT_PRESENT = %s\n", data);

  close(fd);
  return 0;
}




int main(int argc, char **argv)
{
  if(argv[1] == NULL)
  {
      printf("Invalid input\n");
      printf("Error\n");
      return -1;
  }
  else if(!(strcmp(argv[1],"AT")))
  {
    printf("OK\n");
  }

  else if(!(strcmp(argv[1],"ATI2")))
  {
    int fd;
    char data[40];
    ssize_t nr;
 
    // Open file
    fd = open("/proc/device-tree/hat/uuid", O_RDONLY);

    if (fd == -1) 
    {
      printf("No UUID information found.\n");
      return -1;
    }
    else
    {
      //Read file
      nr = read(fd, data, sizeof (data));
      if (nr == -1)
      {
        printf("UUID data error.\n");
        return -1;
      }
      else
      {
        printf("%s\n", data);
        printf("OK\n");
        return 0;
      }      
    }
  }

  else if(!(strcmp(argv[1],"ATI3")))
  {
    int fd;
    char data[20];
    ssize_t nr;
 
    // Open file
    fd = open("/proc/device-tree/hat/vendor", O_RDONLY);

    if (fd == -1) 
    {
      printf("No vendor information found.\n");
      return -1;
    }
    else
    {
      //Read file
      nr = read(fd, data, sizeof (data));
      if (nr == -1)
      {
        printf("Vendor data error.\n");
        return -1;
      }
      else
      {
        printf("%s\n", data);
        printf("OK\n");
        return 0;
      }      
    }
  

    // int status;
    // status = accesshat_eeprom_read_string((__u16)0x002A, vendor_name, 11);
    // if(status == -1)
    // {
    //   printf("Error Reading EEPROM\n");
    //   return -1;
    // }
  } 
  else if(!(strcmp(argv[1],"ATI4")))
  {
    int fd;
    char data[20];
    ssize_t nr;
 
    // Open file
    fd = open("/proc/device-tree/hat/product", O_RDONLY);

    if (fd == -1) 
    {
      printf("No product information found.\n");
      return -1;
    }
    else
    {
      //Read file
      nr = read(fd, data, sizeof (data));
      if (nr == -1)
      {
        printf("product data error.\n");
        return -1;
      }
      else
      {
        printf("%s\n", data);
        printf("OK\n");
        return 0;
      }      
    }    
    // int status;
    // status = accesshat_eeprom_read_string((__u16)0x0035, prod_name, 9);
    // if(status == -1)
    // {
    //   printf("Error Reading EEPROM\n");
    //   return -1;
    // }
    // printf("%s\n",prod_name);
    // printf("OK\n");
    // return 0;
  }
  else if(!(strcmp(argv[1],"ATI5")))
  {
    int fd;
    char data[20];
    ssize_t nr;
 
    // Open file
    fd = open("/proc/device-tree/hat/product_ver", O_RDONLY);

    if (fd == -1) 
    {
      printf("No version information found.\n");
      return -1;
    }
    else
    {
      //Read file
      nr = read(fd, data, sizeof (data));
      if (nr == -1)
      {
        printf("Version data error.\n");
        return -1;
      }
      else
      {
        //printf("%s\n", data);
        printf("%cv%c\n",data[4],data[5]);
        printf("OK\n");
        return 0;
      }      
    }

    // int status;
    // status = accesshat_eeprom_read_string((__u16)0x0080, version, 6);
    // if(status == -1)
    // {
    //   printf("Error Reading EEPROM\n");
    //   return -1;
    // }
    // printf("%s\n",version);
    // printf("OK\n");
    // return 0;
  }    
  else if(!(strncmp(argv[1],"AT#CONFIG=gpios|",16)))
  {
    at_gpio_conf(argc,argv);
  }
  else if(!(strncmp(argv[1],"AT#GPIO?",8)))
  {
    at_gpio_read(argc, argv);
  }
  else if(!(strcmp(argv[1],"AT#TEMP?")))
  {
    at_get_temp();
  }
  else if(!(strncmp(argv[1],"AT#EEPROM?",10)))
  {
    at_eeprom_read(argc,argv);
  }
  else if(!(strncmp(argv[1],"AT#EEPROM=",10)))
  {
    at_eeprom_write(argc,argv);
  }
  else if(!(strncmp(argv[1],"AT#RTC=date|",12)))
  {
    at_rtc_set_date(argc,argv);
  }
  else if(!(strcmp(argv[1],"AT#RTC?date")))
  {
    at_rtc_get_date(argc,argv);
  }
  else if(!(strncmp(argv[1],"AT#RTC=time|",12)))
  {
    at_rtc_set_time(argc,argv);
  }
  else if(!(strcmp(argv[1],"AT#RTC?time")))
  {
    at_rtc_get_time(argc,argv);
  }
  else if(!(strcmp(argv[1],"AT#HWCHK?")))
  {
    at_hwchk(argc,argv);
  }
  else if(!(strcmp(argv[1],"AT#CELLCM?")))
  {
    at_cell_cm(argc,argv);
  }
  else if (!(strcmp(argv[1],"AT#LORA?")))
  {
    at_lora(argc, argv);
  }
  else if(!(strncmp(argv[1],"AT#GPIO=",8)))
  {
    at_gpio_write(argc,argv);
  }
  else if(!(strncmp(argv[1],"AT#HATCHK?",10)))
  {
    at_hat_check(argc,argv);
  }
  else
  {
    printf("Invalid Input\n");
    printf("Error\n");
    return -1;
  }        
}

