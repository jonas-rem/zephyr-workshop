/*
 * Copyright (c) 2026 Jonas Remmert
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT ti_hdc

#include <zephyr/device.h>
#include <zephyr/drivers/emul.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/i2c_emul.h>
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
 */
#define TEMP_RAW_MIN  23418  /* 19 °C: (19+40) * 65536 / 165 */
#define TEMP_RAW_MAX  24211  /* 21 °C: (21+40) * 65536 / 165 */
#define HUM_RAW_MIN   26214  /* 40 %:  40 * 65536 / 100 */
#define HUM_RAW_MAX   39321  /* 60 %:  60 * 65536 / 100 */

struct ti_hdc_emul_data {
	uint8_t cur_reg;         /* last register address written */
	uint16_t reg_temp;       /* reg 0x00 — raw temperature */
	uint16_t reg_humidity;   /* reg 0x01 — raw humidity */
	uint16_t reg_manufid;    /* reg 0xFE */
	uint16_t reg_deviceid;   /* reg 0xFF */
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
	data->reg_temp = rand_in_range(TEMP_RAW_MIN, TEMP_RAW_MAX);
	data->reg_humidity = rand_in_range(HUM_RAW_MIN, HUM_RAW_MAX);
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

		while (remaining >= 2) {
			int ret = ti_hdc_emul_reg_read(data, reg, buf);

			if (ret < 0) {
				return ret;
			}
			buf += 2;
			remaining -= 2;
			reg++;
		}
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
		if (data->cur_reg == TI_HDC_REG_TEMP) {
			ti_hdc_emul_generate_sample(data);
		}
		return 0;
	}

	/* Pattern A: write + read (burst read) */
	if (num_msgs == 2 && (msgs[1].flags & I2C_MSG_READ)) {
		uint8_t reg = data->cur_reg;
		int remaining = msgs[1].len;
		uint8_t *buf = msgs[1].buf;

		while (remaining >= 2) {
			int ret = ti_hdc_emul_reg_read(data, reg, buf);

			if (ret < 0) {
				return ret;
			}
			buf += 2;
			remaining -= 2;
			reg++;
		}
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

	ti_hdc_emul_generate_sample(data);

	return 0;
}

static const struct i2c_emul_api ti_hdc_emul_api = {
	.transfer = ti_hdc_emul_transfer,
};

#define TI_HDC_EMUL(n)                                             \
	static const struct ti_hdc_emul_cfg ti_hdc_emul_cfg_##n;   \
	static struct ti_hdc_emul_data ti_hdc_emul_data_##n;       \
	EMUL_DT_INST_DEFINE(n, ti_hdc_emul_init,                   \
			    &ti_hdc_emul_data_##n,                 \
			    &ti_hdc_emul_cfg_##n,                  \
			    &ti_hdc_emul_api, NULL)

DT_INST_FOREACH_STATUS_OKAY(TI_HDC_EMUL)
