/*
 * Copyright (c) 2026 TiaC Systems
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ENV_SENSOR_H
#define ENV_SENSOR_H

#include <zephyr/drivers/sensor.h>

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

#endif /* ENV_SENSOR_H */
