# Introduction

![ST7920 using U8G2 on Raspberry Pi Demo ](demo.gif  "ST7920 using U8G2 on Raspberry Pi Demo ")

This project serves as an example of how to build and compile a C++ project using U8G2 and Wiring Pi library on the Raspberry Pi. CMake is used as the primary build tool with support for cross-compilation.

The display used for this demo is **ST7920**. If you would like to use a different display, you could easily modify the source and replace the setup/constructor definitions.

This project uses a custom SPI library for communication with the display, if you are wondering why that is considering Wiring Pi already provides such methods, please read the issue [here](https://github.com/olikraus/u8g2/issues/457).

# Project Dependencies

- [CMake 3.11.x ](https://github.com/Kitware/CMake)
- [Raspberry Pi Tools](https://github.com/raspberrypi/tools)  (Required for Cross-Compilation)
- [U8G2 Library](https://github.com/olikraus/u8g2)
- [Wiring Pi 2.46](https://git.drogon.net/?p=wiringPi;a=summary)
- SPI Library (Custom implementation included in this project)

# Build Instructions

The project comes with a utility script for compiling on the Raspberry Pi or for cross-compiling from a Linux operating system. Cross-compilation will automatically take place if the script has detected it is not running on the target platform.

To compile on the Raspberry Pi, simply run the compile script.

```bash
./compile.sh
```

CMake will automatically download the required libraries (wiring pi and u8g2) if it is not found on the target system.

If you are compiling from the Linux platform, you need the Raspberry Pi toolchain (See Project Dependencies). If this is not yet installed in your system then CMake will try and download them for you. If you happen to have the toolchain already installed, then you can pass the existing path to the compile script.

```bash
./compile.sh -t <path-of-the-rpi-toolchain>
```

By default, the cmake builds the binaries with debug information. If you need to compile without debug information then you can specify the build type it in the compile script (release or debug)

```bash
./compile.sh -b<release/debug>
```

The binary output can be found in the following path:

	<project_dir>/build/arm/<debug|release>/bin

# Supported Features

- SPI Hardware/Software implementations
- I2C


# Sample Usage

```c++
u8g2_t u8g2;

int main() {
    //1) Initialize the Struct containing SPI PIN details
    u8g2_rpi_hal_t u8g2_rpi_hal = {14 /*CLOCK/EN*/, 12 /*MOSI/RW*/, 10 /*CS/RS*/};
    u8g2_rpi_hal_init(u8g2_rpi_hal);

    //2) Pass the byte and gpio callbacks to the display setup procedure
    u8g2_Setup_st7920_s_128x64_f(&u8g2, U8G2_R2, cb_byte_spi_hw, cb_gpio_delay_rpi);

    //3) Initialize the display
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
    u8g2_ClearDisplay(&u8g2);


    //4) Do whatever you want
    u8g2_DrawStr(u8g2, 10, 32, "Hello");

    //Flush the buffer
    u8g2_SendBuffer(u8g2);

    return 0;
}
```
