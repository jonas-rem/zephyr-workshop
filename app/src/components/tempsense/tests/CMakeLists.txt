cmake_minimum_required(VERSION 3.20.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../../../cmake/workshop_board.cmake)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(tempsense_test)

zephyr_include_directories(../../../common)

target_sources(app PRIVATE src/test_tempsense.c)

# Include common message channel
add_subdirectory(../../../common common)

# Include tempsense component
if(CONFIG_TEMPSENSE_COMPONENT)
  add_subdirectory(.. tempsense)
endif()
