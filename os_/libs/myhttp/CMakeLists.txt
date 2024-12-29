cmake_minimum_required(VERSION 3.13)

# Set the project name
project(myhttp)

# Add the library
add_library(myhttp myhttp.h myhttp.cpp)

include(../pico_sdk_import.cmake)
pico_sdk_init()

target_include_directories(myhttp PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PICO_MBEDTLS_PATH}/include
)

target_link_libraries(myhttp
    pico_stdlib
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip
    pico_lwip_http
    pico_lwip_netif
    pico_lwip_core6
)