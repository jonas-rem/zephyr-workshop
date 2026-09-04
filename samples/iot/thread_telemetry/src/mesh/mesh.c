/*
 * Copyright (c) 2026 TiaC Systems
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <string.h>

#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <openthread.h>
#include <openthread/dataset.h>
#include <openthread/ip6.h>
#include <openthread/link.h>
#include <openthread/thread.h>

#include "mesh.h"

LOG_MODULE_REGISTER(mesh, CONFIG_TELEMETRY_MESH_LOG_LEVEL);

/* mesh.h keeps the OpenThread headers out, so pin its sizes to theirs here. */
BUILD_ASSERT(MESH_ADDR_STR_SIZE >= OT_IP6_ADDRESS_STRING_SIZE,
	     "MESH_ADDR_STR_SIZE is too small for an IPv6 address");
BUILD_ASSERT(MESH_NETWORK_NAME_SIZE >= OT_NETWORK_NAME_MAX_SIZE + 1,
	     "MESH_NETWORK_NAME_SIZE is too small for a Thread network name");

static bool role_is_attached(otDeviceRole role)
{
	switch (role) {
	case OT_DEVICE_ROLE_CHILD:
	case OT_DEVICE_ROLE_ROUTER:
	case OT_DEVICE_ROLE_LEADER:
		return true;
	default:
		return false;
	}
}

static otDeviceRole mesh_role(void)
{
	otInstance *ot = openthread_get_default_instance();
	otDeviceRole role;

	if (ot == NULL) {
		return OT_DEVICE_ROLE_DISABLED;
	}

	/*
	 * Recursive on purpose: the state changed callback below already runs
	 * with this mutex held, and a Zephyr k_mutex re-locks for its owner.
	 */
	openthread_mutex_lock();
	role = otThreadGetDeviceRole(ot);
	openthread_mutex_unlock();

	return role;
}

bool mesh_is_attached(void)
{
	return role_is_attached(mesh_role());
}

const char *mesh_role_name(void)
{
	return otThreadDeviceRoleToString(mesh_role());
}

/* Called with the OpenThread mutex held. */
static void mesh_info_parent(otInstance *ot, struct mesh_info *info)
{
	otRouterInfo parent;

	if (otThreadGetParentInfo(ot, &parent) != OT_ERROR_NONE) {
		return;
	}

	info->has_parent = true;
	info->parent_rloc16 = parent.mRloc16;
	info->parent_link_quality_in = parent.mLinkQualityIn;
	info->parent_link_quality_out = parent.mLinkQualityOut;

	/* Only known once a few frames have been heard, so a failure is not an error. */
	(void)otThreadGetParentAverageRssi(ot, &info->parent_avg_rssi);
}

/* Called with the OpenThread mutex held. */
static void mesh_info_neighbors(otInstance *ot, struct mesh_info *info)
{
	otNeighborInfoIterator it = OT_NEIGHBOR_INFO_ITERATOR_INIT;
	otNeighborInfo neighbor;

	while (otThreadGetNextNeighborInfo(ot, &it, &neighbor) == OT_ERROR_NONE) {
		if (neighbor.mIsChild) {
			info->child_count++;
		} else {
			info->router_neighbor_count++;
		}
	}
}

int mesh_info_get(struct mesh_info *info)
{
	otInstance *ot = openthread_get_default_instance();
	otDeviceRole role;

	if (info == NULL) {
		LOG_ERR("No buffer to fill the mesh info into");
		return -EINVAL;
	}

	if (ot == NULL) {
		LOG_ERR("No OpenThread instance");
		return -ENODEV;
	}

	memset(info, 0, sizeof(*info));

	openthread_mutex_lock();

	role = otThreadGetDeviceRole(ot);
	info->role = otThreadDeviceRoleToString(role);
	info->attached = role_is_attached(role);

	strncpy(info->network_name, otThreadGetNetworkName(ot), sizeof(info->network_name) - 1);
	info->pan_id = otLinkGetPanId(ot);
	info->channel = otLinkGetChannel(ot);

	info->rloc16 = otThreadGetRloc16(ot);
	otIp6AddressToString(otThreadGetMeshLocalEid(ot), info->mesh_local_eid,
			     sizeof(info->mesh_local_eid));

	if (info->attached) {
		info->partition_id = otThreadGetPartitionId(ot);
		info->leader_router_id = otThreadGetLeaderRouterId(ot);
		mesh_info_parent(ot, info);
		mesh_info_neighbors(ot, info);
	}

	openthread_mutex_unlock();

	return 0;
}

/*
 * Runs on the OpenThread thread with its mutex held, once per batch of
 * changes. The role is what the rest of the node acts on, so that one is
 * logged at INF; the rest is there to make the mesh visible while debugging.
 */
static void on_state_changed(otChangedFlags flags, void *context)
{
	struct mesh_info info;

	ARG_UNUSED(context);

	LOG_DBG("State changed, flags 0x%08x", flags);

	if (mesh_info_get(&info) != 0) {
		return;
	}

	if (flags & OT_CHANGED_THREAD_ROLE) {
		if (info.attached) {
			LOG_INF("Thread role is now %s, RLOC16 0x%04x in partition 0x%08x",
				info.role, info.rloc16, info.partition_id);
		} else {
			LOG_INF("Thread role is now %s", info.role);
		}
	}

	if ((flags & OT_CHANGED_THREAD_PARTITION_ID) && info.attached) {
		LOG_DBG("Partition is now 0x%08x, led by router %u", info.partition_id,
			info.leader_router_id);
	}

	if (flags & OT_CHANGED_THREAD_ML_ADDR) {
		LOG_DBG("Mesh-local EID is now %s", info.mesh_local_eid);
	}

	if (flags & OT_CHANGED_THREAD_RLOC_ADDED) {
		LOG_DBG("RLOC16 is now 0x%04x", info.rloc16);
	}

	if (flags & OT_CHANGED_THREAD_CHILD_ADDED) {
		LOG_INF("A child attached to this node, now %u children", info.child_count);
	}

	if (flags & OT_CHANGED_THREAD_CHILD_REMOVED) {
		LOG_INF("A child left this node, now %u children", info.child_count);
	}

	if (flags & OT_CHANGED_THREAD_NETDATA) {
		LOG_DBG("Thread network data changed");
	}

	if (flags & (OT_CHANGED_IP6_ADDRESS_ADDED | OT_CHANGED_IP6_ADDRESS_REMOVED)) {
		LOG_DBG("IPv6 addresses changed, see \"ot ipaddr\"");
	}
}

static struct openthread_state_changed_callback state_callback = {
	.otCallback = on_state_changed,
};

static int mesh_init(void)
{
	struct mesh_info info;
	int ret;

	/*
	 * The OpenThread instance is created by the L2, at POST_KERNEL, so it
	 * exists by the time any APPLICATION level hook runs.
	 */
	ret = mesh_info_get(&info);
	if (ret) {
		LOG_ERR("Cannot track the mesh without an OpenThread instance: %d", ret);
		return ret;
	}

	ret = openthread_state_changed_callback_register(&state_callback);
	if (ret) {
		LOG_ERR("Failed to register the OpenThread state callback: %d", ret);
		return ret;
	}

	LOG_INF("Tracking the Thread role on network '%s', PAN 0x%04x, channel %u",
		info.network_name, info.pan_id, info.channel);
	LOG_DBG("Thread role at boot: %s, mesh-local EID %s", info.role, info.mesh_local_eid);

	return 0;
}

SYS_INIT(mesh_init, APPLICATION, CONFIG_TELEMETRY_MESH_INIT_PRIORITY);
