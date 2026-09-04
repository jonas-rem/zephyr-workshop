# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake/workshop_board.cmake)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(thread_telemetry)

# src/ holds the application scaffolding, and every module keeps its header
# next to its implementation.
zephyr_include_directories(
  src
  src/display
  src/env_sensor
  src/mesh
  src/node_name
  src/sensor_console
  src/telemetry
  )

target_sources(app PRIVATE src/main.c)
target_sources_ifdef(CONFIG_TELEMETRY_SHELL app PRIVATE src/app_shell.c)

# "telemetry status" is assembled by the linker out of whatever modules were
# built, rather than from a list in the command itself. See src/app_status.h.
zephyr_linker_sources(SECTIONS sections-rom.ld)
zephyr_iterable_section(NAME app_status_entry KVMA RAM_REGION GROUP RODATA_REGION)

# One directory per module, each with its own Kconfig and CMakeLists. A module
# that is switched off is not compiled at all, which is why the sources are
# listed per module rather than globbed. The order sets the order of the
# "telemetry status" lines.
add_subdirectory(src/env_sensor)
add_subdirectory_ifdef(CONFIG_TELEMETRY_NODE_NAME  src/node_name)
add_subdirectory_ifdef(CONFIG_TELEMETRY_SENSOR_CONSOLE src/sensor_console)
add_subdirectory_ifdef(CONFIG_TELEMETRY_DISPLAY    src/display)
add_subdirectory_ifdef(CONFIG_TELEMETRY_MESH       src/mesh)
add_subdirectory_ifdef(CONFIG_TELEMETRY_PUBLISH    src/telemetry)
add_subdirectory_ifdef(CONFIG_TELEMETRY_STATUS_LED src/status_led)
