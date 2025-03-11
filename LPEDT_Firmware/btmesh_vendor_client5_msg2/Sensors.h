/*
 * Sensors.h
 *
 *  Created on: Dec 11, 2024
 *      Author: vishn
 */

#ifndef SENSORS_H_
#define SENSORS_H_

#include <stdint.h>

void Sensors_Init();

void Get_Temp(int *data);

void Get_Humidity(int *data);

void Get_IMU_data(int *a_x, int *a_y, int *a_z, int *g_x, int *g_y, int *g_z);

void Get_Gas(int *data);

void Get_Pressure(int *data);

void Emergency_State();

#endif /* SENSORS_H_ */
