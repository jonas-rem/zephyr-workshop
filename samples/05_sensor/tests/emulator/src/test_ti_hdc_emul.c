/*
 * Copyright (c) 2026 Jonas Remmert
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/drivers/emul.h>
#include <zephyr/drivers/emul_sensor.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/ztest.h>

#define TI_HDC_NODE DT_NODELABEL(ti_hdc)

static const struct device *const sensor = DEVICE_DT_GET(TI_HDC_NODE);
static const struct emul *const emul = EMUL_DT_GET(TI_HDC_NODE);

static void set_channel(enum sensor_channel channel, int64_t micro)
{
	struct sensor_chan_spec ch = {
		.chan_type = channel,
		.chan_idx = 0,
	};
	q31_t lower;
	q31_t upper;
	q31_t epsilon;
	q31_t value;
	int8_t shift;

	zassert_ok(emul_sensor_backend_get_sample_range(emul, ch, &lower, &upper,
							 &epsilon, &shift));
	value = ((micro * BIT64(31)) / 1000000) >> shift;
	zassert_ok(emul_sensor_backend_set_channel(emul, ch, &value, shift));
}

static void *ti_hdc_emul_setup(void)
{
	zassert_true(device_is_ready(sensor));
	zassert_true(emul_sensor_backend_is_supported(emul));
	return NULL;
}

ZTEST(ti_hdc_emul, test_set_channels)
{
	struct sensor_value temperature;
	struct sensor_value humidity;

	set_channel(SENSOR_CHAN_AMBIENT_TEMP, 25500000);
	set_channel(SENSOR_CHAN_HUMIDITY, 62500000);

	zassert_ok(sensor_sample_fetch(sensor));
	zassert_ok(sensor_channel_get(sensor, SENSOR_CHAN_AMBIENT_TEMP, &temperature));
	zassert_ok(sensor_channel_get(sensor, SENSOR_CHAN_HUMIDITY, &humidity));

	zassert_within(sensor_value_to_micro(&temperature), 25500000, 2600);
	zassert_within(sensor_value_to_micro(&humidity), 62500000, 1600);
}

ZTEST_SUITE(ti_hdc_emul, NULL, ti_hdc_emul_setup, NULL, NULL, NULL);
