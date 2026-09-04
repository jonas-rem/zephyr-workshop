/*
 * Copyright (c) 2026 TiaC Systems
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <string.h>

#include <zephyr/drivers/hwinfo.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>

#include "node_name.h"

LOG_MODULE_REGISTER(node_name, CONFIG_TELEMETRY_NODE_NAME_LOG_LEVEL);

#define NODE_NAME_KEY "telemetry/name"

static char node_name[NODE_NAME_SIZE];

/* The shell writes the name while the publish path reads it. */
static K_MUTEX_DEFINE(node_name_lock);

/*
 * The device ID rather than the Thread EUI-64, so that naming a board works
 * in the stages of the workshop that have no radio at all.
 */
static void node_name_set_default(void)
{
	uint8_t id[8];
	ssize_t len;

	len = hwinfo_get_device_id(id, sizeof(id));
	if (len < 3) {
		LOG_WRN("No device ID available (%d), falling back to a fixed name", (int)len);
		strcpy(node_name, "node-unknown");
		return;
	}

	snprintk(node_name, sizeof(node_name), "node-%02x%02x%02x", id[len - 3], id[len - 2],
		 id[len - 1]);

	LOG_DBG("Default name '%s' derived from the %d byte device ID", node_name, (int)len);
}

static int node_name_settings_set(const char *key, size_t len, settings_read_cb read_cb,
				  void *cb_arg)
{
	ssize_t read;

	if (settings_name_next(key, NULL) != 0) {
		LOG_ERR("Unexpected key '%s' under " NODE_NAME_KEY ", ignoring it", key);
		return -ENOENT;
	}

	if (len > CONFIG_TELEMETRY_NAME_MAX_LEN) {
		LOG_ERR("Stored node name is too long (%zu bytes), ignoring it", len);
		return -EINVAL;
	}

	/* Runs from node_name_init() below, before any other thread can read. */
	read = read_cb(cb_arg, node_name, len);
	if (read < 0) {
		LOG_ERR("Failed to read the stored node name: %d", (int)read);
		return (int)read;
	}

	node_name[read] = '\0';

	LOG_DBG("Loaded stored node name '%s' (%d bytes)", node_name, (int)read);

	return 0;
}

SETTINGS_STATIC_HANDLER_DEFINE(telemetry_name, NODE_NAME_KEY, NULL, node_name_settings_set, NULL,
			       NULL);

size_t node_name_get(char *buf, size_t size)
{
	size_t len;

	if (buf == NULL || size == 0) {
		return 0;
	}

	k_mutex_lock(&node_name_lock, K_FOREVER);
	len = MIN(strlen(node_name), size - 1);
	memcpy(buf, node_name, len);
	k_mutex_unlock(&node_name_lock);

	buf[len] = '\0';

	return len;
}

int node_name_set(const char *name)
{
	size_t len;
	int ret;

	if (name == NULL) {
		LOG_ERR("No node name given");
		return -EINVAL;
	}

	len = strlen(name);
	if (len == 0 || len > CONFIG_TELEMETRY_NAME_MAX_LEN) {
		LOG_ERR("Node name must be 1 to %d characters, got %zu",
			CONFIG_TELEMETRY_NAME_MAX_LEN, len);
		return -EINVAL;
	}

	/* Persist first, so a failed write leaves the previous name in use. */
	ret = settings_save_one(NODE_NAME_KEY, name, len);
	if (ret) {
		LOG_ERR("Failed to persist the node name: %d", ret);
		return ret;
	}

	k_mutex_lock(&node_name_lock, K_FOREVER);
	memcpy(node_name, name, len + 1);
	k_mutex_unlock(&node_name_lock);

	LOG_INF("Node name set to '%s'", name);

	return 0;
}

static int node_name_init(void)
{
	int ret;

	node_name_set_default();

	ret = settings_subsys_init();
	if (ret) {
		LOG_ERR("Failed to initialize settings: %d", ret);
		return ret;
	}

	/* Overwrites node_name via the handler above if a name was stored. */
	LOG_DBG("Loading '" NODE_NAME_KEY "' from settings");
	ret = settings_load_subtree("telemetry");
	if (ret) {
		LOG_ERR("Failed to load stored settings: %d", ret);
		return ret;
	}

	LOG_INF("Node name: %s", node_name);

	return 0;
}

SYS_INIT(node_name_init, APPLICATION, CONFIG_TELEMETRY_NODE_NAME_INIT_PRIORITY);
