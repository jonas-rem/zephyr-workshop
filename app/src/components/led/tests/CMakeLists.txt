cmake_minimum_required(VERSION 3.20.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../../../cmake/workshop_board.cmake)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(led_test)

zephyr_include_directories(../../../common)

target_sources(app PRIVATE src/test_led.c)

# Include common message channel
add_subdirectory(../../../common common)

# Include LED component
if(CONFIG_LED_MODULE)
  add_subdirectory(.. led)
endif()
