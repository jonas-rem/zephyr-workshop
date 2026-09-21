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

#define LED0_NODE DT_ALIAS(led0)
#define SETTLE_US 100000

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

static bool led_is_on(void)
{
	int level = gpio_emul_output_get(led.port, led.pin);

	return led.dt_flags & GPIO_ACTIVE_LOW ? !level : level;
}

static void press_button(void)
{
	struct button_event event = {
		.type = BUTTON_EVENT_0_PRESSED,
	};

	zassert_ok(zbus_chan_pub(&button_event_ch, &event, K_MSEC(100)));
}

static void report_result(enum sensor_event_state state)
{
	struct sensor_event event = {
		.state = state,
	};

	zassert_ok(zbus_chan_pub(&sensor_event_ch, &event, K_MSEC(100)));
}

static void before(void *fixture)
{
	ARG_UNUSED(fixture);
	report_result(SENSOR_EVENT_IDLE);
	zassert_true(WAIT_FOR(!led_is_on(), SETTLE_US, k_msleep(1)));
}

ZTEST(led_test, test_button_press_turns_led_on)
{
	press_button();
	zassert_true(WAIT_FOR(led_is_on(), SETTLE_US, k_msleep(1)));
}

ZTEST(led_test, test_success_turns_led_off)
{
	press_button();
	zassert_true(WAIT_FOR(led_is_on(), SETTLE_US, k_msleep(1)));

	report_result(SENSOR_EVENT_SUCCESS);
	zassert_true(WAIT_FOR(!led_is_on(), SETTLE_US, k_msleep(1)));
}

ZTEST(led_test, test_error_turns_led_off)
{
	press_button();
	zassert_true(WAIT_FOR(led_is_on(), SETTLE_US, k_msleep(1)));

	report_result(SENSOR_EVENT_ERROR);
	zassert_true(WAIT_FOR(!led_is_on(), SETTLE_US, k_msleep(1)));
}

ZTEST_SUITE(led_test, NULL, NULL, before, NULL, NULL);
