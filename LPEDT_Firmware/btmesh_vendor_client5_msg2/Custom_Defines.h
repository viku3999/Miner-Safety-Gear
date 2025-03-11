/*
 * Custom_Defines.h
 *
 *  Created on: Dec 9, 2024
 *      Author: vishn
 */

#ifndef CUSTOM_DEFINES_H_
#define CUSTOM_DEFINES_H_

// The following parameters can be changed
#define NEED_LCD 0

#define CLIENT_SLEEP_TIME_MS  400
#define SERVER_SLEEP_TIME_MS  10000

#define RSSI_THREASHOLD -70

// The following parameters should not be changed

#define RSSI_DATA_LENGTH            1
#define EMERGENCY_STATE_DATA_LENGTH 1

#define TEMP_MAX 50
#define HUM_MAX 75
#define GAS_MAX 10
#define PRESSURE_MAX 1700

#define ACC_Z_MIN -3500
#define ACC_Z_MAX 3500


#endif /* CUSTOM_DEFINES_H_ */
