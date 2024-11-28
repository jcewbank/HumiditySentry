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
# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(HumiditySentry C CXX ASM)

set(PICO_CXX_ENABLE_EXCEPTIONS 1)

set(PICO_CXX_ENABLE_RTTI 1)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# From LCD library
add_subdirectory(lib/config)
add_subdirectory(lib/lcd)
add_subdirectory(lib/font)
add_subdirectory(lib/fatfs)
add_subdirectory(lib/sdcard)
include_directories(./lib/config)
include_directories(./lib/lcd)
include_directories(./lib/font)
include_directories(./lib/fatfs)
include_directories(./lib/sdcard)

# New libs
add_subdirectory(ui)
include_directories(./ui)
add_subdirectory(tch)
include_directories(./tch)
add_subdirectory(i2c_prj)
include_directories(./i2c_prj)
add_subdirectory(i2c_prj/sht40)
include_directories(./i2c_prj/sht40)
add_subdirectory(i2c_prj/seesaw)
include_directories(./i2c_prj/seesaw)
add_subdirectory(pi_platform)
include_directories(pi_platform)
add_subdirectory(humidifier)
include_directories(./humidifier)

# Add executable. Default name is the project name, version 0.1

add_executable(HumiditySentry 
    HumiditySentry.c
    )

pico_set_program_name(HumiditySentry "HumiditySentry")
pico_set_program_version(HumiditySentry "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(HumiditySentry 1)
pico_enable_stdio_usb(HumiditySentry 0)

# Add the standard library to the build
target_link_libraries(HumiditySentry
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(HumiditySentry PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

# Add any user requested libraries
target_link_libraries(HumiditySentry 
        hardware_i2c
        hardware_timer
        hardware_clocks
        hardware_spi
        hardware_pwm
        hardware_dma
        pico_cyw43_arch_none
        pico_multicore
        )

pico_add_extra_outputs(HumiditySentry)

target_link_libraries(
    HumiditySentry 
    lcd 
    font
    config
    pico_stdlib
    pico_malloc
    fatfs
    ui
    tch
    i2c_prj
    sht40
    seesaw
    pi_platform
    humidifier
    )

# Set 180MHz system clock
# target_compile_definitions(HumiditySentry PRIVATE
#         PLL_SYS_REFDIV=1
#         PLL_SYS_VCO_FREQ_HZ=1440000000
#         PLL_SYS_POSTDIV1=4
#         PLL_SYS_POSTDIV2=2
# )

# Set 125MHz system clock (default)
target_compile_definitions(HumiditySentry PRIVATE
        PLL_SYS_REFDIV=1
        PLL_SYS_VCO_FREQ_HZ=1500000000
        PLL_SYS_POSTDIV1=6
        PLL_SYS_POSTDIV2=2
)

# Set 8MHz system clock
# Requires non-default reference divider value
# target_compile_definitions(HumiditySentry PRIVATE
#         PLL_SYS_REFDIV=2
#         PLL_SYS_VCO_FREQ_HZ=882000000
#         PLL_SYS_POSTDIV1=7
#         PLL_SYS_POSTDIV2=7
# )
