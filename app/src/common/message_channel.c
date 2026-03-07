#include <zephyr/zbus/zbus.h>
#include "message_channel.h"


ZBUS_CHAN_DEFINE(event_ch,		/* Name */
		 struct event_msg,	/* Message type */
		 NULL,			/* Validator */
		 NULL,			/* User data */
		 ,			/* Observers */
		 ZBUS_MSG_INIT(0)	/* Initial value {0} */
);

ZBUS_CHAN_DEFINE(sys_ctl_ch,		/* Name */
		 enum sys_states,	/* Message type */
		 NULL,			/* Validator */
		 NULL,			/* User data */
		 ,			/* Observers */
		 ZBUS_MSG_INIT(0)	/* Initial value {0} */
);
