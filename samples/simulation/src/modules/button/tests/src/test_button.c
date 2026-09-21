/*
 * Copyright (c) 2026 Jonas Remmert
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/gpio/gpio_emul.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/ztest.h>

#include "message_channel.h"

#define SW0_NODE DT_ALIAS(sw0)
#define SETTLE_MS 20

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW0_NODE, gpios);
static uint32_t event_count;
K_SEM_DEFINE(event_received, 0, 8);

static void event_listener(const struct zbus_channel *channel)
{
	ARG_UNUSED(channel);
	event_count++;
	k_sem_give(&event_received);
}

ZBUS_LISTENER_DEFINE(test_event_listener, event_listener);
ZBUS_CHAN_ADD_OBS(button_event_ch, test_event_listener, 2);

static void set_button(bool pressed)
{
	int level = pressed;

	if (button.dt_flags & GPIO_ACTIVE_LOW) {
		level = !level;
	}
	zassert_ok(gpio_emul_input_set(button.port, button.pin, level));
}

static void press_button(void)
{
	set_button(true);
	zassert_ok(k_sem_take(&event_received, K_MSEC(100)));
	set_button(false);
	k_msleep(SETTLE_MS);
}

static void *suite_setup(void)
{
	zassert_true(gpio_is_ready_dt(&button));
	set_button(false);
	k_msleep(SETTLE_MS);
	return NULL;
}

static void before(void *fixture)
{
	ARG_UNUSED(fixture);
	set_button(false);
	while (k_sem_take(&event_received, K_NO_WAIT) == 0) {
	}
	k_msleep(SETTLE_MS);
}

ZTEST(button_test, test_press_publishes_event)
{
	press_button();
}

ZTEST(button_test, test_bounce_publishes_one_event)
{
	uint32_t initial_count = event_count;

	set_button(true);
	set_button(false);
	set_button(true);
	zassert_ok(k_sem_take(&event_received, K_MSEC(100)));
	k_msleep(SETTLE_MS);
	set_button(false);
	zassert_equal(event_count, initial_count + 1);
}

ZTEST(button_test, test_repeated_presses_publish_repeated_events)
{
	uint32_t initial_count = event_count;

	press_button();
	press_button();
	zassert_equal(event_count, initial_count + 2);
}

ZTEST_SUITE(button_test, NULL, suite_setup, before, NULL, NULL);
