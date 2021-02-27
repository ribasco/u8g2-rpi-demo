cmake_minimum_required(VERSION 3.11)
project(u8g2demo)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Libraries")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Executables")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all static libraries")
set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib)

# Obtain U8G2 Library (REQUIRED)
include("./cmake/GetU8G2.cmake")

# Obtain the required libraries, if none found in the system, cmake will try to download them for you
if (CMAKE_CROSSCOMPILING)
    include("./cmake/GetWiringPi.cmake")
else ()
    # we are not cross compiling, so we must be compiling on ARM system
    message(STATUS "System Processor = ${CMAKE_SYSTEM_PROCESSOR}")
    if (NOT ${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
        message(FATAL_ERROR "You are only allowed to build on top of an ARM system (Raspberry Pi)")
    endif ()

    # Check Wiring Pi
    find_package(WiringPi)
    if (NOT WiringPi_FOUND)
        message(STATUS "Wiring Pi library not found in system. Gonna try to download...")
        include("./cmake/GetWiringPi.cmake")
    endif ()
endif ()

# Make sure the required library directories exists before we proceed

#message(STATUS "[U8G2] U8G2_SRC_FILES = ${U8G2_SRC_FILES}")
message(STATUS "[U8G2] U8G2_HEADER_FILES = ${U8G2_HEADER_FILES}")
message(STATUS "[U8G2] U8G2_INCLUDE_DIR = ${U8G2_INCLUDE_DIR}")
message(STATUS "[U8G2] U8G2_LIBRARIES = ${U8G2_LIBRARIES}")
message(STATUS "[WIRING-PI] WIRINGPI_LIBRARIES = ${WIRINGPI_LIBRARIES}")
message(STATUS "[WIRING-PI] WIRINGPI_INCLUDE_DIR = ${WIRINGPI_INCLUDE_DIR}")

add_executable(u8g2demo main.h main.cpp ${U8G2_HEADER_FILES} ${U8G2_SRC_FILES} spi.cpp spi.h u8g2_hal_rpi.cpp u8g2_hal_rpi.h u8g2_hal_rpi_i2c.cpp u8g2_hal_rpi_i2c.h)

include_directories(${LIB_DIR}/bcm2835/src)
include_directories(${WIRINGPI_INCLUDE_DIR} ${U8G2_INCLUDE_DIR})
target_link_libraries(u8g2demo ${WIRINGPI_LIBRARIES})

if (TARGET wiringPi)
    add_dependencies(u8g2demo wiringPi)
endif ()

if (TARGET u8g2)
    message(STATUS "Found U8G2 Target Library")
endif ()
