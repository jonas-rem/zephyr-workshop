/*
 * Copyright (c) 2026 TiaC Systems
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app, CONFIG_TELEMETRY_APP_LOG_LEVEL);

/*
 * Every module of this node brings itself up from its own SYS_INIT hook, so
 * there is deliberately nothing to start here -- see "Module layout" in the
 * sample documentation. main() only reports that the boot sequence finished.
 */
int main(void)
{
	LOG_INF("Thread telemetry node up on %s", CONFIG_BOARD_TARGET);

	return 0;
}
