#ifndef _MESSAGE_CHANNEL_H_
#define _MESSAGE_CHANNEL_H_

#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>

#define DEFAULT_OBS_PRIO 1

/**
 * @brief System Message Types.
 */
enum sys_msg {
	/**
	 * @brief Button Pressed event
	 */
	SYS_BUTTON_PRESSED,

	/**
	 * @brief System is active
	 */
	SYS_ACTIVE,

	/**
	 * @brief System was active and is transitioning to sleep now.
	 */
	SYS_SLEEP,
};

ZBUS_CHAN_DECLARE(button_ch);
ZBUS_CHAN_DECLARE(led_ch);

#endif /* _MESSAGE_CHANNEL_H_ */
