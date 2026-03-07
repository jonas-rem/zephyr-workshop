#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/zbus/zbus.h>

#include "message_channel.h"

LOG_MODULE_REGISTER(led_component, CONFIG_LED_COMPONENT_LOG_LEVEL);

#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)

#define LED_TICK_MS		     10
#define LED_TICKS_PER_SEC (1000 / LED_TICK_MS)

/* Timing in ticks (10ms each) */
#define POWER_ON_TICKS		     5   /* 50ms */
#define POWER_OFF_TICKS		     50  /* 500ms */
#define PULSE_ON_TICKS		     5   /* 50ms */
#define ALARM_ON_TICKS		     20  /* 200ms */
#define ALARM_OFF_TICKS		     20  /* 200ms */
#define ALARM_DURATION_TICKS	     300 /* 3 seconds */

typedef enum {
	LED_MODE_OFF,
	LED_MODE_POWER_SIGNAL,  /* 50ms on, 500ms off */
	LED_MODE_SENSOR_PULSE,  /* Single 50ms blink */
	LED_MODE_ALARM,         /* 200ms blink for 3s */
} led_mode_t;

typedef struct {
	const struct gpio_dt_spec *spec;
	led_mode_t mode;
	uint32_t tick_count;
	uint32_t duration_ticks;
	bool state;
} led_instance_t;

static const struct gpio_dt_spec led0_spec = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1_spec = GPIO_DT_SPEC_GET(LED1_NODE, gpios);

static led_instance_t leds[] = {
	{&led0_spec, LED_MODE_OFF, 0, 0, false},
	{&led1_spec, LED_MODE_OFF, 0, 0, false},
};

static struct k_timer led_timer;

static bool mode_needs_timer(led_mode_t mode)
{
	return mode != LED_MODE_OFF;
}

static bool any_led_needs_timer(void)
{
	for (int i = 0; i < ARRAY_SIZE(leds); i++) {
		if (mode_needs_timer(leds[i].mode)) {
			return true;
		}
	}
	return false;
}

static void update_timer_state(void)
{
	if (any_led_needs_timer()) {
		k_timer_start(&led_timer, K_MSEC(LED_TICK_MS), K_MSEC(LED_TICK_MS));
	} else {
		k_timer_stop(&led_timer);
	}
}

static void set_led_state(int led_idx, led_instance_t *led, bool state)
{
	if (led->state != state) {
		LOG_DBG("LED%d: %s", led_idx, state ? "ON" : "OFF");
	}
	led->state = state;
	gpio_pin_set_dt(led->spec, state);
}

static void update_led(int led_idx, led_instance_t *led)
{
	bool new_state = led->state;
	uint32_t period;

	switch (led->mode) {
	case LED_MODE_OFF:
		new_state = false;
		break;

	case LED_MODE_POWER_SIGNAL:
		period = POWER_ON_TICKS + POWER_OFF_TICKS;
		new_state = (led->tick_count % period) < POWER_ON_TICKS;
		led->tick_count++;
		break;

	case LED_MODE_SENSOR_PULSE:
		if (led->tick_count < PULSE_ON_TICKS) {
			new_state = true;
			led->tick_count++;
		} else {
			new_state = false;
			led->mode = LED_MODE_OFF;
			led->tick_count = 0;
			update_timer_state();
		}
		break;

	case LED_MODE_ALARM:
		period = ALARM_ON_TICKS + ALARM_OFF_TICKS;
		new_state = (led->tick_count % period) < ALARM_ON_TICKS;
		led->tick_count++;
		if (led->tick_count >= led->duration_ticks) {
			new_state = false;
			led->mode = LED_MODE_OFF;
			led->tick_count = 0;
			led->duration_ticks = 0;
			update_timer_state();
		}
		break;
	}

	if (new_state != led->state) {
		set_led_state(led_idx, led, new_state);
	}
}

static void led_timer_handler(struct k_timer *timer)
{
	for (int i = 0; i < ARRAY_SIZE(leds); i++) {
		update_led(i, &leds[i]);
	}
}

static void set_led_mode(uint8_t led_idx, led_mode_t mode)
{
	if (led_idx >= ARRAY_SIZE(leds)) {
		return;
	}

	led_instance_t *led = &leds[led_idx];

	if (led->mode == mode) {
		return;
	}

	led->mode = mode;
	led->tick_count = 0;

	/* Initialize duration for timed modes */
	if (mode == LED_MODE_ALARM) {
		led->duration_ticks = ALARM_DURATION_TICKS;
	}

	/* Apply immediate state */
	if (mode == LED_MODE_OFF) {
		set_led_state(led_idx, led, false);
	}

	update_timer_state();
}

static void led_sys_ctl_cb(const struct zbus_channel *chan)
{
	const enum sys_states *state = zbus_chan_const_msg(chan);

	if (*state == SYS_SLEEP) {
		set_led_mode(0, LED_MODE_OFF);
	} else {
		set_led_mode(0, LED_MODE_POWER_SIGNAL);
	}
}

static void led_event_cb(const struct zbus_channel *chan)
{
	const struct event_msg *msg = zbus_chan_const_msg(chan);

	switch (msg->event) {
	case SYS_SENSOR_READING:
		set_led_mode(1, LED_MODE_SENSOR_PULSE);
		break;
	case SYS_TEMP_ALERT:
		set_led_mode(1, LED_MODE_ALARM);
		break;
	default:
		break;
	}
}

ZBUS_LISTENER_DEFINE(led_sys_ctl_listener, led_sys_ctl_cb);
ZBUS_LISTENER_DEFINE(led_event_listener, led_event_cb);
ZBUS_CHAN_ADD_OBS(sys_ctl_ch, led_sys_ctl_listener, 1);
ZBUS_CHAN_ADD_OBS(event_ch, led_event_listener, 1);

#ifdef CONFIG_LED_COMPONENT_SHELL
#include <zephyr/shell/shell.h>

static const char *led_mode_to_str(led_mode_t mode)
{
	switch (mode) {
	case LED_MODE_OFF:
		return "OFF";
	case LED_MODE_POWER_SIGNAL:
		return "POWER_SIGNAL";
	case LED_MODE_SENSOR_PULSE:
		return "SENSOR_PULSE";
	case LED_MODE_ALARM:
		return "ALARM";
	default:
		return "UNKNOWN";
	}
}

static int cmd_led_status(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	shell_print(sh, "LED Status:");
	shell_print(sh, "  LED0: %s (state=%s)",
		    led_mode_to_str(leds[0].mode),
		    leds[0].state ? "ON" : "OFF");
	shell_print(sh, "  LED1: %s (state=%s)",
		    led_mode_to_str(leds[1].mode),
		    leds[1].state ? "ON" : "OFF");
	return 0;
}

static int cmd_led_set(const struct shell *sh, size_t argc, char **argv)
{
	if (argc != 3) {
		shell_error(sh, "Usage: led set <0|1> <off|power|pulse|alarm>");
		return -EINVAL;
	}

	uint8_t led_idx = argv[1][0] - '0';
	if (led_idx > 1) {
		shell_error(sh, "Invalid LED index: %s (use 0 or 1)", argv[1]);
		return -EINVAL;
	}

	led_mode_t mode;
	if (strcmp(argv[2], "off") == 0) {
		mode = LED_MODE_OFF;
	} else if (strcmp(argv[2], "power") == 0) {
		mode = LED_MODE_POWER_SIGNAL;
	} else if (strcmp(argv[2], "pulse") == 0) {
		mode = LED_MODE_SENSOR_PULSE;
	} else if (strcmp(argv[2], "alarm") == 0) {
		mode = LED_MODE_ALARM;
	} else {
		shell_error(sh, "Invalid mode: %s (use off, power, pulse, alarm)", argv[2]);
		return -EINVAL;
	}

	set_led_mode(led_idx, mode);
	shell_print(sh, "LED%d set to %s", led_idx, led_mode_to_str(mode));
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(led_cmds,
	SHELL_CMD(status, NULL, "Show LED status", cmd_led_status),
	SHELL_CMD(set, NULL, "Set LED mode: led set <0|1> <off|power|pulse|alarm>", cmd_led_set),
	SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(led, &led_cmds, "LED component commands", NULL);

#endif /* CONFIG_LED_COMPONENT_SHELL */

static int led_init(void)
{
	if (!gpio_is_ready_dt(&led0_spec) || !gpio_is_ready_dt(&led1_spec)) {
		LOG_ERR("LED GPIO not ready");
		return -ENODEV;
	}

	if (gpio_pin_configure_dt(&led0_spec, GPIO_OUTPUT) ||
	    gpio_pin_configure_dt(&led1_spec, GPIO_OUTPUT)) {
		LOG_ERR("LED GPIO config failed");
		return -EIO;
	}

	/* Initialize both LEDs to OFF */
	set_led_state(0, &leds[0], false);
	set_led_state(1, &leds[1], false);

	k_timer_init(&led_timer, led_timer_handler, NULL);

	/* Set initial state for LED0 based on default system state (SLEEP) */
	set_led_mode(0, LED_MODE_OFF);

	LOG_INF("LED component initialized");
	return 0;
}

SYS_INIT(led_init, APPLICATION, CONFIG_LED_COMPONENT_INIT_PRIORITY);
