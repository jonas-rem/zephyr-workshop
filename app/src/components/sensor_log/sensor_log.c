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
	/* TODO Workshop: Implement ring buffer storage
	 *
	 * 1. Get entry size using sizeof(struct log_entry)
	 * 2. Try to write to ring buffer using ring_buf_put()
	 * 3. If write fails (buffer full):
	 *    - Log warning "Log buffer full, overwriting oldest entry"
	 *    - Clear space by reading oldest entry: ring_buf_get(&log_ring_buf, NULL, entry_size)
	 *    - Try writing again
	 * 4. Set has_entries = true after successful storage
	 */
}

static void sensor_log_event_cb(const struct zbus_channel *chan)
{
	/* TODO Workshop: Implement event logging
	 *
	 * 1. Get message from channel using zbus_chan_const_msg(chan)
	 * 2. Create a log_entry struct
	 * 3. Set entry.timestamp using k_uptime_get()
	 * 4. Switch on msg->event:
	 *    - SYS_SENSOR_READING: set type to LOG_ENTRY_SENSOR, store temp
	 *    - SYS_TEMP_ALERT: set type to LOG_ENTRY_ALERT, store temp
	 *    - default: return early (ignore other events like button presses)
	 * 5. Call store_entry(&entry) to save the entry
	 */
}

static void sensor_log_sys_ctl_cb(const struct zbus_channel *chan)
{
	/* TODO Workshop: Implement state change logging
	 *
	 * 1. Get state from channel using zbus_chan_const_msg(chan)
	 * 2. Create a log_entry struct
	 * 3. Set entry.timestamp using k_uptime_get()
	 * 4. Set entry.type to LOG_ENTRY_STATE
	 * 5. Set entry.data.state to the received state value
	 * 6. Call store_entry(&entry) to save the entry
	 */
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

#endif /* CONFIG_ZTEST */
