#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/bos.h>
#include <stdint.h>

LOG_MODULE_REGISTER(tracing_usb_module, CONFIG_TRACING_USB_MODULE_LOG_LEVEL);

/* Default USB device configuration values */
#ifndef CONFIG_TRACING_USB_VID
#define CONFIG_TRACING_USB_VID 0x2fe3
#endif

#ifndef CONFIG_TRACING_USB_PID
#define CONFIG_TRACING_USB_PID 0x0001
#endif

#ifndef CONFIG_TRACING_USB_MANUFACTURER
#define CONFIG_TRACING_USB_MANUFACTURER "Zephyr Project"
#endif

#ifndef CONFIG_TRACING_USB_PRODUCT
#define CONFIG_TRACING_USB_PRODUCT "Zephyr Tracing Device"
#endif

#ifndef CONFIG_TRACING_USB_MAX_POWER
#define CONFIG_TRACING_USB_MAX_POWER 125
#endif

#ifndef CONFIG_TRACING_USB_SELF_POWERED
#define CONFIG_TRACING_USB_SELF_POWERED 1
#endif

#ifndef CONFIG_TRACING_USB_REMOTE_WAKEUP
#define CONFIG_TRACING_USB_REMOTE_WAKEUP 0
#endif

/* By default, do not register the USB DFU class DFU mode instance. */
static const char *const blocklist[] = {
	"dfu_dfu",
	NULL,
};

/* Instantiate USB device context */
USBD_DEVICE_DEFINE(tracing_usbd,
		   DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0)),
		   CONFIG_TRACING_USB_VID, CONFIG_TRACING_USB_PID);

USBD_DESC_LANG_DEFINE(tracing_lang);
USBD_DESC_MANUFACTURER_DEFINE(tracing_mfr, CONFIG_TRACING_USB_MANUFACTURER);
USBD_DESC_PRODUCT_DEFINE(tracing_product, CONFIG_TRACING_USB_PRODUCT);
IF_ENABLED(CONFIG_HWINFO, (USBD_DESC_SERIAL_NUMBER_DEFINE(tracing_sn)));

USBD_DESC_CONFIG_DEFINE(fs_cfg_desc, "FS Configuration");
USBD_DESC_CONFIG_DEFINE(hs_cfg_desc, "HS Configuration");

static const uint8_t attributes = (IS_ENABLED(CONFIG_TRACING_USB_SELF_POWERED) ?
				   USB_SCD_SELF_POWERED : 0) |
				  (IS_ENABLED(CONFIG_TRACING_USB_REMOTE_WAKEUP) ?
				   USB_SCD_REMOTE_WAKEUP : 0);

/* Full speed configuration */
USBD_CONFIGURATION_DEFINE(tracing_fs_config,
			  attributes,
			  CONFIG_TRACING_USB_MAX_POWER, &fs_cfg_desc);

/* High speed configuration */
USBD_CONFIGURATION_DEFINE(tracing_hs_config,
			  attributes,
			  CONFIG_TRACING_USB_MAX_POWER, &hs_cfg_desc);

#if CONFIG_TRACING_USB_20_EXTENSION_DESC
static const struct usb_bos_capability_lpm bos_cap_lpm = {
	.bLength = sizeof(struct usb_bos_capability_lpm),
	.bDescriptorType = USB_DESC_DEVICE_CAPABILITY,
	.bDevCapabilityType = USB_BOS_CAPABILITY_EXTENSION,
	.bmAttributes = 0UL,
};

USBD_DESC_BOS_DEFINE(tracing_usbext, sizeof(bos_cap_lpm), &bos_cap_lpm);
#endif

static void tracing_fix_code_triple(struct usbd_context *uds_ctx,
				    const enum usbd_speed speed)
{
	/* Always use class code information from Interface Descriptors */
	if (IS_ENABLED(CONFIG_USBD_CDC_ACM_CLASS) ||
	    IS_ENABLED(CONFIG_USBD_CDC_ECM_CLASS) ||
	    IS_ENABLED(CONFIG_USBD_CDC_NCM_CLASS) ||
	    IS_ENABLED(CONFIG_USBD_MIDI2_CLASS) ||
	    IS_ENABLED(CONFIG_USBD_AUDIO2_CLASS) ||
	    IS_ENABLED(CONFIG_USBD_VIDEO_CLASS)) {
		usbd_device_set_code_triple(uds_ctx, speed,
					    USB_BCC_MISCELLANEOUS, 0x02, 0x01);
	} else {
		usbd_device_set_code_triple(uds_ctx, speed, 0, 0, 0);
	}
}

static int tracing_usb_init(void)
{
	int err;

	LOG_INF("Initializing USB device for tracing");

	/* Add string descriptors */
	err = usbd_add_descriptor(&tracing_usbd, &tracing_lang);
	if (err) {
		LOG_ERR("Failed to initialize language descriptor (%d)", err);
		return err;
	}

	err = usbd_add_descriptor(&tracing_usbd, &tracing_mfr);
	if (err) {
		LOG_ERR("Failed to initialize manufacturer descriptor (%d)", err);
		return err;
	}

	err = usbd_add_descriptor(&tracing_usbd, &tracing_product);
	if (err) {
		LOG_ERR("Failed to initialize product descriptor (%d)", err);
		return err;
	}

	IF_ENABLED(CONFIG_HWINFO, (
		err = usbd_add_descriptor(&tracing_usbd, &tracing_sn);
	))
	if (err) {
		LOG_ERR("Failed to initialize SN descriptor (%d)", err);
		return err;
	}

	if (USBD_SUPPORTS_HIGH_SPEED &&
	    usbd_caps_speed(&tracing_usbd) == USBD_SPEED_HS) {
		err = usbd_add_configuration(&tracing_usbd, USBD_SPEED_HS,
					     &tracing_hs_config);
		if (err) {
			LOG_ERR("Failed to add High-Speed configuration");
			return err;
		}

		err = usbd_register_all_classes(&tracing_usbd, USBD_SPEED_HS, 1,
						blocklist);
		if (err) {
			LOG_ERR("Failed to add register classes");
			return err;
		}

		tracing_fix_code_triple(&tracing_usbd, USBD_SPEED_HS);
	}

	err = usbd_add_configuration(&tracing_usbd, USBD_SPEED_FS,
				     &tracing_fs_config);
	if (err) {
		LOG_ERR("Failed to add Full-Speed configuration");
		return err;
	}

	err = usbd_register_all_classes(&tracing_usbd, USBD_SPEED_FS, 1, blocklist);
	if (err) {
		LOG_ERR("Failed to add register classes");
		return err;
	}

	tracing_fix_code_triple(&tracing_usbd, USBD_SPEED_FS);
	usbd_self_powered(&tracing_usbd, attributes & USB_SCD_SELF_POWERED);

#if CONFIG_TRACING_USB_20_EXTENSION_DESC
	(void)usbd_device_set_bcd_usb(&tracing_usbd, USBD_SPEED_FS, 0x0201);
	(void)usbd_device_set_bcd_usb(&tracing_usbd, USBD_SPEED_HS, 0x0201);

	err = usbd_add_descriptor(&tracing_usbd, &tracing_usbext);
	if (err) {
		LOG_ERR("Failed to add USB 2.0 Extension Descriptor");
		return err;
	}
#endif

	err = usbd_init(&tracing_usbd);
	if (err) {
		LOG_ERR("Failed to initialize device support");
		return err;
	}

	err = usbd_enable(&tracing_usbd);
	if (err) {
		LOG_ERR("Failed to enable USB device");
		return err;
	}

	LOG_INF("USB device initialized and enabled successfully");
	return 0;
}

SYS_INIT(tracing_usb_init, APPLICATION, CONFIG_TRACING_USB_MODULE_INIT_PRIORITY);
