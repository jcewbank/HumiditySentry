aux_source_directory(. DIR_SHT40_SRCS)
include_directories(../../pi_platform)
include_directories(../i2c_prj)

add_library(sht40 ${DIR_SHT40_SRCS})
target_link_libraries(sht40 PUBLIC pi_platform i2c_prj)
