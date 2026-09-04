/*
 * Copyright (c) 2026 TiaC Systems
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <stddef.h>

#include <zephyr/net/net_ip.h>

/** @brief Buffer size that holds any collector address, including the NUL. */
#define TELEMETRY_COLLECTOR_SIZE NET_IPV6_ADDR_LEN

/**
 * @brief Read the sensor and publish one sample immediately.
 *
 * @retval 0 On success, a negative errno otherwise.
 */
int telemetry_publish_now(void);

/**
 * @brief Copy the destination the CoAP POSTs are sent to into @p buf.
 *
 * @param buf  Destination, at least @ref TELEMETRY_COLLECTOR_SIZE bytes.
 * @param size Size of @p buf.
 *
 * @return Number of characters written, excluding the terminating NUL.
 */
size_t telemetry_collector_get(char *buf, size_t size);

/**
 * @brief Change the CoAP destination at runtime. Not persisted.
 *
 * @retval 0        On success.
 * @retval -EINVAL  Not a valid IPv6 address, or too long.
 */
int telemetry_collector_set(const char *addr);

#endif /* TELEMETRY_H */
