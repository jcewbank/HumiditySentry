aux_source_directory(. DIR_PI_PLATFORM_SRCS)

add_library(pi_platform ${DIR_PI_PLATFORM_SRCS})
target_link_libraries(pi_platform 
    PUBLIC 
    pico_stdlib 
    hardware_i2c 
    hardware_pwm
    pico_cyw43_arch_none
    )
