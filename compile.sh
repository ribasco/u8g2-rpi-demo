#!/usr/bin/env bash

usage() { echo "Usage: $0 -t <rpi tools path> -b <Debug/Release>" 1>&2; exit 1; }

function is_installed {
  local return_=1
  type $1 >/dev/null 2>&1 || { local return_=0; }
  echo "$return_"
}

MAIN_DIR=$(pwd)

# Check if CMAKE is installed
if [ $(is_installed cmake) -ne "1" ]; then
    echo "========================================================================================================================================"
    echo "You need CMAKE 3.11.x or above to compile the program. If its not available in the package manager, then install via source. Steps below"
    echo "========================================================================================================================================"
    echo "Installation steps: "
    echo "- sudo apt-get install libcurl4-openssl-dev"
    echo "- git clone https://github.com/Kitware/CMake.git"
    echo "- cd CMake"
    echo "- ./bootstrap --system-curl"
    echo "- make"
    echo "- make install"
    exit 1
fi

while getopts t:b: option
do
case "${option}"
in
b) BUILD_TYPE=${OPTARG};;
t) RPI_TOOLS_PATH=${OPTARG};;
*)
usage
;;
esac
done

# Use default build type if not specified
if [ -z "$BUILD_TYPE" ]
then
    BUILD_TYPE=Debug
fi

BUILD_TYPE=$(echo ${BUILD_TYPE} | awk '{print tolower($0)}' | awk '{for(j=1;j<=NF;j++){ $j=toupper(substr($j,1,1)) substr($j,2) }}1')

if [ "$BUILD_TYPE" != "Debug" ] && [ "$BUILD_TYPE" != "Release" ];
then
  echo "Invalid build type = ${BUILD_TYPE}. Specify either Debug or Release"
  exit 1
fi

# Machine Type (arm, x86_64 etc)
MACHINE_TYPE=`uname -m`
OS=`uname -o`

# Cross-Compilation Flags
BUILD_DIR=
CC_FLAGS=

# Check the machine type we are running on
if [[ ${MACHINE_TYPE} == *"arm"* ]]; then
    echo "Host system is ARM"
    CC_FLAGS=
else
    if [ "$OS" = "GNU/Linux" ]; then
        # echo "Host system is non-arm machine on a Linux Operating System. Cross-compiling enabled"
        CC_FLAGS="-DCMAKE_TOOLCHAIN_FILE=./cmake/RpiToolchain.cmake"
    else
        echo "Operating system not-supported"
        exit 1
    fi
fi

# Verify the tools path if cross-compilation is enabled
if [ ! -z "$CC_FLAGS" ] && [ ! -z "$RPI_TOOLS_PATH" ] && [ ! -d "${RPI_TOOLS_PATH}" ]
then
    echo "ERROR: Invalid raspberry pi tools path specified = ${RPI_TOOLS_PATH}"
    exit 1
elif [ ! -z "$CC_FLAGS" ] && [ ! -z "$RPI_TOOLS_PATH" ] && [ -d "${RPI_TOOLS_PATH}" ]
then
    CC_FLAGS="${CC_FLAGS} -DRPI_TOOLCHAIN_PATH=${RPI_TOOLS_PATH}"
fi

# Determine build directory
if [ "$BUILD_TYPE" = "Debug" ];
then
    BUILD_DIR="build/arm/debug"
elif [ "$BUILD_TYPE" = "Release" ];
then
    BUILD_DIR="build/arm/release"
fi

CMAKE_BIN_PATH=$(which cmake)
CMAKE_FLAGS="-DCMAKE_BUILD_TYPE=${BUILD_TYPE} ${CC_FLAGS} -H. -B${BUILD_DIR}"
CMAKE_CMD="${CMAKE_BIN_PATH} ${CMAKE_FLAGS}"

echo "==================================================="
echo "Compile Info"
echo "==================================================="
echo "Machine Type = ${MACHINE_TYPE}"
echo "Operating System = ${OS}"
echo "Target Build Directory = ${BUILD_DIR}"
echo "Tools Path = ${RPI_TOOLS_PATH}"
echo "Build Type = ${BUILD_TYPE}"
echo "CMake Bin Path = ${CMAKE_BIN_PATH}"
echo "Cmake Command = ${CMAKE_CMD}"
echo "==================================================="

echo "Running CMake command"

${CMAKE_CMD}

if [ $? -ne 0 ]; then
    echo "ERROR: CMake command failed. Build discontinued"
    exit 1
fi

#Verify that the target build directory exists
if [ ! -d "${BUILD_DIR}" ]; then
    echo "ERROR: Target build directory does not exists"
    exit 1
fi

cd ${BUILD_DIR}

echo "==================================================="
echo "Building Project Binaries"
echo "==================================================="

make
if [ $? -ne 0 ]; then
    echo "ERROR: Build Failed"
    exit 1
fi

read -p "Would you like to run the example? [y/n] " -n 1 -r
echo    # (optional) move to a new line
if [[ $REPLY =~ ^[Yy]$ ]]
then
    ${MAIN_DIR}/${BUILD_DIR}/bin/u8g2demo
fi