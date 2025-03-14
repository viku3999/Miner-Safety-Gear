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
 * @file    i2c.h
 * @brief   Header file for i2c.c which contains I2C based drivers for onboard
 *          sensors
 *
 * @author  Vishnu Kumar Thoodur Venkatachalapathy
 * @date    Feb 7, 2024
 */

#ifndef SRC_I2C_H_
#define SRC_I2C_H_


#define SI7021_DEVICE_ADDR 0x40
#define SI7021_POR_TIME_US 80000
#define SI7021_14B_CONVERSION_TIME_US 10800
#define SI7021_CMD_MEASURE_TEMP_NO_HOLD 0xF3
/**
 * @brief   Initialize the I2C peripheral to work with the Si7021 sensor
 * @return  none
 */
void I2C_Init_Si7021();

/**
 * @brief   Gets temperature data from the onboard Si7021 temperature sensor
 * @return  Temperatue in Celsius
 */
uint32_t I2C_Si7021_Get_Temp();

/**
 * @brief   Sends the given data over the I2C bus to the addressed device.
 *          Function makes use of IRQs.
 * @param   device_addr I2C device address to write data to
 * @param   data        Data to send over the I2C bus
 * @return  none
 */
void I2C_Write_Data_itr(uint8_t device_addr, uint8_t data);

/**
 * @brief   Received data over the I2C bus from the addressed device.
 *          Function makes use of IRQs.
 * @param   device_addr I2C device address to receive data from
 * @return  none
 */
void I2C_Read_Data_irq(uint8_t device_addr);

/**
 * @brief   Returns the data received from the I2C device with endian-ness corrected
 * @return  Data received from the addressed device, with endian-ness corrected
 */
uint16_t I2C_Get_Data();

void BMI270_Get_Chip_Id();

void I2C_Init_BMI270();

void BME688_Get_Chip_Id();

#endif /* SRC_I2C_H_ */
