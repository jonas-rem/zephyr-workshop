/*
 * Copyright (c) 2026 TiaC Systems
 * SPDX-License-Identifier: Apache-2.0
 *
 * The root of the "telemetry" command tree. Every module fills it in from its
 * own <module>_shell.c, so this file knows about none of them.
 */

#include <zephyr/shell/shell.h>

#include "app_status.h"

SHELL_SUBCMD_SET_CREATE(telemetry_cmds, (telemetry));

static int cmd_status(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	STRUCT_SECTION_FOREACH(app_status_entry, entry) {
		entry->print(sh);
	}

	return 0;
}

SHELL_SUBCMD_ADD((telemetry), status, NULL, "Show what every module of this node is doing",
		 cmd_status, 1, 0);

SHELL_CMD_REGISTER(telemetry, &telemetry_cmds, "Thread telemetry node commands", NULL);
