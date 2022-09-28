/**
  *****************************************************************************************
  *@file    : accesshat_inertial_module.c
  *@Brief   : Source file for Driving AccessHAT LSM6DS33 inertial module. 
  
  *****************************************************************************************
*/

#include <stdio.h>
#include <wiringPiI2C.h> 
#include <wiringPi.h>
#include "accesshat_inertial_module.h"
#include <unistd.h>



/* Register Addresses */
#define LSM6DS33_FUNC_CFG_ACCESS_ADDR           0x01
#define LSM6DS33_FIFO_CTRL1_ADDR                0x06
#define LSM6DS33_FIFO_CTRL2_ADDR                0x07 
#define LSM6DS33_FIFO_CTRL3_ADDR                0x08 
#define LSM6DS33_FIFO_CTRL4_ADDR                0x09 
#define LSM6DS33_FIFO_CTRL5_ADDR                0x0A 
#define LSM6DS33_ORIENT_CFG_G_ADDR              0x0B 
#define LSM6DS33_INT1_CTRL_ADDR                 0x0D
#define LSM6DS33_INT2_CTRL_ADDR                 0x0E
#define LSM6DS33_WHO_AM_I_ADDR                  0x0F 
#define LSM6DS33_CTRL1_XL_ADDR                  0x10 
#define LSM6DS33_CTRL2_G_ADDR                   0x11 
#define LSM6DS33_CTRL3_C_ADDR                   0x12 
#define LSM6DS33_CTRL4_C_ADDR                   0x13 
#define LSM6DS33_CTRL5_C_ADDR                   0x14 
#define LSM6DS33_CTRL6_C_ADDR                   0x15 
#define LSM6DS33_CTRL7_G_ADDR                   0x16 
#define LSM6DS33_CTRL8_XL_ADDR                  0x17 
#define LSM6DS33_CTRL9_XL_ADDR                  0x18 
#define LSM6DS33_CTRL10_C_ADDR                  0x19
#define LSM6DS33_WAKE_UP_SRC_ADDR               0x1B
#define LSM6DS33_TAP_SRC_ADDR                   0x1C 
#define LSM6DS33_D6D_SRC_ADDR                   0x1D 
#define LSM6DS33_STATUS_REG_ADDR                0x1E 
#define LSM6DS33_OUT_TEMP_L_ADDR                0x20
#define LSM6DS33_OUT_TEMP_H_ADDR                0x21 
#define LSM6DS33_OUTX_L_G_ADDR                  0x22 
#define LSM6DS33_OUTX_H_G_ADDR                  0x23 
#define LSM6DS33_OUTY_L_G_ADDR                  0x24 
#define LSM6DS33_OUTY_H_G_ADDR                  0x25 
#define LSM6DS33_OUTZ_L_G_ADDR                  0x26 
#define LSM6DS33_OUTZ_H_G_ADDR                  0x27 
#define LSM6DS33_OUTX_L_XL_ADDR                 0x28 
#define LSM6DS33_OUTX_H_XL_ADDR                 0x29 
#define LSM6DS33_OUTY_L_XL_ADDR                 0x2A 
#define LSM6DS33_OUTY_H_XL_ADDR                 0x2B 
#define LSM6DS33_OUTZ_L_XL_ADDR                 0x2C 
#define LSM6DS33_OUTZ_H_XL_ADDR                 0x2D
#define LSM6DS33_FIFO_STATUS1_ADDR              0x3A 
#define LSM6DS33_FIFO_STATUS2_ADDR              0x3B 
#define LSM6DS33_FIFO_STATUS3_ADDR              0x3C 
#define LSM6DS33_FIFO_STATUS4_ADDR              0x3D 
#define LSM6DS33_FIFO_DATA_OUT_L_ADDR           0x3E 
#define LSM6DS33_FIFO_DATA_OUT_H_ADDR           0x3F 
#define LSM6DS33_TIMESTAMP0_REG_ADDR            0x40 
#define LSM6DS33_TIMESTAMP1_REG_ADDR            0x41 
#define LSM6DS33_TIMESTAMP2_REG_ADDR            0x42 
#define LSM6DS33_STEP_TIMESTAMP_L_ADDR          0x49 
#define LSM6DS33_STEP_TIMESTAMP_H_ADDR          0x4A 
#define LSM6DS33_STEP_COUNTER_L_ADDR            0x4B 
#define LSM6DS33_STEP_COUNTER_H_ADDR            0x4C
#define LSM6DS33_FUNC_SRC_ADDR                  0x53 
#define LSM6DS33_TAP_CFG_ADDR                   0x58 
#define LSM6DS33_TAP_THS_6D_ADDR                0x59 
#define LSM6DS33_INT_DUR2_ADDR                  0x5A
#define LSM6DS33_WAKE_UP_THS_ADDR               0x5B 
#define LSM6DS33_WAKE_UP_DUR_ADDR               0x5C 
#define LSM6DS33_FREE_FALL_ADDR                 0x5D 
#define LSM6DS33_MD1_CFG_ADDR                   0x5E 
#define LSM6DS33_MD2_CFG_ADDR                   0x5F 
#define LSM6DS33_PEDO_THS_REG_ADDR              0x0F 
#define LSM6DS33_SM_THS_ADDR                    0x13 
#define LSM6DS33_PEDO_DEB_REG_ADDR              0x14 
#define LSM6DS33_STEP_COUNT_DELTA_ADDR          0x15 


/* Self test limits. */
#define MIN_ST_LIMIT_mg                         90.0f
#define MAX_ST_LIMIT_mg                         1700.0f
#define MIN_ST_LIMIT_mdps                       150000.0f
#define MAX_ST_LIMIT_mdps                       700000.0f


/* Status Register Mask for New data */
#define STATUS_REG_XLDA_NEW_DATA_MSK            0x01
#define STATUS_REG_GDA_NEW_DATA_MSK             0x02
#define STATUS_REG_TDA_NEW_DATA_MSK             0x03

/* Tap detection */
#define TAP_SRC_REG_SINGLE_TAP_MSK              0x20
#define TAP_SRC_REG_DOUBLE_TAP_MSK              0x10
#define WAKE_UP_THS_SINGLE_TAP_EN_VAL           0x00  // Only Single TAP Enabled
#define WAKE_UP_THS_DOUBLE_TAP_EN_VAL           0x80  // Both Single & Double TAP Enabled


/* Free Fall detection event mask */
#define WAKE_UP_SRC_REG_FREEFALL_MSK            0x20

/* Sleep event bit mask*/
#define WAKE_UP_SRC_REG_SLEEP_EVENT_MSK         0x10


/*Gyroscope Interrupts*/
#define GYROSCOPE_INT1_DRDY_G_EN_VAL            0x02
#define GYROSCOPE_INT2_DRDY_G_EN_VAL            0x02


/*Accelerometer Interrupts*/
#define ACCELEROMETER_INT1_DRDY_XL_EN_VAL      0x01
#define ACCELEROMETER_INT2_DRDY_XL_EN_VAL      0x01 

/* Single Tap Interrupts Enable */
#define SINGLE_TAP_INT1_EN_VAL                0x40 
#define SINGLE_TAP_INT2_EN_VAL                0x40
#define SINGLE_TAP_INT1_LATCH_EN_VAL          0x48 
#define SINGLE_TAP_INT2_LATCH_EN_VAL          0x48

/* Double tap interrupt Enable */
#define DOUBLE_TAP_INT1_EN_VAL                0x08
#define DOUBLE_TAP_INT2_EN_VAL                0x08

/* Free Fall interrupt Enable */
#define FREFALL_INT1_EN_VAL                   0x10
#define FREFALL_INT2_EN_VAL                   0x10

/* Wake up detetection event mask */
#define WAKE_UP_SRC_REG_WU_IA_MSK             0x08

/* Wake up interrupt enable */
#define WAKE_UP_IN1_EN_VAL                    0x20
#define WAKE_UP_IN2_EN_VAL                    0x20

/* Inactivity interrupt enable */
#define INACTIVITY_INT1_EN_VAL                0x80
#define INACTIVITY_INT2_EN_VAL                0x80

/* Significant motion detection event mask*/
#define FUNC_SRC_REG_SIGN_MOTION_IA_MSK       0x40

/*Significant motion interrupt enable */
#define SIGN_MOTION_INT_EN_VAL                0xC0 







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
 *@brief    Wirte to I2C Device 
 *@param    
 *@retval   
 */
static int i2c_write(int fd, uint8_t reg_addr, uint8_t data)
{
  int status;

  /*Send the I2C Command */
  status = wiringPiI2CWriteReg8(fd,reg_addr,data);

  return status;
}


/**
 *@brief    Wirte to I2C Device 
 *@param    
 *@retval   
 */
static int i2c_read_then_write(int fd, uint8_t reg_addr, uint8_t data)
{
  int status;
  uint8_t read_data;
  
  /* Read register before writing */
  read_data = wiringPiI2CReadReg8(fd,reg_addr);
  data = (data | read_data);

  /*Send the I2C Command */
  status = wiringPiI2CWriteReg8(fd,reg_addr,data);

  return status;
}




/**
 *@brief    Read from the I2C Device
 *@param    
 *@retval   
 */
static uint8_t i2c_read(int fd, uint8_t reg_addr)
{
 uint8_t read_data;

 /*Read data from I2C Device*/
 read_data = wiringPiI2CReadReg8(fd,reg_addr);

 return read_data;
}


  


/**
  * @defgroup    LSM6DS3_Sensitivity
  * @brief       These functions convert raw-data into engineering units.
  * 
  *
  */
float_t lsm6ds3_from_fs2g_to_mg(int16_t value)
{
  return ((float_t)value * 61.0f / 1000.0f);
}

float_t lsm6ds3_from_fs4g_to_mg(int16_t value)
{
  return ((float_t)value * 122.0f / 1000.0f);
}

float_t lsm6ds3_from_fs8g_to_mg(int16_t value)
{
  return ((float_t)value * 244.0f / 1000.0f);
}

float_t lsm6ds3_from_fs16g_to_mg(int16_t value)
{
  return ((float_t)value * 488.0f / 1000.0f);
}

float_t lsm6ds3_from_fs125dps_to_mdps(int16_t value)
{
  return ((float_t)value   * 4375.0f / 1000.0f);
}

float_t lsm6ds3_from_fs250dps_to_mdps(int16_t value)
{
  return ((float_t)value  * 8750.0f / 1000.0f);
}

float_t lsm6ds3_from_fs500dps_to_mdps(int16_t value)
{
  return ((float_t)value * 1750.0f / 100.0f);
}

float_t lsm6ds3_from_fs1000dps_to_mdps(int16_t value)
{
  return ((float_t)value * 35.0f);
}

float_t lsm6ds3_from_fs2000dps_to_mdps(int16_t value)
{
  return ((float_t)value * 70.0f);
}

float_t lsm6ds3_from_lsb_to_celsius(int16_t value)
{
  return ((float_t)value / 16.0f + 25.0f);
}





/**
 *@brief    Read the WHO_AM_I register from IMU
 *@param    none
 *@retval   returns 0: on success
                   -1: on error
 */
int read_who_am_i(void)
{
  int ret;
  int fd = wiringPiI2CSetup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("Failed to init I2C communication.\n");
    return -1;
  }
  
  uint8_t read_data = wiringPiI2CReadReg8(fd,LSM6DS33_WHO_AM_I_ADDR);
  if(read_data == 0x69)
  {
    /*WHO_AM_I Register read successfully*/
    ret = 0;
  }
  else
  {
   /*Failed to read WHO_AM_I*/
    ret = -1;
  }
 
  close(fd);
  
  return ret;
}



/**
 *@brief    Set Default Values to Accelerometer Configs
 *@param    *xl_config : pointer to accelerometer config structure
 *@retval   returns 0
 */
int accelerometer_config_set_defaults(accelerometer_config_typedef *xl_conf)
{
  xl_conf->xl_odr = ACCELEROMETER_ODR_12_5_HZ_VAL;
  xl_conf->xl_fs = ACCELEROMETER_FS_2G_VAL;
  xl_conf->xl_mode = ACCELEROMETER_LOW_POWER_MODE_VAL;
  xl_conf->xl_bw = ACCELEROMETER_BW_400_HZ_VAL;
  xl_conf->xl_bdu = LSM6DS33_BDU_ENABLE_VAL;
  xl_conf->xl_axis = ACCELEROMETER_XYZ_AXIS_VAL;

  return 0;
}



/**
 *@brief    Initialize Accelerometer
 *@param    *xl_config : pointer to accelerometer config structure
 *@retval   0 : On Success
           -1 : On Error  
 */
int accelerometer_init(accelerometer_config_typedef *xl_conf)
{
  int fd;

  uint8_t ctrl1_xl_reg_data;

  /* Initializes wiringPi */
  wiringPiSetup(); 

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("init_accelerometer : error \n");
    return -1;
  }

  /* 20ms Delay for boot process */
  delay(20);

  /*Put the Accelerometer in Powerdown mode Initially*/
  i2c_write(fd,LSM6DS33_CTRL1_XL_ADDR,0x00);

  /*Set Accelerometer operating mode*/
  i2c_write(fd,LSM6DS33_CTRL6_C_ADDR,xl_conf->xl_mode);

  /*Set Accelerometer axis */
  i2c_write(fd,LSM6DS33_CTRL9_XL_ADDR,xl_conf->xl_axis);

  /*Gather all data to be written into CTRL1_XL(10h) Register and Write to it*/
  ctrl1_xl_reg_data = (xl_conf->xl_odr | xl_conf->xl_fs | xl_conf->xl_bw);
  i2c_write(fd,LSM6DS33_CTRL1_XL_ADDR,ctrl1_xl_reg_data);

  /*Set Block data update value*/
  i2c_read_then_write(fd,LSM6DS33_CTRL3_C_ADDR,xl_conf->xl_bdu);

  close(fd);

  return 0;
}




/**
 *@brief    Get Raw Accelerometer data
 *@param    *val : pointer to data array 
 *@retval   0 : On Success
           -1 : On Error
 */
int accelerometer_get_raw_data(int16_t *val)
{
  int fd;
  uint8_t x_low,x_high,y_low,y_high,z_low,z_high;


  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("accelerometer_get_raw_data : error \n");
    return -1;
  }


  x_low = i2c_read(fd,LSM6DS33_OUTX_L_XL_ADDR);
  x_high = i2c_read(fd,LSM6DS33_OUTX_H_XL_ADDR);

  y_low = i2c_read(fd,LSM6DS33_OUTY_L_XL_ADDR);
  y_high = i2c_read(fd,LSM6DS33_OUTY_H_XL_ADDR);


  z_low = i2c_read(fd,LSM6DS33_OUTZ_L_XL_ADDR);
  z_high = i2c_read(fd,LSM6DS33_OUTZ_H_XL_ADDR);


  val[0] = (int16_t)x_high;
  val[0] = (val[0] * 256) + (int16_t)x_low;

  val[1] = (int16_t)y_high;
  val[1] = (val[1] * 256) + (int16_t)y_low;

  val[2] = (int16_t)z_high;
  val[2] = (val[2] * 256) + (int16_t)z_low;

  close(fd);

  return 0;
  
}




/**
 *@brief    Get the Accelerometer data ready flag
 *@param    none
 *@retval   0 or 1 : on success
            -1 on error
 */      
int accelerometer_flag_data_ready_get(void)
{
  uint8_t status_reg,fd;

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("accelerometer_flag_data_ready_get: error\n");
    return -1;
  }

  status_reg = i2c_read(fd,LSM6DS33_STATUS_REG_ADDR);

  close(fd);

  return (status_reg & STATUS_REG_XLDA_NEW_DATA_MSK);
}




/**
 *@brief    Set Accelerometer Interrupt
 *@param    pin : Interrupt Pin
            function: function pointer to handler
 *@retval   0 : On Success
           -1 : On Error  
 */
int accelerometer_set_interrupt(interrupt_pin_typedef pin, void(*function)(void))
{
  int fd, status;

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("accelerometer_set_interrupt: error\n");
    return -1;
  }
  
  if(pin == INT1_PIN)
  {
    i2c_read_then_write(fd,LSM6DS33_INT1_CTRL_ADDR,ACCELEROMETER_INT1_DRDY_XL_EN_VAL);
  }
  else if(pin == INT2_PIN)
  {
    i2c_read_then_write(fd,LSM6DS33_INT2_CTRL_ADDR,ACCELEROMETER_INT2_DRDY_XL_EN_VAL);
  }

   status = wiringPiISR(pin, INT_EDGE_RISING,function); 

   close(fd);

   return status;
}





/**
 *@brief    Set Default Values to Gyroscope Configs
 *@param    *gy_config : pointer to gyroscope config structure
 *@retval   returns 0
 */
int gyroscope_config_set_defaults(gyroscope_config_typedef *gy_conf)
{
  gy_conf->gy_odr = GYROSCOPE_ODR_12_5_HZ_VAL;
  gy_conf->gy_fs = GYROSCOPE_FS_250_DPS_VAL;
  gy_conf->gy_mode = GYROSCOPE_LOW_POWER_MODE_VAL;
  gy_conf->gy_bdu = LSM6DS33_BDU_ENABLE_VAL;
  gy_conf->gy_axis = GYROSCOPE_XYZ_AXIS_VAL;

  return 0;
}





/**
 *@brief    Initialize Gyroscope
 *@param    *xl_config : pointer to accelerometer config structure
 *@retval   0 : On Success
           -1 : On Error  
 */
int gyroscope_init(gyroscope_config_typedef *gy_conf)
{
  int fd;

  uint8_t ctrl2_g_reg_data;

  /* Initializes wiringPi */
  wiringPiSetup(); 

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("gyroscope_init : error \n");
    return -1;
  }

  /* 20ms Delay for boot process */
  delay(20);

  /*Put the Gyroscope in Powerdown mode Initially*/
  i2c_write(fd,LSM6DS33_CTRL2_G_ADDR,0x00);

  /*Set Gyroscope operating mode*/
  i2c_write(fd,LSM6DS33_CTRL7_G_ADDR,gy_conf->gy_mode);

  /*Set Gyroscope axis */
  i2c_write(fd,LSM6DS33_CTRL10_C_ADDR,gy_conf->gy_axis);

  /*Gather all data to be written into CTRL2_G(11h) Register and Write to it*/
  ctrl2_g_reg_data = (gy_conf->gy_odr | gy_conf->gy_fs);
  i2c_write(fd,LSM6DS33_CTRL2_G_ADDR,ctrl2_g_reg_data);

  /*Set Block data update value*/
  i2c_read_then_write(fd,LSM6DS33_CTRL3_C_ADDR,gy_conf->gy_bdu);

  close(fd);

  return 0;
}






/**
 *@brief    Get Raw Gyroscope data
 *@param    *val : pointer to data array 
 *@retval   0 : On Success
           -1 : On Error
 */
int gyroscope_get_raw_data(int16_t *val)
{
  int fd;
  uint8_t x_low,x_high,y_low,y_high,z_low,z_high;


  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("gyroscope_get_raw_data : error \n");
    return -1;
  }


  x_low = i2c_read(fd,LSM6DS33_OUTX_L_G_ADDR);
  x_high = i2c_read(fd,LSM6DS33_OUTX_H_G_ADDR);

  y_low = i2c_read(fd,LSM6DS33_OUTY_L_G_ADDR);
  y_high = i2c_read(fd,LSM6DS33_OUTY_H_G_ADDR);


  z_low = i2c_read(fd,LSM6DS33_OUTZ_L_G_ADDR);
  z_high = i2c_read(fd,LSM6DS33_OUTZ_H_G_ADDR);


  val[0] = (int16_t)x_high;
  val[0] = (val[0] * 256) + (int16_t)x_low;

  val[1] = (int16_t)y_high;
  val[1] = (val[1] * 256) + (int16_t)y_low;

  val[2] = (int16_t)z_high;
  val[2] = (val[2] * 256) + (int16_t)z_low;

  close(fd);

  return 0;

}




/**
 *@brief    Get the Gyroscope data ready flag
 *@param    none
 *@retval   0 or 1 : on success
            -1 on error
 */      
int gyroscope_flag_data_ready_get(void)
{
  uint8_t status_reg,fd;

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("gyroscope_flag_data_ready_get: error\n");
    return -1;
  }

  status_reg = i2c_read(fd,LSM6DS33_STATUS_REG_ADDR);

  close(fd);

  return (status_reg & STATUS_REG_GDA_NEW_DATA_MSK);
}





/**
 *@brief    Set Gyroscope Interrupt
 *@param    pin : Interrupt Pin
            function: function pointer to handler
 *@retval   0 : On Success
           -1 : On Error  
 */
int gyroscope_set_interrupt(interrupt_pin_typedef pin, void(*function)(void))
{
  int fd, status;

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("gyroscope_set_interrupt: error\n");
    return -1;
  }
  
  if(pin == INT1_PIN)
  {
    i2c_write(fd,LSM6DS33_INT1_CTRL_ADDR,GYROSCOPE_INT1_DRDY_G_EN_VAL);
  }
  else if(pin == INT2_PIN)
  {
    i2c_write(fd,LSM6DS33_INT2_CTRL_ADDR,GYROSCOPE_INT2_DRDY_G_EN_VAL);
  }

   status = wiringPiISR(pin, INT_EDGE_RISING,function); 

   close(fd);

   return status;
}





/**
 *@brief    Set Default Values to Single Tap Configs
 *@param    *st_config : pointer to single tap config structure
 *@retval   returns 0
 */
int single_tap_config_set_defaults(single_tap_config_typedef *st_conf)
{
  st_conf->st_xl_odr = ACCELEROMETER_ODR_416_HZ_VAL;
  st_conf->st_xl_fs = ACCELEROMETER_FS_2G_VAL;
  st_conf->st_axis = TAP_XYZ_EN_VAL;
  st_conf->st_tap_ths = SINGLE_TAP_THD_6D_VAL;
  st_conf->st_quiet_time = SINGLE_TAP_QUIET_TIME_VAL;
  st_conf->st_shock_time = SINGLE_TAP_SHOCK_TIME_VAL;
  st_conf->st_int_latch = INT_LATCH_DISABLE;

  return 0;
}



/**
 *@brief    Single Tap detection Initialization
 *@param    *st_config : pointer to single tap config structure
 *@retval   0 : On Success
           -1 : On Error   
 */
int single_tap_detection_init(single_tap_config_typedef *st_conf)
{
  int fd;

  /* Initializes wiringPi */
  wiringPiSetup(); 

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("single_tap_detection_init : error \n");
    return -1;
  }

  /* 20ms Delay for boot process */
  delay(20);
 
  /* Turn on the accelerometer,ODR_XL = 416 Hz, FS_XL = ±2 g (Default Values ) */ 
  i2c_write(fd,LSM6DS33_CTRL1_XL_ADDR,(st_conf->st_xl_odr | st_conf->st_xl_fs));
  
  /* Enable tap detection on Given Axis */
  i2c_read_then_write(fd,LSM6DS33_TAP_CFG_ADDR,st_conf->st_axis);

  /* Set tap threshold */
  i2c_write(fd,LSM6DS33_TAP_THS_6D_ADDR,st_conf->st_tap_ths ); 
  
  /* Set Quiet and Shock time windows */
  i2c_write(fd,LSM6DS33_INT_DUR2_ADDR,(st_conf->st_quiet_time |st_conf->st_shock_time));
  
  /* Enable Single tap only */
  i2c_read_then_write(fd,LSM6DS33_WAKE_UP_THS_ADDR,WAKE_UP_THS_SINGLE_TAP_EN_VAL); 

  close(fd);

  return 0;
}





/**
 *@brief    Get the Single tap detection event
 *@param    none
 *@retval   0 or 1 : on success
            -1 on error
 */      
int single_tap_get_event(void)
{
  uint8_t tap_src,fd;

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("single_tap_get_event: error\n");
    return -1;
  }

  tap_src = i2c_read(fd,LSM6DS33_TAP_SRC_ADDR);

  close(fd);

  return (tap_src & TAP_SRC_REG_SINGLE_TAP_MSK);
}







/**
 *@brief    Set Single Tap detection Interrupt
 *@param    *st_conf: pointer to single tap conf structure
 *          pin : Interrupt Pin
 *          function: function pointer to handler
 *@retval   0 : On Success
           -1 : On Error  
 */
int single_tap_set_interrupt(single_tap_config_typedef *st_conf,interrupt_pin_typedef pin, void(*function)(void))
{
  int fd, status;

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("single_tap_set_interrupt: error\n");
    return -1;
  }

  if(st_conf->st_int_latch == INT_LATCH_ENABLE)
  {
    /*Enable Interrupt Latch*/
    i2c_read_then_write(fd,LSM6DS33_TAP_CFG_ADDR,0x01);

    if(pin == INT1_PIN)
    {
      i2c_read_then_write(fd,LSM6DS33_MD1_CFG_ADDR,SINGLE_TAP_INT1_LATCH_EN_VAL);
    }
    else if(pin == INT2_PIN)
    {
      i2c_read_then_write(fd,LSM6DS33_MD2_CFG_ADDR,SINGLE_TAP_INT2_LATCH_EN_VAL);
    }
  
  }
  else if (st_conf->st_int_latch == INT_LATCH_DISABLE)
  {
    if(pin == INT1_PIN)
    {
      i2c_read_then_write(fd,LSM6DS33_MD1_CFG_ADDR,SINGLE_TAP_INT1_EN_VAL);
    }
    else if(pin == INT2_PIN)
    {
      i2c_read_then_write(fd,LSM6DS33_MD2_CFG_ADDR,SINGLE_TAP_INT2_EN_VAL);
    }
  }


   status = wiringPiISR(pin, INT_EDGE_RISING, function); 

   close(fd);

   return status;
}



/**
 *@brief    Clear Single Tap interrupt Latch
            This function must be called in interrupt handler
            if interrupt latch is enabled.
 *@param    none
 *@retval   0 : On Success
           -1 : On Error 
 */
int single_tap_clear_interrupt_latch(void)
{
  int fd;

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("single_tap_clear_interrupt_latch: error\n");
    return -1;
  }

  i2c_read(fd,LSM6DS33_TAP_SRC_ADDR);
  close(fd);

  return 0;

}





/**
 *@brief    Set Default Values to double Tap Configs
 *@param    *dt_config : pointer to double tap config structure
 *@retval   returns 0
 */
int double_tap_config_set_defaults(double_tap_config_typedef *dt_conf)
{
  dt_conf->dt_xl_odr = ACCELEROMETER_ODR_416_HZ_VAL;
  dt_conf->dt_xl_fs = ACCELEROMETER_FS_2G_VAL;
  dt_conf->dt_axis = TAP_XYZ_EN_VAL;
  dt_conf->dt_tap_ths = DOUBLE_TAP_THD_6D_VAL;
  dt_conf->dt_dur_time = DOUBLE_TAP_DUR_TIME_VAL;
  dt_conf->dt_quiet_time = DOUBLE_TAP_QUITE_TIME_VAL;
  dt_conf->dt_shock_time = DOUBLE_TAP_SHOCK_TIME_VAL;
  dt_conf->dt_int_latch = INT_LATCH_DISABLE;

  return 0;
}







/**
 *@brief    Double Tap detection Initialization
 *@param    *dt_config : pointer to double tap config structure
 *@retval   0 : On Success
           -1 : On Error   
 */
int double_tap_detection_init(double_tap_config_typedef *dt_conf)
{
  int fd;

  /* Initializes wiringPi */
  wiringPiSetup(); 

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("double_tap_detection_init : error \n");
    return -1;
  }

  /* 20ms Delay for boot process */
  delay(20);
 
  /* Turn on the accelerometer,ODR_XL = 416 Hz, FS_XL = ±2 g (Default Values ) */ 
  i2c_write(fd,LSM6DS33_CTRL1_XL_ADDR,(dt_conf->dt_xl_odr | dt_conf->dt_xl_fs));
  
  /* Enable tap detection on Given Axis */
  i2c_read_then_write(fd,LSM6DS33_TAP_CFG_ADDR,dt_conf->dt_axis);

  /* Set tap threshold */
  i2c_write(fd,LSM6DS33_TAP_THS_6D_ADDR,dt_conf->dt_tap_ths); 
  
  /* Set double tap duration, Quiet and Shock time windows */
  i2c_write(fd,LSM6DS33_INT_DUR2_ADDR,(dt_conf->dt_quiet_time|dt_conf->dt_shock_time|dt_conf->dt_dur_time));
  
  /* Enable Double and Single tap */
  i2c_read_then_write(fd,LSM6DS33_WAKE_UP_THS_ADDR,WAKE_UP_THS_DOUBLE_TAP_EN_VAL); 
  //wiringPiI2CWriteReg8(fd,LSM6DS33_WAKE_UP_THS_ADDR,WAKE_UP_THS_DOUBLE_TAP_EN_VAL);

  close(fd);

  return 0;
}





/**
 *@brief    Get the Double tap detection event
 *@param    none
 *@retval   0 or 1 : on success
            -1 on error
 */      
int double_tap_get_event(void)
{
  uint8_t tap_src,fd;

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("double_tap_get_event: error\n");
    return -1;
  }

  tap_src = i2c_read(fd,LSM6DS33_TAP_SRC_ADDR);

  close(fd);

  return (tap_src & TAP_SRC_REG_DOUBLE_TAP_MSK);
}






/**
 *@brief    Set Double Tap detection Interrupt
 *@param    *dt_conf: pointer to double tap conf structure
 *          pin : Interrupt Pin
 *          function: function pointer to handler
 *@retval   0 : On Success
           -1 : On Error  
 */
int double_tap_set_interrupt(double_tap_config_typedef *dt_conf, interrupt_pin_typedef pin, void(*function)(void))
{
  int fd, status;

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("double_tap_set_interrupt: error\n");
    return -1;
  }

  if(dt_conf->dt_int_latch == INT_LATCH_ENABLE)
  {
    /*Enable Interrupt Latch*/
    i2c_read_then_write(fd,LSM6DS33_TAP_CFG_ADDR,0x01);
  }
  
  if(pin == INT1_PIN)
  {
    i2c_read_then_write(fd,LSM6DS33_MD1_CFG_ADDR,DOUBLE_TAP_INT1_EN_VAL);
  }
  else if(pin == INT2_PIN)
  {
    i2c_read_then_write(fd,LSM6DS33_MD2_CFG_ADDR,DOUBLE_TAP_INT2_EN_VAL);
  }

   status = wiringPiISR(pin, INT_EDGE_RISING, function); 

   close(fd);

   return status;
}







/**
 *@brief    Clear Double Tap interrupt Latch
            This function must be called in interrupt handler
            if interrupt latch is enabled.
 *@param    none
 *@retval   0 : On Success
           -1 : On Error 
 */
int double_tap_clear_interrupt_latch(void)
{
  int fd;

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("double_tap_clear_interrupt_latch: error\n");
    return -1;
  }

  i2c_read(fd,LSM6DS33_TAP_SRC_ADDR);
  close(fd);

  return 0;

}






/**
 *@brief    Set Default Values to Free Fall Configs
 *@param    *ff_config : pointer to freefall config structure
 *@retval   returns 0
 */
int freefall_config_set_defaults(freefall_config_typedef *ff_conf)
{
  ff_conf->ff_xl_odr = ACCELEROMETER_ODR_416_HZ_VAL;
  ff_conf->ff_xl_fs = ACCELEROMETER_FS_2G_VAL;
  ff_conf->ff_ths = FREEFALL_FF_THS_312_MG_VAL;
  ff_conf->ff_dur = FREEFALL_FF_DUR_VAL;
  ff_conf->ff_int_latch = INT_LATCH_DISABLE;

  return 0;
}





/**
 *@brief    Free fall detection Initialization
 *@param    *ff_config : pointer to free fall config structure
 *@retval   0 : On Success
           -1 : On Error   
 */
int freefall_detection_init(freefall_config_typedef *ff_conf)
{
  int fd;

  /* Initializes wiringPi */
  wiringPiSetup(); 

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("freefall_detection_init : error \n");
    return -1;
  }

  /* 20ms Delay for boot process */
  delay(20);
 
  /* Turn on the accelerometer,ODR_XL = 416 Hz, FS_XL = ±2 g (Default Values ) */ 
  i2c_write(fd,LSM6DS33_CTRL1_XL_ADDR,(ff_conf->ff_xl_odr | ff_conf->ff_xl_fs));
  
  /* Set Event duration duration FF_DUR5 bit */
  i2c_write(fd,LSM6DS33_WAKE_UP_DUR_ADDR,0x00);

  /* Set free fall threshold FF_THS[2:0] and Sample Event duration FF_DUR[4:0] */
  i2c_write(fd,LSM6DS33_FREE_FALL_ADDR,(ff_conf->ff_ths | ff_conf->ff_dur));
  
  close(fd);

  return 0;
}





/**
 *@brief    Get the free fall detection event
 *@param    none
 *@retval   0 or 1 : on success
            -1 on error
 */      
int freefall_get_event(void)
{
  uint8_t wake_up_src,fd;

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("freefall_get_event: error\n");
    return -1;
  }

  wake_up_src = i2c_read(fd,LSM6DS33_WAKE_UP_SRC_ADDR);

  close(fd);

  return (wake_up_src & WAKE_UP_SRC_REG_FREEFALL_MSK);
}





/**
 *@brief    Set Free Fall detection Interrupt
 *@param    *ff_conf: pointer to freefall conf structure
 *          pin : Interrupt Pin
 *          function: function pointer to handler
 *@retval   0 : On Success
           -1 : On Error  
 */
int freefall_set_interrupt(freefall_config_typedef *ff_conf, interrupt_pin_typedef pin, void(*function)(void))
{
  int fd, status;

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("freefall_set_interrupt: error\n");
    return -1;
  }

  if(ff_conf->ff_int_latch == INT_LATCH_ENABLE)
  {
    /*Enable Interrupt Latch*/
    i2c_read_then_write(fd,LSM6DS33_TAP_CFG_ADDR,0x01);
  }
  
  if(pin == INT1_PIN)
  {
    i2c_read_then_write(fd,LSM6DS33_MD1_CFG_ADDR,FREFALL_INT1_EN_VAL);
  }
  else if(pin == INT2_PIN)
  {
    i2c_read_then_write(fd,LSM6DS33_MD2_CFG_ADDR,FREFALL_INT2_EN_VAL);
  }

   status = wiringPiISR(pin, INT_EDGE_RISING, function); 

   close(fd);

   return status;
}





/**
 *@brief    Clear Free Fall interrupt Latch
            This function must be called in interrupt handler
            if interrupt latch is enabled.
 *@param    none
 *@retval   0 : On Success
           -1 : On Error 
 */
int freefall_clear_interrupt_latch(void)
{
  int fd;

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("freefall_clear_interrupt_latch: error\n");
    return -1;
  }

  i2c_read(fd,LSM6DS33_WAKE_UP_SRC_ADDR);
  close(fd);

  return 0;

}




/**
 *@brief    Set Default Values to Wake up detection Configs
 *@param    *wu_config : pointer to wake up config structure
 *@retval   returns 0
 */
int wake_up_config_set_defaults(wake_up_config_typedef *wu_conf)
{
  wu_conf->wu_xl_odr = ACCELEROMETER_ODR_416_HZ_VAL;
  wu_conf->wu_xl_fs = ACCELEROMETER_FS_2G_VAL;
  wu_conf->wu_ths = WAKE_UP_THS_VAL;
  wu_conf->wu_dur = WAKE_UP_DUR_VAL;
  wu_conf->wu_int_latch = INT_LATCH_DISABLE;

  return 0;
}





/**
 *@brief    Wake up detection Initialization
 *@param    *wu_config : pointer to wake up config structure
 *@retval   0 : On Success
           -1 : On Error   
 */
int wake_up_detection_init(wake_up_config_typedef *wu_conf)
{
  int fd;

  /* Initializes wiringPi */
  wiringPiSetup(); 

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("wake_up_detection_init : error \n");
    return -1;
  }

  /* 20ms Delay for boot process */
  delay(20);
 
  /* Turn on the accelerometer,ODR_XL = 416 Hz, FS_XL = ±2 g (Default Values ) */ 
  i2c_write(fd,LSM6DS33_CTRL1_XL_ADDR,(wu_conf->wu_xl_odr | wu_conf->wu_xl_fs));

  /* Apply high pass digtial filer */
  i2c_read_then_write(fd,LSM6DS33_TAP_CFG_ADDR,0x10);
  
  /* Set wake up duration  */
  i2c_read_then_write(fd,LSM6DS33_WAKE_UP_DUR_ADDR,wu_conf->wu_dur);


  /* Set wake up threshold  */
  i2c_read_then_write(fd,LSM6DS33_WAKE_UP_THS_ADDR,wu_conf->wu_ths);
  
  close(fd);

  return 0;
}





/**
 *@brief    Get the Wake up detection event
 *@param    none
 *@retval   0 or 1 : on success
            -1 on error
 */      
int wake_up_get_event(void)
{
  uint8_t wake_up_src,fd;

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("wake_up_get_event: error\n");
    return -1;
  }

  wake_up_src = i2c_read(fd,LSM6DS33_WAKE_UP_SRC_ADDR);

  close(fd);

  return (wake_up_src & WAKE_UP_SRC_REG_WU_IA_MSK);

}





/**
 *@brief    Set wake up detection Interrupt
 *@param    *wu_conf: pointer to wake up conf structure
 *          pin : Interrupt Pin
 *          function: function pointer to handler
 *@retval   0 : On Success
           -1 : On Error  
 */
int wake_up_set_interrupt(wake_up_config_typedef *wu_conf, interrupt_pin_typedef pin, void(*function)(void))
{
  int fd, status;

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("wake_up_set_interrupt: error\n");
    return -1;
  }

  if(wu_conf->wu_int_latch == INT_LATCH_ENABLE)
  {
    /*Enable Interrupt Latch*/
    i2c_read_then_write(fd,LSM6DS33_TAP_CFG_ADDR,0x01);
  }
  
  if(pin == INT1_PIN)
  {
    i2c_read_then_write(fd,LSM6DS33_MD1_CFG_ADDR,WAKE_UP_IN1_EN_VAL);
  }
  else if(pin == INT2_PIN)
  {
    i2c_read_then_write(fd,LSM6DS33_MD2_CFG_ADDR,WAKE_UP_IN2_EN_VAL);
  }

   status = wiringPiISR(pin, INT_EDGE_RISING, function); 

   close(fd);

   return status;
}





/**
 *@brief    Clear Wake up interrupt Latch
            This function must be called in interrupt handler
            if interrupt latch is enabled.
 *@param    none
 *@retval   0 : On Success
           -1 : On Error 
 */
int wake_up_clear_interrupt_latch(void)
{
  int fd;

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("wake_up_clear_interrupt_latch: error\n");
    return -1;
  }

  i2c_read(fd,LSM6DS33_WAKE_UP_SRC_ADDR);
  close(fd);

  return 0;

}





/**
 *@brief    Set Default Values to Inactivity detection Configs
 *@param    *inact_config : pointer to inactivity config structure
 *@retval   returns 0
 */
int inactivity_config_set_defaults(inactivity_config_typedef *inact_conf)
{
  inact_conf->inact_xl_odr = ACCELEROMETER_ODR_208_HZ_VAL;
  inact_conf->inact_xl_fs = ACCELEROMETER_FS_2G_VAL;
  inact_conf->inact_ths = INACTIVITY_THS_VAL;
  inact_conf->inact_dur = INACTIVITY_DUR_VAL;

  return 0;
}




/**
 *@brief    inactivity  detection Initialization
 *@param    *inact_config : pointer to wake up config structure
 *@retval   0 : On Success
           -1 : On Error   
 */
int inactivity_detection_init(inactivity_config_typedef *inact_conf)
{
  int fd;

  /* Initializes wiringPi */
  wiringPiSetup(); 

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("inactivity_detection_init : error \n");
    return -1;
  }

  /* 20ms Delay for boot process */
  delay(20);
 
  /* Turn on the accelerometer,ODR_XL = 208 Hz, FS_XL = ±2 g (Default Values ) */ 
  i2c_write(fd,LSM6DS33_CTRL1_XL_ADDR,(inact_conf->inact_xl_odr | inact_conf->inact_xl_fs));

  /* Set duration for inactivity detection */
  i2c_read_then_write(fd,LSM6DS33_WAKE_UP_DUR_ADDR,inact_conf->inact_dur);

  /* Enable inactivity detection and Set inactivity  threshold  */
  i2c_read_then_write(fd,LSM6DS33_WAKE_UP_THS_ADDR,(inact_conf->inact_ths| 0x40));
  
  close(fd);

  return 0;
}





/**
 *@brief    Get the inactivity detection event
 *@param    none
 *@retval   0 or 1 : on success
            -1 on error
 */      
int inactivity_get_event(void)
{
  uint8_t wake_up_src,fd;

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("inactivity_get_event: error\n");
    return -1;
  }

  wake_up_src = i2c_read(fd,LSM6DS33_WAKE_UP_SRC_ADDR);

  close(fd);

  return (wake_up_src & WAKE_UP_SRC_REG_SLEEP_EVENT_MSK);

}





/**
 *@brief    Set inactivity detection Interrupt
 *@param    pin : Interrupt Pin
            function: function pointer to handler
 *@retval   0 : On Success
           -1 : On Error  
 */
int inactivity_set_interrupt(interrupt_pin_typedef pin, void(*function)(void))
{
  int fd, status;

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("inactivity_set_interrupt: error\n");
    return -1;
  }
  
  if(pin == INT1_PIN)
  {
    i2c_read_then_write(fd,LSM6DS33_MD1_CFG_ADDR,INACTIVITY_INT1_EN_VAL);
  }
  else if(pin == INT2_PIN)
  {
    i2c_read_then_write(fd,LSM6DS33_MD2_CFG_ADDR,INACTIVITY_INT2_EN_VAL);
  }

   status = wiringPiISR(pin, INT_EDGE_RISING, function); 

   close(fd);

   return status;
}





/**
 *@brief    Set Default Values to significant motion detection Configs
 *@param    *sm_config : pointer to singinificant motion config structure
 *@retval   returns 0
 */
int significant_motion_config_set_defaults(significant_motion_config_typedef *sm_conf)
{
  sm_conf->sm_xl_odr = ACCELEROMETER_ODR_26_HZ_VAL;
  sm_conf->sm_xl_fs = ACCELEROMETER_FS_2G_VAL;
  sm_conf->sm_ths = SIGNIFICANT_MOTION_THS_VAL;
  sm_conf->sm_int_latch = INT_LATCH_DISABLE;

  return 0;
}





/**
 *@brief    significant motion detection Initialization
 *@param    *sm_config : pointer to significant motion config structure
 *@retval   0 : On Success
           -1 : On Error   
 */
int significant_motion_detection_init(significant_motion_config_typedef *sm_conf)
{
  int fd;
  uint8_t tap_cfg_reg;

  /* Initializes wiringPi */
  wiringPiSetup(); 

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("significant_motion_detection_init : error \n");
    return -1;
  }

  /* 20ms Delay for boot process */
  delay(20);

  /*Enable access to embedded function registers*/
  i2c_write(fd,LSM6DS33_FUNC_CFG_ACCESS_ADDR,0x80);

  /* Set significant motion threshold */
  i2c_write(fd,LSM6DS33_SM_THS_ADDR,sm_conf->sm_ths);

  /*disable access to embedded function registers */
  i2c_write(fd,LSM6DS33_FUNC_CFG_ACCESS_ADDR,0x00);
 
  /* Turn on the accelerometer,ODR_XL = 26 Hz, FS_XL = ±2 g (Default Values ) */ 
  i2c_write(fd,LSM6DS33_CTRL1_XL_ADDR,(sm_conf->sm_xl_odr | sm_conf->sm_xl_fs));

  /* Disable pedometer */
  tap_cfg_reg = wiringPiI2CReadReg8(fd,LSM6DS33_TAP_CFG_ADDR);
  tap_cfg_reg = tap_cfg_reg & 0xBF; 
  i2c_write(fd,LSM6DS33_TAP_CFG_ADDR,tap_cfg_reg);

  /*Enable embedded function and Significant motion detection */
  i2c_write(fd,LSM6DS33_CTRL10_C_ADDR,0x3D);

  /* Enable pedometer algorithm */
  i2c_read_then_write(fd,LSM6DS33_TAP_CFG_ADDR,0x40);
  
  close(fd);

  return 0;
}





/**
 *@brief    Get the significant motion detection event
 *@param    none
 *@retval   0 or 1 : on success
            -1 on error
 */      
int significant_motion_get_event(void)
{
  uint8_t func_src,fd;

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("significant_motion_get_event: error\n");
    return -1;
  }


  func_src = i2c_read(fd,LSM6DS33_FUNC_SRC_ADDR);

  close(fd);

  return (func_src & FUNC_SRC_REG_SIGN_MOTION_IA_MSK);

}





/**
 *@brief    Set significant motion detection Interrupt
            Interrupt will be generated only on INT1 Pin
 *@param    *sm_conf: pointer to significant motion conf structure
 *          function: function pointer to handler
 *@retval   0 : On Success
           -1 : On Error  
 */
int significant_motion_set_interrupt(significant_motion_config_typedef *sm_conf, void(*function)(void))
{
  int fd, status;

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("significant_motion_set_interrupt: error\n");
    return -1;
  }
  
  if(sm_conf->sm_int_latch == INT_LATCH_ENABLE)
  {
    /* Enable interrupt latch */
    i2c_read_then_write(fd,LSM6DS33_TAP_CFG_ADDR, 0x01);
  }

  
  i2c_read_then_write(fd,LSM6DS33_INT1_CTRL_ADDR,SIGN_MOTION_INT_EN_VAL);
 
  status = wiringPiISR(INT1_PIN, INT_EDGE_RISING, function); 

  close(fd);

  return status;
}





/**
 *@brief    Clear significant motion interrupt Latch
            This function must be called in interrupt handler
            if interrupt latch is enabled.
 *@param    none
 *@retval   0 : On Success
           -1 : On Error 
 */
int significant_motion_clear_interrupt_latch(void)
{
  int fd;

  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("significant_motion_clear_interrupt_latch: error\n");
    return -1;
  }

  i2c_read(fd,LSM6DS33_FUNC_SRC_ADDR);
  close(fd);

  return 0;
}











/**
 *@brief    Get Raw temperature data
            Either accelerometer or Gyroscope need to be active before calling this function
 *@param    *val : pointer to data array 
 *@retval   0 : On Success
           -1 : On Error
 */
int temperature_get_raw_data(int16_t *val)
{
  int fd;
  uint8_t t_low, t_high;


  fd = check_i2c_setup(LSM6DS33_DEVICE_ID);
  if(fd == -1)
  {
    printf("temperature_get_raw_data : error \n");
    return -1;
  }


  t_low = i2c_read(fd,LSM6DS33_OUT_TEMP_L_ADDR);
  t_high = i2c_read(fd,LSM6DS33_OUT_TEMP_H_ADDR);


  val[0] = (int16_t)t_high;
  val[0] = (val[0] * 256) + (int16_t)t_low;

  close(fd);

  return 0;
  
}




/*------------------------To be implememted later (if required)---------------------------------*/

/*
int accelerometer_self_test(int fd)
{
  uint8_t x_low,x_high,y_low,y_high,z_low,z_high,statu_reg;
  uint16_t OUTX_NOST, OUTY_NOST, OUTZ_NOST;
  uint16_t OUTX_ST, OUTY_ST, OUTZ_ST;

  //initialize Sensor, turn on Sensor,enable X/Y/Z Axis
  //Set BDU=1, FS=2G, ODR = 52 Hz 
  i2c_write(fd,LSM6DS33_CTRL1_XL_ADDR,0x30);
  i2c_write(fd,LSM6DS33_CTRL2_G_ADDR,0x00);
  i2c_write(fd,LSM6DS33_CTRL3_C_ADDR,0x44);
  i2c_write(fd,LSM6DS33_CTRL4_C_ADDR,0x00);
  i2c_write(fd,LSM6DS33_CTRL5_C_ADDR,0x00);
  i2c_write(fd,LSM6DS33_CTRL6_C_ADDR,0x00);
  i2c_write(fd,LSM6DS33_CTRL7_G_ADDR,0x00);
  i2c_write(fd,LSM6DS33_CTRL8_XL_ADDR,0x00);
  i2c_write(fd,LSM6DS33_CTRL9_XL_ADDR,0x38);
  i2c_write(fd,LSM6DS33_CTRL10_C_ADDR,0x00);

  //Wiat 200ms for stable output 
  //delay(200);

  //Poll for XLDA bit 
  //do {
    //data_ready = accelerometer_flag_data_ready_get(fd);
  //} while(!data_ready); 



} */






/*------------------------To be implememted later (if required)---------------------------------*/
