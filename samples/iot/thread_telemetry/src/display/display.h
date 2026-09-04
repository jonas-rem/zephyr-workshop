/*
 * Copyright (c) 2026 TiaC Systems
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef DISPLAY_H
#define DISPLAY_H

enum display_mode {
	DISPLAY_MODE_TEMPERATURE,
	DISPLAY_MODE_HUMIDITY,
};

/** @brief Currently shown quantity. */
enum display_mode display_mode_get(void);

/** @brief Show @p mode from now on, and refresh the strip at once. */
void display_mode_set(enum display_mode mode);

/** @brief Human readable name of @p mode. */
const char *display_mode_name(enum display_mode mode);

#endif /* DISPLAY_H */
