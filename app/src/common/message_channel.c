#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include "message_channel.h"


ZBUS_CHAN_DEFINE(button_ch,		/* Name */
		 enum sys_events,	/* Message type */
		 NULL,			/* Validator */
		 NULL,			/* User data */
		 ,			/* Observers */
		 ZBUS_MSG_INIT(0)	/* Initial value {0} */
);

ZBUS_CHAN_DEFINE(led_ch,		/* Name */
		 enum sys_states,	/* Message type */
		 NULL,			/* Validator */
		 NULL,			/* User data */
		 ,			/* Observers */
		 ZBUS_MSG_INIT(0)	/* Initial value {0} */
);
