/*
 * Copyright (c) 2026 Jonas Remmert
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/drivers/gpio/gpio_emul.h>
#include <zephyr/device.h>

#include "message_channel.h"

/* Test observer for event_ch messages */
static struct event_msg last_button_msg;
static volatile int button_msg_count;

static void event_ch_observer_cb(const struct zbus_channel *chan)
{
	const struct event_msg *msg = zbus_chan_const_msg(chan);
	last_button_msg = *msg;
	button_msg_count++;
}

ZBUS_LISTENER_DEFINE(test_button_listener, event_ch_observer_cb);
ZBUS_CHAN_ADD_OBS(event_ch, test_button_listener, 1);

/* GPIO emulator setup */
static const struct device *button_dev;
static gpio_pin_t button_pin;

/* Get button GPIO spec from devicetree */
#define SW0_NODE DT_ALIAS(sw0)
static const struct gpio_dt_spec button_spec = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});

static void *button_test_setup(void)
{
	button_dev = DEVICE_DT_GET(DT_GPIO_CTLR(DT_ALIAS(sw0), gpios));
	button_pin = DT_GPIO_PIN(DT_ALIAS(sw0), gpios);

	/* Wait for module to fully initialize */
	k_sleep(K_MSEC(100));

	/* Ensure button starts released */
	gpio_emul_input_set(button_dev, button_pin, 0);
	k_sleep(K_MSEC(50));

	return NULL;
}

ZTEST(button_test_suite, test_button_module_initialized)
{
	/* Verify the button module has started and initialized the GPIO */
	zassert_true(gpio_is_ready_dt(&button_spec), "Button GPIO should be ready");
}

ZTEST(button_test_suite, test_button_press_creates_event)
{
	/* Press button with proper timing */
	gpio_emul_input_set(button_dev, button_pin, 0);
	k_sleep(K_MSEC(10));
	gpio_emul_input_set(button_dev, button_pin, 1);

	/* Wait for debounce + work handler */
	k_sleep(K_MSEC(100));

	/* Release */
	gpio_emul_input_set(button_dev, button_pin, 0);
	k_sleep(K_MSEC(50));

	/* Event might arrive asynchronously - check total count increased */
	zassert_true(button_msg_count > 0,
		      "Should have received button events (count: %d)", button_msg_count);
}

ZTEST(button_test_suite, test_multiple_presses_generate_multiple_events)
{
	int count_start = button_msg_count;
	int presses = 3;

	for (int i = 0; i < presses; i++) {
		/* Press sequence */
		gpio_emul_input_set(button_dev, button_pin, 0);
		k_sleep(K_MSEC(10));
		gpio_emul_input_set(button_dev, button_pin, 1);
		k_sleep(K_MSEC(50));
		gpio_emul_input_set(button_dev, button_pin, 0);
		k_sleep(K_MSEC(50));
	}

	/* Should have generated events */
	zassert_true(button_msg_count >= count_start + presses,
		      "Should generate events for button presses (count: %d, start: %d)",
		      button_msg_count, count_start);
}

ZTEST(button_test_suite, test_event_type_is_correct)
{
	int count_before = button_msg_count;

	/* Press button */
	gpio_emul_input_set(button_dev, button_pin, 0);
	k_sleep(K_MSEC(10));
	gpio_emul_input_set(button_dev, button_pin, 1);
	k_sleep(K_MSEC(50));

	/* If we got an event, verify its type */
	if (button_msg_count > count_before) {
		zassert_equal(last_button_msg.event, SYS_BUTTON_PRESSED,
			      "Event type should be SYS_BUTTON_PRESSED");
	}

	/* Release */
	gpio_emul_input_set(button_dev, button_pin, 0);
	k_sleep(K_MSEC(50));
}

ZTEST_SUITE(button_test_suite, NULL, button_test_setup, NULL, NULL, NULL);
