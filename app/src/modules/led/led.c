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

static enum sys_states read_sys_msg(void)
{
	int ret;
	enum sys_states msg;

	ret = zbus_chan_read(&led_ch, &msg, K_NO_WAIT);
	if (ret != 0) {
		LOG_ERR("zbus_chan_read, error: %d", ret);
		return false;
	}

	return msg;
}

/* LED Task Function */
static void led_fn(void)
{
	const struct zbus_channel *chan;
	enum sys_states sys_state;
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

	/* Boot to State Sleep */
	sys_state = SYS_SLEEP;

	LOG_INF("LED module started\n");
	while (1) {
		switch (sys_state) {
		case SYS_SLEEP:
			gpio_pin_set_dt(&led, 0);
			LOG_DBG("LED off");
			/* Nothing to do, blocking wait to save energy */
			ret = zbus_sub_wait(&led_subscriber, &chan, K_FOREVER);
			break;
		case SYS_STANDBY:
			gpio_pin_set_dt(&led, 1);
			LOG_DBG("LED on");
			k_sleep(K_MSEC(50));
			gpio_pin_set_dt(&led, 0);
			LOG_DBG("LED off");
			k_sleep(K_MSEC(500));
			/* Blinking led, blocking wait not possible */
			ret = zbus_sub_wait(&led_subscriber, &chan, K_NO_WAIT);
			break;
		/* Add your code here */

		/* */
		default:
			break;
		}
		if (ret == 0) {
			/* Wait a short moment for the system to wake up from
			 * sleep.
			 */
			k_sleep(K_MSEC(1));
			sys_state = read_sys_msg();
		}
	}
}

K_THREAD_DEFINE(led_task, CONFIG_LED_MODULE_THREAD_STACK_SIZE, led_fn, NULL,
		NULL, NULL, CONFIG_LED_MODULE_THREAD_STACK_PRIO, 0, 0);
