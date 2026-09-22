/*
 * Copyright (c) 2026 Jonas Remmert
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT ti_hdc

#include <zephyr/device.h>
#include <zephyr/drivers/emul.h>
#include <zephyr/drivers/emul_sensor.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/i2c_emul.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/random/random.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/util.h>

LOG_MODULE_REGISTER(ti_hdc_emul, CONFIG_SENSOR_LOG_LEVEL);

/* Register addresses — must match the real driver (ti_hdc.h) */
#define TI_HDC_REG_TEMP      0x00
#define TI_HDC_REG_HUMIDITY  0x01
#define TI_HDC_REG_MANUFID   0xFE
#define TI_HDC_REG_DEVICEID  0xFF

/*
 * Raw register ranges for random readings.
 * HDC formula: temp_C = raw * 165 / 65536 - 40
 *              hum_%  = raw * 100 / 65536
 *
 * Inverse formula: raw = (temp_C + 40) * 65536 / 165
 */
#define TEMP_RAW_MIN  23418  /* 19 °C: (19+40) * 65536 / 165 */
#define TEMP_RAW_MAX  24211  /* 21 °C: (21+40) * 65536 / 165 */
#define HUM_RAW_MIN   26214  /* 40 %:  40 * 65536 / 100 */
#define HUM_RAW_MAX   39321  /* 60 %:  60 * 65536 / 100 */

/* Temperature ranges for the exercise (first 10 readings: 3-5°C, after: 4-7°C) */
#define TEMP_RAW_3C   17079  /* 3 °C:  (3+40) * 65536 / 165 */
#define TEMP_RAW_4C   17476  /* 4 °C:  (4+40) * 65536 / 165 */
#define TEMP_RAW_5C   17873  /* 5 °C:  (5+40) * 65536 / 165 */
#define TEMP_RAW_7C   18668  /* 7 °C:  (7+40) * 65536 / 165 */

struct ti_hdc_emul_data {
	uint8_t cur_reg;         /* last register address written */
	uint16_t reg_temp;       /* reg 0x00 — raw temperature */
	uint16_t reg_humidity;   /* reg 0x01 — raw humidity */
	uint16_t reg_manufid;    /* reg 0xFE */
	uint16_t reg_deviceid;   /* reg 0xFF */
	uint32_t sample_count;   /* number of temperature readings generated */
	bool fixed_temp;         /* set through the sensor emulator API */
	bool fixed_humidity;     /* set through the sensor emulator API */
	struct k_mutex sample_lock;
};

struct ti_hdc_emul_cfg {
};

static uint16_t rand_in_range(uint16_t min, uint16_t max)
{
	uint32_t range = (uint32_t)(max - min) + 1;

	return min + (sys_rand32_get() % range);
}

static void ti_hdc_emul_generate_sample(struct ti_hdc_emul_data *data)
{
	/* First 10 readings: 3-5°C, following readings: 4-7°C */
	if (!data->fixed_temp) {
		if (data->sample_count < 10) {
			data->reg_temp = rand_in_range(TEMP_RAW_3C, TEMP_RAW_5C);
		} else {
			data->reg_temp = rand_in_range(TEMP_RAW_4C, TEMP_RAW_7C);
		}
	}
	data->sample_count++;
	if (!data->fixed_humidity) {
		data->reg_humidity = rand_in_range(HUM_RAW_MIN, HUM_RAW_MAX);
	}
}

/**
 * Return the 16-bit big-endian value for a given register address.
 * Returns -EINVAL for unknown registers.
 */
static int ti_hdc_emul_reg_read(struct ti_hdc_emul_data *data, uint8_t reg,
				uint8_t *buf)
{
	uint16_t val;

	switch (reg) {
	case TI_HDC_REG_TEMP:
		val = data->reg_temp;
		break;
	case TI_HDC_REG_HUMIDITY:
		val = data->reg_humidity;
		break;
	case TI_HDC_REG_MANUFID:
		val = data->reg_manufid;
		break;
	case TI_HDC_REG_DEVICEID:
		val = data->reg_deviceid;
		break;
	default:
		LOG_ERR("Unknown register 0x%02x", reg);
		return -EINVAL;
	}

	sys_put_be16(val, buf);
	return 0;
}

/**
 * I2C transfer handler.
 *
 * The real TI HDC driver generates three message patterns:
 *
 *  A) i2c_burst_read_dt (init — read manufacturer/device ID):
 *     msg[0]: write 1 byte (register address), no STOP
 *     msg[1]: read 2 bytes, STOP
 *
 *  B) i2c_write_dt (sample trigger):
 *     msg[0]: write 1 byte (register address), STOP
 *
 *  C) i2c_read_dt (sample data):
 *     msg[0]: read 4 bytes, STOP  (temp + humidity, auto-increment)
 */
static int ti_hdc_emul_transfer(const struct emul *target,
				struct i2c_msg *msgs, int num_msgs,
				int addr)
{
	struct ti_hdc_emul_data *data = target->data;

	if (num_msgs < 1) {
		LOG_ERR("Invalid number of messages: %d", num_msgs);
		return -EIO;
	}

	/* Pattern C: pure read — return from cur_reg with auto-increment */
	if (num_msgs == 1 && (msgs[0].flags & I2C_MSG_READ)) {
		uint8_t reg = data->cur_reg;
		int remaining = msgs[0].len;
		uint8_t *buf = msgs[0].buf;

		k_mutex_lock(&data->sample_lock, K_FOREVER);
		while (remaining >= 2) {
			int ret = ti_hdc_emul_reg_read(data, reg, buf);

			if (ret < 0) {
				k_mutex_unlock(&data->sample_lock);
				return ret;
			}
			buf += 2;
			remaining -= 2;
			reg++;
		}
		k_mutex_unlock(&data->sample_lock);
		return 0;
	}

	/* First message must be a write containing the register address */
	if (msgs[0].flags & I2C_MSG_READ) {
		LOG_ERR("Expected write for register address, got read");
		return -EIO;
	}
	if (msgs[0].len < 1) {
		LOG_ERR("Write message too short: %d", msgs[0].len);
		return -EIO;
	}

	data->cur_reg = msgs[0].buf[0];

	/* Pattern B: write-only (sample trigger) — generate new readings */
	if (num_msgs == 1) {
		k_mutex_lock(&data->sample_lock, K_FOREVER);
		if (data->cur_reg == TI_HDC_REG_TEMP) {
			ti_hdc_emul_generate_sample(data);
		}
		k_mutex_unlock(&data->sample_lock);
		return 0;
	}

	/* Pattern A: write + read (burst read) */
	if (num_msgs == 2 && (msgs[1].flags & I2C_MSG_READ)) {
		uint8_t reg = data->cur_reg;
		int remaining = msgs[1].len;
		uint8_t *buf = msgs[1].buf;

		k_mutex_lock(&data->sample_lock, K_FOREVER);
		while (remaining >= 2) {
			int ret = ti_hdc_emul_reg_read(data, reg, buf);

			if (ret < 0) {
				k_mutex_unlock(&data->sample_lock);
				return ret;
			}
			buf += 2;
			remaining -= 2;
			reg++;
		}
		k_mutex_unlock(&data->sample_lock);
		return 0;
	}

	LOG_ERR("Unhandled I2C pattern: %d msgs", num_msgs);

	return -EIO;
}

static int ti_hdc_emul_init(const struct emul *target,
			    const struct device *parent)
{
	struct ti_hdc_emul_data *data = target->data;

	ARG_UNUSED(parent);

	data->cur_reg = 0;
	data->reg_manufid = 0x5449;   /* TI manufacturer ID */
	data->reg_deviceid = 0x1000;  /* HDC1000/1010 device ID */
	k_mutex_init(&data->sample_lock);

	ti_hdc_emul_generate_sample(data);

	return 0;
}

static int ti_hdc_emul_set_channel(const struct emul *target, struct sensor_chan_spec ch,
				   const q31_t *value, int8_t shift)
{
	struct ti_hdc_emul_data *data = target->data;
	int64_t scaled;
	int64_t lower;
	int64_t upper;
	uint32_t raw;

	if (value == NULL || shift < 0 || shift > 31) {
		return -EINVAL;
	}
	if (ch.chan_idx != 0) {
		return -ENOTSUP;
	}

	scaled = (int64_t)*value * BIT64(shift);

	switch (ch.chan_type) {
	case SENSOR_CHAN_AMBIENT_TEMP:
		lower = -40LL * BIT64(31);
		upper = 125LL * BIT64(31);
		if (scaled < lower || scaled > upper) {
			return -ERANGE;
		}

		raw = DIV_ROUND_CLOSEST((scaled - lower) * BIT64(16), 165LL * BIT64(31));
		k_mutex_lock(&data->sample_lock, K_FOREVER);
		data->reg_temp = MIN(raw, UINT16_MAX);
		data->fixed_temp = true;
		k_mutex_unlock(&data->sample_lock);
		break;
	case SENSOR_CHAN_HUMIDITY:
		lower = 0;
		upper = 100LL * BIT64(31);
		if (scaled < lower || scaled > upper) {
			return -ERANGE;
		}

		raw = DIV_ROUND_CLOSEST(scaled * BIT64(16), upper);
		k_mutex_lock(&data->sample_lock, K_FOREVER);
		data->reg_humidity = MIN(raw, UINT16_MAX);
		data->fixed_humidity = true;
		k_mutex_unlock(&data->sample_lock);
		break;
	default:
		return -ENOTSUP;
	}

	return 0;
}

static int ti_hdc_emul_get_sample_range(const struct emul *target, struct sensor_chan_spec ch,
					q31_t *lower, q31_t *upper, q31_t *epsilon, int8_t *shift)
{
	ARG_UNUSED(target);

	if (lower == NULL || upper == NULL || epsilon == NULL || shift == NULL) {
		return -EINVAL;
	}
	if (ch.chan_idx != 0) {
		return -ENOTSUP;
	}

	*shift = 8;
	switch (ch.chan_type) {
	case SENSOR_CHAN_AMBIENT_TEMP:
		*lower = (-40LL * BIT64(31)) >> *shift;
		*upper = (125LL * BIT64(31)) >> *shift;
		*epsilon = DIV_ROUND_UP(165LL * BIT64(31), BIT64(16)) >> *shift;
		break;
	case SENSOR_CHAN_HUMIDITY:
		*lower = 0;
		*upper = (100LL * BIT64(31)) >> *shift;
		*epsilon = DIV_ROUND_UP(100LL * BIT64(31), BIT64(16)) >> *shift;
		break;
	default:
		return -ENOTSUP;
	}

	return 0;
}

static const struct i2c_emul_api ti_hdc_emul_api = {
	.transfer = ti_hdc_emul_transfer,
};

static const struct emul_sensor_driver_api ti_hdc_emul_backend_api = {
	.set_channel = ti_hdc_emul_set_channel,
	.get_sample_range = ti_hdc_emul_get_sample_range,
};

#define TI_HDC_EMUL(n)                                             \
	static const struct ti_hdc_emul_cfg ti_hdc_emul_cfg_##n;   \
	static struct ti_hdc_emul_data ti_hdc_emul_data_##n;       \
	EMUL_DT_INST_DEFINE(n, ti_hdc_emul_init,                   \
			    &ti_hdc_emul_data_##n,                 \
			    &ti_hdc_emul_cfg_##n,                  \
			    &ti_hdc_emul_api,                      \
			    &ti_hdc_emul_backend_api)

DT_INST_FOREACH_STATUS_OKAY(TI_HDC_EMUL)
