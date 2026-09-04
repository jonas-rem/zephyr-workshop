# SPDX-License-Identifier: Apache-2.0

target_sources(app PRIVATE sensor_console.c)
target_sources_ifdef(CONFIG_TELEMETRY_SHELL app PRIVATE sensor_console_shell.c)
