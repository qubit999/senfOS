cmake_minimum_required(VERSION 3.13)

# Set the project name
project(blink)

# Add the library
add_library(blink blink.h blink.cpp)

include(../pico_sdk_import.cmake)
pico_sdk_init()

target_include_directories(blink PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(blink
    pico_stdlib
    hardware_spi 
    hardware_i2c 
    hardware_dma 
    hardware_pio 
    hardware_interp 
    hardware_timer 
    hardware_watchdog 
    hardware_clocks 
    pico_cyw43_arch_lwip_threadsafe_background
)