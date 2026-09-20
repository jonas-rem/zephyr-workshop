#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>

#include "message_channel.h"

LOG_MODULE_REGISTER(temp_alert, CONFIG_TEMP_ALERT_LOG_LEVEL);

/* Consecutive readings above threshold required to trigger alert */
#define TEMP_ALERT_CONSECUTIVE_COUNT 2

/* Counter for consecutive readings above threshold */
static uint8_t consecutive_high_count;

/* Whether we're currently in alert state (above threshold) */
static bool in_alert_state;

/* Work item for deferred publishing */
static struct k_work alert_work;

/* Pending alert message to be published */
static struct event_msg pending_alert;

/* Work handler to publish alert (runs in system workqueue context) */
static void alert_work_handler(struct k_work *work)
{
	int err;

	ARG_UNUSED(work);

	err = zbus_chan_pub(&event_ch, &pending_alert, K_NO_WAIT);
	if (err) {
		LOG_ERR("Failed to publish alert: %d", err);
	}
}

static void temp_alert_event_cb(const struct zbus_channel *chan)
{
	const struct event_msg *msg = zbus_chan_const_msg(chan);

	if (msg->event != SYS_SENSOR_READING) {
		return;
	}

	if (msg->sensor.temp <= CONFIG_TEMP_ALERT_THRESHOLD) {
		consecutive_high_count = 0;
		in_alert_state = false;
		return;
	}

	if (!in_alert_state) {
		consecutive_high_count++;
		if (consecutive_high_count < TEMP_ALERT_CONSECUTIVE_COUNT) {
			return;
		}
		in_alert_state = true;
	}

	pending_alert.event = SYS_TEMP_ALERT;
	pending_alert.sensor.temp = msg->sensor.temp;
	k_work_submit(&alert_work);
}

ZBUS_LISTENER_DEFINE(temp_alert_listener, temp_alert_event_cb);
ZBUS_CHAN_ADD_OBS(event_ch, temp_alert_listener, 1);

static int temp_alert_init(void)
{
	consecutive_high_count = 0;
	in_alert_state = false;

	k_work_init(&alert_work, alert_work_handler);

	LOG_INF("Temperature alert module started (threshold: %d.%02d°C)",
		CONFIG_TEMP_ALERT_THRESHOLD / 100,
		CONFIG_TEMP_ALERT_THRESHOLD % 100);

	return 0;
}

SYS_INIT(temp_alert_init, APPLICATION, CONFIG_TEMP_ALERT_INIT_PRIORITY);
