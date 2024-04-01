#include <zephyr/logging/log.h>
#include <zephyr/init.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/zbus/zbus.h>
#include <inttypes.h>

#include "message_channel.h"

LOG_MODULE_REGISTER(led_module, CONFIG_LED_MODULE_LOG_LEVEL);

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

ZBUS_SUBSCRIBER_DEFINE(led_subscriber, 4);
ZBUS_CHAN_ADD_OBS(led_ch, led_subscriber, DEFAULT_OBS_PRIO);


static bool read_sys_msg(void)
{
	int ret;
	enum sys_msg msg;

	ret = zbus_chan_read(&led_ch, &msg, K_NO_WAIT);
	if (ret == 0) {
		switch (msg) {
		case SYS_ACTIVE:
			LOG_INF("Signal system active\n");
			return true;
			break;
		case SYS_SLEEP:
			LOG_INF("Signal system sleep\n");
			return false;
			break;
		default:
			LOG_ERR("Invalid message");
			return false;
			break;
		}
	} else {
		LOG_ERR("zbus_chan_read, error: %d", ret);
		return false;
	}
}

/* LED Task Function */
static void led_fn(void)
{
	const struct zbus_channel *chan;
	bool sys_active = true;
	int ret;

	if (!gpio_is_ready_dt(&led)) {
		LOG_ERR("LED device not ready");
		return;
	}
	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		LOG_ERR("LED configure error");
		return;
	}

	LOG_INF("LED module started\n");
	while (1) {
		if (sys_active) {
			gpio_pin_set(led.port, led.pin, 1);
			k_sleep(K_MSEC(300));
			gpio_pin_set(led.port, led.pin, 0);
			k_sleep(K_MSEC(300));
			ret = zbus_sub_wait(&led_subscriber, &chan, K_NO_WAIT);
		} else {
			ret = zbus_sub_wait(&led_subscriber, &chan, K_FOREVER);
			/* Wait a short moment for the system to wake up from
			 * sleep */
			k_sleep(K_MSEC(1));
		}
		if (ret == 0) {
			sys_active = read_sys_msg();
		}
	}
}

K_THREAD_DEFINE(led_task, CONFIG_LED_MODULE_THREAD_STACK_SIZE, led_fn, NULL,
		NULL, NULL, CONFIG_LED_MODULE_THREAD_STACK_PRIO, 0, 0);
