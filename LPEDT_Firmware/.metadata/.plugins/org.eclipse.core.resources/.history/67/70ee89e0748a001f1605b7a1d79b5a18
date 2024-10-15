/*******************************************************************************
 * Copyright (C) 2024 by Vishnu Kumar Thoodur Venkatachalapathy
 *
 * Redistribution, modification or use of this software in source or binary
 * forms is permitted as long as the files maintain this copyright. Users are
 * permitted to modify this and use it to learn about the field of embedded
 * software. Vishnu Kumar Thoodur Venkatachalapathy and the University of
 * Colorado are not liable for any misuse of this material.
 * ****************************************************************************/

/**
 * @file    i2c.c
 * @brief   Contains I2C based drivers for onboard sensors
 *
 * @author  Vishnu Kumar Thoodur Venkatachalapathy
 * @date    Feb 7, 2024
 */

#include "sl_i2cspm.h"
#include "em_device.h"
#include "src/gpio.h"
#include "src/timers.h"
#include "scheduler.h"
#include "i2c.h"

// Include logging specifically for this .c file
#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"

I2C_TransferReturn_TypeDef transferStatus; // make this global for IRQs in A4
I2C_TransferSeq_TypeDef transferSequence; // this one can be local
uint8_t cmd_data; // make this global for IRQs in A4
uint8_t read_data[2]  = {0,0}; // make this global for IRQs in A4

/**
 * @brief   Initialize the I2C peripheral to work with the Si7021 sensor
 * @return  none
 */
void I2C_Init_Si7021(){
  // Initialize the Si7021 sensor enable GPIO pin
  Si7021GPIOInit();

  // Init I2C Hardware
  I2CSPM_Init_TypeDef I2C_Config = {
    .port = I2C0,
    .sclPort = gpioPortC,
    .sclPin = 10,
    .sdaPort = gpioPortC,
    .sdaPin = 11,
    .portLocationScl = 14,
    .portLocationSda = 16,
    .i2cRefFreq = 0,
    .i2cMaxFreq = I2C_FREQ_STANDARD_MAX,
    .i2cClhr = i2cClockHLRStandard
  };

  I2CSPM_Init(&I2C_Config);
}

/**
 * @brief   Sends the given data over the I2C bus to the addressed device.
 *          Function makes use of IRQs.
 * @param   device_addr I2C device address to write data to
 * @param   data        Data to send over the I2C bus
 * @return  none
 */
void I2C_Write_Data_itr(uint8_t device_addr, uint8_t data){

  cmd_data = data;

  transferSequence.addr = device_addr << 1; // shift device address left
  transferSequence.flags = I2C_FLAG_WRITE;
  transferSequence.buf[0].data = &cmd_data; // pointer to data to write
  transferSequence.buf[0].len = sizeof(cmd_data);

  // starting I2C Transfer by enabling IRQ and calling the I2C_TransferInit function
  NVIC_EnableIRQ(I2C0_IRQn);
  transferStatus = I2C_TransferInit(I2C0, &transferSequence);

  if(transferStatus < 0 ){
      LOG_ERROR("I2C_TransferInit() Write error = %d", transferStatus);
  }
}

/**
 * @brief   Received data over the I2C bus from the addressed device.
 *          Function makes use of IRQs.
 * @param   device_addr I2C device address to receive data from
 * @return  none
 */
void I2C_Read_Data_irq(uint8_t device_addr){
    transferSequence.addr = device_addr << 1; // shift device address left
    transferSequence.flags = I2C_FLAG_READ;
    transferSequence.buf[0].data = read_data; // pointer to data to write
    transferSequence.buf[0].len = sizeof(read_data);

    // starting I2C Transfer by enabling IRQ and calling the I2C_TransferInit function
    NVIC_EnableIRQ(I2C0_IRQn);
    transferStatus = I2C_TransferInit(I2C0, &transferSequence);

    if(transferStatus < 0 ){
        LOG_ERROR("I2C_TransferInit() Read error = %d", transferStatus);
    }
}

/**
 * @brief   Returns the data received from the I2C device with endian-ness corrected
 * @return  Data received from the addressed device, with endian-ness corrected
 */
uint16_t I2C_Get_Data(){

  // Converting data from big-endian to little-endian as endian-ness is
  // swapped during data transmission
  uint16_t swapped_read_data = ((read_data[0])<<8) | (read_data[1]);
      return swapped_read_data;
}

/**
 * @brief   Sends the given data over the I2C bus to the addressed device
 * @param   device_addr I2C device address to write data to
 * @param   data        Data to send over the I2C bus
 * @return  none
 */
void I2C_Write_Data(uint8_t device_addr, uint8_t data){
  I2C_TransferReturn_TypeDef transferStatus; // make this global for IRQs in A4
  I2C_TransferSeq_TypeDef transferSequence; // this one can be local
  uint8_t cmd_data; // make this global for IRQs in A4

  cmd_data = data;

  transferSequence.addr = device_addr << 1; // shift device address left
  transferSequence.flags = I2C_FLAG_WRITE;
  transferSequence.buf[0].data = &cmd_data; // pointer to data to write
  transferSequence.buf[0].len = sizeof(cmd_data);

  transferStatus = I2CSPM_Transfer (I2C0, &transferSequence);

  if (transferStatus != i2cTransferDone) {
      LOG_ERROR("I2CSPM_Transfer: I2C bus write of cmd=0x%02x failed", data);
  }
}


/**
 * @brief   Received data over the I2C bus from the addressed device
 * @param   device_addr I2C device address to receive data from
 * @return  data recieved from the addressed device, with endian-ness corrected
 */
uint16_t I2C_Read_Data(uint8_t device_addr){
    I2C_TransferReturn_TypeDef transferStatus; // make this global for IRQs in A4
    I2C_TransferSeq_TypeDef transferSequence; // this one can be local
    uint8_t read_data[2]; // make this global for IRQs in A4

    transferSequence.addr = device_addr << 1; // shift device address left
    transferSequence.flags = I2C_FLAG_READ;
    transferSequence.buf[0].data = read_data; // pointer to data to write
    transferSequence.buf[0].len = sizeof(read_data);

    transferStatus = I2CSPM_Transfer (I2C0, &transferSequence);

    if (transferStatus != i2cTransferDone) {
        LOG_ERROR("I2CSPM_Transfer: I2C read for device ID 0x%02x failed", device_addr);
    }

    // Converting data from big-endian to little-endian as endian-ness is
    // swapped during data transmission
    uint16_t swapped_read_data = ((read_data[0])<<8) | (read_data[1]);

    return swapped_read_data;
}


/**
 * @brief   Gets temperature data from the onboard Si7021 temperature sensor
 * @return  Temperatue in Celsius
 */
uint32_t I2C_Si7021_Get_Temp(){
  // Send Measure Temperature command
  uint16_t read_data; // make this global for IRQs in A4
  uint32_t temperature_reading = 0;

  // Turning on the SI7021 sensor and waiting till it is ready for conversion
  // as per the recommended waiting period given in the data sheet
  si7021TurnOn();
  timerWaitUs_polled(SI7021_POR_TIME_US);

  // Sending a temperature conversion request to the sensor
//  I2C_Write_Data(SI7021_DEVICE_ADDR, SI7021_CMD_MEASURE_TEMP_NO_HOLD);
  I2C_Write_Data_itr(SI7021_DEVICE_ADDR, SI7021_CMD_MEASURE_TEMP_NO_HOLD);

  // Waiting for the conversion to complete (waiting time given in datasheet)
  timerWaitUs_polled(SI7021_14B_CONVERSION_TIME_US);

//  read_data = I2C_Read_Data(SI7021_DEVICE_ADDR);

//  read_data = I2C_Read_Data_irq(SI7021_DEVICE_ADDR);
  I2C_Read_Data_irq(SI7021_DEVICE_ADDR);
  read_data = I2C_Get_Data();

  // Turning off the sensor to conserve energy
  si7021TurnOff();

  // Converting the data received from the sensor into temperature in Celsius
  // using the formula given in the SI7021 sensor application note AN607
  temperature_reading = ((read_data*175.72)/65536) - 46.85;

  return (temperature_reading);
}

