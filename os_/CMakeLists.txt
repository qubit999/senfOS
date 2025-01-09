# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(os_ C CXX ASM)

set(PICO_CXX_ENABLE_EXCEPTIONS 1)

set(PICO_CXX_ENABLE_RTTI 1)

set(PICO_DEFAULT_LINKER_SCRIPT
${CMAKE_CURRENT_LIST_DIR}/memmap_custom_4MB.ld
)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_subdirectory(Pico_code/c/lib/Config)
add_subdirectory(Pico_code/c/lib/Infrared)
add_subdirectory(Pico_code/c/lib/OLED)
add_subdirectory(Pico_code/c/lib/Icm20948)
add_subdirectory(Pico_code/c/lib/LCD)
add_subdirectory(Pico_code/c/lib/Fonts)
add_subdirectory(Pico_code/c/lib/GUI)
add_subdirectory(Pico_code/c/examples)

include_directories(./Pico_code/c/lib/Config)
include_directories(./Pico_code/c/lib/GUI)
include_directories(./Pico_code/c/lib/Icm20948)
include_directories(./Pico_code/c/lib/LCD)
include_directories(./Pico_code/c/lib/Infrared)
include_directories(./Pico_code/c/lib/OLED)
include_directories(./Pico_code/c/lib/Fonts)
include_directories(./Pico_code/c/examples)

# add_subdirectory(Pico_ePaper_Code/c/lib/Config)
# add_subdirectory(Pico_ePaper_Code/c/lib/e-Paper)
# add_subdirectory(Pico_ePaper_Code/c/lib/Fonts)
# add_subdirectory(Pico_ePaper_Code/c/lib/GUI)
# add_subdirectory(Pico_ePaper_Code/c/examples)

# include_directories(./Pico_ePaper_Code/c/examples)
# include_directories(./Pico_ePaper_Code/c/lib/Config)
# include_directories(./Pico_ePaper_Code/c/lib/GUI)
# include_directories(./Pico_ePaper_Code/c/lib/e-Paper)

# Add my libraries
add_subdirectory(libs/)
add_subdirectory(myhttpserver/)
include_directories(./libs/)
include_directories(./myhttpserver/)
# End of my libraries

# Enable C++ exceptions
add_compile_options(-fexceptions)

add_executable(os_ main.cpp settings.h)

pico_set_program_name(os_ "senfOS")
pico_set_program_version(os_ "0.2")

set(PICO_DEFAULT_USB_BAUD_RATE 115200)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(os_ 0)
pico_enable_stdio_usb(os_ 1)

set(SDK_PATH ${PICO_SDK_PATH})
include_directories(${SDK_PATH}/usr/include)
link_directories(${SDK_PATH}/usr/lib)

# Add the standard include files to the build
target_include_directories(os_ PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${PICO_SDK_PATH}
    ${PICO_SDK_PATH}/lib/lwip/src/include
    ${PICO_SDK_PATH}/src/rp2_common/pico_lwip/include
)

# Add any user requested libraries
target_link_libraries(os_ 
    pico_stdlib
    pico_cyw43_arch_lwip_threadsafe_background
    hardware_spi 
    hardware_i2c 
    hardware_dma
    hardware_pwm
    hardware_pio 
    hardware_interp 
    hardware_timer 
    hardware_watchdog 
    hardware_clocks
    # examples
    # ePaper
    # GUI
    # Fonts
    # Config
    blink
    mywifi
    myhttp
    myhttpserver
    hotspot
)

pico_add_extra_outputs(os_)