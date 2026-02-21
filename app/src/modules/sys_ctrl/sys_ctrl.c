#include <zephyr/zbus/zbus.h>
#include <zephyr/shell/shell.h>
#include <zephyr/logging/log.h>

#include "message_channel.h"
#include "modules/sys_ctrl/sys_ctrl.h"

LOG_MODULE_REGISTER(sys_ctrl, LOG_LEVEL_DBG);

/* Boot to State Standby */
static enum sys_states sys_state = SYS_STANDBY;

static void sys_ctrl_led_msg(enum sys_states msg)
{
	int err;

	err = zbus_chan_pub(&led_ch, &msg, K_SECONDS(1));
	if (err) {
		LOG_ERR("zbus_chan_pub, error: %d", err);
	}
}

void sys_ctrl_handle_button_press(void)
{
	/* Assign new system state */
	switch (sys_state) {
	case SYS_SLEEP:
		sys_state = SYS_STANDBY;
		LOG_INF("System state standby");
		break;
	case SYS_STANDBY:
		sys_state = SYS_SLEEP;
		LOG_INF("System state sleep");
		break;
	default:
		return;
	}

	sys_ctrl_led_msg(sys_state);
}

enum sys_states sys_ctrl_get_state(void)
{
	return sys_state;
}

static int sys_ctrl_init(void)
{
	/* Notify the LED module about the initial system state */
	sys_ctrl_led_msg(sys_state);
	return 0;
}

SYS_INIT(sys_ctrl_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

static void sys_ctrl_button_msg_cb(const struct zbus_channel *chan)
{
	const enum sys_events *msg_type;

	/* Get message from channel. */
	msg_type = zbus_chan_const_msg(chan);

	if (*msg_type != SYS_BUTTON_PRESSED) {
		/* Ignore other messages */
		return;
	}

	sys_ctrl_handle_button_press();
}

ZBUS_LISTENER_DEFINE(sys_ctrl_listener, sys_ctrl_button_msg_cb);
ZBUS_CHAN_ADD_OBS(button_ch, sys_ctrl_listener, DEFAULT_OBS_PRIO);

static int cmd_button(const struct shell *sh, size_t argc, char **argv,
		    void *data)
{
	int err;
	enum sys_events msg;

	msg = SYS_BUTTON_PRESSED;

	err = zbus_chan_pub(&button_ch, &msg, K_SECONDS(1));
	if (err) {
		LOG_ERR("zbus_chan_pub, error: %d", err);
	}

	return 0;
}

SHELL_SUBCMD_DICT_SET_CREATE(sub_button_cmds, cmd_button,
	(button_press, 1, "Trigger Button Press")
);

SHELL_STATIC_SUBCMD_SET_CREATE(my_app_cmds,
	SHELL_CMD(button, &sub_button_cmds, "Button test commands", NULL),
	SHELL_SUBCMD_SET_END /* Array terminated. */
);
SHELL_CMD_REGISTER(my_app, &my_app_cmds, "my App test", NULL);
