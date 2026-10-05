/*
 * Copyright (c) 2026 Jonas Remmert
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/drivers/emul.h>
#include <zephyr/drivers/emul_sensor.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/ztest.h>

#include "message_channel.h"

#define SENSOR_NODE DT_NODELABEL(ti_hdc)

static const struct emul *const sensor_emulator = EMUL_DT_GET(SENSOR_NODE);
static struct sensor_event events[1];
static size_t event_count;
K_SEM_DEFINE(event_received, 0, 4);

static void event_listener(const struct zbus_channel *channel)
{
	if (event_count < ARRAY_SIZE(events)) {
		events[event_count++] =
			*(const struct sensor_event *)zbus_chan_const_msg(channel);
	}
	k_sem_give(&event_received);
}

ZBUS_LISTENER_DEFINE(test_event_listener, event_listener);
ZBUS_CHAN_ADD_OBS(sensor_event_ch, test_event_listener, 2);

static void set_channel(enum sensor_channel type, int64_t micro_units)
{
	struct sensor_chan_spec channel = { .chan_type = type, .chan_idx = 0 };
	q31_t lower;
	q31_t upper;
	q31_t epsilon;
	q31_t value;
	int8_t shift;

	zassert_ok(emul_sensor_backend_get_sample_range(sensor_emulator, channel, &lower,
							 &upper, &epsilon, &shift));
	value = ((micro_units * BIT64(31)) / 1000000) >> shift;
	zassert_ok(emul_sensor_backend_set_channel(sensor_emulator, channel, &value, shift));
}

static void before(void *fixture)
{
	ARG_UNUSED(fixture);
	event_count = 0;
	while (k_sem_take(&event_received, K_NO_WAIT) == 0) {
	}
}

ZTEST(sensor_test, test_button_event_publishes_emulated_measurement)
{
	struct button_event event = {
		.type = BUTTON_EVENT_0_PRESSED,
	};

	set_channel(SENSOR_CHAN_AMBIENT_TEMP, 25500000);
	set_channel(SENSOR_CHAN_HUMIDITY, 63250000);

	zassert_ok(zbus_chan_pub(&button_event_ch, &event, K_MSEC(100)));
	zassert_ok(k_sem_take(&event_received, K_MSEC(200)));

	zassert_equal(event_count, 1);
	zassert_equal(events[0].state, SENSOR_EVENT_SUCCESS);
	zassert_equal(events[0].temperature_milli_c, 25500);
	zassert_equal(events[0].humidity_milli_percent, 63250);
}

ZTEST_SUITE(sensor_test, NULL, NULL, before, NULL, NULL);
