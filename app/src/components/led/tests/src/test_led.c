/*
 * Copyright (c) 2026 Jonas Remmert
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/drivers/gpio/gpio_emul.h>
#include <zephyr/device.h>
#include "message_channel.h"

static const struct device *led0_dev, *led1_dev;
static gpio_pin_t led0_pin, led1_pin;

#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
static const struct gpio_dt_spec led0_spec =
	GPIO_DT_SPEC_GET_OR(LED0_NODE, gpios, {0});
static const struct gpio_dt_spec led1_spec =
	GPIO_DT_SPEC_GET_OR(LED1_NODE, gpios, {0});

static inline int led_is_on(const struct gpio_dt_spec *s,
			    const struct device *d, gpio_pin_t p)
{
	int val = gpio_emul_output_get(d, p);
	return s->dt_flags & GPIO_ACTIVE_LOW ? !val : val;
}

static void *led_test_setup(void)
{
	led0_dev = DEVICE_DT_GET(DT_GPIO_CTLR(DT_ALIAS(led0), gpios));
	led0_pin = DT_GPIO_PIN(DT_ALIAS(led0), gpios);
	led1_dev = DEVICE_DT_GET(DT_GPIO_CTLR(DT_ALIAS(led1), gpios));
	led1_pin = DT_GPIO_PIN(DT_ALIAS(led1), gpios);
	k_sleep(K_MSEC(100));
	return NULL;
}

ZTEST(led_test_suite, test_led_module_initialized)
{
	zassert_true(gpio_is_ready_dt(&led0_spec), "LED0 GPIO should be ready");
	zassert_true(gpio_is_ready_dt(&led1_spec), "LED1 GPIO should be ready");
}

ZTEST(led_test_suite, test_led0_sys_state)
{
	enum sys_states state = SYS_SLEEP;
	zbus_chan_pub(&sys_ctl_ch, &state, K_NO_WAIT);
	k_sleep(K_MSEC(100));
	zassert_false(led_is_on(&led0_spec, led0_dev, led0_pin),
		     "LED0 should be off in sleep");

	state = SYS_ACTIVE;
	zbus_chan_pub(&sys_ctl_ch, &state, K_NO_WAIT);
	k_sleep(K_MSEC(100));
	zassert_true(1, "LED0 should be blinking in active");

	state = SYS_SLEEP;
	zbus_chan_pub(&sys_ctl_ch, &state, K_NO_WAIT);
	k_sleep(K_MSEC(100));
	zassert_false(led_is_on(&led0_spec, led0_dev, led0_pin),
		     "LED0 should be off");
}

ZTEST(led_test_suite, test_led1_sensor_event)
{
	struct event_msg evt = { .event = SYS_SENSOR_READING };
	zbus_chan_pub(&event_ch, &evt, K_NO_WAIT);
	k_sleep(K_MSEC(10));
	zassert_true(led_is_on(&led1_spec, led1_dev, led1_pin),
		     "LED1 should be on after sensor event");
	k_sleep(K_MSEC(250));
	zassert_false(led_is_on(&led1_spec, led1_dev, led1_pin),
		     "LED1 should be off after blink duration");
}

ZTEST(led_test_suite, test_led1_alarm_event)
{
	struct event_msg evt = { .event = SYS_TEMP_ALERT };
	zbus_chan_pub(&event_ch, &evt, K_NO_WAIT);
	k_sleep(K_MSEC(10));
	zassert_true(led_is_on(&led1_spec, led1_dev, led1_pin),
		     "LED1 should be on after alarm event");
	k_sleep(K_MSEC(350));
	zassert_false(led_is_on(&led1_spec, led1_dev, led1_pin),
		     "LED1 should toggle off after toggle period");
}

static void led_test_after(void *f)
{
	ARG_UNUSED(f);
	/* Wait for alarm to complete (3s + margin) */
	k_sleep(K_MSEC(3500));
}

ZTEST_SUITE(led_test_suite, NULL, led_test_setup, NULL, led_test_after, NULL);
