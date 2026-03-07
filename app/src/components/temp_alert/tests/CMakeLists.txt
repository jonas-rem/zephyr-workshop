cmake_minimum_required(VERSION 3.20.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../../../cmake/workshop_board.cmake)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(temp_alert_test)

zephyr_include_directories(../../../common)

target_sources(app PRIVATE src/test_temp_alert.c)

# Include common message channel
add_subdirectory(../../../common common)

# Include temp_alert component
if(CONFIG_TEMP_ALERT_COMPONENT)
  add_subdirectory(.. temp_alert)
endif()
