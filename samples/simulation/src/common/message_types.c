/*
 * Copyright (c) 2026 Jonas Remmert
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/sys/util.h>

#include "message_types.h"

static const char *const sensor_event_state_names[] = {
	[SENSOR_EVENT_IDLE] = "IDLE",
	[SENSOR_EVENT_SUCCESS] = "SUCCESS",
	[SENSOR_EVENT_ERROR] = "ERROR",
};

const char *sensor_event_state_str(enum sensor_event_state state)
{
	if (state >= ARRAY_SIZE(sensor_event_state_names)) {
		return "UNKNOWN";
	}
	return sensor_event_state_names[state];
}
