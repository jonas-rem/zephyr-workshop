/*
 * Copyright (c) 2026 Jonas Remmert
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>
#include <zephyr/zbus/zbus.h>

#include "message_channel.h"

/* Helper to get current state from zbus channel */
static enum sys_states get_current_state(void)
{
	enum sys_states state;
	zbus_chan_read(&sys_ctl_ch, &state, K_NO_WAIT);
	return state;
}

ZTEST(sys_ctrl_test_suite, test_initial_state)
{
	enum sys_states state = get_current_state();

	/* State should be either ACTIVE or SLEEP (valid states) */
	zassert_true(state == SYS_ACTIVE || state == SYS_SLEEP,
		      "Initial state should be valid (ACTIVE or SLEEP), got %d", state);
}

ZTEST(sys_ctrl_test_suite, test_button_press_active_to_sleep)
{
	struct event_msg btn_msg = { .event = SYS_BUTTON_PRESSED };
	enum sys_states initial_state = get_current_state();

	/* First press: toggles state */
	zbus_chan_pub(&event_ch, &btn_msg, K_NO_WAIT);
	k_yield();

	/* State should have toggled */
	if (initial_state == SYS_ACTIVE) {
		zassert_equal(get_current_state(), SYS_SLEEP,
			      "State should toggle to SLEEP");
	} else {
		zassert_equal(get_current_state(), SYS_ACTIVE,
			      "State should toggle to ACTIVE");
	}
}

ZTEST(sys_ctrl_test_suite, test_button_press_sleep_to_active)
{
	struct event_msg btn_msg = { .event = SYS_BUTTON_PRESSED };
	enum sys_states state_before = get_current_state();

	/* Press: toggles state */
	zbus_chan_pub(&event_ch, &btn_msg, K_NO_WAIT);
	k_yield();

	enum sys_states state_after = get_current_state();

	/* State should have toggled */
	zassert_not_equal(state_before, state_after,
			  "State should change after button press");
}

ZTEST(sys_ctrl_test_suite, test_rapid_button_presses)
{
	struct event_msg btn_msg = { .event = SYS_BUTTON_PRESSED };
	enum sys_states initial_state = get_current_state();

	/* Multiple rapid button presses */
	for (int i = 0; i < 10; i++) {
		zbus_chan_pub(&event_ch, &btn_msg, K_NO_WAIT);
		k_yield();
	}

	/* After 10 toggles, should be back to initial state */
	zassert_equal(get_current_state(), initial_state,
		      "State should return to initial after even toggles");
}

ZTEST(sys_ctrl_test_suite, test_state_toggle_consistency)
{
	struct event_msg btn_msg = { .event = SYS_BUTTON_PRESSED };
	enum sys_states prev_state = get_current_state();

	/* Toggle multiple times and verify consistency */
	for (int i = 0; i < 6; i++) {
		zbus_chan_pub(&event_ch, &btn_msg, K_NO_WAIT);
		k_yield();

		enum sys_states current_state = get_current_state();

		/* State should alternate */
		zassert_not_equal(current_state, prev_state,
				  "State should alternate on each press");

		prev_state = current_state;
	}
}

static void *sys_ctrl_test_setup(void)
{
	/* Wait for SYS_INIT to complete */
	k_sleep(K_MSEC(50));
	return NULL;
}

ZTEST_SUITE(sys_ctrl_test_suite, NULL, sys_ctrl_test_setup, NULL, NULL, NULL);
