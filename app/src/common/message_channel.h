#ifndef _MESSAGE_CHANNEL_H_
#define _MESSAGE_CHANNEL_H_

#include <zephyr/zbus/zbus.h>

/**
 * @brief System State.
 */
enum sys_states {
	/**
	 * @brief System is idle (low-power, no measurements).
	 */
	SYS_SLEEP,

	/**
	 * @brief System is active (measuring, trip in progress).
	 */
	SYS_ACTIVE,
};

/**
 * @brief System Events.
 */
enum sys_events {
	/**
	 * @brief Button Pressed event
	 */
	SYS_BUTTON_PRESSED,

	/**
	 * @brief Sensor reading available
	 */
	SYS_SENSOR_READING,

	/**
	 * @brief Temperature threshold exceeded
	 */
	SYS_TEMP_ALERT,
};

/**
 * @brief Sensor data payload.
 */
struct sensor_data {
	int32_t temp; /* Temperature in 0.01 °C */
};

/**
 * @brief Generic event message carried on event_ch.
 */
struct event_msg {
	enum sys_events event;
	union {
		struct sensor_data sensor;
	};
};

ZBUS_CHAN_DECLARE(event_ch);
ZBUS_CHAN_DECLARE(sys_ctl_ch);

#endif /* _MESSAGE_CHANNEL_H_ */
