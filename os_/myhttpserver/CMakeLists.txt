cmake_minimum_required(VERSION 3.13)

project(myhttpserver C CXX ASM)

set(MBEDTLS_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/mbedtls_config.h")

set(CMAKE_VERBOSE_MAKEFILE ON)

set(MAKE_FS_DATA_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/makefsdata)

execute_process(COMMAND
        perl ${MAKE_FS_DATA_SCRIPT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        ECHO_OUTPUT_VARIABLE
        ECHO_ERROR_VARIABLE
)

file(RENAME fsdata.c my_fsdata.c)

add_library(myhttpserver myhttpserver.h myhttpserver.cpp)

include(../pico_sdk_import.cmake)
pico_sdk_init()

# Add include directories for Pico_code libraries
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../Pico_code/c/lib/Config
    ${CMAKE_CURRENT_SOURCE_DIR}/../Pico_code/c/lib/GUI
    ${CMAKE_CURRENT_SOURCE_DIR}/../Pico_code/c/lib/Icm20948
    ${CMAKE_CURRENT_SOURCE_DIR}/../Pico_code/c/lib/LCD
    ${CMAKE_CURRENT_SOURCE_DIR}/../Pico_code/c/lib/Infrared
    ${CMAKE_CURRENT_SOURCE_DIR}/../Pico_code/c/lib/OLED
    ${CMAKE_CURRENT_SOURCE_DIR}/../Pico_code/c/lib/Fonts
    ${CMAKE_CURRENT_SOURCE_DIR}/../Pico_code/c/examples
)

target_include_directories(myhttpserver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(myhttpserver
    pico_stdlib
    pico_cyw43_arch_lwip_poll
    pico_lwip_http
    pico_lwip_sntp
    pico_lwip_mbedtls
    pico_mbedtls
    hardware_spi
    Config
    Infrared
    OLED
    Icm20948
    LCD
    Fonts
    GUI
    examples
)