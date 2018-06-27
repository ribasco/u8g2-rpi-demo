cmake_minimum_required(VERSION 3.11)
project(u8g2demo)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Libraries")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Executables")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all static libraries")

if (CMAKE_CROSSCOMPILING)
    include(ExternalProject)
    set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib)
    include("./cmake/GetWiringPi.cmake")
    include("./cmake/GetU8G2.cmake")
else ()
    find_package(WiringPi REQUIRED)
endif ()

add_executable(u8g2demo main.h main.cpp ${U8G2_HEADER_FILES} ${U8G2_SRC_FILES} spi.cpp spi.h u8g2_hal_rpi.cpp u8g2_hal_rpi.h)

message(STATUS "[U8G2] U8G2_INCLUDE_DIR = ${U8G2_INCLUDE_DIR}")
message(STATUS "[U8G2] U8G2_LIBRARIES = ${U8G2_LIBRARIES}")
message(STATUS "[WIRING-PI] WIRINGPI_LIBRARIES = ${WIRINGPI_LIBRARIES}")
message(STATUS "[WIRING-PI] WIRINGPI_INCLUDE_DIR = ${WIRINGPI_INCLUDE_DIR}")

include_directories(${LIB_DIR}/bcm2835/src)
include_directories(${WIRINGPI_INCLUDE_DIR} ${U8G2_INCLUDE_DIR})
target_link_libraries(u8g2demo ${WIRINGPI_LIBRARIES})

if (TARGET wiringPi)
    add_dependencies(u8g2demo wiringPi)
endif ()