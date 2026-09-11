/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 Navimatix GmbH
 * SPDX-FileCopyrightText: Copyright (c) 2021-2026 TiaC Systems
 * SPDX-FileCopyrightText: Copyright (c) 2019-2021 Li-Pro.Net
 * SPDX-FileCopyrightText: Copyright (c) 2012-2014 Wind River Systems, Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/sys/printk.h>

static int say_hello(void)
{
	printk("Hardware Evaluation! I'm THE My Shell from %s\n", CONFIG_BOARD);
	return 0;
}

/* Register our hello function  */
SYS_INIT(say_hello, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

/* Empty main */
int main(void) { return 0; }
