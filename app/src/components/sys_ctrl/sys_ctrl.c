#include <zephyr/zbus/zbus.h>
#include <zephyr/shell/shell.h>
#include <zephyr/logging/log.h>

#include "message_channel.h"

LOG_MODULE_REGISTER(sys_ctrl, LOG_LEVEL_DBG);

/* Boot to State Active */
static enum sys_states sys_state = SYS_ACTIVE;

static void sys_ctrl_led_msg(enum sys_states msg)
{
	int err;

	err = zbus_chan_pub(&sys_ctl_ch, &msg, K_SECONDS(1));
	if (err) {
		LOG_ERR("zbus_chan_pub, error: %d", err);
	}
}

void sys_ctrl_handle_button_press(void)
{
	/* Assign new system state */
	switch (sys_state) {
	case SYS_SLEEP:
		sys_state = SYS_ACTIVE;
		LOG_INF("System state active");
		break;
	case SYS_ACTIVE:
		sys_state = SYS_SLEEP;
		LOG_INF("System state sleep");
		break;
	default:
		return;
	}

	sys_ctrl_led_msg(sys_state);
}

static int sys_ctrl_init(void)
{
	/* Notify the LED module about the initial system state */
	sys_ctrl_led_msg(sys_state);

	LOG_INF("sys_ctl module started");

	return 0;
}

static void sys_ctrl_button_msg_cb(const struct zbus_channel *chan)
{
	const struct event_msg *msg;

	/* Get message from channel. */
	msg = zbus_chan_const_msg(chan);

	if (msg->event != SYS_BUTTON_PRESSED) {
		/* Ignore other messages */
		return;
	}

	sys_ctrl_handle_button_press();
}

ZBUS_LISTENER_DEFINE(sys_ctrl_listener, sys_ctrl_button_msg_cb);
ZBUS_CHAN_ADD_OBS(event_ch, sys_ctrl_listener, 1);

#ifdef CONFIG_SYS_CTRL_COMPONENT_SHELL
#include <zephyr/shell/shell.h>

static int cmd_sysctrl_state(const struct shell *sh, size_t argc, char **argv)
{
	const char *state_str;

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	switch (sys_state) {
	case SYS_SLEEP:
		state_str = "SLEEP";
		break;
	case SYS_ACTIVE:
		state_str = "ACTIVE";
		break;
	default:
		state_str = "UNKNOWN";
		break;
	}

	shell_print(sh, "System state: %s (%d)", state_str, sys_state);
	return 0;
}

static int cmd_sysctrl_button(const struct shell *sh, size_t argc, char **argv)
{
	int err;
	struct event_msg msg;

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	msg.event = SYS_BUTTON_PRESSED;
	shell_print(sh, "Simulating button press");

	err = zbus_chan_pub(&event_ch, &msg, K_SECONDS(1));
	if (err) {
		shell_error(sh, "Failed to publish button event: %d", err);
		return err;
	}

	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sysctrl_cmds,
	SHELL_CMD(state, NULL, "Show current system state", cmd_sysctrl_state),
	SHELL_CMD(button, NULL, "Simulate button press", cmd_sysctrl_button),
	SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(sys_ctrl, &sysctrl_cmds, "System control component commands", NULL);

#endif /* CONFIG_SYS_CTRL_COMPONENT_SHELL */

SYS_INIT(sys_ctrl_init, APPLICATION, CONFIG_SYS_CTRL_COMPONENT_INIT_PRIORITY);
