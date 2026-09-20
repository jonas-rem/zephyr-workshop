#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/ring_buffer.h>
#include <string.h>

#include "message_channel.h"

LOG_MODULE_REGISTER(sensor_log, CONFIG_SENSOR_LOG_LOG_LEVEL);

/* Log entry types */
enum log_entry_type {
	LOG_ENTRY_STATE,
	LOG_ENTRY_SENSOR,
	LOG_ENTRY_ALERT,
};

/* Single log entry */
struct log_entry {
	int64_t timestamp;
	enum log_entry_type type;
	union {
		enum sys_states state;
		int32_t temp;
	} data;
};

/* Ring buffer for log entries */
static struct ring_buf log_ring_buf;
static uint8_t log_buffer[CONFIG_SENSOR_LOG_BUFFER_SIZE * sizeof(struct log_entry)];

static bool has_entries;

static void store_entry(const struct log_entry *entry)
{
	size_t entry_size = sizeof(struct log_entry);
	uint32_t written;

	written = ring_buf_put(&log_ring_buf, (const uint8_t *)entry, entry_size);
	if (written != entry_size) {
		LOG_WRN("Log buffer full, overwriting oldest entry");
		ring_buf_get(&log_ring_buf, NULL, entry_size);
		written = ring_buf_put(&log_ring_buf, (const uint8_t *)entry, entry_size);
	}

	if (written == entry_size) {
		has_entries = true;
	}
}

static void sensor_log_event_cb(const struct zbus_channel *chan)
{
	const struct event_msg *msg = zbus_chan_const_msg(chan);
	struct log_entry entry = { .timestamp = k_uptime_get() };

	switch (msg->event) {
	case SYS_SENSOR_READING:
		entry.type = LOG_ENTRY_SENSOR;
		entry.data.temp = msg->sensor.temp;
		break;
	case SYS_TEMP_ALERT:
		entry.type = LOG_ENTRY_ALERT;
		entry.data.temp = msg->sensor.temp;
		break;
	default:
		return;
	}

	store_entry(&entry);
}

static void sensor_log_sys_ctl_cb(const struct zbus_channel *chan)
{
	const enum sys_states *state = zbus_chan_const_msg(chan);
	struct log_entry entry = {
		.timestamp = k_uptime_get(),
		.type = LOG_ENTRY_STATE,
		.data.state = *state,
	};

	store_entry(&entry);
}

ZBUS_LISTENER_DEFINE(sensor_log_event_listener, sensor_log_event_cb);
ZBUS_LISTENER_DEFINE(sensor_log_sys_ctl_listener, sensor_log_sys_ctl_cb);
ZBUS_CHAN_ADD_OBS(event_ch, sensor_log_event_listener, 1);
ZBUS_CHAN_ADD_OBS(sys_ctl_ch, sensor_log_sys_ctl_listener, 1);

#ifdef CONFIG_SENSOR_LOG_SHELL

static void print_entry(const struct shell *sh, const struct log_entry *entry)
{
	int64_t ms = entry->timestamp;
	int64_t secs = ms / 1000;
	int64_t frac = ms % 1000;

	switch (entry->type) {
	case LOG_ENTRY_STATE:
		shell_print(sh, "[%02lld:%02lld:%02lld.%03lld] STATE    %s",
			    secs / 3600, (secs % 3600) / 60, secs % 60, frac,
			    entry->data.state == SYS_ACTIVE ? "ACTIVE" : "SLEEP");
		break;

	case LOG_ENTRY_SENSOR:
		shell_print(sh, "[%02lld:%02lld:%02lld.%03lld] SENSOR   temp=%d.%02d °C",
			    secs / 3600, (secs % 3600) / 60, secs % 60, frac,
			    entry->data.temp / 100, entry->data.temp % 100);
		break;

	case LOG_ENTRY_ALERT:
		shell_print(sh, "[%02lld:%02lld:%02lld.%03lld] ALERT    temp=%d.%02d °C",
			    secs / 3600, (secs % 3600) / 60, secs % 60, frac,
			    entry->data.temp / 100, entry->data.temp % 100);
		break;
	}
}

static int cmd_sensor_log_history(const struct shell *sh, size_t argc, char **argv)
{
	struct log_entry entry;
	size_t entry_size = sizeof(struct log_entry);
	uint32_t count = 0;
	uint8_t *data_ptr;
	uint32_t remaining;
	uint32_t processed = 0;

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	if (!has_entries) {
		shell_print(sh, "No entries logged yet");
		return 0;
	}

	shell_print(sh, "Sensor log history:");

	/* Get total data size in buffer */
	remaining = ring_buf_size_get(&log_ring_buf);

	/* Process entries in chunks - ring buffer may wrap around */
	while (remaining >= entry_size) {
		uint32_t claimed;
		uint32_t chunk_size;

		/* Claim available data (may be less than remaining if buffer wraps) */
		claimed = ring_buf_get_claim(&log_ring_buf, &data_ptr, remaining);

		if (claimed < entry_size) {
			/* Not enough contiguous data, finish and break */
			ring_buf_get_finish(&log_ring_buf, 0);
			break;
		}

		/* Process all complete entries in this chunk */
		chunk_size = claimed - (claimed % entry_size);
		for (uint32_t i = 0; i < chunk_size; i += entry_size) {
			memcpy(&entry, data_ptr + i, entry_size);
			print_entry(sh, &entry);
			count++;
		}

		processed += chunk_size;
		remaining -= chunk_size;

		/* Finish without consuming (size = 0) to allow re-claim from beginning if wrapped */
		ring_buf_get_finish(&log_ring_buf, 0);
	}

	if (count == 0) {
		shell_print(sh, "(Buffer is empty)");
	}

	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sensor_log_cmds,
	SHELL_CMD(history, NULL, "Show all buffered log entries", cmd_sensor_log_history),
	SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(sensor_log, &sensor_log_cmds, "Sensor log commands", NULL);

#endif /* CONFIG_SENSOR_LOG_SHELL */

static int sensor_log_init(void)
{
	ring_buf_init(&log_ring_buf, sizeof(log_buffer), log_buffer);
	has_entries = false;

	LOG_INF("Sensor log module started (buffer: %d entries)",
		CONFIG_SENSOR_LOG_BUFFER_SIZE);

	return 0;
}

SYS_INIT(sensor_log_init, APPLICATION, CONFIG_SENSOR_LOG_INIT_PRIORITY);

#ifdef CONFIG_ZTEST

/* Test API: Get number of entries in buffer */
size_t sensor_log_get_entry_count(void)
{
	return ring_buf_size_get(&log_ring_buf) / sizeof(struct log_entry);
}

/* Test API: Get the most recent entry (for testing content validation)
 * Returns 0 on success, -ENOENT if buffer is empty
 */
int sensor_log_get_last_entry(struct log_entry *entry)
{
	size_t entry_size = sizeof(struct log_entry);
	size_t count = sensor_log_get_entry_count();
	uint8_t *data_ptr;
	uint32_t claimed;
	uint32_t remaining;
	uint32_t processed = 0;
	int ret = -ENOENT;

	if (count == 0 || entry == NULL) {
		return -ENOENT;
	}

	/* Get total data size in buffer */
	remaining = ring_buf_size_get(&log_ring_buf);

	/* Process entries to find the last one */
	while (remaining >= entry_size) {
		claimed = ring_buf_get_claim(&log_ring_buf, &data_ptr, remaining);
		if (claimed < entry_size) {
			ring_buf_get_finish(&log_ring_buf, 0);
			break;
		}

		uint32_t chunk_size = claimed - (claimed % entry_size);
		for (uint32_t i = 0; i < chunk_size; i += entry_size) {
			memcpy(entry, data_ptr + i, entry_size);
			processed += entry_size;
		}

		remaining -= chunk_size;
		ring_buf_get_finish(&log_ring_buf, 0);
	}

	/* If we found at least one entry, entry now contains the last one */
	if (processed >= entry_size) {
		ret = 0;
	}

	return ret;
}

/* Test API: Reset/clear the log buffer */
void sensor_log_reset(void)
{
	ring_buf_init(&log_ring_buf, sizeof(log_buffer), log_buffer);
	has_entries = false;
}

#endif /* CONFIG_ZTEST */
