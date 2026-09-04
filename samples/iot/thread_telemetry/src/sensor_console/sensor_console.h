/*
 * Copyright (c) 2026 TiaC Systems
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SENSOR_CONSOLE_H
#define SENSOR_CONSOLE_H

#include <stdint.h>

/** @brief Bounds of the log interval, the same as the Kconfig range. */
#define SENSOR_CONSOLE_INTERVAL_MIN_S 1
#define SENSOR_CONSOLE_INTERVAL_MAX_S 3600

/** @brief What the periodic read has been doing since boot. */
struct sensor_console_stats {
	/** Seconds between two logged readings. */
	uint32_t interval_s;
	/** Milliseconds until the next reading is logged. */
	uint32_t next_in_ms;
	/** Readings logged so far. */
	uint32_t readings;
	/** Reads that failed; the sensor module logs the reason for each one. */
	uint32_t failures;
};

/**
 * @brief Change how often a reading is logged. Not persisted.
 *
 * Takes effect immediately: the next reading is logged right away and the
 * new interval counts from there.
 *
 * @retval 0        On success.
 * @retval -EINVAL  Outside SENSOR_CONSOLE_INTERVAL_MIN_S..MAX_S.
 */
int sensor_console_interval_set(uint32_t seconds);

/** @brief Fill @p stats with a snapshot of the periodic read. */
void sensor_console_stats_get(struct sensor_console_stats *stats);

#endif /* SENSOR_CONSOLE_H */
