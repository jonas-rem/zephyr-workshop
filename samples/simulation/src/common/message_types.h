/*
 * Copyright (c) 2026 Jonas Remmert
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_MESSAGE_TYPES_H_
#define APP_MESSAGE_TYPES_H_

#include <stdint.h>

enum button_event_type {
	BUTTON_EVENT_0_PRESSED,
};

struct button_event {
	enum button_event_type type;
};

enum sensor_event_state {
	SENSOR_EVENT_IDLE,
	SENSOR_EVENT_SUCCESS,
	SENSOR_EVENT_ERROR,
};

const char *sensor_event_state_str(enum sensor_event_state state);

struct sensor_event {
	enum sensor_event_state state;
	int32_t temperature_milli_c;
	int32_t humidity_milli_percent;
	int error;
};

#endif /* APP_MESSAGE_TYPES_H_ */
