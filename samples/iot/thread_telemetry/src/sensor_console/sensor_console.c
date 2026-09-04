/*
 * Copyright (c) 2026 TiaC Systems
 * SPDX-License-Identifier: Apache-2.0
 *
 * The whole node in the first stage of the workshop: read the sensor every
 * few seconds and print the reading. No display, no radio.
 */

#include <errno.h>

#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/atomic.h>

#include "env_sensor.h"
#include "sensor_console.h"

LOG_MODULE_REGISTER(sensor_console, CONFIG_TELEMETRY_SENSOR_CONSOLE_LOG_LEVEL);

static void sensor_console_work_handler(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(sensor_console_work, sensor_console_work_handler);

/* The shell changes the interval and reads the counters while the work runs. */
static atomic_t interval_s = ATOMIC_INIT(CONFIG_TELEMETRY_SENSOR_CONSOLE_INTERVAL_S);
static atomic_t readings;
static atomic_t failures;

static void sensor_console_work_handler(struct k_work *work)
{
	char temp_str[ENV_SENSOR_STR_SIZE];
	char hum_str[ENV_SENSOR_STR_SIZE];
	struct sensor_value temp;
	struct sensor_value hum;
	int ret;

	ARG_UNUSED(work);

	ret = env_sensor_read(&temp, &hum);
	if (ret == 0) {
		atomic_inc(&readings);
		LOG_INF("%s C, %s %%rH", env_sensor_str(temp_str, sizeof(temp_str), &temp),
			env_sensor_str(hum_str, sizeof(hum_str), &hum));
	} else {
		/* The sensor module has logged why; here it only counts. */
		atomic_inc(&failures);
		LOG_DBG("Reading skipped, %ld failures so far", atomic_get(&failures));
	}

	k_work_reschedule(&sensor_console_work, K_SECONDS(atomic_get(&interval_s)));
}

int sensor_console_interval_set(uint32_t seconds)
{
	if (seconds < SENSOR_CONSOLE_INTERVAL_MIN_S || seconds > SENSOR_CONSOLE_INTERVAL_MAX_S) {
		LOG_ERR("Log interval %u s is outside %d..%d s", seconds,
			SENSOR_CONSOLE_INTERVAL_MIN_S, SENSOR_CONSOLE_INTERVAL_MAX_S);
		return -EINVAL;
	}

	atomic_set(&interval_s, seconds);
	LOG_INF("Logging a reading every %u s from now on", seconds);

	/* Log one now; the handler reschedules with the new interval. */
	k_work_reschedule(&sensor_console_work, K_NO_WAIT);

	return 0;
}

void sensor_console_stats_get(struct sensor_console_stats *stats)
{
	k_ticks_t remaining;

	if (stats == NULL) {
		LOG_ERR("No buffer to fill the console stats into");
		return;
	}

	remaining = k_work_delayable_remaining_get(&sensor_console_work);

	stats->interval_s = atomic_get(&interval_s);
	stats->next_in_ms = (uint32_t)k_ticks_to_ms_floor64(remaining);
	stats->readings = atomic_get(&readings);
	stats->failures = atomic_get(&failures);
}

static int sensor_console_init(void)
{
	LOG_INF("Logging a reading every %d s", CONFIG_TELEMETRY_SENSOR_CONSOLE_INTERVAL_S);
	LOG_DBG("Periodic read runs on the system workqueue, first one now");

	k_work_reschedule(&sensor_console_work, K_NO_WAIT);

	return 0;
}

SYS_INIT(sensor_console_init, APPLICATION, CONFIG_TELEMETRY_SENSOR_CONSOLE_INIT_PRIORITY);
