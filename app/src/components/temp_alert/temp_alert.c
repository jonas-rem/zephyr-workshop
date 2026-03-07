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

	err = zbus_chan_pub(&event_ch, &pending_alert, K_NO_WAIT);
	if (err) {
		LOG_ERR("Failed to publish alert: %d", err);
	}
}

static void temp_alert_event_cb(const struct zbus_channel *chan)
{
	/* TODO Workshop: Implement temperature threshold logic
	 *
	 * 1. Get the message from the channel using zbus_chan_const_msg()
	 * 2. Check if it's a SYS_SENSOR_READING event, ignore others
	 * 3. Get temperature from msg->sensor.temp
	 * 4. Compare against CONFIG_TEMP_ALERT_THRESHOLD
	 * 5. Track consecutive readings above threshold (use consecutive_high_count)
	 * 6. After 2 consecutive readings above threshold:
	 *    - Set pending_alert.event = SYS_TEMP_ALERT
	 *    - Set pending_alert.sensor.temp = temperature
	 *    - Submit alert_work via k_work_submit()
	 * 7. Reset counter when temperature drops below threshold
	 * 8. Update in_alert_state flag appropriately
	 */
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
