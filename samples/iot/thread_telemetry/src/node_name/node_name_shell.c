/*
 * Copyright (c) 2026 TiaC Systems
 * SPDX-License-Identifier: Apache-2.0
 *
 * Shell bindings for the node name. Compiled only when
 * CONFIG_TELEMETRY_SHELL is set; node_name.c stays free of shell code.
 */

#include <errno.h>

#include <zephyr/shell/shell.h>

#include "node_name.h"

static int cmd_name(const struct shell *sh, size_t argc, char **argv)
{
	char name[NODE_NAME_SIZE];
	int ret;

	if (argc == 1) {
		(void)node_name_get(name, sizeof(name));
		shell_print(sh, "%s", name);
		return 0;
	}

	ret = node_name_set(argv[1]);
	if (ret == -EINVAL) {
		shell_error(sh, "Name must be 1 to %d characters",
			    CONFIG_TELEMETRY_NAME_MAX_LEN);
		return ret;
	} else if (ret) {
		shell_error(sh, "Failed to store the name: %d", ret);
		return ret;
	}

	shell_print(sh, "Node name is now '%s' and survives a reboot", argv[1]);

	return 0;
}

SHELL_SUBCMD_ADD((telemetry), name, NULL, "Show or set the node name: telemetry name [<name>]",
		 cmd_name, 1, 1);
