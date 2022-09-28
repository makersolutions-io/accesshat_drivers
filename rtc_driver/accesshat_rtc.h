/**
  *****************************************************************************************
  *@file    : accesshat_rtc.c
  *@Brief   : Header file for Driving AccessHAT RTC (MCP7940N) 
  
  *****************************************************************************************
*/


#ifndef ACCESSHAT_RTC_H
#define ACCESSHAT_RTC_H

#include <stdint.h>
#include <stdbool.h>


/* Time format typedef*/
typedef enum {
              TIME_12H,
              TIME_24H
             } rtc_time_format_typedef;


/* AM/PM indicator typedef */
typedef enum {
              AM,
              PM
             } rtc_am_pm_typedef;


/* Alarm typedef  */
typedef enum {
              ALARM0,
              ALARM1
             } rtc_alarm_typedef;


/*Alarm mask */
typedef enum {
              SEC = 0x00,  //Second match
              MIN = 0x01,  //Minutes match
              HOUR = 0x02, // Hours match
              WKDAY = 0x03, // Day of week match
              DATE = 0x04, // Date match
              ALL = 0x07// Second,minute,hour,date,day of week match
            } rtc_alarm_mask_typedef;



/**
 *@brief    Set the RTC Time
 *@param    sec : seconds, min : minute, hour : hours, format : 24hr/12hr, ampm : AM/PM
 *@retval   0 : On Success
           -1 : On Error  
 */
int rtc_set_time(uint8_t sec,uint8_t min,uint8_t hour,rtc_time_format_typedef format, rtc_am_pm_typedef ampm);



/**
 *@brief    Get the RTC Time
 *@param    *val : pointer to data array
 *@retval   0 : On Success
           -1 : On Error  
 */
int rtc_get_time(uint8_t* val);



/**
 *@brief    Set the RTC date
 *@param    sec : seconds, min : minute, hour : hours, format : 24hr/12hr, ampm : AM/PM
 *@retval   0 : On Success
           -1 : On Error  
 */
int rtc_set_date(uint8_t day,uint8_t month, uint8_t year,uint8_t weekday);



/**
 *@brief    Get the RTC date
 *@param    *val : pointer to data array
 *@retval   0 : On Success
           -1 : On Error  
 */
int rtc_get_date(uint8_t* val);



/**
 *@brief    Clear alarm interrupt flag
 *@param    alarm : ALARM0 or ALARM1
 *@retval   0 : On Success
           -1 : On Error  
 */
int rtc_clear_alarm_flag(rtc_alarm_typedef alarm);


/**
 *@brief    Clear alarm interrupt flag
 *@param    alarm : ALARM0 or ALARM1
 *@retval   0/1 : On Success
           -1 : On Error  
 */
bool rtc_get_alarm_flag(rtc_alarm_typedef alarm);


/**
 *@brief    Set Alarm (rtc_set_time() abd rtc_set_date() function must be called before setting alarm)
 *@param    sec : seconds, min : minute, hour : hours, day: day of month, month, weekday: day of week
 *@retval   0 : On Success
           -1 : On Error  
 */
int rtc_set_alarm(uint8_t hour,uint8_t min,uint8_t sec, uint8_t month,uint8_t day,uint8_t weekday, rtc_alarm_typedef alarm, rtc_alarm_mask_typedef mask);


/**
 *@brief    RTC Set alarm interrupt
 *@param    function: function pointer to handler
 *@retval   0 : On Success
           -1 : On Error  
 */
int rtc_set_alarm_interrupt(void(*function)(void));



#endif
