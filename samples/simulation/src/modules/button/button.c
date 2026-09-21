/*
 * Copyright (c) 2026 Jonas Remmert
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#ifdef CONFIG_GPIO_EMUL
#include <zephyr/drivers/gpio/gpio_emul.h>
#endif
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "message_channel.h"

LOG_MODULE_REGISTER(app_simulation_button, CONFIG_APP_TEST_BUTTON_LOG_LEVEL);

#define SW0_NODE DT_ALIAS(sw0)
#define DEBOUNCE_MS 10

#if !DT_NODE_HAS_STATUS(SW0_NODE, okay)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW0_NODE, gpios);
static struct gpio_callback button_callback;
static uint32_t event_count;

static void publish_event(struct k_work *work)
{
	struct button_event event = {
		.type = BUTTON_EVENT_0_PRESSED,
	};
	int state;
	int err;

	ARG_UNUSED(work);

	state = gpio_pin_get_dt(&button);
	if (state <= 0) {
		if (state < 0) {
			LOG_ERR("Failed to read button: %d", state);
		} else {
			LOG_DBG("Button released before debounce expired, ignoring");
		}
		return;
	}

	err = zbus_chan_pub(&button_event_ch, &event, K_MSEC(100));
	if (err != 0) {
		LOG_ERR("Failed to publish button event: %d", err);
		return;
	}

	event_count++;

	LOG_DBG("Button pressed, published event %u", event_count);
}

K_WORK_DELAYABLE_DEFINE(publish_event_work, publish_event);

static void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	ARG_UNUSED(dev);
	ARG_UNUSED(cb);
	ARG_UNUSED(pins);

	k_work_reschedule(&publish_event_work, K_MSEC(DEBOUNCE_MS));
}

static int button_init(void)
{
	int err;

	if (!gpio_is_ready_dt(&button)) {
		return -ENODEV;
	}

	err = gpio_pin_configure_dt(&button, GPIO_INPUT);
	if (err != 0) {
		return err;
	}

#ifdef CONFIG_GPIO_EMUL
	err = gpio_emul_input_set(button.port, button.pin,
				  button.dt_flags & GPIO_ACTIVE_LOW ? 1 : 0);
	if (err != 0) {
		return err;
	}
#endif

	gpio_init_callback(&button_callback, button_pressed, BIT(button.pin));
	err = gpio_add_callback(button.port, &button_callback);
	if (err != 0) {
		return err;
	}

	err = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
	if (err != 0) {
		return err;
	}

	LOG_INF("Button initialized");
	return 0;
}

#ifdef CONFIG_APP_TEST_BUTTON_SHELL
#include <zephyr/shell/shell.h>

static int set_emulated_button(bool pressed)
{
	int level = pressed;

	if (button.dt_flags & GPIO_ACTIVE_LOW) {
		level = !level;
	}
	return gpio_emul_input_set(button.port, button.pin, level);
}

static void release_emulated_button(struct k_work *work)
{
	ARG_UNUSED(work);
	set_emulated_button(false);
}

K_WORK_DELAYABLE_DEFINE(release_button_work, release_emulated_button);

static int cmd_button_emulate(const struct shell *sh, size_t argc, char **argv)
{
	int err;

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	err = set_emulated_button(true);
	if (err != 0) {
		shell_error(sh, "Failed to press emulated button: %d", err);
		return err;
	}

	k_work_reschedule(&release_button_work, K_MSEC(DEBOUNCE_MS * 3));

	shell_print(sh, "Emulated button press started");
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(button_commands,
	SHELL_CMD(emulate, NULL, "Press the emulated GPIO button", cmd_button_emulate),
	SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(button, &button_commands, "Button commands", NULL);
#endif

SYS_INIT(button_init, APPLICATION, CONFIG_APP_TEST_BUTTON_INIT_PRIORITY);
