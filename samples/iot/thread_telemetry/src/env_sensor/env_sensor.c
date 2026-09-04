/*
 * Copyright (c) 2026 TiaC Systems
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "env_sensor.h"

LOG_MODULE_REGISTER(env_sensor, CONFIG_TELEMETRY_ENV_SENSOR_LOG_LEVEL);

static const struct device *const sensor = DEVICE_DT_GET(DT_NODELABEL(sht4x));

/* The display and the publish path both sample; the SHT4x driver is not reentrant. */
static K_MUTEX_DEFINE(sensor_lock);

/* Called with sensor_lock held, so it can simply return on the first error. */
static int sensor_sample(struct sensor_value *temp, struct sensor_value *hum)
{
	int ret;

	ret = sensor_sample_fetch(sensor);
	if (ret) {
		LOG_ERR("Failed to fetch a sample: %d", ret);
		return ret;
	}

	ret = sensor_channel_get(sensor, SENSOR_CHAN_AMBIENT_TEMP, temp);
	if (ret) {
		LOG_ERR("Failed to read the temperature: %d", ret);
		return ret;
	}

	ret = sensor_channel_get(sensor, SENSOR_CHAN_HUMIDITY, hum);
	if (ret) {
		LOG_ERR("Failed to read the humidity: %d", ret);
		return ret;
	}

	return 0;
}

int env_sensor_read(struct sensor_value *temp, struct sensor_value *hum)
{
	int ret;

	if (temp == NULL || hum == NULL) {
		LOG_ERR("No buffer to read the sensor into");
		return -EINVAL;
	}

	if (!device_is_ready(sensor)) {
		LOG_ERR("Sensor %s is not ready", sensor->name);
		return -ENODEV;
	}

	/* The lock is taken and released in one place, around a single call. */
	k_mutex_lock(&sensor_lock, K_FOREVER);
	ret = sensor_sample(temp, hum);
	k_mutex_unlock(&sensor_lock);

	if (ret == 0) {
		LOG_DBG("Raw reading: temp %d.%06d, hum %d.%06d", temp->val1, temp->val2, hum->val1,
			hum->val2);
	}

	return ret;
}

static int env_sensor_init(void)
{
	if (!device_is_ready(sensor)) {
		LOG_ERR("Sensor %s is not ready", sensor->name);
		return -ENODEV;
	}

	LOG_INF("Reading %s", sensor->name);
	LOG_DBG("Every reader takes the sensor lock, so the display, the console and the "
		"publish path never overlap on the bus");

	return 0;
}

SYS_INIT(env_sensor_init, APPLICATION, CONFIG_TELEMETRY_ENV_SENSOR_INIT_PRIORITY);
