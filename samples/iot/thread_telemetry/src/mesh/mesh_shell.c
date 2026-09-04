/*
 * Copyright (c) 2026 TiaC Systems
 * SPDX-License-Identifier: Apache-2.0
 *
 * Shell bindings for the mesh state. Joining and leaving is OpenThread's
 * business and the "ot" commands already cover it, so this only reports:
 * one line in "telemetry status", and "telemetry mesh" for the full picture.
 * Compiled only when CONFIG_TELEMETRY_SHELL is set; mesh.c stays free of
 * shell code.
 */

#include <zephyr/shell/shell.h>

#include "app_status.h"
#include "mesh.h"

static void mesh_status(const struct shell *sh)
{
	APP_STATUS_PRINT(sh, "Mesh role", "%s", mesh_role_name());
}

static int cmd_mesh(const struct shell *sh, size_t argc, char **argv)
{
	struct mesh_info info;
	int ret;

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	ret = mesh_info_get(&info);
	if (ret) {
		shell_error(sh, "Failed to read the mesh state: %d", ret);
		return ret;
	}

	APP_STATUS_PRINT(sh, "Role", "%s%s", info.role, info.attached ? " (attached)" : "");
	APP_STATUS_PRINT(sh, "Network", "'%s', PAN 0x%04x, channel %u", info.network_name,
			 info.pan_id, info.channel);
	APP_STATUS_PRINT(sh, "ML-EID", "%s", info.mesh_local_eid);

	if (!info.attached) {
		shell_print(sh, "Not attached, so there is no partition, parent or neighbor yet");
		return 0;
	}

	APP_STATUS_PRINT(sh, "RLOC16", "0x%04x", info.rloc16);
	APP_STATUS_PRINT(sh, "Partition", "0x%08x, led by router %u", info.partition_id,
			 info.leader_router_id);

	if (info.has_parent) {
		APP_STATUS_PRINT(sh, "Parent", "0x%04x, link quality in/out %u/%u, RSSI %d dBm",
				 info.parent_rloc16, info.parent_link_quality_in,
				 info.parent_link_quality_out, info.parent_avg_rssi);
	}

	APP_STATUS_PRINT(sh, "Neighbors", "%u children, %u routers", info.child_count,
			 info.router_neighbor_count);

	return 0;
}

APP_STATUS_ENTRY_DEFINE(e_mesh, mesh_status);

SHELL_SUBCMD_ADD((telemetry), mesh, NULL,
		 "Show the mesh state: role, network, addresses, parent and neighbors", cmd_mesh, 1,
		 0);
