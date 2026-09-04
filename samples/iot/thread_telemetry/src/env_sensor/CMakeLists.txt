# SPDX-License-Identifier: Apache-2.0

target_sources(app PRIVATE env_sensor.c)
target_sources_ifdef(CONFIG_TELEMETRY_SHELL app PRIVATE env_sensor_shell.c)
