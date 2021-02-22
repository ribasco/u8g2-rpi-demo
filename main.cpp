#include <iostream>
#include <wiringPi.h>
#include <cstring>
#include <csignal>
#include <unistd.h>

#include "u8g2_hal_rpi.h"
#include "u8g2_hal_rpi_i2c.h"

using namespace std;

u8g2_t u8g2;

static volatile bool complete = false;
static int forced_exit_attempts = 0;

// for drawBannerText()
static const char* scroll_text = "UG82 running on Raspberry Pi!";
static u8g2_uint_t scroll_start_x = 255;
static u8g2_uint_t scroll_step_x = 5;
static u8g2_uint_t scroll_y = 32;
static unsigned int scroll_delay = 500;

// for debug trace
#define DEBUG_TRACE(x) { \
    int t0 = millis(); \
    x \
    int t1 = millis(); \
    cout << #x << " takes " << t1 - t0 << " msec" << endl; \
}

void exitHandler(int s) {
    if (forced_exit_attempts >= 2) {
        cout << "Forcing shutdown" << endl;
        u8g2_ClearDisplay(&u8g2);
        exit(1);
    }
    complete = true;
    forced_exit_attempts++;
}

void drawU8G2Logo(u8g2_t *u8g2) {
    u8g2_ClearBuffer(u8g2);
    u8g2_SetFontMode(u8g2, 1);

    u8g2_SetFontDirection(u8g2, 0);
    u8g2_SetFont(u8g2, u8g2_font_inb16_mf);
    u8g2_DrawStr(u8g2, 0, 22, "U");

    u8g2_SetFontDirection(u8g2, 1);
    u8g2_SetFont(u8g2, u8g2_font_inb19_mn);
    u8g2_DrawStr(u8g2, 14, 8, "8");

    u8g2_SetFontDirection(u8g2, 0);
    u8g2_SetFont(u8g2, u8g2_font_inb16_mf);
    u8g2_DrawStr(u8g2, 36, 22, "g");
    u8g2_DrawStr(u8g2, 48, 22, "\xb2");

    u8g2_DrawHLine(u8g2, 2, 25, 34);
    u8g2_DrawHLine(u8g2, 3, 26, 34);
    u8g2_DrawVLine(u8g2, 32, 22, 12);
    u8g2_DrawVLine(u8g2, 33, 23, 12);
    u8g2_SendBuffer(u8g2);
}

void drawBannerText(u8g2_t *u8g2, uint32_t durationSecs) {

    u8g2_uint_t x = scroll_start_x;
    uint32_t prevMillis = 0;

    //Set Font
    u8g2_SetFont(u8g2, u8g2_font_7x13B_mf);
    uint32_t elapsed = 0;

    while (elapsed != durationSecs) {
        if (complete)
            break;

        uint32_t curMillis = millis();

        //Count the number of elapsed seconds
        if ((curMillis - prevMillis) >= 1000) {
            prevMillis = curMillis;
            elapsed++;
        }

        u8g2_ClearBuffer(u8g2);
        u8g2_DrawStr(u8g2, x, scroll_y, scroll_text);
        // DEBUG_TRACE(
        u8g2_SendBuffer(u8g2);
        // )
        if (x >= scroll_step_x) {
            x -= scroll_step_x;
        }
        else {
            x = scroll_start_x;
        }
        delay(scroll_delay);
    }
}

int main(int argc, char* argv[]) {
    struct sigaction sigIntHandler{};
    sigIntHandler.sa_handler = exitHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, nullptr);

    //Initialize Wiring Pi
    wiringPiSetup();

    if ((argc == 1)
    ||  (argc > 1 && strcmp(argv[argc-1], "st7920_s_128x64") == 0)) {

    //Configure the SPI pins
    u8g2_rpi_hal_t u8g2_rpi_hal = {14 /*CLOCK/EN*/, 12 /*MOSI/RW*/, 10 /*CS/RS*/};
    u8g2_rpi_hal_init(u8g2_rpi_hal);

    //for bit bang implementations, you can use the pre-built callbacks available in the u8g2 library
    //see https://github.com/olikraus/u8g2/wiki/Porting-to-new-MCU-platform#communication-callback-eg-u8x8_byte_hw_i2c
    //ex: u8x8_byte_4wire_sw_spi
    u8g2_Setup_st7920_s_128x64_f(&u8g2, U8G2_R2, cb_byte_spi_hw, cb_gpio_delay_rpi);

    }
    else if (argc > 1 && strcmp(argv[argc-1], "ssd1327_i2c_ea_w128128") == 0) {
        // Initialize I2C
        const u8g2_hal_rpi_i2c_config_t config = {
            "/dev/i2c-1" // ifname
            , 0x3c // target_addr
            , 0 // debug
            , 0 // verbose
        };
        u8g2_hal_rpi_i2c_set_config(config);

        // Setup U8g2 for display "ea_w128128" and controller "ssd1327" via I2C
        u8g2_Setup_ssd1327_i2c_ea_w128128_f(&u8g2, U8G2_R0, u8g2_hal_rpi_i2c_cb, u8g2_hal_rpi_i2c_cb);

        // For better U8g2 performance, you can change I2C bus speed to 400 kb/s with the following configuration.
        // /boot/config.txt: `dtparam=i2c_arm=on,i2c_arm_baudrate=400000`

        // for drawBannerText()
        scroll_text = "Raspberry Pi I2C";
        scroll_start_x = 128;
        scroll_step_x = 8;
        scroll_delay = 100;

        cout << "U8g2 and HAL Configuration" << endl
             << "  display_type: " << argv[argc-1] << endl
             << "  interface_type: I2C" << endl
             << "  interface_name: " << config.ifname << endl
             << "  target_address: " << config.target_addr << endl
             << "  debug: " << config.debug << endl
             << "  verbose: " << config.verbose << endl;
    }
    else {
        cerr << "Usage: " << argv[0] << " [DISPLAY_TYPE]" << endl
             << "  DISPLAY_TYPE: one of the following names" << endl
             << "    st7920_s_128x64 (default type)" << endl
             << "    ssd1327_i2c_ea_w128128" << endl;
	return 0;
    }

    //Initialize Display
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
    u8g2_ClearDisplay(&u8g2);

    cout << "Running display demo...Press Ctrl+C to exit" << endl;

    //Run display loop
    while (!complete) {
        ///Draw U8G2 Logo
        drawU8G2Logo(&u8g2);
        delay(5000);

        //Run Banner Animation
        drawBannerText(&u8g2, 20);
    }

    cout << "Exiting demo" << endl;
    u8g2_ClearDisplay(&u8g2);
}
