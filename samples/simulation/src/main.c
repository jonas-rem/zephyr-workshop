/*
 * Copyright (c) 2026 Jonas Remmert
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app_simulation, LOG_LEVEL_INF);

int main(void)
{
	LOG_INF("App simulation booted");
	return 0;
}
