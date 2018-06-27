#include <iostream>
#include <wiringPi.h>
#include <cstring>
#include <csignal>
#include <unistd.h>
#include "u8g2_hal_rpi.h"

using namespace std;

u8g2_t u8g2;

static volatile bool complete = false;
static int forced_exit_attempts = 0;

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

    uint8_t x = 255;
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

        if (x <= 0)
            x = 255;
        u8g2_ClearBuffer(u8g2);
        string text = "UG82 running on Raspberry Pi!";
        u8g2_DrawStr(u8g2, x, 32, text.c_str());
        u8g2_SendBuffer(u8g2);
        x-=5;
        delay(500);
    }
}

int main() {
    struct sigaction sigIntHandler{};
    sigIntHandler.sa_handler = exitHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, nullptr);

    //Initialize Wiring Pi
    wiringPiSetup();

    //Configure the SPI pins
    u8g2_rpi_hal_t u8g2_rpi_hal = {14 /*CLOCK/EN*/, 12 /*MOSI/RW*/, 10 /*CS/RS*/};
    u8g2_rpi_hal_init(u8g2_rpi_hal);

    //for bit bang implementations, you can use the pre-built callbacks available in the u8g2 library
    //see https://github.com/olikraus/u8g2/wiki/Porting-to-new-MCU-platform#communication-callback-eg-u8x8_byte_hw_i2c
    //ex: u8x8_byte_4wire_sw_spi
    u8g2_Setup_st7920_s_128x64_f(&u8g2, U8G2_R2, cb_byte_spi_hw, cb_gpio_delay_rpi);

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