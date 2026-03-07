/*
 * Copyright (c) 2026 Jonas Remmert
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>
#include <zephyr/zbus/zbus.h>

#include "message_channel.h"

/* Counter for received sensor readings */
static uint8_t reading_count;

/* Last temperature reading for validation (in 0.01°C units) */
static int32_t last_temperature;

/* ZBus observer to catch sensor reading events */
static void tempsense_test_cb(const struct zbus_channel *chan)
{
	const struct event_msg *msg = zbus_chan_const_msg(chan);

	if (msg->event == SYS_SENSOR_READING) {
		reading_count++;
		last_temperature = msg->sensor.temp;
	}
}

ZBUS_LISTENER_DEFINE(tempsense_test_listener, tempsense_test_cb);
ZBUS_CHAN_ADD_OBS(event_ch, tempsense_test_listener, 2);

static void *tempsense_test_setup(void)
{
	enum sys_states state;

	reading_count = 0;
	last_temperature = 0;

	/* Ensure system starts in SLEEP state */
	state = SYS_SLEEP;
	zbus_chan_pub(&sys_ctl_ch, &state, K_NO_WAIT);
	k_sleep(K_MSEC(50));

	return NULL;
}

/* Test that tempsense responds to ACTIVE state (starts sampling) */
ZTEST(tempsense_test_suite, test_tempsense_start_sampling)
{
	enum sys_states state;

	/* Transition to ACTIVE */
	state = SYS_ACTIVE;
	zbus_chan_pub(&sys_ctl_ch, &state, K_NO_WAIT);

	/* Wait long enough for at least one reading (100ms interval + margin) */
	k_sleep(K_MSEC(150));

	/* Should have received sensor readings while in ACTIVE state */
	zassert_true(reading_count > 0,
		     "Expected readings in ACTIVE state, got %d", reading_count);

	/* Validate temperature is in expected range (3-7°C = 300-700 centidegrees) */
	zassert_true(last_temperature >= 300 && last_temperature <= 700,
		     "Temperature %d out of expected range (300-700)", last_temperature);
}

/* Test that tempsense responds to SLEEP state (stops sampling) */
ZTEST(tempsense_test_suite, test_tempsense_stop_sampling)
{
	enum sys_states state;
	uint8_t count_before;

	/* Start in ACTIVE state and verify readings happen first */
	state = SYS_ACTIVE;
	zbus_chan_pub(&sys_ctl_ch, &state, K_NO_WAIT);
	k_sleep(K_MSEC(150));

	/* Verify we got readings before testing stop */
	zassert_true(reading_count > 0,
		     "Should have readings before testing stop, got %d", reading_count);

	/* Reset counter and transition to SLEEP */
	reading_count = 0;
	state = SYS_SLEEP;
	zbus_chan_pub(&sys_ctl_ch, &state, K_NO_WAIT);
	k_sleep(K_MSEC(50));

	/* Record count and wait briefly */
	count_before = reading_count;
	k_sleep(K_MSEC(200));

	/* Should not have received new readings while sleeping */
	zassert_equal(reading_count, count_before,
		     "Should not receive readings in SLEEP state");
}

ZTEST_SUITE(tempsense_test_suite, NULL, tempsense_test_setup, NULL, NULL, NULL);
