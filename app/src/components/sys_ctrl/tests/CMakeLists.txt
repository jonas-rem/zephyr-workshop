cmake_minimum_required(VERSION 3.20.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../../../cmake/workshop_board.cmake)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(sys_ctrl_test)

zephyr_include_directories(../../../common)

target_sources(app PRIVATE src/test_sys_ctrl.c)

# Include common message channel
add_subdirectory(../../../common common)

# Include sys_ctrl component
if(CONFIG_SYS_CTRL_COMPONENT)
  add_subdirectory(.. sys_ctrl)
endif()
