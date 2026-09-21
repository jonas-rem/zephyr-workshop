/*
 * Copyright (c) 2026 Jonas Remmert
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "message_channel.h"

LOG_MODULE_REGISTER(app_simulation_led, CONFIG_APP_TEST_LED_LOG_LEVEL);

#define LED0_NODE DT_ALIAS(led0)

#if !DT_NODE_HAS_STATUS(LED0_NODE, okay)
#error "Unsupported board: led0 devicetree alias is not defined"
#endif

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/*
 * The LED shows whether a sensor read is in progress.
 */
ZBUS_MSG_SUBSCRIBER_DEFINE(led_subscriber);
ZBUS_CHAN_ADD_OBS(button_event_ch, led_subscriber, 1);
ZBUS_CHAN_ADD_OBS(sensor_event_ch, led_subscriber, 1);

union led_message {
	struct button_event button;
	struct sensor_event sensor;
};

static void set_output(bool on)
{
	int err = gpio_pin_set_dt(&led, on);

	if (err != 0) {
		LOG_ERR("Failed to set LED: %d", err);
		return;
	}

	/* native_sim has no visible LED, so the log is the only feedback. */
	LOG_DBG("LED %s", on ? "ON" : "OFF");
}

static int init(void)
{
	int err;

	if (!gpio_is_ready_dt(&led)) {
		LOG_ERR("LED device not ready");
		return -ENODEV;
	}

	err = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
	if (err != 0) {
		LOG_ERR("Failed to configure LED: %d", err);
		return err;
	}

	LOG_INF("LED initialized");
	return 0;
}

static void led_thread(void *p1, void *p2, void *p3)
{
	const struct zbus_channel *channel;
	union led_message message;

	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	if (init() != 0) {
		return;
	}

	while (true) {
		int err = zbus_sub_wait_msg(&led_subscriber, &channel, &message, K_FOREVER);

		if (err != 0) {
			LOG_ERR("Failed to receive message: %d", err);
			continue;
		}

		if (channel == &button_event_ch) {
			if (message.button.type == BUTTON_EVENT_0_PRESSED) {
				set_output(true);
			}
		} else if (channel == &sensor_event_ch) {
			switch (message.sensor.state) {
			case SENSOR_EVENT_IDLE:
			case SENSOR_EVENT_SUCCESS:
			case SENSOR_EVENT_ERROR:
				set_output(false);
				break;
			default:
				break;
			}
		}
	}
}

K_THREAD_DEFINE(led_thread_id, 1024, led_thread, NULL, NULL, NULL,
		CONFIG_APP_TEST_LED_THREAD_PRIORITY, 0, 0);
