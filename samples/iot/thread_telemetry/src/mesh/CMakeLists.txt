# SPDX-License-Identifier: Apache-2.0

target_sources(app PRIVATE mesh.c)
target_sources_ifdef(CONFIG_TELEMETRY_SHELL app PRIVATE mesh_shell.c)
