# SPDX-FileCopyrightText: Copyright (c) 2026 Navimatix GmbH
# SPDX-FileCopyrightText: Copyright (c) 2026 TiaC Systems
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

# Standard Zephyr application boilerplate.
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project("RGB Matrix Level" VERSION 0.1)

assert(CONFIG_DISPLAY "Zephyr's display driver API must be enabled")
assert(CONFIG_SENSOR "Zephyr's sensor driver API must be enabled")

target_sources(app PRIVATE
    src/main.c
    src/bubble.c
    src/matrix.c
    src/tilt.c
)

target_include_directories(app PRIVATE
    src
)
