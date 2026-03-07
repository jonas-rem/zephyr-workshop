cmake_minimum_required(VERSION 3.20.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../../../cmake/workshop_board.cmake)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(sensor_log_test)

zephyr_include_directories(../../../common)

target_sources(app PRIVATE src/test_sensor_log.c)

# Include common message channel
add_subdirectory(../../../common common)

# Include sensor_log component
if(CONFIG_SENSOR_LOG_COMPONENT)
  add_subdirectory(.. sensor_log)
endif()
