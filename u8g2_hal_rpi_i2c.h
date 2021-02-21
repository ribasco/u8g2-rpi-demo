/**
 *  This file provides inerfaces for U8g2 HAL implementation for Raspberry Pi I2C.
 *  @brief U8g2 HAL implementation for Raspberry Pi I2C
 *  @file
 *  @author Tomoyuki Nakashima
*/

#ifndef U8G2DEMO_U8G2_HAL_RPI_I2C_H
#define U8G2DEMO_U8G2_HAL_RPI_I2C_H

// u8g2
#include <u8g2.h>

/**
 *  The struct provides I2C Configuration Parameters.
 *  @brief I2C Configuration Parameters
 */
struct u8g2_hal_rpi_i2c_config_t {
    const char* ifname; ///< I2C interface name, e.g., "/dev/i2c-1"
    int target_addr; ///< I2C slave address for the target device
    int debug; ///< 1 if debug output is required
    int verbose; ///< 1 if verbose output is required
};

/**
 *  Set I2C Configuration.
 *  @brief Set I2C Configuration
 *  @param param I2C Configuration
 */
void u8g2_hal_rpi_i2c_set_config(u8g2_hal_rpi_i2c_config_t param);

/**
 *  The function provides I2C Callback that conforms to U8g2's `u8x8_msg_cb` and applicable to both `byte_cb` and `gpio_and_delay_cb`.
 *  @brief I2C Callback
 */
uint8_t u8g2_hal_rpi_i2c_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

#endif // U8G2DEMO_U8G2_HAL_RPI_I2C_H
