#include "bme688.h"
#include <string.h>
#include "sl_sleeptimer.h"
#include <math.h>

// BME688 register addresses
#define BME688_REG_CHIP_ID      0xD0
#define BME688_REG_CTRL_HUM     0x72
#define BME688_REG_CTRL_MEAS    0x74
#define BME688_REG_HUMIDITY_MSB 0x25
#define BME688_EXPECTED_CHIP_ID 0x61 // Expected CHIP_ID for BME688
// Humidity Registers
#define BME688_PAR_H1_LSB_REG  0xE2
#define BME688_PAR_H1_MSB_REG  0xE3
#define BME688_PAR_H2_LSB_REG  0xE2
#define BME688_PAR_H2_MSB_REG  0xE1
#define BME688_PAR_H3_REG      0xE4
#define BME688_PAR_H4_REG      0xE5
#define BME688_PAR_H5_REG      0xE6
#define BME688_PAR_H6_REG      0xE7
#define BME688_PAR_H7_REG      0xE8
// Pressure Registers
#define BME688_PAR_P1_LSB_REG  0x8E
#define BME688_PAR_P1_MSB_REG  0x8F
#define BME688_PAR_P2_LSB_REG  0x90
#define BME688_PAR_P2_MSB_REG  0x91
#define BME688_PAR_P3_REG      0x92
#define BME688_PAR_P4_LSB_REG  0x94
#define BME688_PAR_P4_MSB_REG  0x95
#define BME688_PAR_P5_LSB_REG  0x96
#define BME688_PAR_P5_MSB_REG  0x97
#define BME688_PAR_P6_REG      0x99
#define BME688_PAR_P7_REG      0x98
#define BME688_PAR_P8_LSB_REG  0x9C
#define BME688_PAR_P8_MSB_REG  0x9D
#define BME688_PAR_P9_LSB_REG  0x9E
#define BME688_PAR_P9_MSB_REG  0x9F
#define BME688_PAR_P10_REG     0xA0
#define BME688_PRESSURE_MSB_REG 0x1F
// Gas Measurement Registers
#define GAS_R_MSB_REG 0x2C
#define GAS_R_LSB_REG 0x2D
#define GAS_RANGE_REG 0x2D

/**************************************************************************//**
 * @brief Reads a register from the BME688 sensor.
 *****************************************************************************/
sl_status_t sl_bme688_read_register(sl_i2cspm_t *i2cspm, uint8_t addr, uint8_t reg, uint8_t *data, size_t len)
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

/**************************************************************************//**
 * @brief Writes to a register on the BME688 sensor.
 *****************************************************************************/
sl_status_t sl_bme688_write_register(sl_i2cspm_t *i2cspm, uint8_t addr, uint8_t reg, const uint8_t *data, size_t len)
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

/**************************************************************************//**
 * @brief Initializes the BME688 sensor.
 *****************************************************************************/
sl_status_t sl_bme688_init(sl_i2cspm_t *i2cspm, uint8_t addr)
{
    uint8_t chip_id;
    sl_status_t status;

    // Check if the sensor is present
    if (!sl_bme688_present(i2cspm, addr, &chip_id)) {
        sl_sleeptimer_delay_millisecond(10); // Delay for sensor initialization
        if (!sl_bme688_present(i2cspm, addr, &chip_id)) {
            return SL_STATUS_INITIALIZATION;
        }
    }

    // Check CHIP_ID
    if (chip_id != BME688_EXPECTED_CHIP_ID) {
        return SL_STATUS_INITIALIZATION;
    }

    // Configure humidity oversampling to 1x
    uint8_t ctrl_hum = 0x01;
    status = sl_bme688_write_register(i2cspm, addr, BME688_REG_CTRL_HUM, &ctrl_hum, 1);
    if (status != SL_STATUS_OK) {
        return status;
    }

    // Configure temperature, pressure oversampling, and set to normal mode
    uint8_t ctrl_meas = 0x27; // Temp and pressure: 1x oversampling, normal mode
    status = sl_bme688_write_register(i2cspm, addr, BME688_REG_CTRL_MEAS, &ctrl_meas, 1);
    if (status != SL_STATUS_OK) {
        return status;
    }

    return SL_STATUS_OK;
}

/**************************************************************************//**
 * @brief Checks if a BME688 is present on the I2C bus.
 *****************************************************************************/
bool sl_bme688_present(sl_i2cspm_t *i2cspm, uint8_t addr, uint8_t *chip_id)
{
    uint8_t read_data = 0;
    sl_status_t status = sl_bme688_read_register(i2cspm, addr, BME688_REG_CHIP_ID, &read_data, 1);
    if (status != SL_STATUS_OK) {
        return false;
    }

    if (chip_id) {
        *chip_id = read_data;
    }

    return (read_data == BME688_EXPECTED_CHIP_ID);
}

// Function to read the calibration parameters
sl_status_t sl_bme688_read_calibration_params(sl_i2cspm_t *i2cspm, uint8_t addr, int16_t *par_h1, int16_t *par_h2,
                                               int8_t *par_h3, int8_t *par_h4, int8_t *par_h5, uint8_t *par_h6, uint8_t *par_h7)
{
    uint8_t data[2];

    // Read par_h1
    sl_bme688_read_register(i2cspm, addr, BME688_PAR_H1_LSB_REG, &data[0], 1);
    sl_bme688_read_register(i2cspm, addr, BME688_PAR_H1_MSB_REG, &data[1], 1);
    *par_h1 = (((int16_t)data[1]) << 4) | (((int16_t)data[0]) & 0x000F);

    // Read par_h2
    sl_bme688_read_register(i2cspm, addr, BME688_PAR_H2_LSB_REG, &data[0], 1);
    sl_bme688_read_register(i2cspm, addr, BME688_PAR_H2_MSB_REG, &data[1], 1);
    *par_h2 = (((int16_t)data[1]) << 4) | (int16_t)((data[0] >> 4));

    // Read par_h3
    sl_bme688_read_register(i2cspm, addr, BME688_PAR_H3_REG, (uint8_t *)par_h3, 1);

    // Read par_h4
    sl_bme688_read_register(i2cspm, addr, BME688_PAR_H4_REG, (uint8_t *)par_h4, 1);

    // Read par_h5
    sl_bme688_read_register(i2cspm, addr, BME688_PAR_H5_REG, (uint8_t *)par_h5, 1);

    // Read par_h6
    sl_bme688_read_register(i2cspm, addr, BME688_PAR_H6_REG, par_h6, 1);

    // Read par_h7
    sl_bme688_read_register(i2cspm, addr, BME688_PAR_H7_REG, par_h7, 1);

    return SL_STATUS_OK;
}

// Function to compute compensated humidity
sl_status_t sl_bme688_compute_humidity(sl_i2cspm_t *i2cspm, uint8_t addr, int16_t humidity_raw, double temp_comp, double *humidity_percent)
{
    int16_t par_h1, par_h2;
    int8_t par_h3, par_h4, par_h5;
    uint8_t par_h6, par_h7;
    sl_status_t status;

    // Read calibration parameters
    status = sl_bme688_read_calibration_params(i2cspm, addr, &par_h1, &par_h2, &par_h3, &par_h4, &par_h5, &par_h6, &par_h7);
    if (status != SL_STATUS_OK) {
        return status;
    }

    // Compensated humidity calculation
    double var1 = humidity_raw - (((double)par_h1 * 16.0) + (((double)par_h3 / 2.0) * temp_comp));
    double var2 = var1 * (((double)par_h2 / 262144.0) * (1.0 + (((double)par_h4 / 16384.0) * temp_comp) +
                                               (((double)par_h5 / 1048576.0) * temp_comp * temp_comp)));
    double var3 = (double)par_h6 / 16384.0;
    double var4 = (double)par_h7 / 2097152.0;

    *humidity_percent = (double)(var2 + ((var3 + (var4 * temp_comp)) * var2 * var2))*3;

    // Clamp humidity to valid range (0% to 100%)
    if (*humidity_percent > 100.0) {
        *humidity_percent = 100.0;
    } else if (*humidity_percent < 0.0) {
        *humidity_percent = 0.0;
    }

    return SL_STATUS_OK;
}

/**************************************************************************//**
 * @brief Reads the humidity from the BME688 sensor.
 *****************************************************************************/
sl_status_t sl_bme688_read_humidity(sl_i2cspm_t *i2cspm, uint8_t addr, double *humidity_data)
{
    uint8_t raw_data[2] = {0};
    sl_status_t status = sl_bme688_read_register(i2cspm, addr, BME688_REG_HUMIDITY_MSB, raw_data, 2);

    if (status != SL_STATUS_OK) {
        return status;
    }

    // Combine MSB and LSB to form the raw humidity value
    int16_t humidity_raw = (int16_t)((raw_data[0] << 8) | raw_data[1]);

    // Compute compensated humidity
    return sl_bme688_compute_humidity(i2cspm, addr, humidity_raw, 21.00, humidity_data);  // Example temperature compensation value
}

/**************************************************************************//**
 * @brief Reads the pressure from the BME688 sensor.
 *****************************************************************************/
static sl_status_t read_pressure_calibration_params(sl_i2cspm_t *i2cspm, uint8_t addr,
                                                    uint16_t *par_p1, int16_t *par_p2,
                                                    int8_t *par_p3, int16_t *par_p4,
                                                    int16_t *par_p5, int8_t *par_p6,
                                                    int8_t *par_p7, int16_t *par_p8,
                                                    int16_t *par_p9, int8_t *par_p10)
{
    uint8_t data[2];

    // Read par_p1
    sl_bme688_read_register(i2cspm, addr, BME688_PAR_P1_LSB_REG, &data[0], 1);
    sl_bme688_read_register(i2cspm, addr, BME688_PAR_P1_MSB_REG, &data[1], 1);
    *par_p1 = (uint16_t)((data[1] << 8) | data[0]);

    // Read par_p2
    sl_bme688_read_register(i2cspm, addr, BME688_PAR_P2_LSB_REG, &data[0], 1);
    sl_bme688_read_register(i2cspm, addr, BME688_PAR_P2_MSB_REG, &data[1], 1);
    *par_p2 = (int16_t)((data[1] << 8) | data[0]);

    // Read other parameters
    sl_bme688_read_register(i2cspm, addr, BME688_PAR_P3_REG, (uint8_t *)par_p3, 1);
    sl_bme688_read_register(i2cspm, addr, BME688_PAR_P4_LSB_REG, &data[0], 1);
    sl_bme688_read_register(i2cspm, addr, BME688_PAR_P4_MSB_REG, &data[1], 1);
    *par_p4 = (int16_t)((data[1] << 8) | data[0]);

    sl_bme688_read_register(i2cspm, addr, BME688_PAR_P5_LSB_REG, &data[0], 1);
    sl_bme688_read_register(i2cspm, addr, BME688_PAR_P5_MSB_REG, &data[1], 1);
    *par_p5 = (int16_t)((data[1] << 8) | data[0]);

    sl_bme688_read_register(i2cspm, addr, BME688_PAR_P6_REG, (uint8_t *)par_p6, 1);
    sl_bme688_read_register(i2cspm, addr, BME688_PAR_P7_REG, (uint8_t *)par_p7, 1);

    sl_bme688_read_register(i2cspm, addr, BME688_PAR_P8_LSB_REG, &data[0], 1);
    sl_bme688_read_register(i2cspm, addr, BME688_PAR_P8_MSB_REG, &data[1], 1);
    *par_p8 = (int16_t)((data[1] << 8) | data[0]);

    sl_bme688_read_register(i2cspm, addr, BME688_PAR_P9_LSB_REG, &data[0], 1);
    sl_bme688_read_register(i2cspm, addr, BME688_PAR_P9_MSB_REG, &data[1], 1);
    *par_p9 = (int16_t)((data[1] << 8) | data[0]);

    sl_bme688_read_register(i2cspm, addr, BME688_PAR_P10_REG, (uint8_t *)par_p10, 1);

    return SL_STATUS_OK;
}

// Compute Compensated Pressure
sl_status_t sl_bme688_compute_pressure(sl_i2cspm_t *i2cspm, uint8_t addr, int32_t press_raw, float t_fine, double *press_comp)
{
    uint16_t par_p1;
    int16_t par_p2, par_p4, par_p5, par_p8, par_p9;
    int8_t par_p3, par_p6, par_p7, par_p10;

    sl_status_t status = read_pressure_calibration_params(i2cspm, addr, &par_p1, &par_p2, &par_p3, &par_p4,
                                                          &par_p5, &par_p6, &par_p7, &par_p8, &par_p9, &par_p10);
    if (status != SL_STATUS_OK) {
        return status;
    }

    double var1 = ((double)t_fine / 2.0) - 64000.0;
    double var2 = var1 * var1 * ((double)par_p6 / 131072.0);
    var2 += var1 * (double)par_p5 * 2.0;
    var2 = (var2 / 4.0) + ((double)par_p4 * 65536.0);
    var1 = (((double)par_p3 * var1 * var1 / 16384.0) + ((double)par_p2 * var1)) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * (double)par_p1;

    if (var1 == 0) {
        return SL_STATUS_FAIL; // Avoid division by zero
    }

    double press = 1048576.0 - (double)press_raw;
    press = (press - (var2 / 4096.0)) * 6250.0 / var1;
    var1 = ((double)par_p9 * press * press) / 2147483648.0;
    var2 = press * ((double)par_p8 / 32768.0);
    double var3 = (press / 256.0) * (press / 256.0) * (press / 256.0) * ((double)par_p10 / 131072.0);

    *press_comp = press + (var1 + var2 + var3 + ((double)par_p7 * 128.0)) / 16.0;

    return SL_STATUS_OK;
}

/**************************************************************************//**
 * @brief Measure the gas readout data from the BME688 sensor.
 *****************************************************************************/
sl_status_t sl_bme688_read_gas_resistance(sl_i2cspm_t *i2cspm, uint8_t addr, double *gas_res)
{
    uint8_t gas_data[4]; // To hold MSB and LSB for each field
    uint8_t gas_range;
    uint32_t gas_adc;
    sl_status_t status;

    // Read gas ADC MSB and LSB (Fields 0, 1, 2)
    status = sl_bme688_read_register(i2cspm, addr, 0x2C, &gas_data[0], 1); // MSB for Field 0
    if (status != SL_STATUS_OK) {
        return status;
    }
    status = sl_bme688_read_register(i2cspm, addr, 0x2D, &gas_data[1], 1); // LSB for Field 0
    if (status != SL_STATUS_OK) {
        return status;
    }
    status = sl_bme688_read_register(i2cspm, addr, 0x3D, &gas_data[2], 1); // MSB for Field 1
    if (status != SL_STATUS_OK) {
        return status;
    }
    status = sl_bme688_read_register(i2cspm, addr, 0x3E, &gas_data[3], 1); // LSB for Field 1
    if (status != SL_STATUS_OK) {
        return status;
    }

    // Read gas range (Fields 0, 1, 2 for gas_range)
    status = sl_bme688_read_register(i2cspm, addr, 0x4E, &gas_range, 1); // gas_range Field 0
    if (status != SL_STATUS_OK) {
        return status;
    }

    // Extract gas ADC (10-bit value, combining MSB and LSB based on Field positions)
    gas_adc = ((uint32_t)(gas_data[0] << 6) | ((gas_data[1] & 0xC0) >> 6)); // Field 0 <7:6>
    gas_adc |= ((uint32_t)(gas_data[2] << 6) | ((gas_data[3] & 0xC0) >> 6)); // Field 1 <7:6>

    // Extract gas range (lower nibble)
    gas_range &= 0x0F;

    // Calculate gas resistance (Ohms) using the provided formula
    uint32_t var1 = (UINT32_C(262144)) >> gas_range;  // var1 = 262144 >> gas_range
    int32_t var2 = ((int32_t)gas_adc) - INT32_C(512); // var2 = gas_adc - 512
    var2 *= INT32_C(3);                               // var2 = var2 * 3
    var2 += INT32_C(4096);                            // var2 = var2 + 4096

    *gas_res = 1000000.0f * ((float)var1 / (float)var2); // gas_res = 1000000 * var1 / var2

    return SL_STATUS_OK;
}

