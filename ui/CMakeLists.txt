# Based on LCD library CMakeLists.txt
aux_source_directory(. DIR_UI_SRCS)
include_directories(../lib/config)
include_directories(../lib/lcd)
include_directories(../lib/font)
include_directories(../i2c_prj/sht40)
include_directories(../i2c_prj/stemma)
include_directories(../tch)

add_library(ui ${DIR_UI_SRCS})
target_link_libraries(ui PUBLIC config lcd font sht40 seesaw i2c_prj tch)
