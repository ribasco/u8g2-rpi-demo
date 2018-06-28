set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR}/install/wiringPi)
set(WIRINGPI_DIR ${LIB_DIR}/wiringPi)

# Setup wiring pi library
if (NOT EXISTS ${WIRINGPI_DIR} OR (NOT EXISTS ${WIRINGPI_DIR}/VERSION))
    message(STATUS "[WIRING-PI] Not yet existing: ${WIRINGPI_DIR}, Working directory: ${CMAKE_SOURCE_DIR}")
    message(STATUS "[WIRING-PI] Downloading WiringPi")
    message(STATUS "[WIRING-PI] SOURCE_DIR = ${CMAKE_SOURCE_DIR}/lib/wiringPi")

    file(DOWNLOAD https://github.com/ribasco/wiringPi/archive/master.zip ${LIB_DIR}/wiringPi.zip SHOW_PROGRESS)

    if (NOT EXISTS ${LIB_DIR}/wiringPi.zip)
        message(FATAL_ERROR "Could not find wiringPi-master.zip")
    endif ()

    message(STATUS "[WIRING-PI] Unzipping '${LIB_DIR}/wiringPi.zip' to '${LIB_DIR}'")
    execute_process(COMMAND unzip ${LIB_DIR}/wiringPi.zip WORKING_DIRECTORY ${LIB_DIR} ERROR_VARIABLE tc_unzip)

    if (tc_unzip)
        message(FATAL_ERROR "[WIRING-PI] Could not unzip contents of the downloaded toolchain (${tc_unzip})")
    endif ()

    message(STATUS "[WIRING-PI] Moving '${LIB_DIR}/wiringPi-master/' to '${LIB_DIR}/wiringPi'")
    execute_process(COMMAND mv ${LIB_DIR}/wiringPi-master/ ${LIB_DIR}/wiringPi ERROR_VARIABLE tc_move)

    if (tc_move)
        message(FATAL_ERROR "[WIRING-PI] Could not perform move operation (${tc_move})")
    endif ()

    message(STATUS "[WIRING-PI] Removing wiringPi.zip from ${LIB_DIR}")
    execute_process(COMMAND rm ${LIB_DIR}/wiringPi.zip ERROR_VARIABLE tc_remove ERROR_VARIABLE tc_remove)

    if (tc_remove)
        message(FATAL_ERROR "[WIRING-PI] Could not perform move operation (${tc_remove})")
    endif ()
endif ()

# Have CMake find our pthreads library within our toolchain (required for this library)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)

file(GLOB WPI_SRC_FILES "lib/wiringPi/wiringPi/*.c")
file(GLOB WPI_HEADER_FILES "lib/wiringPi/wiringPi/*.h")
file(COPY ${WPI_HEADER_FILES} DESTINATION ${PROJECT_BINARY_DIR}/install/wiringPi/include)

# Create our library target
add_library(wiringPi SHARED ${WPI_SRC_FILES})

set_target_properties(wiringPi
        PROPERTIES
        LINKER_LANGUAGE C
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/install/wiringPi/lib"
        )

# be sure to include the current source directory for header files
target_include_directories(wiringPi PRIVATE ${CMAKE_SOURCE_DIR}/lib/wiringPi/wiringPi)
target_link_libraries(wiringPi ${CMAKE_THREAD_LIBS_INIT} crypt m rt)

set(WIRINGPI_LIBRARIES "${PROJECT_BINARY_DIR}/install/wiringPi/lib/${CMAKE_SHARED_LIBRARY_PREFIX}wiringPi${CMAKE_SHARED_LIBRARY_SUFFIX}")
set(WIRINGPI_INCLUDE_DIR "${PROJECT_BINARY_DIR}/install/wiringPi/include")