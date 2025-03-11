/***************************************************************************//**
 * @file bme688.h
 * @brief Driver for the BMI270 Inertial Measurement Unit sensor
 *
 ******************************************************************************/

#ifndef BME688_H
#define BME688_H

#include <stdbool.h>
#include "sl_status.h"
#include "sl_i2cspm.h"

/***************************************************************************//**
 * @addtogroup BME688 - Gas Sensor
 * @brief Bosch BME688 Gas Sensor I2C driver.
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
* @name Sensor Defines
******************************************************************************/
/** I2C device address for BME688 */
#define BME688_ADDR 0x76

/**************************************************************************//**
 * @brief
 *   Initialize the bme688 sensor.
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 * @param[in] addr
 *   The I2C address to probe.
 * @retval SL_STATUS_OK An bme688 device is present on the I2C bus
 * @retval SL_STATUS_INITIALIZATION No bme688 device present
 *****************************************************************************/
sl_status_t sl_bme688_init(sl_i2cspm_t *i2cspm, uint8_t addr);

/**************************************************************************//**
 * @brief
 *   Check whether an bme688 is present on the I2C bus or not.
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 * @param[in] addr
 *   The I2C address to probe.
 * @param[out] chip_id
 *   Write chip ID from SNB_3 if device responds. Pass in NULL to discard.
 *
 * @retval true An bme688 device is present on the I2C bus
 * @retval false No bme688 device present
 *****************************************************************************/
bool sl_bme688_present(sl_i2cspm_t *i2cspm, uint8_t addr, uint8_t *device_id);

/**************************************************************************//**
 * @brief
 *  Measure humidity, detect gas from an bme688 sensor.
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 * @param[in] addr
 *   The I2C address of the sensor.
 * @param[out] humidity_data
 *   The
 * @param[out] posData
 *   The position in .
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmission error
 *****************************************************************************/
sl_status_t sl_bme688_read_humidity(sl_i2cspm_t *i2cspm, uint8_t addr,
                                      double *humidity_data);

sl_status_t sl_bme688_compute_pressure(sl_i2cspm_t *i2cspm, uint8_t addr,
                                       int32_t press_raw, float t_fine, double *press_comp);

sl_status_t sl_bme688_read_gas_resistance(sl_i2cspm_t *i2cspm, uint8_t addr, double *gas_res);


#ifdef __cplusplus
}
#endif

#endif /* BMI270_H */
