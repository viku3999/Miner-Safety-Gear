/*
 * Sensors.c
 *
 *  Created on: Dec 11, 2024
 *      Author: vishn
 */

#include "app_log.h"
#include "sl_status.h"


#include "sl_sensor_rht.h"
#include "sl_health_thermometer.h"
#include "bmi270.h"
#include "bme688.h"
#include "sl_i2cspm_sensor_config.h"
#include "sl_pwm_instances.h"
#include "sl_simple_led_instances.h"

void Sensors_Init(){
  // Init temperature sensor.
    sl_status_t sc;

//    sc = sl_sensor_rht_init();
    sl_sensor_rht_init();

//    if (sc == SL_STATUS_OK) {
      app_log("Relative Humidity and Temperature sensor initialized.\n\r");
//      app_log_nl();
//    }

    sc = sl_bme688_init(SL_I2CSPM_SENSOR_PERIPHERAL, BME688_ADDR);
    if (sc == SL_STATUS_OK) {
      app_log_warning("Gas sensor initialized.\n\r");
      app_log_nl();
    }

    int16_t acc_gyr_data[6] = {0, 0, 0, 0, 0, 0};
    sc = sl_bmi270_init(SL_I2CSPM_SENSOR_PERIPHERAL, BMI270_ADDR, acc_gyr_data);
    if (sc == SL_STATUS_OK) {
      app_log_warning("IMU sensor initialized.\n\r");
      app_log_nl();
    }

    app_log_info("Accel: X=%d, Y=%d, Z=%d\n\r", acc_gyr_data[0], acc_gyr_data[1], acc_gyr_data[2]);
    app_log_info("Gyro: X=%d, Y=%d, Z=%d\n\r", acc_gyr_data[3], acc_gyr_data[4], acc_gyr_data[5]);

    app_log_warning("Initialization Complete....\n\r");
}

void Get_Temp(int *data){
//  *data = 10;
  sl_status_t sc;
  int32_t temperature = 0;
  uint32_t humidity = 0;
  float tmp_c = 0.0;
  // float tmp_f = 0.0;

  // Measure temperature; units are % and milli-Celsius.
  sc = sl_sensor_rht_get(&humidity, &temperature);

  if (SL_STATUS_NOT_INITIALIZED == sc) {
    app_log_info("Relative Humidity and Temperature sensor is not initialized.");
    app_log_nl();
  }
  else if (sc != SL_STATUS_OK) {
    app_log_warning("Invalid RHT reading: %lu %ld\n\r", humidity, temperature);
  }

  tmp_c = (float)temperature / 1000;
  *data = tmp_c;

  app_log_info("Temperature: %5.2f C\n\r", tmp_c);
}

void Get_Humidity(int *data){
//  *data = 22;

  sl_status_t sc;
  double humidity_percent = 0.0;
  sc = sl_bme688_read_humidity(SL_I2CSPM_SENSOR_PERIPHERAL, BME688_ADDR, &humidity_percent);
  if (sc != SL_STATUS_OK) {
      app_log_warning("Failed to read humidity data\n\r");
  } else {
      app_log_info("Humidity: %.2lf%%\n\r", humidity_percent);
      *data = humidity_percent;
  }
}

void Get_IMU_data(int *a_x, int *a_y, int *a_z, int *g_x, int *g_y, int *g_z){
//  *a_x = 76;
//  *a_y = 77;
//  *a_z = 78;
//  *g_x = 88;
//  *g_y = 89;
//  *g_z = 90;

  sl_status_t sc;
  int16_t acc_gyr_data[6] = {0, 0, 0, 0, 0, 0};
  sc = sl_bmi270_read_acc_gyr(SL_I2CSPM_SENSOR_PERIPHERAL, BMI270_ADDR, acc_gyr_data);

  if (sc != SL_STATUS_OK) {
      app_log_warning("Failed to read IMU data\n\r");
  }
  else {
    // Print accelerometer data
    app_log_info("Accel: X=%d, Y=%d, Z=%d\n\r", acc_gyr_data[0], acc_gyr_data[1], acc_gyr_data[2]);
    app_log_info("Gyro: X=%d, Y=%d, Z=%d\n\r", acc_gyr_data[3], acc_gyr_data[4], acc_gyr_data[5]);
    *a_x = acc_gyr_data[0];
    *a_y = acc_gyr_data[1];
    *a_z = acc_gyr_data[2];
    *g_x = acc_gyr_data[3];
    *g_y = acc_gyr_data[4];
    *g_z = acc_gyr_data[5];
  }

}

void Get_Gas(int *data){
  sl_status_t sc;
  double gas_readout = 0.0;
  sc = sl_bme688_read_gas_resistance(SL_I2CSPM_SENSOR_PERIPHERAL, BME688_ADDR, &gas_readout);
  if (sc == SL_STATUS_OK) {
      app_log_info("Gas Readout: %.2lf\n\r", gas_readout);
  } else {
      app_log_warning("Failed to read gas readout data\n\r");
      *data = gas_readout;
  }
}

void Get_Pressure(int *data){
  sl_status_t sc;
  int32_t press_raw = 0;  // Replace with actual raw pressure reading
  double press_comp;
  float tmp_c = 0.0;

  sc = sl_bme688_compute_pressure(SL_I2CSPM_SENSOR_PERIPHERAL, BME688_ADDR, press_raw, tmp_c, &press_comp);
  if (sc == SL_STATUS_OK) {
      app_log_info("Pressure: %.6f hPa\n\r", press_comp/100.0);
      *data = (press_comp/100.0);
  } else {
      app_log_warning("Failed to read pressure data\n\r");
  }
}

void Emergency_State(){
  while(1){
      sl_led_turn_on(SL_SIMPLE_LED_INSTANCE(0));
      app_log_info("LED on.\n\r");
      sl_pwm_set_duty_cycle(&sl_pwm_buzzer_msg, 50);
      sl_pwm_start(&sl_pwm_buzzer_msg);
      app_log_info("Buzzer on.\n\r");
  }
}
