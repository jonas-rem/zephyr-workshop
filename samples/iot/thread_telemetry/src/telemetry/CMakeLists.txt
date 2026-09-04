# SPDX-License-Identifier: Apache-2.0

target_sources(app PRIVATE telemetry.c)
target_sources_ifdef(CONFIG_TELEMETRY_SHELL app PRIVATE telemetry_shell.c)
