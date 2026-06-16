#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

/*
 * This module demonstrates SYS_INIT with APPLICATION level priority.
 * Higher priority value (10) means it runs after lower priority values.
 */

#define MODULE_2_INIT_PRIORITY 999

int module_2_init(void)
{
	printk("Module 2 initialized (level: APPLICATION, priority: %d)\n",
	       MODULE_2_INIT_PRIORITY);

	return 0;
}

/* Register module_2_init to run at APPLICATION level with priority 999 */
/* Higher priority number = runs later within the same level */
SYS_INIT(module_2_init, APPLICATION, MODULE_2_INIT_PRIORITY);
