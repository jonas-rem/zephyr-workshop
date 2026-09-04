# SPDX-License-Identifier: Apache-2.0

target_sources(app PRIVATE node_name.c)
target_sources_ifdef(CONFIG_TELEMETRY_SHELL app PRIVATE node_name_shell.c)
