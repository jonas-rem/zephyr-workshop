#include <zephyr/kernel.h>

#define MODULE_1_INIT_PRIORITY 0

int module_1_init(void)
{
	printk("Module 1 initialized (priority: APPLICATION, level: %d)\n",
		MODULE_1_INIT_PRIORITY);

	return 0;
}

/* Register module_1_init to run at APPLICATION level with priority 0 */
SYS_INIT(module_1_init, APPLICATION, MODULE_1_INIT_PRIORITY);
