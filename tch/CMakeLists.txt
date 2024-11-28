# Based on LCD library CMakeLists.txt
aux_source_directory(. DIR_TCH_SRCS)
include_directories(../lib/config)
include_directories(../lib/lcd)

add_library(tch ${DIR_TCH_SRCS})
target_link_libraries(tch PUBLIC config lcd)
