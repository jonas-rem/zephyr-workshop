/*
 * Copyright (c) 2026 TiaC Systems
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ENV_SENSOR_H
#define ENV_SENSOR_H

#include <stddef.h>

#include <zephyr/drivers/sensor.h>

/** @brief Buffer size that holds any reading rendered by env_sensor_str(). */
#define ENV_SENSOR_STR_SIZE 16

/**
 * @brief Read the on-board temperature and humidity sensor.
 *
 * Concurrent callers are serialised, so this is safe to call from several
 * threads.
 *
 * @param temp Ambient temperature, in degrees Celsius.
 * @param hum  Relative humidity, in percent.
 *
 * @retval 0        On success.
 * @retval -EINVAL  @p temp or @p hum is NULL.
 * @retval -ENODEV  The sensor is not available.
 */
int env_sensor_read(struct sensor_value *temp, struct sensor_value *hum);

/**
 * @brief Render @p val into @p buf with two decimals.
 *
 * Done by hand rather than with "%f", which keeps CONFIG_CBPRINTF_FP_SUPPORT
 * out of the image. Both the console log and the JSON payload use this, so a
 * reading looks the same wherever it turns up.
 *
 * @param buf  Destination, at least @ref ENV_SENSOR_STR_SIZE bytes.
 * @param size Size of @p buf.
 * @param val  Reading to render.
 *
 * @return @p buf, so the call can be used directly as a "%s" argument.
 */
const char *env_sensor_str(char *buf, size_t size, const struct sensor_value *val);

#endif /* ENV_SENSOR_H */
