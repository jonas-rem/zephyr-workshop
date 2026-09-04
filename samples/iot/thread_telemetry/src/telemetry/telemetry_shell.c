/*
 * Copyright (c) 2026 TiaC Systems
 * SPDX-License-Identifier: Apache-2.0
 *
 * Root of the "telemetry" command tree, plus the shell bindings that belong
 * to the publishing module itself. Compiled only when CONFIG_TELEMETRY_SHELL
 * is set; telemetry.c stays free of shell code.
 */

#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>

#include "mesh.h"
#include "node_name.h"
#include "telemetry.h"

/* Each module adds its own subcommands from its <module>_shell.c. */
SHELL_SUBCMD_SET_CREATE(telemetry_cmds, (telemetry));

static int cmd_collector(const struct shell *sh, size_t argc, char **argv)
{
	char addr[TELEMETRY_COLLECTOR_SIZE];
	int ret;

	if (argc == 1) {
		(void)telemetry_collector_get(addr, sizeof(addr));
		shell_print(sh, "%s", addr);
		return 0;
	}

	ret = telemetry_collector_set(argv[1]);
	if (ret) {
		shell_error(sh, "'%s' is not a valid IPv6 address", argv[1]);
		return ret;
	}

	shell_print(sh, "Collector address is now %s (not persisted)", argv[1]);

	return 0;
}

static int cmd_publish(const struct shell *sh, size_t argc, char **argv)
{
	int ret;

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	if (!mesh_is_attached()) {
		shell_warn(sh, "Not attached to the mesh, sending anyway");
	}

	ret = telemetry_publish_now();
	if (ret) {
		shell_error(sh, "Publish failed: %d", ret);
		return ret;
	}

	shell_print(sh, "Published");

	return 0;
}

static int cmd_status(const struct shell *sh, size_t argc, char **argv)
{
	char name[NODE_NAME_SIZE];
	char addr[TELEMETRY_COLLECTOR_SIZE];

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	(void)node_name_get(name, sizeof(name));
	(void)telemetry_collector_get(addr, sizeof(addr));

	shell_print(sh, "Node name : %s", name);
	shell_print(sh, "Collector : coap://[%s]/%s", addr, CONFIG_TELEMETRY_URI_PATH);
	shell_print(sh, "Interval  : %d s", CONFIG_TELEMETRY_INTERVAL_S);
	shell_print(sh, "Mesh role : %s", mesh_role_name());

	return 0;
}

SHELL_SUBCMD_ADD((telemetry), collector, NULL,
		 "Show or set the collector address: telemetry collector [<ipv6>]", cmd_collector,
		 1, 1);
SHELL_SUBCMD_ADD((telemetry), publish, NULL, "Publish one sample right now", cmd_publish, 1, 0);
SHELL_SUBCMD_ADD((telemetry), status, NULL, "Show the node configuration and mesh state",
		 cmd_status, 1, 0);

SHELL_CMD_REGISTER(telemetry, &telemetry_cmds, "Thread telemetry node commands", NULL);
