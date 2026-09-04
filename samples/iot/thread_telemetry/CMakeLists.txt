# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/workshop_board.cmake)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(thread_telemetry)

# Every module keeps its header next to its implementation.
zephyr_include_directories(
  src/display
  src/env_sensor
  src/mesh
  src/node_name
  src/telemetry
  )

target_sources(app PRIVATE src/main.c)

# One directory per module, each with its own Kconfig and CMakeLists. The
# sources are listed there rather than globbed here, so that a module -- and
# its shell bindings -- can be left out of the build entirely.
add_subdirectory(src/env_sensor)
add_subdirectory(src/mesh)
add_subdirectory(src/node_name)
add_subdirectory(src/telemetry)
add_subdirectory_ifdef(CONFIG_TELEMETRY_DISPLAY src/display)
add_subdirectory_ifdef(CONFIG_TELEMETRY_STATUS_LED src/status_led)
