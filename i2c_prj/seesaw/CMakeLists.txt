aux_source_directory(. DIR_SEESAW_SRCS)
include_directories(../../pi_platform)

add_library(seesaw ${DIR_SEESAW_SRCS})
target_link_libraries(seesaw PUBLIC pi_platform)
