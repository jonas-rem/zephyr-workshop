/*
 * Copyright (c) 2026 Jonas Remmert
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app_test, LOG_LEVEL_INF);

int main(void)
{
	LOG_INF("App test booted");
	return 0;
}
