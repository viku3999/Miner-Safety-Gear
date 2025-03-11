/***************************************************************************//**
 * @file
 * @brief Driver for the BMI270 Inertial Measurement Unit sensor
 *
 ******************************************************************************/

#ifndef BMI270_H
#define BMI270_H

#include <stdbool.h>
#include "sl_status.h"
#include "sl_i2cspm.h"

/***************************************************************************//**
 * @addtogroup BMI270 - IMU Sensor
 * @brief Bosch BMI270 IMU Sensor I2C driver.
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
* @name Sensor Defines
******************************************************************************/
/** I2C device address for BMI270 */
#define BMI270_ADDR      0X68

/** Chip ID value for BMI270 */
#define BMI270_CHIP_ID 0x24

/**************************************************************************//**
 * @brief
 *   Initialize the bmi270 sensor.
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 * @param[in] addr
 *   The I2C address to probe.
 * @retval SL_STATUS_OK An bmi270 device is present on the I2C bus
 * @retval SL_STATUS_INITIALIZATION No bmi270 device present
 *****************************************************************************/
sl_status_t sl_bmi270_init(sl_i2cspm_t *i2cspm, uint8_t addr, int16_t *acc_gyr_data);

/**************************************************************************//**
 * @brief
 *   Check whether an bmi270 is present on the I2C bus or not.
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 * @param[in] addr
 *   The I2C address to probe.
 * @param[out] chip_id
 *   Write chip ID from SNB_3 if device responds. Pass in NULL to discard.
 *   Should be 0x24 for bmi270
 * @retval true An bmi270 device is present on the I2C bus
 * @retval false No bmi270 device present
 *****************************************************************************/
bool sl_bmi270_present(sl_i2cspm_t *i2cspm, uint8_t addr, uint8_t *device_id);

/**************************************************************************//**
 * @brief
 *  Measure inertial state from an bmi270 sensor.
 * @param[in] i2cspm
 *   The I2C peripheral to use.
 * @param[in] addr
 *   The I2C address of the sensor.
 * @param[out] imuData
 *   The
 * @param[out] posData
 *   The position in .
 * @retval SL_STATUS_OK Success
 * @retval SL_STATUS_TRANSMIT I2C transmission error
 *****************************************************************************/
sl_status_t sl_bmi270_read_acc_gyr(sl_i2cspm_t *i2cspm, uint8_t addr,
                                 int16_t *acc_gyr_data);

#ifdef __cplusplus
}
#endif

#endif /* BMI270_CONFIG_H */
