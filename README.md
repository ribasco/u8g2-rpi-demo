# Introduction

This project serves as an example of how to build and compile a C++ project using U8G2 and Wiring Pi library on the Raspberry Pi. CMake is used as the primary build tool with support for cross-compilation.

The display used for this demo is **ST7920**. If you would like to use a different display, you could easily modify the source and replace the setup/constructor definitions.

Click [here](https://github.com/olikraus/u8g2/issues/457)  for more details about the implementation.

# Project Dependencies

- [CMake 3.11.x ](https://github.com/Kitware/CMake)
- [Raspberry Pi Tools](https://github.com/raspberrypi/tools)  (Required for Cross-Compilation)
- [U8G2 Library](https://github.com/olikraus/u8g2)
- [Wiring Pi 2.46](https://git.drogon.net/?p=wiringPi;a=summary)
- SPI Library (Custom implementation included in this project)

# Build Instructions

The project includes a convenience utility for building on the Raspberry Pi or for cross-compiling from a Linux operating system.

To compile on the Raspberry Pi, simply run the compile script.

```bash
./compile.sh
```

CMake will automatically download the required libraries (wiring pi and u8g2) if it is not found on the target system.

If you are compiling from the Linux platform, you need the Raspberry Pi toolchain (See Project Dependencies). If this is not yet in your system then CMake will try and download them for you. If you happen to have the toolchain already installed, then you can pass the existing path to the compile script.

```bash
./compile.sh -t <path-of-the-rpi-toolchain>
```

By default, the cmake builds the binaries with debug information. If you need to compile without debug information then you can specify the build type it in the compile script (release or debug)

```bash
./compile.sh -b<release/debug>
```

The binary output can be found in the

	<project_dir>/build/arm/<debug|release>/bin