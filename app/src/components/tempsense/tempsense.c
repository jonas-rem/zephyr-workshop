#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/device.h>

#include "message_channel.h"

LOG_MODULE_REGISTER(tempsense, CONFIG_TEMPSENSE_LOG_LEVEL);

/* Forward declarations */
static void tempsense_work_handler(struct k_work *work);
static int tempsense_read_sensor(struct sensor_value *temp_val);

/* Delayable work for periodic sensor reading - initialized at compile time */
K_WORK_DELAYABLE_DEFINE(tempsense_work, tempsense_work_handler);

/* Sensor device */
static const struct device *sensor_dev;

/* Current sampling state */
static bool sampling_active;

/* Read temperature from the sensor into sensor_value struct */
static int tempsense_read_sensor(struct sensor_value *temp_val)
{
	/* TODO Workshop: Implement sensor reading
	 *
	 * 1. Check if sensor_dev is ready using device_is_ready()
	 * 2. Fetch sample using sensor_sample_fetch(sensor_dev)
	 * 3. Get temperature using sensor_channel_get(sensor_dev, SENSOR_CHAN_AMBIENT_TEMP, temp_val)
	 * 4. Return 0 on success, negative error code on failure
	 */
	return -ENOSYS;
}

/* Get temperature from sensor and publish to event_ch */
static int read_and_publish_temperature(void)
{
	/* TODO Workshop: Implement sensor reading and publishing
	 *
	 * 1. Call tempsense_read_sensor() to get temperature
	 * 2. Convert to 0.01°C: temp_centi = (temp_val.val1 * 100) + (temp_val.val2 / 10000)
	 * 3. Create event_msg with event = SYS_SENSOR_READING and sensor.temp = temp_centi
	 * 4. Publish to event_ch using zbus_chan_pub()
	 * 5. Return 0 on success, negative error code on failure
	 */
	return -ENOSYS;
}

/* Work handler for periodic sampling */
static void tempsense_work_handler(struct k_work *work)
{
	/* TODO Workshop: Implement periodic sampling
	 *
	 * 1. Check if sampling_active is true, return early if not
	 * 2. Call read_and_publish_temperature() to read and publish
	 * 3. Reschedule next reading using k_work_reschedule()
	 *    with K_MSEC(CONFIG_TEMPSENSE_SAMPLE_INTERVAL_MS)
	 */
}

/* Start periodic sampling */
static void tempsense_start(void)
{
	/* TODO Workshop: Implement sampling start
	 *
	 * 1. Check if already active, return early if sampling_active is true
	 * 2. Set sampling_active = true
	 * 3. Log "Temperature sampling started"
	 * 4. Schedule first reading using k_work_reschedule(&tempsense_work, K_NO_WAIT)
	 */
}

/* Stop periodic sampling */
static void tempsense_stop(void)
{
	/* TODO Workshop: Implement sampling stop
	 *
	 * 1. Check if already stopped, return early if sampling_active is false
	 * 2. Set sampling_active = false
	 * 3. Cancel pending work using k_work_cancel_delayable(&tempsense_work)
	 * 4. Log "Temperature sampling stopped"
	 */
}

/* ZBus callback for system state changes */
static void tempsense_sys_ctl_cb(const struct zbus_channel *chan)
{
	/* TODO Workshop: Implement state change handling
	 *
	 * 1. Get state from channel using zbus_chan_const_msg(chan)
	 * 2. If state is SYS_ACTIVE, call tempsense_start()
	 * 3. Otherwise, call tempsense_stop()
	 */
}

ZBUS_LISTENER_DEFINE(tempsense_sys_ctl_listener, tempsense_sys_ctl_cb);
ZBUS_CHAN_ADD_OBS(sys_ctl_ch, tempsense_sys_ctl_listener, 1);

#ifdef CONFIG_TEMPSENSE_SHELL
#include <zephyr/shell/shell.h>

static int cmd_tempsense_read(const struct shell *sh, size_t argc, char **argv)
{
	struct sensor_value temp_val;
	int err;

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	err = tempsense_read_sensor(&temp_val);
	if (err) {
		shell_error(sh, "Failed to read temperature: %d", err);
		return err;
	}

	shell_print(sh, "Temperature: %d.%06d C", temp_val.val1, temp_val.val2);
	return 0;
}

static int cmd_tempsense_status(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	shell_print(sh, "Temperature sensor status:");
	shell_print(sh, "  Sampling: %s", sampling_active ? "active" : "stopped");
	shell_print(sh, "  Interval: %d ms", CONFIG_TEMPSENSE_SAMPLE_INTERVAL_MS);
	shell_print(sh, "  Device ready: %s",
		    device_is_ready(sensor_dev) ? "yes" : "no");
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(tempsense_cmds,
	SHELL_CMD(read, NULL, "Read temperature manually", cmd_tempsense_read),
	SHELL_CMD(status, NULL, "Show tempsense status", cmd_tempsense_status),
	SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(tempsense, &tempsense_cmds, "Temperature sensor commands", NULL);

#endif /* CONFIG_TEMPSENSE_SHELL */

static int tempsense_init(void)
{
	sensor_dev = DEVICE_DT_GET_ONE(ti_hdc);

	if (!device_is_ready(sensor_dev)) {
		LOG_ERR("Temperature sensor device not ready");
		return -ENODEV;
	}

	sampling_active = false;

	LOG_INF("Temperature sensor module started (interval: %d ms, ~%d s)",
		CONFIG_TEMPSENSE_SAMPLE_INTERVAL_MS,
		CONFIG_TEMPSENSE_SAMPLE_INTERVAL_MS / 1000);

	return 0;
}

SYS_INIT(tempsense_init, APPLICATION, CONFIG_TEMPSENSE_INIT_PRIORITY);
