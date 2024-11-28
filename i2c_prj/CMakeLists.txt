aux_source_directory(. DIR_I2C_PRJ_SRCS)
include_directories(../pi_platform)

add_library(i2c_prj ${DIR_I2C_PRJ_SRCS})
target_link_libraries(i2c_prj PUBLIC pi_platform)

