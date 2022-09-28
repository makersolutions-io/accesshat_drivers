/**
  *****************************************************************************************
  *@file    : accesshat_rtc.c
  *@Brief   : Source file for Driving AccessHAT RTC (real time clock) 
  
  *****************************************************************************************
*/

#include <stdio.h>
#include <wiringPiI2C.h> 
#include <wiringPi.h>
#include "accesshat_rtc.h"
#include <unistd.h>


/* MCP7940N I2C Device ID */
#define MCP7940N_DEVICE_ID                    0x6F


/* Timekeeping Register address */

#define MCP7940N_RTCSEC_ADDR                  0x00
#define MCP7940N_RTCMIN_ADDR                  0x01
#define MCP7940N_RTCHOUR_ADDR                 0x02
#define MCP7940N_RTCWKDAY_ADDR                0x03
#define MCP7940N_RTCDATE_ADDR                 0x04
#define MCP7940N_RTCMTH_ADDR                  0x05
#define MCP7940N_RTCYEAR_ADDR                 0x06
#define MCP7940N_CONTROL_ADDR                 0x07
#define MCP7940N_OSCTRIM_ADDR                 0x08


/* Alarms Register address */

#define MCP7940N_ALM0SEC_ADDR                 0x0A
#define MCP7940N_ALM0MIN_ADDR                 0x0B
#define MCP7940N_ALM0HOUR_ADDR                0x0C
#define MCP7940N_ALM0WKDAY_ADDR               0x0D
#define MCP7940N_ALM0DATE_ADDR                0x0E
#define MCP7940N_ALM0MTH_ADDR                 0x0F

#define MCP7940N_ALM1SEC_ADDR                 0x11
#define MCP7940N_ALM1MIN_ADDR                 0x12
#define MCP7940N_ALM1HOUR_ADDR                0x13
#define MCP7940N_ALM1WKDAY_ADDR               0x14
#define MCP7940N_ALM1DATE_ADDR                0x15
#define MCP7940N_ALM1MTH_ADDR                 0x16

/* Power fail Time-Stamp */
#define MCP7940N_PWRDNMIN_ADDR                0x18
#define MCP7940N_PWRDNHOUR_ADDR               0x19
#define MCP7940N_PWRDNDATE_ADDR               0x1A
#define MCP7940N_PWRDNMTH_ADDR                0x1B

#define MCP7940N_PWRUPMIN_ADDR                0x1C
#define MCP7940N_PWRUPHOUR_ADDR               0x1D
#define MCP7940N_PWRUPDATE_ADDR               0x1E
#define MCP7940N_PWRUPMTH_ADDR                0x1F





/**
 *@brief    Check the I2C communication status
 *@param    i2c_device_id 
 *@retval   fd : returns i2c setup status 
 */
static int check_i2c_setup(uint8_t i2c_device_id)
{
  int fd = wiringPiI2CSetup(i2c_device_id);
  if(fd == -1)
  {
    printf("Failed to init I2C communication.\n");
    return -1;
  }

  //printf("I2C communication successfully setup.\n");
  return fd;
}





/**
 *@brief    Clear ST and EXTOSC bit to avoid roll over.
 *@param    none
 *@retval   0 : On Success
           -1 : On Error  
 */
static int rtc_clear_st_extosc_bit(void)
{
  int fd;
  
  uint8_t data_st, data_extosc; 

  fd = check_i2c_setup(MCP7940N_DEVICE_ID);
  if(fd == -1)
  {
    printf("rtc_clear_st_extosc_bit : error \n");
    return -1;
  }

  /* Clear ST bit */
  data_st = wiringPiI2CReadReg8(fd,MCP7940N_RTCSEC_ADDR);
  data_st = (data_st & 0x7F);
  wiringPiI2CWriteReg8(fd,MCP7940N_RTCSEC_ADDR,data_st);

  /* Clear EXTOSC bit */
  data_extosc = wiringPiI2CReadReg8(fd,MCP7940N_CONTROL_ADDR);
  data_extosc = (data_extosc & 0xF7);
  wiringPiI2CWriteReg8(fd,MCP7940N_CONTROL_ADDR,data_extosc);

  close(fd);

  return 0;
}





/**
 *@brief    Enable ST and EXTOS bit
 *@param    none
 *@retval   0 : On Success
           -1 : On Error  
 */
static int rtc_enable_st_extosc_bit(void)
{
  int fd;
  
  uint8_t data_st, data_extosc; 

  fd = check_i2c_setup(MCP7940N_DEVICE_ID);
  if(fd == -1)
  {
    printf("rtc_enable_st_extosc_bit: error\n");
    return -1;
  }

  
  /* Set EXTOSC bit */
  data_extosc = wiringPiI2CReadReg8(fd,MCP7940N_CONTROL_ADDR);
  data_extosc = (data_extosc | 0x00);
  wiringPiI2CWriteReg8(fd,MCP7940N_CONTROL_ADDR,data_extosc);
  
  /* Set ST bit */
  data_st = wiringPiI2CReadReg8(fd,MCP7940N_RTCSEC_ADDR);
  data_st = (data_st | 0x80);
  wiringPiI2CWriteReg8(fd,MCP7940N_RTCSEC_ADDR,data_st);

  close(fd);

  return 0;
}





/**
 *@brief    Get the status of OSCRUN bit.
 *@param    none
 *@retval   0 : On Success
           -1 : On Error  
 */
static bool rtc_get_oscrun_bit(void)
{
  int fd;
  
  uint8_t read_data;

  fd = check_i2c_setup(MCP7940N_DEVICE_ID);
  if(fd == -1)
  {
    printf("rtc_get_oscrun_bit: error \n");
    return -1;
  }
  
  read_data = wiringPiI2CReadReg8(fd,MCP7940N_RTCWKDAY_ADDR);
  
  close(fd);
  return (read_data & 0x20);
}





/**
 *@brief    Set the RTC Time
 *@param    sec : seconds, min : minute, hour : hours, format : 24hr/12hr, ampm : AM/PM
 *@retval   0 : On Success
           -1 : On Error  
 */
int rtc_set_time(uint8_t sec,uint8_t min,uint8_t hour,rtc_time_format_typedef format, rtc_am_pm_typedef ampm)
{
  int fd;

  /* Input range check */
  if((sec < 0x00) || (sec > 0x59))
  {
    printf("rtc_set_time sec: invalid input\n");
    return -1;
  }


  if((min < 0x00) || (min > 0x59))
  {
    printf("rtc_set_time min: invalid input\n");
    return -1;
  }
  
  if(format == TIME_12H)
  {
    if((hour < 0x00) || (hour > 0x12))
    {
      printf("rtc_set_time hour: invalid input\n");
      return -1;
    }
  }
  else if(format == TIME_24H)
  {
    if((hour < 0x00) || (hour > 0x23))
    {
      printf("rtc_set_time hour: invalid input\n");
      return -1;
    }    
  }

  
  /* check i2c communication*/
  fd = check_i2c_setup(MCP7940N_DEVICE_ID);
  if(fd == -1)
  {
    printf("rct_set_sec: i2c error \n");
    return -1;
  }


  /* Clear ST and EXTOSC bit to avoid roll over issue */
  rtc_clear_st_extosc_bit();

  /* wait for OSCRUN bit to clear */
  while(rtc_get_oscrun_bit());


  /*Update the second*/
  sec = (sec & 0x7F);
  wiringPiI2CWriteReg8(fd,MCP7940N_RTCSEC_ADDR,sec);

  /*Update the minute */
  wiringPiI2CWriteReg8(fd,MCP7940N_RTCMIN_ADDR,min);

  if(format == TIME_24H)
  {
    hour = (hour & 0x3F);
    wiringPiI2CWriteReg8(fd,MCP7940N_RTCHOUR_ADDR,hour);
  }
  else if (format == TIME_12H)
  {
    hour = (hour & 0x1F);
    hour = (hour | 0x40);

    if(ampm == AM)
    {
      hour = (hour & 0x5F);
    }
    else if(ampm == PM)
    {
      hour = (hour | 0x20);
    }
    
    wiringPiI2CWriteReg8(fd,MCP7940N_RTCHOUR_ADDR,hour);
  }
 
  /*Set VBATEN bit to enable backup power*/
  uint8_t read_data = wiringPiI2CReadReg8(fd,MCP7940N_RTCWKDAY_ADDR);
  read_data = read_data | 0x08;
  wiringPiI2CWriteReg8(fd,MCP7940N_RTCWKDAY_ADDR,read_data);

  /*Enable ST and EXTOSC */
  rtc_enable_st_extosc_bit();

  /* wait for osc to run */
  while(!(rtc_get_oscrun_bit()));

  close(fd);

  return 0;
}





/**
 *@brief    Get the RTC Time
 *@param    *val : pointer to data array
 *@retval   0 : On Success
           -1 : On Error  
 */
int rtc_get_time(uint8_t* val)
{
  int fd;

  uint8_t temp, format;
  /* check i2c communication*/
  fd = check_i2c_setup(MCP7940N_DEVICE_ID);
  if(fd == -1)
  {
    printf("rtc_get_time: i2c error \n");
    return -1;
  }

  /* Read Seconds */
  temp = wiringPiI2CReadReg8(fd,MCP7940N_RTCSEC_ADDR);
  val[0] = (temp & 0x7F);

  /* Read Minute */
  val[1] = wiringPiI2CReadReg8(fd,MCP7940N_RTCMIN_ADDR);

  /* Read hours */
  temp = wiringPiI2CReadReg8(fd,MCP7940N_RTCHOUR_ADDR);


  /*Get format details */
  format = (temp & 0x40);
  val[3] = format;

  if(format == 0x40)
  {
    /* 12H format */
    val[2] = (temp & 0x1F);
    val[4] = (temp & 0x20);
  }
  else
  {
    /*24H format */
    val[2] = (temp & 0x3F);
  }


  close(fd);


  return 0;

}





/**
 *@brief    Set the RTC date
 *@param    day: day of month, month, year, weekday: day of week
 *@retval   0 : On Success
           -1 : On Error  
 */
int rtc_set_date(uint8_t day,uint8_t month, uint8_t year,uint8_t weekday)
{
  int fd;

  uint8_t read_data;
  

  /* Input range check */
  if((day < 0x00) || (day > 0x31))
  {
    printf("rtc_set_date day: invalid input\n");
    return -1;
  }


  if((month < 0x00) || (month > 0x12))
  {
    printf("rtc_set_date month: invalid input\n");
    return -1;
  }
  
  if((year < 0x00) || (year > 0x99))
  {
    printf("rtc_set_date year: invalid input\n");
    return -1;
  }


  if((weekday < 0x00)||(weekday > 0x07))
  {
    printf("rtc_set_date weekday: invalid input\n");
    return -1;
  }

  
  /* check i2c communication*/
  fd = check_i2c_setup(MCP7940N_DEVICE_ID);
  if(fd == -1)
  {
    printf("rct_set_sec: i2c error \n");
    return -1;
  }


  /* Clear ST and EXTOSC bit to avoid roll over issue */
  rtc_clear_st_extosc_bit();

  /* wait for OSCRUN bit to clear */
  while(rtc_get_oscrun_bit());
 
   /*Enable VBATEN bit to enable backup power*/
  read_data = wiringPiI2CReadReg8(fd,MCP7940N_RTCWKDAY_ADDR);
  read_data = read_data | 0x08;
  wiringPiI2CWriteReg8(fd,MCP7940N_RTCWKDAY_ADDR,read_data);


  /*Update the weekday (make sure only last 3 bits of RTCWKDAY reg is changed)*/
  read_data = wiringPiI2CReadReg8(fd,MCP7940N_RTCWKDAY_ADDR);
  read_data = (read_data & 0xF8);
  weekday = (weekday & 0x07);
  weekday = (weekday | read_data);
  wiringPiI2CWriteReg8(fd,MCP7940N_RTCWKDAY_ADDR,weekday);


  /*Update the day*/
  wiringPiI2CWriteReg8(fd,MCP7940N_RTCDATE_ADDR,day);

  /*Update the month */
  wiringPiI2CWriteReg8(fd,MCP7940N_RTCMTH_ADDR,month);

  /*Update the Year */
  wiringPiI2CWriteReg8(fd,MCP7940N_RTCYEAR_ADDR,year);


  /*Enable ST and EXTOSC */
  rtc_enable_st_extosc_bit();

  close(fd);

  return 0;
}





/**
 *@brief    Get the RTC date
 *@param    *val : pointer to data array
 *@retval   0 : On Success
           -1 : On Error  
 */
int rtc_get_date(uint8_t* val)
{
  int fd;

  uint8_t temp;

  /* check i2c communication*/
  fd = check_i2c_setup(MCP7940N_DEVICE_ID);
  if(fd == -1)
  {
    printf("rtc_get_date: i2c error \n");
    return -1;
  }

  /* Read day */
  val[0] = wiringPiI2CReadReg8(fd,MCP7940N_RTCDATE_ADDR);

  /* Read Month */
  val[1] = wiringPiI2CReadReg8(fd,MCP7940N_RTCMTH_ADDR);

  /* Read Year */
  val[2] = wiringPiI2CReadReg8(fd,MCP7940N_RTCYEAR_ADDR);
  
  /* Read Weekday */
  temp = wiringPiI2CReadReg8(fd,MCP7940N_RTCWKDAY_ADDR);
  val[3] = (temp & 0x07);

  close(fd);

  return 0;

}





/**
 *@brief    Clear alarm interrupt flag
 *@param    alarm : ALARM0 or ALARM1
 *@retval   0 : On Success
           -1 : On Error  
 */
int rtc_clear_alarm_flag(rtc_alarm_typedef alarm)
{
  int fd;
  uint8_t read_data;

  /* check i2c communication*/
  fd = check_i2c_setup(MCP7940N_DEVICE_ID);
  if(fd == -1)
  {
    printf("rtc_clear_alarm_flag: i2c error \n");
    return -1;
  }

  if(alarm == ALARM0)
  {
    read_data = wiringPiI2CReadReg8(fd,MCP7940N_ALM0WKDAY_ADDR);
    read_data = (read_data & 0xF7);
    wiringPiI2CWriteReg8(fd,MCP7940N_ALM0WKDAY_ADDR,read_data);
  }
  else if (alarm == ALARM1)
  {
    read_data = wiringPiI2CReadReg8(fd,MCP7940N_ALM1WKDAY_ADDR);
    read_data = (read_data & 0xF7);
    wiringPiI2CWriteReg8(fd,MCP7940N_ALM1WKDAY_ADDR,read_data);   
  }

  close(fd);

  return 0;
}





/**
 *@brief    Get the Alarm Interrupt flag bit
 *@param    alarm : ALARM0 or ALARM1
 *@retval   0/1 : On Success
           -1 : On Error  
 */
bool rtc_get_alarm_flag(rtc_alarm_typedef alarm)
{
  int fd;
  uint8_t read_data;

  /* check i2c communication*/
  fd = check_i2c_setup(MCP7940N_DEVICE_ID);
  if(fd == -1)
  {
    printf("rtc_clear_alarm_flag: i2c error \n");
    return -1;
  }

  if(alarm == ALARM0)
  {
    read_data = wiringPiI2CReadReg8(fd,MCP7940N_ALM0WKDAY_ADDR);

  }
  else if (alarm == ALARM1)
  {
    read_data = wiringPiI2CReadReg8(fd,MCP7940N_ALM1WKDAY_ADDR);
  }

  else
  {
    printf("Error in rtc_get_alarm_flag.\n");
    close(fd);
    return -2;
  }

  close(fd);
  return(read_data & 0x08);
  
}





/**
 *@brief    Set Alarm (rtc_set_time() abd rtc_set_date() function must be called before setting alarm)
 *@param    sec : seconds, min : minute, hour : hours, day: day of month, month, weekday: day of week
 *@retval   0 : On Success
           -1 : On Error  
 */
int rtc_set_alarm(uint8_t hour,uint8_t min,uint8_t sec, uint8_t month,uint8_t day,uint8_t weekday, rtc_alarm_typedef alarm, rtc_alarm_mask_typedef mask)
{
  int fd;
  uint8_t read_data, format, ampm;

  /* Input range check */
  if((sec < 0x00) || (sec > 0x59))
  {
    printf("rtc_set_alarm sec: invalid input\n");
    return -1;
  }


  if((min < 0x00) || (min > 0x59))
  {
    printf("rtc_set_alarm min: invalid input\n");
    return -1;
  }
  
  if((hour < 0x00) || (hour > 0x23))
  {
    printf("rtc_set_alarm hour: invalid input\n");
    return -1;
  } 


  if((day < 0x00) || (day > 0x31))
  {
    printf("rtc_set_alarm day: invalid input\n");
    return -1;
  }


  if((month < 0x00) || (month > 0x12))
  {
    printf("rtc_set_alarm month: invalid input\n");
    return -1;
  }

  if((weekday < 0x00)||(weekday > 0x07))
  {
    printf("rtc_set_alarm weekday: invalid input\n");
    return -1;
  }   
  

  /* check i2c communication*/
  fd = check_i2c_setup(MCP7940N_DEVICE_ID);
  if(fd == -1)
  {
    printf("rtc_set_alarm: i2c error \n");
    return -1;
  }

  if(alarm == ALARM0)
  {
    /* Configure ALM0MASK  */
    mask = (mask << 4);
    read_data = wiringPiI2CReadReg8(fd,MCP7940N_ALM0WKDAY_ADDR);
    read_data = (read_data & 0x8F);
    mask = (mask | read_data);
    wiringPiI2CWriteReg8(fd,MCP7940N_ALM0WKDAY_ADDR,mask);

    /* Set or clear ALMPOL ( Setting HIGH here ) */
    read_data = wiringPiI2CReadReg8(fd,MCP7940N_ALM0WKDAY_ADDR);
    read_data = (read_data | 0x80);
    wiringPiI2CWriteReg8(fd,MCP7940N_ALM0WKDAY_ADDR,read_data);

    /*Clear ALM0IF flag*/
    rtc_clear_alarm_flag(alarm);

    /*Load Second Value*/
    wiringPiI2CWriteReg8(fd,MCP7940N_ALM0SEC_ADDR,sec);

    /*Load Minute Value*/
    wiringPiI2CWriteReg8(fd,MCP7940N_ALM0MIN_ADDR,min);

    /*Load hour Value*/
    read_data = wiringPiI2CReadReg8(fd,MCP7940N_RTCHOUR_ADDR);
    format = (read_data & 0x40);
    ampm = (read_data & 0x20);

    if(format == 0x40)
    {
      /*12H format*/
      if(ampm == 0x00)
      {
        /* its AM */
        hour = (hour | 0x40);
        hour = (hour & 0xDF);
        wiringPiI2CWriteReg8(fd,MCP7940N_ALM0HOUR_ADDR,hour);
      }
      else if(ampm == 0x20)
      {
        /*its PM*/
        hour = (hour | 0x40);
        hour = (hour | 0x20);
        wiringPiI2CWriteReg8(fd,MCP7940N_ALM0HOUR_ADDR,hour);
      }
    }
    else if(format == 0x00)
    {
      /*24H format */
      hour = (hour &0x3F);
      wiringPiI2CWriteReg8(fd,MCP7940N_ALM0HOUR_ADDR,hour);

    }   

    /*Load day Value*/
    wiringPiI2CWriteReg8(fd,MCP7940N_ALM0DATE_ADDR,day);

    /*Load month value*/
    wiringPiI2CWriteReg8(fd,MCP7940N_ALM0MTH_ADDR,month);


    /*Load day of the week*/
    read_data = wiringPiI2CReadReg8(fd,MCP7940N_ALM0WKDAY_ADDR);
    weekday = (weekday & 0x07);
    read_data = (read_data & 0xF8);
    read_data = (read_data | weekday);
    wiringPiI2CWriteReg8(fd,MCP7940N_ALM0WKDAY_ADDR,read_data);

    /*Enable Alarm0 module*/
    read_data = wiringPiI2CReadReg8(fd,MCP7940N_CONTROL_ADDR);
    read_data = (read_data|0x10);
    wiringPiI2CWriteReg8(fd,MCP7940N_CONTROL_ADDR,read_data);
  }
  else if (alarm == ALARM1)
  {
    /* Configure ALM1MASK  */
    mask = (mask << 4);
    read_data = wiringPiI2CReadReg8(fd,MCP7940N_ALM1WKDAY_ADDR);
    read_data = (read_data & 0x8F);
    mask = (mask | read_data);
    wiringPiI2CWriteReg8(fd,MCP7940N_ALM1WKDAY_ADDR,mask);

    /* Set or clear ALMPOL ( Setting HIHG here ) */
    read_data = wiringPiI2CReadReg8(fd,MCP7940N_ALM1WKDAY_ADDR);
    read_data = (read_data | 0x80);
    wiringPiI2CWriteReg8(fd,MCP7940N_ALM1WKDAY_ADDR,read_data);

    /*Clear ALM0IF flag*/
    rtc_clear_alarm_flag(alarm);

    /*Load Second Value*/
    wiringPiI2CWriteReg8(fd,MCP7940N_ALM1SEC_ADDR,sec);

    /*Load Minute Value*/
    wiringPiI2CWriteReg8(fd,MCP7940N_ALM1MIN_ADDR,min);

    /*Load hour Value*/
    read_data = wiringPiI2CReadReg8(fd,MCP7940N_RTCHOUR_ADDR);
    format = (read_data & 0x40);
    ampm = (read_data & 0x20);

    if(format == 0x40)
    {
      /*12H format*/
      if(ampm == 0x00)
      {
        /* its AM */
        hour = (hour | 0x40);
        hour = (hour & 0xDF);
        wiringPiI2CWriteReg8(fd,MCP7940N_ALM1HOUR_ADDR,hour);
      }
      else if(ampm == 0x20)
      {
        /*its PM*/
        hour = (hour | 0x40);
        hour = (hour | 0x20);
        wiringPiI2CWriteReg8(fd,MCP7940N_ALM1HOUR_ADDR,hour);
      }
    }
    else if(format == 0x00)
    {
      /*24H format */
      hour = (hour & 0x3F);
      wiringPiI2CWriteReg8(fd,MCP7940N_ALM1HOUR_ADDR,hour);

    }   

    /*Load day Value*/
    wiringPiI2CWriteReg8(fd,MCP7940N_ALM1DATE_ADDR,day);

    /*Load month value*/
    wiringPiI2CWriteReg8(fd,MCP7940N_ALM1MTH_ADDR,month);


    /*Load day of the week*/
    read_data = wiringPiI2CReadReg8(fd,MCP7940N_ALM1WKDAY_ADDR);
    weekday = (weekday & 0x07);
    read_data = (read_data & 0xF8);
    read_data = (read_data | weekday);
    wiringPiI2CWriteReg8(fd,MCP7940N_ALM1WKDAY_ADDR,read_data);

    /*Enable Alarm1 module*/
    read_data = wiringPiI2CReadReg8(fd,MCP7940N_CONTROL_ADDR);
    read_data = (read_data|0x20);
    wiringPiI2CWriteReg8(fd,MCP7940N_CONTROL_ADDR,read_data);
  }

  close(fd);

  return 0;
}





/**
 *@brief    RTC Set alarm interrupt
 *@param    function: function pointer to handler
 *@retval   0 : On Success
           -1 : On Error  
 */
int rtc_set_alarm_interrupt(void(*function)(void))
{
  int fd, status;

  /* Initializes wiringPi */
  wiringPiSetup(); 

  fd = check_i2c_setup(MCP7940N_DEVICE_ID);
  if(fd == -1)
  {
    printf("rtc_set_alarm_interrupt: error\n");
    return -1;
  }
   
   /*GPIO17 is Pin 0 in wiring Pi*/
   status = wiringPiISR(0, INT_EDGE_RISING, function); 

   close(fd);

   return status;
}

