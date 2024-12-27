cmake_minimum_required(VERSION 3.13)

project(mywifi)

add_library(mywifi mywifi.h mywifi.cpp)

include(../pico_sdk_import.cmake)
pico_sdk_init()

target_include_directories(mywifi PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}

)

target_link_libraries(mywifi
    pico_stdlib
    pico_cyw43_arch_lwip_poll
    pico_lwip
    pico_lwip_core6
)

