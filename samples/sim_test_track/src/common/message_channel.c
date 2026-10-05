/*
 * Copyright (c) 2026 Jonas Remmert
 * SPDX-License-Identifier: Apache-2.0
 */

#include "message_channel.h"

ZBUS_CHAN_DEFINE(button_event_ch,
		 struct button_event,
		 NULL,
		 NULL,
		 ZBUS_OBSERVERS_EMPTY,
		 ZBUS_MSG_INIT(.type = BUTTON_EVENT_0_PRESSED));

ZBUS_CHAN_DEFINE(sensor_event_ch,
		 struct sensor_event,
		 NULL,
		 NULL,
		 ZBUS_OBSERVERS_EMPTY,
		 ZBUS_MSG_INIT(.state = SENSOR_EVENT_IDLE));
