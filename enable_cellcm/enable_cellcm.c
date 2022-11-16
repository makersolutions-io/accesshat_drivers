#include <stdio.h>
#include <string.h>  
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>
#include <unistd.h>
#include <wiringPiI2C.h> 
#include <wiringSerial.h>
#include <accesshat_gpio.h>
#include <accesshat_relay.h>
#include <accesshat_eeprom.h>
#include <accesshat_rtc.h>
#include <accesshat_inertial_module.h>


int main(void)
{

  int fd;

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
  
   printf("Set RST_CM pin HIGH = OK \n");
  close(fd);

}

