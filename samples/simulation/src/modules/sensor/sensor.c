/*
 * Copyright (c) 2026 Jonas Remmert
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "message_channel.h"

LOG_MODULE_REGISTER(app_simulation_sensor, CONFIG_APP_SIM_SENSOR_LOG_LEVEL);

static const struct device *sensor_device;
static uint32_t read_count;
static uint32_t error_count;

ZBUS_MSG_SUBSCRIBER_DEFINE(button_subscriber);
ZBUS_CHAN_ADD_OBS(button_event_ch, button_subscriber, 1);

static int publish_event(enum sensor_event_state state, int error,
			 const struct sensor_value *temperature,
			 const struct sensor_value *humidity)
{
	struct sensor_event event = {
		.state = state,
		.error = error,
	};

	if (temperature != NULL) {
		event.temperature_milli_c = sensor_value_to_milli(temperature);
	}
	if (humidity != NULL) {
		event.humidity_milli_percent = sensor_value_to_milli(humidity);
	}

	return zbus_chan_pub(&sensor_event_ch, &event, K_MSEC(100));
}

static void perform_read(void)
{
	struct sensor_value temperature;
	struct sensor_value humidity;
	int err;

	LOG_DBG("Reading sensor");

	err = sensor_sample_fetch(sensor_device);
	if (err == 0) {
		err = sensor_channel_get(sensor_device, SENSOR_CHAN_AMBIENT_TEMP, &temperature);
	}
	if (err == 0) {
		err = sensor_channel_get(sensor_device, SENSOR_CHAN_HUMIDITY, &humidity);
	}

	if (err != 0) {
		LOG_ERR("Sensor read failed: %d", err);
		error_count++;
		publish_event(SENSOR_EVENT_ERROR, err, NULL, NULL);
		return;
	}

	read_count++;

	/* Narrowed to the width the sensor_event message carries. */
	LOG_DBG("Read %u: %d mC, %d m%%", read_count,
		(int32_t)sensor_value_to_milli(&temperature),
		(int32_t)sensor_value_to_milli(&humidity));

	err = publish_event(SENSOR_EVENT_SUCCESS, 0, &temperature, &humidity);
	if (err != 0) {
		LOG_ERR("Failed to publish sensor result: %d", err);
	}
}

static void sensor_thread(void *p1, void *p2, void *p3)
{
	const struct zbus_channel *channel;
	struct button_event event;
	int err;

	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	sensor_device = DEVICE_DT_GET_ONE(ti_hdc);
	if (!device_is_ready(sensor_device)) {
		LOG_ERR("Sensor device not ready");
		return;
	}

	LOG_INF("Sensor initialized");

	while (true) {
		err = zbus_sub_wait_msg(&button_subscriber, &channel, &event, K_FOREVER);
		if (err != 0) {
			LOG_ERR("Failed to receive button event: %d", err);
			continue;
		}

		if (channel != &button_event_ch) {
			continue;
		}

		if (event.type == BUTTON_EVENT_0_PRESSED) {
			perform_read();
		}
	}
}

K_THREAD_DEFINE(sensor_thread_id, 1024, sensor_thread, NULL, NULL, NULL,
		CONFIG_APP_SIM_SENSOR_THREAD_PRIORITY, 0, 0);

#ifdef CONFIG_APP_SIM_SENSOR_SHELL
#include <zephyr/shell/shell.h>

static int cmd_sensor_info(const struct shell *sh, size_t argc, char **argv)
{
	struct sensor_event event;
	int err;

	ARG_UNUSED(sh);
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	LOG_INF("device=%s ready=%s", sensor_device != NULL ? sensor_device->name : "none",
		(sensor_device != NULL && device_is_ready(sensor_device)) ? "yes" : "no");
	LOG_INF("reads_ok=%u reads_failed=%u", read_count, error_count);

	err = zbus_chan_read(&sensor_event_ch, &event, K_MSEC(100));
	if (err != 0) {
		LOG_ERR("Failed to read latest event: %d", err);
		return err;
	}

	LOG_INF("last_state=%s", sensor_event_state_str(event.state));
	if (event.state == SENSOR_EVENT_SUCCESS) {
		LOG_INF("temperature_milli_c=%d humidity_milli_percent=%d",
			event.temperature_milli_c, event.humidity_milli_percent);
	}

	return 0;
}

static int cmd_sensor_latest(const struct shell *sh, size_t argc, char **argv)
{
	struct sensor_event event;
	int err;

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	err = zbus_chan_read(&sensor_event_ch, &event, K_MSEC(100));
	if (err != 0) {
		return err;
	}

	if (event.state == SENSOR_EVENT_ERROR) {
		shell_print(sh, "Sensor state: ERROR (%d)", event.error);
		return 0;
	}

	shell_print(sh, "Sensor state: %s", sensor_event_state_str(event.state));
	if (event.state == SENSOR_EVENT_SUCCESS) {
		shell_print(sh, "Temperature: %s%d.%03d C",
			    event.temperature_milli_c < 0 ? "-" : "",
			    abs(event.temperature_milli_c / 1000),
			    abs(event.temperature_milli_c % 1000));
		shell_print(sh, "Humidity: %d.%03d %%",
			    event.humidity_milli_percent / 1000,
			    abs(event.humidity_milli_percent % 1000));
	}

	return 0;
}

static int cmd_sensor_read(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	if (sensor_device == NULL || !device_is_ready(sensor_device)) {
		shell_error(sh, "Sensor device not ready");
		return -ENODEV;
	}

	perform_read();
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sensor_commands,
	SHELL_CMD(read, NULL, "Read the sensor and publish the result", cmd_sensor_read),
	SHELL_CMD(latest, NULL, "Show the latest sensor lifecycle event", cmd_sensor_latest),
	SHELL_CMD(info, NULL, "Log sensor module state", cmd_sensor_info),
	SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(sensor, &sensor_commands, "Sensor commands", NULL);
#endif
