/**
 *  This file provides inerfaces for U8g2 HAL implementation for Raspberry Pi I2C.
 *  @brief U8g2 HAL implementation for Raspberry Pi I2C
 *  @file
 *  @author Tomoyuki Nakashima
 *  @copyright Copyright 2021 Tomoyuki Nakashima  
    <br>
    Licensed under the Apache License, Version 2.0 (the "License");  
    you may not use this file except in compliance with the License.  
    You may obtain a copy of the License at  

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software  
    distributed under the License is distributed on an "AS IS" BASIS,  
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  
    See the License for the specific language governing permissions and  
    limitations under the License.  
    <br>
    See `LICENSE` in the project root for more information.
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
 *  @remark For better U8g2 performance, you can change I2C bus speed to 400 kb/s with the following configuration. <br>
 *  /boot/config.txt: `dtparam=i2c_arm=on,i2c_arm_baudrate=400000`
 */
void u8g2_hal_rpi_i2c_set_config(u8g2_hal_rpi_i2c_config_t param);

/**
 *  The function provides I2C Callback that conforms to U8g2's `u8x8_msg_cb` and applicable to both `byte_cb` and `gpio_and_delay_cb`.
 *  @brief I2C Callback
 */
uint8_t u8g2_hal_rpi_i2c_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

#endif // U8G2DEMO_U8G2_HAL_RPI_I2C_H
