cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(led_sample)

target_sources(app PRIVATE src/main.c)
add_subdirectory(./../../app/src/common common)
add_subdirectory(./../../app/src/modules/led led)
