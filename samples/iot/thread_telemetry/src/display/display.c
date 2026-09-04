/*
 * Copyright (c) 2026 TiaC Systems
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>

#include <zephyr/device.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/init.h>
#include <zephyr/input/input.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/atomic.h>

#include "display.h"
#include "env_sensor.h"

LOG_MODULE_REGISTER(display, CONFIG_TELEMETRY_DISPLAY_LOG_LEVEL);

#define STRIP_NODE DT_ALIAS(led_strip)
#define STRIP_LEN  DT_PROP(STRIP_NODE, chain_length)

#define PERMILLE_FULL 1000U

static const struct device *const strip = DEVICE_DT_GET(STRIP_NODE);

static struct led_rgb pixels[STRIP_LEN];

/* SW0 and the shell both switch modes, the work handler reads them. */
static atomic_t current_mode = ATOMIC_INIT(DISPLAY_MODE_TEMPERATURE);

static void display_work_handler(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(display_work, display_work_handler);

static const struct led_rgb temp_gradient[] = {
	{.r = 0, .g = 255, .b = 0},
	{.r = 255, .g = 255, .b = 0},
	{.r = 255, .g = 128, .b = 0},
	{.r = 255, .g = 0, .b = 0},
};

static const struct led_rgb humidity_gradient[] = {
	{.r = 0, .g = 0, .b = 255},
	{.r = 64, .g = 0, .b = 255},
	{.r = 128, .g = 0, .b = 255},
	{.r = 192, .g = 0, .b = 255},
	{.r = 255, .g = 0, .b = 255},
};

static uint32_t sensor_permille(const struct sensor_value *val, int32_t min, int32_t max)
{
	int32_t centi = val->val1 * 100 + val->val2 / 10000;
	int32_t span = (max - min) * 100;

	if (span <= 0) {
		return 0;
	}

	if (centi <= min * 100) {
		return 0;
	}

	if (centi >= max * 100) {
		return PERMILLE_FULL;
	}

	return (uint32_t)((centi - min * 100) * (int32_t)PERMILLE_FULL / span);
}

static uint8_t lerp8(uint8_t from, uint8_t to, uint32_t frac)
{
	int32_t delta = (int32_t)to - (int32_t)from;

	return (uint8_t)((int32_t)from + delta * (int32_t)frac / (int32_t)PERMILLE_FULL);
}

static struct led_rgb gradient_at(const struct led_rgb *stops, size_t count, uint32_t permille)
{
	const uint32_t segments = count - 1;
	uint32_t scaled = permille * segments;
	uint32_t index = scaled / PERMILLE_FULL;
	uint32_t frac = scaled % PERMILLE_FULL;
	struct led_rgb out;

	if (index >= segments) {
		return stops[segments];
	}

	out.r = lerp8(stops[index].r, stops[index + 1].r, frac);
	out.g = lerp8(stops[index].g, stops[index + 1].g, frac);
	out.b = lerp8(stops[index].b, stops[index + 1].b, frac);

	return out;
}

static struct led_rgb dim(struct led_rgb color, uint32_t permille)
{
	color.r = (uint8_t)(color.r * permille / PERMILLE_FULL);
	color.g = (uint8_t)(color.g * permille / PERMILLE_FULL);
	color.b = (uint8_t)(color.b * permille / PERMILLE_FULL);

	return color;
}

static void render_bar(uint32_t permille, const struct led_rgb *stops, size_t count)
{
	uint32_t scaled = permille * STRIP_LEN;
	uint32_t active = MIN(scaled / PERMILLE_FULL, STRIP_LEN - 1);
	uint32_t within = scaled - active * PERMILLE_FULL;

	for (size_t i = 0; i < STRIP_LEN; i++) {
		if (i < active) {
			pixels[i] = gradient_at(stops, count, PERMILLE_FULL);
		} else if (i == active) {
			pixels[i] = gradient_at(stops, count, within);
		} else {
			pixels[i] = (struct led_rgb){0};
		}
	}
}

static void render_temperature(const struct sensor_value *temp)
{
	render_bar(sensor_permille(temp, CONFIG_TELEMETRY_DISPLAY_TEMP_MIN_C,
				   CONFIG_TELEMETRY_DISPLAY_TEMP_MAX_C),
		   temp_gradient, ARRAY_SIZE(temp_gradient));
}

static void render_humidity(const struct sensor_value *hum)
{
	render_bar(sensor_permille(hum, CONFIG_TELEMETRY_DISPLAY_HUM_MIN_PCT,
				   CONFIG_TELEMETRY_DISPLAY_HUM_MAX_PCT),
		   humidity_gradient, ARRAY_SIZE(humidity_gradient));
}

/* One final pass, so both modes are scaled identically. */
static void apply_brightness(void)
{
	for (size_t i = 0; i < STRIP_LEN; i++) {
		pixels[i] = dim(pixels[i], CONFIG_TELEMETRY_DISPLAY_BRIGHTNESS);
	}
}

static void display_work_handler(struct k_work *work)
{
	struct sensor_value temp;
	struct sensor_value hum;
	int ret;

	ARG_UNUSED(work);

	ret = env_sensor_read(&temp, &hum);
	if (ret == 0) {
		if (display_mode_get() == DISPLAY_MODE_TEMPERATURE) {
			render_temperature(&temp);
		} else {
			render_humidity(&hum);
		}

		apply_brightness();

		ret = led_strip_update_rgb(strip, pixels, STRIP_LEN);
		if (ret) {
			LOG_ERR("Failed to update the LED strip: %d", ret);
		}
	}

	k_work_reschedule(&display_work, K_MSEC(CONFIG_TELEMETRY_DISPLAY_INTERVAL_MS));
}

enum display_mode display_mode_get(void)
{
	return (enum display_mode)atomic_get(&current_mode);
}

void display_mode_set(enum display_mode mode)
{
	if (atomic_set(&current_mode, mode) == (atomic_val_t)mode) {
		return;
	}

	LOG_INF("Display mode: %s", display_mode_name(mode));

	/* Redraw now rather than at the next scheduled refresh. */
	(void)k_work_reschedule(&display_work, K_NO_WAIT);
}

const char *display_mode_name(enum display_mode mode)
{
	return (mode == DISPLAY_MODE_TEMPERATURE) ? "temperature" : "humidity";
}

static void mode_button_cb(struct input_event *evt, void *user_data)
{
	ARG_UNUSED(user_data);

	/* Act on press only, so a long press does not toggle twice. */
	if (evt->type != INPUT_EV_KEY || evt->code != INPUT_KEY_0 || evt->value == 0) {
		return;
	}

	LOG_DBG("SW0 pressed, toggling the displayed quantity");

	display_mode_set(display_mode_get() == DISPLAY_MODE_TEMPERATURE ? DISPLAY_MODE_HUMIDITY
									: DISPLAY_MODE_TEMPERATURE);
}

INPUT_CALLBACK_DEFINE(NULL, mode_button_cb, NULL);

static int display_init(void)
{
	if (!device_is_ready(strip)) {
		LOG_ERR("LED strip %s is not ready", strip->name);
		return -ENODEV;
	}

	LOG_INF("Showing %s on %d LEDs, press SW0 to switch", display_mode_name(display_mode_get()),
		STRIP_LEN);
	LOG_DBG("Refreshing %s every %d ms at %d/1000 brightness, %d..%d C and %d..%d %%rH",
		strip->name, CONFIG_TELEMETRY_DISPLAY_INTERVAL_MS,
		CONFIG_TELEMETRY_DISPLAY_BRIGHTNESS, CONFIG_TELEMETRY_DISPLAY_TEMP_MIN_C,
		CONFIG_TELEMETRY_DISPLAY_TEMP_MAX_C, CONFIG_TELEMETRY_DISPLAY_HUM_MIN_PCT,
		CONFIG_TELEMETRY_DISPLAY_HUM_MAX_PCT);

	k_work_reschedule(&display_work, K_NO_WAIT);

	return 0;
}

SYS_INIT(display_init, APPLICATION, CONFIG_TELEMETRY_DISPLAY_INIT_PRIORITY);
