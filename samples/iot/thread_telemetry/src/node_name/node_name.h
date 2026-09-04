/*
 * Copyright (c) 2026 TiaC Systems
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef NODE_NAME_H
#define NODE_NAME_H

#include <stddef.h>

/** @brief Buffer size that holds any node name, including the terminating NUL. */
#define NODE_NAME_SIZE (CONFIG_TELEMETRY_NAME_MAX_LEN + 1)

/**
 * @brief Copy the current node name into @p buf.
 *
 * The name can change at any time from the shell, so callers take a copy
 * under the module's lock instead of holding a pointer into its storage.
 *
 * @param buf  Destination, at least @ref NODE_NAME_SIZE bytes to never truncate.
 * @param size Size of @p buf.
 *
 * @return Number of characters written, excluding the terminating NUL.
 */
size_t node_name_get(char *buf, size_t size);

/**
 * @brief Set and persist the node name.
 *
 * The name is only adopted once it has reached storage, so a failed write
 * leaves the previous name in use.
 *
 * @retval 0        On success.
 * @retval -EINVAL  Empty, too long, or NULL.
 */
int node_name_set(const char *name);

#endif /* NODE_NAME_H */
