#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_DBG);

int main(void)
{
	printk("System booted. Main thread going to sleep.\n");
	return 0;
}
