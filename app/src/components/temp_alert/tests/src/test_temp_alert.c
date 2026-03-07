/*
 * Copyright (c) 2026 Jonas Remmert
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>
#include <zephyr/zbus/zbus.h>

#include "message_channel.h"

/* Counter for received alert events */
static uint8_t alert_count;
static int32_t last_alert_temp;

/* ZBus observer to catch alert events */
static void temp_alert_test_cb(const struct zbus_channel *chan)
{
	const struct event_msg *msg = zbus_chan_const_msg(chan);

	if (msg->event == SYS_TEMP_ALERT) {
		alert_count++;
		last_alert_temp = msg->sensor.temp;
	}
}

ZBUS_LISTENER_DEFINE(temp_alert_test_listener, temp_alert_test_cb);
ZBUS_CHAN_ADD_OBS(event_ch, temp_alert_test_listener, 2);

static void *temp_alert_test_setup(void)
{
	alert_count = 0;
	last_alert_temp = 0;
	k_sleep(K_MSEC(50));
	return NULL;
}

/* Test 1: Single reading above threshold should not trigger alert */
ZTEST(temp_alert_test_suite, test_temp_alert_single_no_alert)
{
	int32_t threshold = CONFIG_TEMP_ALERT_THRESHOLD;
	struct event_msg high_msg = {
		.event = SYS_SENSOR_READING,
		.sensor = { .temp = threshold + 100 } /* 1°C above threshold */
	};

	zbus_chan_pub(&event_ch, &high_msg, K_NO_WAIT);
	k_yield();

	zassert_equal(alert_count, 0, "Single reading should not trigger alert");
}

/* Test 2: Two consecutive readings above threshold should trigger alert */
ZTEST(temp_alert_test_suite, test_temp_alert_two_consecutive_triggers)
{
	int32_t threshold = CONFIG_TEMP_ALERT_THRESHOLD;
	struct event_msg high_msg = {
		.event = SYS_SENSOR_READING,
		.sensor = { .temp = threshold + 100 } /* 1°C above threshold */
	};

	/* First reading - no alert */
	zbus_chan_pub(&event_ch, &high_msg, K_NO_WAIT);
	k_yield();
	zassert_equal(alert_count, 0, "First reading should not trigger");

	/* Second reading - alert triggered */
	zbus_chan_pub(&event_ch, &high_msg, K_NO_WAIT);
	k_yield();
	zassert_equal(alert_count, 1, "Second consecutive reading should trigger alert");
	zassert_equal(last_alert_temp, threshold + 100,
		      "Alert should contain the temperature that triggered it");

	/* Third reading - continues alerting */
	zbus_chan_pub(&event_ch, &high_msg, K_NO_WAIT);
	k_yield();
	zassert_equal(alert_count, 2, "Third reading should trigger another alert");
}

/* Test 3: Low temperature reading resets the consecutive counter */
ZTEST(temp_alert_test_suite, test_temp_alert_low_resets)
{
	int32_t threshold = CONFIG_TEMP_ALERT_THRESHOLD;
	struct event_msg high_msg = {
		.event = SYS_SENSOR_READING,
		.sensor = { .temp = threshold + 100 } /* 1°C above threshold */
	};
	struct event_msg low_msg = {
		.event = SYS_SENSOR_READING,
		.sensor = { .temp = threshold - 200 } /* 2°C below threshold */
	};

	/* One high reading - no alert */
	zbus_chan_pub(&event_ch, &high_msg, K_NO_WAIT);
	k_yield();
	zassert_equal(alert_count, 0, "First high should not trigger");

	/* Low reading resets counter */
	zbus_chan_pub(&event_ch, &low_msg, K_NO_WAIT);
	k_yield();

	/* One high after low - counter was reset, so still no alert */
	zbus_chan_pub(&event_ch, &high_msg, K_NO_WAIT);
	k_yield();
	zassert_equal(alert_count, 0,
		      "Single high after low should not trigger (counter reset)");
}

ZTEST_SUITE(temp_alert_test_suite, NULL, temp_alert_test_setup, NULL, NULL, NULL);
