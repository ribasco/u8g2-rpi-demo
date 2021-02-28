/**
 *  This file provides implementation for U8g2 HAL implementation for Raspberry Pi I2C.
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

// u8g2 + external I/F of this file
#include "u8g2_hal_rpi_i2c.h"

// std::cout, std::cerr, std::strerror()
#include <iostream>
#include <cstring>

// assert()
#include <cassert>

// open(), close(), write() and ioctl() for I2C deriver
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

// delay()
#include <wiringPi.h>

using namespace std;

static u8g2_hal_rpi_i2c_config_t i2c_config;

void u8g2_hal_rpi_i2c_set_config(u8g2_hal_rpi_i2c_config_t config) {
    assert(config.ifname);
    assert(config.ifname[0]);
    assert(config.target_addr >= 0x00);
    assert(config.target_addr <= 0x7F);
    i2c_config = config;
}

uint8_t u8g2_hal_rpi_i2c_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    static uint8_t send_buf[32]; // u8g2/u8x8 will never send more than 32 bytes between START_TRANSFER and END_TRANSFER
    static uint8_t send_cnt = 0;
    static int last_errno = -1;
    switch (msg) {
        case U8X8_MSG_BYTE_INIT: { // 20
            if (i2c_config.debug) cout << "U8X8_MSG_BYTE_INIT" << endl;
            break;
        }
        case U8X8_MSG_BYTE_SEND: { // 23
            if (i2c_config.verbose) cout << "U8X8_MSG_BYTE_SEND " << int(arg_int) << endl;
            uint8_t* p = (uint8_t*) arg_ptr;      
            while (arg_int > 0)
            {
	        send_buf[send_cnt++] = *p++;
	        arg_int--;
            }  
            break;
        }
        case U8X8_MSG_BYTE_START_TRANSFER: { // 24
            if (i2c_config.verbose) cout << "U8X8_MSG_BYTE_START_TRANSFER" << endl;
            send_cnt = 0;
            break;
        }
        case U8X8_MSG_BYTE_END_TRANSFER: { // 25
            if (i2c_config.verbose) cout << "U8X8_MSG_BYTE_END_TRANSFER " << int(send_cnt) << endl;
            if (send_cnt <= 0) break;
            int i2c_fd = open(i2c_config.ifname, O_RDWR);
            if (i2c_fd < 0) {
                if (last_errno != errno) {
                    cerr << "Error: open() failed"
                         << ", path: " << i2c_config.ifname
                         << ", error: " << strerror(errno) << endl;
                    last_errno = errno;
                }
                break;
            }
            if (ioctl(i2c_fd, I2C_SLAVE, i2c_config.target_addr) < 0) {
                if (last_errno != errno) {
                    cerr << "Error: ioctl() failed"
                         << ", request: I2C_SLAVE"
                         << ", address: " << int(i2c_config.target_addr)
                         << ", error: " << strerror(errno) << endl;
                    last_errno = errno;
                }
            }
            else if (write(i2c_fd, send_buf, send_cnt) < 0) {
                if (last_errno != errno) {
                    cerr << "Error: write() failed"
                         << ", count: " << int(send_cnt)
                         << ", error: " << strerror(errno) << endl;
                    last_errno = errno;
                }
            }
            close(i2c_fd);
            send_cnt = 0;
            break;
        }
        case U8X8_MSG_GPIO_AND_DELAY_INIT: { // 40
            if (i2c_config.debug) cout << "U8X8_MSG_GPIO_AND_DELAY_INIT" << endl;
            break;
        }
        case U8X8_MSG_DELAY_MILLI: { // 41
            if (i2c_config.debug) cout << "U8X8_MSG_DELAY_MILLI " << int(arg_int) << endl;
            delay(arg_int);
            break;
        }
        case U8X8_MSG_DELAY_10MICRO: { // 42
            if (i2c_config.debug) cout << "U8X8_MSG_DELAY_10MICRO " << int(arg_int) << endl;
            delayMicroseconds(arg_int * 10);
            break;
        }
        case U8X8_MSG_DELAY_100NANO: { // 43
            if (i2c_config.debug) cout << "U8X8_MSG_DELAY_100NANO " << int(arg_int) << endl;
            delayMicroseconds((arg_int + 9) / 10);
            break;
        }
        case U8X8_MSG_DELAY_NANO: { // 44
            if (i2c_config.debug) cout << "U8X8_MSG_DELAY_NANO " << int(arg_int) << endl;
            delayMicroseconds(arg_int == 0 ? 0 : 1);
            break;
        }
        case U8X8_MSG_GPIO_RESET: { // 64 + 11
            if (i2c_config.debug) cout << "U8X8_MSG_GPIO_RESET " << endl;
            // not supported
            break;
        }
        default: {
            if (i2c_config.debug) cout << "U8X8_MSG " << int(msg) << endl;
            return 0;
        }
    }
    return 1;
}
