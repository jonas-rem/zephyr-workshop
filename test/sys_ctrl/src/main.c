#include <zephyr/ztest.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/logging/log.h>
#include "message_channel.h"
#include "modules/sys_ctrl/sys_ctrl.h"

LOG_MODULE_REGISTER(test);

ZTEST_SUITE(sys_ctrl_suite, NULL, NULL, NULL, NULL, NULL);

ZTEST(sys_ctrl_suite, test_initial_state)
{
	zassert_equal(sys_ctrl_get_state(), SYS_STANDBY, "Initial state should be SYS_STANDBY");
}

ZTEST(sys_ctrl_suite, test_state_transitions)
{
	/* Initial should be STANDBY */
	zassert_equal(sys_ctrl_get_state(), SYS_STANDBY, "Expected STANDBY");

	/* Press button -> Should go to SLEEP */
	sys_ctrl_handle_button_press();
	zassert_equal(sys_ctrl_get_state(), SYS_SLEEP, "Expected transition to SLEEP");

	/* Press button again -> Should go back to STANDBY */
	sys_ctrl_handle_button_press();
	zassert_equal(sys_ctrl_get_state(), SYS_STANDBY, "Expected transition back to STANDBY");
}
