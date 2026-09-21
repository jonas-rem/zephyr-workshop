/*
 * Copyright (c) 2026 Jonas Remmert
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_MESSAGE_CHANNEL_H_
#define APP_MESSAGE_CHANNEL_H_

#include <zephyr/zbus/zbus.h>

#include "message_types.h"

ZBUS_CHAN_DECLARE(button_event_ch);
ZBUS_CHAN_DECLARE(sensor_event_ch);

#endif /* APP_MESSAGE_CHANNEL_H_ */
