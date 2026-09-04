# SPDX-License-Identifier: Apache-2.0

target_sources(app PRIVATE display.c)
target_sources_ifdef(CONFIG_TELEMETRY_SHELL app PRIVATE display_shell.c)
