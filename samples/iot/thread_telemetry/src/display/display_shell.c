/*
 * Copyright (c) 2026 TiaC Systems
 * SPDX-License-Identifier: Apache-2.0
 *
 * Shell bindings for the LED strip display. Compiled only when
 * CONFIG_TELEMETRY_SHELL is set; display.c stays free of shell code.
 */

#include <errno.h>
#include <string.h>

#include <zephyr/shell/shell.h>

#include "display.h"

static int cmd_display(const struct shell *sh, size_t argc, char **argv)
{
	if (argc == 1) {
		shell_print(sh, "%s", display_mode_name(display_mode_get()));
		return 0;
	}

	if (strcmp(argv[1], "temperature") == 0) {
		display_mode_set(DISPLAY_MODE_TEMPERATURE);
	} else if (strcmp(argv[1], "humidity") == 0) {
		display_mode_set(DISPLAY_MODE_HUMIDITY);
	} else {
		shell_error(sh, "Unknown quantity '%s', use temperature or humidity", argv[1]);
		return -EINVAL;
	}

	shell_print(sh, "Showing %s", display_mode_name(display_mode_get()));

	return 0;
}

SHELL_SUBCMD_ADD((telemetry), display, NULL,
		 "Show or set the displayed quantity: telemetry display [temperature|humidity]",
		 cmd_display, 1, 1);
