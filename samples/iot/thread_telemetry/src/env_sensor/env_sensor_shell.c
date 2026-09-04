/*
 * Copyright (c) 2026 TiaC Systems
 * SPDX-License-Identifier: Apache-2.0
 *
 * Shell bindings for the sensor. Compiled only when CONFIG_TELEMETRY_SHELL is
 * set; env_sensor.c stays free of shell code.
 */

#include <zephyr/shell/shell.h>

#include "app_status.h"
#include "env_sensor.h"

static int print_reading(const struct shell *sh, bool as_status)
{
	char temp_str[ENV_SENSOR_STR_SIZE];
	char hum_str[ENV_SENSOR_STR_SIZE];
	struct sensor_value temp;
	struct sensor_value hum;
	int ret;

	ret = env_sensor_read(&temp, &hum);
	if (ret) {
		if (as_status) {
			APP_STATUS_PRINT(sh, "Sensor", "unavailable (%d)", ret);
		} else {
			shell_error(sh, "Failed to read the sensor: %d", ret);
		}

		return ret;
	}

	APP_STATUS_PRINT(sh, "Sensor", "%s C, %s %%rH",
			 env_sensor_str(temp_str, sizeof(temp_str), &temp),
			 env_sensor_str(hum_str, sizeof(hum_str), &hum));

	return 0;
}

static void sensor_status(const struct shell *sh)
{
	(void)print_reading(sh, true);
}

static int cmd_sensor(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	return print_reading(sh, false);
}

APP_STATUS_ENTRY_DEFINE(a_sensor, sensor_status);

SHELL_SUBCMD_ADD((telemetry), sensor, NULL, "Read the sensor once and print it", cmd_sensor, 1, 0);
