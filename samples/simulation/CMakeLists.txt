cmake_minimum_required(VERSION 3.20.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/workshop_board.cmake)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(app_simulation)

zephyr_include_directories(src/common)

target_sources(app PRIVATE src/main.c)
add_subdirectory(src/common common)

if(CONFIG_APP_TEST_BUTTON)
  add_subdirectory(src/modules/button button)
endif()

if(CONFIG_APP_TEST_SENSOR)
  add_subdirectory(src/modules/sensor sensor)
endif()

if(CONFIG_APP_TEST_LED)
  add_subdirectory(src/modules/led led)
endif()
