//
// Created by raffy on 6/27/18.
//

#include <wiringPi.h>
#include <iostream>
#include "spi.h"
#include "u8g2_hal_rpi.h"

#define U8G2_HAL_SPI_SPEED 500000
#define U8G2_HAL_SPI_MODE SPI_DEV_MODE_0
#define U8G2_HAL_SPI_CHANNEL 0

using namespace std;

static u8g2_rpi_hal_t u8g2_rpi_hal;

void u8g2_rpi_hal_init(u8g2_rpi_hal_t param) {
    u8g2_rpi_hal = param;
}

uint8_t cb_byte_spi_hw(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch (msg) {
        case U8X8_MSG_BYTE_SEND: {
            if (spi_write(U8G2_HAL_SPI_CHANNEL, (uint8_t *) arg_ptr, arg_int) < 1)
                cerr << "Unable to send data" << endl;
            break;
        }
        case U8X8_MSG_BYTE_INIT: {
            //disable chip-select
            u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);

            //initialize spi device
            int fd = spi_setup(U8G2_HAL_SPI_CHANNEL, U8G2_HAL_SPI_SPEED, U8G2_HAL_SPI_MODE);
            if (fd < 0) {
                cerr << "Problem initializing SPI on RPI" << endl;
                return 0;
            }

            //IMPORTANT: Make sure we reset the pin modes
            // to activate the SPI hardware features!!!!
            pinModeAlt(u8g2_rpi_hal.mosi, 0b100);
            pinModeAlt(u8g2_rpi_hal.clk, 0b100);
            pinModeAlt(u8g2_rpi_hal.cs, 0b100);
            break;
        }
        case U8X8_MSG_BYTE_START_TRANSFER: {
            u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_enable_level);
            u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->post_chip_enable_wait_ns, nullptr);
            break;
        }
        case U8X8_MSG_BYTE_END_TRANSFER: {
            u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->pre_chip_disable_wait_ns, nullptr);
            u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
            break;
        }
        default:
            return 0;
    }
    return 1;
}

uint8_t cb_gpio_delay_rpi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, U8X8_UNUSED void *arg_ptr) {
    switch (msg) {
        case U8X8_MSG_GPIO_AND_DELAY_INIT: {
            pinMode(u8g2_rpi_hal.mosi, OUTPUT);
            pinMode(u8g2_rpi_hal.clk, OUTPUT);
            pinMode(u8g2_rpi_hal.cs, OUTPUT);
            break;
        }
        case U8X8_MSG_DELAY_MILLI: {
            delay(arg_int);
            break;
        }
        case U8X8_MSG_DELAY_NANO: {
            //this is important. Removing this will cause garbage data to be displayed.
            delayMicroseconds(arg_int == 0 ? 0 : 1);
            break;
        }
        case U8X8_MSG_GPIO_SPI_CLOCK: {
            digitalWrite(u8g2_rpi_hal.clk, arg_int);
            break;
        }
        case U8X8_MSG_GPIO_SPI_DATA: {
            digitalWrite(u8g2_rpi_hal.mosi, arg_int);
            break;
        }
        case U8X8_MSG_GPIO_CS: {
            digitalWrite(u8g2_rpi_hal.cs, arg_int);
            break;
        }
        default: {
            return 0;
        }
    }
    return 1;
}

uint8_t cb_byte_i2c_hw(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch (msg) {
        case U8X8_MSG_BYTE_INIT: {
            //disable chip-select
            //TODO: Add implementation
            break;
        }
        case U8X8_MSG_BYTE_SEND: {
            //TODO: Add implementation
            break;
        }
        case U8X8_MSG_BYTE_START_TRANSFER: {
            //TODO: Add implementation
            break;
        }
        case U8X8_MSG_BYTE_END_TRANSFER: {
            //TODO: Add implementation
            break;
        }
        default:
            return 0;
    }
    return 1;
}
