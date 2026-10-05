/*
 * Copyright (c) 2026 Jonas Remmert
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(sim_test_track, LOG_LEVEL_INF);

int main(void)
{
	LOG_INF("Sim and testing track booted");
	return 0;
}
