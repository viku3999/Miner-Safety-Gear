/** @file bmi270.c **/
#include <stddef.h>
#include "bmi270.h"
#include "bmi270_config.h"
#include "sl_i2cspm.h"
#include "sl_sleeptimer.h"
#include <string.h>

/** BMI270 Commands */
#define READ_CHIP_ID 0x00
#define READ_ACCEL_GYRO_START_ADDR 0x12
#define PWR_CONF_REG_ADDR 0x7C
#define PWR_CTRL_REG_ADDR 0x7D
#define ACC_CONF_REG_ADDR 0x40
#define GYR_CONF_REG_ADDR 0x42
#define BST_READ_REG_ADDR 0x0C
#define ACC_RANGE_REG_ADDR 0x41
#define GYR_RANGE_REG_ADDR 0x43
#define INIT_CTRL_REG_ADDR 0x59
#define INIT_DATA_REG_ADDR 0x5E
#define INIT_ADDR_0_REG_ADDR 0x5B
#define INIT_ADDR_1_REG_ADDR 0x5C
#define CONFIG_FILE_SIZE     8192
#define CHUNK_SIZE           2048
#define INTERNAL_STATUS_REG_ADDR 0x21

#define CHIP_ID_EXPECTED 0x24

int cnt = 0;

sl_status_t sl_bmi270_read_register(sl_i2cspm_t *i2cspm, uint8_t addr, uint8_t reg, uint8_t *data, size_t len);
sl_status_t sl_bmi270_write_register(sl_i2cspm_t *i2cspm, uint8_t addr, uint8_t reg, const uint8_t *data, size_t len);
sl_status_t sl_bmi270_write_init_register (sl_i2cspm_t *i2cspm, uint8_t addr, uint8_t *data, size_t len);
/***************************************************************************//**
 *    Initializes the BMI270 sensor
 ******************************************************************************/
sl_status_t sl_bmi270_init(sl_i2cspm_t *i2cspm, uint8_t addr, int16_t *acc_gyr_data)
{
//    uint8_t chip_id;
    sl_status_t status;

//    // Check if sensor is present
//    if (!sl_bmi270_present(i2cspm, addr, &chip_id)) {
//        sl_sleeptimer_delay_millisecond(80); // Delay for sensor to initialize
//        if (!sl_bmi270_present(i2cspm, addr, &chip_id)) {
//            return SL_STATUS_INITIALIZATION; // Initialization failed
//        }
//    }
//
//    // Check CHIP_ID
//    if (chip_id != CHIP_ID_EXPECTED) {
//        return SL_STATUS_INITIALIZATION; // Incorrect chip ID
//    }

    // Disable adv_power_save bit
    uint8_t config_data[] = {0x00};
    status = sl_bmi270_write_register(i2cspm, addr, PWR_CONF_REG_ADDR, config_data, 1);
    if (status != SL_STATUS_OK) {
        return status;
    }

    sl_sleeptimer_delay_millisecond(1);

    // Prepare config load INIT_CTRL = 0x00
    config_data[0] = 0x00;
    status = sl_bmi270_write_register(i2cspm, addr, INIT_CTRL_REG_ADDR, config_data, 1);
    if (status != SL_STATUS_OK) {
        return status;
    }

    // Burst Write config to INIT_DATA
//    status = sl_bmi270_load_config(i2cspm, addr, bmi270_config_file);
//    if (status != SL_STATUS_OK) {
//        return status;
//    }

    status = sl_bmi270_write_init_register(i2cspm, addr, bmi270_config_file, 8193); // register added to conf file
    if (status != SL_STATUS_OK) {
        return status;
    }

    // Complete Config Load
    config_data[0] = 0x01;
    status = sl_bmi270_write_register(i2cspm, addr, INIT_CTRL_REG_ADDR, config_data, 1);
    if (status != SL_STATUS_OK) {
        return status;
    }

    uint8_t internal_status;
    status = sl_bmi270_read_register(i2cspm, addr, INTERNAL_STATUS_REG_ADDR, &internal_status, 1);
    if (internal_status != 0x01) {
        return SL_STATUS_FAIL; // Configuration incomplete
    }

    // Configure perfomnce mode
    config_data[0] = 0x0E;
    status = sl_bmi270_write_register(i2cspm, addr, PWR_CTRL_REG_ADDR, config_data, 1);
    if (status != SL_STATUS_OK) {
        return status;
    }

    // Accelerometer
    config_data[0] = 0xA8; // set acc_odr to 100 Hz
    status = sl_bmi270_write_register(i2cspm, addr, ACC_CONF_REG_ADDR, config_data, 1);
    if (status != SL_STATUS_OK) {
        return status;
    }

    // Gyroscope
    config_data[0] = 0xE9; // set acc_odr to 100 Hz
    status = sl_bmi270_write_register(i2cspm, addr, GYR_CONF_REG_ADDR, config_data, 1);
    if (status != SL_STATUS_OK) {
        return status;
    }

    // Enable the adv_power_save bit and leave the fifo_self_wakeup enables
    config_data[0] = 0x02; // set acc_odr to 50 Hz
    status = sl_bmi270_write_register(i2cspm, addr, PWR_CONF_REG_ADDR, config_data, 1);
    if (status != SL_STATUS_OK) {
        return status;
    }

    uint8_t acc_data[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    sl_bmi270_read_register(i2cspm, addr, BST_READ_REG_ADDR, acc_data, 12);

    acc_gyr_data[0] = (int16_t)(acc_data[1] << 8) | (int16_t)(acc_data[0]);
    acc_gyr_data[1] = (int16_t)(acc_data[3] << 8) | (int16_t)(acc_data[2]);
    acc_gyr_data[2] = (int16_t)(acc_data[5] << 8) | (int16_t)(acc_data[4]);

    acc_gyr_data[3] = (int16_t)(acc_data[7] << 8) | (int16_t)(acc_data[6]);
    acc_gyr_data[4] = (int16_t)(acc_data[9] << 8) | (int16_t)(acc_data[8]);
    acc_gyr_data[5] = (int16_t)(acc_data[11] << 8) | (int16_t)(acc_data[10]);

    return SL_STATUS_OK;
}

/***************************************************************************//**
 *   Checks if a BMI270 is present on the I2C bus or not.
 ******************************************************************************/
bool sl_bmi270_present(sl_i2cspm_t *i2cspm, uint8_t addr, uint8_t *chip_id)
{
    uint8_t read_data = 0;
    sl_status_t status = sl_bmi270_read_register(i2cspm, addr, READ_CHIP_ID, &read_data, 1);
    if (status != SL_STATUS_OK) {
        return false;
    }
    if (chip_id) {
        *chip_id = read_data;
    }
    return (read_data == CHIP_ID_EXPECTED); // Expected CHIP_ID for BMI270
}

/***************************************************************************//**
 *   Reads accelerometer data from the BMI270 sensor
 ******************************************************************************/
sl_status_t sl_bmi270_read_acc_gyr(sl_i2cspm_t *i2cspm, uint8_t addr,
                                 int16_t *acc_gyr_data)
{
  uint8_t acc_data[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  sl_bmi270_read_register(i2cspm, addr, BST_READ_REG_ADDR, acc_data, 12);

  acc_gyr_data[0] = (int16_t)(acc_data[1] << 8) | (int16_t)(acc_data[0]);
  acc_gyr_data[1] = (int16_t)(acc_data[3] << 8) | (int16_t)(acc_data[2]);
  acc_gyr_data[2] = (int16_t)(acc_data[5] << 8) | (int16_t)(acc_data[4]);

  acc_gyr_data[3] = (int16_t)(acc_data[7] << 8) | (int16_t)(acc_data[6]);
  acc_gyr_data[4] = (int16_t)(acc_data[9] << 8) | (int16_t)(acc_data[8]);
  acc_gyr_data[5] = (int16_t)(acc_data[11] << 8) | (int16_t)(acc_data[10]);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 *   Helper function to read a register from BMI270
 ******************************************************************************/
sl_status_t sl_bmi270_read_register(sl_i2cspm_t *i2cspm, uint8_t addr, uint8_t reg,
                                    uint8_t *data, size_t len)
{
    I2C_TransferSeq_TypeDef seq;
    seq.addr = addr << 1;
    seq.flags = I2C_FLAG_WRITE_READ;

    uint8_t reg_addr = reg;
    seq.buf[0].data = &reg_addr;
    seq.buf[0].len = 1;
    seq.buf[1].data = data;
    seq.buf[1].len = len;

    if (I2CSPM_Transfer(i2cspm, &seq) != i2cTransferDone) {
        return SL_STATUS_TRANSMIT;
    }
    return SL_STATUS_OK;
}

/***************************************************************************//**
 *   Helper function to write to a register of BMI270
 ******************************************************************************/
sl_status_t sl_bmi270_write_register(sl_i2cspm_t *i2cspm, uint8_t addr, uint8_t reg,
                                     const uint8_t *data, size_t len)
{
    I2C_TransferSeq_TypeDef seq;
    seq.addr = addr << 1;
    seq.flags = I2C_FLAG_WRITE;

    uint8_t buf[len + 1];
    buf[0] = reg;
    memcpy(&buf[1], data, len);

    seq.buf[0].data = buf;
    seq.buf[0].len = len + 1;

    if (I2CSPM_Transfer(i2cspm, &seq) != i2cTransferDone) {
        return SL_STATUS_TRANSMIT;
    }
    return SL_STATUS_OK;
}

/***************************************************************************//**
 *   Helper function to write to a register of BMI270
 ******************************************************************************/
sl_status_t sl_bmi270_write_init_register(sl_i2cspm_t *i2cspm, uint8_t addr,
                                     uint8_t *data, size_t len)
{
    I2C_TransferSeq_TypeDef seq;
    seq.addr = addr << 1;
    seq.flags = I2C_FLAG_WRITE;

    seq.buf[0].data = data;
    seq.buf[0].len = len;

    if (I2CSPM_Transfer(i2cspm, &seq) != i2cTransferDone) {
        return SL_STATUS_TRANSMIT;
    }
    return SL_STATUS_OK;
}
