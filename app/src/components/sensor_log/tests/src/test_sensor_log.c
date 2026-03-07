/*
 * Copyright (c) 2026 Jonas Remmert
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>
#include <zephyr/zbus/zbus.h>

#include "message_channel.h"

/* Log entry types - must match sensor_log.c */
enum log_entry_type {
	LOG_ENTRY_STATE,
	LOG_ENTRY_SENSOR,
	LOG_ENTRY_ALERT,
};

/* Single log entry - must match sensor_log.c */
struct log_entry {
	int64_t timestamp;
	enum log_entry_type type;
	union {
		enum sys_states state;
		int32_t temp;
	} data;
};

/* Forward declaration of test APIs from sensor_log.c */
size_t sensor_log_get_entry_count(void);
int sensor_log_get_last_entry(struct log_entry *entry);
void sensor_log_reset(void);

/* Reset buffer before each test to ensure isolation */
static void sensor_log_test_before(void *fixture)
{
	ARG_UNUSED(fixture);
	sensor_log_reset();
	k_sleep(K_MSEC(10));
}

/* Test 1: Sensor readings are logged with correct content */
ZTEST(sensor_log_test_suite, test_sensor_reading_logged)
{
	struct event_msg msg = {
		.event = SYS_SENSOR_READING,
		.sensor = { .temp = 320 }
	};
	struct log_entry entry;
	int64_t timestamp_before = k_uptime_get();

	zbus_chan_pub(&event_ch, &msg, K_NO_WAIT);
	k_yield();

	/* Validate entry content - timestamp proves it was stored */
	zassert_ok(sensor_log_get_last_entry(&entry),
		   "Should be able to retrieve last entry");
	zassert_equal(entry.type, LOG_ENTRY_SENSOR,
		      "Entry type should be SENSOR");
	zassert_equal(entry.data.temp, 320,
		      "Temperature should be 320 (3.20°C)");
	zassert_true(entry.timestamp >= timestamp_before,
		     "Timestamp should be >= timestamp_before");
}

/* Test 2: Temperature alerts are logged with correct content */
ZTEST(sensor_log_test_suite, test_alert_logged)
{
	struct event_msg msg = {
		.event = SYS_TEMP_ALERT,
		.sensor = { .temp = 600 }
	};
	struct log_entry entry;
	int64_t timestamp_before = k_uptime_get();

	zbus_chan_pub(&event_ch, &msg, K_NO_WAIT);
	k_yield();

	/* Validate entry content - timestamp proves it was stored */
	zassert_ok(sensor_log_get_last_entry(&entry),
		   "Should be able to retrieve last entry");
	zassert_equal(entry.type, LOG_ENTRY_ALERT,
		      "Entry type should be ALERT");
	zassert_equal(entry.data.temp, 600,
		      "Temperature should be 600 (6.00°C)");
	zassert_true(entry.timestamp >= timestamp_before,
		     "Timestamp should be >= timestamp_before");
}

/* Test 3: State changes are logged with correct content */
ZTEST(sensor_log_test_suite, test_state_change_logged)
{
	enum sys_states state = SYS_ACTIVE;
	struct log_entry entry;
	int64_t timestamp_before = k_uptime_get();

	zbus_chan_pub(&sys_ctl_ch, &state, K_NO_WAIT);
	k_yield();

	/* Validate entry content - timestamp proves it was stored */
	zassert_ok(sensor_log_get_last_entry(&entry),
		   "Should be able to retrieve last entry");
	zassert_equal(entry.type, LOG_ENTRY_STATE,
		      "Entry type should be STATE");
	zassert_equal(entry.data.state, SYS_ACTIVE,
		      "State should be ACTIVE");
	zassert_true(entry.timestamp >= timestamp_before,
		     "Timestamp should be >= timestamp_before");
}

/* Test 4: Button events are ignored */
ZTEST(sensor_log_test_suite, test_button_ignored)
{
	struct event_msg msg = { .event = SYS_BUTTON_PRESSED };
	size_t count_before = sensor_log_get_entry_count();

	for (int i = 0; i < 5; i++) {
		zbus_chan_pub(&event_ch, &msg, K_NO_WAIT);
		k_yield();
	}

	zassert_equal(sensor_log_get_entry_count(), count_before,
		      "Button events should not be stored");
}

/* Test 5: Buffer wraps when full */
ZTEST(sensor_log_test_suite, test_buffer_wraps)
{
	struct event_msg msg = {
		.event = SYS_SENSOR_READING,
		.sensor = { .temp = 400 }
	};
	size_t buffer_size = CONFIG_SENSOR_LOG_BUFFER_SIZE;

	/* Fill buffer completely plus 2 more */
	for (size_t i = 0; i < buffer_size + 2; i++) {
		zbus_chan_pub(&event_ch, &msg, K_NO_WAIT);
		k_yield();
	}

	/* After wrap, count should equal buffer size, not buffer_size + 2 */
	zassert_equal(sensor_log_get_entry_count(), buffer_size,
		      "Buffer should wrap, count should equal buffer size");
}

ZTEST_SUITE(sensor_log_test_suite, NULL, NULL, sensor_log_test_before, NULL, NULL);
