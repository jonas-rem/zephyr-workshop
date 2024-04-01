cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(application)

target_sources(app PRIVATE src/main.c)
add_subdirectory(src/common common)
add_subdirectory(src/modules/led led)
add_subdirectory(src/modules/button button)
