# SPDX-License-Identifier: Apache-2.0

zephyr_library()
zephyr_library_sources(emul_ti_hdc.c)
zephyr_library_sources_ifdef(CONFIG_SHELL ti_hdc_emul_shell.c)
