# Setup u8g2 library
set(U8G2_DIR ${LIB_DIR}/u8g2)

if (CMAKE_CROSSCOMPILING)

    if (NOT EXISTS ${U8G2_DIR})
        set(U8G2_ARCHIVE_FILE_NAME u8g2.zip)
        set(U8G2_ARCHIVE_FILE_PATH ${LIB_DIR}/${U8G2_ARCHIVE_FILE_NAME})

        message(STATUS "[U8G2] Downloading to ${U8G2_ARCHIVE_FILE_PATH}")
        message(STATUS "[U8G2] Not yet existing: ${U8G2_DIR}, Working directory: ${CMAKE_SOURCE_DIR}")
        file(DOWNLOAD https://github.com/olikraus/u8g2/archive/master.zip ${U8G2_ARCHIVE_FILE_PATH} SHOW_PROGRESS)

        if (NOT EXISTS ${U8G2_ARCHIVE_FILE_PATH})
            message(FATAL_ERROR "Could not find ${U8G2_ARCHIVE_FILE_PATH}")
        endif ()

        message(STATUS "[U8G2] Unzipping '${U8G2_ARCHIVE_FILE_NAME}' to '${LIB_DIR}'")
        execute_process(COMMAND unzip ${U8G2_ARCHIVE_FILE_PATH} WORKING_DIRECTORY ${LIB_DIR} ERROR_VARIABLE tc_unzip)

        if (tc_unzip)
            message(FATAL_ERROR "[U8G2] Could not unzip contents of the downloaded toolchain (${tc_unzip})")
        endif ()

        message(STATUS "[U8G2] Moving '${LIB_DIR}/u8g2-master/' to '${U8G2_DIR}'")
        execute_process(COMMAND mv ${LIB_DIR}/u8g2-master/ ${U8G2_DIR} ERROR_VARIABLE tc_move)

        if (tc_move)
            message(FATAL_ERROR "[U8G2] Could not perform move operation (${tc_move})")
        endif ()

        message(STATUS "[U8G2] Removing u8g2.zip from ${LIB_DIR}")
        execute_process(COMMAND rm ${U8G2_ARCHIVE_FILE_PATH} ERROR_VARIABLE tc_remove ERROR_VARIABLE tc_remove)

        if (tc_remove)
            message(FATAL_ERROR "[U8G2] Could not perform remove operation (${tc_remove})")
        endif ()
    endif ()

    if (NOT EXISTS ${U8G2_DIR}/csrc)
        message(FATAL_ERROR "[U8G2] Could not locate csrc directory")
    endif ()

    file(GLOB U8G2_SRC_FILES "${U8G2_DIR}/csrc/*.c")
    file(GLOB U8G2_HEADER_FILES "${U8G2_DIR}/csrc/*.h")

    #file(GLOB U8G2_SRC_FILES "${U8G2_DIR}/csrc/*.c" "${U8G2_DIR}/cppsrc/*.cpp")
    #file(GLOB U8G2_HEADER_FILES "${U8G2_DIR}/csrc/*.h" "${U8G2_DIR}/cppsrc/*.h")

    file(COPY ${U8G2_HEADER_FILES} DESTINATION ${PROJECT_BINARY_DIR}/install/u8g2/include)

    set(U8G2_INCLUDE_DIR ${PROJECT_BINARY_DIR}/install/u8g2/include)

    #[[add_library(u8g2 SHARED ${U8G2_INCLUDE_DIR})
    set_target_properties(u8g2
            PROPERTIES
            LINKER_LANGUAGE C
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/install/u8g2/lib"
            )

    # be sure to include the current source directory for header files
    target_include_directories(u8g2 PRIVATE ${CMAKE_SOURCE_DIR}/lib/u8g2/csrc)
    #target_link_libraries(u8g2 ${CMAKE_THREAD_LIBS_INIT} crypt m rt)

    set(U8G2_LIBRARIES "${PROJECT_BINARY_DIR}/install/u8g2/lib/${CMAKE_SHARED_LIBRARY_PREFIX}u8g2${CMAKE_SHARED_LIBRARY_SUFFIX}")]]
endif ()