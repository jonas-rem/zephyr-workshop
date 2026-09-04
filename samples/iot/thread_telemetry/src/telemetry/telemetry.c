/*
 * Copyright (c) 2026 TiaC Systems
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <zephyr/drivers/sensor.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/net_ip.h>

#include <openthread.h>
#include <openthread/coap.h>
#include <openthread/thread.h>

#include "env_sensor.h"
#include "mesh.h"
#include "node_name.h"
#include "telemetry.h"

LOG_MODULE_REGISTER(telemetry, CONFIG_TELEMETRY_LOG_LEVEL);

#define CENTI_STR_SIZE 16

/* Payload size is derived from the name limit; see the sample documentation. */
#define TELEMETRY_JSON_OVERHEAD     44
#define TELEMETRY_UPTIME_MAX_DIGITS 20
#define TELEMETRY_PAYLOAD_SIZE                                                                     \
	(TELEMETRY_JSON_OVERHEAD + CONFIG_TELEMETRY_NAME_MAX_LEN + 2 * (CENTI_STR_SIZE - 1) +       \
	 TELEMETRY_UPTIME_MAX_DIGITS + 1)

BUILD_ASSERT(TELEMETRY_PAYLOAD_SIZE <= 256,
	     "Telemetry payload too large for a single 6LoWPAN datagram; "
	     "lower CONFIG_TELEMETRY_NAME_MAX_LEN");

static void telemetry_work_handler(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(telemetry_work, telemetry_work_handler);

/* The shell changes the collector while the publish path reads it. */
static K_MUTEX_DEFINE(collector_lock);
static char collector_addr[TELEMETRY_COLLECTOR_SIZE];
static otIp6Address collector_ip;

static void centi_str(char *buf, size_t size, const struct sensor_value *val)
{
	int32_t centi = val->val1 * 100 + val->val2 / 10000;

	snprintk(buf, size, "%s%d.%02d", centi < 0 ? "-" : "", abs(centi) / 100, abs(centi) % 100);
}

static int telemetry_format(char *buf, size_t size, const struct sensor_value *temp,
			    const struct sensor_value *hum)
{
	char name[NODE_NAME_SIZE];
	char temp_str[CENTI_STR_SIZE];
	char hum_str[CENTI_STR_SIZE];
	int len;

	(void)node_name_get(name, sizeof(name));
	centi_str(temp_str, sizeof(temp_str), temp);
	centi_str(hum_str, sizeof(hum_str), hum);

	len = snprintk(buf, size,
		       "{\"node\":\"%s\",\"temp_c\":%s,\"hum_pct\":%s,\"uptime_s\":%lld}",
		       name, temp_str, hum_str, (long long)(k_uptime_get() / MSEC_PER_SEC));

	if (len < 0 || (size_t)len >= size) {
		LOG_ERR("Payload does not fit into %zu bytes", size);
		return -ENOMEM;
	}

	LOG_DBG("Formatted %d bytes of JSON", len);

	return len;
}

/* Fill in a freshly allocated message. Called with the OpenThread mutex held. */
static otError coap_message_fill(otMessage *msg, const uint8_t *payload, size_t len)
{
	otError err;

	otCoapMessageInit(msg, OT_COAP_TYPE_NON_CONFIRMABLE, OT_COAP_CODE_POST);

	err = otCoapMessageAppendUriPathOptions(msg, CONFIG_TELEMETRY_URI_PATH);
	if (err != OT_ERROR_NONE) {
		LOG_ERR("Failed to append the URI path: %s", otThreadErrorToString(err));
		return err;
	}

	err = otCoapMessageAppendContentFormatOption(msg, OT_COAP_OPTION_CONTENT_FORMAT_JSON);
	if (err != OT_ERROR_NONE) {
		LOG_ERR("Failed to append the content format: %s", otThreadErrorToString(err));
		return err;
	}

	err = otCoapMessageSetPayloadMarker(msg);
	if (err != OT_ERROR_NONE) {
		LOG_ERR("Failed to set the payload marker: %s", otThreadErrorToString(err));
		return err;
	}

	err = otMessageAppend(msg, payload, len);
	if (err != OT_ERROR_NONE) {
		LOG_ERR("Failed to append the payload: %s", otThreadErrorToString(err));
		return err;
	}

	return OT_ERROR_NONE;
}

/*
 * Owns the message from allocation to hand-over. otCoapSendRequest() takes
 * ownership only when it succeeds, so every other path frees it here.
 * Called with the OpenThread mutex held.
 */
static int coap_post(otInstance *ot, const otMessageInfo *msg_info, const uint8_t *payload,
		     size_t len)
{
	otMessage *msg;
	otError err;

	msg = otCoapNewMessage(ot, NULL);
	if (msg == NULL) {
		LOG_ERR("Failed to allocate a CoAP message");
		return -ENOMEM;
	}

	if (coap_message_fill(msg, payload, len) != OT_ERROR_NONE) {
		otMessageFree(msg);
		return -EBADMSG;
	}

	err = otCoapSendRequest(ot, msg, msg_info, NULL, NULL);
	if (err != OT_ERROR_NONE) {
		LOG_ERR("Failed to send the CoAP request: %s", otThreadErrorToString(err));
		otMessageFree(msg);
		return -EIO;
	}

	return 0;
}

static int telemetry_send(const uint8_t *payload, size_t len)
{
	otInstance *ot = openthread_get_default_instance();
	char addr[TELEMETRY_COLLECTOR_SIZE];
	otMessageInfo msg_info;
	int ret;

	if (ot == NULL) {
		LOG_ERR("No OpenThread instance");
		return -ENODEV;
	}

	memset(&msg_info, 0, sizeof(msg_info));
	msg_info.mPeerPort = OT_DEFAULT_COAP_PORT;

	/* The address is parsed when it is set, not once per publication. */
	k_mutex_lock(&collector_lock, K_FOREVER);
	msg_info.mPeerAddr = collector_ip;
	memcpy(addr, collector_addr, sizeof(addr));
	k_mutex_unlock(&collector_lock);

	LOG_DBG("POSTing %zu bytes to [%s]:%u", len, addr, msg_info.mPeerPort);

	/* The lock is taken and released in one place, around a single call. */
	openthread_mutex_lock();
	ret = coap_post(ot, &msg_info, payload, len);
	openthread_mutex_unlock();

	return ret;
}

int telemetry_publish_now(void)
{
	uint8_t payload[TELEMETRY_PAYLOAD_SIZE];
	struct sensor_value temp;
	struct sensor_value hum;
	int len;
	int ret;

	ret = env_sensor_read(&temp, &hum);
	if (ret) {
		return ret;
	}

	len = telemetry_format(payload, sizeof(payload), &temp, &hum);
	if (len < 0) {
		return len;
	}

	ret = telemetry_send(payload, len);
	if (ret) {
		return ret;
	}

	LOG_INF("Published %s", payload);

	return 0;
}

static void telemetry_work_handler(struct k_work *work)
{
	ARG_UNUSED(work);

	if (mesh_is_attached()) {
		LOG_DBG("Publish interval elapsed as %s, publishing", mesh_role_name());
		(void)telemetry_publish_now();
	} else {
		LOG_DBG("Not attached to the Thread mesh yet (%s), skipping this interval",
			mesh_role_name());
	}

	k_work_reschedule(&telemetry_work, K_SECONDS(CONFIG_TELEMETRY_INTERVAL_S));
}

size_t telemetry_collector_get(char *buf, size_t size)
{
	size_t len;

	if (buf == NULL || size == 0) {
		return 0;
	}

	k_mutex_lock(&collector_lock, K_FOREVER);
	len = MIN(strlen(collector_addr), size - 1);
	memcpy(buf, collector_addr, len);
	k_mutex_unlock(&collector_lock);

	buf[len] = '\0';

	return len;
}

static int collector_set(const char *addr)
{
	otIp6Address parsed;
	size_t len;

	if (addr == NULL) {
		LOG_ERR("No collector address given");
		return -EINVAL;
	}

	len = strlen(addr);
	if (len >= sizeof(collector_addr)) {
		LOG_ERR("Collector address '%s' is longer than %zu characters", addr,
			sizeof(collector_addr) - 1);
		return -EINVAL;
	}

	if (otIp6AddressFromString(addr, &parsed) != OT_ERROR_NONE) {
		LOG_ERR("Collector address '%s' is not an IPv6 address", addr);
		return -EINVAL;
	}

	k_mutex_lock(&collector_lock, K_FOREVER);
	collector_ip = parsed;
	memcpy(collector_addr, addr, len + 1);
	k_mutex_unlock(&collector_lock);

	return 0;
}

int telemetry_collector_set(const char *addr)
{
	int ret;

	ret = collector_set(addr);
	if (ret) {
		return ret;
	}

	LOG_INF("Collector address set to %s", addr);

	return 0;
}

static int telemetry_init(void)
{
	otInstance *ot = openthread_get_default_instance();
	otError err;
	int ret;

	/*
	 * A malformed Kconfig default should fail here, not on every publish.
	 * collector_set() has already logged what is wrong with it.
	 */
	ret = collector_set(CONFIG_TELEMETRY_COLLECTOR_ADDRESS);
	if (ret) {
		LOG_ERR("Not publishing: CONFIG_TELEMETRY_COLLECTOR_ADDRESS is unusable");
		return ret;
	}

	if (ot == NULL) {
		LOG_ERR("No OpenThread instance");
		return -ENODEV;
	}

	openthread_mutex_lock();
	err = otCoapStart(ot, OT_DEFAULT_COAP_PORT);
	openthread_mutex_unlock();

	if (err != OT_ERROR_NONE) {
		LOG_ERR("Failed to start CoAP: %s", otThreadErrorToString(err));
		return -EIO;
	}

	LOG_INF("Publishing to coap://[%s]/%s every %d s", collector_addr,
		CONFIG_TELEMETRY_URI_PATH, CONFIG_TELEMETRY_INTERVAL_S);
	LOG_DBG("CoAP started on UDP port %u, payload buffer is %d bytes", OT_DEFAULT_COAP_PORT,
		TELEMETRY_PAYLOAD_SIZE);

	k_work_reschedule(&telemetry_work, K_SECONDS(CONFIG_TELEMETRY_INTERVAL_S));

	return 0;
}

SYS_INIT(telemetry_init, APPLICATION, CONFIG_TELEMETRY_INIT_PRIORITY);
