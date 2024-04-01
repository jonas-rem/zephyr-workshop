#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>

#include "message_channel.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app, CONFIG_BUTTON_MODULE_LOG_LEVEL);


void button_test_msg_cb(const struct zbus_channel *chan)
{
	const enum sys_msg *msg_type;

	/* Get message from channel. */
	msg_type = zbus_chan_const_msg(chan);

	switch (*msg_type) {
	case SYS_BUTTON_PRESSED:
		LOG_INF("Button pressed!");
		break;
	default:
		/* Ignore nonrelevant messages */
		break;
	}
}

int main(void)
{
	LOG_INF("Button test start");

	return 0;
}

ZBUS_LISTENER_DEFINE(button_test, button_test_msg_cb);
ZBUS_CHAN_ADD_OBS(button_ch, button_test, DEFAULT_OBS_PRIO);
