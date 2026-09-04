/*
 * Copyright (c) 2026 TiaC Systems
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef MESH_H
#define MESH_H

#include <stdbool.h>
#include <stdint.h>

/** @brief Buffer size that holds any IPv6 address string, including the NUL. */
#define MESH_ADDR_STR_SIZE 40

/** @brief Buffer size that holds any Thread network name, including the NUL. */
#define MESH_NETWORK_NAME_SIZE 17

/**
 * @brief A snapshot of where this node stands in the Thread mesh.
 *
 * Plain integers and strings only, so that the shell can print it without
 * pulling in any OpenThread header. Everything below @ref attached is only
 * meaningful while the node is attached.
 */
struct mesh_info {
	/** Device role: "disabled", "detached", "child", "router" or "leader". */
	const char *role;
	/** Whether the node is attached, that is child, router or leader. */
	bool attached;

	/** Operational dataset the node is configured with. */
	char network_name[MESH_NETWORK_NAME_SIZE];
	uint16_t pan_id;
	uint8_t channel;

	/** Routing locator of this node, 0xfffe while not attached. */
	uint16_t rloc16;
	/** Mesh-local endpoint identifier, the stable address of this node. */
	char mesh_local_eid[MESH_ADDR_STR_SIZE];

	/** Partition this node belongs to, and the router leading it. */
	uint32_t partition_id;
	uint8_t leader_router_id;

	/** Parent router, only valid while @ref has_parent is set (child role). */
	bool has_parent;
	uint16_t parent_rloc16;
	uint8_t parent_link_quality_in;
	uint8_t parent_link_quality_out;
	int8_t parent_avg_rssi;

	/** Neighbors this node has a direct link to. */
	uint16_t child_count;
	uint16_t router_neighbor_count;
};

/** @brief Whether the node is attached to the Thread mesh. */
bool mesh_is_attached(void);

/** @brief Current Thread device role: "disabled", "detached", "child", ... */
const char *mesh_role_name(void);

/**
 * @brief Fill @p info with a snapshot of the node's mesh state.
 *
 * Safe to call from any thread, and from an OpenThread state changed callback.
 *
 * @retval 0        On success.
 * @retval -EINVAL  @p info is NULL.
 * @retval -ENODEV  There is no OpenThread instance.
 */
int mesh_info_get(struct mesh_info *info);

#endif /* MESH_H */
