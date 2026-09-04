/*
 * Copyright (c) 2026 TiaC Systems
 * SPDX-License-Identifier: Apache-2.0
 *
 * LD1 follows the Thread attachment state.
 */

#include <errno.h>

#include <zephyr/drivers/gpio.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <openthread.h>
#include <openthread/thread.h>

#include "mesh.h"

LOG_MODULE_REGISTER(status_led, CONFIG_TELEMETRY_STATUS_LED_LOG_LEVEL);

static const struct gpio_dt_spec status_led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

static void status_led_update(void)
{
	bool attached = mesh_is_attached();
	int ret;

	ret = gpio_pin_set_dt(&status_led, attached ? 1 : 0);
	if (ret) {
		LOG_ERR("Failed to switch the status LED %s: %d", attached ? "on" : "off", ret);
		return;
	}

	LOG_DBG("Status LED %s, role %s", attached ? "on" : "off", mesh_role_name());
}

static void on_state_changed(otChangedFlags flags, void *context)
{
	ARG_UNUSED(context);

	if ((flags & OT_CHANGED_THREAD_ROLE) == 0) {
		return;
	}

	status_led_update();
}

static struct openthread_state_changed_callback state_callback = {
	.otCallback = on_state_changed,
};

static int status_led_init(void)
{
	int ret;

	/* The rest of the node runs without LD1, but not having it is still an error. */
	if (!gpio_is_ready_dt(&status_led)) {
		LOG_ERR("Status LED on %s is not ready", status_led.port->name);
		return -ENODEV;
	}

	ret = gpio_pin_configure_dt(&status_led, GPIO_OUTPUT_INACTIVE);
	if (ret) {
		LOG_ERR("Failed to configure the status LED: %d", ret);
		return ret;
	}

	/*
	 * OpenThread keeps a list of these, so registering one here does not
	 * take the slot away from any other module.
	 */
	ret = openthread_state_changed_callback_register(&state_callback);
	if (ret) {
		LOG_ERR("Failed to register the OpenThread state callback: %d", ret);
		return ret;
	}

	LOG_INF("Status LED on %s pin %u follows the mesh attachment", status_led.port->name,
		status_led.pin);

	status_led_update();

	return 0;
}

SYS_INIT(status_led_init, APPLICATION, CONFIG_TELEMETRY_STATUS_LED_INIT_PRIORITY);
