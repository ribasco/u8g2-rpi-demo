# Define our host system
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)
#set(CMAKE_SYSTEM_PROCESSOR arm)

# Set module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Load Raspberry Pi Toolchain
find_package(RpiTools REQUIRED)

if (NOT RPI_TOOLCHAIN_FOUND)
    message(FATAL_ERROR "Please provide the path of your raspberry pi toolchain")
endif ()

set(CMAKE_STAGING_PREFIX ${RPI_TOOLCHAIN_DIR})
set(CMAKE_SYSROOT ${CMAKE_STAGING_PREFIX}/arm-bcm2708/arm-linux-gnueabihf/arm-linux-gnueabihf/sysroot)

#https://github.com/raspberrypi/tools.git

# Define the cross compiler locations
set(CMAKE_C_COMPILER ${CMAKE_STAGING_PREFIX}/arm-bcm2708/arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER ${CMAKE_STAGING_PREFIX}/arm-bcm2708/arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++)

message(STATUS "[TOOLCHAIN] STAGING PREFIX  = ${CMAKE_STAGING_PREFIX}")
message(STATUS "[TOOLCHAIN] MAKE            = ${CMAKE_MAKE_PROGRAM}")
message(STATUS "[TOOLCHAIN] C COMPILER      = ${CMAKE_C_COMPILER}")
message(STATUS "[TOOLCHAIN] C++ COMPILER    = ${CMAKE_CXX_COMPILER}")
message(STATUS "[TOOLCHAIN] SYSROOT         = ${CMAKE_SYSROOT}\n")

# Define the sysroot path for the RaspberryPi distribution in our tools folder
set(CMAKE_FIND_ROOT_PATH ${CMAKE_SYSROOT})

# Use our definitions for compiler tools
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# Search for libraries and headers in the target directories only
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

#-std=c++17
#-std=c11
#add_definitions(-Wall -std=c11)