#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>

#include "message_channel.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app, CONFIG_LED_MODULE_LOG_LEVEL);

static bool sys_active = true;

static void led_msg(enum sys_msg msg)
{
	int err;

	err = zbus_chan_pub(&led_ch, &msg, K_SECONDS(1));
	if (err) {
		LOG_ERR("zbus_chan_pub, error: %d", err);
	}
}

static void button_msg_cb(const struct zbus_channel *chan)
{
	const enum sys_msg *msg_type;

	/* Get message from channel. */
	msg_type = zbus_chan_const_msg(chan);

	switch (*msg_type) {
	case SYS_BUTTON_PRESSED:
		sys_active = !sys_active;
		if (sys_active) {
			LOG_INF("System active");
			led_msg(SYS_ACTIVE);
		} else {
			LOG_INF("System sleep");
			led_msg(SYS_SLEEP);
		}
		break;
	default:
		/* Ignore nonrelevant messages */
		break;
	}
}

int main(void)
{
	LOG_INF("System %s", sys_active ? "active" : "sleep");

	return 0;
}

ZBUS_LISTENER_DEFINE(button_test, button_msg_cb);
ZBUS_CHAN_ADD_OBS(button_ch, button_test, DEFAULT_OBS_PRIO);
