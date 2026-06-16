/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/input/input.h>

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw0), gpios, {0});

bool led_state = true;

static void button_input_cb(struct input_event *evt, void *user_data)
{
	int ret;

	if (evt->sync == 0) {
		return;
	}

	printk("Button %d %s at %" PRIu32 "\n",
			evt->code,
			evt->value ? "pressed" : "released",
			k_cycle_get_32());

	if (evt->value == 1 && evt->code == button.pin) {
		ret = gpio_pin_toggle_dt(&led);
		if (ret < 0) {
			return;
		}

		led_state = !led_state;
		printf("LED state: %s\n", led_state ? "ON" : "OFF");
	}
}

INPUT_CALLBACK_DEFINE(NULL, button_input_cb, NULL);

int main(void)
{
	int ret;

	if (!gpio_is_ready_dt(&led)) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

	printf("Press a button\n");

	k_sleep(K_FOREVER);

	return 0;
}
