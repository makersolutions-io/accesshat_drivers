/**
  *****************************************************************************************
  *@file    : accesshat_inertial_module.h
  *@Brief   : Header file for Driving AccessHAT LSM6DS33 inertial module. 
  
  *****************************************************************************************
*/


#ifndef ACCESSHAT_INERTIAL_MODULE_H
#define ACCESSHAT_INERTIAL_MODULE_H

#include <stdint.h>
#include <float.h>
#include <math.h>


/* LSM6DS33 I2C Device ID */
#define LSM6DS33_DEVICE_ID                   0x6A


/* Accelerometer Output Data Rate Values */
#define ACCELEROMETER_ODR_12_5_HZ_VAL        0x10
#define ACCELEROMETER_ODR_26_HZ_VAL          0x20
#define ACCELEROMETER_ODR_52_HZ_VAL          0x30
#define ACCELEROMETER_ODR_104_HZ_VAL         0x40
#define ACCELEROMETER_ODR_208_HZ_VAL         0x50
#define ACCELEROMETER_ODR_416_HZ_VAL         0x60
#define ACCELEROMETER_ODR_833_HZ_VAL         0x70
#define ACCELEROMETER_ODR_1_66_KHZ_VAL       0x80
#define ACCELEROMETER_ODR_3_33_KHZ_VAL       0x90
#define ACCELEROMETER_ODR_6_66_KHZ_VAL       0xA0

/* Accelerometer Full Scale Selection Values */
#define ACCELEROMETER_FS_2G_VAL              0x00
#define ACCELEROMETER_FS_4G_VAL              0x08
#define ACCELEROMETER_FS_8G_VAL              0x0C
#define ACCELEROMETER_FS_16G_VAL             0x04

/* Accelerometer Anti-aliasing filter bandwidth selection values*/
#define ACCELEROMETER_BW_400_HZ_VAL           0x00
#define ACCELEROMETER_BW_200_HZ_VAL           0x01
#define ACCELEROMETER_BW_100_HZ_VAL           0x02
#define ACCELEROMETER_BW_50_HZ_VAL            0x03

/* Accelerometer x,y,z axis enable */
#define ACCELEROMETER_X_AXIS_ONLY_VAL         0x08
#define ACCELEROMETER_Y_AXIS_ONLY_VAL         0x10
#define ACCELEROMETER_Z_AXIS_ONLY_VAL         0x20
#define ACCELEROMETER_XYZ_AXIS_VAL            0x38

/*Accelerometer Operating mode */
#define ACCELEROMETER_HIGH_PERF_MODE_VAL      0x00 //high-performance mode is valid for all ODRs (from 12.5 Hz upto 6.66 kHz).
#define ACCELEROMETER_LOW_POWER_MODE_VAL      0x10 //Low-power mode is available for lower ODRs (12.5, 26, 52 Hz)
#define ACCELEROMETER_NORMAL_MODE_VAL         0x10 //normal mode is available for ODRs equal to 104 and 208 Hz.


/* Accelerometer Output Data Rate Values */
#define GYROSCOPE_ODR_12_5_HZ_VAL             0x10
#define GYROSCOPE_ODR_26_HZ_VAL               0x20
#define GYROSCOPE_ODR_52_HZ_VAL               0x30
#define GYROSCOPE_ODR_104_HZ_VAL              0x40
#define GYROSCOPE_ODR_208_HZ_VAL              0x50
#define GYROSCOPE_ODR_416_HZ_VAL              0x60
#define GYROSCOPE_ODR_833_HZ_VAL              0x70
#define GYROSCOPE_ODR_1_66_KHZ_VAL            0x80

/* Gyroscope full scale selection values */
#define GYROSCOPE_FS_125_DPS_VAL              0x02
#define GYROSCOPE_FS_250_DPS_VAL              0x00
#define GYROSCOPE_FS_500_DPS_VAL              0x04
#define GYROSCOPE_FS_1000_DPS_VAL             0x08
#define GYROSCOPE_FS_2000_DPS_VAL             0x0C

/* Gyroscope Operating mode */
#define GYROSCOPE_HIGH_PERF_MODE_VAL          0x00 //high-performance mode is valid for all ODRs (from 12.5 Hz up to 1.6 kHz).
#define GYROSCOPE_LOW_POWER_MODE_VAL          0x80 //Low-power mode is available for lower ODR (12.5, 26, 52 Hz)
#define GYROSCOPE_NORMAL_MODE_VAL             0x80 //normal mode is available for ODRs equal to 104 and 208 Hz.


/* Gyroscope x,y,z axis enable */
#define GYROSCOPE_X_AXIS_ONLY_VAL             0x08
#define GYROSCOPE_Y_AXIS_ONLY_VAL             0x10
#define GYROSCOPE_Z_AXIS_ONLY_VAL             0x20
#define GYROSCOPE_XYZ_AXIS_VAL                0x38 


/* Block data update */
#define LSM6DS33_BDU_ENABLE_VAL               0x40 //output registers not updated until MSB and LSB have been read
#define LSM6DS33_BDU_DISABLE_VAL              0x00 //continuous update


/* Tap Detection Axis Enable */
#define TAP_X_EN_ONLY_VAL                     0x80
#define TAP_Y_EN_ONLY_VAL                     0x40
#define TAP_Z_EN_ONLY_VAL                     0x20
#define TAP_XYZ_EN_VAL                        0x0E

/* Single tap detection threshold values */
#define SINGLE_TAP_THD_6D_VAL                 0x09   // Tap threshold = (SINGLE_TAP_THD_6D_VAL * (FS_XL/ (2^5)))
#define SINGLE_TAP_QUIET_TIME_VAL             0x04   
#define SINGLE_TAP_SHOCK_TIME_VAL             0x02

/* Double tap detection threshold values */
#define DOUBLE_TAP_THD_6D_VAL                 0x0C // Tap threshold = (SINGLE_TAP_THD_6D_VAL * (FS_XL/ (2^5)))
#define DOUBLE_TAP_DUR_TIME_VAL               0x70
#define DOUBLE_TAP_QUITE_TIME_VAL             0x0C
#define DOUBLE_TAP_SHOCK_TIME_VAL             0x03


/* Free Fall thereshold Values */
#define FREEFALL_FF_THS_156_MG_VAL            0x00
#define FREEFALL_FF_THS_219_MG_VAL            0x01
#define FREEFALL_FF_THS_250_MG_VAL            0x02
#define FREEFALL_FF_THS_312_MG_VAL            0x03
#define FREEFALL_FF_THS_344_MG_VAL            0x04
#define FREEFALL_FF_THS_406_MG_VAL            0x05
#define FREEFALL_FF_THS_469_MG_VAL            0x06
#define FREEFALL_FF_THS_500_MG_VAL            0x07

/*Free Fall Event Duration Value */
#define FREEFALL_FF_DUR_VAL                   0x30


/* Wake up detection parameters */
#define WAKE_UP_THS_VAL                       0x02                                          
#define WAKE_UP_DUR_VAL                       0x00


/* Wake up detection parameters */
#define INACTIVITY_THS_VAL                    0x02                                          
#define INACTIVITY_DUR_VAL                    0x02

/* Significant motion threshold */
#define SIGNIFICANT_MOTION_THS_VAL            0x08


/* Interrupt Latch Enable */
typedef enum {
              INT_LATCH_ENABLE,
              INT_LATCH_DISABLE
             }interrupt_latch_typedef;


/* Interrupt Pin Typedef */
typedef enum{
             INT1_PIN = 4, //GPIO_23 (wiringPi pin 4)
             INT2_PIN = 5  //GPIO_24 (wiringPi pin 5)
            }interrupt_pin_typedef;


/* Accelerometer config typedef */
typedef struct accelerometer_config
{
  uint8_t xl_odr; // Ouput data rate
  uint8_t xl_fs;  // full scale selection2
  uint8_t xl_mode; // Operating mode
  uint8_t xl_bw; // Anti-aliasing filter bandwidth
  uint8_t xl_bdu; //Block data update
  uint8_t xl_axis; // Axis selection (x,y,z)

} accelerometer_config_typedef;


/* Gyroscope config typedef */
typedef struct gyroscope_config
{
  uint8_t gy_odr; // Ouput data rate
  uint8_t gy_fs;  // full scale selection2
  uint8_t gy_mode; // Operating mode
  uint8_t gy_bdu; //Block data update
  uint8_t gy_axis; // Axis selection (x,y,z)

} gyroscope_config_typedef;


/* Single Tap detection config typedef */
typedef struct single_tap_config
{
  uint8_t st_xl_odr; // Ouput data rate for accelerometer
  uint8_t st_xl_fs;  // full scale selection for accelerometer
  uint8_t st_axis; // Axis selection (x,y,z)
  uint8_t st_tap_ths; // tap threshold
  uint8_t st_quiet_time; // Quiet Time
  uint8_t st_shock_time; // Shock Time
  interrupt_latch_typedef st_int_latch;// Interrupt latch enable or disable 

} single_tap_config_typedef;



/* Double Tap detection config typedef */
typedef struct double_tap_config
{
  uint8_t dt_xl_odr; // Ouput data rate for accelerometer
  uint8_t dt_xl_fs;  // full scale selection for accelerometer
  uint8_t dt_axis; // Axis selection (x,y,z)
  uint8_t dt_tap_ths; // tap threshold
  uint8_t dt_dur_time; // Max time gap for double tap detection
  uint8_t dt_quiet_time; // Quiet Time
  uint8_t dt_shock_time; // Shock Time
  interrupt_latch_typedef dt_int_latch;// Interrupt latch enable or disable 

} double_tap_config_typedef;


/* Free Fall detection config typedef */
typedef struct freefall_config
{
  uint8_t ff_xl_odr; // Ouput data rate for accelerometer
  uint8_t ff_xl_fs;  // full scale selection for accelerometer
  uint8_t ff_ths; // Freefall threshold
  uint8_t ff_dur; // Freefall event duration
  interrupt_latch_typedef ff_int_latch;// Interrupt latch enable or disable 

} freefall_config_typedef;


/* Wake up detection config typedef */
typedef struct wake_up_config
{
  uint8_t wu_xl_odr; // Ouput data rate for accelerometer
  uint8_t wu_xl_fs;  // full scale selection for accelerometer
  uint8_t wu_ths; // Wake up threshold
  uint8_t wu_dur; // Wake duration
  interrupt_latch_typedef wu_int_latch;// Interrupt latch enable or disable 

} wake_up_config_typedef;



/* Inactivity detection config typedef */
typedef struct inactivity_config
{
  uint8_t inact_xl_odr; // Ouput data rate for accelerometer
  uint8_t inact_xl_fs;  // full scale selection for accelerometer
  uint8_t inact_ths; // Inactivity threshold
  uint8_t inact_dur; // Inactivity duration

} inactivity_config_typedef;


/* Significant Motion detection config typedef */
typedef struct significant_motion
{
  uint8_t sm_xl_odr; // Ouput data rate for accelerometer
  uint8_t sm_xl_fs;  // full scale selection for accelerometer
  uint8_t sm_ths; // Significant motion threshold
  interrupt_latch_typedef sm_int_latch;// Interrupt latch enable or disable 
} significant_motion_config_typedef;





/**
  * @defgroup    LSM6DS3_Sensitivity
  * @brief       These functions convert raw-data into engineering units.
  * 
  *
  */
float_t lsm6ds3_from_fs2g_to_mg(int16_t value);
float_t lsm6ds3_from_fs4g_to_mg(int16_t value);
float_t lsm6ds3_from_fs8g_to_mg(int16_t value);
float_t lsm6ds3_from_fs16g_to_mg(int16_t value);
float_t lsm6ds3_from_fs125dps_to_mdps(int16_t value);
float_t lsm6ds3_from_fs250dps_to_mdps(int16_t value);
float_t lsm6ds3_from_fs500dps_to_mdps(int16_t value);
float_t lsm6ds3_from_fs1000dps_to_mdps(int16_t lsb);
float_t lsm6ds3_from_fs2000dps_to_mdps(int16_t lsb);
float_t lsm6ds3_from_lsb_to_celsius(int16_t lsb);



/**
 *@brief    Read the WHO_AM_I register from IMU
 *@param    none
 *@retval   returns 0: on success
                   -1: on error
 */
int read_who_am_i(void);

/**
 *@brief    Set Default Values to Accelerometer Configs
 *@param    *xl_config : pointer to accelerometer config structure
 *@retval   returns 0
 */
int accelerometer_config_set_defaults(accelerometer_config_typedef *xl_conf);



/**
 *@brief    Initialize Accelerometer
 *@param    *xl_config : pointer to accelerometer config structure
 *@retval   0 : On Success
           -1 : On Error  
 */
int accelerometer_init(accelerometer_config_typedef *xl_conf);



/**
 *@brief    Get Raw Accelerometer data
 *@param    *val : pointer to data array 
 *@retval   0 : On Success
           -1 : On Error
 */
int accelerometer_get_raw_data(int16_t *val);



/**
 *@brief    Get the Accelerometer data ready flag
 *@param    none
 *@retval   0 or 1 : on success
            -1 on error
 */      
int accelerometer_flag_data_ready_get(void);


/**
 *@brief    Set Accelerometer Interrupt
 *@param    pin : Interrupt Pin
            function: function pointer to handler
 *@retval   0 : On Success
           -1 : On Error  
 */
int accelerometer_set_interrupt(interrupt_pin_typedef pin, void(*function)(void));



/**
 *@brief    Set Default Values to Gyroscope Configs
 *@param    *gy_config : pointer to gyroscope config structure
 *@retval   returns 0
 */
int gyroscope_config_set_defaults(gyroscope_config_typedef *gy_conf);


/**
 *@brief    Initialize Gyroscope
 *@param    *xl_config : pointer to accelerometer config structure
 *@retval   0 : On Success
           -1 : On Error  
 */
int gyroscope_init(gyroscope_config_typedef *gy_conf);


/**
 *@brief    Get Raw Gyroscope data
 *@param    *val : pointer to data array 
 *@retval   0 : On Success
           -1 : On Error
 */
int gyroscope_get_raw_data(int16_t *val);


/**
 *@brief    Get the Gyroscope data ready flag
 *@param    none
 *@retval   0 or 1 : on success
            -1 on error
 */      
int gyroscope_flag_data_ready_get(void);


/**
 *@brief    Set Gyroscope Interrupt
 *@param    pin : Interrupt Pin
            function: function pointer to handler
 *@retval   0 : On Success
           -1 : On Error  
 */
int gyroscope_set_interrupt(interrupt_pin_typedef pin, void(*function)(void));



/**
 *@brief    Set Default Values to Single Tap Configs
 *@param    *st_config : pointer to single tap config structure
 *@retval   returns 0
 */
int single_tap_config_set_defaults(single_tap_config_typedef *st_conf);


/**
 *@brief    Single Tap detection Initialization
 *@param    *st_config : pointer to single tap config structure
 *@retval   0 : On Success
           -1 : On Error   
 */
int single_tap_detection_init(single_tap_config_typedef *st_conf);


/**
 *@brief    Get the Single tap detection event
 *@param    none
 *@retval   0 or 1 : on success
            -1 on error
 */      
int single_tap_get_event(void);


/**
 *@brief    Set Single Tap detection Interrupt
 *@param    *st_conf: pointer to single tap conf structure
 *          pin : Interrupt Pin
 *          function: function pointer to handler
 *@retval   0 : On Success
           -1 : On Error  
 */
int single_tap_set_interrupt(single_tap_config_typedef *st_conf,interrupt_pin_typedef pin, void(*function)(void));


/**
 *@brief    Clear Single Tap interrupt Latch
            This function must be called in interrupt handler
            if interrupt latch is enabled.
 *@param    none
 *@retval   0 : On Success
           -1 : On Error 
 */
int single_tap_clear_interrupt_latch(void);


/**
 *@brief    Set Default Values to double Tap Configs
 *@param    *dt_config : pointer to double tap config structure
 *@retval   returns 0
 */
int double_tap_config_set_defaults(double_tap_config_typedef *dt_conf);


/**
 *@brief    Double Tap detection Initialization
 *@param    *dt_config : pointer to double tap config structure
 *@retval   0 : On Success
           -1 : On Error   
 */
int double_tap_detection_init(double_tap_config_typedef *dt_conf);


/**
 *@brief    Get the Double tap detection event
 *@param    none
 *@retval   0 or 1 : on success
            -1 on error
 */      
int double_tap_get_event(void);


/**
 *@brief    Set Double Tap detection Interrupt
 *@param    *dt_conf: pointer to double tap conf structure
 *          pin : Interrupt Pin
 *          function: function pointer to handler
 *@retval   0 : On Success
           -1 : On Error  
 */
int double_tap_set_interrupt(double_tap_config_typedef *dt_conf, interrupt_pin_typedef pin, void(*function)(void));


/**
 *@brief    Clear Double Tap interrupt Latch
            This function must be called in interrupt handler
            if interrupt latch is enabled.
 *@param    none
 *@retval   0 : On Success
           -1 : On Error 
 */
int double_tap_clear_interrupt_latch(void);


/**
 *@brief    Set Default Values to Free Fall Configs
 *@param    *ff_config : pointer to freefall config structure
 *@retval   returns 0
 */
int freefall_config_set_defaults(freefall_config_typedef *ff_conf);


/**
 *@brief    Free fall detection Initialization
 *@param    *ff_config : pointer to free fall config structure
 *@retval   0 : On Success
           -1 : On Error   
 */
int freefall_detection_init(freefall_config_typedef *ff_conf);


/**
 *@brief    Get the free fall detection event
 *@param    none
 *@retval   0 or 1 : on success
            -1 on error
 */      
int freefall_get_event(void);


/**
 *@brief    Set Free Fall detection Interrupt
 *@param    *ff_conf: pointer to freefall conf structure
 *          pin : Interrupt Pin
 *          function: function pointer to handler
 *@retval   0 : On Success
           -1 : On Error  
 */
int freefall_set_interrupt(freefall_config_typedef *ff_conf, interrupt_pin_typedef pin, void(*function)(void));


/**
 *@brief    Clear Free Fall interrupt Latch
            This function must be called in interrupt handler
            if interrupt latch is enabled.
 *@param    none
 *@retval   0 : On Success
           -1 : On Error 
 */
int freefall_clear_interrupt_latch(void);


/**
 *@brief    Set Default Values to Wake up detection Configs
 *@param    *wu_config : pointer to wake up config structure
 *@retval   returns 0
 */
int wake_up_config_set_defaults(wake_up_config_typedef *wu_conf);


/**
 *@brief    Wake up detection Initialization
 *@param    *wu_config : pointer to wake up config structure
 *@retval   0 : On Success
           -1 : On Error   
 */
int wake_up_detection_init(wake_up_config_typedef *wu_conf);


/**
 *@brief    Get the Wake up detection event
 *@param    none
 *@retval   0 or 1 : on success
            -1 on error
 */      
int wake_up_get_event(void);


/**
 *@brief    Set wake up detection Interrupt
 *@param    *wu_conf: pointer to wake up conf structure
 *          pin : Interrupt Pin
 *          function: function pointer to handler
 *@retval   0 : On Success
           -1 : On Error  
 */
int wake_up_set_interrupt(wake_up_config_typedef *wu_conf, interrupt_pin_typedef pin, void(*function)(void));


/**
 *@brief    Clear Wake up interrupt Latch
            This function must be called in interrupt handler
            if interrupt latch is enabled.
 *@param    none
 *@retval   0 : On Success
           -1 : On Error 
 */
int wake_up_clear_interrupt_latch(void);


/**
 *@brief    Set Default Values to Inactivity detection Configs
 *@param    *inact_config : pointer to inactivity config structure
 *@retval   returns 0
 */
int inactivity_config_set_defaults(inactivity_config_typedef *inact_conf);


/**
 *@brief    inactivity  detection Initialization
 *@param    *inact_config : pointer to wake up config structure
 *@retval   0 : On Success
           -1 : On Error   
 */
int inactivity_detection_init(inactivity_config_typedef *inact_conf);


/**
 *@brief    Get the inactivity detection event
 *@param    none
 *@retval   0 or 1 : on success
            -1 on error
 */      
int inactivity_get_event(void);


/**
 *@brief    Set inactivity detection Interrupt
 *@param    pin : Interrupt Pin
            function: function pointer to handler
 *@retval   0 : On Success
           -1 : On Error  
 */
int inactivity_set_interrupt(interrupt_pin_typedef pin, void(*function)(void));


/**
 *@brief    Set Default Values to significant motion detection Configs
 *@param    *sm_config : pointer to singinificant motion config structure
 *@retval   returns 0
 */
int significant_motion_config_set_defaults(significant_motion_config_typedef *sm_conf);

/**
 *@brief    significant motion detection Initialization
 *@param    *sm_config : pointer to significant motion config structure
 *@retval   0 : On Success
           -1 : On Error   
 */
int significant_motion_detection_init(significant_motion_config_typedef *sm_conf);


/**
 *@brief    Get the significant motion detection event
 *@param    none
 *@retval   0 or 1 : on success
            -1 on error
 */      
int significant_motion_get_event(void);


/**
 *@brief    Set significant motion detection Interrupt
            Interrupt will be generated only on INT1 Pin
 *@param    *sm_conf: pointer to significant motion conf structure
 *          function: function pointer to handler
 *@retval   0 : On Success
           -1 : On Error  
 */
int significant_motion_set_interrupt(significant_motion_config_typedef *sm_conf, void(*function)(void));


/**
 *@brief    Clear significant motion interrupt Latch
            This function must be called in interrupt handler
            if interrupt latch is enabled.
 *@param    none
 *@retval   0 : On Success
           -1 : On Error 
 */
int significant_motion_clear_interrupt_latch(void);


/**
 *@brief    Get Raw temperature data
            Either accelerometer or Gyroscope need to be active before calling this function
 *@param    *val : pointer to data array 
 *@retval   0 : On Success
           -1 : On Error
 */
int temperature_get_raw_data(int16_t *val);



#endif
