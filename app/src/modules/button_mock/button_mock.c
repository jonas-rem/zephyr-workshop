/*
 * Copyright (c) 2026 Jonas Remmert
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/gpio/gpio_emul.h>
#include <zephyr/shell/shell.h>

LOG_MODULE_REGISTER(button_mock, LOG_LEVEL_DBG);

#define BUTTON_NODE DT_ALIAS(sw0)
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);

static struct k_work_delayable button_press_work;
static struct k_work_delayable button_release_work;

static void button_release_work_handler(struct k_work *work)
{
	gpio_emul_input_set_dt(&button, 0);
	LOG_INF("Button released");
}

static void button_press_work_handler(struct k_work *work)
{
	gpio_emul_input_set_dt(&button, 1);
	LOG_INF("Button pressed (mock)");
	
	/* Schedule button release after 100ms */
	k_work_schedule(&button_release_work, K_MSEC(100));
}

/* Shell command to trigger button press */
static int cmd_mock_button(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);
	
	LOG_INF("Triggering mock button press");
	k_work_schedule(&button_press_work, K_NO_WAIT);
	
	return 0;
}

SHELL_CMD_REGISTER(mock_button, NULL, "Simulate button press via GPIO", cmd_mock_button);

static int button_mock_init(void)
{
	LOG_INF("Button mock module initialized");
	
	k_work_init_delayable(&button_press_work, button_press_work_handler);
	k_work_init_delayable(&button_release_work, button_release_work_handler);
	
	return 0;
}

SYS_INIT(button_mock_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
