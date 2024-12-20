cmake_minimum_required(VERSION 3.13)

project(hotspot)

add_library(hotspot hotspot.h hotspot.cpp)

include(../pico_sdk_import.cmake)
pico_sdk_init()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../myhttpserver/
)

target_include_directories(hotspot PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(hotspot
    pico_stdlib
    pico_cyw43_arch_lwip_threadsafe_background
    myhttpserver
)

