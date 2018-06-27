FIND_PATH(WIRINGPI_INCLUDE_DIR NAMES wiringPi.h
        PATHS
        /usr
        /usr/local
        /opt
        PATH_SUFFIXES
        )

FIND_LIBRARY(WIRINGPI_LIBRARIES NAMES wiringPi
        PATHS
        /usr
        /usr/local
        /opt
        )

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(WiringPi FOUND_VAR "WiringPi_FOUND" REQUIRED_VARS WIRINGPI_LIBRARIES WIRINGPI_INCLUDE_DIR)



