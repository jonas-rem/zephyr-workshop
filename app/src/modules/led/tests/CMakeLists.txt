cmake_minimum_required(VERSION 3.20.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../../../cmake/workshop_board.cmake)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(app_test_led_test)

zephyr_include_directories(../../../common)
target_sources(app PRIVATE src/test_led.c)
add_subdirectory(../../../common common)
add_subdirectory(.. led)
