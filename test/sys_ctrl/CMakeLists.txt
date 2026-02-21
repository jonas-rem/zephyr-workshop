cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(sys_ctrl_test)

target_sources(app PRIVATE src/main.c)

# We want to test sys_ctrl in isolation
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../app/src/common common)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../app/src/modules/sys_ctrl sys_ctrl)

target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../app/include)
target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../app/src)
