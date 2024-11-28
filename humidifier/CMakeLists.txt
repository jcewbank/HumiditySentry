aux_source_directory(. DIR_HUMIDIFIER_SRCS)
include_directories(../i2c_prj/seesaw)
include_directories(../i2c_prj)

add_library(humidifier ${DIR_HUMIDIFIER_SRCS})
target_link_libraries(humidifier
    PUBLIC
    pico_stdlib
    seesaw
    i2c_prj
    )
