#include <iostream>
#include <u8g2.h>
#include <wiringPi.h>
#include <cstring>

#define USE_SPI_HW false

#if USE_SPI_HW

#include "spi.h"

#endif

using namespace std;

struct u8g2_rpi_hal_t {
    int clk;  //en
    int mosi; //rw, data
    int cs; //rs
    //int reset; //reset (is this needed?)
};

u8g2_rpi_hal_t u8g2_rpi_hal = {14 /*CLOCK/EN*/, 12 /*MOSI/RW*/, 10 /*CS/RS*/};

#if USE_SPI_HW

uint8_t cb_byte_spi_hw(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch (msg) {
        case U8X8_MSG_BYTE_SEND: {
            if (spi_write(0, (uint8_t *) arg_ptr, arg_int) < 1)
                cerr << "Unable to send data" << endl;
            break;
        }
        case U8X8_MSG_BYTE_INIT: {
            //disable chip-select
            u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);

            //initialize spi device
            int fd = spi_setup(0, 500000, SPI_DEV_MODE_0);
            if (fd < 0) {
                cerr << "Problem initializing SPI on RPI" << endl;
                return 0;
            }

            //IMPORTANT: Make sure we reset the pin modes to activate the SPI hardware features!!!!
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

#endif

/**
 * Bit-bang SPI Implementation (identical to u8x8_byte_4wire_sw_spi). Used for  testing only.
 */
uint8_t cb_byte_spi_sw(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    uint8_t i, b;
    uint8_t *data;
    auto takeover_edge = static_cast<uint8_t>(u8x8_GetSPIClockPhase(u8x8));
    auto not_takeover_edge = static_cast<uint8_t>(1 - takeover_edge);

    switch (msg) {
        case U8X8_MSG_BYTE_SEND:
            data = (uint8_t *) arg_ptr;
            while (arg_int > 0) {
                b = *data;
                data++;
                arg_int--;
                for (i = 0; i < 8; i++) {
                    if (b & 128)
                        u8x8_gpio_SetSPIData(u8x8, 1);
                    else
                        u8x8_gpio_SetSPIData(u8x8, 0);
                    b <<= 1;

                    u8x8_gpio_SetSPIClock(u8x8, not_takeover_edge);
                    u8x8_gpio_Delay(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->sda_setup_time_ns);
                    u8x8_gpio_SetSPIClock(u8x8, takeover_edge);
                    u8x8_gpio_Delay(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->sck_pulse_width_ns);
                }
            }
            break;

        case U8X8_MSG_BYTE_INIT:
            /* disable chipselect */
            u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
            /* for SPI: setup correct level of the clock signal */
            u8x8_gpio_SetSPIClock(u8x8, u8x8_GetSPIClockPhase(u8x8));
            break;
        case U8X8_MSG_BYTE_SET_DC:
            u8x8_gpio_SetDC(u8x8, arg_int);
            break;
        case U8X8_MSG_BYTE_START_TRANSFER:
            u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_enable_level);
            u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->post_chip_enable_wait_ns, NULL);
            break;
        case U8X8_MSG_BYTE_END_TRANSFER:
            u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->pre_chip_disable_wait_ns, NULL);
            u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
            break;
        default:
            return 0;
    }
    return 1;
}

/**
 * GPIO and Delay Callback
 */
uint8_t cb_gpio_rpi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, U8X8_UNUSED void *arg_ptr) {
    switch (msg) {
        case U8X8_MSG_GPIO_AND_DELAY_INIT: {
#if !USE_SPI_HW
            pinMode(u8g2_rpi_hal.mosi, OUTPUT);
            pinMode(u8g2_rpi_hal.clk, OUTPUT);
            pinMode(u8g2_rpi_hal.cs, OUTPUT);
#endif
            //pinMode(u8g2_rpi_hal.reset, OUTPUT);
            break;
        }
        case U8X8_MSG_DELAY_MILLI: {
            delay(arg_int);
            break;
        }
        case U8X8_MSG_DELAY_NANO: {
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

/**
 * Byte Communications Callback (This is a proxy method)
 */
uint8_t cb_byte_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
#if USE_SPI_HW
    cb_byte_spi_hw(u8x8, msg, arg_int, arg_ptr);
#else
    u8x8_byte_4wire_sw_spi(u8x8, msg, arg_int, arg_ptr);
    //cb_byte_spi_sw(u8x8, msg, arg_int, arg_ptr);
#endif
}

int main() {
    //Initialize Wiring Pi
    wiringPiSetup();

    u8g2_t u8g2;
    u8g2_Setup_st7920_s_128x64_f(&u8g2, U8G2_R2, cb_byte_spi, cb_gpio_rpi);

    //Initialize Display
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
    u8g2_ClearDisplay(&u8g2);

    //Set Font
    u8g2_SetFont(&u8g2, u8g2_font_7x13B_mf);

#if USE_SPI_HW
    cout << "[MODE] SPI Hardware" << endl;
    string mode = "SPI Hardware";
#else
    cout << "[MODE] SPI Software bit-bang implementation" << endl;
    string mode = "SPI Bit-Bang";
#endif

    //Run display loop
    uint8_t x = 0;
    while (true) {
        if (x <= 0)
            x = 255;
        u8g2_ClearBuffer(&u8g2);
        string text = "UG82 running on Raspberry Pi: " + mode;
        u8g2_DrawStr(&u8g2, x, 32, text.c_str());
        u8g2_SendBuffer(&u8g2);
        x--;
        delay(10);
    }
}