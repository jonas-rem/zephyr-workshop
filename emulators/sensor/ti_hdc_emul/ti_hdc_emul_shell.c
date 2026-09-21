/*
 * Copyright (c) 2026 Jonas Remmert
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT ti_hdc

#include <zephyr/drivers/emul.h>
#include <zephyr/drivers/emul_sensor.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/util.h>

static int parse_micro(const char *str, int64_t *value)
{
	const char *pos = str;
	uint64_t whole = 0;
	uint64_t fraction = 0;
	uint32_t fraction_digits = 0;
	bool negative = false;

	if (*pos == '-') {
		negative = true;
		pos++;
	} else if (*pos == '+') {
		pos++;
	}

	if (*pos < '0' || *pos > '9') {
		return -EINVAL;
	}

	while (*pos >= '0' && *pos <= '9') {
		whole = whole * 10 + (*pos++ - '0');
		if (whole > INT32_MAX) {
			return -ERANGE;
		}
	}

	if (*pos == '.') {
		pos++;
		while (*pos >= '0' && *pos <= '9') {
			if (fraction_digits == 6) {
				return -EINVAL;
			}
			fraction = fraction * 10 + (*pos++ - '0');
			fraction_digits++;
		}
	}

	if (*pos != '\0') {
		return -EINVAL;
	}

	while (fraction_digits++ < 6) {
		fraction *= 10;
	}

	*value = whole * 1000000 + fraction;
	if (negative) {
		*value = -*value;
	}

	return 0;
}

static int set_channel(const struct shell *sh, const char *value_str, enum sensor_channel channel,
		       const char *name)
{
	const struct emul *emul = EMUL_DT_GET(DT_DRV_INST(0));
	struct sensor_chan_spec ch = {
		.chan_type = channel,
		.chan_idx = 0,
	};
	q31_t lower;
	q31_t upper;
	q31_t epsilon;
	q31_t value;
	int64_t scaled;
	int64_t micro;
	int8_t shift;
	int err;

	err = parse_micro(value_str, &micro);
	if (err) {
		shell_error(sh, "Invalid value: %s", value_str);
		return err;
	}

	err = emul_sensor_backend_get_sample_range(emul, ch, &lower, &upper, &epsilon, &shift);
	if (err) {
		shell_error(sh, "Channel is not supported: %d", err);
		return err;
	}

	scaled = (micro / 1000000) * BIT64(31);
	scaled += ((micro % 1000000) * BIT64(31)) / 1000000;
	scaled /= BIT64(shift);
	if (scaled < INT32_MIN || scaled > INT32_MAX) {
		shell_error(sh, "Value is outside the supported range");
		return -ERANGE;
	}
	value = scaled;
	err = emul_sensor_backend_set_channel(emul, ch, &value, shift);
	if (err) {
		shell_error(sh, "Value is outside the supported range: %d", err);
		return err;
	}

	shell_print(sh, "%s set to %s", name, value_str);
	return 0;
}

static int cmd_set_temp(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);

	return set_channel(sh, argv[1], SENSOR_CHAN_AMBIENT_TEMP, "Temperature");
}

static int cmd_set_humidity(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);

	return set_channel(sh, argv[1], SENSOR_CHAN_HUMIDITY, "Humidity");
}

SHELL_STATIC_SUBCMD_SET_CREATE(sensor_emul_set_cmds,
			       SHELL_CMD_ARG(temp, NULL, "Set temperature in degrees Celsius",
					     cmd_set_temp, 2, 0),
			       SHELL_CMD_ARG(humidity, NULL, "Set relative humidity in percent",
					     cmd_set_humidity, 2, 0),
			       SHELL_SUBCMD_SET_END);

SHELL_STATIC_SUBCMD_SET_CREATE(sensor_emul_cmds,
			       SHELL_CMD(set, &sensor_emul_set_cmds,
					 "Set an emulated sensor channel", NULL),
			       SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(sensor_emul, &sensor_emul_cmds, "Sensor emulator commands", NULL);
