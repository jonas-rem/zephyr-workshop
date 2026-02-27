#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_DBG);

int main(void)
{
	LOG_INF("System booted. Main thread going to sleep.");

	return 0;
}
