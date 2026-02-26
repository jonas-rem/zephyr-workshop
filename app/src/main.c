#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#if defined(CONFIG_USB_DEVICE_STACK_NEXT)
#include "sample_usbd.h"
#include <zephyr/device.h>
#endif

LOG_MODULE_REGISTER(app, LOG_LEVEL_DBG);

int main(void)
{
#if defined(CONFIG_USB_DEVICE_STACK_NEXT)
	struct usbd_context *sample_usbd = sample_usbd_init_device(NULL);

	if (sample_usbd == NULL) {
		printk("Failed to initialize USB device\n");
		return 0;
	}

	if (usbd_enable(sample_usbd)) {
		printk("USB backend enable failed\n");
		return 0;
	}
#endif /* CONFIG_USB_DEVICE_STACK_NEXT */

	printk("System booted. Main thread going to sleep.\n");
	return 0;
}
