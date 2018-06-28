set(TOOLS_DIR_PATH ${CMAKE_CURRENT_LIST_DIR}/../tools)
set(RPI_TOOLCHAIN_PATH "${TOOLS_DIR_PATH}/rpi" CACHE PATH "The path of the Raspberry Pi Toolchain")

message(STATUS "[FIND-TOOLCHAIN] Search Path = ${RPI_TOOLCHAIN_PATH}")

# Check if the path exists, if it doesn't, download a copy to the source directory
if (NOT EXISTS ${RPI_TOOLCHAIN_PATH})
    message(STATUS "[FIND-TOOLCHAIN] Downloading toolchain to ${TOOLS_DIR_PATH}/toolchain.zip")
    file(DOWNLOAD https://github.com/raspberrypi/tools/archive/master.zip ${TOOLS_DIR_PATH}/toolchain.zip SHOW_PROGRESS)

    message(STATUS "[FIND-TOOLCHAIN] Unzipping '${TOOLS_DIR_PATH}/toolchain.zip' to '${TOOLS_DIR_PATH}'")
    execute_process(COMMAND unzip ${TOOLS_DIR_PATH}/toolchain.zip WORKING_DIRECTORY ${TOOLS_DIR_PATH} ERROR_VARIABLE tc_unzip)

    if (tc_unzip)
        message(FATAL_ERROR "[FIND-TOOLCHAIN] Could not unzip contents of the downloaded toolchain (${tc_unzip})")
    endif ()

    message(STATUS "[FIND-TOOLCHAIN] Moving '${TOOLS_DIR_PATH}/tools-master/' to '${RPI_TOOLCHAIN_PATH}'")
    execute_process(COMMAND mv ${TOOLS_DIR_PATH}/tools-master/ ${RPI_TOOLCHAIN_PATH} ERROR_VARIABLE tc_move)

    if (tc_move)
        message(FATAL_ERROR "[FIND-TOOLCHAIN] Could not perform move operation (${tc_move})")
    endif ()

    message(STATUS "[FIND-TOOLCHAIN] Removing toolchain.zip from ${TOOLS_DIR_PATH}" ERROR_VARIABLE tc_remove)
    execute_process(COMMAND rm ${TOOLS_DIR_PATH}/toolchain.zip)

    if (tc_remove)
        message(FATAL_ERROR "[FIND-TOOLCHAIN] Could not perform delete operation")
    endif ()
endif ()

if (EXISTS "${RPI_TOOLCHAIN_PATH}/arm-bcm2708")
    message(STATUS "[FIND-TOOLCHAIN] Found toolchain = ${RPI_TOOLCHAIN_PATH}")
    set(RPI_TOOLCHAIN_DIR ${RPI_TOOLCHAIN_PATH})
    set(RPI_TOOLCHAIN_FOUND true)
else ()
    set(RPI_TOOLCHAIN_FOUND false)
endif ()

mark_as_advanced(
        RPI_TOOLCHAIN_DIR
)