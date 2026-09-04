/*
 * Copyright (c) 2026 TiaC Systems
 * SPDX-License-Identifier: Apache-2.0
 *
 * Shell bindings for the console log. Compiled only when
 * CONFIG_TELEMETRY_SHELL is set; sensor_console.c stays free of shell code.
 */

#include <zephyr/shell/shell.h>

#include "app_status.h"
#include "sensor_console.h"

static void sensor_console_status(const struct shell *sh)
{
	struct sensor_console_stats stats;

	sensor_console_stats_get(&stats);

	APP_STATUS_PRINT(sh, "Log every", "%u s, %u readings so far", stats.interval_s,
			 stats.readings);
}

static int cmd_console(const struct shell *sh, size_t argc, char **argv)
{
	struct sensor_console_stats stats;
	unsigned long seconds;
	int err = 0;
	int ret;

	if (argc == 1) {
		sensor_console_stats_get(&stats);

		APP_STATUS_PRINT(sh, "Log every", "%u s, next in %u ms", stats.interval_s,
				 stats.next_in_ms);
		APP_STATUS_PRINT(sh, "Readings", "%u logged, %u failed", stats.readings,
				 stats.failures);
		return 0;
	}

	seconds = shell_strtoul(argv[1], 10, &err);
	if (err) {
		shell_error(sh, "'%s' is not a number of seconds", argv[1]);
		return err;
	}

	ret = sensor_console_interval_set(seconds);
	if (ret) {
		shell_error(sh, "Interval must be %d to %d seconds", SENSOR_CONSOLE_INTERVAL_MIN_S,
			    SENSOR_CONSOLE_INTERVAL_MAX_S);
		return ret;
	}

	shell_print(sh, "Logging a reading every %lu s (not persisted)", seconds);

	return 0;
}

APP_STATUS_ENTRY_DEFINE(c_sensor_console, sensor_console_status);

SHELL_SUBCMD_ADD((telemetry), console, NULL,
		 "Show the console log stats or set its interval: telemetry console [<seconds>]",
		 cmd_console, 1, 1);
